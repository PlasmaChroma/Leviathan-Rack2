#include "CrownstepShared.hpp"

Crownstep::Crownstep() {
	config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

	configParam<CrownstepSeqLengthQuantity>(
		SEQ_LENGTH_PARAM, float(SEQ_LENGTH_MIN), float(SEQ_LENGTH_MAX), float(SEQ_LENGTH_MAX), "Sequence length");
	configParam<CrownstepRootQuantity>(ROOT_PARAM, 0.f, 11.f, 0.f, "Bias");
	configParam<CrownstepScaleQuantity>(SCALE_PARAM, 0.f, float(SCALES.size() - 1), 0.f, "Scale");
	configParam(RUN_PARAM, 0.f, 1.f, 1.f, "Run");
	configParam(NEW_GAME_PARAM, 0.f, 1.f, 0.f, "New game");

	paramQuantities[SEQ_LENGTH_PARAM]->snapEnabled = true;
	paramQuantities[ROOT_PARAM]->snapEnabled = true;
	paramQuantities[SCALE_PARAM]->snapEnabled = true;

	configInput(CLOCK_INPUT, "Clock");
	configInput(RESET_INPUT, "Reset");
	configInput(TRANSPOSE_INPUT, "Transpose");
	configInput(ROOT_INPUT, "Bias");

	configOutput(PITCH_OUTPUT, "Pitch");
	configOutput(ACCENT_OUTPUT, "Accent");
	configOutput(MOD_OUTPUT, "Mod");
	configOutput(EOC_OUTPUT, "End of cycle");

	startAiWorker();
	setGameMode(GAME_MODE_CHECKERS, true);
}

Crownstep::~Crownstep() {
	stopAiWorker();
}

bool Crownstep::isChessMode() const {
	return gameRules && std::strcmp(gameRules->gameId(), "chess") == 0;
}

bool Crownstep::isOthelloMode() const {
	return gameRules && std::strcmp(gameRules->gameId(), "othello") == 0;
}

