#pragma once

// Included by Chimera.cpp after Chimera is complete. The waveform layer is
// cached by Rack's FramebufferWidget; only the small overlay redraws per frame.
struct ChimeraWaveformLayer : Widget {
    chimera::RenderFrameMetrics* metrics = nullptr;
    std::shared_ptr<const chimera::WaveformSummary> summary;

    void draw(const DrawArgs& args) override {
        chimera::RenderComponentTimer timer(metrics, true);
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
                nvgRGB(142, 189, 227));
        }
    }
};

struct ChimeraVuMeterWidget final : TransparentWidget {
    chimera::RenderFrameMetrics* metrics = nullptr;
    Chimera* owner = nullptr;
    unsigned channel = 0;
    float level = 0.f;
    std::shared_ptr<window::Image> faceImage;
    std::shared_ptr<window::Image> overlayImage;
    std::string facePath;
    std::string overlayPath;

    static float needleAngleForLevel(float normalizedRms) {
        constexpr float rest = float(-138.0 * M_PI / 180.0);
        constexpr float zeroVu = float(-58.0 * M_PI / 180.0);
        constexpr float plusThree = float(-40.0 * M_PI / 180.0);
        constexpr float plusThreeLevel = 1.41253754f;
        const float bounded = clamp(normalizedRms, 0.f, plusThreeLevel);
        if (bounded <= 1.f) return rest + (zeroVu - rest) * bounded;
        return zeroVu + (plusThree - zeroVu) *
            ((bounded - 1.f) / (plusThreeLevel - 1.f));
    }

    void onContextDestroy(const ContextDestroyEvent& e) override {
        visual_assets::onRasterContextDestroy(e.vg);
        faceImage.reset();
        overlayImage.reset();
        facePath.clear();
        overlayPath.clear();
        TransparentWidget::onContextDestroy(e);
    }

    void onContextCreate(const ContextCreateEvent& e) override {
        visual_assets::onRasterContextCreate(e.vg);
        faceImage.reset();
        overlayImage.reset();
        facePath.clear();
        overlayPath.clear();
        TransparentWidget::onContextCreate(e);
    }

    void step() override {
        const float power = owner && channel < 2 ?
            owner->publishedVuPower[channel].load(std::memory_order_acquire) : 0.f;
        level = std::sqrt(clamp(power, 0.f, 4.f));
        if (level < 1e-5f) level = 0.f;
        TransparentWidget::step();
    }

    int imageHandle(NVGcontext* vg, std::shared_ptr<window::Image>& image,
                    std::string& path, const char* relativePath) {
        if (!vg || !APP || !APP->window) return -1;
        if (path.empty()) path = asset::plugin(pluginInstance, relativePath);
        if (!image) image = APP->window->loadImage(path);
        return visual_assets::loadRasterMipmapHandle(vg, image, path);
    }

    void draw(const DrawArgs& args) override {
        chimera::RenderComponentTimer timer(metrics, false);
        const int image = imageHandle(args.vg, faceImage, facePath, "res/icon/LeviathanVU-256.png");
        if (image < 0 || box.size.x <= 0.f || box.size.y <= 0.f) return;
        const float diameter = std::min(box.size.x, box.size.y);
        const Vec center(box.size.x * 0.5f, box.size.y * 0.5f);
        const float radius = diameter * 0.5f;
        nvgBeginPath(args.vg);
        nvgCircle(args.vg, center.x, center.y, radius);
        nvgFillPaint(args.vg, nvgImagePattern(
            args.vg, center.x - radius, center.y - radius, diameter, diameter,
            0.f, image, 1.f));
        nvgFill(args.vg);

        // The authored face's movement pivots at approximately (128, 194).
        const Vec pivot(center.x, center.y - radius + diameter * (194.f / 256.f));
        const float angle = needleAngleForLevel(level);
        const float length = diameter * (116.f / 256.f);
        const Vec direction(std::cos(angle), std::sin(angle));
        const Vec tip = pivot.plus(direction.mult(length));
        nvgLineCap(args.vg, NVG_ROUND);
        nvgBeginPath(args.vg);
        nvgMoveTo(args.vg, pivot.x + 0.7f, pivot.y + 0.8f);
        nvgLineTo(args.vg, tip.x + 0.7f, tip.y + 0.8f);
        nvgStrokeColor(args.vg, nvgRGBA(10, 4, 4, 180));
        nvgStrokeWidth(args.vg, std::max(1.25f, diameter * 0.014f));
        nvgStroke(args.vg);
        nvgBeginPath(args.vg);
        nvgMoveTo(args.vg, pivot.x, pivot.y);
        nvgLineTo(args.vg, tip.x, tip.y);
        nvgStrokeColor(args.vg, nvgRGBA(224, 32, 28, 255));
        nvgStrokeWidth(args.vg, std::max(0.9f, diameter * 0.009f));
        nvgStroke(args.vg);

        // Authored bezel/escutcheon masks both the full needle and its shadow.
        const int overlay = imageHandle(args.vg, overlayImage, overlayPath, "res/icon/VU-Overlay-256.png");
        if (overlay >= 0) {
            nvgBeginPath(args.vg);
            nvgRect(args.vg, center.x - radius, center.y - radius, diameter, diameter);
            nvgFillPaint(args.vg, nvgImagePattern(args.vg, center.x - radius, center.y - radius,
                diameter, diameter, 0.f, overlay, 1.f));
            nvgFill(args.vg);
        }
    }
};

