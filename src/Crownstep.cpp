#include "plugin.hpp"
#include "CrownstepCore.hpp"

#include <cstdio>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <exception>
#include <fstream>
#include <mutex>
#include <regex>
#include <sstream>
#include <string>
#include <thread>
#include <condition_variable>

using crownstep::AI_SIDE;
using crownstep::BoardState;
using crownstep::ChessState;
using crownstep::BOARD_VALUE_LAYOUT_NAMES;
using crownstep::DIFFICULTY_NAMES;
using crownstep::HUMAN_SIDE;
using crownstep::KEY_NAMES;
using crownstep::Move;
using crownstep::PITCH_INTERPRETATION_NAMES;
using crownstep::SCALES;
using crownstep::Step;

static constexpr float NO_SEQUENCE_PITCH_VOLTS = -10.f;
static constexpr float AI_TURN_DELAY_SECONDS = 0.5f;
static constexpr float OTHELLO_FLIP_SECONDS_PER_PIECE = 0.1f;
static constexpr int ROOT_CV_MAX_OFFSET_SEMITONES = 10;
static constexpr float ROOT_CV_VOLTS_PER_SEMITONE = 1.f;
static constexpr float TRANSPOSE_CV_ZERO_DEADBAND_VOLTS = 1e-3f;
static constexpr int SEQ_LENGTH_MIN = 1;
static constexpr int SEQ_LENGTH_MAX = 64;
static constexpr std::array<const char*, 2> BOARD_TEXTURE_NAMES = {{"Wood", "Marble"}};
static constexpr std::array<const char*, 3> GAME_MODE_NAMES = {{"Checkers", "Chess", "Othello"}};

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

static bool loadPointFromSvgMm(const std::string& svgPath, const std::string& elementId, Vec* outPointMm) {
	if (!outPointMm) {
		return false;
	}

	std::ifstream svgFile(svgPath);
	if (!svgFile.good()) {
		return false;
	}

	std::ostringstream svgBuffer;
	svgBuffer << svgFile.rdbuf();
	const std::string svgText = svgBuffer.str();

	const std::regex elementRegex(
		"<(ellipse|circle|rect)\\b[^>]*\\bid\\s*=\\s*\"" + elementId + "\"[^>]*>",
		std::regex::icase
	);
	std::smatch elementMatch;
	if (!std::regex_search(svgText, elementMatch, elementRegex)) {
		return false;
	}
	const std::string elementTag = elementMatch.str(0);

	auto parseAttrMm = [&](const char* attr, float* outMm) -> bool {
		if (!outMm) {
			return false;
		}
		const std::regex attrRegex(std::string("\\b") + attr + "\\s*=\\s*\"([^\"]+)\"", std::regex::icase);
		std::smatch attrMatch;
		if (!std::regex_search(elementTag, attrMatch, attrRegex)) {
			return false;
		}
		*outMm = std::stof(attrMatch.str(1)) * 0.01f;
		return true;
	};

	float cxMm = 0.f;
	float cyMm = 0.f;
	if (parseAttrMm("cx", &cxMm) && parseAttrMm("cy", &cyMm)) {
		*outPointMm = Vec(cxMm, cyMm);
		return true;
	}

	float xMm = 0.f;
	float yMm = 0.f;
	float wMm = 0.f;
	float hMm = 0.f;
	if (parseAttrMm("x", &xMm) && parseAttrMm("y", &yMm) && parseAttrMm("width", &wMm) && parseAttrMm("height", &hMm)) {
		*outPointMm = Vec(xMm + 0.5f * wMm, yMm + 0.5f * hMm);
		return true;
	}

	return false;
}

struct Crownstep;

struct CrownstepSeqLengthQuantity final : ParamQuantity {
	std::string getDisplayValueString() override;
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
	enum BoardTextureMode {
		BOARD_TEXTURE_WOOD = 0,
		BOARD_TEXTURE_MARBLE,
		BOARD_TEXTURE_COUNT
	};
	enum GameMode {
		GAME_MODE_CHECKERS = 0,
		GAME_MODE_CHESS,
		GAME_MODE_OTHELLO,
		GAME_MODE_COUNT
	};

	BoardState board = crownstep::makeInitialBoard();
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
	bool quantizationEnabled = true;
	bool pitchBipolarEnabled = false;
	int pitchInterpretationMode = 0;
	int boardValueLayoutMode = 0;
	int pitchDividerMode = 0;
	int boardTextureMode = BOARD_TEXTURE_WOOD;
	int gameMode = GAME_MODE_CHECKERS;
	bool opponentHintsPreviewActive = false;
	int playhead = 0;
	int displayedStep = 0;
	float transportTimeSeconds = 0.f;
	float lastClockEdgeSeconds = -1.f;
	float previousClockPeriodSeconds = -1.f;
	float heldPitch = NO_SEQUENCE_PITCH_VOLTS;
	float heldAccent = 0.f;
	float heldMod = 0.f;
	float modOutputVolts = 0.f;
	float modGlideStartVolts = 0.f;
	float modGlideTargetVolts = 0.f;
	float modGlideStartSeconds = 0.f;
	float modGlideDurationSeconds = 0.f;
	double aiTurnDelayStartSeconds = 0.0;
	double uiLastServiceSeconds = 0.0;
	float captureFlashSeconds = 0.f;
	bool modGlideActive = false;
	bool aiTurnDelayPending = false;
	bool aiTurnDelayActive = false;
	int cachedRootSemitoneWrapped = 0;
	int cachedRootSemitoneLinear = 0;
	bool cachedRootSemitoneValid = false;
	bool gameOver = false;
	ChessState chessState = crownstep::chessInitialState();
	const crownstep::IGameRules* gameRules = &crownstep::checkersRules();
	struct AiWorkerRequest {
		uint64_t id = 0;
		int gameMode = GAME_MODE_CHECKERS;
		int difficulty = 0;
		BoardState board {};
		ChessState chessState = crownstep::chessInitialState();
	};
	struct AiWorkerResult {
		uint64_t id = 0;
		Move move;
	};
	std::thread aiWorkerThread;
	std::mutex aiWorkerMutex;
	std::condition_variable aiWorkerCv;
	AiWorkerRequest aiWorkerRequest;
	AiWorkerResult aiWorkerResult;
	uint64_t aiWorkerNextRequestId = 1;
	uint64_t aiWorkerInFlightRequestId = 0;
	bool aiWorkerStopRequested = false;
	bool aiWorkerHasRequest = false;
	bool aiWorkerHasResult = false;
	mutable std::recursive_mutex sequenceMutex;

	Crownstep() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		configParam<CrownstepSeqLengthQuantity>(
			SEQ_LENGTH_PARAM, float(SEQ_LENGTH_MIN), float(SEQ_LENGTH_MAX), float(SEQ_LENGTH_MAX), "Sequence length");
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

