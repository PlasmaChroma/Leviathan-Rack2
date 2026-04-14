#include "CrownstepShared.hpp"

json_t* Crownstep::dataToJson() {
	json_t* rootJ = json_object();
	json_object_set_new(rootJ, "turnSide", json_integer(turnSide));
	json_object_set_new(rootJ, "winnerSide", json_integer(winnerSide));
	json_object_set_new(rootJ, "selectedSquare", json_integer(selectedSquare));
	json_object_set_new(rootJ, "aiDifficulty", json_integer(aiDifficulty));
	json_object_set_new(rootJ, "quantizationEnabled", json_boolean(quantizationEnabled));
	json_object_set_new(rootJ, "pitchBipolarEnabled", json_boolean(pitchBipolarEnabled));
	json_object_set_new(rootJ, "melodicBiasEnabled", json_boolean(melodicBiasEnabled));
	json_object_set_new(rootJ, "pitchInterpretationMode", json_integer(pitchInterpretationMode));
	json_object_set_new(rootJ, "boardValueLayoutMode", json_integer(boardValueLayoutMode));
	json_object_set_new(rootJ, "boardValueLayoutInverted", json_boolean(boardValueLayoutInverted));
	json_object_set_new(rootJ, "pitchDividerMode", json_integer(pitchDividerMode));
	json_object_set_new(rootJ, "showCellPitchOverlay", json_boolean(showCellPitchOverlay));
	json_object_set_new(rootJ, "boardTextureMode", json_integer(boardTextureMode));
	json_object_set_new(rootJ, "gameMode", json_integer(gameMode));
	json_object_set_new(rootJ, "highlightMode", json_integer(highlightMode));
	json_object_set_new(rootJ, "playerMode", json_integer(playerMode));
	json_object_set_new(rootJ, "stepCounterStyle", json_integer(stepCounterStyle));
	json_object_set_new(rootJ, "sequenceCapOverride", json_integer(sequenceCapOverride));
	json_object_set_new(rootJ, "chessCastleWK", json_boolean(chessState.whiteCanCastleKingSide));
	json_object_set_new(rootJ, "chessCastleWQ", json_boolean(chessState.whiteCanCastleQueenSide));
	json_object_set_new(rootJ, "chessCastleBK", json_boolean(chessState.blackCanCastleKingSide));
	json_object_set_new(rootJ, "chessCastleBQ", json_boolean(chessState.blackCanCastleQueenSide));
	json_object_set_new(rootJ, "chessEnPassantTarget", json_integer(chessState.enPassantTargetIndex));
	json_object_set_new(rootJ, "playhead", json_integer(playhead));
	json_object_set_new(rootJ, "gameOver", json_boolean(gameOver));
	json_object_set_new(rootJ, "lastMoveSide", json_integer(lastMoveSide));

	json_t* boardJ = json_array();
	for (int piece : board) {
		json_array_append_new(boardJ, json_integer(piece));
	}
	json_object_set_new(rootJ, "board", boardJ);

	json_t* historyJ = json_array();
	{
		std::lock_guard<std::recursive_mutex> lock(sequenceMutex);
		for (const Step& step : history) {
			json_t* stepJ = json_object();
			json_object_set_new(stepJ, "pitch", json_real(step.pitch));
			json_object_set_new(stepJ, "gate", json_boolean(step.gate));
			json_object_set_new(stepJ, "accent", json_real(step.accent));
			json_object_set_new(stepJ, "mod", json_real(step.mod));
			json_array_append_new(historyJ, stepJ);
		}
	}
	json_object_set_new(rootJ, "history", historyJ);

	json_t* moveHistoryJ = json_array();
	{
		std::lock_guard<std::recursive_mutex> lock(sequenceMutex);
		for (const Move& move : moveHistory) {
			json_t* moveJ = json_object();
			json_object_set_new(moveJ, "origin", json_integer(move.originIndex));
			json_object_set_new(moveJ, "destination", json_integer(move.destinationIndex));
			json_object_set_new(moveJ, "isCapture", json_boolean(move.isCapture));
			json_object_set_new(moveJ, "isMultiCapture", json_boolean(move.isMultiCapture));
			json_object_set_new(moveJ, "isKing", json_boolean(move.isKing));

			json_t* pathJ = json_array();
			for (int index : move.path) {
				json_array_append_new(pathJ, json_integer(index));
			}
			json_object_set_new(moveJ, "path", pathJ);

			json_t* capturedJ = json_array();
			for (int index : move.captured) {
				json_array_append_new(capturedJ, json_integer(index));
			}
			json_object_set_new(moveJ, "captured", capturedJ);
			json_array_append_new(moveHistoryJ, moveJ);
		}
	}
	json_object_set_new(rootJ, "moveHistory", moveHistoryJ);
	return rootJ;
}