struct ChimeraReelsWidget final : TransparentWidget {
    chimera::RenderFrameMetrics* metrics = nullptr;
    static constexpr float kNormalRpm = 33.f;
    static constexpr float kTapeHubRatio = 0.36f;
    static constexpr float kTapeOuterRatio = 0.91f;
    Chimera* owner = nullptr;
    Vec centers[2];
    Vec intakePoints[2];
    float radii[2] = {0.f, 0.f};
    float angle = 0.f;
    double lastStepTime = 0.0;
    std::shared_ptr<window::Image> reelImage;
    int imageHandle = -1; // Borrowed from the shared per-context raster cache.
    std::string loadedPath;

    static float angularVelocity(float playbackRate) {
        return playbackRate * kNormalRpm * float(2.0 * M_PI / 60.0);
    }

    static float tapeProgress(std::uint32_t position, std::uint32_t total) {
        if (!total) return 0.f;
        return clamp(float(std::min(position, total)) / float(total), 0.f, 1.f);
    }

    static float tapeRadius(float reelRadius, float amount) {
        const float inner = reelRadius * kTapeHubRatio;
        const float outer = reelRadius * kTapeOuterRatio;
        const float bounded = clamp(amount, 0.f, 1.f);
        return std::sqrt(inner * inner + bounded * (outer * outer - inner * inner));
    }

    static Vec tapeExitPoint(Vec center, Vec intake, float radius) {
        const Vec towardIntake = intake.minus(center);
        const float distanceSquared = towardIntake.x * towardIntake.x +
            towardIntake.y * towardIntake.y;
        if (distanceSquared <= 0.f || radius <= 0.f) return center;
        const float radiusSquared = radius * radius;
        if (distanceSquared <= radiusSquared)
            return center.plus(towardIntake.mult(radius / std::sqrt(distanceSquared)));
        // Inner-facing tangents: clockwise rotation feeds the left span out
        // and winds the right span in. Reversing transport reverses both.
        const float side = towardIntake.x >= 0.f ? 1.f : -1.f;
        const Vec perpendicular(side * towardIntake.y, -side * towardIntake.x);
        const float tangentScale = radius * std::sqrt(distanceSquared - radiusSquared) /
            distanceSquared;
        return center.plus(towardIntake.mult(radiusSquared / distanceSquared))
            .plus(perpendicular.mult(tangentScale));
    }

    static Vec tapeIntakePoint(Vec reelCenter, Vec vuCenter) {
        const float spacing = vuCenter.x - reelCenter.x;
        // Move toward the VU without lowering the intake, softening the approach.
        return Vec(reelCenter.x + spacing * 0.6f,
            reelCenter.y + std::fabs(spacing) * 0.5f);
    }

