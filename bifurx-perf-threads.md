Yes, Dragon King Leviathan — and for Bifurx specifically it’s a plausible next optimization layer.

The right framing is:

> **Do not move UI drawing to a background thread.**
> **Move UI data preparation / geometry / spectrum computation to a background worker, then render the latest completed snapshot on the main UI thread.**

Rack’s GUI work is expected to happen on the main thread; Andrew Belt explicitly confirmed “all GUI things happen on the main thread,” including Rack-as-DAW-plugin cases. Rack also has its own engine/audio threading model, so a Bifurx worker would be an additional CPU helper, not a replacement for Rack’s engine threads. ([VCV Community][1])

## What can safely move off-thread

For Bifurx, good worker-thread candidates are:

| Candidate                             | Move to worker? | Notes                                                                         |
| ------------------------------------- | --------------: | ----------------------------------------------------------------------------- |
| Module response curve calculation     |         **Yes** | Best candidate. Compute points/vertices from a copied preview state.          |
| FFT/spectrum display preparation      |         **Yes** | Also good. Worker can consume latest analysis buffer and emit display points. |
| Refined marker curve point generation |         **Yes** | This is exactly the kind of “prepare geometry” work that can arrive late.     |
| OpenGL VBO uploads / `glDraw*`        |          **No** | Keep GL calls in `drawFramebuffer()` / UI thread.                             |
| NanoVG drawing                        |          **No** | Same: draw on UI thread only.                                                 |
| Direct reads/writes of Rack widgets   |          **No** | Snapshot the needed values first.                                             |
| Audio DSP/filter processing           |          **No** | Not part of this optimization; keep real-time path isolated.                  |

The repo already splits Bifurx across `Bifurx.cpp`, `Bifurx.hpp`, `BifurxGL.cpp`, and `BifurxUI.cpp`, so it is structurally ready for this kind of separation. The current source also already has preview/model/impulse-style calculation code and a GL `drawFramebuffer()` path, which suggests a natural boundary: worker prepares arrays, GL renderer consumes arrays. ([GitHub][2])

## The safe architecture

Use a **snapshot pipeline**:

```cpp
Audio/Module thread
    ↓ publishes small preview state, analysis frame counters, maybe ring-buffer samples

UI thread / ModuleWidget
    ↓ captures latest visible state: size, mode, flags, preview state, display scale

Background worker
    ↓ computes curve vertices, spectrum vertices, marker positions, labels

UI draw thread
    ↓ renders latest completed snapshot, even if it is 1–3 frames old
```

The key is that each stage owns its data. Do **not** share mutable `std::vector`s between threads. The VCV community guidance around worker threads calls out that sharing standard containers between threads is tricky, and recommends atomic flags / ready states or cloned/proxied read-only data instead. ([VCV Community][3])

## Best implementation pattern

Use **triple buffering** or **atomic shared snapshot swap**.

Something like:

```cpp
struct BifurxUiRenderRequest {
    uint64_t seq = 0;

    float width = 0.f;
    float height = 0.f;
    float sampleRate = 48000.f;

    BifurxPreviewState previewState;

    bool showModuleResponseOverlay = true;
    bool fftScaleDynamic = true;
    bool useGlShaderRenderer = true;

    float displayTopDbfs = 0.f;
};

struct BifurxUiRenderSnapshot {
    uint64_t seq = 0;

    std::vector<Vec> responseCurve;
    std::vector<Vec> responseFill;
    std::vector<Vec> softCapOverlay;
    std::vector<Vec> spectrumCurve;

    float markerXA = 0.f;
    float markerXB = 0.f;

    char labelA[16] = {};
    char labelB[16] = {};
};
```

Then:

```cpp
std::atomic<uint64_t> requestedSeq{0};
std::atomic<uint64_t> completedSeq{0};

std::mutex requestMutex;
BifurxUiRenderRequest latestRequest;

std::mutex snapshotMutex;
std::shared_ptr<const BifurxUiRenderSnapshot> latestSnapshot;
```

Worker loop:

```cpp
void BifurxUiWorker::run() {
    while (!stopRequested.load()) {
        BifurxUiRenderRequest req;

        {
            std::lock_guard<std::mutex> lock(requestMutex);
            req = latestRequest;
        }

        if (req.seq == lastProcessedSeq) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        auto snap = std::make_shared<BifurxUiRenderSnapshot>();
        snap->seq = req.seq;

        // Heavy CPU-only work here:
        // - build preview model
        // - calculate response curve
        // - calculate refined curve points
        // - prepare spectrum display vertices
        // - prepare label strings

        {
            std::lock_guard<std::mutex> lock(snapshotMutex);
            latestSnapshot = snap;
        }

        completedSeq.store(req.seq, std::memory_order_release);
        lastProcessedSeq = req.seq;
    }
}
```

