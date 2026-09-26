#pragma once

// Included by Chimera.cpp after Chimera is complete. The waveform layer is
// cached by Rack's FramebufferWidget; only the small overlay redraws per frame.
struct ChimeraWaveformLayer : Widget {
    std::shared_ptr<const chimera::WaveformSummary> summary;

    void draw(const DrawArgs& args) override {
        NVGcontext* vg = args.vg;
        const float w = box.size.x, h = box.size.y;
        nvgBeginPath(vg);
        nvgRoundedRect(vg, 0.f, 0.f, w, h, 4.f);
        nvgFillColor(vg, nvgRGB(8, 21, 30));
        nvgFill(vg);
        const float traceTop = 4.f, traceBottom = h * 0.82f;
        const bool stereo = summary && summary->stereo;
        const float laneHeight = stereo ? (traceBottom - traceTop - 4.f) * 0.5f :
            traceBottom - traceTop;
        const float left = 5.f, width = w - 10.f;
        const auto drawLane = [&](float top, const std::array<float, chimera::WaveformSummary::kBins>* low,
                                  const std::array<float, chimera::WaveformSummary::kBins>* high,
                                  NVGcolor color) {
            const float mid = top + laneHeight * 0.5f;
            nvgBeginPath(vg);
            nvgMoveTo(vg, left, mid);
            nvgLineTo(vg, w - left, mid);
            nvgStrokeColor(vg, nvgRGBA(83, 125, 135, 95));
            nvgStrokeWidth(vg, 1.f);
            nvgStroke(vg);
            if (!summary || !summary->frames) return;
            const float scale = laneHeight * 0.46f / std::max(summary->peak, 0.015f);
            nvgBeginPath(vg);
            for (std::size_t i = 0; i < chimera::WaveformSummary::kBins; ++i) {
                const float x = left + width * (float(i) + 0.5f) /
                    float(chimera::WaveformSummary::kBins);
                nvgMoveTo(vg, x, mid - (*high)[i] * scale);
                nvgLineTo(vg, x, mid - (*low)[i] * scale);
            }
            nvgStrokeColor(vg, color);
            nvgStrokeWidth(vg, std::max(1.f, width / float(chimera::WaveformSummary::kBins) * 0.78f));
            nvgStroke(vg);
        };
        drawLane(traceTop, summary ? &summary->leftLow : nullptr,
            summary ? &summary->leftHigh : nullptr, nvgRGB(106, 221, 215));
        if (stereo) {
            const float divider = traceTop + laneHeight + 2.f;
            nvgBeginPath(vg);
            nvgMoveTo(vg, left, divider);
            nvgLineTo(vg, w - left, divider);
            nvgStrokeColor(vg, nvgRGBA(83, 125, 135, 55));
            nvgStrokeWidth(vg, 1.f);
            nvgStroke(vg);
            drawLane(divider + 2.f, &summary->rightLow, &summary->rightHigh,
                nvgRGB(177, 157, 239));
        }
    }
};

struct ChimeraDisplayOverlay : Widget {
    Chimera* owner = nullptr;
    chimera::MarkerDisplay markers;
    std::shared_ptr<const chimera::WaveformSummary> summary;
    std::string stateText = "REEL READY";
    std::string detailText = "NO REEL";
    std::string saveText;
    int cachedState = -1;
    int cachedErrorKind = -1;
    bool cachedBusy = false, cachedDirty = false;
    std::uint32_t cachedFrames = UINT32_MAX;
    unsigned cachedCount = UINT32_MAX, cachedCurrent = UINT32_MAX;
    unsigned cachedRequested = UINT32_MAX;