    Rect drawBounds(unsigned i) const {
        const Vec pad = mm2px(Vec(1.6f, 0.55f)).plus(Vec(1.f, 1.f));
        const Vec lo(std::min(centers[i].x - radii[i], intakePoints[i].x - pad.x),
                     std::min(centers[i].y - radii[i], intakePoints[i].y - pad.y));
        const Vec hi(std::max(centers[i].x + radii[i], intakePoints[i].x + pad.x),
                     std::max(centers[i].y + radii[i], intakePoints[i].y + pad.y));
        return Rect(lo, hi.minus(lo));
    }

    void onContextDestroy(const ContextDestroyEvent& e) override {
        visual_assets::onRasterContextDestroy(e.vg);
        reelImage.reset();
        imageHandle = -1;
        TransparentWidget::onContextDestroy(e);
    }

    void onContextCreate(const ContextCreateEvent& e) override {
        visual_assets::onRasterContextCreate(e.vg);
        reelImage.reset();
        imageHandle = -1;
        TransparentWidget::onContextCreate(e);
    }

    bool ensureImage(NVGcontext* vg) {
        if (!vg || !APP || !APP->window) return false;
        if (loadedPath.empty())
            loadedPath = asset::plugin(pluginInstance, "res/icon/Reel4-256px.png");
        if (!reelImage) reelImage = APP->window->loadImage(loadedPath);
        imageHandle = visual_assets::loadRasterMipmapHandle(vg, reelImage, loadedPath);
        return imageHandle > 0;
    }

    void step() override {
        const double now = system::getTime();
        if (lastStepTime > 0.0 && now >= lastStepTime) {
            const double elapsed = std::min(now - lastStepTime, 0.25);
            const float rate = owner ?
                owner->publishedPlaybackRate.load(std::memory_order_acquire) : 0.f;
            angle = std::fmod(angle + angularVelocity(rate) * float(elapsed),
                              float(2.0 * M_PI));
        }
        lastStepTime = now;
        TransparentWidget::step();
    }