void Crownstep::dataFromJson(json_t* rootJ) {
	if (!rootJ) {
		return;
	}

	json_t* gameModeJ = json_object_get(rootJ, "gameMode");
	int loadedGameMode = GAME_MODE_CHECKERS;
	if (gameModeJ) {
		loadedGameMode = clamp(int(json_integer_value(gameModeJ)), 0, GAME_MODE_COUNT - 1);
	}
	setGameMode(loadedGameMode, false);
	board = gameRules ? gameRules->makeInitialBoard() : crownstep::makeInitialBoard();
	json_t* playerModeJ = json_object_get(rootJ, "playerMode");
	if (playerModeJ) {
		playerMode = clamp(int(json_integer_value(playerModeJ)), 0, PLAYER_MODE_COUNT - 1);
	}

	json_t* turnJ = json_object_get(rootJ, "turnSide");
	if (turnJ) {
		int side = int(json_integer_value(turnJ));
		turnSide = (side > 0) ? HUMAN_SIDE : ((side < 0) ? AI_SIDE : (gameRules ? gameRules->humanSide() : HUMAN_SIDE));
	}
	json_t* winnerJ = json_object_get(rootJ, "winnerSide");
	if (winnerJ) {
		winnerSide = int(json_integer_value(winnerJ));
	}
	json_t* selectedJ = json_object_get(rootJ, "selectedSquare");
	if (selectedJ) {
		selectedSquare = int(json_integer_value(selectedJ));
	}
	hoveredSquare = -1;
	json_t* difficultyJ = json_object_get(rootJ, "aiDifficulty");
	if (difficultyJ) {
		aiDifficulty = clamp(int(json_integer_value(difficultyJ)), 0, 2);
	}
	json_t* quantizationEnabledJ = json_object_get(rootJ, "quantizationEnabled");
	if (quantizationEnabledJ) {
		quantizationEnabled = json_is_true(quantizationEnabledJ);
	}
	json_t* pitchBipolarEnabledJ = json_object_get(rootJ, "pitchBipolarEnabled");
	if (pitchBipolarEnabledJ) {
		pitchBipolarEnabled = json_is_true(pitchBipolarEnabledJ);
	}
	json_t* melodicBiasEnabledJ = json_object_get(rootJ, "melodicBiasEnabled");
	if (melodicBiasEnabledJ) {
		melodicBiasEnabled = json_is_true(melodicBiasEnabledJ);
	}
	json_t* pitchInterpretationModeJ = json_object_get(rootJ, "pitchInterpretationMode");
	if (pitchInterpretationModeJ) {
		pitchInterpretationMode =
			clamp(int(json_integer_value(pitchInterpretationModeJ)), 0, int(PITCH_INTERPRETATION_NAMES.size()) - 1);
	}
	bool loadedPitchDividerMode = false;
	json_t* pitchDividerModeJ = json_object_get(rootJ, "pitchDividerMode");
	if (pitchDividerModeJ) {
		pitchDividerMode =
			clamp(int(json_integer_value(pitchDividerModeJ)), 0, int(crownstep::PITCH_DIVIDER_NAMES.size()) - 1);
		loadedPitchDividerMode = true;
	}
	json_t* showCellPitchOverlayJ = json_object_get(rootJ, "showCellPitchOverlay");
	if (showCellPitchOverlayJ) {
		showCellPitchOverlay = json_is_true(showCellPitchOverlayJ);
	}
	json_t* boardValueLayoutModeJ = json_object_get(rootJ, "boardValueLayoutMode");
	if (boardValueLayoutModeJ) {
		int storedLayoutMode = int(json_integer_value(boardValueLayoutModeJ));
		// Backward compatibility: legacy /2 interleave layouts (3..5) now
		// map to base layouts (0..2) with divider set to Half.
		if (storedLayoutMode >= int(BOARD_VALUE_LAYOUT_NAMES.size()) &&
			storedLayoutMode < int(BOARD_VALUE_LAYOUT_NAMES.size()) * 2) {
			boardValueLayoutMode = storedLayoutMode - int(BOARD_VALUE_LAYOUT_NAMES.size());
			if (!loadedPitchDividerMode) {
				pitchDividerMode = 1;
			}
		}
		else {
			boardValueLayoutMode = clamp(storedLayoutMode, 0, int(BOARD_VALUE_LAYOUT_NAMES.size()) - 1);
		}
	}
	json_t* boardValueLayoutInvertedJ = json_object_get(rootJ, "boardValueLayoutInverted");
	if (boardValueLayoutInvertedJ) {
		boardValueLayoutInverted = json_is_true(boardValueLayoutInvertedJ);
	}
	json_t* boardTextureModeJ = json_object_get(rootJ, "boardTextureMode");
	if (boardTextureModeJ) {
		boardTextureMode = clamp(int(json_integer_value(boardTextureModeJ)), 0, int(BOARD_TEXTURE_NAMES.size()) - 1);
	}
	json_t* highlightModeJ = json_object_get(rootJ, "highlightMode");
	if (highlightModeJ) {
		highlightMode = clamp(int(json_integer_value(highlightModeJ)), 0, HIGHLIGHT_COUNT - 1);
	}
	json_t* stepCounterStyleJ = json_object_get(rootJ, "stepCounterStyle");
	if (stepCounterStyleJ) {
		stepCounterStyle = STEP_COUNTER_RIBBON;
	}
	json_t* sequenceCapOverrideJ = json_object_get(rootJ, "sequenceCapOverride");
	if (sequenceCapOverrideJ) {
		sequenceCapOverride = int(json_integer_value(sequenceCapOverrideJ));
		if (sequenceCapOverride < -1) {
			sequenceCapOverride = -1;
		}
	}
	json_t* playheadJ = json_object_get(rootJ, "playhead");
	if (playheadJ) {
		playhead = std::max(0, int(json_integer_value(playheadJ)));
	}
	json_t* gameOverJ = json_object_get(rootJ, "gameOver");
	if (gameOverJ) {
		gameOver = json_is_true(gameOverJ);
	}
	json_t* lastMoveSideJ = json_object_get(rootJ, "lastMoveSide");
	if (lastMoveSideJ) {
		int side = int(json_integer_value(lastMoveSideJ));
		lastMoveSide = (side > 0) ? HUMAN_SIDE : ((side < 0) ? AI_SIDE : 0);
	}

	json_t* boardJ = json_object_get(rootJ, "board");
	if (boardJ && json_is_array(boardJ)) {
		int cellCount = boardCellCount();
		for (int i = 0; i < cellCount; ++i) {
			json_t* pieceJ = json_array_get(boardJ, i);
			if (pieceJ) {
				board[size_t(i)] = int(json_integer_value(pieceJ));
			}
		}
	}
	if (isChessMode()) {
		json_t* castleWkJ = json_object_get(rootJ, "chessCastleWK");
		json_t* castleWqJ = json_object_get(rootJ, "chessCastleWQ");
		json_t* castleBkJ = json_object_get(rootJ, "chessCastleBK");
		json_t* castleBqJ = json_object_get(rootJ, "chessCastleBQ");
		json_t* enPassantTargetJ = json_object_get(rootJ, "chessEnPassantTarget");
		bool hasSerializedChessState = castleWkJ || castleWqJ || castleBkJ || castleBqJ || enPassantTargetJ;
		if (hasSerializedChessState) {
			chessState.whiteCanCastleKingSide = castleWkJ ? json_is_true(castleWkJ) : false;
			chessState.whiteCanCastleQueenSide = castleWqJ ? json_is_true(castleWqJ) : false;
			chessState.blackCanCastleKingSide = castleBkJ ? json_is_true(castleBkJ) : false;
			chessState.blackCanCastleQueenSide = castleBqJ ? json_is_true(castleBqJ) : false;
			chessState.enPassantTargetIndex = enPassantTargetJ ? int(json_integer_value(enPassantTargetJ)) : -1;
			if (chessState.enPassantTargetIndex < 0 || chessState.enPassantTargetIndex >= crownstep::CHESS_BOARD_SIZE) {
				chessState.enPassantTargetIndex = -1;
			}
		}
		else {
			chessState = crownstep::chessInferStateFromBoard(board);
		}
	}
	else {
		chessState = crownstep::chessInitialState();
	}

	{
		std::lock_guard<std::recursive_mutex> lock(sequenceMutex);
		history.clear();
		json_t* historyJ = json_object_get(rootJ, "history");
		if (historyJ && json_is_array(historyJ)) {
			size_t index = 0;
			json_t* stepJ = nullptr;
			json_array_foreach(historyJ, index, stepJ) {
				Step step;
				step.pitch = float(json_number_value(json_object_get(stepJ, "pitch")));
				step.gate = json_is_true(json_object_get(stepJ, "gate"));
				step.accent = float(json_number_value(json_object_get(stepJ, "accent")));
				step.mod = float(json_number_value(json_object_get(stepJ, "mod")));
				history.push_back(step);
			}
		}

		moveHistory.clear();
		json_t* moveHistoryJ = json_object_get(rootJ, "moveHistory");
		if (moveHistoryJ && json_is_array(moveHistoryJ)) {
			size_t moveIndex = 0;
			json_t* moveJ = nullptr;
			json_array_foreach(moveHistoryJ, moveIndex, moveJ) {
				Move move;
				move.originIndex = int(json_integer_value(json_object_get(moveJ, "origin")));
				move.destinationIndex = int(json_integer_value(json_object_get(moveJ, "destination")));
				move.isCapture = json_is_true(json_object_get(moveJ, "isCapture"));
				move.isMultiCapture = json_is_true(json_object_get(moveJ, "isMultiCapture"));
				move.isKing = json_is_true(json_object_get(moveJ, "isKing"));

				json_t* pathJ = json_object_get(moveJ, "path");
				if (pathJ && json_is_array(pathJ)) {
					size_t pathIndex = 0;
					json_t* pathEntryJ = nullptr;
					json_array_foreach(pathJ, pathIndex, pathEntryJ) {
						move.path.push_back(int(json_integer_value(pathEntryJ)));
					}
				}
				json_t* capturedJ = json_object_get(moveJ, "captured");
				if (capturedJ && json_is_array(capturedJ)) {
					size_t captureIndex = 0;
					json_t* captureEntryJ = nullptr;
					json_array_foreach(capturedJ, captureIndex, captureEntryJ) {
						move.captured.push_back(int(json_integer_value(captureEntryJ)));
					}
				}
				moveHistory.push_back(move);
			}
		}

		lastMove = moveHistory.empty() ? Move() : moveHistory.back();
		if (!lastMoveSideJ) {
			lastMoveSide = moveHistory.empty() ? 0 : opposingSide(turnSide);
		}
	}
		captureFlashSeconds = 0.f;
		resetMoveAnimation();
		cachedRootSemitoneValid = false;
		refreshLegalMoves();
		advanceForcedPassesIfNeeded();
	}
