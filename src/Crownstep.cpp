#include "plugin.hpp"
#include "CrownstepCore.hpp"

#include <cmath>
#include <exception>
#include <fstream>
#include <regex>
#include <sstream>
#include <string>

using crownstep::AI_SIDE;
using crownstep::BOARD_SIZE;
using crownstep::DIFFICULTY_NAMES;
using crownstep::HUMAN_SIDE;
using crownstep::KEY_NAMES;
using crownstep::Move;
using crownstep::SCALES;
using crownstep::SEQ_CAP_NAMES;
using crownstep::SEQ_CAPS;
using crownstep::Step;

static bool loadRectFromSvgMm(const std::string& svgPath, const std::string& rectId, math::Rect* outRect) {
	if (!outRect) {
		return false;
	}

	std::ifstream svgFile(svgPath);
	if (!svgFile.good()) {
		return false;
	}

	std::ostringstream svgBuffer;
	svgBuffer << svgFile.rdbuf();
	const std::string svgText = svgBuffer.str();

	const std::regex rectRegex("<rect\\b[^>]*\\bid\\s*=\\s*\"" + rectId + "\"[^>]*>", std::regex::icase);
	std::smatch rectMatch;
	if (!std::regex_search(svgText, rectMatch, rectRegex)) {
		return false;
	}
	const std::string rectTag = rectMatch.str(0);

	auto parseAttrMm = [&](const char* attr, float* outMm) -> bool {
		if (!outMm) {
			return false;
		}
		const std::regex attrRegex(std::string("\\b") + attr + "\\s*=\\s*\"([^\"]+)\"", std::regex::icase);
		std::smatch attrMatch;
		if (!std::regex_search(rectTag, attrMatch, attrRegex)) {
			return false;
		}
		// Inkscape panel coordinates are in 1/100 mm (viewBox-style coordinates).
		*outMm = std::stof(attrMatch.str(1)) * 0.01f;
		return true;
	};

	float xMm = 0.f;
	float yMm = 0.f;
	float wMm = 0.f;
	float hMm = 0.f;
	if (!parseAttrMm("x", &xMm) || !parseAttrMm("y", &yMm) || !parseAttrMm("width", &wMm) || !parseAttrMm("height", &hMm)) {
		return false;
	}

	outRect->pos = Vec(xMm, yMm);
	outRect->size = Vec(wMm, hMm);
	return true;
}

struct CrownstepSeqLengthQuantity final : ParamQuantity {
	std::string getDisplayValueString() override {
		int index = clamp(int(std::round(getValue())), 0, int(SEQ_CAP_NAMES.size()) - 1);
		return SEQ_CAP_NAMES[size_t(index)];
	}
};

struct CrownstepScaleQuantity final : ParamQuantity {
	std::string getDisplayValueString() override {
		int index = clamp(int(std::round(getValue())), 0, int(SCALES.size()) - 1);
		return SCALES[size_t(index)].name;
	}
};

struct CrownstepRootQuantity final : ParamQuantity {
	std::string getDisplayValueString() override {
		int index = clamp(int(std::round(getValue())), 0, 11);
		return KEY_NAMES[size_t(index)];
	}
};

struct Crownstep : Module {
	enum ParamId {
		SEQ_LENGTH_PARAM,
		ROOT_PARAM,
		SCALE_PARAM,
		RUN_PARAM,
		NEW_GAME_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		CLOCK_INPUT,
		RESET_INPUT,
		TRANSPOSE_INPUT,
		ROOT_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		PITCH_OUTPUT,
		ACCENT_OUTPUT,
		MOD_OUTPUT,
		EOC_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		RUN_LIGHT,
		HUMAN_TURN_LIGHT,
		AI_TURN_LIGHT,
		LIGHTS_LEN
	};

	std::array<int, BOARD_SIZE> board = crownstep::makeInitialBoard();
	std::vector<Step> history;
	std::vector<Move> moveHistory;
	std::vector<Move> humanMoves;
	std::vector<Move> aiMoves;
	std::vector<int> highlightedDestinations;
	std::vector<int> opponentHighlightedDestinations;
	Move lastMove;
	struct MoveVisualAnimation {
		std::vector<int> path;
		std::vector<int> capturedIndices;
		std::vector<int> capturedPieces;
		int movingPiece = 0;
		int destinationIndex = -1;
		float elapsedSeconds = 0.f;
		float durationSeconds = 0.f;
		bool active = false;
	};
	MoveVisualAnimation moveAnimation;
	std::vector<MoveVisualAnimation> moveAnimationQueue;

	dsp::SchmittTrigger clockTrigger;
	dsp::SchmittTrigger resetTrigger;
	dsp::SchmittTrigger newGameTrigger;
	dsp::PulseGenerator eocPulse;

	int selectedSquare = -1;
	int hoveredSquare = -1;
	int turnSide = HUMAN_SIDE;
	int winnerSide = 0;
	int lastMoveSide = 0;
	int aiDifficulty = 1;
	int playhead = 0;
	float transportTimeSeconds = 0.f;
	float lastClockEdgeSeconds = -1.f;
	float previousClockPeriodSeconds = -1.f;
	float heldPitch = 0.f;
	float heldAccent = 0.f;
	float heldMod = 0.f;
	float modOutputVolts = 0.f;
	float modGlideStartVolts = 0.f;
	float modGlideTargetVolts = 0.f;
	float modGlideStartSeconds = 0.f;
	float modGlideDurationSeconds = 0.f;
	float captureFlashSeconds = 0.f;
	bool modGlideActive = false;
	bool gameOver = false;

	Crownstep() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		configParam<CrownstepSeqLengthQuantity>(SEQ_LENGTH_PARAM, 0.f, 4.f, 0.f, "Sequence length");
		configParam<CrownstepRootQuantity>(ROOT_PARAM, 0.f, 11.f, 0.f, "Root");
		configParam<CrownstepScaleQuantity>(SCALE_PARAM, 0.f, float(SCALES.size() - 1), 0.f, "Scale");
		configParam(RUN_PARAM, 0.f, 1.f, 1.f, "Run");
		configParam(NEW_GAME_PARAM, 0.f, 1.f, 0.f, "New game");

		paramQuantities[SEQ_LENGTH_PARAM]->snapEnabled = true;
		paramQuantities[ROOT_PARAM]->snapEnabled = true;
		paramQuantities[SCALE_PARAM]->snapEnabled = true;

		configInput(CLOCK_INPUT, "Clock");
		configInput(RESET_INPUT, "Reset");
		configInput(TRANSPOSE_INPUT, "Transpose");
		configInput(ROOT_INPUT, "Root");

		configOutput(PITCH_OUTPUT, "Pitch");
		configOutput(ACCENT_OUTPUT, "Accent");
		configOutput(MOD_OUTPUT, "Mod");
		configOutput(EOC_OUTPUT, "End of cycle");