    void draw(const DrawArgs& args) override {
        chimera::RenderComponentTimer timer(metrics, false);
        const bool visible[2] = {args.clipBox.intersects(drawBounds(0)),
                                 args.clipBox.intersects(drawBounds(1))};
        if (!visible[0] && !visible[1]) return;
        if (!ensureImage(args.vg)) return;
        float progress = 0.f;
        bool haveTape = false;
        if (owner) {
            const bool recording = owner->recordingActive.load(std::memory_order_acquire);
            const std::uint32_t position = recording ?
                owner->publishedRecordFrame.load(std::memory_order_acquire) :
                owner->publishedPlayFrame.load(std::memory_order_acquire);
            const std::uint32_t total = recording ?
                owner->publishedCapacityFrames.load(std::memory_order_acquire) :
                owner->publishedValidFrames.load(std::memory_order_acquire);
            haveTape = total != 0;
            progress = tapeProgress(position, total);
        }
        // Draw the housing beneath the tape so it can visibly cross the rear rim.
        for (unsigned i = 0; i < 2; ++i) {
            if (!visible[i] || radii[i] <= 0.f) continue;
            const Vec p = intakePoints[i];
            const Vec size = mm2px(Vec(3.2f, 1.1f));
            const float x = p.x - size.x * 0.5f;
            const float y = p.y - size.y * 0.5f;
            nvgBeginPath(args.vg);
            nvgRoundedRect(args.vg, x, y, size.x, size.y, size.y * 0.4f);
            nvgFillPaint(args.vg, nvgLinearGradient(args.vg, x, y, x, y + size.y,
                nvgRGB(140, 161, 166), nvgRGB(43, 61, 69)));
            nvgFill(args.vg);
            nvgBeginPath(args.vg);
            nvgRoundedRect(args.vg, x + 1.f, y + 1.f, size.x - 2.f,
                size.y - 2.f, 0.6f);
            nvgFillColor(args.vg, nvgRGB(5, 12, 17));
            nvgFill(args.vg);
        }
        if (haveTape) {
            const float amounts[2] = {1.f - progress, progress};
            // Opaque shared color avoids double-alpha brightening at the tape/spool overlap.
            const NVGcolor tapeColor = nvgRGB(106, 221, 215);
            for (unsigned i = 0; i < 2; ++i) {
                if (!visible[i] || radii[i] <= 0.f) continue;
                const float tape = tapeRadius(radii[i], amounts[i]);
                const float tapeWidth = std::max(1.f, radii[i] * 0.055f);
                // Inset the stroke centerline so its outer edge meets the wound tape.
                const Vec exit = tapeExitPoint(centers[i], intakePoints[i],
                    std::max(0.f, tape - tapeWidth * 0.5f));
                nvgBeginPath(args.vg);
                nvgMoveTo(args.vg, exit.x, exit.y);
                nvgLineTo(args.vg, intakePoints[i].x, intakePoints[i].y);
                nvgStrokeColor(args.vg, tapeColor);
                nvgStrokeWidth(args.vg, tapeWidth);
                nvgLineCap(args.vg, NVG_ROUND);
                nvgStroke(args.vg);
                nvgBeginPath(args.vg);
                nvgCircle(args.vg, centers[i].x, centers[i].y, tape);
                nvgFillColor(args.vg, tapeColor);
                nvgFill(args.vg);
            }
        }
        // The front lip hides the endpoint after the tape enters the slot center.
        for (unsigned i = 0; i < 2; ++i) {
            if (!visible[i] || radii[i] <= 0.f) continue;
            const Vec p = intakePoints[i];
            const Vec size = mm2px(Vec(3.2f, 1.1f));
            const float x = p.x - size.x * 0.5f;
            const float y = p.y - size.y * 0.5f;
            nvgSave(args.vg);
            nvgIntersectScissor(args.vg, x, p.y, size.x, size.y * 0.5f + 1.f);
            nvgBeginPath(args.vg);
            nvgRoundedRect(args.vg, x, y, size.x, size.y, size.y * 0.4f);
            nvgFillPaint(args.vg, nvgLinearGradient(args.vg, x, p.y, x, y + size.y,
                nvgRGB(91, 114, 123), nvgRGB(43, 61, 69)));
            nvgFill(args.vg);
            nvgRestore(args.vg);
        }
        for (unsigned i = 0; i < 2; ++i) {
            if (!visible[i] || radii[i] <= 0.f) continue;
            nvgSave(args.vg);
            nvgTranslate(args.vg, centers[i].x, centers[i].y);
            nvgRotate(args.vg, angle);
            nvgBeginPath(args.vg);
            nvgCircle(args.vg, 0.f, 0.f, radii[i]);
            nvgFillPaint(args.vg, nvgImagePattern(
                args.vg, -radii[i], -radii[i], 2.f * radii[i], 2.f * radii[i],
                0.f, imageHandle, 1.f));
            nvgFill(args.vg);
            nvgRestore(args.vg);
        }
    }
};

struct ChimeraDisplayOverlay : Widget {
    chimera::RenderFrameMetrics* metrics = nullptr;
    Chimera* owner = nullptr;
    chimera::MarkerDisplay markers;
    std::shared_ptr<const chimera::WaveformSummary> summary;
    std::string stateText = "REEL READY";
    std::string detailText = "NO REEL";
    std::string saveText;
    int cachedState = -1;
    int cachedErrorKind = -1;
    int cachedPreRecord = -1;
    bool cachedBusy = false, cachedDirty = false;
    std::uint32_t cachedFrames = UINT32_MAX;
    unsigned cachedCount = UINT32_MAX, cachedCurrent = UINT32_MAX;
    unsigned cachedRequested = UINT32_MAX;

    static float markerStemWidth(unsigned count) {
        return count > 120u ? 0.8f : (count > 48u ? 1.f : 1.35f);
    }

    struct Layer : Widget {
        ChimeraDisplayOverlay* display = nullptr;
        int part = 0;
        void draw(const DrawArgs& args) override { display->drawContents(args, part); }
    };
    widget::FramebufferWidget* markerCache = nullptr;
    widget::FramebufferWidget* foregroundCache = nullptr;
    Layer* layers[5] = {};
    std::uint32_t renderFrames = 0;
    unsigned renderCurrent = 0, renderRequested = 0;
    Chimera* renderOwner = nullptr;
    bool textChanged = false;
    struct MarkerTarget {
        bool valid = false;
        bool remove = false;
        unsigned index = 0;
        std::uint32_t frame = 0;
        float x = 0.f;
    };
    MarkerTarget pendingTarget;
    Chimera* pendingOwner = nullptr;
    std::uint64_t pendingRevision = 0;
    Vec hoverPosition;
    Vec dragMovement;
    bool hovered = false;
    bool unspliceHovered = false;
    bool dragCancelled = false;

