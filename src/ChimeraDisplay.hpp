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

struct ChimeraVuMeterWidget final : TransparentWidget {
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
        faceImage.reset();
        overlayImage.reset();
        facePath.clear();
        overlayPath.clear();
        TransparentWidget::onContextDestroy(e);
    }

    void onContextCreate(const ContextCreateEvent& e) override {
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
        const std::string desired = asset::plugin(pluginInstance, relativePath);
        if (!image || path != desired) {
            image = APP->window->loadImage(desired);
            path = desired;
        }
        return visual_assets::loadRasterMipmapHandle(vg, image, desired);
    }

    void draw(const DrawArgs& args) override {
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
    static constexpr float kNormalRpm = 33.f;
    static constexpr float kTapeHubRatio = 0.36f;
    static constexpr float kTapeOuterRatio = 0.91f;
    Chimera* owner = nullptr;
    Vec centers[2];
    Vec intakePoints[2];
    float radii[2] = {0.f, 0.f};
    float angle = 0.f;
    double lastStepTime = 0.0;
    NVGcontext* ownerVg = nullptr;
    int imageHandle = -1;
    int imageWidth = 0, imageHeight = 0;
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

    ~ChimeraReelsWidget() override {
        nvg_gfx_lifecycle::resetOwnedNvgImage(
            ownerVg, imageHandle, imageWidth, imageHeight, nullptr, false);
    }

    void onContextDestroy(const ContextDestroyEvent& e) override {
        nvg_gfx_lifecycle::resetOwnedNvgImage(
            ownerVg, imageHandle, imageWidth, imageHeight, nullptr, false);
        loadedPath.clear();
        TransparentWidget::onContextDestroy(e);
    }

    void onContextCreate(const ContextCreateEvent& e) override {
        nvg_gfx_lifecycle::resetOwnedNvgImage(
            ownerVg, imageHandle, imageWidth, imageHeight, nullptr, false);
        loadedPath.clear();
        TransparentWidget::onContextCreate(e);
    }

    bool ensureImage(NVGcontext* vg) {
        if (!vg) return false;
        const std::string path = asset::plugin(pluginInstance, "res/icon/Reel4-256px.png");
        if (ownerVg == vg && loadedPath == path && imageHandle > 0 &&
            imageWidth > 0 && imageHeight > 0 &&
            nvg_gfx_lifecycle::ownedNvgImageSizeMatches(
                vg, imageHandle, imageWidth, imageHeight)) return true;
        nvg_gfx_lifecycle::resetOwnedNvgImage(
            ownerVg, imageHandle, imageWidth, imageHeight, vg, ownerVg == vg);
        loadedPath.clear();
        imageHandle = nvgCreateImage(vg, path.c_str(), NVG_IMAGE_GENERATE_MIPMAPS);
        if (imageHandle <= 0) {
            imageHandle = -1;
            return false;
        }
        ownerVg = vg;
        loadedPath = path;
        nvgImageSize(vg, imageHandle, &imageWidth, &imageHeight);
        if (imageWidth <= 0 || imageHeight <= 0) {
            nvg_gfx_lifecycle::resetOwnedNvgImage(
                ownerVg, imageHandle, imageWidth, imageHeight, vg, true);
            loadedPath.clear();
            return false;
        }
        return true;
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
        if (haveTape) {
            const float amounts[2] = {1.f - progress, progress};
            // Opaque shared color avoids double-alpha brightening at the tape/spool overlap.
            const NVGcolor tapeColor = nvgRGB(106, 221, 215);
            for (unsigned i = 0; i < 2; ++i) {
                if (radii[i] <= 0.f) continue;
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
        // Fixed guides cover the tape endpoints, including when the reels are empty.
        for (unsigned i = 0; i < 2; ++i) {
            if (radii[i] <= 0.f) continue;
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
        for (unsigned i = 0; i < 2; ++i) {
            if (radii[i] <= 0.f) continue;
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
            dirty == cachedDirty && frames / 48000 == cachedFrames / 48000 &&
            count == cachedCount && current == cachedCurrent &&
            requested == cachedRequested) return;
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
                detailText += " >" + std::to_string(requested + 1);
        }
        // Keep transport and patch-save status visible at the sides. A missed
        // safeguard remains visible after stop, until the next take or Reel.
        if (preRecord == Chimera::PreRecordUnavailable) detailText = "NO PRE-REC CUT";
        else if (preRecord == Chimera::PreRecordFailed) detailText = "PRE-REC FAILED";
        else if (preRecord == Chimera::PreRecordPending) detailText = "PRE-REC SAVING";
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
            const unsigned current = owner->publishedRegion.load(std::memory_order_acquire);
            const unsigned requested = owner->publishedRequestedRegion.load(std::memory_order_acquire);
            const NVGcolor ordinaryColor = nvgRGBA(238, 177, 83, 225);
            const NVGcolor currentColor = nvgRGBA(166, 244, 96, 255);
            const NVGcolor requestedColor = nvgRGBA(196, 161, 246, 255);

            // Stems retain precise timing at any density. Marker zero is now
            // included, so the first Splice has a visible anchor at the left.
            nvgBeginPath(vg);
            for (unsigned i = 0; i < markers.count; ++i) {
                if (i == current || (i == requested && requested != current)) continue;
                const float x = xFor(markers.markers[i]);
                nvgMoveTo(vg, x, traceTop);
                nvgLineTo(vg, x, traceBottom);
            }
            nvgStrokeColor(vg, ordinaryColor);
            nvgStrokeWidth(vg, markerStemWidth(markers.count));
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

            // Selected and queued markers retain distinct colors and thicker stems.
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
            if (requested != current)
                drawEmphasizedAnchor(requested, requestedColor, 2.f);
            drawEmphasizedAnchor(current, currentColor, 2.35f);
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