UI thread:

```cpp
void BifurxDisplay::step() {
    BifurxUiRenderRequest req;
    req.seq = ++localSeq;
    req.width = box.size.x;
    req.height = box.size.y;
    req.previewState = module->lastPreviewState; // copied, not referenced
    req.showModuleResponseOverlay = module->showModuleResponseOverlay;
    req.fftScaleDynamic = module->fftScaleDynamic;
    req.displayTopDbfs = module->displayTopDbfs;

    worker.submitLatest(req);

    auto snap = worker.getLatestSnapshot();
    if (snap && snap->seq != renderedSeq) {
        renderSnapshot = snap;
        renderedSeq = snap->seq;
        setDirty();
    }
}
```

Then `draw()` / `drawFramebuffer()` only reads `renderSnapshot`.

## Important rule: coalesce, don’t queue

For this module, you do **not** want a FIFO queue of 60 outdated render jobs. You want:

> “Here is the newest request. Throw away older unprocessed requests.”

That gives you graceful latency. If the worker falls behind, the display updates less often, but it does not accumulate sludge.

This is the desired behavior:

```text
UI submits seq 101
UI submits seq 102
UI submits seq 103
Worker wakes up
Worker processes only 103
```

The visual result may be a few frames stale, but it remains responsive and CPU-bounded.

## Where I would put the worker

I would put the worker behind the **Bifurx widget/UI layer**, not the audio `Module` core.

Reason: the worker is only useful when the UI exists and is visible. If Bifurx is loaded in a patch but not being viewed, burning a thread to prepare pretty curves is wasteful.

Recommended shape:

```cpp
class BifurxUiWorker {
public:
    void start();
    void stop();

    void submitLatest(const BifurxUiRenderRequest& req);
    std::shared_ptr<const BifurxUiRenderSnapshot> getLatestSnapshot();

private:
    std::thread thread;
    std::atomic<bool> stopRequested{false};
};
```

Create it in the display/widget constructor, stop/join it in the destructor.

## But avoid one thread per Bifurx instance if possible

One worker per visible Bifurx is simple, but imagine a patch with 12 Bifurx modules open. That becomes a tiny dragon swarm chewing scheduler time.

Better long-term version:

```text
BifurxUiRenderService
    one shared worker thread
    receives latest request per visible Bifurx display
    processes jobs round-robin or newest-first
```

For the first pass, one worker per visible widget is acceptable. For plugin polish, a shared service is cleaner.

## What latency is acceptable?

For response curves and spectrum overlays, I’d target:

```text
Worker update rate: 15–30 Hz
Allowed visual latency: 1–4 UI frames
Drop behavior: always drop stale requests
```

That is usually musically fine. A filter response curve does not need to track at audio-rate. It just needs to feel alive.

## Biggest danger

The biggest trap is accidentally doing this:

```cpp
worker thread:
    module->params[FREQ_PARAM].getValue();
    widget->box.size;
    glBindBuffer(...);
    nvgBeginPath(...);
```

That is the forbidden spellbook.

Instead:

```cpp
UI thread:
    copy all required values into request struct

worker:
    compute pure data from copied request

UI thread:
    draw pure data
```

## My recommendation for Bifurx

Yes: implement this, but only for **preview/spectrum/curve geometry**, not the whole UI.

I’d do it in this order:

1. Create `BifurxUiRenderRequest` and `BifurxUiRenderSnapshot`.
2. Move response curve point generation into a pure function.
3. Move spectrum display point generation into a pure function.
4. Have GL/NanoVG renderers consume the same snapshot.
5. Add a context-menu option:

```text
Visual worker thread: Off / On
Visual update rate: 15 Hz / 30 Hz / 60 Hz
```

Default could be **On at 30 Hz** once stable.

The sonic core stays sovereign. The UI becomes a slightly delayed oracle: always reflecting the newest completed truth, never blocking the ritual.

[1]: https://community.vcvrack.com/t/ui-threading-question/6477 "UI Threading question - Development - VCV Community"
[2]: https://github.com/PlasmaChroma/Leviathan-Rack2/tree/expander/src "Leviathan-Rack2/src at expander · PlasmaChroma/Leviathan-Rack2 · GitHub"
[3]: https://community.vcvrack.com/t/worker-threads/19282 "Worker threads - Development - VCV Community"