    static float xForFrame(std::uint32_t frame, std::uint32_t frames, float displayWidth) {
        if (!frames || displayWidth <= 10.f) return 5.f;
        return 5.f + (displayWidth - 10.f) *
            float(std::min(frame, frames)) / float(frames);
    }
    static std::uint32_t frameForX(float x, std::uint32_t frames, float displayWidth) {
        if (frames < 2 || displayWidth <= 10.f) return 0;
        const double fraction = double(clamp(x, 5.f, displayWidth - 5.f) - 5.f) /
            double(displayWidth - 10.f);
        return std::uint32_t(std::max(1.0, std::min(double(frames - 1),
            std::round(fraction * double(frames)))));
    }
    bool markerEditsReady() const {
        const float w = box.size.x, h = box.size.y;
        return owner && renderFrames >= 2 && w > 10.f && h > 5.f &&
            !owner->recordingOrArmed.load(std::memory_order_acquire) &&
            !owner->ioBusy.load(std::memory_order_acquire) &&
            markers.documentRevision == owner->publishedDocumentRevision.load(std::memory_order_acquire);
    }
    MarkerTarget targetAt(Vec position, bool remove) const {
        MarkerTarget target;
        const float w = box.size.x, h = box.size.y;
        if (!markerEditsReady() ||
            position.x < 5.f || position.x > w - 5.f ||
            position.y < 4.f || position.y > h * 0.82f) return target;
        if (remove) {
            float closest = 6.f;
            for (unsigned i = 1; i < markers.count; ++i) {
                const float markerX = xForFrame(markers.markers[i], renderFrames, w);
                const float distance = std::fabs(markerX - position.x);
                if (distance >= closest) continue;
                closest = distance;
                target.valid = true;
                target.remove = true;
                target.index = i;
                target.frame = markers.markers[i];
                target.x = markerX;
            }
            return target;
        }
        if (markers.count >= chimera::kMaxSplices) return target;
        target.frame = frameForX(position.x, renderFrames, w);
        for (unsigned i = 0; i < markers.count; ++i)
            if (markers.markers[i] == target.frame) return MarkerTarget();
        target.valid = true;
        target.x = xForFrame(target.frame, renderFrames, w);
        return target;
    }
    MarkerTarget unspliceButtonTarget() const {
        MarkerTarget target;
        if (!markerEditsReady()) return target;
        const unsigned index = owner->publishedRegion.load(std::memory_order_acquire) + 1u;
        if (index >= markers.count) return target;
        target.valid = true;
        target.remove = true;
        target.index = index;
        target.frame = markers.markers[index];
        target.x = xForFrame(target.frame, renderFrames, box.size.x);
        return target;
    }
    void onHover(const event::Hover& e) override {
        hovered = true;
        hoverPosition = e.pos;
        e.consume(this);
    }
    void onLeave(const event::Leave& e) override {
        hovered = false;
        Widget::onLeave(e);
    }
    void onButton(const event::Button& e) override {
        if (e.button != GLFW_MOUSE_BUTTON_LEFT) return;
        if (e.action == GLFW_PRESS) {
            pendingTarget = targetAt(e.pos, (e.mods & GLFW_MOD_SHIFT) != 0);
            pendingOwner = owner;
            pendingRevision = owner ? owner->publishedDocumentRevision.load(std::memory_order_acquire) : 0;
            dragMovement = Vec();
            dragCancelled = false;
        }
        e.consume(this);
    }
    void onDragMove(const event::DragMove& e) override {
        dragMovement = dragMovement.plus(e.mouseDelta);
        if (std::fabs(dragMovement.x) > 4.f || std::fabs(dragMovement.y) > 4.f)
            dragCancelled = true;
    }
    void onDragDrop(const event::DragDrop& e) override {
        if (e.button != GLFW_MOUSE_BUTTON_LEFT || e.origin != this) return;
        if (pendingTarget.valid && !dragCancelled && owner && owner == pendingOwner &&
            owner->publishedDocumentRevision.load(std::memory_order_acquire) == pendingRevision) {
            std::string error;
            const bool accepted = pendingTarget.remove ?
                owner->requestMarkerEdit(pendingTarget.index, 0, 1u, error) :
                owner->requestInsertMarker(pendingTarget.frame, error);
            if (!accepted && !error.empty())
                osdialog_message(OSDIALOG_ERROR, OSDIALOG_OK, error.c_str());
        }
        pendingTarget = MarkerTarget();
        e.consume(this);
    }