		refreshLegalMoves();
	}

	void resetMoveAnimation() {
		moveAnimation.path.clear();
		moveAnimation.capturedIndices.clear();
		moveAnimation.capturedPieces.clear();
		moveAnimation.movingPiece = 0;
		moveAnimation.destinationIndex = -1;
		moveAnimation.elapsedSeconds = 0.f;
		moveAnimation.durationSeconds = 0.f;
		moveAnimation.active = false;
		moveAnimationQueue.clear();
	}

	void beginMoveAnimation(const Move& move, const std::array<int, BOARD_SIZE>& beforeBoard) {
		MoveVisualAnimation nextAnimation;
		if (move.originIndex < 0 || move.originIndex >= BOARD_SIZE || move.destinationIndex < 0 || move.destinationIndex >= BOARD_SIZE) {
			return;
		}

		int movingPiece = beforeBoard[size_t(move.originIndex)];
		if (movingPiece == 0) {
			return;
		}

		nextAnimation.movingPiece = movingPiece;
		nextAnimation.destinationIndex = move.destinationIndex;
		nextAnimation.path.push_back(move.originIndex);
		if (move.path.empty()) {
			nextAnimation.path.push_back(move.destinationIndex);
		}
		else {
			for (int pathIndex : move.path) {
				nextAnimation.path.push_back(pathIndex);
			}
			if (nextAnimation.path.back() != move.destinationIndex) {
				nextAnimation.path.push_back(move.destinationIndex);
			}
		}

		for (int captureIndex : move.captured) {
			nextAnimation.capturedIndices.push_back(captureIndex);
			if (captureIndex >= 0 && captureIndex < BOARD_SIZE) {
				nextAnimation.capturedPieces.push_back(beforeBoard[size_t(captureIndex)]);
			}
			else {
				nextAnimation.capturedPieces.push_back(0);
			}
		}

		if (nextAnimation.path.size() < 2) {
			return;
		}

		float segments = float(nextAnimation.path.size() - 1);
		nextAnimation.durationSeconds = clamp(0.085f * segments, 0.12f, 0.34f);
		nextAnimation.elapsedSeconds = 0.f;
		nextAnimation.active = true;

		if (!moveAnimation.active) {
			moveAnimation = nextAnimation;
		}
		else {
			moveAnimationQueue.push_back(nextAnimation);
		}
	}

	void onReset() override {
		resetPlayback();
	}

	void resetPlayback() {
		playhead = 0;
		lastClockEdgeSeconds = -1.f;
		previousClockPeriodSeconds = -1.f;
		heldPitch = 0.f;
		heldAccent = 0.f;
		heldMod = 0.f;
		modOutputVolts = 0.f;
		modGlideStartVolts = 0.f;
		modGlideTargetVolts = 0.f;
		modGlideStartSeconds = transportTimeSeconds;
		modGlideDurationSeconds = 0.f;
		modGlideActive = false;
		resetMoveAnimation();
	}

	int currentSequenceCap() {
		int index = clamp(int(std::round(params[SEQ_LENGTH_PARAM].getValue())), 0, int(SEQ_CAPS.size()) - 1);
		return SEQ_CAPS[size_t(index)];
	}

	int currentScaleIndex() {
		return clamp(int(std::round(params[SCALE_PARAM].getValue())), 0, int(SCALES.size()) - 1);
	}

	float transposeVolts() {
		return clamp(inputs[TRANSPOSE_INPUT].getVoltage(), -10.f, 10.f);
	}

	int rootSemitone() {
		int knobSemitone = clamp(int(std::round(params[ROOT_PARAM].getValue())), 0, 11);
		float rootCv = clamp(inputs[ROOT_INPUT].getVoltage(), -10.f, 10.f);
		int total = knobSemitone + int(std::round(rootCv * 12.f));
		return crownstep::wrapSemitone12(total);
	}

	Step makeStepFromMove(const Move& move) {
		return crownstep::makeStepFromMove(move, currentScaleIndex(), rootSemitone(), transposeVolts());
	}

	void startNewGame() {
		board = crownstep::makeInitialBoard();
		history.clear();
		moveHistory.clear();
		highlightedDestinations.clear();
		opponentHighlightedDestinations.clear();
		selectedSquare = -1;
		hoveredSquare = -1;
		lastMove = Move();
		turnSide = HUMAN_SIDE;
		winnerSide = 0;
		lastMoveSide = 0;
		gameOver = false;
		captureFlashSeconds = 0.f;
		resetPlayback();
		resetMoveAnimation();
		refreshLegalMoves();
	}

	void setHoveredSquare(int index) {
		int normalizedIndex = (index >= 0 && index < BOARD_SIZE) ? index : -1;
		if (hoveredSquare == normalizedIndex) {
			return;
		}
		hoveredSquare = normalizedIndex;

		// Hover only influences UI move-hint previewing while the user is
		// selecting a human move and the game is still active.
		if (!gameOver && turnSide == HUMAN_SIDE && selectedSquare >= 0) {
			refreshLegalMoves();
		}
	}

	void refreshLegalMoves() {
		humanMoves = crownstep::generateLegalMovesForSide(board, HUMAN_SIDE);
		aiMoves = crownstep::generateLegalMovesForSide(board, AI_SIDE);
		highlightedDestinations.clear();
		opponentHighlightedDestinations.clear();
		if (selectedSquare >= 0) {
				for (const Move& move : humanMoves) {
				if (move.originIndex == selectedSquare) {
					highlightedDestinations.push_back(move.destinationIndex);
				}
			}
				if (highlightedDestinations.empty()) {
					selectedSquare = -1;
				}
			}
		bool showOpponentTips = (selectedSquare >= 0) || (turnSide == AI_SIDE);
		const std::vector<Move>* opponentMoveSource = &aiMoves;
		std::vector<Move> previewAiMoves;
		if (selectedSquare >= 0 && hoveredSquare >= 0) {
			for (const Move& move : humanMoves) {
				if (move.originIndex == selectedSquare && move.destinationIndex == hoveredSquare) {
					std::array<int, BOARD_SIZE> previewBoard = crownstep::applyMoveToBoard(board, move);
					previewAiMoves = crownstep::generateLegalMovesForSide(previewBoard, AI_SIDE);
					opponentMoveSource = &previewAiMoves;
					break;
				}
			}
		}
		if (showOpponentTips) {
			std::array<bool, BOARD_SIZE> seen {};
			seen.fill(false);
			for (const Move& move : *opponentMoveSource) {
				int destination = move.destinationIndex;
				if (destination < 0 || destination >= BOARD_SIZE || seen[size_t(destination)]) {
					continue;
				}
				seen[size_t(destination)] = true;
				opponentHighlightedDestinations.push_back(destination);
			}
		}
		const std::vector<Move>& activeMoves = (turnSide == HUMAN_SIDE) ? humanMoves : aiMoves;
		gameOver = activeMoves.empty();
		winnerSide = gameOver ? -turnSide : 0;
	}

	int searchDepthForDifficulty() const {
		return crownstep::searchDepthForDifficulty(aiDifficulty);
	}

	Move chooseAiMove() const {
		return crownstep::chooseAiMove(board, aiDifficulty);
	}

	float expressiveModForMove(
		const Move& move,
		const std::array<int, BOARD_SIZE>& beforeBoard,
		const std::array<int, BOARD_SIZE>& afterBoard,
		int moverSide
	) const {
		auto captureCount = [](const std::vector<Move>& moves) {
			int captures = 0;
			for (const Move& candidate : moves) {
				if (candidate.isCapture) {
					captures++;
				}
			}
			return captures;
		};

		auto pieceCounts = [](const std::array<int, BOARD_SIZE>& sourceBoard, int* men, int* kings) {
			int localMen = 0;
			int localKings = 0;
			for (int piece : sourceBoard) {
				if (piece == 1 || piece == -1) {
					localMen++;
				}
				else if (piece == 2 || piece == -2) {
					localKings++;
				}
			}
			if (men) {
				*men = localMen;
			}
			if (kings) {
				*kings = localKings;
			}
		};

		float moveEnergy = crownstep::normalizedMoveMod(move);

		int beforeEval = crownstep::evaluatePosition(beforeBoard);
		int afterEval = crownstep::evaluatePosition(afterBoard);
		float moverBefore = (moverSide == AI_SIDE) ? float(beforeEval) : -float(beforeEval);
		float moverAfter = (moverSide == AI_SIDE) ? float(afterEval) : -float(afterEval);
		float evalSwingNorm = clamp(std::fabs(moverAfter - moverBefore) / 260.f, 0.f, 1.f);

		std::vector<Move> afterHumanMoves = crownstep::generateLegalMovesForSide(afterBoard, HUMAN_SIDE);
		std::vector<Move> afterAiMoves = crownstep::generateLegalMovesForSide(afterBoard, AI_SIDE);
		int pressureCaptures = captureCount(afterHumanMoves) + captureCount(afterAiMoves);
		float pressureNorm = clamp(float(pressureCaptures) / 5.f, 0.f, 1.f);

		float mobilityDeltaNorm =
			clamp(std::fabs(float(int(afterHumanMoves.size()) - int(afterAiMoves.size()))) / 12.f, 0.f, 1.f);

		int men = 0;
		int kings = 0;
		pieceCounts(afterBoard, &men, &kings);
		// Lower piece count generally means a more exposed/endgame board state.
		float phaseNorm = clamp((24.f - (float(men) + float(kings))) / 24.f, 0.f, 1.f);

		float materialNorm = clamp(std::fabs(float(crownstep::evaluateBoardMaterial(afterBoard))) / 900.f, 0.f, 1.f);

		float boardContext =
			0.34f * evalSwingNorm + 0.24f * pressureNorm + 0.18f * mobilityDeltaNorm + 0.14f * phaseNorm + 0.10f * materialNorm;

		float combined = 0.45f * moveEnergy + 0.55f * boardContext;
		return clamp(combined, 0.f, 1.f);
	}

	void commitMove(const Move& move, int moverSide) {
		if (move.originIndex < 0 || move.destinationIndex < 0) {
			return;
		}
		const std::array<int, BOARD_SIZE> beforeBoard = board;
		const std::array<int, BOARD_SIZE> afterBoard = crownstep::applyMoveToBoard(beforeBoard, move);
		beginMoveAnimation(move, beforeBoard);
		board = afterBoard;
		lastMove = move;
		lastMoveSide = moverSide;
		moveHistory.push_back(move);
		Step step = makeStepFromMove(move);
		step.mod = expressiveModForMove(move, beforeBoard, afterBoard, moverSide);
		history.push_back(step);
		selectedSquare = -1;
		highlightedDestinations.clear();
		turnSide = -moverSide;
		captureFlashSeconds = move.isCapture ? 0.16f : 0.f;
		refreshLegalMoves();
	}

	void maybeRunAiTurn() {
		if (turnSide != AI_SIDE || gameOver) {
			return;
		}
		Move move = chooseAiMove();
		if (move.originIndex < 0) {
			return;
		}
		commitMove(move, AI_SIDE);
	}

	void onBoardSquarePressed(int index) {
		if (turnSide != HUMAN_SIDE || gameOver) {
			return;
		}
		if (index < 0 || index >= BOARD_SIZE) {
			return;
		}

		int piece = board[size_t(index)];
		if (crownstep::pieceSide(piece) == HUMAN_SIDE) {
			selectedSquare = index;
			refreshLegalMoves();
			return;
		}

		if (selectedSquare < 0) {
			return;
		}

		for (const Move& move : humanMoves) {
			if (move.originIndex == selectedSquare && move.destinationIndex == index) {
				commitMove(move, HUMAN_SIDE);
				maybeRunAiTurn();
				return;
			}
		}
	}

	int activeLength() {
		return crownstep::activeLength(int(history.size()), currentSequenceCap());
	}

	int activeStartIndex() {
		return crownstep::activeStartIndex(int(history.size()), currentSequenceCap());
	}

	float pitchForSequenceIndex(int sequenceIndex) {
		if (sequenceIndex < 0) {
			return 0.f;
		}

		// Preferred path: derive pitch from the underlying move sequence so
		// quantizer settings are applied live at playback time.
		if (sequenceIndex < int(moveHistory.size())) {
			const Move& move = moveHistory[size_t(sequenceIndex)];
			return crownstep::mapPitch(move, currentScaleIndex(), rootSemitone(), transposeVolts());
		}

		// Backward compatibility for older saves that may not contain moveHistory.
		if (sequenceIndex < int(history.size())) {
			return history[size_t(sequenceIndex)].pitch;
		}

		return 0.f;
	}

	void emitStepAtClockEdge() {
		int length = activeLength();
		if (length <= 0) {
			heldPitch = 0.f;
			heldAccent = 0.f;
			heldMod = 0.f;
			modOutputVolts = 0.f;
			modGlideStartVolts = 0.f;
			modGlideTargetVolts = 0.f;
			modGlideDurationSeconds = 0.f;
			modGlideActive = false;
			playhead = 0;
			return;
		}

		playhead = clamp(playhead, 0, std::max(length - 1, 0));
		int sequenceIndex = activeStartIndex() + playhead;
		const Step& step = history[size_t(sequenceIndex)];
		heldPitch = pitchForSequenceIndex(sequenceIndex);
		heldAccent = step.accent;
		heldMod = step.mod * 10.f;
		if (previousClockPeriodSeconds > 0.f) {
			modGlideStartVolts = modOutputVolts;
			modGlideTargetVolts = heldMod;
			modGlideStartSeconds = transportTimeSeconds;
			modGlideDurationSeconds = previousClockPeriodSeconds;
			modGlideActive = std::fabs(modGlideTargetVolts - modGlideStartVolts) > 1e-6f;
			if (!modGlideActive) {
				modOutputVolts = modGlideTargetVolts;
			}
		}
		else {
			// Before we have a valid clock period, snap directly.
			modGlideStartVolts = heldMod;
			modGlideTargetVolts = heldMod;
			modGlideStartSeconds = transportTimeSeconds;
			modGlideDurationSeconds = 0.f;
			modGlideActive = false;
			modOutputVolts = heldMod;
		}

		playhead++;
		if (playhead >= length) {
			playhead = 0;
			eocPulse.trigger(1e-3f);
		}
	}

	void process(const ProcessArgs& args) override {
		transportTimeSeconds += args.sampleTime;
		captureFlashSeconds = std::max(0.f, captureFlashSeconds - args.sampleTime);
		if (moveAnimation.active) {
			moveAnimation.elapsedSeconds += args.sampleTime;
			if (moveAnimation.elapsedSeconds >= moveAnimation.durationSeconds) {
				moveAnimation.active = false;
				if (!moveAnimationQueue.empty()) {
					moveAnimation = moveAnimationQueue.front();
					moveAnimationQueue.erase(moveAnimationQueue.begin());
				}
			}
		}
		else if (!moveAnimationQueue.empty()) {
			moveAnimation = moveAnimationQueue.front();
			moveAnimationQueue.erase(moveAnimationQueue.begin());
		}

		if (newGameTrigger.process(params[NEW_GAME_PARAM].getValue())) {
			startNewGame();
		}

		if (resetTrigger.process(inputs[RESET_INPUT].getVoltage())) {
			playhead = 0;
		}

		bool running = true;
		if (clockTrigger.process(inputs[CLOCK_INPUT].getVoltage())) {
			if (lastClockEdgeSeconds >= 0.f) {
				previousClockPeriodSeconds = std::max(transportTimeSeconds - lastClockEdgeSeconds, 1e-6f);
			}
			lastClockEdgeSeconds = transportTimeSeconds;
			if (running) {
				emitStepAtClockEdge();
			}
		}

		if (modGlideActive && modGlideDurationSeconds > 0.f) {
			float t = clamp((transportTimeSeconds - modGlideStartSeconds) / modGlideDurationSeconds, 0.f, 1.f);
			modOutputVolts = modGlideStartVolts + (modGlideTargetVolts - modGlideStartVolts) * t;
			if (t >= 1.f) {
				modGlideActive = false;
			}
		}
		outputs[PITCH_OUTPUT].setVoltage(heldPitch);
		outputs[ACCENT_OUTPUT].setVoltage(heldAccent);
		outputs[MOD_OUTPUT].setVoltage(modOutputVolts);
		outputs[EOC_OUTPUT].setVoltage(eocPulse.process(args.sampleTime) ? 10.f : 0.f);

		lights[RUN_LIGHT].setBrightness(0.f);
		lights[HUMAN_TURN_LIGHT].setBrightness(!gameOver && turnSide == HUMAN_SIDE ? 1.f : 0.f);
		lights[AI_TURN_LIGHT].setBrightness(!gameOver && turnSide == AI_SIDE ? 1.f : 0.f);
	}

	json_t* dataToJson() override {
		json_t* rootJ = json_object();
		json_object_set_new(rootJ, "turnSide", json_integer(turnSide));
		json_object_set_new(rootJ, "winnerSide", json_integer(winnerSide));
		json_object_set_new(rootJ, "selectedSquare", json_integer(selectedSquare));
		json_object_set_new(rootJ, "aiDifficulty", json_integer(aiDifficulty));
		json_object_set_new(rootJ, "playhead", json_integer(playhead));
		json_object_set_new(rootJ, "gameOver", json_boolean(gameOver));
		json_object_set_new(rootJ, "lastMoveSide", json_integer(lastMoveSide));

		json_t* boardJ = json_array();
		for (int piece : board) {
			json_array_append_new(boardJ, json_integer(piece));
		}
		json_object_set_new(rootJ, "board", boardJ);

		json_t* historyJ = json_array();
		for (const Step& step : history) {
			json_t* stepJ = json_object();
			json_object_set_new(stepJ, "pitch", json_real(step.pitch));
			json_object_set_new(stepJ, "gate", json_boolean(step.gate));
			json_object_set_new(stepJ, "accent", json_real(step.accent));
			json_object_set_new(stepJ, "mod", json_real(step.mod));
			json_array_append_new(historyJ, stepJ);
		}
		json_object_set_new(rootJ, "history", historyJ);

		json_t* moveHistoryJ = json_array();
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
		json_object_set_new(rootJ, "moveHistory", moveHistoryJ);
		return rootJ;
	}

	void dataFromJson(json_t* rootJ) override {
		if (!rootJ) {
			return;
		}

		json_t* turnJ = json_object_get(rootJ, "turnSide");
		if (turnJ) {
			turnSide = json_integer_value(turnJ) >= 0 ? HUMAN_SIDE : AI_SIDE;
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
			for (int i = 0; i < BOARD_SIZE; ++i) {
				json_t* pieceJ = json_array_get(boardJ, i);
				if (pieceJ) {
					board[size_t(i)] = int(json_integer_value(pieceJ));
				}
			}
		}

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
			lastMoveSide = moveHistory.empty() ? 0 : -turnSide;
		}
		captureFlashSeconds = 0.f;
		resetMoveAnimation();
		refreshLegalMoves();
	}
};

