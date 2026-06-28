#include "Wyrm.hpp"
#include "WyrmSand.hpp"
#include "PanelSvgUtils.hpp"
#include "visual/VisualAssets.hpp"

#include <cstdio>

void drawWyrmStepTriangle(const Widget::DrawArgs& args, const Vec& size, bool pointRight) {
	const float cx = 0.5f * size.x;
	const float cy = 0.5f * size.y;
	const float halfW = 2.8f;
	const float halfH = 3.3f;
	const float offset = pointRight ? (halfW / 3.f) : (-halfW / 3.f);
	nvgBeginPath(args.vg);
	if (pointRight) {
		nvgMoveTo(args.vg, cx - halfW + offset, cy - halfH);
		nvgLineTo(args.vg, cx + halfW + offset, cy);
		nvgLineTo(args.vg, cx - halfW + offset, cy + halfH);
	}
	else {
		nvgMoveTo(args.vg, cx + halfW + offset, cy - halfH);
		nvgLineTo(args.vg, cx - halfW + offset, cy);
		nvgLineTo(args.vg, cx + halfW + offset, cy + halfH);
	}
	nvgClosePath(args.vg);
	nvgFillColor(args.vg, nvgRGBA(225, 232, 240, 244));
	nvgFill(args.vg);
}

struct WyrmWaveLeftButton final : TL1105 {
	Wyrm* module = nullptr;
	void onButton(const event::Button& e) override {
		TL1105::onButton(e);
		if (module && e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS) {
			const int current = clamp(module->selectedShape, 0, SHAPE_COUNT - 1);
			const int next = (current + SHAPE_COUNT - 1) % SHAPE_COUNT;
			module->setFactoryShape(next);
		}
	}
	void draw(const DrawArgs& args) override {
		TL1105::draw(args);
		drawWyrmStepTriangle(args, box.size, false);
	}
};

struct WyrmWaveRightButton final : TL1105 {
	Wyrm* module = nullptr;
	void onButton(const event::Button& e) override {
		TL1105::onButton(e);
		if (module && e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS) {
			const int current = clamp(module->selectedShape, 0, SHAPE_COUNT - 1);
			const int next = (current + 1) % SHAPE_COUNT;
			module->setFactoryShape(next);
		}
	}
	void draw(const DrawArgs& args) override {
		TL1105::draw(args);
		drawWyrmStepTriangle(args, box.size, true);
	}
};

struct WyrmShapeMenuItem : MenuItem {
	Wyrm* module = nullptr;
	int shape = SHAPE_SINE;

	void onAction(const event::Action& e) override {
		if (module) module->setFactoryShape(shape);
		MenuItem::onAction(e);
	}

	void step() override {
		rightText = (module && module->selectedShape == shape) ? "✓" : "";
		MenuItem::step();
	}
};

struct WyrmPointCountMenuItem : MenuItem {
	Wyrm* module = nullptr;
	int count = kWyrmPointCountDefault;

	void onAction(const event::Action& e) override {
		if (module) {
			module->setPointCount(count);
		}
		MenuItem::onAction(e);
	}

	void step() override {
		rightText = (module && module->pointCount == count) ? "✓" : "";
		MenuItem::step();
	}
};

struct WyrmEditorIconButton : TransparentWidget {
	enum Kind {
		LOCK,
		RESET
	};

	Wyrm* module = nullptr;
	Kind kind = LOCK;
	bool hovered = false;
	std::shared_ptr<window::Svg> lockClosedNormalSvg;
	std::shared_ptr<window::Svg> lockClosedHighlightedSvg;
	std::shared_ptr<window::Svg> lockOpenNormalSvg;
	std::shared_ptr<window::Svg> lockOpenHighlightedSvg;
	std::shared_ptr<window::Svg> resetNormalSvg;
	std::shared_ptr<window::Svg> resetHighlightedSvg;