    void initializeLayers() {
        for (int part = 0; part < 5; ++part) {
            layers[part] = new Layer;
            layers[part]->display = this;
            layers[part]->part = part;
            if (part == 1 || part == 2 || part == 3) addChild(layers[part]);
            else {
                auto* cache = new widget::FramebufferWidget;
                cache->oversample = 1.f;
                cache->addChild(layers[part]);
                addChild(cache);
                if (part == 0) markerCache = cache;
                else foregroundCache = cache;
            }
        }
    }

    void step() override {
        textChanged = false;
        const bool markerChange = owner ? owner->markerDisplay.consume(markers, renderOwner != owner) : false;
        updateState();
        const auto frames = owner ? std::max(markers.frames,
            owner->publishedValidFrames.load(std::memory_order_acquire)) : 0;
        const unsigned current = owner ? owner->publishedRegion.load(std::memory_order_acquire) : 0;
        const unsigned requested = owner ? owner->publishedRequestedRegion.load(std::memory_order_acquire) : 0;
        const bool resized = layers[0] && (layers[0]->box.size.x != box.size.x ||
            layers[0]->box.size.y != box.size.y);
        if (markerCache && (markerChange || renderOwner != owner || frames != renderFrames || resized))
            markerCache->setDirty();
        if ((textChanged || resized || renderOwner != owner) && foregroundCache)
            foregroundCache->setDirty();
        renderOwner = owner;
        renderFrames = frames;
        renderCurrent = current;
        renderRequested = requested;
        if (markerCache) {
            markerCache->box.size = box.size;
            for (auto* layer : layers) layer->box.size = box.size;
            // The footer cache only covers text; selection highlights stay live.
            const float footerY = std::floor(box.size.y * 0.82f);
            foregroundCache->box.pos = Vec(0.f, footerY);
            foregroundCache->box.size = Vec(box.size.x, box.size.y - footerY);
            layers[4]->box.size = foregroundCache->box.size;
        }
        Widget::step();
    }

    void draw(const DrawArgs& args) override {
        if (owner && !args.fb)
            owner->displayHeartbeatNs.store(Chimera::steadyNs(), std::memory_order_release);
        Widget::draw(args);
    }