struct CrownstepBoardWidget final : Widget {
	Crownstep* module = nullptr;

	explicit CrownstepBoardWidget(Crownstep* crownstepModule) {
		module = crownstepModule;
	}

	int indexFromLocalPos(Vec pos) const {
		if (!box.size.x || !box.size.y) {
			return -1;
		}
		if (pos.x < 0.f || pos.y < 0.f || pos.x >= box.size.x || pos.y >= box.size.y) {
			return -1;
		}
		float cellWidth = box.size.x / 8.f;
		float cellHeight = box.size.y / 8.f;
		int col = clamp(int(pos.x / cellWidth), 0, 7);
		int row = clamp(int(pos.y / cellHeight), 0, 7);
		return crownstep::coordToIndex(row, col);
	}

	void onButton(const event::Button& e) override {
		if (!module || e.button != GLFW_MOUSE_BUTTON_LEFT || e.action != GLFW_PRESS) {
			Widget::onButton(e);
			return;
		}
		int index = indexFromLocalPos(e.pos);
		if (index >= 0) {
			module->onBoardSquarePressed(index);
			e.consume(this);
			return;
		}
		Widget::onButton(e);
	}

	void onHover(const event::Hover& e) override {
		if (module) {
			module->setHoveredSquare(indexFromLocalPos(e.pos));
		}
		Widget::onHover(e);
	}

