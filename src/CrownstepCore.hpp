#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <vector>

namespace crownstep {

static constexpr int BOARD_SIZE = 32;
static constexpr int HUMAN_SIDE = 1;
static constexpr int AI_SIDE = -1;

struct Scale {
	const char* name;
	std::array<int, 12> semitones;
	int length;

	Scale(const char* scaleName, std::array<int, 12> scaleSemitones, int scaleLength)
		: name(scaleName), semitones(scaleSemitones), length(scaleLength) {
	}
};

static constexpr std::array<const char*, 12> KEY_NAMES = {{"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"}};

static const std::array<Scale, 13> SCALES = {{
	{"Major", {0, 2, 4, 5, 7, 9, 11}, 7},
	{"Minor", {0, 2, 3, 5, 7, 8, 10}, 7},
	{"Harmonic Minor", {0, 2, 3, 5, 7, 8, 11}, 7},
	{"Melodic Minor", {0, 2, 3, 5, 7, 9, 11}, 7},
	{"Dorian", {0, 2, 3, 5, 7, 9, 10}, 7},
	{"Phrygian", {0, 1, 3, 5, 7, 8, 10}, 7},
	{"Lydian", {0, 2, 4, 6, 7, 9, 11}, 7},
	{"Mixolydian", {0, 2, 4, 5, 7, 9, 10}, 7},
	{"Locrian", {0, 1, 3, 5, 6, 8, 10}, 7},
	{"Major Pentatonic", {0, 2, 4, 7, 9}, 5},
	{"Minor Pentatonic", {0, 3, 5, 7, 10}, 5},
	{"Blues", {0, 3, 5, 6, 7, 10}, 6},
	{"Chromatic", {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}, 12},
}};

static constexpr std::array<int, 5> SEQ_CAPS = {{0, 8, 16, 32, 64}};
static constexpr std::array<const char*, 5> SEQ_CAP_NAMES = {{"Full", "8", "16", "32", "64"}};
static constexpr std::array<const char*, 3> DIFFICULTY_NAMES = {{"Easy", "Normal", "Hard"}};
static constexpr std::array<const char*, 3> PITCH_INTERPRETATION_NAMES = {
	{"Origin Square", "Destination Square", "Blend (O+D)/2"}
};
static constexpr std::array<const char*, 3> BOARD_VALUE_LAYOUT_NAMES = {
	{"Linear", "Serpentine Rows", "Center-Out"}
};
static constexpr std::array<const char*, 4> PITCH_DIVIDER_NAMES = {
	{"Full", "Half", "Third", "Quarter"}
};
static constexpr std::array<float, 4> PITCH_DIVIDER_VALUES = {
	{1.f, 2.f, 3.f, 4.f}
};

struct Move {
	int originIndex = -1;
	int destinationIndex = -1;
	std::vector<int> path;
	std::vector<int> captured;
	bool isCapture = false;
	bool isMultiCapture = false;
	bool isKing = false;
};

struct Step {
	float pitch = 0.f;
	bool gate = true;
	float accent = 0.f;
	float mod = 0.f;
};

inline int pieceSide(int piece) {
	if (piece > 0) {
		return HUMAN_SIDE;
	}
	if (piece < 0) {
		return AI_SIDE;
	}
	return 0;
}

inline bool pieceIsKing(int piece) {
	return std::abs(piece) == 2;
}

inline int makeKingForSide(int side) {
	return (side > 0) ? 2 : -2;
}

inline bool indexToCoord(int index, int* row, int* col) {
	if (index < 0 || index >= BOARD_SIZE) {
		return false;
	}
	int r = index / 4;
	int c = (index % 4) * 2 + ((r + 1) % 2);
	if (row) {
		*row = r;
	}
	if (col) {
		*col = c;
	}
	return true;
}

inline int coordToIndex(int row, int col) {
	if (row < 0 || row >= 8 || col < 0 || col >= 8) {
		return -1;
	}
	if (((row + col) & 1) == 0) {
		return -1;
	}
	return row * 4 + col / 2;
}

inline std::array<int, BOARD_SIZE> makeInitialBoard() {
	std::array<int, BOARD_SIZE> board {};
	for (int i = 0; i < BOARD_SIZE; ++i) {
		int row = 0;
		int col = 0;
		indexToCoord(i, &row, &col);
		if (row <= 2) {
			board[i] = -1;
		}
		else if (row >= 5) {
			board[i] = 1;
		}
	}
	return board;
}

inline int wrapSemitone12(int value) {
	int wrapped = value % 12;
	if (wrapped < 0) {
		wrapped += 12;
	}
	return wrapped;
}

inline float clampf(float value, float lo, float hi) {
	return std::max(lo, std::min(value, hi));
}

inline float normalizedMoveMod(const Move& move) {
	float value = 0.2f;
	if (move.isCapture) {
		value += 0.3f;
	}
	if (move.isMultiCapture) {
		value += 0.3f;
	}
	if (move.isKing) {
		value += 0.4f;
	}
	return clampf(value, 0.f, 1.f);
}

inline float moveAccent(const Move& move) {
	if (move.isMultiCapture) {
		return 2.f;
	}
	if (move.isCapture) {
		return 1.f;
	}
	if (move.isKing) {
		return 1.5f;
	}
	return 0.f;
}

inline bool isPromotionSquare(int side, int index) {
	int row = 0;
	indexToCoord(index, &row, nullptr);
	return (side == HUMAN_SIDE) ? (row == 0) : (row == 7);
}

inline std::vector<int> movementDirectionsForPiece(int piece, bool capture) {
	std::vector<int> directions;
	int side = pieceSide(piece);
	if (pieceIsKing(piece) || side == HUMAN_SIDE) {
		directions.push_back(capture ? -2 : -1);
	}
	if (pieceIsKing(piece) || side == AI_SIDE) {
		directions.push_back(capture ? 2 : 1);
	}
	return directions;
}

inline void addSimpleMovesForPiece(const std::array<int, BOARD_SIZE>& sourceBoard, int index, std::vector<Move>* moves) {
	if (!moves) {
		return;
	}
	int piece = sourceBoard[size_t(index)];
	if (piece == 0) {
		return;
	}
	int row = 0;
	int col = 0;
	indexToCoord(index, &row, &col);
	for (int dr : movementDirectionsForPiece(piece, false)) {
		for (int dc : {-1, 1}) {
			int dest = coordToIndex(row + dr, col + dc);
			if (dest < 0 || sourceBoard[size_t(dest)] != 0) {
				continue;
			}
			Move move;
			move.originIndex = index;
			move.destinationIndex = dest;
			move.path.push_back(dest);
			move.isKing = (!pieceIsKing(piece) && isPromotionSquare(pieceSide(piece), dest));
			moves->push_back(move);
		}
	}
}

inline void collectCapturesRecursive(
	const std::array<int, BOARD_SIZE>& sourceBoard,
	int originIndex,
	int currentIndex,
	int currentPiece,
	std::vector<int> path,
	std::vector<int> captured,
	std::vector<Move>* outMoves
) {
	if (!outMoves) {
		return;
	}
	int row = 0;
	int col = 0;
	indexToCoord(currentIndex, &row, &col);
	bool foundChild = false;

	for (int dr : movementDirectionsForPiece(currentPiece, true)) {
		for (int dc : {-2, 2}) {
			int jumpedIndex = coordToIndex(row + dr / 2, col + dc / 2);
			int destIndex = coordToIndex(row + dr, col + dc);
			if (jumpedIndex < 0 || destIndex < 0) {
				continue;
			}
			int jumpedPiece = sourceBoard[size_t(jumpedIndex)];
			if (jumpedPiece == 0 || pieceSide(jumpedPiece) == pieceSide(currentPiece) || sourceBoard[size_t(destIndex)] != 0) {
				continue;
			}

			foundChild = true;
			std::array<int, BOARD_SIZE> nextBoard = sourceBoard;
			nextBoard[size_t(currentIndex)] = 0;
			nextBoard[size_t(jumpedIndex)] = 0;

			int movedPiece = currentPiece;
			bool promoted = false;
			if (!pieceIsKing(currentPiece) && isPromotionSquare(pieceSide(currentPiece), destIndex)) {
				movedPiece = makeKingForSide(pieceSide(currentPiece));
				promoted = true;
			}
			nextBoard[size_t(destIndex)] = movedPiece;

			std::vector<int> nextPath = path;
			nextPath.push_back(destIndex);
			std::vector<int> nextCaptured = captured;
			nextCaptured.push_back(jumpedIndex);

			if (promoted) {
				Move move;
				move.originIndex = originIndex;
				move.destinationIndex = destIndex;
				move.path = nextPath;
				move.captured = nextCaptured;
				move.isCapture = true;
				move.isMultiCapture = nextCaptured.size() > 1;
				move.isKing = true;
				outMoves->push_back(move);
			}
			else {
				collectCapturesRecursive(nextBoard, originIndex, destIndex, movedPiece, nextPath, nextCaptured, outMoves);
			}
		}
	}

	if (!foundChild && !captured.empty()) {
		Move move;
		move.originIndex = originIndex;
		move.destinationIndex = currentIndex;
		move.path = path;
		move.captured = captured;
		move.isCapture = true;
		move.isMultiCapture = captured.size() > 1;
		move.isKing = pieceIsKing(currentPiece);
		outMoves->push_back(move);
	}
}

inline std::vector<Move> generateLegalMovesForSide(const std::array<int, BOARD_SIZE>& sourceBoard, int side) {
	std::vector<Move> captures;
	std::vector<Move> simpleMoves;

	for (int i = 0; i < BOARD_SIZE; ++i) {
		int piece = sourceBoard[size_t(i)];
		if (piece == 0 || pieceSide(piece) != side) {
			continue;
		}
		collectCapturesRecursive(sourceBoard, i, i, piece, {}, {}, &captures);
	}
	if (!captures.empty()) {
		return captures;
	}
	for (int i = 0; i < BOARD_SIZE; ++i) {
		int piece = sourceBoard[size_t(i)];
		if (piece == 0 || pieceSide(piece) != side) {
			continue;
		}
		addSimpleMovesForPiece(sourceBoard, i, &simpleMoves);
	}
	return simpleMoves;
}

inline std::array<int, BOARD_SIZE> applyMoveToBoard(const std::array<int, BOARD_SIZE>& sourceBoard, const Move& move) {
	std::array<int, BOARD_SIZE> nextBoard = sourceBoard;
	if (move.originIndex < 0 || move.originIndex >= BOARD_SIZE || move.destinationIndex < 0 || move.destinationIndex >= BOARD_SIZE) {
		return nextBoard;
	}
	int movingPiece = nextBoard[size_t(move.originIndex)];
	nextBoard[size_t(move.originIndex)] = 0;
	for (int captureIndex : move.captured) {
		if (captureIndex >= 0 && captureIndex < BOARD_SIZE) {
			nextBoard[size_t(captureIndex)] = 0;
		}
	}
	if (move.isKing && !pieceIsKing(movingPiece)) {
		movingPiece = makeKingForSide(pieceSide(movingPiece));
	}
	nextBoard[size_t(move.destinationIndex)] = movingPiece;
	return nextBoard;
}

inline int evaluateBoardMaterial(const std::array<int, BOARD_SIZE>& board) {
	int score = 0;
	for (int piece : board) {
		switch (piece) {
			case 1: score -= 100; break;
			case 2: score -= 175; break;
			case -1: score += 100; break;
			case -2: score += 175; break;
			default: break;
		}
	}
	return score;
}

inline int countMobilityScore(const std::vector<Move>& moves) {
	return int(moves.size()) * 6;
}

inline int countCapturePressure(const std::vector<Move>& moves) {
	int score = 0;
	for (const Move& move : moves) {
		if (move.isCapture) {
			score += 18;
		}
		if (move.isMultiCapture) {
			score += 10;
		}
		if (move.isKing) {
			score += 12;
		}
	}
	return score;
}

inline int evaluatePosition(const std::array<int, BOARD_SIZE>& sourceBoard) {
	std::vector<Move> nextAiMoves = generateLegalMovesForSide(sourceBoard, AI_SIDE);
	std::vector<Move> nextHumanMoves = generateLegalMovesForSide(sourceBoard, HUMAN_SIDE);
	int score = evaluateBoardMaterial(sourceBoard);
	score += countMobilityScore(nextAiMoves);
	score -= countMobilityScore(nextHumanMoves);
	score += countCapturePressure(nextAiMoves);
	score -= countCapturePressure(nextHumanMoves);
	if (nextHumanMoves.empty()) {
		score += 10000;
	}
	if (nextAiMoves.empty()) {
		score -= 10000;
	}
	return score;
}

inline int searchScore(const std::array<int, BOARD_SIZE>& sourceBoard, int sideToMove, int depth, int alpha, int beta) {
	std::vector<Move> moves = generateLegalMovesForSide(sourceBoard, sideToMove);
	if (depth <= 0 || moves.empty()) {
		int score = evaluatePosition(sourceBoard);
		return (sideToMove == AI_SIDE) ? score : -score;
	}
	int best = std::numeric_limits<int>::min();
	for (const Move& move : moves) {
		std::array<int, BOARD_SIZE> nextBoard = applyMoveToBoard(sourceBoard, move);
		int value = -searchScore(nextBoard, -sideToMove, depth - 1, -beta, -alpha);
		best = std::max(best, value);
		alpha = std::max(alpha, value);
		if (alpha >= beta) {
			break;
		}
	}
	return best;
}

inline int searchDepthForDifficulty(int difficulty) {
	switch (std::max(0, std::min(difficulty, 2))) {
		case 0: return 1;
		case 2: return 3;
		default: return 2;
	}
}

inline Move chooseAiMove(const std::array<int, BOARD_SIZE>& board, int difficulty) {
	std::vector<Move> moves = generateLegalMovesForSide(board, AI_SIDE);
	if (moves.empty()) {
		return Move();
	}
	int bestIndex = 0;
	int bestScore = std::numeric_limits<int>::min();
	int depth = searchDepthForDifficulty(difficulty);
	for (int i = 0; i < int(moves.size()); ++i) {
		std::array<int, BOARD_SIZE> nextBoard = applyMoveToBoard(board, moves[size_t(i)]);
		int score = -searchScore(nextBoard, HUMAN_SIDE, depth - 1, std::numeric_limits<int>::min() / 2,
			std::numeric_limits<int>::max() / 2);
		if (score > bestScore) {
			bestScore = score;
			bestIndex = i;
		}
	}
	return moves[size_t(bestIndex)];
}

inline float interpretPitchIndexForMove(const Move& move, int interpretationMode) {
	int mode = std::max(0, std::min(interpretationMode, int(PITCH_INTERPRETATION_NAMES.size()) - 1));
	float origin = float(std::max(0, std::min(move.originIndex, BOARD_SIZE - 1)));
	float destination = float(std::max(0, std::min(move.destinationIndex, BOARD_SIZE - 1)));
	if (mode == 1) {
		return destination;
	}
	if (mode == 2) {
		return 0.5f * (origin + destination);
	}
	return origin;
}

inline float boardCenterMetric(int boardIndex) {
	int row = 0;
	int col = 0;
	indexToCoord(boardIndex, &row, &col);
	float dx = float(col) - 3.5f;
	float dy = float(row) - 3.5f;
	// Tie-breakers keep rank stable for symmetric cells.
	return dx * dx + dy * dy + float(row) * 0.01f + float(col) * 0.001f;
}

inline int boardValueForIndex(int boardIndex, int layoutMode) {
	int index = std::max(0, std::min(boardIndex, BOARD_SIZE - 1));
	int mode = std::max(0, std::min(layoutMode, int(BOARD_VALUE_LAYOUT_NAMES.size()) - 1));
	switch (mode) {
		case 1: {
			int row = index / 4;
			int posInRow = index % 4;
			int serpentinePos = (row & 1) ? (3 - posInRow) : posInRow;
			return row * 4 + serpentinePos;
		}
		case 2: {
			float metric = boardCenterMetric(index);
			int rank = 0;
			for (int i = 0; i < BOARD_SIZE; ++i) {
				if (boardCenterMetric(i) < metric) {
					rank++;
				}
			}
			return rank;
		}
		case 0:
		default:
			return index;
	}
}

inline float boardValueForSampledIndex(float sampledIndex, int layoutMode) {
	float x = std::max(0.f, std::min(sampledIndex, float(BOARD_SIZE - 1)));
	int low = int(std::floor(x));
	int high = int(std::ceil(x));
	float lowValue = float(boardValueForIndex(low, layoutMode));
	if (high <= low) {
		return lowValue;
	}
	float highValue = float(boardValueForIndex(high, layoutMode));
	float t = x - float(low);
	return lowValue + (highValue - lowValue) * t;
}

inline float sampledBoardValueForMove(const Move& move, int interpretationMode, int layoutMode) {
	int mode = std::max(0, std::min(interpretationMode, int(PITCH_INTERPRETATION_NAMES.size()) - 1));
	float originIndex = float(std::max(0, std::min(move.originIndex, BOARD_SIZE - 1)));
	float destinationIndex = float(std::max(0, std::min(move.destinationIndex, BOARD_SIZE - 1)));
	float originValue = boardValueForSampledIndex(originIndex, layoutMode);
	float destinationValue = boardValueForSampledIndex(destinationIndex, layoutMode);
	if (mode == 1) {
		return destinationValue;
	}
	if (mode == 2) {
		return 0.5f * (originValue + destinationValue);
	}
	return originValue;
}

inline float pitchDividerForMode(int dividerMode) {
	int mode = std::max(0, std::min(dividerMode, int(PITCH_DIVIDER_VALUES.size()) - 1));
	return PITCH_DIVIDER_VALUES[size_t(mode)];
}

inline float applyPitchDividerToBoardValue(float boardValueIndex, int dividerMode) {
	return boardValueIndex / pitchDividerForMode(dividerMode);
}

inline float pitchBipolarCenterOffset(int dividerMode) {
	return (0.5f * float(BOARD_SIZE - 1)) / pitchDividerForMode(dividerMode);
}

inline float mapPitchFromIndex(float index, bool isKing, int scaleIndex, int rootSemitone, float transposeVolts) {
	const Scale& scale = SCALES[size_t(std::max(0, std::min(scaleIndex, int(SCALES.size()) - 1)))];
	int scaleLen = std::max(scale.length, 1);
	int idx = int(std::lround(index));
	int octave = int(std::floor(double(idx) / double(scaleLen)));
	int scaleDegree = idx - octave * scaleLen;
	int semitone = scale.semitones[size_t(scaleDegree)] + octave * 12 + wrapSemitone12(rootSemitone);
	if (isKing) {
		semitone += 12;
	}
	return float(semitone) / 12.f + transposeVolts;
}

inline float mapRawPitchFromIndex(float index, bool isKing, float transposeVolts) {
	float semitone = index;
	if (isKing) {
		semitone += 12.f;
	}
	return semitone / 12.f + transposeVolts;
}

inline float mapPitch(const Move& move, int scaleIndex, int rootSemitone, float transposeVolts) {
	float index = interpretPitchIndexForMove(move, 0);
	return mapPitchFromIndex(index, move.isKing, scaleIndex, rootSemitone, transposeVolts);
}

inline float mapRawPitch(const Move& move, float transposeVolts) {
	float index = interpretPitchIndexForMove(move, 0);
	return mapRawPitchFromIndex(index, move.isKing, transposeVolts);
}

inline Step makeStepFromMove(const Move& move, int scaleIndex, int rootSemitone, float transposeVolts) {
	Step step;
	step.pitch = mapPitch(move, scaleIndex, rootSemitone, transposeVolts);
	step.gate = true;
	step.accent = moveAccent(move);
	step.mod = normalizedMoveMod(move);
	return step;
}

inline int activeLength(int historySize, int sequenceCap) {
	if (historySize <= 0) {
		return 0;
	}
	if (sequenceCap == 0) {
		return historySize;
	}
	return std::min(sequenceCap, historySize);
}

inline int activeStartIndex(int historySize, int sequenceCap) {
	return std::max(0, historySize - activeLength(historySize, sequenceCap));
}

} // namespace crownstep