		startAiWorker();
		setGameMode(GAME_MODE_CHECKERS, true);
	}

	~Crownstep() override {
		stopAiWorker();
	}

	bool isChessMode() const {
		return gameRules && std::strcmp(gameRules->gameId(), "chess") == 0;
	}

	bool isOthelloMode() const {
		return gameRules && std::strcmp(gameRules->gameId(), "othello") == 0;
	}

	static Move chooseAiMoveForSnapshot(const AiWorkerRequest& request) {
		switch (request.gameMode) {
			case GAME_MODE_CHESS:
				return crownstep::chessChooseAiMove(request.board, request.difficulty, request.chessState);
			case GAME_MODE_OTHELLO:
				return crownstep::othelloChooseAiMove(request.board, request.difficulty);
			case GAME_MODE_CHECKERS:
			default:
				return crownstep::chooseAiMove(request.board, request.difficulty);
		}
	}

	void runAiWorkerLoop() {
		while (true) {
			AiWorkerRequest request;
			{
				std::unique_lock<std::mutex> lock(aiWorkerMutex);
				aiWorkerCv.wait(lock, [&]() {
					return aiWorkerStopRequested || aiWorkerHasRequest;
				});
				if (aiWorkerStopRequested) {
					return;
				}
				request = aiWorkerRequest;
				aiWorkerHasRequest = false;
			}

			AiWorkerResult result;
			result.id = request.id;
			result.move = chooseAiMoveForSnapshot(request);

			{
				std::lock_guard<std::mutex> lock(aiWorkerMutex);
				aiWorkerResult = result;
				aiWorkerHasResult = true;
			}
		}
	}

	void startAiWorker() {
		std::lock_guard<std::mutex> lock(aiWorkerMutex);
		if (aiWorkerThread.joinable()) {
			return;
		}
		aiWorkerStopRequested = false;
		aiWorkerThread = std::thread([this]() {
			runAiWorkerLoop();
		});
	}

	void stopAiWorker() {
		{
			std::lock_guard<std::mutex> lock(aiWorkerMutex);
			aiWorkerStopRequested = true;
			aiWorkerHasRequest = false;
		}
		aiWorkerCv.notify_all();
		if (aiWorkerThread.joinable()) {
			aiWorkerThread.join();
		}
	}

	void cancelAiTurnWork() {
		aiTurnDelayPending = false;
		aiTurnDelayActive = false;
		aiTurnDelayStartSeconds = 0.f;
		std::lock_guard<std::mutex> lock(aiWorkerMutex);
		aiWorkerHasRequest = false;
		aiWorkerHasResult = false;
		aiWorkerInFlightRequestId = 0;
	}

	void clearAiWorkerQueueState() {
		std::lock_guard<std::mutex> lock(aiWorkerMutex);
		aiWorkerHasRequest = false;
		aiWorkerHasResult = false;
		aiWorkerInFlightRequestId = 0;
	}

	bool consumeReadyAiResult(Move* outMove) {
		if (!outMove) {
			return false;
		}
		std::lock_guard<std::mutex> lock(aiWorkerMutex);
		if (!aiWorkerHasResult) {
			return false;
		}
		*outMove = aiWorkerResult.move;
		uint64_t resultId = aiWorkerResult.id;
		aiWorkerHasResult = false;
		if (aiWorkerInFlightRequestId != 0 && resultId == aiWorkerInFlightRequestId) {
			aiWorkerInFlightRequestId = 0;
			return true;
		}
		return false;
	}

	bool dispatchAiRequestIfIdle() {
		std::lock_guard<std::mutex> lock(aiWorkerMutex);
		if (aiWorkerInFlightRequestId != 0 || aiWorkerHasRequest) {
			return false;
		}
		AiWorkerRequest request;
		request.id = aiWorkerNextRequestId++;
		request.gameMode = gameMode;
		request.difficulty = aiDifficulty;
		request.board = board;
		request.chessState = chessState;
		aiWorkerRequest = request;
		aiWorkerHasRequest = true;
		aiWorkerInFlightRequestId = request.id;
		aiWorkerCv.notify_one();
		return true;
	}

	void setGameMode(int mode, bool startFreshGame) {
		int nextMode = clamp(mode, 0, GAME_MODE_COUNT - 1);
		cancelAiTurnWork();
		gameMode = nextMode;
		switch (gameMode) {
			case GAME_MODE_CHESS:
				gameRules = &crownstep::chessRules();
				break;
			case GAME_MODE_OTHELLO:
				gameRules = &crownstep::othelloRules();
				break;
			case GAME_MODE_CHECKERS:
			default:
				gameRules = &crownstep::checkersRules();
				break;
		}
		if (startFreshGame) {
			startNewGame();
		}
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

	void beginMoveAnimation(const Move& move, const BoardState& beforeBoard, int moverSide) {
		MoveVisualAnimation nextAnimation;
		int cellCount = boardCellCount();
		if (move.originIndex < 0 || move.originIndex >= cellCount || move.destinationIndex < 0 || move.destinationIndex >= cellCount) {
			return;
		}

		int movingPiece = beforeBoard[size_t(move.originIndex)];
		if (movingPiece == 0 && isOthelloMode()) {
			// Othello places a new disc on an empty destination square.
			movingPiece = moverSide;
		}
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
			if (captureIndex >= 0 && captureIndex < cellCount) {
				nextAnimation.capturedPieces.push_back(beforeBoard[size_t(captureIndex)]);
			}
			else {
				nextAnimation.capturedPieces.push_back(0);
			}
		}
		if (isOthelloMode() && !nextAnimation.capturedIndices.empty()) {
			int destRow = 0;
			int destCol = 0;
			boardIndexToCoord(nextAnimation.destinationIndex, &destRow, &destCol);
			struct OrderedFlip {
				float dist2 = 0.f;
				int index = -1;
				int piece = 0;
			};
			std::vector<OrderedFlip> ordered;
			ordered.reserve(nextAnimation.capturedIndices.size());
			for (size_t i = 0; i < nextAnimation.capturedIndices.size(); ++i) {
				int captureIndex = nextAnimation.capturedIndices[i];
				int capturePiece = (i < nextAnimation.capturedPieces.size()) ? nextAnimation.capturedPieces[i] : 0;
				int row = 0;
				int col = 0;
				boardIndexToCoord(captureIndex, &row, &col);
				float dr = float(row - destRow);
				float dc = float(col - destCol);
				OrderedFlip flip;
				flip.dist2 = dr * dr + dc * dc;
				flip.index = captureIndex;
				flip.piece = capturePiece;
				ordered.push_back(flip);
			}
			std::sort(ordered.begin(), ordered.end(), [](const OrderedFlip& a, const OrderedFlip& b) {
				if (a.dist2 != b.dist2) {
					return a.dist2 < b.dist2;
				}
				return a.index < b.index;
			});
			nextAnimation.capturedIndices.clear();
			nextAnimation.capturedPieces.clear();
			for (const OrderedFlip& flip : ordered) {
				nextAnimation.capturedIndices.push_back(flip.index);
				nextAnimation.capturedPieces.push_back(flip.piece);
			}
		}

		if (nextAnimation.path.size() < 2) {
			return;
		}

		float segments = float(nextAnimation.path.size() - 1);
		nextAnimation.durationSeconds = clamp(0.085f * segments, 0.12f, 0.34f);
		if (isOthelloMode() && !nextAnimation.capturedIndices.empty()) {
			nextAnimation.durationSeconds = std::max(
				nextAnimation.durationSeconds,
				OTHELLO_FLIP_SECONDS_PER_PIECE * float(nextAnimation.capturedIndices.size())
			);
		}
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
		displayedStep = 0;
		lastClockEdgeSeconds = -1.f;
		previousClockPeriodSeconds = -1.f;
		heldPitch = NO_SEQUENCE_PITCH_VOLTS;
		heldAccent = 0.f;
		heldMod = 0.f;
		modOutputVolts = 0.f;
		modGlideStartVolts = 0.f;
			modGlideTargetVolts = 0.f;
			modGlideStartSeconds = transportTimeSeconds;
			modGlideDurationSeconds = 0.f;
			modGlideActive = false;
			cachedRootSemitoneValid = false;
			cancelAiTurnWork();
			resetMoveAnimation();
		}

	void armDelayedAiTurnAfterHumanMove() {
		if (gameOver || turnSide != aiSide()) {
			aiTurnDelayPending = false;
			aiTurnDelayActive = false;
			return;
		}
		clearAiWorkerQueueState();
		aiTurnDelayPending = true;
		aiTurnDelayActive = false;
		aiTurnDelayStartSeconds = 0.0;
		uiLastServiceSeconds = 0.0;
	}

	void advanceUiAnimationClock(double nowSeconds) {
		if (nowSeconds <= 0.0) {
			return;
		}
		if (uiLastServiceSeconds <= 0.0) {
			uiLastServiceSeconds = nowSeconds;
			return;
		}
		float dt = float(nowSeconds - uiLastServiceSeconds);
		if (dt < 0.f) {
			dt = 0.f;
		}
		// Clamp to avoid giant jumps after debugger pauses/window stalls.
		dt = std::min(dt, 0.1f);
		uiLastServiceSeconds = nowSeconds;

		captureFlashSeconds = std::max(0.f, captureFlashSeconds - dt);
		if (moveAnimation.active) {
			moveAnimation.elapsedSeconds += dt;
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
	}

	int currentSequenceCap() {
		int requested = clamp(int(std::round(params[SEQ_LENGTH_PARAM].getValue())), SEQ_LENGTH_MIN, SEQ_LENGTH_MAX);
		// Max knob turn means full history window.
		if (requested >= SEQ_LENGTH_MAX) {
			return 0;
		}
		return requested;
	}

	int currentScaleIndex() {
		return clamp(int(std::round(params[SCALE_PARAM].getValue())), 0, int(SCALES.size()) - 1);
	}

	float transposeVolts() {
		float value = clamp(inputs[TRANSPOSE_INPUT].getVoltage(), -10.f, 10.f);
		if (std::fabs(value) < TRANSPOSE_CV_ZERO_DEADBAND_VOLTS) {
			return 0.f;
		}
		return value;
	}

	int rootCvOffsetSemitone() {
		float rootCv = clamp(inputs[ROOT_INPUT].getVoltage(), -10.f, 10.f);
		// Semitone-domain mapping with direct CV anchors:
		// -10V -> -10 semitones, 0V -> 0 semitones, +10V -> +10 semitones.
		int cvOffsetSemitones = int(std::lround(rootCv / ROOT_CV_VOLTS_PER_SEMITONE));
		cvOffsetSemitones = clamp(cvOffsetSemitones, -ROOT_CV_MAX_OFFSET_SEMITONES, ROOT_CV_MAX_OFFSET_SEMITONES);
		return cvOffsetSemitones;
	}

	int rootSemitoneLinear() {
		int knobSemitone = clamp(int(std::round(params[ROOT_PARAM].getValue())), 0, 11);
		int cvOffsetSemitones = rootCvOffsetSemitone();
		return knobSemitone + cvOffsetSemitones;
	}

	int rootSemitone() {
		int total = rootSemitoneLinear();
		return crownstep::wrapSemitone12(total);
	}

	int humanSide() const {
		return gameRules ? gameRules->humanSide() : HUMAN_SIDE;
	}

	int aiSide() const {
		return gameRules ? gameRules->aiSide() : AI_SIDE;
	}

	int opposingSide(int side) const {
		if (side == humanSide()) {
			return aiSide();
		}
		if (side == aiSide()) {
			return humanSide();
		}
		return -side;
	}

	bool boardIndexToCoord(int index, int* row, int* col) const {
		if (gameRules) {
			return gameRules->indexToCoord(index, row, col);
		}
		return crownstep::indexToCoord(index, row, col);
	}

	int boardCoordToIndex(int row, int col) const {
		if (gameRules) {
			return gameRules->coordToIndex(row, col);
		}
		return crownstep::coordToIndex(row, col);
	}

	int boardCellCount() const {
		int localCount = int(board.size());
		int rulesCount = gameRules ? gameRules->boardCellCount() : localCount;
		return std::max(0, std::min(localCount, rulesCount));
	}

	float pitchForMove(const Move& move) {
		auto chessBoardValueForIndex = [&](int boardIndex, int layoutMode) {
			int clamped = clamp(boardIndex, 0, crownstep::CHESS_BOARD_SIZE - 1);
			int mode = clamp(layoutMode, 0, int(BOARD_VALUE_LAYOUT_NAMES.size()) - 1);
			switch (mode) {
				case 1: {
					int row = clamped / 8;
					int col = clamped % 8;
					int serpentineCol = (row & 1) ? (7 - col) : col;
					return row * 8 + serpentineCol;
				}
				case 2: {
					int row = clamped / 8;
					int col = clamped % 8;
					float dx = float(col) - 3.5f;
					float dy = float(row) - 3.5f;
					float metric = dx * dx + dy * dy + float(row) * 0.01f + float(col) * 0.001f;
					int rank = 0;
					for (int i = 0; i < crownstep::CHESS_BOARD_SIZE; ++i) {
						int ir = i / 8;
						int ic = i % 8;
						float idx = float(ic) - 3.5f;
						float idy = float(ir) - 3.5f;
						float m = idx * idx + idy * idy + float(ir) * 0.01f + float(ic) * 0.001f;
						if (m < metric) {
							rank++;
						}
					}
					return rank;
				}
				case 0:
				default:
					return clamped;
			}
		};
		auto chessBoardValueForSampledIndex = [&](float sampledIndex, int layoutMode) {
			float x = clamp(sampledIndex, 0.f, float(crownstep::CHESS_BOARD_SIZE - 1));
			int low = int(std::floor(x));
			int high = int(std::ceil(x));
			float lowValue = float(chessBoardValueForIndex(low, layoutMode));
			if (high <= low) {
				return lowValue;
			}
			float highValue = float(chessBoardValueForIndex(high, layoutMode));
			return lowValue + (highValue - lowValue) * (x - float(low));
		};
		auto sampledBoardValueForActiveGame = [&]() {
			if (boardCellCount() != crownstep::CHESS_BOARD_SIZE) {
				return crownstep::sampledBoardValueForMove(move, pitchInterpretationMode, boardValueLayoutMode);
			}
			int mode = clamp(pitchInterpretationMode, 0, int(PITCH_INTERPRETATION_NAMES.size()) - 1);
			float origin = float(clamp(move.originIndex, 0, crownstep::CHESS_BOARD_SIZE - 1));
			float destination = float(clamp(move.destinationIndex, 0, crownstep::CHESS_BOARD_SIZE - 1));
			float originValue = chessBoardValueForSampledIndex(origin, boardValueLayoutMode);
			float destinationValue = chessBoardValueForSampledIndex(destination, boardValueLayoutMode);
			if (mode == 1) {
				return destinationValue;
			}
			if (mode == 2) {
				return 0.5f * (originValue + destinationValue);
			}
			return originValue;
		};

		float boardValueIndex = sampledBoardValueForActiveGame();
		boardValueIndex = crownstep::applyPitchDividerToBoardValue(boardValueIndex, pitchDividerMode);
		if (pitchBipolarEnabled) {
			float center = (0.5f * float(boardCellCount() - 1)) / crownstep::pitchDividerForMode(pitchDividerMode);
			boardValueIndex -= center;
		}
		if (quantizationEnabled) {
			return crownstep::mapPitchFromIndex(
				boardValueIndex,
				move.isKing,
				currentScaleIndex(),
				rootSemitone(),
				transposeVolts()
			);
		}
		float rawTranspose = transposeVolts() + float(rootSemitoneLinear()) / 12.f;
		return crownstep::mapRawPitchFromIndex(boardValueIndex, move.isKing, rawTranspose);
	}

	Step makeStepFromMove(const Move& move) {
		Step step = crownstep::makeStepFromMove(move, currentScaleIndex(), rootSemitone(), transposeVolts());
		step.pitch = pitchForMove(move);
		return step;
	}

	void startNewGame() {
		board = gameRules ? gameRules->makeInitialBoard() : crownstep::makeInitialBoard();
		chessState = crownstep::chessInitialState();
		{
			std::lock_guard<std::recursive_mutex> lock(sequenceMutex);
			history.clear();
			moveHistory.clear();
		}
		highlightedDestinations.clear();
		opponentHighlightedDestinations.clear();
		selectedSquare = -1;
		hoveredSquare = -1;
		lastMove = Move();
		turnSide = humanSide();
		winnerSide = 0;
		lastMoveSide = 0;
		gameOver = false;
		captureFlashSeconds = 0.f;
		resetPlayback();
		resetMoveAnimation();
		refreshLegalMoves();
		advanceForcedPassesIfNeeded();
	}

	void setHoveredSquare(int index) {
		int maxIndex = boardCellCount();
		int normalizedIndex = (index >= 0 && index < maxIndex) ? index : -1;
		if (hoveredSquare == normalizedIndex) {
			return;
		}
		hoveredSquare = normalizedIndex;

		// Hover only influences UI move-hint previewing while the user is
		// selecting a human move and the game is still active.
		if (!gameOver && turnSide == humanSide() && selectedSquare >= 0) {
			refreshLegalMoves();
		}
	}

	void refreshLegalMoves() {
		if (isChessMode()) {
			humanMoves = crownstep::chessGenerateLegalMovesForSide(board, humanSide(), chessState);
			aiMoves = crownstep::chessGenerateLegalMovesForSide(board, aiSide(), chessState);
		}
		else {
			humanMoves = gameRules ? gameRules->generateLegalMovesForSide(board, humanSide())
			                      : crownstep::generateLegalMovesForSide(board, humanSide());
			aiMoves = gameRules ? gameRules->generateLegalMovesForSide(board, aiSide())
			                    : crownstep::generateLegalMovesForSide(board, aiSide());
		}
		highlightedDestinations.clear();
		opponentHighlightedDestinations.clear();
		opponentHintsPreviewActive = false;
		if (isOthelloMode() && turnSide == humanSide()) {
			std::vector<uint8_t> seen(size_t(boardCellCount()), 0u);
			for (const Move& move : humanMoves) {
				if (move.destinationIndex < 0 || move.destinationIndex >= boardCellCount()) {
					continue;
				}
				if (seen[size_t(move.destinationIndex)] != 0u) {
					continue;
				}
				seen[size_t(move.destinationIndex)] = 1u;
				highlightedDestinations.push_back(move.destinationIndex);
			}
		}
		else if (selectedSquare >= 0) {
			for (const Move& move : humanMoves) {
				if (move.originIndex == selectedSquare) {
					highlightedDestinations.push_back(move.destinationIndex);
				}
			}
			if (highlightedDestinations.empty()) {
				selectedSquare = -1;
			}
		}
		bool showOpponentTips = (selectedSquare >= 0) || (turnSide == aiSide());
		const std::vector<Move>* opponentMoveSource = &aiMoves;
		std::vector<Move> previewAiMoves;
		if (selectedSquare >= 0 && hoveredSquare >= 0) {
			for (const Move& move : humanMoves) {
				if (move.originIndex == selectedSquare && move.destinationIndex == hoveredSquare) {
					BoardState previewBoard {};
					if (isChessMode()) {
						ChessState previewState;
						previewBoard = crownstep::chessApplyMoveToBoard(board, move, chessState, &previewState);
						previewAiMoves = crownstep::chessGenerateLegalMovesForSide(previewBoard, aiSide(), previewState);
					}
					else {
						previewBoard = gameRules ? gameRules->applyMoveToBoard(board, move) : crownstep::applyMoveToBoard(board, move);
						previewAiMoves = gameRules ? gameRules->generateLegalMovesForSide(previewBoard, aiSide())
						                           : crownstep::generateLegalMovesForSide(previewBoard, aiSide());
					}
					opponentMoveSource = &previewAiMoves;
					opponentHintsPreviewActive = true;
					break;
				}
			}
		}
		if (showOpponentTips) {
			int cellCount = boardCellCount();
			std::vector<uint8_t> seen(size_t(cellCount), 0u);
			for (const Move& move : *opponentMoveSource) {
				int destination = move.destinationIndex;
				if (destination < 0 || destination >= cellCount || seen[size_t(destination)] != 0u) {
					continue;
				}
				seen[size_t(destination)] = 1u;
				opponentHighlightedDestinations.push_back(destination);
			}
		}
		const std::vector<Move>& activeMoves = (turnSide == humanSide()) ? humanMoves : aiMoves;
		const std::vector<Move>& opposingMoves = (turnSide == humanSide()) ? aiMoves : humanMoves;
		if (isOthelloMode()) {
			gameOver = activeMoves.empty() && opposingMoves.empty();
		}
		else {
			gameOver = activeMoves.empty();
		}
		winnerSide = gameOver
			? (gameRules ? gameRules->winnerForNoLegalMoves(board, turnSide) : opposingSide(turnSide))
			: 0;
	}

	void advanceForcedPassesIfNeeded() {
		if (!isOthelloMode() || gameOver) {
			return;
		}
		for (int i = 0; i < 2; ++i) {
			const std::vector<Move>& activeMoves = (turnSide == humanSide()) ? humanMoves : aiMoves;
			const std::vector<Move>& opposingMoves = (turnSide == humanSide()) ? aiMoves : humanMoves;
			if (!activeMoves.empty() || gameOver) {
				return;
			}
			if (opposingMoves.empty()) {
				gameOver = true;
				winnerSide = gameRules ? gameRules->winnerForNoLegalMoves(board, turnSide) : 0;
				return;
			}
			turnSide = opposingSide(turnSide);
			selectedSquare = -1;
			hoveredSquare = -1;
			highlightedDestinations.clear();
			opponentHighlightedDestinations.clear();
			refreshLegalMoves();
		}
	}

	int searchDepthForDifficulty() const {
		return gameRules ? gameRules->searchDepthForDifficulty(aiDifficulty) : crownstep::searchDepthForDifficulty(aiDifficulty);
	}

	float expressiveModForMove(
		const Move& move,
		const BoardState& beforeBoard,
		const BoardState& afterBoard,
		int moverSide
	) const {
		auto pieceCounts = [](const BoardState& sourceBoard, int* men, int* kings) {
			int localMen = 0;
			int localKings = 0;
			for (int piece : sourceBoard) {
				if (piece == 0) {
					continue;
				}
				if (std::abs(piece) <= 1) {
					localMen++;
				}
				else {
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
		float captureNorm = clamp(float(move.captured.size()) / 4.f, 0.f, 1.f);

		int men = 0;
		int kings = 0;
		pieceCounts(afterBoard, &men, &kings);
		// Lower piece count generally means a more exposed/endgame board state.
		float initialPieceCount = isChessMode() ? 32.f : 24.f;
		float phaseNorm = clamp((initialPieceCount - (float(men) + float(kings))) / initialPieceCount, 0.f, 1.f);

		int beforeMaterial = gameRules ? gameRules->evaluateBoardMaterial(beforeBoard) : crownstep::evaluateBoardMaterial(beforeBoard);
		int afterMaterial = gameRules ? gameRules->evaluateBoardMaterial(afterBoard) : crownstep::evaluateBoardMaterial(afterBoard);
		float moverMaterialBefore = (moverSide == aiSide()) ? float(beforeMaterial) : -float(beforeMaterial);
		float moverMaterialAfter = (moverSide == aiSide()) ? float(afterMaterial) : -float(afterMaterial);
		float materialSwingNorm = clamp(std::fabs(moverMaterialAfter - moverMaterialBefore) / 900.f, 0.f, 1.f);

		float combined = 0.56f * moveEnergy + 0.20f * materialSwingNorm + 0.14f * captureNorm + 0.10f * phaseNorm;
		return clamp(combined, 0.f, 1.f);
	}

	void commitMove(const Move& move, int moverSide) {
		if (move.originIndex < 0 || move.destinationIndex < 0) {
			return;
		}
		const BoardState beforeBoard = board;
		ChessState nextChessState = chessState;
		const BoardState afterBoard = isChessMode()
			? crownstep::chessApplyMoveToBoard(beforeBoard, move, chessState, &nextChessState)
			: (isOthelloMode()
				? crownstep::othelloApplyMoveToBoard(beforeBoard, move, moverSide)
				: (gameRules ? gameRules->applyMoveToBoard(beforeBoard, move) : crownstep::applyMoveToBoard(beforeBoard, move)));
		beginMoveAnimation(move, beforeBoard, moverSide);
		board = afterBoard;
		if (isChessMode()) {
			chessState = nextChessState;
		}
		lastMove = move;
		lastMoveSide = moverSide;
		Step step = makeStepFromMove(move);
		step.mod = expressiveModForMove(move, beforeBoard, afterBoard, moverSide);
		{
			std::lock_guard<std::recursive_mutex> lock(sequenceMutex);
			moveHistory.push_back(move);
			history.push_back(step);
		}
		selectedSquare = -1;
		highlightedDestinations.clear();
		turnSide = opposingSide(moverSide);
		captureFlashSeconds = move.isCapture ? 0.16f : 0.f;
		refreshLegalMoves();
		advanceForcedPassesIfNeeded();
	}

	// UI-thread service: AI search runs in worker thread, UI applies ready moves.
	void serviceAiTurnFromUiThread() {
		double nowSeconds = system::getTime();
		advanceUiAnimationClock(nowSeconds);

		if (gameOver || turnSide != aiSide()) {
			aiTurnDelayPending = false;
			aiTurnDelayActive = false;
			clearAiWorkerQueueState();
			return;
		}

		if (aiTurnDelayPending) {
				bool animationsActive = moveAnimation.active || !moveAnimationQueue.empty();
				if (!aiTurnDelayActive) {
					if (!animationsActive) {
						aiTurnDelayActive = true;
						aiTurnDelayStartSeconds = nowSeconds;
						// Start AI thinking immediately when the mandatory delay window starts.
						dispatchAiRequestIfIdle();
					}
					return;
				}

				if (nowSeconds - aiTurnDelayStartSeconds < AI_TURN_DELAY_SECONDS) {
					// Keep worker busy while we enforce minimum think-time.
					dispatchAiRequestIfIdle();
					return;
				}

			aiTurnDelayPending = false;
			aiTurnDelayActive = false;
		}

		// If we reached AI turn through a path without an armed delay, still dispatch.
		dispatchAiRequestIfIdle();

		Move readyMove;
		bool hasReadyMove = consumeReadyAiResult(&readyMove);
		if (hasReadyMove && !gameOver && turnSide == aiSide()) {
			if (readyMove.originIndex >= 0 && readyMove.destinationIndex >= 0) {
				commitMove(readyMove, aiSide());
			}
			else if (isOthelloMode()) {
				advanceForcedPassesIfNeeded();
			}
		}
	}

	void onBoardSquarePressed(int index) {
		if (turnSide != humanSide() || gameOver) {
			return;
		}
		int cellCount = boardCellCount();
		if (index < 0 || index >= cellCount) {
			return;
		}

		if (isOthelloMode()) {
			for (const Move& move : humanMoves) {
				if (move.destinationIndex == index) {
					commitMove(move, humanSide());
					armDelayedAiTurnAfterHumanMove();
					return;
				}
			}
			return;
		}

		int piece = board[size_t(index)];
		if (crownstep::pieceSide(piece) == humanSide()) {
			selectedSquare = index;
			refreshLegalMoves();
			return;
		}

		if (selectedSquare < 0) {
			return;
		}

		for (const Move& move : humanMoves) {
			if (move.originIndex == selectedSquare && move.destinationIndex == index) {
				commitMove(move, humanSide());
				armDelayedAiTurnAfterHumanMove();
				return;
			}
		}
	}

	int activeLength() {
		std::lock_guard<std::recursive_mutex> lock(sequenceMutex);
		return crownstep::activeLength(int(history.size()), currentSequenceCap());
	}

	int activeStartIndex() {
		std::lock_guard<std::recursive_mutex> lock(sequenceMutex);
		return crownstep::activeStartIndex(int(history.size()), currentSequenceCap());
	}

	float pitchForSequenceIndex(int sequenceIndex) {
		std::lock_guard<std::recursive_mutex> lock(sequenceMutex);
		if (sequenceIndex < 0) {
			return 0.f;
		}

		// Preferred path: derive pitch from move sequence so interpretation
		// and quantization settings are applied live at playback time.
		if (sequenceIndex < int(moveHistory.size())) {
			const Move& move = moveHistory[size_t(sequenceIndex)];
			return pitchForMove(move);
		}

		// Backward compatibility for older saves that may not contain moveHistory.
		if (sequenceIndex < int(history.size())) {
			return history[size_t(sequenceIndex)].pitch;
		}

		return 0.f;
	}

	void refreshHeldPitchForCurrentStep() {
		std::lock_guard<std::recursive_mutex> lock(sequenceMutex);
		int historySize = int(history.size());
		int sequenceCap = currentSequenceCap();
		int length = crownstep::activeLength(historySize, sequenceCap);
		if (length <= 0) {
			heldPitch = NO_SEQUENCE_PITCH_VOLTS;
			return;
		}
		if (displayedStep <= 0) {
			return;
		}
		int shownStep = clamp(displayedStep, 1, length);
		int sequenceIndex = crownstep::activeStartIndex(historySize, sequenceCap) + (shownStep - 1);
		heldPitch = pitchForSequenceIndex(sequenceIndex);
	}

	void emitStepAtClockEdge() {
		std::lock_guard<std::recursive_mutex> lock(sequenceMutex);
		int length = activeLength();
		if (length <= 0) {
			displayedStep = 0;
			heldPitch = NO_SEQUENCE_PITCH_VOLTS;
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
		displayedStep = playhead + 1;
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

		if (newGameTrigger.process(params[NEW_GAME_PARAM].getValue())) {
			startNewGame();
		}

		if (resetTrigger.process(inputs[RESET_INPUT].getVoltage())) {
			playhead = 0;
			displayedStep = 0;
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

		int effectiveRootWrapped = rootSemitone();
		int effectiveRootLinear = rootSemitoneLinear();
		if (!cachedRootSemitoneValid) {
			cachedRootSemitoneWrapped = effectiveRootWrapped;
			cachedRootSemitoneLinear = effectiveRootLinear;
			cachedRootSemitoneValid = true;
		}
		else if (
			effectiveRootWrapped != cachedRootSemitoneWrapped
			|| effectiveRootLinear != cachedRootSemitoneLinear
		) {
			cachedRootSemitoneWrapped = effectiveRootWrapped;
			cachedRootSemitoneLinear = effectiveRootLinear;
			refreshHeldPitchForCurrentStep();
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
		lights[HUMAN_TURN_LIGHT].setBrightness(!gameOver && turnSide == humanSide() ? 1.f : 0.f);
		lights[AI_TURN_LIGHT].setBrightness(!gameOver && turnSide == aiSide() ? 1.f : 0.f);
	}

	json_t* dataToJson() override {
		json_t* rootJ = json_object();
		json_object_set_new(rootJ, "turnSide", json_integer(turnSide));
		json_object_set_new(rootJ, "winnerSide", json_integer(winnerSide));
		json_object_set_new(rootJ, "selectedSquare", json_integer(selectedSquare));
		json_object_set_new(rootJ, "aiDifficulty", json_integer(aiDifficulty));
		json_object_set_new(rootJ, "quantizationEnabled", json_boolean(quantizationEnabled));
		json_object_set_new(rootJ, "pitchBipolarEnabled", json_boolean(pitchBipolarEnabled));
		json_object_set_new(rootJ, "pitchInterpretationMode", json_integer(pitchInterpretationMode));
		json_object_set_new(rootJ, "boardValueLayoutMode", json_integer(boardValueLayoutMode));
		json_object_set_new(rootJ, "pitchDividerMode", json_integer(pitchDividerMode));
		json_object_set_new(rootJ, "boardTextureMode", json_integer(boardTextureMode));
		json_object_set_new(rootJ, "gameMode", json_integer(gameMode));
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

	void dataFromJson(json_t* rootJ) override {
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

		json_t* turnJ = json_object_get(rootJ, "turnSide");
		if (turnJ) {
			turnSide = json_integer_value(turnJ) >= 0 ? humanSide() : aiSide();
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
		json_t* boardTextureModeJ = json_object_get(rootJ, "boardTextureMode");
		if (boardTextureModeJ) {
			boardTextureMode = clamp(int(json_integer_value(boardTextureModeJ)), 0, int(BOARD_TEXTURE_NAMES.size()) - 1);
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
			lastMoveSide = (side > 0) ? humanSide() : ((side < 0) ? aiSide() : 0);
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
	};

std::string CrownstepSeqLengthQuantity::getDisplayValueString() {
	int requested = clamp(int(std::round(getValue())), SEQ_LENGTH_MIN, SEQ_LENGTH_MAX);
	if (requested >= SEQ_LENGTH_MAX) {
		return "Full";
	}
	const Crownstep* crownstepModule = dynamic_cast<const Crownstep*>(module);
	if (!crownstepModule) {
		return std::to_string(requested);
	}
	int available = int(crownstepModule->history.size());
	if (available > 0 && requested >= available) {
		return "Full";
	}
	return std::to_string(requested);
}

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
		return module ? module->boardCoordToIndex(row, col) : crownstep::coordToIndex(row, col);
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
		bool othelloBoard = module && module->isOthelloMode();
		for (int row = 0; row < 8; ++row) {
			for (int col = 0; col < 8; ++col) {
				bool dark = ((row + col) & 1) == 1;
				bool marbleTexture = module && module->boardTextureMode == Crownstep::BOARD_TEXTURE_MARBLE;
				float x = col * cellWidth;
				float y = row * cellHeight;
				float seed = float((row * 29 + col * 17) % 97) * 0.17f;
				if (othelloBoard) {
					// Othello board: green felt/table look with subtle per-cell variation.
					float tint = 0.5f + 0.5f * std::sin(seed * 1.7f + float((row + col) & 1) * 0.9f);
					NVGcolor topColor = nvgRGBA(
						int(18.f + 8.f * tint),
						int(104.f + 16.f * tint),
						int(56.f + 10.f * tint),
						255
					);
					NVGcolor bottomColor = nvgRGBA(
						int(10.f + 6.f * tint),
						int(72.f + 11.f * tint),
						int(40.f + 8.f * tint),
						255
					);
					nvgBeginPath(args.vg);
					nvgRect(args.vg, x, y, cellWidth, cellHeight);
					NVGpaint basePaint = nvgLinearGradient(args.vg, x, y, x, y + cellHeight, topColor, bottomColor);
					nvgFillPaint(args.vg, basePaint);
					nvgFill(args.vg);
				}
				else if (marbleTexture) {
					NVGcolor topColor = dark ? nvgRGB(72, 74, 82) : nvgRGB(208, 212, 222);
					NVGcolor bottomColor = dark ? nvgRGB(42, 44, 52) : nvgRGB(166, 172, 184);
					nvgBeginPath(args.vg);
					nvgRect(args.vg, x, y, cellWidth, cellHeight);
					NVGpaint basePaint = nvgLinearGradient(args.vg, x, y, x, y + cellHeight, topColor, bottomColor);
					nvgFillPaint(args.vg, basePaint);
					nvgFill(args.vg);

					NVGcolor cloudA = dark ? nvgRGBA(116, 122, 140, 30) : nvgRGBA(255, 255, 255, 44);
					NVGcolor cloudB = dark ? nvgRGBA(30, 34, 46, 22) : nvgRGBA(144, 152, 168, 28);
					nvgBeginPath(args.vg);
					nvgRect(args.vg, x, y, cellWidth, cellHeight);
					NVGpaint cloudPaint = nvgLinearGradient(args.vg, x, y, x + cellWidth, y + cellHeight, cloudA, cloudB);
					nvgFillPaint(args.vg, cloudPaint);
					nvgFill(args.vg);

					for (int vein = 0; vein < 3; ++vein) {
						float t = (vein + 1.f) / 4.f;
						float y0 = y + t * cellHeight + std::sin(seed * 0.9f + float(vein) * 1.8f) * 0.9f;
						float y1 = y0 + std::cos(seed * 0.6f + float(vein) * 1.5f) * 1.1f;
						NVGcolor veinColor = dark ? nvgRGBA(178, 186, 206, 44) : nvgRGBA(116, 124, 144, 54);
						nvgBeginPath(args.vg);
						nvgMoveTo(args.vg, x + 0.8f, y0);
						nvgBezierTo(args.vg, x + cellWidth * 0.30f, y0 - 1.1f, x + cellWidth * 0.65f, y1 + 1.1f, x + cellWidth - 0.8f, y1);
						nvgStrokeColor(args.vg, veinColor);
						nvgStrokeWidth(args.vg, 0.62f + 0.12f * float(vein & 1));
						nvgStroke(args.vg);
					}
				}
				else {
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
				}
			}
		}
		if (othelloBoard) {
			NVGcolor gridColor = nvgRGBA(8, 42, 24, 214);
			nvgBeginPath(args.vg);
			for (int i = 0; i <= 8; ++i) {
				float x = float(i) * cellWidth;
				nvgMoveTo(args.vg, x, 0.f);
				nvgLineTo(args.vg, x, box.size.y);
			}
			for (int i = 0; i <= 8; ++i) {
				float y = float(i) * cellHeight;
				nvgMoveTo(args.vg, 0.f, y);
				nvgLineTo(args.vg, box.size.x, y);
			}
			nvgStrokeColor(args.vg, gridColor);
			nvgStrokeWidth(args.vg, 1.1f);
			nvgStroke(args.vg);

			nvgBeginPath(args.vg);
			nvgRect(args.vg, 0.7f, 0.7f, box.size.x - 1.4f, box.size.y - 1.4f);
			nvgStrokeColor(args.vg, nvgRGBA(4, 24, 14, 228));
			nvgStrokeWidth(args.vg, 1.6f);
			nvgStroke(args.vg);
		}

				if (module) {
					// Temporary UX tweak: hide AI potential-move hint dots/rings.
					const bool renderOpponentMoveHints = false;
					if (!module->gameOver && module->selectedSquare >= 0) {
					int row = 0;
					int col = 0;
					if (module->boardIndexToCoord(module->selectedSquare, &row, &col)) {
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
						if (!module->boardIndexToCoord(destinationIndex, &row, &col)) {
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
						if (renderOpponentMoveHints) for (int destinationIndex : module->opponentHighlightedDestinations) {
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
						if (!module->boardIndexToCoord(destinationIndex, &row, &col)) {
							continue;
						}
						if (module->opponentHintsPreviewActive && module->board[size_t(destinationIndex)] != 0) {
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
						NVGcolor edgeColor =
							(module->lastMoveSide == module->humanSide()) ? nvgRGB(98, 235, 154) : nvgRGB(255, 213, 79);
					for (int highlightIndex : {module->lastMove.originIndex, module->lastMove.destinationIndex}) {
						int row = 0;
						int col = 0;
						if (!module->boardIndexToCoord(highlightIndex, &row, &col)) {
							continue;
						}
						float phase = float(highlightIndex) * 0.34f + ((module->lastMoveSide == module->humanSide()) ? 0.f : 1.5f);
						float pulse = 0.5f + 0.5f * std::sin(module->transportTimeSeconds * 3.8f + phase);
						nvgBeginPath(args.vg);
						nvgRect(args.vg, col * cellWidth - 0.5f, row * cellHeight - 0.5f, cellWidth + 1.f, cellHeight + 1.f);
						if (module->lastMoveSide == module->humanSide()) {
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
					if (module->boardIndexToCoord(module->lastMove.destinationIndex, &row, &col)) {
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
							if (module->isOthelloMode()) {
								float discR = radius * 1.02f;
								// Human side renders black discs, AI renders white discs.
								NVGcolor edge = humanPiece ? nvgRGBA(28, 28, 32, strokeAlpha) : nvgRGBA(198, 200, 208, strokeAlpha);
								NVGcolor high = humanPiece ? nvgRGBA(96, 100, 114, fillAlpha) : nvgRGBA(254, 254, 255, fillAlpha);
								NVGcolor low = humanPiece ? nvgRGBA(12, 14, 18, fillAlpha) : nvgRGBA(202, 206, 216, fillAlpha);

								nvgBeginPath(args.vg);
								nvgCircle(args.vg, centerX, centerY, discR);
								NVGpaint discPaint = nvgRadialGradient(
									args.vg,
									centerX - discR * 0.24f,
									centerY - discR * 0.26f,
									discR * 0.12f,
									discR * 1.05f,
									high,
									low
								);
								nvgFillPaint(args.vg, discPaint);
								nvgFill(args.vg);
								nvgStrokeColor(args.vg, edge);
								nvgStrokeWidth(args.vg, 1.15f);
								nvgStroke(args.vg);

								nvgBeginPath(args.vg);
								nvgCircle(args.vg, centerX - discR * 0.21f, centerY - discR * 0.24f, discR * 0.28f);
								nvgFillColor(args.vg, nvgRGBA(255, 255, 255, int(40.f * alpha)));
								nvgFill(args.vg);
								return;
							}
								if (module->isChessMode()) {
									int pieceType = std::abs(piece);
									float pieceR = radius * 2.42f;
									const float chessPieceYOffset = pieceR * 0.03f;
									const float chessOutlineStroke = 1.08f;
									const float kingCrossInnerStroke = chessOutlineStroke;
									const float kingCrossOutlineStroke = 1.92f;
									NVGcolor pieceFill = humanPiece ? nvgRGBA(238, 232, 214, fillAlpha) : nvgRGBA(34, 36, 44, fillAlpha);
									NVGcolor pieceEdge = humanPiece ? nvgRGBA(76, 68, 56, strokeAlpha) : nvgRGBA(214, 220, 234, strokeAlpha);
									NVGcolor pieceDetail = humanPiece ? nvgRGBA(44, 36, 28, fillAlpha) : nvgRGBA(246, 246, 252, fillAlpha);
									NVGcolor pieceContrast = humanPiece ? nvgRGBA(24, 24, 30, strokeAlpha) : nvgRGBA(250, 250, 255, strokeAlpha);
									float baseY = centerY + pieceR * 0.29f;
									nvgSave(args.vg);
									nvgTranslate(args.vg, 0.f, chessPieceYOffset);

									auto fillAndStrokeCurrentPath = [&]() {
										nvgFillColor(args.vg, pieceFill);
										nvgFill(args.vg);
										nvgStrokeColor(args.vg, pieceEdge);
										nvgStrokeWidth(args.vg, chessOutlineStroke);
										nvgStroke(args.vg);
									};

									auto drawPedestal = [&]() {
										nvgBeginPath(args.vg);
										nvgRoundedRect(
											args.vg,
											centerX - pieceR * 0.42f,
											baseY,
											pieceR * 0.84f,
											pieceR * 0.12f,
											pieceR * 0.04f
										);
										fillAndStrokeCurrentPath();
										nvgBeginPath(args.vg);
										nvgMoveTo(args.vg, centerX - pieceR * 0.35f, baseY + pieceR * 0.055f);
										nvgLineTo(args.vg, centerX + pieceR * 0.35f, baseY + pieceR * 0.055f);
										nvgStrokeColor(args.vg, pieceDetail);
										nvgStrokeWidth(args.vg, chessOutlineStroke * 0.85f);
										nvgStroke(args.vg);
									};

									auto drawTrapezoidBody = [&](float yTop, float yBottom, float halfTopW, float halfBottomW) {
										nvgBeginPath(args.vg);
										nvgMoveTo(args.vg, centerX - halfBottomW, yBottom);
										nvgLineTo(args.vg, centerX - halfTopW, yTop);
										nvgLineTo(args.vg, centerX + halfTopW, yTop);
										nvgLineTo(args.vg, centerX + halfBottomW, yBottom);
										nvgClosePath(args.vg);
										fillAndStrokeCurrentPath();
									};

									switch (pieceType) {
									case crownstep::CHESS_PAWN: {
										drawPedestal();
										nvgBeginPath(args.vg);
										nvgCircle(args.vg, centerX, centerY - pieceR * 0.26f, pieceR * 0.14f);
										fillAndStrokeCurrentPath();
										nvgBeginPath(args.vg);
										nvgMoveTo(args.vg, centerX - pieceR * 0.20f, baseY);
										nvgBezierTo(args.vg,
											centerX - pieceR * 0.20f, centerY - pieceR * 0.01f,
											centerX - pieceR * 0.10f, centerY - pieceR * 0.17f,
											centerX, centerY - pieceR * 0.17f);
										nvgBezierTo(args.vg,
											centerX + pieceR * 0.10f, centerY - pieceR * 0.17f,
											centerX + pieceR * 0.20f, centerY - pieceR * 0.01f,
											centerX + pieceR * 0.20f, baseY);
										nvgClosePath(args.vg);
										fillAndStrokeCurrentPath();
										break;
									}
									case crownstep::CHESS_ROOK: {
										drawPedestal();
										drawTrapezoidBody(centerY - pieceR * 0.32f, baseY, pieceR * 0.20f, pieceR * 0.29f);
										nvgBeginPath(args.vg);
										nvgRoundedRect(
											args.vg,
											centerX - pieceR * 0.32f,
											centerY - pieceR * 0.42f,
											pieceR * 0.64f,
											pieceR * 0.12f,
											pieceR * 0.02f
										);
										fillAndStrokeCurrentPath();
										for (int i = -1; i <= 1; ++i) {
											float notchX = centerX + float(i) * pieceR * 0.15f;
											nvgBeginPath(args.vg);
											nvgRect(args.vg, notchX - pieceR * 0.045f, centerY - pieceR * 0.42f, pieceR * 0.09f, pieceR * 0.05f);
											nvgFillColor(args.vg, pieceContrast);
											nvgFill(args.vg);
										}
										break;
									}
									case crownstep::CHESS_KNIGHT: {
										drawPedestal();
										nvgBeginPath(args.vg);
										nvgMoveTo(args.vg, centerX - pieceR * 0.31f, baseY);
										nvgBezierTo(args.vg,
											centerX - pieceR * 0.31f, centerY - pieceR * 0.18f,
											centerX - pieceR * 0.15f, centerY - pieceR * 0.42f,
											centerX + pieceR * 0.03f, centerY - pieceR * 0.50f);
										nvgBezierTo(args.vg,
											centerX + pieceR * 0.22f, centerY - pieceR * 0.57f,
											centerX + pieceR * 0.31f, centerY - pieceR * 0.39f,
											centerX + pieceR * 0.20f, centerY - pieceR * 0.31f);
										nvgBezierTo(args.vg,
											centerX + pieceR * 0.12f, centerY - pieceR * 0.26f,
											centerX + pieceR * 0.10f, centerY - pieceR * 0.18f,
											centerX + pieceR * 0.16f, centerY - pieceR * 0.10f);
										nvgBezierTo(args.vg,
											centerX + pieceR * 0.08f, centerY + pieceR * 0.05f,
											centerX - pieceR * 0.08f, centerY + pieceR * 0.03f,
											centerX - pieceR * 0.16f, centerY - pieceR * 0.05f);
										nvgLineTo(args.vg, centerX - pieceR * 0.22f, baseY);
										nvgClosePath(args.vg);
										fillAndStrokeCurrentPath();
										nvgBeginPath(args.vg);
										nvgCircle(args.vg, centerX + pieceR * 0.08f, centerY - pieceR * 0.38f, pieceR * 0.030f);
										nvgFillColor(args.vg, pieceContrast);
										nvgFill(args.vg);
										nvgBeginPath(args.vg);
										nvgMoveTo(args.vg, centerX - pieceR * 0.15f, centerY - pieceR * 0.10f);
										nvgLineTo(args.vg, centerX + pieceR * 0.02f, centerY - pieceR * 0.20f);
										nvgStrokeColor(args.vg, pieceDetail);
										nvgStrokeWidth(args.vg, chessOutlineStroke);
										nvgStroke(args.vg);
										break;
									}
									case crownstep::CHESS_BISHOP: {
										drawPedestal();
										nvgBeginPath(args.vg);
										nvgCircle(args.vg, centerX, centerY - pieceR * 0.46f, pieceR * 0.07f);
										fillAndStrokeCurrentPath();
										nvgBeginPath(args.vg);
										nvgMoveTo(args.vg, centerX, centerY - pieceR * 0.40f);
										nvgBezierTo(args.vg,
											centerX + pieceR * 0.22f, centerY - pieceR * 0.26f,
											centerX + pieceR * 0.21f, centerY - pieceR * 0.03f,
											centerX, baseY);
										nvgBezierTo(args.vg,
											centerX - pieceR * 0.21f, centerY - pieceR * 0.03f,
											centerX - pieceR * 0.22f, centerY - pieceR * 0.26f,
											centerX, centerY - pieceR * 0.40f);
										nvgClosePath(args.vg);
										fillAndStrokeCurrentPath();
										nvgBeginPath(args.vg);
										nvgMoveTo(args.vg, centerX - pieceR * 0.09f, centerY - pieceR * 0.24f);
										nvgLineTo(args.vg, centerX + pieceR * 0.09f, centerY - pieceR * 0.08f);
										nvgStrokeColor(args.vg, pieceDetail);
										nvgStrokeWidth(args.vg, chessOutlineStroke);
										nvgStroke(args.vg);
										break;
									}
									case crownstep::CHESS_QUEEN: {
										drawPedestal();
										drawTrapezoidBody(centerY - pieceR * 0.29f, baseY, pieceR * 0.11f, pieceR * 0.30f);
										nvgBeginPath(args.vg);
										nvgMoveTo(args.vg, centerX - pieceR * 0.35f, centerY - pieceR * 0.31f);
										nvgLineTo(args.vg, centerX - pieceR * 0.24f, centerY - pieceR * 0.49f);
										nvgLineTo(args.vg, centerX - pieceR * 0.10f, centerY - pieceR * 0.34f);
										nvgLineTo(args.vg, centerX, centerY - pieceR * 0.54f);
										nvgLineTo(args.vg, centerX + pieceR * 0.10f, centerY - pieceR * 0.34f);
										nvgLineTo(args.vg, centerX + pieceR * 0.24f, centerY - pieceR * 0.49f);
										nvgLineTo(args.vg, centerX + pieceR * 0.35f, centerY - pieceR * 0.31f);
										nvgLineTo(args.vg, centerX + pieceR * 0.30f, centerY - pieceR * 0.23f);
										nvgLineTo(args.vg, centerX - pieceR * 0.30f, centerY - pieceR * 0.23f);
										nvgClosePath(args.vg);
										fillAndStrokeCurrentPath();
										for (int i = -2; i <= 2; ++i) {
											float cx = centerX + float(i) * pieceR * 0.12f;
											float cy = centerY - pieceR * (0.55f - std::abs(float(i)) * 0.06f);
											nvgBeginPath(args.vg);
											nvgCircle(args.vg, cx, cy, pieceR * 0.038f);
											fillAndStrokeCurrentPath();
										}
										break;
									}
									case crownstep::CHESS_KING:
									default: {
										drawPedestal();
										drawTrapezoidBody(centerY - pieceR * 0.34f, baseY, pieceR * 0.13f, pieceR * 0.28f);
										nvgBeginPath(args.vg);
										nvgRoundedRect(
											args.vg,
											centerX - pieceR * 0.15f,
											centerY - pieceR * 0.44f,
											pieceR * 0.30f,
											pieceR * 0.10f,
											pieceR * 0.03f
										);
										fillAndStrokeCurrentPath();
										nvgBeginPath(args.vg);
										nvgMoveTo(args.vg, centerX - pieceR * 0.13f, centerY - pieceR * 0.22f);
										nvgLineTo(args.vg, centerX + pieceR * 0.13f, centerY - pieceR * 0.22f);
										nvgStrokeColor(args.vg, pieceDetail);
										nvgStrokeWidth(args.vg, chessOutlineStroke);
										nvgStroke(args.vg);

										nvgBeginPath(args.vg);
										nvgMoveTo(args.vg, centerX, centerY - pieceR * 0.66f);
										nvgLineTo(args.vg, centerX, centerY - pieceR * 0.40f);
										nvgMoveTo(args.vg, centerX - pieceR * 0.13f, centerY - pieceR * 0.53f);
										nvgLineTo(args.vg, centerX + pieceR * 0.13f, centerY - pieceR * 0.53f);
										nvgStrokeColor(args.vg, pieceContrast);
										nvgStrokeWidth(args.vg, kingCrossOutlineStroke);
										nvgStroke(args.vg);

										nvgBeginPath(args.vg);
										nvgMoveTo(args.vg, centerX, centerY - pieceR * 0.66f);
										nvgLineTo(args.vg, centerX, centerY - pieceR * 0.40f);
										nvgMoveTo(args.vg, centerX - pieceR * 0.13f, centerY - pieceR * 0.53f);
										nvgLineTo(args.vg, centerX + pieceR * 0.13f, centerY - pieceR * 0.53f);
										NVGcolor crossInner = humanPiece ? nvgRGBA(246, 246, 252, strokeAlpha) : nvgRGBA(18, 18, 24, strokeAlpha);
										nvgStrokeColor(args.vg, crossInner);
										nvgStrokeWidth(args.vg, kingCrossInnerStroke);
										nvgStroke(args.vg);
										break;
									}
									}
									nvgRestore(args.vg);
									return;
								}

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

						// Checker-like stacked rim: outer annulus plus discrete radial ridge facets.
						float rimOuterR = radius * 1.01f;
						float rimInnerR = radius * 0.80f;
						NVGcolor rimBandColor = humanPiece ? nvgRGBA(112, 38, 30, int(128.f * alpha))
						                                   : nvgRGBA(26, 26, 32, int(146.f * alpha));
						nvgBeginPath(args.vg);
						nvgCircle(args.vg, centerX, centerY, rimOuterR);
						nvgCircle(args.vg, centerX, centerY, rimInnerR);
						nvgPathWinding(args.vg, NVG_HOLE);
						nvgFillColor(args.vg, rimBandColor);
						nvgFill(args.vg);

						const int ridgeCount = 32;
						float step = 2.f * float(M_PI) / float(ridgeCount);
						float ridgeSpan = step * 0.56f;
						float ridgeInnerR = radius * 0.86f;
						float ridgeOuterR = radius * 1.00f;
						for (int ridge = 0; ridge < ridgeCount; ++ridge) {
							float aMid = float(ridge) * step;
							float a0 = aMid - ridgeSpan * 0.5f;
							float a1 = aMid + ridgeSpan * 0.5f;
							float c0 = std::cos(a0);
							float s0 = std::sin(a0);
							float c1 = std::cos(a1);
							float s1 = std::sin(a1);
							float x0i = centerX + c0 * ridgeInnerR;
							float y0i = centerY + s0 * ridgeInnerR;
							float x1i = centerX + c1 * ridgeInnerR;
							float y1i = centerY + s1 * ridgeInnerR;
							float x1o = centerX + c1 * ridgeOuterR;
							float y1o = centerY + s1 * ridgeOuterR;
							float x0o = centerX + c0 * ridgeOuterR;
							float y0o = centerY + s0 * ridgeOuterR;

							NVGcolor ridgeFillA = humanPiece ? nvgRGBA(248, 176, 154, int(112.f * alpha))
							                                 : nvgRGBA(172, 172, 184, int(94.f * alpha));
							NVGcolor ridgeFillB = humanPiece ? nvgRGBA(198, 104, 86, int(104.f * alpha))
							                                 : nvgRGBA(112, 112, 122, int(86.f * alpha));
							NVGcolor ridgeStroke = humanPiece ? nvgRGBA(86, 24, 18, int(110.f * alpha))
							                                  : nvgRGBA(8, 8, 12, int(116.f * alpha));

							nvgBeginPath(args.vg);
							nvgMoveTo(args.vg, x0i, y0i);
							nvgLineTo(args.vg, x1i, y1i);
							nvgLineTo(args.vg, x1o, y1o);
							nvgLineTo(args.vg, x0o, y0o);
							nvgClosePath(args.vg);
							nvgFillColor(args.vg, (ridge & 1) ? ridgeFillA : ridgeFillB);
							nvgFill(args.vg);
							nvgStrokeColor(args.vg, ridgeStroke);
							nvgStrokeWidth(args.vg, 0.34f);
							nvgStroke(args.vg);
						}

						// Tie the rim together with thin contour rings.
						nvgBeginPath(args.vg);
						nvgCircle(args.vg, centerX, centerY, rimOuterR);
						nvgStrokeColor(args.vg, humanPiece ? nvgRGBA(255, 212, 196, int(60.f * alpha))
						                                  : nvgRGBA(196, 196, 206, int(46.f * alpha)));
						nvgStrokeWidth(args.vg, 0.70f);
						nvgStroke(args.vg);
						nvgBeginPath(args.vg);
						nvgCircle(args.vg, centerX, centerY, rimInnerR);
						nvgStrokeColor(args.vg, humanPiece ? nvgRGBA(70, 18, 14, int(82.f * alpha))
						                                  : nvgRGBA(6, 6, 10, int(96.f * alpha)));
						nvgStrokeWidth(args.vg, 0.64f);
						nvgStroke(args.vg);

						// Top sheen.
						nvgBeginPath(args.vg);
						nvgCircle(args.vg, centerX - radius * 0.20f, centerY - radius * 0.24f, radius * 0.34f);
						nvgFillColor(args.vg, nvgRGBA(255, 255, 255, int(36.f * alpha)));
						nvgFill(args.vg);

						if (crownstep::pieceIsKing(piece)) {
							// Ultra-simple crown: mostly gold, minimal edging, wider outer points.
							NVGcolor crownEdge = nvgRGBA(172, 126, 40, int(132.f * alpha));
							NVGcolor crownFillTop = nvgRGBA(255, 238, 172, int(246.f * alpha));
							NVGcolor crownFillBottom = nvgRGBA(236, 184, 70, int(244.f * alpha));
							float crownYOffset = radius * 0.07f;
							float leftX = centerX - radius * 0.56f;
							float rightX = centerX + radius * 0.56f;
							float bandTopY = centerY + radius * 0.05f + crownYOffset;
							float bandBottomY = centerY + radius * 0.24f + crownYOffset;
							float sideTipY = centerY - radius * 0.36f + crownYOffset;
							float centerTipY = centerY - radius * 0.55f + crownYOffset;
							float valleyY = centerY - radius * 0.08f + crownYOffset;
							float leftTipX = centerX - radius * 0.50f;
							float rightTipX = centerX + radius * 0.50f;
							float leftValleyX = centerX - radius * 0.18f;
							float rightValleyX = centerX + radius * 0.18f;

							nvgBeginPath(args.vg);
							nvgMoveTo(args.vg, leftX, bandTopY);
							nvgLineTo(args.vg, leftTipX, sideTipY);
							nvgLineTo(args.vg, leftValleyX, valleyY);
							nvgLineTo(args.vg, centerX, centerTipY);
							nvgLineTo(args.vg, rightValleyX, valleyY);
							nvgLineTo(args.vg, rightTipX, sideTipY);
							nvgLineTo(args.vg, rightX, bandTopY);
							nvgClosePath(args.vg);
							NVGpaint crownBodyPaint = nvgLinearGradient(
								args.vg, centerX, centerY - radius * 0.58f, centerX, bandBottomY, crownFillTop, crownFillBottom);
							nvgFillPaint(args.vg, crownBodyPaint);
							nvgFill(args.vg);
							nvgStrokeColor(args.vg, crownEdge);
							nvgStrokeWidth(args.vg, 0.62f);
							nvgStroke(args.vg);

							nvgBeginPath(args.vg);
							nvgRoundedRect(args.vg, leftX - radius * 0.03f, bandTopY, (rightX - leftX) + radius * 0.06f,
								bandBottomY - bandTopY, radius * 0.06f);
							nvgFillColor(args.vg, nvgRGBA(230, 174, 58, int(244.f * alpha)));
							nvgFill(args.vg);
							nvgStrokeColor(args.vg, crownEdge);
							nvgStrokeWidth(args.vg, 0.58f);
							nvgStroke(args.vg);

							// Three tip beads improve recognition at distance.
							for (int i = 0; i < 3; ++i) {
								float tipX = (i == 0) ? leftTipX : ((i == 1) ? centerX : rightTipX);
								float tipY = (i == 1) ? centerTipY : sideTipY;
								float beadR = radius * 0.07f;
								nvgBeginPath(args.vg);
								nvgCircle(args.vg, tipX, tipY, beadR);
								nvgFillColor(args.vg, nvgRGBA(255, 223, 120, int(246.f * alpha)));
								nvgFill(args.vg);
								nvgStrokeColor(args.vg, crownEdge);
								nvgStrokeWidth(args.vg, 0.55f);
								nvgStroke(args.vg);
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
					auto drawPieceAtScaled = [&](float centerX, float centerY, int piece, float alpha, float scaleX, float scaleY) {
						nvgSave(args.vg);
						nvgTranslate(args.vg, centerX, centerY);
						nvgScale(args.vg, scaleX, scaleY);
						nvgTranslate(args.vg, -centerX, -centerY);
						drawPieceAt(centerX, centerY, piece, alpha);
						nvgRestore(args.vg);
					};
					auto isActiveAnimatedCaptureIndex = [&](int index) {
						if (!module->isOthelloMode() || !module->moveAnimation.active) {
							return false;
						}
						for (int captureIndex : module->moveAnimation.capturedIndices) {
							if (captureIndex == index) {
								return true;
							}
						}
						return false;
					};

					const bool showMovablePieceHints = !module->gameOver && module->turnSide == module->humanSide();
					int cellCount = module->boardCellCount();
					std::vector<uint8_t> movableOrigins(size_t(cellCount), 0u);
					if (showMovablePieceHints) {
						for (const Move& move : module->humanMoves) {
							if (move.originIndex >= 0 && move.originIndex < cellCount) {
								movableOrigins[size_t(move.originIndex)] = 1u;
							}
						}
					}

					for (int i = 0; i < cellCount; ++i) {
						int piece = module->board[size_t(i)];
						if (piece == 0) {
							continue;
						}
						if (isActiveAnimatedCaptureIndex(i)) {
							continue;
						}
						if ((module->moveAnimation.active && i == module->moveAnimation.destinationIndex) || indexIsQueuedDestination(i)) {
							continue;
						}
						int row = 0;
						int col = 0;
						if (!module->boardIndexToCoord(i, &row, &col)) {
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
						if (!module->boardIndexToCoord(startIndex, &row, &col)) {
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
						if (!module->boardIndexToCoord(captureIndex, &row, &col)) {
							continue;
						}
						float centerX = (col + 0.5f) * cellWidth;
						float centerY = (row + 0.5f) * cellHeight;
						if (module->isOthelloMode()) {
							int flippedPiece = module->board[size_t(captureIndex)];
							if (flippedPiece == 0) {
								flippedPiece = -capturedPiece;
							}
							float flipStart = OTHELLO_FLIP_SECONDS_PER_PIECE * float(i);
							float flipEnd = flipStart + OTHELLO_FLIP_SECONDS_PER_PIECE;
							if (module->moveAnimation.elapsedSeconds <= flipStart) {
								drawPieceAt(centerX, centerY, capturedPiece, 1.f);
								continue;
							}
							if (module->moveAnimation.elapsedSeconds >= flipEnd) {
								drawPieceAt(centerX, centerY, flippedPiece, 1.f);
								continue;
							}
							float local = clamp(
								(module->moveAnimation.elapsedSeconds - flipStart) / OTHELLO_FLIP_SECONDS_PER_PIECE,
								0.f,
								1.f
							);
							float firstHalf = clamp(local * 2.f, 0.f, 1.f);
							float secondHalf = clamp((local - 0.5f) * 2.f, 0.f, 1.f);
							float widthScale = std::max(0.06f, std::fabs(std::cos(local * float(M_PI))));
							if (local < 0.5f) {
								drawPieceAtScaled(centerX, centerY, capturedPiece, 1.f - 0.2f * firstHalf, widthScale, 1.f);
							}
							else {
								drawPieceAtScaled(centerX, centerY, flippedPiece, 0.8f + 0.2f * secondHalf, widthScale, 1.f);
							}
						}
						else {
							float ghostAlpha = clamp(0.72f - t * 1.45f, 0.f, 0.72f);
							drawPieceAt(centerX, centerY, capturedPiece, ghostAlpha);
						}
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
					if (module->boardIndexToCoord(fromIndex, &fromRow, &fromCol) &&
						module->boardIndexToCoord(toIndex, &toRow, &toCol)) {
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
							if (destinationIndex < 0 || destinationIndex >= cellCount) {
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
						if (!module->boardIndexToCoord(destinationIndex, &row, &col)) {
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

						if (renderOpponentMoveHints) for (int destinationIndex : module->opponentHighlightedDestinations) {
							bool overlapsHuman = false;
						for (int humanDestination : module->highlightedDestinations) {
							if (humanDestination == destinationIndex) {
								overlapsHuman = true;
								break;
							}
						}
							if (overlapsHuman || destinationIndex < 0 || destinationIndex >= cellCount) {
								continue;
							}
						if (module->opponentHintsPreviewActive && module->board[size_t(destinationIndex)] != 0) {
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
						if (!module->boardIndexToCoord(destinationIndex, &row, &col)) {
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

					// Human-turn assist: ring pieces that have at least one legal move.
						if (showMovablePieceHints) {
							const bool selectedSquareGlowActive = !module->gameOver && module->selectedSquare >= 0;
							for (int i = 0; i < cellCount; ++i) {
								if (!movableOrigins[size_t(i)]) {
									continue;
								}
							if (selectedSquareGlowActive && i == module->selectedSquare) {
								continue;
							}
							int piece = module->board[size_t(i)];
							if (crownstep::pieceSide(piece) != module->humanSide()) {
								continue;
							}
						int row = 0;
						int col = 0;
						if (!module->boardIndexToCoord(i, &row, &col)) {
							continue;
						}
							float centerX = (col + 0.5f) * cellWidth;
							float centerY = (row + 0.5f) * cellHeight;
							float phase = float(i) * 0.37f + 0.4f;
							float pulse = 0.5f + 0.5f * std::sin(module->transportTimeSeconds * 4.6f + phase);
							float ringRadius = std::min(cellWidth, cellHeight) * (0.43f + 0.03f * pulse);

							nvgBeginPath(args.vg);
							nvgCircle(args.vg, centerX, centerY, ringRadius);
							nvgStrokeColor(args.vg, nvgRGBA(88, 240, 154, int(38.f + 44.f * pulse)));
							nvgStrokeWidth(args.vg, 3.6f);
							nvgStroke(args.vg);

							nvgBeginPath(args.vg);
							nvgCircle(args.vg, centerX, centerY, ringRadius);
							nvgStrokeColor(args.vg, nvgRGBA(98, 235, 154, int(182.f + 58.f * pulse)));
							nvgStrokeWidth(args.vg, 1.85f);
							nvgStroke(args.vg);
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

struct CrownstepStepCounterWidget final : Widget {
	Crownstep* module = nullptr;

	explicit CrownstepStepCounterWidget(Crownstep* crownstepModule) {
		module = crownstepModule;
	}

	void draw(const DrawArgs& args) override {
		if (!module) {
			return;
		}

		int totalSteps = module->activeLength();
		int currentStep = module->displayedStep;
		currentStep = clamp(currentStep, 0, totalSteps);

		char stepCounterText[64];
		std::snprintf(stepCounterText, sizeof(stepCounterText), "%d / %d", currentStep, totalSteps);

		const float x = mm2px(Vec(6.6f, 0.f)).x;
		const float y = mm2px(Vec(0.f, 4.0f)).y;
		const float w = mm2px(Vec(26.0f, 0.f)).x;
		const float h = mm2px(Vec(0.f, 4.5f)).y;

		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, x, y, w, h, 3.5f);
		nvgFillColor(args.vg, nvgRGBA(10, 12, 14, 186));
		nvgFill(args.vg);

		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, x + 0.5f, y + 0.5f, w - 1.f, h - 1.f, 3.0f);
		nvgStrokeColor(args.vg, nvgRGBA(236, 222, 198, 94));
		nvgStrokeWidth(args.vg, 1.0f);
		nvgStroke(args.vg);

		nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
		nvgFontSize(args.vg, 11.5f);
		nvgFillColor(args.vg, nvgRGB(242, 228, 204));
		nvgText(args.vg, x + w * 0.5f, y + h * 0.53f, stepCounterText, nullptr);
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
			CrownstepStepCounterWidget* stepCounterWidget = new CrownstepStepCounterWidget(module);
			stepCounterWidget->box = box.zeroPos();
			addChild(stepCounterWidget);

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
		Vec humanLightPos(82.5f, 93.f);
		Vec aiLightPos(86.5f, 93.f);

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

		// Prefer explicit component anchors from the SVG "components" layer.
		auto applyPointOverride = [&](const char* elementId, Vec* outPos) {
			Vec pointMm;
			if (loadPointFromSvgMm(panelPath, elementId, &pointMm)) {
				*outPos = pointMm;
			}
		};
		applyPointOverride("SEQ_LENGTH_PARAM", &seqPos);
		applyPointOverride("NEW_GAME_PARAM", &newGamePos);
		applyPointOverride("CLOCK_INPUT", &clockPos);
		applyPointOverride("RESET_INPUT", &resetPos);
		applyPointOverride("TRANSPOSE_INPUT", &transposePos);
		applyPointOverride("ROOT_INPUT", &rootCvPos);
		applyPointOverride("PITCH_OUTPUT", &pitchPos);
		applyPointOverride("ACCENT_OUTPUT", &accentPos);
		applyPointOverride("MOD_OUTPUT", &modPos);
		applyPointOverride("EOC_OUTPUT", &eocPos);
		applyPointOverride("HUMAN_TURN_LIGHT", &humanLightPos);
		applyPointOverride("AI_TURN_LIGHT", &aiLightPos);

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

		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(humanLightPos), module, Crownstep::HUMAN_TURN_LIGHT));
		addChild(createLightCentered<SmallLight<BlueLight>>(mm2px(aiLightPos), module, Crownstep::AI_TURN_LIGHT));
	}

	void step() override {
		ModuleWidget::step();
		Crownstep* crownstepModule = dynamic_cast<Crownstep*>(module);
		if (crownstepModule) {
			crownstepModule->serviceAiTurnFromUiThread();
		}
	}

	void appendContextMenu(Menu* menu) override {
		ModuleWidget::appendContextMenu(menu);
		Crownstep* module = dynamic_cast<Crownstep*>(this->module);
		menu->addChild(new MenuSeparator());
		MenuLabel* gameLabel = new MenuLabel();
		gameLabel->text = "Game";
		menu->addChild(gameLabel);
		menu->addChild(createSubmenuItem("Mode", "", [=](Menu* gameMenu) {
			for (int i = 0; i < int(GAME_MODE_NAMES.size()); ++i) {
				gameMenu->addChild(createCheckMenuItem(
					GAME_MODE_NAMES[size_t(i)],
					"",
					[=]() {
						return module && module->gameMode == i;
					},
					[=]() {
						if (module) {
							module->setGameMode(i, true);
						}
					}
				));
			}
		}));
		menu->addChild(createSubmenuItem("AI Difficulty", "", [=](Menu* difficultyMenu) {
			for (int i = 0; i < int(DIFFICULTY_NAMES.size()); ++i) {
				CrownstepDifficultyItem* item = new CrownstepDifficultyItem();
				item->text = DIFFICULTY_NAMES[size_t(i)];
				item->module = module;
				item->difficulty = i;
				difficultyMenu->addChild(item);
			}
		}));
		menu->addChild(new MenuSeparator());
		MenuLabel* quantizerLabel = new MenuLabel();
		quantizerLabel->text = "Quantizer";
		menu->addChild(quantizerLabel);
		menu->addChild(createCheckMenuItem(
			"Enable Quantization",
			"",
			[=]() {
				return module && module->quantizationEnabled;
			},
			[=]() {
				if (module) {
					module->quantizationEnabled = !module->quantizationEnabled;
				}
			}
		));
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
		menu->addChild(new MenuSeparator());
		MenuLabel* pitchLabel = new MenuLabel();
		pitchLabel->text = "Pitch";
		menu->addChild(pitchLabel);
		menu->addChild(createCheckMenuItem(
			"Bipolar",
			"",
			[=]() {
				return module && module->pitchBipolarEnabled;
			},
			[=]() {
				if (module) {
					module->pitchBipolarEnabled = !module->pitchBipolarEnabled;
				}
			}
		));
		menu->addChild(createSubmenuItem("Pitch Data Source", "", [=](Menu* interpretationMenu) {
			for (int i = 0; i < int(PITCH_INTERPRETATION_NAMES.size()); ++i) {
				interpretationMenu->addChild(createCheckMenuItem(
					PITCH_INTERPRETATION_NAMES[size_t(i)],
					"",
					[=]() {
						return module && module->pitchInterpretationMode == i;
					},
					[=]() {
						if (module) {
							module->pitchInterpretationMode = i;
						}
					}
				));
			}
		}));
		menu->addChild(createSubmenuItem("Board Value Layout", "", [=](Menu* valueLayoutMenu) {
			for (int i = 0; i < int(BOARD_VALUE_LAYOUT_NAMES.size()); ++i) {
				valueLayoutMenu->addChild(createCheckMenuItem(
					BOARD_VALUE_LAYOUT_NAMES[size_t(i)],
					"",
					[=]() {
						return module && module->boardValueLayoutMode == i;
					},
					[=]() {
						if (module) {
							module->boardValueLayoutMode = i;
						}
					}
				));
			}
		}));
		menu->addChild(createSubmenuItem("Divider", "", [=](Menu* dividerMenu) {
			for (int i = 0; i < int(crownstep::PITCH_DIVIDER_NAMES.size()); ++i) {
				dividerMenu->addChild(createCheckMenuItem(
					crownstep::PITCH_DIVIDER_NAMES[size_t(i)],
					"",
					[=]() {
						return module && module->pitchDividerMode == i;
					},
					[=]() {
						if (module) {
							module->pitchDividerMode = i;
						}
					}
				));
			}
		}));
		menu->addChild(new MenuSeparator());
		MenuLabel* boardLabel = new MenuLabel();
		boardLabel->text = "Board";
		menu->addChild(boardLabel);
		menu->addChild(createSubmenuItem("Texture", "", [=](Menu* textureMenu) {
			for (int i = 0; i < int(BOARD_TEXTURE_NAMES.size()); ++i) {
				textureMenu->addChild(createCheckMenuItem(
					BOARD_TEXTURE_NAMES[size_t(i)],
					"",
					[=]() {
						return module && module->boardTextureMode == i;
					},
					[=]() {
						if (module) {
							module->boardTextureMode = i;
						}
					}
				));
			}
		}));
	}
};

Model* modelCrownstep = createModel<Crownstep, CrownstepWidget>("Crownstep");
