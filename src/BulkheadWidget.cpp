#include "Bulkhead.hpp"
#include "PanelSvgUtils.hpp"

namespace {

constexpr float CANVAS_PAD = 6.f;
constexpr float ROOM_MIN_GAP = 0.5f;

bulkhead::geometry::RoomBounds worldBounds() {
	bulkhead::geometry::RoomBounds b;
	b.left = -10.f;
	b.right = 10.f;
	b.bottom = -6.f;
	b.top = 6.f;
	return b;
}

bulkhead::geometry::Vec2 makeVec2(float x, float y) {
	bulkhead::geometry::Vec2 v;
	v.x = x;
	v.y = y;
	return v;
}

inline Vec roomToCanvas(const bulkhead::geometry::RoomBounds& bounds, const bulkhead::geometry::Vec2& point, const Vec& size) {
	const float x = rescale(clamp(point.x, bounds.left, bounds.right), bounds.left, bounds.right, CANVAS_PAD, size.x - CANVAS_PAD);
	const float y = rescale(clamp(point.y, bounds.bottom, bounds.top), bounds.bottom, bounds.top, size.y - CANVAS_PAD, CANVAS_PAD);
	return Vec(x, y);
}

inline bulkhead::geometry::Vec2 canvasToRoom(const bulkhead::geometry::RoomBounds& bounds, const Vec& point, const Vec& size) {
	bulkhead::geometry::Vec2 p;
	p.x = rescale(clamp(point.x, CANVAS_PAD, size.x - CANVAS_PAD), CANVAS_PAD, size.x - CANVAS_PAD, bounds.left, bounds.right);
	p.y = rescale(clamp(point.y, CANVAS_PAD, size.y - CANVAS_PAD), size.y - CANVAS_PAD, CANVAS_PAD, bounds.bottom, bounds.top);
	return p;
}

inline void clampPointToRoom(const bulkhead::geometry::RoomBounds& room, bulkhead::geometry::Vec2* point) {
	if (!point) {
		return;
	}
	const float margin = 0.1f;
	point->x = clamp(point->x, room.left + margin, room.right - margin);
	point->y = clamp(point->y, room.bottom + margin, room.top - margin);
}

struct BulkheadRoomCanvasWidget : TransparentWidget {
	enum DragTarget {
		DRAG_NONE = 0,
		DRAG_LISTENER = 1,
		DRAG_SPEAKER_LEFT = 2,
		DRAG_SPEAKER_RIGHT = 3,
		DRAG_WALL_LEFT = 4,
		DRAG_WALL_RIGHT = 5,
		DRAG_WALL_TOP = 6,
		DRAG_WALL_BOTTOM = 7
	};

	Bulkhead* module = nullptr;
	DragTarget dragTarget = DRAG_NONE;
	DragTarget hoverTarget = DRAG_NONE;
	Vec lastDragPos;

	explicit BulkheadRoomCanvasWidget(Bulkhead* module) : module(module) {
	}

	DragTarget pickTarget(Vec localPos) const {
		if (!module) {
			return DRAG_NONE;
		}
		const auto world = worldBounds();
		const Vec listenerPx = roomToCanvas(world, module->listener, box.size);
		const Vec leftPx = roomToCanvas(world, module->speakerLeft, box.size);
		const Vec rightPx = roomToCanvas(world, module->speakerRight, box.size);
		const bulkhead::geometry::Vec2 rTL = makeVec2(module->room.left, module->room.top);
		const bulkhead::geometry::Vec2 rTR = makeVec2(module->room.right, module->room.top);
		const bulkhead::geometry::Vec2 rBL = makeVec2(module->room.left, module->room.bottom);
		const Vec roomTL = roomToCanvas(world, rTL, box.size);
		const Vec roomTR = roomToCanvas(world, rTR, box.size);
		const Vec roomBL = roomToCanvas(world, rBL, box.size);
		const float hitRadius = 16.f;
		const float wallHit = 10.f;
		if (listenerPx.minus(localPos).norm() <= hitRadius) {
			return DRAG_LISTENER;
		}
		if (leftPx.minus(localPos).norm() <= hitRadius) {
			return DRAG_SPEAKER_LEFT;
		}
		if (rightPx.minus(localPos).norm() <= hitRadius) {
			return DRAG_SPEAKER_RIGHT;
		}
		if (std::fabs(localPos.x - roomTL.x) <= wallHit && localPos.y >= roomTL.y && localPos.y <= roomBL.y) {
			return DRAG_WALL_LEFT;
		}
		if (std::fabs(localPos.x - roomTR.x) <= wallHit && localPos.y >= roomTL.y && localPos.y <= roomBL.y) {
			return DRAG_WALL_RIGHT;
		}
		if (std::fabs(localPos.y - roomTL.y) <= wallHit && localPos.x >= roomTL.x && localPos.x <= roomTR.x) {
			return DRAG_WALL_TOP;
		}
		if (std::fabs(localPos.y - roomBL.y) <= wallHit && localPos.x >= roomTL.x && localPos.x <= roomTR.x) {
			return DRAG_WALL_BOTTOM;
		}
		return DRAG_NONE;
	}