    void updateState() {
        if (!owner) markers = chimera::MarkerDisplay{};
        if (!owner) {
            textChanged = stateText != "CHIMERA" || detailText != "REEL ENGINE" || !saveText.empty();
            stateText = "CHIMERA";
            detailText = "REEL ENGINE";
            saveText.clear();
            return;
        }
        const int state = owner->publishedRecordState.load(std::memory_order_acquire);
        const int preRecord = owner->preRecordStatus();
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
        if (state == cachedState && preRecord == cachedPreRecord &&
            errorKind == cachedErrorKind && busy == cachedBusy &&
            dirty == cachedDirty && (frames == 0) == (cachedFrames == 0) &&
            frames / 48000 == cachedFrames / 48000 &&
            count == cachedCount && current == cachedCurrent &&
            requested == cachedRequested && renderOwner == owner) return;
        textChanged = true;
        cachedState = state;
        cachedPreRecord = preRecord;
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
                detailText += u8" \u2192 " + std::to_string(requested + 1);
        }
        // Keep transport and patch-save status visible at the sides. A missed
        // safeguard remains visible after stop, until the next take or Reel.
        if (preRecord == Chimera::PreRecordUnavailable) detailText = "NO PRE-REC CUT";
        else if (preRecord == Chimera::PreRecordFailed) detailText = "PRE-REC FAILED";
        else if (preRecord == Chimera::PreRecordPending) detailText = "PRE-REC SAVING";
    }

    void drawContents(const DrawArgs& args, int part) {
        chimera::RenderComponentTimer timer(metrics, part == 0 || part == 4);
        if (!APP || !APP->window || !APP->window->uiFont) return;
        NVGcontext* vg = args.vg;
        const float w = box.size.x, h = box.size.y;
        const float left = 5.f, width = w - 10.f;
        const float traceTop = 4.f, traceBottom = h * 0.82f;
        if (part == 3) {
            const bool removePreview = APP->window->getMods() & GLFW_MOD_SHIFT;
            const MarkerTarget preview = hovered ? targetAt(hoverPosition, removePreview) :
                (unspliceHovered ? unspliceButtonTarget() : MarkerTarget());
            if (preview.valid) {
                const NVGcolor color = preview.remove ?
                    nvgRGBA(255, 100, 112, 230) : nvgRGBA(106, 221, 215, 210);
                nvgBeginPath(vg);
                nvgMoveTo(vg, preview.x, traceTop);
                nvgLineTo(vg, preview.x, traceBottom);
                nvgStrokeColor(vg, color);
                nvgStrokeWidth(vg, preview.remove ? 3.f : 1.5f);
                nvgStroke(vg);
                nvgBeginPath(vg);
                nvgCircle(vg, preview.x, traceTop + 2.5f, 2.5f);
                nvgFillColor(vg, color);
                nvgFill(vg);
            }
            return;
        }
        // Marker metadata comes directly from the core, independently of the
        // slower waveform scan. The summary only supplies the waveform backdrop.
        const std::uint32_t displayFrames = renderFrames;
        if (owner && displayFrames) {
            const auto xFor = [&](std::uint32_t frame) {
                return left + width * float(std::min(frame, displayFrames)) /
                    float(displayFrames);
            };
            const unsigned current = renderCurrent;
            const unsigned requested = renderRequested;
            const NVGcolor ordinaryColor = nvgRGBA(238, 177, 83, 225);
            const NVGcolor currentColor = nvgRGBA(166, 244, 96, 255);
            const NVGcolor requestedColor = nvgRGBA(196, 161, 246, 255);

            if (part == 0) {
                // Stems retain precise timing at any density. Marker zero is now
                // included, so the first Splice has a visible anchor at the left.
                nvgBeginPath(vg);
                for (unsigned i = 0; i < markers.count; ++i) {
                    const float x = xFor(markers.markers[i]);
                    nvgMoveTo(vg, x, traceTop);
                    nvgLineTo(vg, x, traceBottom);
                }
                nvgStrokeColor(vg, ordinaryColor);
                nvgStrokeWidth(vg, markerStemWidth(markers.count));
                nvgStroke(vg);
            }
            if (part == 1) {
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
            if (part == 2) {
                // Requested is purple and Current is green. Organize and Shift
                // both move Requested. When they coincide, purple remains visible.
                const auto drawEmphasizedAnchor = [&](unsigned index, NVGcolor color,
                                                       float stemWidth) {
                    if (index >= markers.count) return;
                    const float x = xFor(markers.markers[index]);
                    nvgBeginPath(vg);
                    nvgMoveTo(vg, x, traceTop);
                    nvgLineTo(vg, x, traceBottom);
                    nvgStrokeColor(vg, color);
                    nvgStrokeWidth(vg, stemWidth);
                    nvgStroke(vg);
                };
                drawEmphasizedAnchor(requested, requestedColor,
                    requested == current ? 4.25f : 2.f);
                drawEmphasizedAnchor(current, currentColor, 2.35f);
            }
        }
        if (part != 4) return;
        const float textY = h * 0.92f - std::floor(h * 0.82f);
        nvgFontFaceId(vg, APP->window->uiFont->handle);
        nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
        nvgFontSize(vg, 9.f);
        nvgFillColor(vg, nvgRGB(246, 231, 193));
        nvgText(vg, 6.f, textY, stateText.c_str(), nullptr);
        nvgTextAlign(vg, NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);
        nvgText(vg, w - 6.f, textY, saveText.c_str(), nullptr);
        nvgFontSize(vg, 8.5f);
        nvgFillColor(vg, nvgRGB(166, 209, 212));
        nvgTextAlign(vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
        nvgText(vg, w * 0.5f, textY, detailText.c_str(), nullptr);
    }
};