	WyrmEditorIconButton(Wyrm* module, Kind kind) {
		this->module = module;
		this->kind = kind;
		if (kind == LOCK) {
			lockClosedNormalSvg = Svg::load(asset::plugin(pluginInstance, "res/icon/lock_closed-normal.svg"));
			lockClosedHighlightedSvg = Svg::load(asset::plugin(pluginInstance, "res/icon/lock_closed-highlighted.svg"));
			lockOpenNormalSvg = Svg::load(asset::plugin(pluginInstance, "res/icon/lock_open-normal.svg"));
			lockOpenHighlightedSvg = Svg::load(asset::plugin(pluginInstance, "res/icon/lock_open-highlighted.svg"));
		}
		else {
			resetNormalSvg = Svg::load(asset::plugin(pluginInstance, "res/icon/reset-normal.svg"));
			resetHighlightedSvg = Svg::load(asset::plugin(pluginInstance, "res/icon/reset-highlighted.svg"));
		}
	}

	void step() override {
		hovered = false;
		if (parent && APP && APP->scene && APP->scene->rack) {
			const Vec local = APP->scene->rack->getMousePos().minus(parent->box.pos).minus(box.pos);
			hovered = (local.x >= 0.f && local.x <= box.size.x && local.y >= 0.f && local.y <= box.size.y);
		}
		TransparentWidget::step();
	}

	void onHover(const event::Hover& e) override {
		hovered = true;
		TransparentWidget::onHover(e);
	}

	void onLeave(const event::Leave& e) override {
		hovered = false;
		TransparentWidget::onLeave(e);
	}

	void onButton(const event::Button& e) override {
		if (!module || e.button != GLFW_MOUSE_BUTTON_LEFT || e.action != GLFW_PRESS) {
			TransparentWidget::onButton(e);
			return;
		}
		if (kind == LOCK) {
			module->editorLocked.store(!module->editorLocked.load(std::memory_order_relaxed), std::memory_order_relaxed);
		}
		else {
			module->setFactoryShape(module->selectedShape);
		}
		e.consume(this);
	}

	void drawSvgIcon(const DrawArgs& args, const std::shared_ptr<window::Svg>& svg) {
		if (!svg) {
			return;
		}
		const Vec svgSize = svg->getSize();
		if (svgSize.x <= 1.f || svgSize.y <= 1.f) {
			return;
		}
		const float targetSize = 0.72f * std::min(box.size.x, box.size.y);
		const float scale = targetSize / std::max(svgSize.x, svgSize.y);
		nvgSave(args.vg);
		nvgTranslate(args.vg, 0.5f * box.size.x, 0.5f * box.size.y);
		nvgScale(args.vg, scale, scale);
		nvgTranslate(args.vg, -0.5f * svgSize.x, -0.5f * svgSize.y);
		svg->draw(args.vg);
		nvgRestore(args.vg);
	}

	void drawLockIcon(const DrawArgs& args) {
		const bool locked = module && module->editorLocked.load(std::memory_order_relaxed);
		const std::shared_ptr<window::Svg>& svg = locked
			? (hovered ? lockClosedHighlightedSvg : lockClosedNormalSvg)
			: (hovered ? lockOpenHighlightedSvg : lockOpenNormalSvg);
		drawSvgIcon(args, svg);
	}

	void drawResetIcon(const DrawArgs& args) {
		drawSvgIcon(args, hovered ? resetHighlightedSvg : resetNormalSvg);
	}

	void draw(const DrawArgs& args) override {
		if (kind == LOCK) {
			drawLockIcon(args);
		}
		else {
			drawResetIcon(args);
		}
	}
};

struct WyrmFrequencyReadoutWidget final : Widget {
	Wyrm* module = nullptr;

	static std::string formatFrequencyText(float hz) {
		if (!std::isfinite(hz) || hz < 0.f) {
			hz = 0.f;
		}
		if (hz < 1.f) {
			return string::f("%.1f mHz", hz * 1000.f);
		}
		if (hz >= 1000.f) {
			return string::f("%.2f kHz", hz / 1000.f);
		}
		if (hz < 10.f) {
			return string::f("%.2f Hz", hz);
		}
		if (hz < 100.f) {
			return string::f("%.1f Hz", hz);
		}
		return string::f("%.0f Hz", hz);
	}