Move Crownstep::chooseAiMoveForSnapshot(const AiWorkerRequest& request) {
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

void Crownstep::runAiWorkerLoop() {
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

void Crownstep::startAiWorker() {
	std::lock_guard<std::mutex> lock(aiWorkerMutex);
	if (aiWorkerThread.joinable()) {
		return;
	}
	aiWorkerStopRequested = false;
	aiWorkerThread = std::thread([this]() {
		runAiWorkerLoop();
	});
}

void Crownstep::stopAiWorker() {
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

void Crownstep::cancelAiTurnWork() {
	aiTurnDelayPending = false;
	aiTurnDelayActive = false;
	aiTurnDelayStartSeconds = 0.f;
	std::lock_guard<std::mutex> lock(aiWorkerMutex);
	aiWorkerHasRequest = false;
	aiWorkerHasResult = false;
	aiWorkerInFlightRequestId = 0;
}

void Crownstep::clearAiWorkerQueueState() {
	std::lock_guard<std::mutex> lock(aiWorkerMutex);
	aiWorkerHasRequest = false;
	aiWorkerHasResult = false;
	aiWorkerInFlightRequestId = 0;
}

bool Crownstep::consumeReadyAiResult(Move* outMove) {
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

bool Crownstep::dispatchAiRequestIfIdle() {
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

void Crownstep::setGameMode(int mode, bool startFreshGame) {
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

void Crownstep::resetMoveAnimation() {
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

void Crownstep::beginMoveAnimation(const Move& move, const BoardState& beforeBoard, int moverSide) {
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

void Crownstep::onReset() {
	resetPlayback();
}

void Crownstep::resetPlayback() {
	playhead = 0;
	displayedStep = 0;
	heldPitch = NO_SEQUENCE_PITCH_VOLTS;
	heldAccent = 0.f;
	heldMod = 0.f;
	modOutputVolts = 0.f;
	cachedRootSemitoneValid = false;
	cancelAiTurnWork();
	resetMoveAnimation();
}

void Crownstep::armDelayedAiTurnAfterHumanMove() {
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

void Crownstep::advanceUiAnimationClock(double nowSeconds) {
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

int Crownstep::currentSequenceCap() {
	int requested = clamp(int(std::round(params[SEQ_LENGTH_PARAM].getValue())), SEQ_LENGTH_MIN, SEQ_LENGTH_MAX);
	// Max knob turn means full history window.
	if (requested >= SEQ_LENGTH_MAX) {
		return 0;
	}
	return requested;
}

int Crownstep::currentScaleIndex() {
	return clamp(int(std::round(params[SCALE_PARAM].getValue())), 0, int(SCALES.size()) - 1);
}

float Crownstep::transposeVolts() {
	float value = clamp(inputs[TRANSPOSE_INPUT].getVoltage(), -10.f, 10.f);
	if (std::fabs(value) < TRANSPOSE_CV_ZERO_DEADBAND_VOLTS) {
		return 0.f;
	}
	return value;
}

int Crownstep::rootCvOffsetSemitone() {
	float rootCv = clamp(inputs[ROOT_INPUT].getVoltage(), -10.f, 10.f);
	// Semitone-domain mapping with direct CV anchors:
	// -10V -> -10 semitones, 0V -> 0 semitones, +10V -> +10 semitones.
	int cvOffsetSemitones = int(std::lround(rootCv / ROOT_CV_VOLTS_PER_SEMITONE));
	cvOffsetSemitones = clamp(cvOffsetSemitones, -ROOT_CV_MAX_OFFSET_SEMITONES, ROOT_CV_MAX_OFFSET_SEMITONES);
	return cvOffsetSemitones;
}

int Crownstep::rootSemitoneLinear() {
	int knobSemitone = clamp(int(std::round(params[ROOT_PARAM].getValue())), 0, 11);
	int cvOffsetSemitones = rootCvOffsetSemitone();
	return knobSemitone + cvOffsetSemitones;
}

int Crownstep::rootSemitone() {
	int total = rootSemitoneLinear();
	return crownstep::wrapSemitone12(total);
}

int Crownstep::humanSide() const {
	return gameRules ? gameRules->humanSide() : HUMAN_SIDE;
}

int Crownstep::aiSide() const {
	return gameRules ? gameRules->aiSide() : AI_SIDE;
}

int Crownstep::opposingSide(int side) const {
	if (side == humanSide()) {
		return aiSide();
	}
	if (side == aiSide()) {
		return humanSide();
	}
	return -side;
}

bool Crownstep::boardIndexToCoord(int index, int* row, int* col) const {
	if (gameRules) {
		return gameRules->indexToCoord(index, row, col);
	}
	return crownstep::indexToCoord(index, row, col);
}

int Crownstep::boardCoordToIndex(int row, int col) const {
	if (gameRules) {
		return gameRules->coordToIndex(row, col);
	}
	return crownstep::coordToIndex(row, col);
}

int Crownstep::boardCellCount() const {
	int localCount = int(board.size());
	int rulesCount = gameRules ? gameRules->boardCellCount() : localCount;
	return std::max(0, std::min(localCount, rulesCount));
}

float Crownstep::pitchForMove(const Move& move) {
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
	// Bias acts as a raw index-domain offset on the board-derived value.
	boardValueIndex += float(rootSemitoneLinear());
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
			0,
			transposeVolts()
		);
	}
	return crownstep::mapRawPitchFromIndex(boardValueIndex, move.isKing, transposeVolts());
}

Step Crownstep::makeStepFromMove(const Move& move) {
	Step step = crownstep::makeStepFromMove(move, currentScaleIndex(), rootSemitone(), transposeVolts());
	step.pitch = pitchForMove(move);
	if (isOthelloMode()) {
		// Reversi accent: 1 flipped disc is baseline (0V), then +0.5V per extra flipped disc.
		int flipped = std::max(0, int(move.captured.size()));
		step.accent = 0.5f * float(std::max(0, flipped - 1));
	}
	return step;
}

void Crownstep::startNewGame() {
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

void Crownstep::setHoveredSquare(int index) {
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

void Crownstep::refreshLegalMoves() {
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

void Crownstep::advanceForcedPassesIfNeeded() {
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

int Crownstep::searchDepthForDifficulty() const {
	return gameRules ? gameRules->searchDepthForDifficulty(aiDifficulty) : crownstep::searchDepthForDifficulty(aiDifficulty);
}

float Crownstep::expressiveModForMove(
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

void Crownstep::commitMove(const Move& move, int moverSide) {
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
	if (isChessMode()) {
		float captureAccent = 0.f;
		for (int captureIndex : move.captured) {
			if (captureIndex < 0 || captureIndex >= crownstep::CHESS_BOARD_SIZE) {
				continue;
			}
			int capturedPiece = beforeBoard[size_t(captureIndex)];
			if (capturedPiece == 0) {
				continue;
			}
			switch (crownstep::chessPieceType(capturedPiece)) {
				case crownstep::CHESS_PAWN:
					captureAccent += 1.f;
					break;
				case crownstep::CHESS_KNIGHT:
				case crownstep::CHESS_BISHOP:
					captureAccent += 3.f;
					break;
				case crownstep::CHESS_ROOK:
					captureAccent += 5.f;
					break;
				case crownstep::CHESS_QUEEN:
					captureAccent += 9.f;
					break;
				default:
					break;
			}
		}
		step.accent = captureAccent;
	}
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
void Crownstep::serviceAiTurnFromUiThread() {
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

void Crownstep::onBoardSquarePressed(int index) {
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