	void applyDragAt(Vec localPos) {
		if (!module || dragTarget == DRAG_NONE) {
			return;
		}
		const auto world = worldBounds();
		bulkhead::geometry::Vec2 point = canvasToRoom(world, localPos, box.size);
		if (dragTarget == DRAG_WALL_LEFT) {
			module->room.left = clamp(point.x, world.left, module->listener.x - ROOM_MIN_GAP);
			clampPointToRoom(module->room, &module->speakerLeft);
			clampPointToRoom(module->room, &module->speakerRight);
			return;
		}
		if (dragTarget == DRAG_WALL_RIGHT) {
			module->room.right = clamp(point.x, module->listener.x + ROOM_MIN_GAP, world.right);
			clampPointToRoom(module->room, &module->speakerLeft);
			clampPointToRoom(module->room, &module->speakerRight);
			return;
		}
		if (dragTarget == DRAG_WALL_TOP) {
			module->room.top = clamp(point.y, module->listener.y + ROOM_MIN_GAP, world.top);
			clampPointToRoom(module->room, &module->speakerLeft);
			clampPointToRoom(module->room, &module->speakerRight);
			return;
		}
		if (dragTarget == DRAG_WALL_BOTTOM) {
			module->room.bottom = clamp(point.y, world.bottom, module->listener.y - ROOM_MIN_GAP);
			clampPointToRoom(module->room, &module->speakerLeft);
			clampPointToRoom(module->room, &module->speakerRight);
			return;
		}
		clampPointToRoom(module->room, &point);
		if (dragTarget == DRAG_LISTENER) {
			module->listener = point;
		} else if (dragTarget == DRAG_SPEAKER_LEFT) {
			module->speakerLeft = point;
		} else if (dragTarget == DRAG_SPEAKER_RIGHT) {
			module->speakerRight = point;
		}
	}

	void onButton(const event::Button& e) override {
		if (e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS) {
			dragTarget = pickTarget(e.pos);
			if (dragTarget != DRAG_NONE) {
				lastDragPos = e.pos;
				applyDragAt(e.pos);
				e.consume(this);
				return;
			}
		}
		if (e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_RELEASE && dragTarget != DRAG_NONE) {
			dragTarget = DRAG_NONE;
			e.consume(this);
			return;
		}
		Widget::onButton(e);
	}

	void onHover(const event::Hover& e) override {
		hoverTarget = pickTarget(e.pos);
		Widget::onHover(e);
	}

	void onLeave(const event::Leave& e) override {
		hoverTarget = DRAG_NONE;
		Widget::onLeave(e);
	}

	void onDragMove(const event::DragMove& e) override {
		if (dragTarget == DRAG_NONE || !module) {
			Widget::onDragMove(e);
			return;
		}
		lastDragPos = lastDragPos.plus(e.mouseDelta);
		applyDragAt(lastDragPos);
		e.consume(this);
	}

	void onDragEnd(const event::DragEnd& e) override {
		dragTarget = DRAG_NONE;
		Widget::onDragEnd(e);
	}

	void draw(const DrawArgs& args) override {
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, 0.f, 0.f, box.size.x, box.size.y, 6.f);
		nvgFillColor(args.vg, nvgRGBA(7, 12, 18, 235));
		nvgFill(args.vg);

		const float cx = box.size.x * 0.5f;
		const float cy = box.size.y * 0.5f;
		const auto world = worldBounds();
		Vec leftSpeakerPx(box.size.x * 0.22f, box.size.y * 0.24f);
		Vec rightSpeakerPx(box.size.x * 0.78f, box.size.y * 0.24f);
		Vec listenerPx(box.size.x * 0.5f, box.size.y * 0.5f);
		Vec roomTL(CANVAS_PAD, CANVAS_PAD);
		Vec roomTR(box.size.x - CANVAS_PAD, CANVAS_PAD);
		Vec roomBL(CANVAS_PAD, box.size.y - CANVAS_PAD);
		Vec roomBR(box.size.x - CANVAS_PAD, box.size.y - CANVAS_PAD);