	void draw(const DrawArgs& args) override {
		if (!module || !APP || !APP->window || !APP->window->uiFont) {
			return;
		}
		nvgFontSize(args.vg, std::max(9.5f, box.size.y * 0.72f));
		nvgFontFaceId(args.vg, APP->window->uiFont->handle);
		nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 255));
		nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
		const float displayHz = module->displayFrequencyHz.load(std::memory_order_relaxed);
		const std::string freqText = formatFrequencyText(displayHz);
		std::string readoutText;
		if (module->waveCustomized) {
			readoutText = string::f("Custom: %s", freqText.c_str());
		}
		else {
			const int shapeIndex = clamp(module->selectedShape, 0, SHAPE_COUNT - 1);
			readoutText = string::f("%s: %s", kWyrmShapeLabels[shapeIndex], freqText.c_str());
		}
		nvgText(args.vg, 0.5f * box.size.x, 0.5f * box.size.y, readoutText.c_str(), nullptr);
	}
};

struct WyrmWidget : ModuleWidget {
	std::shared_ptr<window::Svg> ageSigilSvg;
	bool ageSigilUnlocked = false;

	explicit WyrmWidget(Wyrm* module) {
		setModule(module);
		PreviewBuildLogTimer previewBuildTimer("Wyrm", module);
		const std::string panelPath = asset::plugin(pluginInstance, "res/wyrm.svg");
		setPanel(createPanel(panelPath));
		previewBuildTimer.markPanelDone();
		try {
			ageSigilSvg = Svg::load(asset::plugin(pluginInstance, "res/icon/Vahdrim'Keth.svg"));
		}
		catch (const std::exception& e) {
			WARN("Wyrm: failed to load age sigil SVG: %s", e.what());
			ageSigilSvg.reset();
		}

		addChild(createWidget<CyanOrbScrew>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<CyanOrbScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<CyanOrbScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<CyanOrbScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		auto applyPt = [&](const char* id, Vec* pos) {
			Vec p;
			if (panel_svg::loadPointFromSvgMm(panelPath, id, &p)) {
				*pos = p;
			}
		};

		math::Rect editorRectMm(Vec(6.0f, 16.0f), Vec(59.12f, 52.0f));
		panel_svg::loadRectFromSvgMm(panelPath, "WYRm_WAVE_EDITOR", &editorRectMm);
		math::Rect freqReadoutRectMm(Vec(editorRectMm.pos.x, editorRectMm.pos.y + editorRectMm.size.y + 1.1f), Vec(editorRectMm.size.x, 3.8f));
		Vec freqPos(17.5f, 80.0f);
		Vec waveformSelectPos(35.56f, 75.2f);
		Vec finePos(35.56f, 80.0f);
		Vec fmAttenPos(53.62f, 80.0f);
		Vec foldPos(35.56f, 98.0f);
		Vec lockPos(10.50f, 75.50f);
		Vec resetPos(17.25f, 75.50f);
		Vec slitherPos(17.50f, 112.80f);
		Vec slitherSpeedPos(26.50f, 112.80f);
		Vec slitherCvPos = slitherPos.plus(Vec(6.8f, 0.f));
		Vec slitherSpeedCvPos = slitherSpeedPos.plus(Vec(6.8f, 0.f));
		Vec voctPos(14.0f, 111.0f);
		Vec fmPos(28.0f, 111.0f);
		Vec syncPos(43.0f, 111.0f);
		Vec syncModePos(50.0f, 111.0f);
		Vec lfoModePos(57.0f, 111.0f);
		Vec foldCvPos(64.0f, 111.0f);
		Vec rawOutPos(24.0f, 122.0f);
		Vec outPos(47.0f, 122.0f);
		applyPt("WYRM_FREQ_PARAM", &freqPos);
		applyPt("WYRM_WAVEFORM_SELECT", &waveformSelectPos);
		applyPt("WYRM_FINE_PARAM", &finePos);
		applyPt("WYRM_FM_ATTEN_PARAM", &fmAttenPos);
		applyPt("WYRM_FOLD_PARAM", &foldPos);
		applyPt("WYRM_LOCK_BUTTON", &lockPos);
		applyPt("WYRM_RESET_BUTTON", &resetPos);
		applyPt("WYRM_SLITHER_PARAM", &slitherPos);
		applyPt("WYRM_SLITHER_SPEED_PARAM", &slitherSpeedPos);
		applyPt("WYRM_SLITHER_CV_INPUT", &slitherCvPos);
		applyPt("WYRM_SLITHER_SPEED_CV_INPUT", &slitherSpeedCvPos);
		applyPt("WYRM_VOCT_INPUT", &voctPos);
		applyPt("WYRM_FM_INPUT", &fmPos);
		applyPt("WYRM_SYNC_INPUT", &syncPos);
		applyPt("WYRM_SYNC_MODE_PARAM", &syncModePos);
		applyPt("WYRM_LFO_MODE_PARAM", &lfoModePos);
		applyPt("WYRM_FOLD_CV_INPUT", &foldCvPos);
		applyPt("WYRM_RAW_OUTPUT", &rawOutPos);
		applyPt("WYRM_OUT_OUTPUT", &outPos);
		previewBuildTimer.setAtlasStatus(panel_svg::getAtlasStatusLabelForSvg(panelPath));
		previewBuildTimer.markAnchorsDone();

		std::shared_ptr<WyrmSand> sandState = std::make_shared<WyrmSand>();
		auto* sandGl = createWyrmSandGlWidget(module, sandState);
		sandGl->box.pos = mm2px(editorRectMm.pos);
		sandGl->box.size = mm2px(editorRectMm.size);
		addChild(sandGl);
		auto* editor = createWyrmWaveEditor(module, sandState);
		auto* editorFb = new widget::FramebufferWidget();
		editorFb->box.pos = mm2px(editorRectMm.pos);
		editorFb->box.size = mm2px(editorRectMm.size);
		editorFb->dirtyOnSubpixelChange = false;
		editor->box.size = editorFb->box.size;
		editorFb->addChild(editor);
		addChild(editorFb);
		auto* freqReadout = new WyrmFrequencyReadoutWidget();
		freqReadout->module = module;
		freqReadout->box.pos = mm2px(freqReadoutRectMm.pos);
		freqReadout->box.size = mm2px(freqReadoutRectMm.size);
		addChild(freqReadout);

		auto addEditorIconButton = [&](WyrmEditorIconButton::Kind kind, Vec posMm) {
			auto* button = new WyrmEditorIconButton(module, kind);
			button->box.size = mm2px(Vec(5.2f, 5.2f));
			button->box.pos = mm2px(posMm).minus(button->box.size.mult(0.5f));
			addChild(button);
		};
		addEditorIconButton(WyrmEditorIconButton::LOCK, lockPos);
		addEditorIconButton(WyrmEditorIconButton::RESET, resetPos);
		auto* waveLeft = createParamCentered<WyrmWaveLeftButton>(mm2px(waveformSelectPos.plus(Vec(-2.5f, 0.f))), module, Wyrm::WAVE_LEFT_PARAM);
		waveLeft->module = module;
		addParam(waveLeft);
		auto* waveRight = createParamCentered<WyrmWaveRightButton>(mm2px(waveformSelectPos.plus(Vec(2.5f, 0.f))), module, Wyrm::WAVE_RIGHT_PARAM);
		waveRight->module = module;
		addParam(waveRight);

		addParam(createParamCentered<LeviathanHaloKnob2>(mm2px(freqPos), module, Wyrm::FREQ_PARAM));
		addParam(createParamCentered<BipolarTinyClockworkGearKnob>(mm2px(finePos), module, Wyrm::FINE_PARAM));
		{
			Eclipse2Knob* fmAttenKnob = createParamCentered<Eclipse2Knob>(mm2px(fmAttenPos), module, Wyrm::FM_ATTEN_PARAM);
			fmAttenKnob->setProgressRingBipolar(true);
			addParam(fmAttenKnob);
		}
		addParam(createParamCentered<Eclipse2Knob>(mm2px(foldPos), module, Wyrm::FOLD_PARAM));
		addParam(createParamCentered<Eclipse2Knob>(mm2px(slitherPos), module, Wyrm::SLITHER_PARAM));
		addParam(createParamCentered<Eclipse2Knob>(mm2px(slitherSpeedPos), module, Wyrm::SLITHER_SPEED_PARAM));

		addInput(createInputCentered<Magitek2InputJack>(mm2px(voctPos), module, Wyrm::VOCT_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(fmPos), module, Wyrm::FM_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(syncPos), module, Wyrm::SYNC_INPUT));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<WhiteLight>>>(
			mm2px(syncModePos), module, Wyrm::SYNC_MODE_PARAM, Wyrm::SYNC_MODE_LIGHT
		));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<WhiteLight>>>(
			mm2px(lfoModePos), module, Wyrm::LFO_MODE_PARAM, Wyrm::LFO_MODE_LIGHT
		));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(foldCvPos), module, Wyrm::FOLD_CV_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(slitherCvPos), module, Wyrm::SLITHER_CV_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(slitherSpeedCvPos), module, Wyrm::SLITHER_SPEED_CV_INPUT));

		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(rawOutPos), module, Wyrm::RAW_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(outPos), module, Wyrm::OUT_OUTPUT));
	}

	void step() override {
		ModuleWidget::step();
		Wyrm* wyrm = dynamic_cast<Wyrm*>(module);
		if (!wyrm || ageSigilUnlocked) return;
		const double createdUnixTimeSec = wyrm->createdUnixTimeSec;
		if (std::isfinite(createdUnixTimeSec) && createdUnixTimeSec > 0.0) {
			ageSigilUnlocked = (system::getUnixTime() - createdUnixTimeSec) >= 666.0;
		}
	}

	void draw(const DrawArgs& args) override {
		ModuleWidget::draw(args);
		Wyrm* wyrm = dynamic_cast<Wyrm*>(module);
		if (wyrm && isDragonKingDebugEnabled() && APP && APP->window && APP->window->uiFont) {
			char debugIdLabel[32];
			std::snprintf(debugIdLabel, sizeof(debugIdLabel), "ID:%u", wyrm->debugInstanceId);
			const float x = box.size.x - mm2px(0.9f);
			const float y = mm2px(2.5f);
			nvgSave(args.vg);
			nvgFontFaceId(args.vg, APP->window->uiFont->handle);
			nvgFontSize(args.vg, 6.8f);
			nvgTextAlign(args.vg, NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);
			nvgFillColor(args.vg, nvgRGBA(8, 10, 14, 210));
			nvgText(args.vg, x + 0.45f, y + 0.45f, debugIdLabel, nullptr);
			nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 230));
			nvgText(args.vg, x, y, debugIdLabel, nullptr);
			nvgRestore(args.vg);
		}
		if (!wyrm || !ageSigilSvg || !ageSigilUnlocked) {
			return;
		}
		const Vec sigilSize = mm2px(Vec(3.8f, 4.6f));
		const Vec rightSigilCenter = mm2px(Vec(54.8f, 4.47f));
		const Vec leftSigilCenter(box.size.x - rightSigilCenter.x, rightSigilCenter.y);
		const Vec svgSize = ageSigilSvg->getSize();
		if (svgSize.x <= 1.f || svgSize.y <= 1.f) {
			return;
		}
		const float scaleX = sigilSize.x / svgSize.x;
		const float scaleY = sigilSize.y / svgSize.y;
		auto drawSigilAt = [&](const Vec& center) {
			nvgSave(args.vg);
			nvgTranslate(args.vg, center.x, center.y);
			nvgScale(args.vg, scaleX, scaleY);
			nvgTranslate(args.vg, -svgSize.x * 0.5f, -svgSize.y * 0.5f);
			ageSigilSvg->draw(args.vg);
			nvgRestore(args.vg);
		};
		drawSigilAt(leftSigilCenter);
		drawSigilAt(rightSigilCenter);
	}

	void appendContextMenu(Menu* menu) override {
		ModuleWidget::appendContextMenu(menu);
		auto* module = dynamic_cast<Wyrm*>(this->module);
		if (!module) return;
		auto sandRendererLabel = [&](int backend) {
			switch (backend) {
				case WYRM_RENDER_OPENGL: return "OpenGL";
				case WYRM_RENDER_OPENGL_SHDR: return "OpenGL SHDR";
				case WYRM_RENDER_NANOVG:
				default: return "NanoVG";
			}
		};
		auto applyRenderMode = [=](int mode) {
			mode = clamp(mode, WYRM_RENDER_NANOVG, WYRM_RENDER_OPENGL_SHDR);
			module->renderMode.store(mode, std::memory_order_relaxed);
			switch (mode) {
				case WYRM_RENDER_OPENGL:
					module->sandBackend.store(WYRMSAND_OPENGL_TEXTURE, std::memory_order_relaxed);
					break;
				case WYRM_RENDER_OPENGL_SHDR:
					module->sandBackend.store(WYRMSAND_SHADER_FEEDBACK, std::memory_order_relaxed);
					break;
				case WYRM_RENDER_NANOVG:
				default:
					module->sandBackend.store(WYRMSAND_NANOVG_IMAGE, std::memory_order_relaxed);
					break;
			}
		};
		auto sandDetailLabel = [&](int detail) {
			switch (detail) {
				case WYRMSAND_DETAIL_LOW: return "Low";
				case WYRMSAND_DETAIL_MEDIUM: return "Medium";
				case WYRMSAND_DETAIL_HIGH: return "High";
				case WYRMSAND_DETAIL_AUTO: return "Auto";
				default: return "Unknown";
			}
		};
		auto sandPersistenceLabel = [&](int persistence) {
			switch (persistence) {
				case WYRMSAND_PERSISTENCE_SHORT: return "Short";
				case WYRMSAND_PERSISTENCE_MEDIUM: return "Medium";
				case WYRMSAND_PERSISTENCE_LONG: return "Long";
				default: return "Unknown";
			}
		};

		menu->addChild(new MenuSeparator());
		menu->addChild(createCheckMenuItem("Lock Wave Editor", "",
			[=]() { return module->editorLocked.load(std::memory_order_relaxed); },
			[=]() { module->editorLocked.store(!module->editorLocked.load(std::memory_order_relaxed), std::memory_order_relaxed); }
		));
		menu->addChild(createSubmenuItem("Renderer", sandRendererLabel(module->renderMode.load(std::memory_order_relaxed)), [=](Menu* rendererMenu) {
			rendererMenu->addChild(createCheckMenuItem("NanoVG", "",
				[=]() { return module->renderMode.load(std::memory_order_relaxed) == WYRM_RENDER_NANOVG; },
				[=]() { applyRenderMode(WYRM_RENDER_NANOVG); }
			));
			rendererMenu->addChild(createCheckMenuItem("OpenGL", "",
				[=]() { return module->renderMode.load(std::memory_order_relaxed) == WYRM_RENDER_OPENGL; },
				[=]() { applyRenderMode(WYRM_RENDER_OPENGL); }
			));
			rendererMenu->addChild(createCheckMenuItem("OpenGL SHDR", "",
				[=]() { return module->renderMode.load(std::memory_order_relaxed) == WYRM_RENDER_OPENGL_SHDR; },
				[=]() { applyRenderMode(WYRM_RENDER_OPENGL_SHDR); }
			));
		}));
		menu->addChild(createSubmenuItem("Sand", "", [=](Menu* submenu) {
			submenu->addChild(createCheckMenuItem("Sand View", "",
				[=]() { return module->sandViewEnabled.load(std::memory_order_relaxed); },
				[=]() { module->sandViewEnabled.store(!module->sandViewEnabled.load(std::memory_order_relaxed), std::memory_order_relaxed); }
			));
			submenu->addChild(new MenuSeparator());
			submenu->addChild(createSubmenuItem("Detail", sandDetailLabel(module->sandDetail.load(std::memory_order_relaxed)), [=](Menu* detailMenu) {
				detailMenu->addChild(createCheckMenuItem("Auto", "",
					[=]() { return module->sandDetail.load(std::memory_order_relaxed) == WYRMSAND_DETAIL_AUTO; },
					[=]() { module->sandDetail.store(WYRMSAND_DETAIL_AUTO, std::memory_order_relaxed); }
				));
				detailMenu->addChild(createCheckMenuItem("Low", "",
					[=]() { return module->sandDetail.load(std::memory_order_relaxed) == WYRMSAND_DETAIL_LOW; },
					[=]() { module->sandDetail.store(WYRMSAND_DETAIL_LOW, std::memory_order_relaxed); }
				));
				detailMenu->addChild(createCheckMenuItem("Medium", "",
					[=]() { return module->sandDetail.load(std::memory_order_relaxed) == WYRMSAND_DETAIL_MEDIUM; },
					[=]() { module->sandDetail.store(WYRMSAND_DETAIL_MEDIUM, std::memory_order_relaxed); }
				));
				detailMenu->addChild(createCheckMenuItem("High", "",
					[=]() { return module->sandDetail.load(std::memory_order_relaxed) == WYRMSAND_DETAIL_HIGH; },
					[=]() { module->sandDetail.store(WYRMSAND_DETAIL_HIGH, std::memory_order_relaxed); }
				));
			}));
			submenu->addChild(createSubmenuItem("Persistence", sandPersistenceLabel(module->sandPersistence.load(std::memory_order_relaxed)), [=](Menu* persistenceMenu) {
				persistenceMenu->addChild(createCheckMenuItem("Short", "",
					[=]() { return module->sandPersistence.load(std::memory_order_relaxed) == WYRMSAND_PERSISTENCE_SHORT; },
					[=]() { module->sandPersistence.store(WYRMSAND_PERSISTENCE_SHORT, std::memory_order_relaxed); }
				));
				persistenceMenu->addChild(createCheckMenuItem("Medium", "",
					[=]() { return module->sandPersistence.load(std::memory_order_relaxed) == WYRMSAND_PERSISTENCE_MEDIUM; },
					[=]() { module->sandPersistence.store(WYRMSAND_PERSISTENCE_MEDIUM, std::memory_order_relaxed); }
				));
				persistenceMenu->addChild(createCheckMenuItem("Long", "",
					[=]() { return module->sandPersistence.load(std::memory_order_relaxed) == WYRMSAND_PERSISTENCE_LONG; },
					[=]() { module->sandPersistence.store(WYRMSAND_PERSISTENCE_LONG, std::memory_order_relaxed); }
				));
			}));
		}));
		menu->addChild(createSubmenuItem("Rocks", string::f("%d", module->rockCount), [=](Menu* submenu) {
			const bool dragModeSelected = (module->rockMouseMode == ROCK_MOUSE_DRAGS);
			const std::string dragLabel = dragModeSelected ? "Mouse Drags Rocks" : "Mouse Drags Rocks (shift)";
			const std::string liftLabel = dragModeSelected ? "Mouse Lifts Rocks (shift)" : "Mouse Lifts Rocks";
			submenu->addChild(createCheckMenuItem(
				dragLabel, "",
				[=]() {
					return module->rockMouseMode == ROCK_MOUSE_DRAGS;
				},
				[=]() {
					module->rockMouseMode = ROCK_MOUSE_DRAGS;
					module->liftedRock = -1;
					module->publishRockState();
				}));
			submenu->addChild(createCheckMenuItem(
				liftLabel, "",
				[=]() {
					return module->rockMouseMode == ROCK_MOUSE_LIFTS;
				},
				[=]() {
					module->rockMouseMode = ROCK_MOUSE_LIFTS;
					module->publishRockState();
				}));
			submenu->addChild(new MenuSeparator());
			for (int count = 0; count <= kWyrmMaxRocks; ++count) {
				submenu->addChild(createCheckMenuItem(
					string::f("%d", count), "",
					[=]() {
						return module->rockCount == count;
					},
					[=]() {
						module->setRockCount(count);
					}));
			}
		}));
		menu->addChild(new MenuSeparator());
		menu->addChild(createSubmenuItem("Point Count", string::f("%d", module->pointCount), [=](Menu* submenu) {
			for (int count : {32, 48, 64, 128, 256}) {
				auto* item = new WyrmPointCountMenuItem();
				item->text = string::f("%d", count);
				item->module = module;
				item->count = count;
				submenu->addChild(item);
			}
		}));
		menu->addChild(new MenuSeparator());
		menu->addChild(createMenuLabel("Factory Shape"));
		for (int i = 0; i < SHAPE_COUNT; ++i) {
			auto* item = new WyrmShapeMenuItem();
			item->text = kWyrmShapeLabels[i];
			item->module = module;
			item->shape = i;
			menu->addChild(item);
		}
	}
};

Model* modelWyrm = createModel<Wyrm, WyrmWidget>("Wyrm");
