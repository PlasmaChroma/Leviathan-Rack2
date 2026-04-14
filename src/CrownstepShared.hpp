#pragma once

#include "plugin.hpp"
#include "CrownstepCore.hpp"

#include <cstdio>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <exception>
#include <atomic>
#include <array>
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
static constexpr float EOC_ACTIVITY_PULSE_SECONDS = 0.25f;
static constexpr float OTHELLO_FLIP_SECONDS_PER_PIECE = 0.1f;
static constexpr int ROOT_CV_MAX_OFFSET_SEMITONES = 10;
static constexpr float ROOT_CV_VOLTS_PER_SEMITONE = 1.f;
static constexpr float TRANSPOSE_CV_ZERO_DEADBAND_VOLTS = 1e-3f;
static constexpr int SEQ_LENGTH_MIN = 1;
static constexpr int SEQ_LENGTH_MAX = 64;
static constexpr std::array<const char*, 4> BOARD_TEXTURE_NAMES = {{"Wood", "Marble", "Fabric", "Red/Black"}};
static constexpr std::array<const char*, 3> GAME_MODE_NAMES = {{"Checkers", "Chess", "Reversi"}};
static constexpr std::array<const char*, 4> HIGHLIGHT_MODE_NAMES = {{"Purple", "Cyan", "Green", "Off"}};
static constexpr std::array<const char*, 2> PLAYER_MODE_NAMES = {{"Cause", "Effect"}};
static constexpr std::array<const char*, 2> STEP_COUNTER_STYLE_NAMES = {{"Ribbon", "Basic"}};

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
		DEBUG_ADD_MOVES_PARAM,
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
		BOARD_TEXTURE_FABRIC,
		BOARD_TEXTURE_RED_BLACK,
		BOARD_TEXTURE_COUNT
	};
	enum GameMode {
		GAME_MODE_CHECKERS = 0,
		GAME_MODE_CHESS,
		GAME_MODE_OTHELLO,
		GAME_MODE_COUNT
	};
	enum HighlightMode {
		HIGHLIGHT_PURPLE = 0,
		HIGHLIGHT_CYAN,
		HIGHLIGHT_GREEN,
		HIGHLIGHT_OFF,
		HIGHLIGHT_COUNT
	};
	enum PlayerMode {
		PLAYER_INIT = 0,
		PLAYER_FOLLOW,
		PLAYER_MODE_COUNT
	};
	enum StepCounterStyle {
		STEP_COUNTER_RIBBON = 0,
		STEP_COUNTER_BASIC,
		STEP_COUNTER_STYLE_COUNT
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
	dsp::SchmittTrigger debugAddMovesTrigger;
	bool eocGateHigh = false;
	std::atomic<int> eocActivityPulseRequests {0};
	int eocActivityPulseQueued = 0;
	float eocActivityPulseRemainingSeconds = 0.f;

	int selectedSquare = -1;
	int hoveredSquare = -1;
	int turnSide = HUMAN_SIDE;
	int winnerSide = 0;
	int lastMoveSide = 0;
	int aiDifficulty = 1;
	bool quantizationEnabled = true;
	bool pitchBipolarEnabled = false;
	bool melodicBiasEnabled = false;
	int pitchInterpretationMode = 0;
	int boardValueLayoutMode = 0;
	uint32_t boardValueRandomSeed = 1u;
	bool boardValueLayoutInverted = false;
	int pitchDividerMode = 0;
	bool showCellPitchOverlay = false;
	int boardTextureMode = BOARD_TEXTURE_WOOD;
	int gameMode = GAME_MODE_CHECKERS;
	int highlightMode = HIGHLIGHT_GREEN;
	int playerMode = PLAYER_INIT;
	int stepCounterStyle = STEP_COUNTER_RIBBON;
	int sequenceCapOverride = -1; // -1: use knob, 0: full, >0: explicit recent-window cap
	bool opponentHintsPreviewActive = false;
	int playhead = 0;
	int displayedStep = 0;
	double transportTimeSeconds = 0.0;
	float heldPitch = NO_SEQUENCE_PITCH_VOLTS;
	float heldAccent = 0.f;
	float heldMod = 0.f;
	float modOutputVolts = 0.f;
	double aiTurnDelayStartSeconds = 0.0;
	double uiLastServiceSeconds = 0.0;
	float captureFlashSeconds = 0.f;
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
		int aiSide = AI_SIDE;
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


	Crownstep();
	~Crownstep() override;

	bool isChessMode() const;
	bool isOthelloMode() const;

	static Move chooseAiMoveForSnapshot(const AiWorkerRequest& request);

	void runAiWorkerLoop();
	void startAiWorker();
	void stopAiWorker();
	void cancelAiTurnWork();
	void clearAiWorkerQueueState();
	bool consumeReadyAiResult(Move* outMove);
	bool dispatchAiRequestIfIdle();

	void setGameMode(int mode, bool startFreshGame);

	void resetMoveAnimation();
	void beginMoveAnimation(const Move& move, const BoardState& beforeBoard, int moverSide);

	void onReset() override;
	void resetPlayback();
	void armDelayedAiTurnAfterHumanMove();
	void advanceUiAnimationClock(double nowSeconds);

	int currentSequenceCap();
	int currentScaleIndex();
	float transposeVolts();
	int rootCvOffsetSemitone();
	int rootSemitoneLinear();
	int rootSemitone();

	int humanSide() const;
	int aiSide() const;
	int opposingSide(int side) const;

	bool boardIndexToCoord(int index, int* row, int* col) const;
	int boardCoordToIndex(int row, int col) const;
	int boardCellCount() const;
	float boardValueIndexForMove(const Move& move);
	float mapPitchFromBoardValueIndex(float boardValueIndex, bool isKing);
	float applyMelodicBiasToBoardValueIndex(float previousBoardValueIndex, float currentBoardValueIndex, const Move& move) const;

	float pitchPreviewForBoardIndex(int boardIndex);
	float pitchForMove(const Move& move);
	Step makeStepFromMove(const Move& move);
	void appendDebugRandomMoves(int count = 10);

	void startNewGame();
	void setHoveredSquare(int index);
	void refreshLegalMoves();
	void advanceForcedPassesIfNeeded();

	int searchDepthForDifficulty() const;
	float expressiveModForMove(const Move& move, const BoardState& beforeBoard, const BoardState& afterBoard, int moverSide) const;
	void commitMove(const Move& move, int moverSide);

	void serviceAiTurnFromUiThread();
	void onBoardSquarePressed(int index);

	int activeLength();
	int activeStartIndex();
	float pitchForSequenceIndex(int sequenceIndex);
	void refreshHeldPitchForCurrentStep();
	void randomizeBoardValueLayout();
	void emitStepAtClockEdge();

	void process(const ProcessArgs& args) override;

	json_t* dataToJson() override;
	void dataFromJson(json_t* rootJ) override;

private:
	std::array<int, crownstep::MAX_BOARD_SIZE> randomBoardValueMap {};
	uint32_t randomBoardValueMapSeed = 0u;
	int randomBoardValueMapCellCount = -1;
	void rebuildRandomBoardValueMap();
	float randomBoardValueForSampledIndex(float sampledIndex);
};
