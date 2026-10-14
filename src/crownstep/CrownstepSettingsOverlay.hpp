#pragma once

#include "CrownstepShared.hpp"

#include <functional>

struct CrownstepSettingsOpenButton final : widget::OpaqueWidget {
	bool enabled = true;
	std::function<void()> openAction;

	void draw(const DrawArgs& args) override;
	void onButton(const event::Button& e) override;
};

struct CrownstepSettingsOverlay final : widget::OpaqueWidget {
	enum Tab {
		TAB_GAME,
		TAB_PITCH,
		TAB_MAP,
		TAB_LOOK,
		TAB_COUNT
	};

	Crownstep* module = nullptr;
	Tab activeTab = TAB_GAME;
	int pendingGameMode = Crownstep::GAME_MODE_CHECKERS;
	int pendingPlayerMode = Crownstep::PLAYER_INIT;
	bool confirmationOpen = false;
	bool rangeDragging = false;
	bool drawAboveRackCables = false;
	float rangeDragOldValue = 0.f;
	float rangeDragValue = 0.f;
	std::function<void()> closeAction;

	explicit CrownstepSettingsOverlay(Crownstep* module);

	void open();
	void close();
	void syncPendingGameSetup();
	bool hasPendingGameSetup() const;
	void requestNewGame();
	void confirmNewGame();
	void cancelConfirmation();
	void openExactRangeMenu(const Vec& localPos);

	void draw(const DrawArgs& args) override;
	void drawLayer(const DrawArgs& args, int layer) override;
	void drawSurface(const DrawArgs& args);
	void onButton(const event::Button& e) override;
	void onDragMove(const event::DragMove& e) override;
	void onDragEnd(const event::DragEnd& e) override;
	void onHoverScroll(const event::HoverScroll& e) override;
	void onSelectKey(const event::SelectKey& e) override;
	void onHoverKey(const event::HoverKey& e) override;
};