    void step() override {
        if (owner) owner->markerDisplay.consume(markers);
        else markers = chimera::MarkerDisplay{};
        if (!owner) {
            stateText = "CHIMERA";
            detailText = "REEL ENGINE";
            saveText.clear();
            return;
        }
        const int state = owner->publishedRecordState.load(std::memory_order_acquire);
        const bool saveError = owner->saveFailure.load(std::memory_order_acquire);
        const bool error = saveError || owner->ioError.load(std::memory_order_acquire) ||
            owner->bridgeError.load(std::memory_order_acquire) ||
            owner->recordNotReady.load(std::memory_order_acquire);
        const int errorKind = saveError ? 1 :
            owner->bridgeError.load(std::memory_order_acquire) ? 2 :
            owner->recordNotReady.load(std::memory_order_acquire) ? 3 : error ? 4 : 0;
        const bool busy = owner->ioBusy.load(std::memory_order_acquire) ||
            owner->saveInProgress.load(std::memory_order_acquire);
        const std::uint32_t frames = owner->publishedValidFrames.load(std::memory_order_acquire);
        const unsigned count = owner->publishedMarkerCount.load(std::memory_order_acquire);
        const unsigned current = owner->publishedRegion.load(std::memory_order_acquire);
        const unsigned requested = owner->publishedRequestedRegion.load(std::memory_order_acquire);
        const bool dirty = owner->unsavedImport.load(std::memory_order_acquire) ||
            owner->publishedAudioRevision.load(std::memory_order_acquire) !=
                owner->savedAudioRevision.load(std::memory_order_acquire) ||
            owner->publishedDocumentRevision.load(std::memory_order_acquire) !=
                owner->savedDocumentRevision.load(std::memory_order_acquire) ||
            !owner->hasSavedReel.load(std::memory_order_acquire);
        if (state == cachedState && errorKind == cachedErrorKind && busy == cachedBusy &&
            dirty == cachedDirty && frames / 48000 == cachedFrames / 48000 &&
            count == cachedCount && current == cachedCurrent &&
            requested == cachedRequested) return;
        cachedState = state;
        cachedErrorKind = errorKind;
        cachedBusy = busy;
        cachedDirty = dirty;
        cachedFrames = frames;
        cachedCount = count;
        cachedCurrent = current;
        cachedRequested = requested;
        static const char* states[] = {"IDLE", "ARM CURRENT", "ARM APPEND",
            "ARM STOP", "REC CURRENT", "REC APPEND"};
        stateText = states[std::max(0, std::min(state, 5))];
        if (saveError) stateText = "SAVE ERROR";
        else if (owner->bridgeError.load(std::memory_order_acquire)) stateText = "RATE ERROR";
        else if (owner->recordNotReady.load(std::memory_order_acquire)) stateText = "REEL NOT READY";
        else if (error) stateText = "REEL ERROR";
        else if (busy) stateText += " / IO";
        saveText = frames ? (dirty ? "UNSAVED" : "SAVED") : "";
        if (!frames) detailText = "NO AUDIO";
        else {
            detailText = std::to_string(frames / 48000) + "s  SPL " +
                std::to_string(current + 1) + "/" + std::to_string(count);
            if (requested != current)
                detailText += " >" + std::to_string(requested + 1);
        }
        Widget::step();
    }

    void draw(const DrawArgs& args) override {
        if (!APP || !APP->window || !APP->window->uiFont) return;
        NVGcontext* vg = args.vg;
        const float w = box.size.x, h = box.size.y;
        const float left = 5.f, width = w - 10.f;
        const float traceTop = 4.f, traceBottom = h * 0.82f;
        // Marker metadata comes directly from the core, independently of the
        // slower waveform scan. The summary only supplies the waveform backdrop.
        const std::uint32_t displayFrames = owner ? std::max(markers.frames,
            owner->publishedValidFrames.load(std::memory_order_acquire)) : 0;
        if (owner && displayFrames) {
            const auto xFor = [&](std::uint32_t frame) {
                return left + width * float(std::min(frame, displayFrames)) /
                    float(displayFrames);
            };
            nvgBeginPath(vg);
            for (unsigned i = 1; i < markers.count; ++i) {
                const float x = xFor(markers.markers[i]);
                nvgMoveTo(vg, x, traceTop);
                nvgLineTo(vg, x, traceBottom);
            }
            nvgStrokeColor(vg, nvgRGBA(235, 183, 104, 200));
            nvgStrokeWidth(vg, 1.f);
            nvgStroke(vg);
            const int record = owner->publishedRecordState.load(std::memory_order_acquire);
            if (record == 4 || record == 5 ||
                owner->publishedAudioRevision.load(std::memory_order_acquire) >
                    (summary ? summary->audioRevision : 0)) {
                const float x1 = xFor(owner->publishedRecordStartFrame.load(std::memory_order_acquire));
                const float x2 = xFor(owner->publishedRecordFrame.load(std::memory_order_acquire));
                nvgBeginPath(vg);
                nvgRect(vg, std::min(x1, x2), traceTop, std::max(1.f, std::fabs(x2 - x1)),
                        traceBottom - traceTop);
                nvgFillColor(vg, nvgRGBA(243, 113, 89, 52));
                nvgFill(vg);
            }
            const float playX = xFor(owner->publishedPlayFrame.load(std::memory_order_acquire));
            nvgBeginPath(vg);
            nvgMoveTo(vg, playX, traceTop);
            nvgLineTo(vg, playX, traceBottom);
            nvgStrokeColor(vg, nvgRGB(246, 234, 168));
            nvgStrokeWidth(vg, 1.5f);
            nvgStroke(vg);
        }
        nvgFontFaceId(vg, APP->window->uiFont->handle);
        nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
        nvgFontSize(vg, 9.f);
        nvgFillColor(vg, nvgRGB(246, 231, 193));
        nvgText(vg, 6.f, h * 0.92f, stateText.c_str(), nullptr);
        nvgTextAlign(vg, NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);
        nvgText(vg, w - 6.f, h * 0.92f, saveText.c_str(), nullptr);
        nvgFontSize(vg, 8.5f);
        nvgFillColor(vg, nvgRGB(166, 209, 212));
        nvgTextAlign(vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
        nvgText(vg, w * 0.5f, h * 0.92f, detailText.c_str(), nullptr);
    }
};