	void onLeave(const event::Leave& e) override {
		if (module) {
			module->setHoveredSquare(-1);
		}
		Widget::onLeave(e);
	}

	void draw(const DrawArgs& args) override {
		nvgSave(args.vg);

		float cellWidth = box.size.x / 8.f;
		float cellHeight = box.size.y / 8.f;
		for (int row = 0; row < 8; ++row) {
			for (int col = 0; col < 8; ++col) {
				bool dark = ((row + col) & 1) == 1;
				float x = col * cellWidth;
				float y = row * cellHeight;

				NVGcolor topColor = dark ? nvgRGB(112, 78, 50) : nvgRGB(224, 198, 160);
				NVGcolor bottomColor = dark ? nvgRGB(72, 46, 30) : nvgRGB(186, 147, 102);

				nvgBeginPath(args.vg);
				nvgRect(args.vg, x, y, cellWidth, cellHeight);
				NVGpaint basePaint = nvgLinearGradient(args.vg, x, y, x, y + cellHeight, topColor, bottomColor);
				nvgFillPaint(args.vg, basePaint);
				nvgFill(args.vg);

				NVGcolor sheenLeft = dark ? nvgRGBA(255, 216, 156, 20) : nvgRGBA(255, 236, 198, 34);
				NVGcolor sheenRight = dark ? nvgRGBA(40, 20, 8, 22) : nvgRGBA(76, 48, 24, 24);
				nvgBeginPath(args.vg);
				nvgRect(args.vg, x, y, cellWidth, cellHeight);
				NVGpaint sheenPaint = nvgLinearGradient(args.vg, x, y, x + cellWidth, y, sheenLeft, sheenRight);
				nvgFillPaint(args.vg, sheenPaint);
				nvgFill(args.vg);

				float seed = float((row * 29 + col * 17) % 97) * 0.17f;
				for (int grain = 0; grain < 4; ++grain) {
					float t = (grain + 1) / 5.f;
					float grainY = y + t * cellHeight + std::sin(seed + t * 6.28318f) * 0.65f;
					float thickness = 0.5f + 0.15f * float(grain & 1);
					int alpha = dark ? 28 : 24;

					nvgBeginPath(args.vg);
					nvgRect(args.vg, x + 0.65f, grainY, cellWidth - 1.3f, thickness);
					nvgFillColor(args.vg, nvgRGBA(255, 224, 170, alpha));
					nvgFill(args.vg);
				}

				if (((row * 8 + col + 3) % 11) == 0) {
					float knotX = x + cellWidth * (0.28f + 0.42f * std::fabs(std::sin(seed)));
					float knotY = y + cellHeight * (0.30f + 0.32f * std::fabs(std::sin(seed * 1.7f)));
					float knotRadius = std::min(cellWidth, cellHeight) * 0.13f;
					NVGcolor knotInner = dark ? nvgRGBA(48, 27, 14, 55) : nvgRGBA(136, 89, 50, 46);
					NVGcolor knotOuter = nvgRGBA(0, 0, 0, 0);

					nvgBeginPath(args.vg);
					nvgRect(args.vg, x, y, cellWidth, cellHeight);
					NVGpaint knotPaint =
						nvgRadialGradient(args.vg, knotX, knotY, knotRadius * 0.2f, knotRadius, knotInner, knotOuter);
					nvgFillPaint(args.vg, knotPaint);
					nvgFill(args.vg);
				}
			}
		}

			if (module) {
				if (!module->gameOver && module->selectedSquare >= 0) {
					int row = 0;
					int col = 0;
					if (crownstep::indexToCoord(module->selectedSquare, &row, &col)) {
						float pulse = 0.5f + 0.5f * std::sin(module->transportTimeSeconds * 4.6f + 0.8f);
						nvgBeginPath(args.vg);
						nvgRect(args.vg, col * cellWidth - 1.f, row * cellHeight - 1.f, cellWidth + 2.f, cellHeight + 2.f);
						nvgFillColor(args.vg, nvgRGBA(88, 240, 154, int(24.f + 48.f * pulse)));
						nvgFill(args.vg);
						nvgBeginPath(args.vg);
						nvgRect(args.vg, col * cellWidth, row * cellHeight, cellWidth, cellHeight);
						nvgStrokeColor(args.vg, nvgRGBA(98, 235, 154, int(188.f + 54.f * pulse)));
						nvgStrokeWidth(args.vg, 2.15f);
						nvgStroke(args.vg);
					}
				}
				if (!module->gameOver) {
					for (int destinationIndex : module->highlightedDestinations) {
						int row = 0;
						int col = 0;
						if (!crownstep::indexToCoord(destinationIndex, &row, &col)) {
							continue;
						}
						float centerX = (col + 0.5f) * cellWidth;
						float centerY = (row + 0.5f) * cellHeight;
						float phase = float(destinationIndex) * 0.43f;
						float breath = 0.5f + 0.5f * std::sin(module->transportTimeSeconds * 4.4f + phase);
						float glowRadius = std::min(cellWidth, cellHeight) * (0.17f + 0.06f * breath);
						nvgBeginPath(args.vg);
						nvgCircle(args.vg, centerX, centerY, glowRadius);
						nvgFillColor(args.vg, nvgRGBA(88, 240, 154, int(44.f + 50.f * breath)));
						nvgFill(args.vg);
						nvgBeginPath(args.vg);
						nvgCircle(args.vg, centerX, centerY, std::min(cellWidth, cellHeight) * 0.105f);
						nvgFillColor(args.vg, nvgRGB(98, 235, 154));
						nvgFill(args.vg);
					}
					for (int destinationIndex : module->opponentHighlightedDestinations) {
						bool overlapsHuman = false;
						for (int humanDestination : module->highlightedDestinations) {
							if (humanDestination == destinationIndex) {
								overlapsHuman = true;
								break;
							}
						}
						if (overlapsHuman) {
							continue;
						}
						int row = 0;
						int col = 0;
						if (!crownstep::indexToCoord(destinationIndex, &row, &col)) {
							continue;
						}
						float centerX = (col + 0.5f) * cellWidth;
						float centerY = (row + 0.5f) * cellHeight;
						float phase = float(destinationIndex) * 0.39f + 1.7f;
						float breath = 0.5f + 0.5f * std::sin(module->transportTimeSeconds * 4.1f + phase);
						float glowRadius = std::min(cellWidth, cellHeight) * (0.16f + 0.055f * breath);
						nvgBeginPath(args.vg);
						nvgCircle(args.vg, centerX, centerY, glowRadius);
						nvgFillColor(args.vg, nvgRGBA(255, 216, 114, int(36.f + 48.f * breath)));
						nvgFill(args.vg);
						nvgBeginPath(args.vg);
						nvgCircle(args.vg, centerX, centerY, std::min(cellWidth, cellHeight) * 0.092f);
						nvgFillColor(args.vg, nvgRGB(255, 213, 79));
						nvgFill(args.vg);
					}
				}
				if (!module->gameOver && module->lastMove.originIndex >= 0) {
					NVGcolor edgeColor = (module->lastMoveSide == HUMAN_SIDE) ? nvgRGB(98, 235, 154) : nvgRGB(255, 213, 79);
					for (int highlightIndex : {module->lastMove.originIndex, module->lastMove.destinationIndex}) {
						int row = 0;
						int col = 0;
						if (!crownstep::indexToCoord(highlightIndex, &row, &col)) {
							continue;
						}
						float phase = float(highlightIndex) * 0.34f + ((module->lastMoveSide == HUMAN_SIDE) ? 0.f : 1.5f);
						float pulse = 0.5f + 0.5f * std::sin(module->transportTimeSeconds * 3.8f + phase);
						nvgBeginPath(args.vg);
						nvgRect(args.vg, col * cellWidth - 0.5f, row * cellHeight - 0.5f, cellWidth + 1.f, cellHeight + 1.f);
						if (module->lastMoveSide == HUMAN_SIDE) {
							nvgFillColor(args.vg, nvgRGBA(88, 240, 154, int(18.f + 34.f * pulse)));
						}
						else {
							nvgFillColor(args.vg, nvgRGBA(255, 216, 114, int(18.f + 34.f * pulse)));
						}
						nvgFill(args.vg);
						nvgBeginPath(args.vg);
						nvgRect(args.vg, col * cellWidth + 1.f, row * cellHeight + 1.f, cellWidth - 2.f, cellHeight - 2.f);
						nvgStrokeColor(args.vg, edgeColor);
						nvgStrokeWidth(args.vg, 2.0f);
						nvgStroke(args.vg);
					}
				}
				if (!module->gameOver && module->captureFlashSeconds > 0.f && module->lastMove.destinationIndex >= 0) {
					int row = 0;
					int col = 0;
					if (crownstep::indexToCoord(module->lastMove.destinationIndex, &row, &col)) {
					float alpha = clamp(module->captureFlashSeconds / 0.16f, 0.f, 1.f);
					nvgBeginPath(args.vg);
					nvgRect(args.vg, col * cellWidth + 2.f, row * cellHeight + 2.f, cellWidth - 4.f, cellHeight - 4.f);
					nvgFillColor(args.vg, nvgRGBA(255, 210, 120, int(90.f * alpha)));
						nvgFill(args.vg);
					}
				}

					auto drawPieceAt = [&](float centerX, float centerY, int piece, float alpha) {
						if (piece == 0) {
							return;
						}
						alpha = clamp(alpha, 0.f, 1.f);
						float radius = std::min(cellWidth, cellHeight) * 0.36f;
						int fillAlpha = int(255.f * alpha);
						int strokeAlpha = int(240.f * alpha);
						bool humanPiece = piece > 0;

						NVGcolor coreInner = humanPiece ? nvgRGBA(237, 112, 94, fillAlpha) : nvgRGBA(78, 78, 86, fillAlpha);
						NVGcolor coreOuter = humanPiece ? nvgRGBA(152, 46, 38, fillAlpha) : nvgRGBA(12, 12, 16, fillAlpha);
						NVGcolor rimBright = humanPiece ? nvgRGBA(255, 228, 208, strokeAlpha) : nvgRGBA(210, 210, 216, strokeAlpha);
						NVGcolor rimDark = humanPiece ? nvgRGBA(82, 22, 16, int(210.f * alpha)) : nvgRGBA(6, 6, 9, int(215.f * alpha));

						// Base disc with beveled radial falloff.
						nvgBeginPath(args.vg);
						nvgCircle(args.vg, centerX, centerY, radius);
						NVGpaint corePaint = nvgRadialGradient(args.vg, centerX - radius * 0.18f, centerY - radius * 0.2f,
							radius * 0.14f, radius * 1.06f, coreInner, coreOuter);
						nvgFillPaint(args.vg, corePaint);
						nvgFill(args.vg);

						// Outer and inner rim lines for a machined/checker edge feel.
						nvgBeginPath(args.vg);
						nvgCircle(args.vg, centerX, centerY, radius);
						nvgStrokeColor(args.vg, rimBright);
						nvgStrokeWidth(args.vg, 1.55f);
						nvgStroke(args.vg);
						nvgBeginPath(args.vg);
						nvgCircle(args.vg, centerX, centerY, radius * 0.83f);
						nvgStrokeColor(args.vg, rimDark);
						nvgStrokeWidth(args.vg, 1.05f);
						nvgStroke(args.vg);

						// Tiny radial pips around the edge to imply textured perimeter.
						for (int pip = 0; pip < 14; ++pip) {
							float a = float(pip) * (2.f * float(M_PI) / 14.f);
							float ringX = centerX + std::cos(a) * radius * 0.92f;
							float ringY = centerY + std::sin(a) * radius * 0.92f;
							int pipAlpha = (pip & 1) ? int(48.f * alpha) : int(28.f * alpha);
							if (humanPiece) {
								nvgFillColor(args.vg, nvgRGBA(255, 214, 188, pipAlpha));
							}
							else {
								nvgFillColor(args.vg, nvgRGBA(170, 170, 180, pipAlpha));
							}
							nvgBeginPath(args.vg);
							nvgCircle(args.vg, ringX, ringY, radius * 0.042f);
							nvgFill(args.vg);
						}

						// Top sheen.
						nvgBeginPath(args.vg);
						nvgCircle(args.vg, centerX - radius * 0.20f, centerY - radius * 0.24f, radius * 0.34f);
						nvgFillColor(args.vg, nvgRGBA(255, 255, 255, int(36.f * alpha)));
						nvgFill(args.vg);

						if (crownstep::pieceIsKing(piece)) {
							// Symmetric outline crown icon.
							NVGcolor crownStrokeDark = nvgRGBA(82, 54, 18, int(230.f * alpha));
							NVGcolor crownStrokeLight = nvgRGBA(255, 224, 142, int(244.f * alpha));
							NVGcolor crownFillSoft = nvgRGBA(250, 214, 110, int(42.f * alpha));
							float crownBaseY = centerY + radius * 0.24f;
							float crownShoulderY = centerY + radius * 0.06f;
							float crownPeakY = centerY - radius * 0.24f;
							float crownCenterPeakY = centerY - radius * 0.31f;

							nvgBeginPath(args.vg);
							nvgMoveTo(args.vg, centerX - radius * 0.56f, crownBaseY);
							nvgLineTo(args.vg, centerX - radius * 0.56f, crownShoulderY);
							nvgLineTo(args.vg, centerX - radius * 0.35f, crownPeakY);
							nvgLineTo(args.vg, centerX - radius * 0.17f, crownShoulderY);
							nvgLineTo(args.vg, centerX, crownCenterPeakY);
							nvgLineTo(args.vg, centerX + radius * 0.17f, crownShoulderY);
							nvgLineTo(args.vg, centerX + radius * 0.35f, crownPeakY);
							nvgLineTo(args.vg, centerX + radius * 0.56f, crownShoulderY);
							nvgLineTo(args.vg, centerX + radius * 0.56f, crownBaseY);
							nvgClosePath(args.vg);
							nvgFillColor(args.vg, crownFillSoft);
							nvgFill(args.vg);

							nvgStrokeColor(args.vg, crownStrokeDark);
							nvgStrokeWidth(args.vg, 1.45f);
							nvgStroke(args.vg);
							nvgStrokeColor(args.vg, crownStrokeLight);
							nvgStrokeWidth(args.vg, 0.82f);
							nvgStroke(args.vg);

							nvgBeginPath(args.vg);
							nvgRect(args.vg, centerX - radius * 0.56f, crownBaseY - radius * 0.01f, radius * 1.12f, radius * 0.12f);
							nvgStrokeColor(args.vg, crownStrokeDark);
							nvgStrokeWidth(args.vg, 1.2f);
							nvgStroke(args.vg);
							nvgBeginPath(args.vg);
							nvgRect(args.vg, centerX - radius * 0.54f, crownBaseY + radius * 0.01f, radius * 1.08f, radius * 0.08f);
							nvgStrokeColor(args.vg, crownStrokeLight);
							nvgStrokeWidth(args.vg, 0.74f);
							nvgStroke(args.vg);

							for (float dx : {-0.34f, 0.f, 0.34f}) {
								nvgBeginPath(args.vg);
								nvgCircle(args.vg, centerX + radius * dx, centerY - radius * 0.07f, radius * 0.058f);
								nvgStrokeColor(args.vg, crownStrokeLight);
								nvgStrokeWidth(args.vg, 0.7f);
								nvgStroke(args.vg);
								nvgBeginPath(args.vg);
								nvgCircle(args.vg, centerX + radius * dx, centerY - radius * 0.07f, radius * 0.02f);
								nvgFillColor(args.vg, nvgRGBA(255, 235, 170, int(205.f * alpha)));
								nvgFill(args.vg);
							}
						}
					};

					auto indexIsQueuedDestination = [&](int index) {
						for (const Crownstep::MoveVisualAnimation& queued : module->moveAnimationQueue) {
							if (queued.destinationIndex == index) {
								return true;
							}
						}
						return false;
					};

					for (int i = 0; i < BOARD_SIZE; ++i) {
						int piece = module->board[size_t(i)];
						if (piece == 0) {
							continue;
						}
						if ((module->moveAnimation.active && i == module->moveAnimation.destinationIndex) || indexIsQueuedDestination(i)) {
							continue;
						}
						int row = 0;
						int col = 0;
						if (!crownstep::indexToCoord(i, &row, &col)) {
						continue;
					}
					float centerX = (col + 0.5f) * cellWidth;
						float centerY = (row + 0.5f) * cellHeight;
						drawPieceAt(centerX, centerY, piece, 1.f);
					}

					// Staged queued moves: keep the queued moving piece visible at its start cell
					// until that queued animation becomes active, preventing destination teleport.
					for (const Crownstep::MoveVisualAnimation& queued : module->moveAnimationQueue) {
						if (queued.path.empty() || queued.movingPiece == 0) {
							continue;
						}
						int startIndex = queued.path.front();
						int row = 0;
						int col = 0;
						if (!crownstep::indexToCoord(startIndex, &row, &col)) {
							continue;
						}
						drawPieceAt((col + 0.5f) * cellWidth, (row + 0.5f) * cellHeight, queued.movingPiece, 0.95f);
					}

				if (module->moveAnimation.active && module->moveAnimation.path.size() >= 2) {
					float duration = std::max(module->moveAnimation.durationSeconds, 1e-6f);
					float t = clamp(module->moveAnimation.elapsedSeconds / duration, 0.f, 1.f);

					for (size_t i = 0; i < module->moveAnimation.capturedIndices.size(); ++i) {
						int captureIndex = module->moveAnimation.capturedIndices[i];
						int capturedPiece = (i < module->moveAnimation.capturedPieces.size()) ? module->moveAnimation.capturedPieces[i] : 0;
						if (capturedPiece == 0) {
							continue;
						}
						int row = 0;
						int col = 0;
						if (!crownstep::indexToCoord(captureIndex, &row, &col)) {
							continue;
						}
						float ghostAlpha = clamp(0.72f - t * 1.45f, 0.f, 0.72f);
						drawPieceAt((col + 0.5f) * cellWidth, (row + 0.5f) * cellHeight, capturedPiece, ghostAlpha);
					}

					int segments = int(module->moveAnimation.path.size()) - 1;
					float segmentPos = t * float(segments);
					int segmentIndex = clamp(int(std::floor(segmentPos)), 0, std::max(segments - 1, 0));
					float localT = segmentPos - float(segmentIndex);
					if (segmentIndex >= segments) {
						segmentIndex = segments - 1;
						localT = 1.f;
					}

					int fromIndex = module->moveAnimation.path[size_t(segmentIndex)];
					int toIndex = module->moveAnimation.path[size_t(segmentIndex + 1)];
					int fromRow = 0;
					int fromCol = 0;
					int toRow = 0;
					int toCol = 0;
					if (crownstep::indexToCoord(fromIndex, &fromRow, &fromCol) && crownstep::indexToCoord(toIndex, &toRow, &toCol)) {
						float fromX = (fromCol + 0.5f) * cellWidth;
						float fromY = (fromRow + 0.5f) * cellHeight;
						float toX = (toCol + 0.5f) * cellWidth;
						float toY = (toRow + 0.5f) * cellHeight;
						float centerX = fromX + (toX - fromX) * localT;
						float centerY = fromY + (toY - fromY) * localT;
						float shadowRadius = std::min(cellWidth, cellHeight) * 0.40f;

						nvgBeginPath(args.vg);
						nvgCircle(args.vg, centerX + 0.9f, centerY + 1.2f, shadowRadius);
						nvgFillColor(args.vg, nvgRGBA(0, 0, 0, 42));
						nvgFill(args.vg);

						drawPieceAt(centerX, centerY, module->moveAnimation.movingPiece, 1.f);
					}
				}

				// Keep forced/destination hint visibility even when a destination is visually occupied
				// (e.g. during queued animations) by drawing a top-layer pulse over the piece.
				if (!module->gameOver) {
					for (int destinationIndex : module->highlightedDestinations) {
						if (destinationIndex < 0 || destinationIndex >= BOARD_SIZE) {
							continue;
						}
						bool occupied = module->board[size_t(destinationIndex)] != 0;
						bool landingAnimation =
							(module->moveAnimation.active && module->moveAnimation.destinationIndex == destinationIndex)
							|| indexIsQueuedDestination(destinationIndex);
						if (!occupied && !landingAnimation) {
							continue;
						}
						int row = 0;
						int col = 0;
						if (!crownstep::indexToCoord(destinationIndex, &row, &col)) {
							continue;
						}
						float centerX = (col + 0.5f) * cellWidth;
						float centerY = (row + 0.5f) * cellHeight;
						float phase = float(destinationIndex) * 0.43f + 0.9f;
						float breath = 0.5f + 0.5f * std::sin(module->transportTimeSeconds * 4.8f + phase);
						float ringRadius = std::min(cellWidth, cellHeight) * (0.21f + 0.05f * breath);
						nvgBeginPath(args.vg);
						nvgCircle(args.vg, centerX, centerY, ringRadius);
						nvgStrokeColor(args.vg, nvgRGBA(98, 235, 154, int(186.f + 62.f * breath)));
						nvgStrokeWidth(args.vg, 1.9f);
						nvgStroke(args.vg);
					}

					for (int destinationIndex : module->opponentHighlightedDestinations) {
						bool overlapsHuman = false;
						for (int humanDestination : module->highlightedDestinations) {
							if (humanDestination == destinationIndex) {
								overlapsHuman = true;
								break;
							}
						}
						if (overlapsHuman || destinationIndex < 0 || destinationIndex >= BOARD_SIZE) {
							continue;
						}
						bool occupied = module->board[size_t(destinationIndex)] != 0;
						bool landingAnimation =
							(module->moveAnimation.active && module->moveAnimation.destinationIndex == destinationIndex)
							|| indexIsQueuedDestination(destinationIndex);
						if (!occupied && !landingAnimation) {
							continue;
						}
						int row = 0;
						int col = 0;
						if (!crownstep::indexToCoord(destinationIndex, &row, &col)) {
							continue;
						}
						float centerX = (col + 0.5f) * cellWidth;
						float centerY = (row + 0.5f) * cellHeight;
						float phase = float(destinationIndex) * 0.39f + 2.2f;
						float breath = 0.5f + 0.5f * std::sin(module->transportTimeSeconds * 4.2f + phase);
						float ringRadius = std::min(cellWidth, cellHeight) * (0.205f + 0.05f * breath);
						nvgBeginPath(args.vg);
						nvgCircle(args.vg, centerX, centerY, ringRadius);
						nvgStrokeColor(args.vg, nvgRGBA(255, 213, 79, int(180.f + 68.f * breath)));
						nvgStrokeWidth(args.vg, 1.85f);
						nvgStroke(args.vg);
						nvgBeginPath(args.vg);
						nvgCircle(args.vg, centerX, centerY, std::min(cellWidth, cellHeight) * 0.062f);
						nvgFillColor(args.vg, nvgRGBA(255, 222, 128, int(190.f + 56.f * breath)));
						nvgFill(args.vg);
					}
				}

				if (module->gameOver) {
				nvgBeginPath(args.vg);
				nvgRect(args.vg, 0.f, 0.f, box.size.x, box.size.y);
				nvgFillColor(args.vg, nvgRGBA(8, 8, 10, 92));
				nvgFill(args.vg);
				nvgBeginPath(args.vg);
				nvgRect(args.vg, 0.f, box.size.y * 0.39f, box.size.x, box.size.y * 0.22f);
				nvgFillColor(args.vg, nvgRGBA(10, 10, 12, 180));
				nvgFill(args.vg);
				nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
				nvgFontSize(args.vg, 15.f);
				nvgFillColor(args.vg, nvgRGB(244, 229, 206));
				nvgText(args.vg, box.size.x * 0.5f, box.size.y * 0.47f, "Game Complete", nullptr);
				nvgFontSize(args.vg, 10.5f);
				nvgFillColor(args.vg, nvgRGB(213, 189, 160));
				nvgText(args.vg, box.size.x * 0.5f, box.size.y * 0.545f, "Playback Mode", nullptr);
			}
		}
		nvgRestore(args.vg);
	}
};