		if (module) {
			leftSpeakerPx = roomToCanvas(world, module->speakerLeft, box.size);
			rightSpeakerPx = roomToCanvas(world, module->speakerRight, box.size);
			listenerPx = roomToCanvas(world, module->listener, box.size);
			const bulkhead::geometry::Vec2 rTL = makeVec2(module->room.left, module->room.top);
			const bulkhead::geometry::Vec2 rTR = makeVec2(module->room.right, module->room.top);
			const bulkhead::geometry::Vec2 rBL = makeVec2(module->room.left, module->room.bottom);
			const bulkhead::geometry::Vec2 rBR = makeVec2(module->room.right, module->room.bottom);
			roomTL = roomToCanvas(world, rTL, box.size);
			roomTR = roomToCanvas(world, rTR, box.size);
			roomBL = roomToCanvas(world, rBL, box.size);
			roomBR = roomToCanvas(world, rBR, box.size);
		}

		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, roomTL.x, roomTL.y);
		nvgLineTo(args.vg, roomTR.x, roomTR.y);
		nvgLineTo(args.vg, roomBR.x, roomBR.y);
		nvgLineTo(args.vg, roomBL.x, roomBL.y);
		nvgClosePath(args.vg);
		nvgFillColor(args.vg, nvgRGBA(24, 31, 39, 140));
		nvgFill(args.vg);
		nvgStrokeColor(args.vg, nvgRGBA(112, 226, 236, 220));
		nvgStrokeWidth(args.vg, 1.5f);
		nvgStroke(args.vg);

		const bool hlLeft = (hoverTarget == DRAG_WALL_LEFT || dragTarget == DRAG_WALL_LEFT);
		const bool hlRight = (hoverTarget == DRAG_WALL_RIGHT || dragTarget == DRAG_WALL_RIGHT);
		const bool hlTop = (hoverTarget == DRAG_WALL_TOP || dragTarget == DRAG_WALL_TOP);
		const bool hlBottom = (hoverTarget == DRAG_WALL_BOTTOM || dragTarget == DRAG_WALL_BOTTOM);
		auto drawWallHighlight = [&](const Vec& a, const Vec& b, bool active) {
			if (!active) {
				return;
			}
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, a.x, a.y);
			nvgLineTo(args.vg, b.x, b.y);
			nvgStrokeColor(args.vg, nvgRGBA(175, 245, 250, 235));
			nvgStrokeWidth(args.vg, 4.5f);
			nvgStroke(args.vg);
		};
		drawWallHighlight(roomTL, roomBL, hlLeft);
		drawWallHighlight(roomTR, roomBR, hlRight);
		drawWallHighlight(roomTL, roomTR, hlTop);
		drawWallHighlight(roomBL, roomBR, hlBottom);

		auto drawSpeaker = [&](const Vec& pos, const Vec& toward) {
			const Vec dir = toward.minus(pos).normalize();
			const Vec right(-dir.y, dir.x);
			const Vec tip = pos.plus(dir.mult(11.f));
			const Vec baseA = pos.minus(dir.mult(10.f)).plus(right.mult(7.f));
			const Vec baseB = pos.minus(dir.mult(10.f)).minus(right.mult(7.f));
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, tip.x, tip.y);
			nvgLineTo(args.vg, baseA.x, baseA.y);
			nvgLineTo(args.vg, baseB.x, baseB.y);
			nvgClosePath(args.vg);
			nvgFillColor(args.vg, nvgRGBA(132, 104, 255, 240));
			nvgFill(args.vg);
		};
		drawSpeaker(leftSpeakerPx, listenerPx);
		drawSpeaker(rightSpeakerPx, listenerPx);

		nvgBeginPath(args.vg);
		nvgCircle(args.vg, listenerPx.x, listenerPx.y, 8.f);
		nvgFillColor(args.vg, nvgRGBA(33, 208, 219, 240));
		nvgFill(args.vg);

		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, listenerPx.x, listenerPx.y);
		nvgLineTo(args.vg, listenerPx.x, listenerPx.y - 14.f);
		nvgStrokeColor(args.vg, nvgRGBA(33, 208, 219, 220));
		nvgStrokeWidth(args.vg, 1.5f);
		nvgStroke(args.vg);

		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, cx, 8.f);
		nvgLineTo(args.vg, cx, box.size.y - 8.f);
		nvgMoveTo(args.vg, 8.f, cy);
		nvgLineTo(args.vg, box.size.x - 8.f, cy);
		nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 36));
		nvgStrokeWidth(args.vg, 1.0f);
		nvgStroke(args.vg);
	}
};

