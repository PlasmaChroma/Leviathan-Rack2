#pragma once

#include "plugin.hpp"
#include "CrownstepCore.hpp"

#include <cstdio>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <exception>
#include <mutex>
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