struct CrownstepDifficultyItem final : MenuItem {
	Crownstep* module = nullptr;
	int difficulty = 0;

	void onAction(const event::Action& e) override {
		if (module) {
			module->aiDifficulty = difficulty;
		}
		MenuItem::onAction(e);
	}

	void step() override {
		rightText = (module && module->aiDifficulty == difficulty) ? "✓" : "";
		MenuItem::step();
	}
};

struct CrownstepWidget final : ModuleWidget {
	explicit CrownstepWidget(Crownstep* module) {
		setModule(module);
		const std::string panelPath = asset::plugin(pluginInstance, "res/crownstep.svg");
		try {
			setPanel(createPanel(panelPath));
		}
		catch (const std::exception& e) {
			WARN("Crownstep panel load failed (%s), using fallback: %s", panelPath.c_str(), e.what());
			setPanel(createPanel(asset::plugin(pluginInstance, "res/proc.svg")));
		}

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
			addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
			addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

			CrownstepBoardWidget* boardWidget = new CrownstepBoardWidget(module);
			math::Rect boardRectMm;
			if (loadRectFromSvgMm(panelPath, "BOARD_AREA", &boardRectMm)) {
				boardWidget->box.pos = mm2px(boardRectMm.pos);
				boardWidget->box.size = mm2px(boardRectMm.size);
			}
			else {
				boardWidget->box.pos = mm2px(Vec(5.5f, 11.f));
				boardWidget->box.size = mm2px(Vec(80.5f, 80.5f));
			}
			addChild(boardWidget);

		// Bottom control layout:
		// left cluster = inputs, center = knobs/button, right cluster = outputs.
		math::Rect inputsAreaMm;
		math::Rect outputsAreaMm;
		math::Rect controlsAreaMm;
		bool hasInputsArea = loadRectFromSvgMm(panelPath, "INPUTS_AREA", &inputsAreaMm);
		bool hasOutputsArea = loadRectFromSvgMm(panelPath, "OUTPUTS_AREA", &outputsAreaMm);
		bool hasControlsArea = loadRectFromSvgMm(panelPath, "CONTROLS_AREA", &controlsAreaMm);
		bool hasBottomAnchors = hasInputsArea || hasOutputsArea || hasControlsArea;

		auto pointInRect = [](const math::Rect& rect, float u, float v) {
			return Vec(rect.pos.x + rect.size.x * u, rect.pos.y + rect.size.y * v);
		};

		Vec seqPos(43.f, 101.5f);
		Vec newGamePos(43.f, 114.0f);
		Vec clockPos(12.f, 108.0f);
		Vec resetPos(28.f, 108.0f);
		Vec transposePos(12.f, 121.0f);
		Vec rootCvPos(28.f, 121.0f);
		Vec pitchPos(58.f, 108.0f);
		Vec accentPos(58.f, 121.0f);
		Vec modPos(74.f, 121.0f);
		Vec eocPos(86.f, 121.0f);

		if (hasInputsArea) {
			clockPos = pointInRect(inputsAreaMm, 0.30f, 0.38f);
			resetPos = pointInRect(inputsAreaMm, 0.70f, 0.38f);
			transposePos = pointInRect(inputsAreaMm, 0.30f, 0.78f);
			rootCvPos = pointInRect(inputsAreaMm, 0.70f, 0.78f);
		}
		if (hasOutputsArea) {
			pitchPos = pointInRect(outputsAreaMm, 0.17f, 0.58f);
			accentPos = pointInRect(outputsAreaMm, 0.39f, 0.58f);
			modPos = pointInRect(outputsAreaMm, 0.61f, 0.58f);
			eocPos = pointInRect(outputsAreaMm, 0.83f, 0.58f);
		}
		if (hasBottomAnchors) {
			float controlX = 43.f;
			float controlY = 98.f;
			float controlH = 22.f;
			if (hasControlsArea) {
				controlX = controlsAreaMm.pos.x + controlsAreaMm.size.x * 0.5f;
				controlY = controlsAreaMm.pos.y;
				controlH = controlsAreaMm.size.y;
			}
			else {
				if (hasInputsArea && hasOutputsArea) {
					float leftX = inputsAreaMm.pos.x + inputsAreaMm.size.x;
					float rightX = outputsAreaMm.pos.x;
					controlX = (leftX + rightX) * 0.5f;
					controlY = (inputsAreaMm.pos.y + outputsAreaMm.pos.y) * 0.5f;
					controlH = (inputsAreaMm.size.y + outputsAreaMm.size.y) * 0.5f;
				}
				else if (hasInputsArea) {
					controlX = inputsAreaMm.pos.x + inputsAreaMm.size.x + 8.f;
					controlY = inputsAreaMm.pos.y;
					controlH = inputsAreaMm.size.y;
				}
				else if (hasOutputsArea) {
					controlX = outputsAreaMm.pos.x - 8.f;
					controlY = outputsAreaMm.pos.y;
					controlH = outputsAreaMm.size.y;
				}
			}
			seqPos = Vec(controlX, controlY + controlH * 0.28f);
			newGamePos = Vec(controlX, controlY + controlH * 0.76f);
		}

		addParam(createParamCentered<RoundBlackKnob>(mm2px(seqPos), module, Crownstep::SEQ_LENGTH_PARAM));
		addParam(createParamCentered<LEDButton>(mm2px(newGamePos), module, Crownstep::NEW_GAME_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(clockPos), module, Crownstep::CLOCK_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(resetPos), module, Crownstep::RESET_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(transposePos), module, Crownstep::TRANSPOSE_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(rootCvPos), module, Crownstep::ROOT_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(pitchPos), module, Crownstep::PITCH_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(accentPos), module, Crownstep::ACCENT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(modPos), module, Crownstep::MOD_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(eocPos), module, Crownstep::EOC_OUTPUT));

		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(82.5f, 93.f)), module, Crownstep::HUMAN_TURN_LIGHT));
		addChild(createLightCentered<SmallLight<BlueLight>>(mm2px(Vec(86.5f, 93.f)), module, Crownstep::AI_TURN_LIGHT));
	}

	void appendContextMenu(Menu* menu) override {
		ModuleWidget::appendContextMenu(menu);
		Crownstep* module = dynamic_cast<Crownstep*>(this->module);
		menu->addChild(new MenuSeparator());
		MenuLabel* quantizerLabel = new MenuLabel();
		quantizerLabel->text = "Quantizer";
		menu->addChild(quantizerLabel);
		menu->addChild(createSubmenuItem("Scale", "", [=](Menu* scaleMenu) {
			for (int i = 0; i < int(SCALES.size()); ++i) {
				scaleMenu->addChild(createCheckMenuItem(
					SCALES[size_t(i)].name,
					"",
					[=]() {
						return module && clamp(int(std::round(module->params[Crownstep::SCALE_PARAM].getValue())), 0,
							int(SCALES.size()) - 1) == i;
					},
					[=]() {
						if (module) {
							module->params[Crownstep::SCALE_PARAM].setValue(float(i));
						}
					}
				));
			}
		}));
		menu->addChild(createSubmenuItem("Key", "", [=](Menu* keyMenu) {
			for (int i = 0; i < int(KEY_NAMES.size()); ++i) {
				keyMenu->addChild(createCheckMenuItem(
					KEY_NAMES[size_t(i)],
					"",
					[=]() {
						return module && clamp(int(std::round(module->params[Crownstep::ROOT_PARAM].getValue())), 0, 11) == i;
					},
					[=]() {
						if (module) {
							module->params[Crownstep::ROOT_PARAM].setValue(float(i));
						}
					}
				));
			}
		}));
		MenuLabel* label = new MenuLabel();
		label->text = "AI Difficulty";
		menu->addChild(label);
		for (int i = 0; i < int(DIFFICULTY_NAMES.size()); ++i) {
			CrownstepDifficultyItem* item = new CrownstepDifficultyItem();
			item->text = DIFFICULTY_NAMES[size_t(i)];
			item->module = module;
			item->difficulty = i;
			menu->addChild(item);
		}
	}
};

Model* modelCrownstep = createModel<Crownstep, CrownstepWidget>("Crownstep");