bool loadAnchorPointMm(const std::string& panelPath, const char* id, Vec* outMm, const Vec& fallbackMm) {
	if (panel_svg::loadPointFromSvgMm(panelPath, id, outMm)) {
		return true;
	}
	*outMm = fallbackMm;
	return false;
}

} // namespace

BulkheadWidget::BulkheadWidget(Bulkhead* module) {
	setModule(module);
	PreviewBuildLogTimer previewBuildTimer("Bulkhead", module);
	const std::string panelPath = asset::plugin(pluginInstance, "res/bulkhead.svg");
	setPanel(createPanel(panelPath));
	previewBuildTimer.markPanelDone();
	previewBuildTimer.setAtlasStatus(panel_svg::getAtlasStatusLabelForSvg(panelPath));

	math::Rect roomCanvasMm;
	if (!panel_svg::loadRectFromSvgMm(panelPath, "room_canvas", &roomCanvasMm)) {
		roomCanvasMm.pos = Vec(5.5f, 12.5f);
		roomCanvasMm.size = Vec(70.28f, 61.f);
	}
	auto* roomCanvas = new BulkheadRoomCanvasWidget(module);
	roomCanvas->box.pos = mm2px(roomCanvasMm.pos);
	roomCanvas->box.size = mm2px(roomCanvasMm.size);
	addChild(roomCanvas);

	auto addKnob = [&](int paramId, const char* anchorId, Vec fallbackMm) {
		Vec posMm;
		loadAnchorPointMm(panelPath, anchorId, &posMm, fallbackMm);
		addParam(createParamCentered<RoundBlackKnob>(mm2px(posMm), module, paramId));
	};
	auto addInputPort = [&](int inputId, const char* anchorId, Vec fallbackMm) {
		Vec posMm;
		loadAnchorPointMm(panelPath, anchorId, &posMm, fallbackMm);
		addInput(createInputCentered<PJ301MPort>(mm2px(posMm), module, inputId));
	};
	auto addOutputPort = [&](int outputId, const char* anchorId, Vec fallbackMm) {
		Vec posMm;
		loadAnchorPointMm(panelPath, anchorId, &posMm, fallbackMm);
		addOutput(createOutputCentered<PJ301MPort>(mm2px(posMm), module, outputId));
	};

	addKnob(Bulkhead::DECAY_PARAM, "decay_param", Vec(7.f, 81.7f));
	addKnob(Bulkhead::DIFFUSE_PARAM, "diffuse_param", Vec(20.35f, 81.7f));
	addKnob(Bulkhead::MIX_PARAM, "mix_param", Vec(33.7f, 81.7f));
	addKnob(Bulkhead::ABSORB_PARAM, "absorb_param", Vec(47.05f, 81.7f));
	addKnob(Bulkhead::EARLY_LATE_PARAM, "early_late_param", Vec(60.4f, 81.7f));
	addKnob(Bulkhead::MOTION_PARAM, "motion_param", Vec(73.75f, 81.7f));

	addInputPort(Bulkhead::LST_X_INPUT, "lst_x_input", Vec(9.f, 103.5f));
	addInputPort(Bulkhead::LST_Y_INPUT, "lst_y_input", Vec(19.4f, 103.5f));
	addInputPort(Bulkhead::WALL_LEFT_INPUT, "wall_left_input", Vec(29.8f, 103.5f));
	addInputPort(Bulkhead::WALL_RIGHT_INPUT, "wall_right_input", Vec(40.2f, 103.5f));
	addInputPort(Bulkhead::WALL_FRONT_INPUT, "wall_front_input", Vec(50.6f, 103.5f));
	addInputPort(Bulkhead::WALL_BACK_INPUT, "wall_back_input", Vec(61.f, 103.5f));
	addInputPort(Bulkhead::IN_L_INPUT, "in_l_input", Vec(9.f, 113.5f));
	addInputPort(Bulkhead::IN_R_INPUT, "in_r_input", Vec(19.4f, 113.5f));

	addOutputPort(Bulkhead::OUT_L_OUTPUT, "out_l_output", Vec(61.f, 113.5f));
	addOutputPort(Bulkhead::OUT_R_OUTPUT, "out_r_output", Vec(72.28f, 113.5f));

	previewBuildTimer.markAnchorsDone();
}

Model* modelBulkhead = createModel<Bulkhead, BulkheadWidget>("Bulkhead");
