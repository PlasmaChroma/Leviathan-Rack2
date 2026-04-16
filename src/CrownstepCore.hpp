#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <vector>

namespace crownstep {

static constexpr int CHECKERS_BOARD_SIZE = 32;
static constexpr int CHESS_BOARD_SIZE = 64;
static constexpr int OTHELLO_BOARD_SIZE = 64;
static constexpr int MAX_BOARD_SIZE = CHESS_BOARD_SIZE;
static constexpr int BOARD_SIZE = CHECKERS_BOARD_SIZE;
static constexpr int HUMAN_SIDE = 1;
static constexpr int AI_SIDE = -1;
using BoardState = std::array<int, MAX_BOARD_SIZE>;

static constexpr int CHESS_PAWN = 1;
static constexpr int CHESS_KNIGHT = 2;
static constexpr int CHESS_BISHOP = 3;
static constexpr int CHESS_ROOK = 4;
static constexpr int CHESS_QUEEN = 5;
static constexpr int CHESS_KING = 6;

struct ChessState {
	bool whiteCanCastleKingSide = true;
	bool whiteCanCastleQueenSide = true;
	bool blackCanCastleKingSide = true;
	bool blackCanCastleQueenSide = true;
	int enPassantTargetIndex = -1;
};

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
static constexpr std::array<const char*, 4> DIFFICULTY_NAMES = {{
	"Too young to die",
	"Not too rough",
	"Hurt me plenty",
	"Ultra violence"
}};
static constexpr std::array<const char*, 3> PITCH_INTERPRETATION_NAMES = {
	{"Origin Square", "Destination Square", "Blend (O+D)/2"}
};
static constexpr int LEGACY_BOARD_VALUE_LAYOUT_COUNT = 7;
static constexpr int BOARD_VALUE_LAYOUT_RANDOM = 7;
static constexpr std::array<const char*, 8> BOARD_VALUE_LAYOUT_NAMES = {
	{"Center-Out", "Linear (Horizontal)", "Linear (Vertical)", "Linear (Diagonal)",
		"Serpentine (Horizontal)", "Serpentine (Vertical)", "Serpentine (Diagonal)", "Random"}
};
static constexpr std::array<const char*, 7> PITCH_DIVIDER_NAMES = {
	{"Full", "Half", "Third", "Quarter", "1.5x", "2x", "3x"}
};
static constexpr std::array<float, 7> PITCH_DIVIDER_VALUES = {
	{1.f, 0.5f, 1.f / 3.f, 0.25f, 1.5f, 2.f, 3.f}
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

inline BoardState makeInitialBoard() {
	BoardState board {};
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

inline void addSimpleMovesForPiece(const BoardState& sourceBoard, int index, std::vector<Move>* moves) {
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
	const BoardState& sourceBoard,
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
			BoardState nextBoard = sourceBoard;
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

inline std::vector<Move> generateLegalMovesForSide(const BoardState& sourceBoard, int side) {
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

inline BoardState applyMoveToBoard(const BoardState& sourceBoard, const Move& move) {
	BoardState nextBoard = sourceBoard;
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

inline int evaluateBoardMaterial(const BoardState& board) {
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

inline int evaluatePosition(const BoardState& sourceBoard) {
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

inline int searchScore(const BoardState& sourceBoard, int sideToMove, int depth, int alpha, int beta) {
	std::vector<Move> moves = generateLegalMovesForSide(sourceBoard, sideToMove);
	if (depth <= 0 || moves.empty()) {
		int score = evaluatePosition(sourceBoard);
		return (sideToMove == AI_SIDE) ? score : -score;
	}
	int best = std::numeric_limits<int>::min();
	for (const Move& move : moves) {
		BoardState nextBoard = applyMoveToBoard(sourceBoard, move);
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
	switch (std::max(0, std::min(difficulty, 3))) {
		case 0: return 1;
		case 2: return 3;
		case 3: return 4;
		default: return 2;
	}
}

inline Move chooseAiMove(const BoardState& board, int difficulty) {
	std::vector<Move> moves = generateLegalMovesForSide(board, AI_SIDE);
	if (moves.empty()) {
		return Move();
	}
	int bestIndex = 0;
	int bestScore = std::numeric_limits<int>::min();
	int depth = searchDepthForDifficulty(difficulty);
	for (int i = 0; i < int(moves.size()); ++i) {
		BoardState nextBoard = applyMoveToBoard(board, moves[size_t(i)]);
		int score = -searchScore(nextBoard, HUMAN_SIDE, depth - 1, std::numeric_limits<int>::min() / 2,
			std::numeric_limits<int>::max() / 2);
		if (score > bestScore) {
			bestScore = score;
			bestIndex = i;
		}
	}
	return moves[size_t(bestIndex)];
}

inline bool chessIndexToCoord(int index, int* row, int* col) {
	if (index < 0 || index >= CHESS_BOARD_SIZE) {
		return false;
	}
	if (row) {
		*row = index / 8;
	}
	if (col) {
		*col = index % 8;
	}
	return true;
}

inline int chessCoordToIndex(int row, int col) {
	if (row < 0 || row >= 8 || col < 0 || col >= 8) {
		return -1;
	}
	return row * 8 + col;
}

inline int chessPieceType(int piece) {
	return std::abs(piece);
}

inline ChessState chessInitialState() {
	return ChessState();
}

inline int chessKingStartIndexForSide(int side) {
	return (side == HUMAN_SIDE) ? chessCoordToIndex(7, 4) : chessCoordToIndex(0, 4);
}

inline int chessKingsideRookStartIndexForSide(int side) {
	return (side == HUMAN_SIDE) ? chessCoordToIndex(7, 7) : chessCoordToIndex(0, 7);
}

inline int chessQueensideRookStartIndexForSide(int side) {
	return (side == HUMAN_SIDE) ? chessCoordToIndex(7, 0) : chessCoordToIndex(0, 0);
}

inline bool chessCanCapturePiece(int movingSide, int destinationPiece) {
	if (destinationPiece == 0 || pieceSide(destinationPiece) == movingSide) {
		return false;
	}
	return chessPieceType(destinationPiece) != CHESS_KING;
}

inline ChessState chessInferStateFromBoard(const BoardState& board) {
	ChessState state;
	state.enPassantTargetIndex = -1;
	state.whiteCanCastleKingSide =
		(board[size_t(chessKingStartIndexForSide(HUMAN_SIDE))] == CHESS_KING) &&
		(board[size_t(chessKingsideRookStartIndexForSide(HUMAN_SIDE))] == CHESS_ROOK);
	state.whiteCanCastleQueenSide =
		(board[size_t(chessKingStartIndexForSide(HUMAN_SIDE))] == CHESS_KING) &&
		(board[size_t(chessQueensideRookStartIndexForSide(HUMAN_SIDE))] == CHESS_ROOK);
	state.blackCanCastleKingSide =
		(board[size_t(chessKingStartIndexForSide(AI_SIDE))] == -CHESS_KING) &&
		(board[size_t(chessKingsideRookStartIndexForSide(AI_SIDE))] == -CHESS_ROOK);
	state.blackCanCastleQueenSide =
		(board[size_t(chessKingStartIndexForSide(AI_SIDE))] == -CHESS_KING) &&
		(board[size_t(chessQueensideRookStartIndexForSide(AI_SIDE))] == -CHESS_ROOK);
	return state;
}

inline bool chessCanCastleForSide(const ChessState& state, int side, bool kingSide) {
	if (side == HUMAN_SIDE) {
		return kingSide ? state.whiteCanCastleKingSide : state.whiteCanCastleQueenSide;
	}
	return kingSide ? state.blackCanCastleKingSide : state.blackCanCastleQueenSide;
}

inline void chessClearCastlingForSide(ChessState* state, int side) {
	if (!state) {
		return;
	}
	if (side == HUMAN_SIDE) {
		state->whiteCanCastleKingSide = false;
		state->whiteCanCastleQueenSide = false;
	}
	else if (side == AI_SIDE) {
		state->blackCanCastleKingSide = false;
		state->blackCanCastleQueenSide = false;
	}
}

inline void chessClearRookCastlingRightForIndex(ChessState* state, int rookIndex) {
	if (!state) {
		return;
	}
	if (rookIndex == chessKingsideRookStartIndexForSide(HUMAN_SIDE)) {
		state->whiteCanCastleKingSide = false;
	}
	else if (rookIndex == chessQueensideRookStartIndexForSide(HUMAN_SIDE)) {
		state->whiteCanCastleQueenSide = false;
	}
	else if (rookIndex == chessKingsideRookStartIndexForSide(AI_SIDE)) {
		state->blackCanCastleKingSide = false;
	}
	else if (rookIndex == chessQueensideRookStartIndexForSide(AI_SIDE)) {
		state->blackCanCastleQueenSide = false;
	}
}

inline int chessBackRankPieceTypeForCol(int col) {
	switch (col) {
		case 0:
		case 7: return CHESS_ROOK;
		case 1:
		case 6: return CHESS_KNIGHT;
		case 2:
		case 5: return CHESS_BISHOP;
		case 3: return CHESS_QUEEN;
		case 4: return CHESS_KING;
		default: return 0;
	}
}

inline BoardState chessMakeInitialBoard() {
	BoardState board {};
	for (int col = 0; col < 8; ++col) {
		board[size_t(chessCoordToIndex(0, col))] = -chessBackRankPieceTypeForCol(col);
		board[size_t(chessCoordToIndex(1, col))] = -CHESS_PAWN;
		board[size_t(chessCoordToIndex(6, col))] = CHESS_PAWN;
		board[size_t(chessCoordToIndex(7, col))] = chessBackRankPieceTypeForCol(col);
	}
	return board;
}

inline void chessAppendMove(
	int originIndex,
	int destinationIndex,
	int movingPiece,
	int capturedIndex,
	std::vector<Move>* moves
) {
	if (!moves) {
		return;
	}
	Move move;
	move.originIndex = originIndex;
	move.destinationIndex = destinationIndex;
	move.path.push_back(destinationIndex);
	move.isCapture = capturedIndex >= 0;
	move.isMultiCapture = false;
	move.isKing = (chessPieceType(movingPiece) == CHESS_KING);
	if (capturedIndex >= 0) {
		move.captured.push_back(capturedIndex);
	}
	moves->push_back(move);
}

inline bool chessSquareIsAttackedBySide(const BoardState& board, int targetIndex, int attackerSide);
inline bool chessIsKingInCheck(const BoardState& board, int side);

inline void chessAddPseudoMovesForPiece(const BoardState& board, int index, std::vector<Move>* moves, const ChessState* state = nullptr) {
	if (!moves || index < 0 || index >= CHESS_BOARD_SIZE) {
		return;
	}
	int piece = board[size_t(index)];
	if (piece == 0) {
		return;
	}
	int side = pieceSide(piece);
	int row = 0;
	int col = 0;
	chessIndexToCoord(index, &row, &col);
	int type = chessPieceType(piece);

	if (type == CHESS_PAWN) {
		int forward = (side == HUMAN_SIDE) ? -1 : 1;
		int startRow = (side == HUMAN_SIDE) ? 6 : 1;
		int nextRow = row + forward;
		int nextIndex = chessCoordToIndex(nextRow, col);
		if (nextIndex >= 0 && board[size_t(nextIndex)] == 0) {
			chessAppendMove(index, nextIndex, piece, -1, moves);
			int doubleRow = row + forward * 2;
			int doubleIndex = chessCoordToIndex(doubleRow, col);
			if (row == startRow && doubleIndex >= 0 && board[size_t(doubleIndex)] == 0) {
				chessAppendMove(index, doubleIndex, piece, -1, moves);
			}
		}
		for (int dc : {-1, 1}) {
			int captureCol = col + dc;
			int captureIndex = chessCoordToIndex(nextRow, captureCol);
			if (captureIndex < 0) {
				continue;
			}
			int capturePiece = board[size_t(captureIndex)];
			if (chessCanCapturePiece(side, capturePiece)) {
				chessAppendMove(index, captureIndex, piece, captureIndex, moves);
			}
		}
		if (state && state->enPassantTargetIndex >= 0) {
			int targetRow = 0;
			int targetCol = 0;
			if (chessIndexToCoord(state->enPassantTargetIndex, &targetRow, &targetCol) &&
				targetRow == nextRow && std::abs(targetCol - col) == 1 &&
				board[size_t(state->enPassantTargetIndex)] == 0) {
				int capturedIndex = state->enPassantTargetIndex - forward * 8;
				if (capturedIndex >= 0 && capturedIndex < CHESS_BOARD_SIZE) {
					int capturedPiece = board[size_t(capturedIndex)];
					if (capturedPiece == -side * CHESS_PAWN) {
						chessAppendMove(index, state->enPassantTargetIndex, piece, capturedIndex, moves);
					}
				}
			}
		}
		return;
	}

	if (type == CHESS_KNIGHT) {
		static constexpr int kOffsets[8][2] = {
			{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
			{1, -2}, {1, 2}, {2, -1}, {2, 1}
		};
		for (const auto& offset : kOffsets) {
			int r = row + offset[0];
			int c = col + offset[1];
			int destination = chessCoordToIndex(r, c);
			if (destination < 0) {
				continue;
			}
			int destinationPiece = board[size_t(destination)];
			if (destinationPiece == 0) {
				chessAppendMove(index, destination, piece, -1, moves);
			}
			else if (chessCanCapturePiece(side, destinationPiece)) {
				chessAppendMove(index, destination, piece, destination, moves);
			}
		}
		return;
	}

	auto addSliding = [&](const int directions[][2], int directionCount) {
		for (int i = 0; i < directionCount; ++i) {
			int dr = directions[i][0];
			int dc = directions[i][1];
			int r = row + dr;
			int c = col + dc;
			while (true) {
				int destination = chessCoordToIndex(r, c);
				if (destination < 0) {
					break;
				}
				int destinationPiece = board[size_t(destination)];
				if (destinationPiece == 0) {
					chessAppendMove(index, destination, piece, -1, moves);
				}
				else {
					if (chessCanCapturePiece(side, destinationPiece)) {
						chessAppendMove(index, destination, piece, destination, moves);
					}
					break;
				}
				r += dr;
				c += dc;
			}
		}
	};

	if (type == CHESS_BISHOP || type == CHESS_QUEEN) {
		static constexpr int kDiagDirs[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
		addSliding(kDiagDirs, 4);
	}
	if (type == CHESS_ROOK || type == CHESS_QUEEN) {
		static constexpr int kOrthoDirs[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
		addSliding(kOrthoDirs, 4);
	}
	if (type == CHESS_KING) {
		for (int dr = -1; dr <= 1; ++dr) {
			for (int dc = -1; dc <= 1; ++dc) {
				if (dr == 0 && dc == 0) {
					continue;
				}
				int destination = chessCoordToIndex(row + dr, col + dc);
				if (destination < 0) {
					continue;
				}
					int destinationPiece = board[size_t(destination)];
					if (destinationPiece == 0) {
						chessAppendMove(index, destination, piece, -1, moves);
					}
					else if (chessCanCapturePiece(side, destinationPiece)) {
						chessAppendMove(index, destination, piece, destination, moves);
					}
				}
			}
		if (state && index == chessKingStartIndexForSide(side) && !chessIsKingInCheck(board, side)) {
			int row = (side == HUMAN_SIDE) ? 7 : 0;
			int opponent = -side;
			if (chessCanCastleForSide(*state, side, true)) {
				int rookIndex = chessCoordToIndex(row, 7);
				int passIndex = chessCoordToIndex(row, 5);
				int destination = chessCoordToIndex(row, 6);
				if (board[size_t(rookIndex)] == side * CHESS_ROOK &&
					board[size_t(passIndex)] == 0 &&
					board[size_t(destination)] == 0 &&
					!chessSquareIsAttackedBySide(board, passIndex, opponent) &&
					!chessSquareIsAttackedBySide(board, destination, opponent)) {
					chessAppendMove(index, destination, piece, -1, moves);
				}
			}
			if (chessCanCastleForSide(*state, side, false)) {
				int rookIndex = chessCoordToIndex(row, 0);
				int passIndex = chessCoordToIndex(row, 3);
				int destination = chessCoordToIndex(row, 2);
				int bufferIndex = chessCoordToIndex(row, 1);
				if (board[size_t(rookIndex)] == side * CHESS_ROOK &&
					board[size_t(passIndex)] == 0 &&
					board[size_t(destination)] == 0 &&
					board[size_t(bufferIndex)] == 0 &&
					!chessSquareIsAttackedBySide(board, passIndex, opponent) &&
					!chessSquareIsAttackedBySide(board, destination, opponent)) {
					chessAppendMove(index, destination, piece, -1, moves);
				}
			}
		}
	}
}

inline int chessFindKingIndex(const BoardState& board, int side) {
	const int target = side * CHESS_KING;
	for (int i = 0; i < CHESS_BOARD_SIZE; ++i) {
		if (board[size_t(i)] == target) {
			return i;
		}
	}
	return -1;
}

inline bool chessSquareIsAttackedBySide(const BoardState& board, int targetIndex, int attackerSide) {
	int targetRow = 0;
	int targetCol = 0;
	if (!chessIndexToCoord(targetIndex, &targetRow, &targetCol)) {
		return false;
	}

	// Pawn attacks.
	int attackerForward = (attackerSide == HUMAN_SIDE) ? -1 : 1;
	int pawnRow = targetRow - attackerForward;
	for (int dc : {-1, 1}) {
		int pawnCol = targetCol + dc;
		int pawnIndex = chessCoordToIndex(pawnRow, pawnCol);
		if (pawnIndex >= 0 && board[size_t(pawnIndex)] == attackerSide * CHESS_PAWN) {
			return true;
		}
	}

	// Knight attacks.
	static constexpr int kKnightOffsets[8][2] = {
		{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
		{1, -2}, {1, 2}, {2, -1}, {2, 1}
	};
	for (const auto& offset : kKnightOffsets) {
		int index = chessCoordToIndex(targetRow + offset[0], targetCol + offset[1]);
		if (index >= 0 && board[size_t(index)] == attackerSide * CHESS_KNIGHT) {
			return true;
		}
	}

	auto attackedBySlider = [&](const int directions[][2], int directionCount, int sliderA, int sliderB) {
		for (int i = 0; i < directionCount; ++i) {
			int dr = directions[i][0];
			int dc = directions[i][1];
			int r = targetRow + dr;
			int c = targetCol + dc;
			while (true) {
				int index = chessCoordToIndex(r, c);
				if (index < 0) {
					break;
				}
				int piece = board[size_t(index)];
				if (piece != 0) {
					if (pieceSide(piece) == attackerSide) {
						int type = chessPieceType(piece);
						if (type == sliderA || type == sliderB) {
							return true;
						}
					}
					break;
				}
				r += dr;
				c += dc;
			}
		}
		return false;
	};

	static constexpr int kDiagDirs[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
	if (attackedBySlider(kDiagDirs, 4, CHESS_BISHOP, CHESS_QUEEN)) {
		return true;
	}
	static constexpr int kOrthoDirs[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
	if (attackedBySlider(kOrthoDirs, 4, CHESS_ROOK, CHESS_QUEEN)) {
		return true;
	}

	// King attacks.
	for (int dr = -1; dr <= 1; ++dr) {
		for (int dc = -1; dc <= 1; ++dc) {
			if (dr == 0 && dc == 0) {
				continue;
			}
			int index = chessCoordToIndex(targetRow + dr, targetCol + dc);
			if (index >= 0 && board[size_t(index)] == attackerSide * CHESS_KING) {
				return true;
			}
		}
	}

	return false;
}

inline bool chessIsKingInCheck(const BoardState& board, int side) {
	int kingIndex = chessFindKingIndex(board, side);
	if (kingIndex < 0) {
		return true;
	}
	return chessSquareIsAttackedBySide(board, kingIndex, -side);
}

inline BoardState chessApplyMoveToBoard(
	const BoardState& sourceBoard,
	const Move& move,
	const ChessState& sourceState,
	ChessState* outState
) {
	ChessState nextState = sourceState;
	nextState.enPassantTargetIndex = -1;
	BoardState nextBoard = sourceBoard;
	if (move.originIndex < 0 || move.originIndex >= CHESS_BOARD_SIZE ||
		move.destinationIndex < 0 || move.destinationIndex >= CHESS_BOARD_SIZE) {
		if (outState) {
			*outState = nextState;
		}
		return nextBoard;
	}
	int movingPiece = nextBoard[size_t(move.originIndex)];
	if (movingPiece == 0) {
		if (outState) {
			*outState = nextState;
		}
		return nextBoard;
	}
	int movingSide = pieceSide(movingPiece);
	int movingType = chessPieceType(movingPiece);
	if (movingType == CHESS_KING) {
		chessClearCastlingForSide(&nextState, movingSide);
	}
	else if (movingType == CHESS_ROOK) {
		chessClearRookCastlingRightForIndex(&nextState, move.originIndex);
	}
	int destinationPiece = nextBoard[size_t(move.destinationIndex)];
	if (destinationPiece != 0 && chessPieceType(destinationPiece) == CHESS_ROOK) {
		chessClearRookCastlingRightForIndex(&nextState, move.destinationIndex);
	}
	nextBoard[size_t(move.originIndex)] = 0;
	for (int captureIndex : move.captured) {
		if (captureIndex >= 0 && captureIndex < CHESS_BOARD_SIZE) {
			int capturedPiece = nextBoard[size_t(captureIndex)];
			if (capturedPiece != 0 && chessPieceType(capturedPiece) == CHESS_ROOK) {
				chessClearRookCastlingRightForIndex(&nextState, captureIndex);
			}
			nextBoard[size_t(captureIndex)] = 0;
		}
	}
	int originRow = 0;
	int originCol = 0;
	int destinationCol = 0;
	chessIndexToCoord(move.originIndex, &originRow, &originCol);
	chessIndexToCoord(move.destinationIndex, nullptr, &destinationCol);
	if (movingType == CHESS_KING && originRow == ((movingSide == HUMAN_SIDE) ? 7 : 0) &&
		std::abs(destinationCol - originCol) == 2) {
		bool kingSide = destinationCol > originCol;
		int rookFrom = kingSide ? chessKingsideRookStartIndexForSide(movingSide) : chessQueensideRookStartIndexForSide(movingSide);
		int rookTo = kingSide ? chessCoordToIndex(originRow, 5) : chessCoordToIndex(originRow, 3);
		int rookPiece = nextBoard[size_t(rookFrom)];
		nextBoard[size_t(rookFrom)] = 0;
		nextBoard[size_t(rookTo)] = rookPiece;
	}
	int destinationRow = 0;
	chessIndexToCoord(move.destinationIndex, &destinationRow, nullptr);
	if (movingType == CHESS_PAWN) {
		if (std::abs(destinationRow - originRow) == 2) {
			nextState.enPassantTargetIndex = chessCoordToIndex((originRow + destinationRow) / 2, originCol);
		}
		bool promote = (pieceSide(movingPiece) == HUMAN_SIDE) ? (destinationRow == 0) : (destinationRow == 7);
		if (promote) {
			movingPiece = pieceSide(movingPiece) * CHESS_QUEEN;
		}
	}
	nextBoard[size_t(move.destinationIndex)] = movingPiece;
	if (outState) {
		*outState = nextState;
	}
	return nextBoard;
}

inline BoardState chessApplyMoveToBoard(const BoardState& sourceBoard, const Move& move) {
	ChessState inferred = chessInferStateFromBoard(sourceBoard);
	return chessApplyMoveToBoard(sourceBoard, move, inferred, nullptr);
}

inline std::vector<Move> chessGenerateLegalMovesForSide(const BoardState& sourceBoard, int side, const ChessState& state) {
	std::vector<Move> pseudo;
	for (int i = 0; i < CHESS_BOARD_SIZE; ++i) {
		int piece = sourceBoard[size_t(i)];
		if (piece == 0 || pieceSide(piece) != side) {
			continue;
		}
		chessAddPseudoMovesForPiece(sourceBoard, i, &pseudo, &state);
	}

	std::vector<Move> legal;
	legal.reserve(pseudo.size());
	for (const Move& move : pseudo) {
		ChessState nextState;
		BoardState nextBoard = chessApplyMoveToBoard(sourceBoard, move, state, &nextState);
		if (!chessIsKingInCheck(nextBoard, side)) {
			legal.push_back(move);
		}
	}
	return legal;
}

inline std::vector<Move> chessGenerateLegalMovesForSide(const BoardState& sourceBoard, int side) {
	ChessState inferred = chessInferStateFromBoard(sourceBoard);
	return chessGenerateLegalMovesForSide(sourceBoard, side, inferred);
}

inline int chessPieceMaterialValue(int pieceType) {
	switch (pieceType) {
		case CHESS_PAWN: return 100;
		case CHESS_KNIGHT: return 320;
		case CHESS_BISHOP: return 330;
		case CHESS_ROOK: return 500;
		case CHESS_QUEEN: return 900;
		case CHESS_KING: return 20000;
		default: return 0;
	}
}

inline int chessEvaluateBoardMaterial(const BoardState& board) {
	int score = 0;
	for (int i = 0; i < CHESS_BOARD_SIZE; ++i) {
		int piece = board[size_t(i)];
		if (piece == 0) {
			continue;
		}
		int value = chessPieceMaterialValue(chessPieceType(piece));
		score += (pieceSide(piece) == AI_SIDE) ? value : -value;
	}
	return score;
}

inline int chessEvaluatePosition(const BoardState& board, const ChessState& state) {
	std::vector<Move> aiMoves = chessGenerateLegalMovesForSide(board, AI_SIDE, state);
	std::vector<Move> humanMoves = chessGenerateLegalMovesForSide(board, HUMAN_SIDE, state);

	int score = chessEvaluateBoardMaterial(board);
	score += int(aiMoves.size()) * 4;
	score -= int(humanMoves.size()) * 4;

	if (humanMoves.empty()) {
		score += chessIsKingInCheck(board, HUMAN_SIDE) ? 100000 : 0;
	}
	if (aiMoves.empty()) {
		score -= chessIsKingInCheck(board, AI_SIDE) ? 100000 : 0;
	}
	return score;
}

inline int chessEvaluatePosition(const BoardState& board) {
	ChessState inferred = chessInferStateFromBoard(board);
	return chessEvaluatePosition(board, inferred);
}

inline int chessOrderPieceValueForType(int pieceType) {
	switch (pieceType) {
		case CHESS_PAWN: return 100;
		case CHESS_KNIGHT: return 320;
		case CHESS_BISHOP: return 330;
		case CHESS_ROOK: return 500;
		case CHESS_QUEEN: return 900;
		case CHESS_KING: return 20000;
		default: return 0;
	}
}

inline int chessMoveOrderingScore(const BoardState& board, const Move& move) {
	int movingPiece = 0;
	if (move.originIndex >= 0 && move.originIndex < CHESS_BOARD_SIZE) {
		movingPiece = board[size_t(move.originIndex)];
	}
	int movingType = chessPieceType(movingPiece);
	int score = 0;

	// MVV/LVA-style capture prioritization keeps alpha-beta cutoffs effective.
	if (move.isCapture) {
		int capturedValue = 0;
		for (int captureIndex : move.captured) {
			if (captureIndex >= 0 && captureIndex < CHESS_BOARD_SIZE) {
				capturedValue += chessOrderPieceValueForType(chessPieceType(board[size_t(captureIndex)]));
			}
		}
		if (capturedValue == 0 && move.destinationIndex >= 0 && move.destinationIndex < CHESS_BOARD_SIZE) {
			capturedValue = chessOrderPieceValueForType(chessPieceType(board[size_t(move.destinationIndex)]));
		}
		int movingValue = chessOrderPieceValueForType(movingType);
		score += 6000 + capturedValue * 12 - movingValue;
	}

	if (movingType == CHESS_PAWN) {
		int destinationRow = 0;
		if (chessIndexToCoord(move.destinationIndex, &destinationRow, nullptr)) {
			bool promote = (pieceSide(movingPiece) == HUMAN_SIDE) ? (destinationRow == 0) : (destinationRow == 7);
			if (promote) {
				score += 5000;
			}
		}
	}

	if (move.isKing) {
		int originRow = 0;
		int originCol = 0;
		int destinationCol = 0;
		if (chessIndexToCoord(move.originIndex, &originRow, &originCol) &&
			chessIndexToCoord(move.destinationIndex, nullptr, &destinationCol) &&
			std::abs(destinationCol - originCol) == 2) {
			score += 700;
		}
	}

	return score;
}

inline void chessSortMovesForSearch(const BoardState& board, std::vector<Move>* moves) {
	if (!moves || moves->size() < 2) {
		return;
	}
	std::stable_sort(moves->begin(), moves->end(), [&](const Move& a, const Move& b) {
		int scoreA = chessMoveOrderingScore(board, a);
		int scoreB = chessMoveOrderingScore(board, b);
		if (scoreA != scoreB) {
			return scoreA > scoreB;
		}
		if (a.originIndex != b.originIndex) {
			return a.originIndex < b.originIndex;
		}
		return a.destinationIndex < b.destinationIndex;
	});
}

inline int chessSearchDepthForDifficulty(int difficulty) {
	switch (std::max(0, std::min(difficulty, 3))) {
		case 0: return 1;
		case 2: return 3;
		case 3: return 4;
		default: return 2;
	}
}

inline int chessSearchScore(const BoardState& board, const ChessState& state, int sideToMove, int depth, int alpha, int beta) {
	std::vector<Move> moves = chessGenerateLegalMovesForSide(board, sideToMove, state);
	if (depth <= 0 || moves.empty()) {
		int score = chessEvaluatePosition(board, state);
		return (sideToMove == AI_SIDE) ? score : -score;
	}
	chessSortMovesForSearch(board, &moves);
	int best = std::numeric_limits<int>::min();
	for (const Move& move : moves) {
		ChessState nextState;
		BoardState nextBoard = chessApplyMoveToBoard(board, move, state, &nextState);
		int value = -chessSearchScore(nextBoard, nextState, -sideToMove, depth - 1, -beta, -alpha);
		best = std::max(best, value);
		alpha = std::max(alpha, value);
		if (alpha >= beta) {
			break;
		}
	}
	return best;
}

inline int chessSearchScore(const BoardState& board, int sideToMove, int depth, int alpha, int beta) {
	ChessState inferred = chessInferStateFromBoard(board);
	return chessSearchScore(board, inferred, sideToMove, depth, alpha, beta);
}

inline Move chessChooseAiMove(const BoardState& board, int difficulty, const ChessState& state) {
	std::vector<Move> moves = chessGenerateLegalMovesForSide(board, AI_SIDE, state);
	if (moves.empty()) {
		return Move();
	}
	chessSortMovesForSearch(board, &moves);
	int depth = chessSearchDepthForDifficulty(difficulty);
	int bestIndex = 0;
	int bestScore = std::numeric_limits<int>::min();
	for (int i = 0; i < int(moves.size()); ++i) {
		ChessState nextState;
		BoardState nextBoard = chessApplyMoveToBoard(board, moves[size_t(i)], state, &nextState);
		int score = -chessSearchScore(
			nextBoard,
			nextState,
			HUMAN_SIDE,
			depth - 1,
			std::numeric_limits<int>::min() / 2,
			std::numeric_limits<int>::max() / 2
		);
		// Prefer captures when scores tie to keep the AI active in MVP mode.
		if (score > bestScore || (score == bestScore && moves[size_t(i)].isCapture && !moves[size_t(bestIndex)].isCapture)) {
			bestScore = score;
			bestIndex = i;
		}
	}
	return moves[size_t(bestIndex)];
}

inline Move chessChooseAiMove(const BoardState& board, int difficulty) {
	ChessState inferred = chessInferStateFromBoard(board);
	return chessChooseAiMove(board, difficulty, inferred);
}

inline int chessWinnerForNoLegalMoves(const BoardState& board, int sideToMove) {
	if (chessIsKingInCheck(board, sideToMove)) {
		return -sideToMove;
	}
	return 0;
}

inline bool othelloIndexToCoord(int index, int* row, int* col) {
	return chessIndexToCoord(index, row, col);
}

inline int othelloCoordToIndex(int row, int col) {
	return chessCoordToIndex(row, col);
}

inline BoardState othelloMakeInitialBoard() {
	BoardState board {};
	// Human is black, AI is white.
	board[size_t(othelloCoordToIndex(3, 3))] = AI_SIDE;
	board[size_t(othelloCoordToIndex(3, 4))] = HUMAN_SIDE;
	board[size_t(othelloCoordToIndex(4, 3))] = HUMAN_SIDE;
	board[size_t(othelloCoordToIndex(4, 4))] = AI_SIDE;
	return board;
}

inline bool othelloCollectDirectionFlips(
	const BoardState& board,
	int side,
	int row,
	int col,
	int dr,
	int dc,
	std::vector<int>* flips
) {
	if (!flips) {
		return false;
	}
	std::vector<int> local;
	int r = row + dr;
	int c = col + dc;
	while (true) {
		int idx = othelloCoordToIndex(r, c);
		if (idx < 0) {
			return false;
		}
		int piece = board[size_t(idx)];
		if (piece == 0) {
			return false;
		}
		if (piece == side) {
			if (local.empty()) {
				return false;
			}
			flips->insert(flips->end(), local.begin(), local.end());
			return true;
		}
		local.push_back(idx);
		r += dr;
		c += dc;
	}
}

inline std::vector<Move> othelloGenerateLegalMovesForSide(const BoardState& board, int side) {
	std::vector<Move> moves;
	for (int row = 0; row < 8; ++row) {
		for (int col = 0; col < 8; ++col) {
			int destination = othelloCoordToIndex(row, col);
			if (destination < 0 || board[size_t(destination)] != 0) {
				continue;
			}
			std::vector<int> flips;
			for (int dr = -1; dr <= 1; ++dr) {
				for (int dc = -1; dc <= 1; ++dc) {
					if (dr == 0 && dc == 0) {
						continue;
					}
					othelloCollectDirectionFlips(board, side, row, col, dr, dc, &flips);
				}
			}
			if (flips.empty()) {
				continue;
			}
			Move move;
			move.originIndex = destination;
			move.destinationIndex = destination;
			move.path.push_back(destination);
			move.captured = flips;
			move.isCapture = true;
			move.isMultiCapture = flips.size() > 1;
			move.isKing = false;
			moves.push_back(move);
		}
	}
	return moves;
}

inline BoardState othelloApplyMoveToBoard(const BoardState& sourceBoard, const Move& move, int sideToMove) {
	BoardState nextBoard = sourceBoard;
	if (move.destinationIndex < 0 || move.destinationIndex >= OTHELLO_BOARD_SIZE) {
		return nextBoard;
	}
	if (nextBoard[size_t(move.destinationIndex)] != 0) {
		return nextBoard;
	}
	nextBoard[size_t(move.destinationIndex)] = sideToMove;
	for (int captureIndex : move.captured) {
		if (captureIndex >= 0 && captureIndex < OTHELLO_BOARD_SIZE) {
			nextBoard[size_t(captureIndex)] = sideToMove;
		}
	}
	return nextBoard;
}

inline BoardState othelloApplyMoveToBoard(const BoardState& sourceBoard, const Move& move) {
	int sideToMove = HUMAN_SIDE;
	if (!move.captured.empty()) {
		int capturedIndex = move.captured.front();
		if (capturedIndex >= 0 && capturedIndex < OTHELLO_BOARD_SIZE) {
			int capturedPiece = sourceBoard[size_t(capturedIndex)];
			if (capturedPiece != 0) {
				sideToMove = -pieceSide(capturedPiece);
			}
		}
	}
	else if (move.originIndex >= 0 && move.originIndex < OTHELLO_BOARD_SIZE) {
		int piece = sourceBoard[size_t(move.originIndex)];
		if (piece != 0) {
			sideToMove = pieceSide(piece);
		}
	}
	return othelloApplyMoveToBoard(sourceBoard, move, sideToMove);
}

inline int othelloCountPiecesForSide(const BoardState& board, int side) {
	int count = 0;
	for (int i = 0; i < OTHELLO_BOARD_SIZE; ++i) {
		if (board[size_t(i)] == side) {
			count++;
		}
	}
	return count;
}

inline int othelloCornerScore(const BoardState& board) {
	int score = 0;
	for (int index : {othelloCoordToIndex(0, 0), othelloCoordToIndex(0, 7), othelloCoordToIndex(7, 0), othelloCoordToIndex(7, 7)}) {
		if (index < 0) {
			continue;
		}
		int piece = board[size_t(index)];
		if (piece == AI_SIDE) {
			score += 30;
		}
		else if (piece == HUMAN_SIDE) {
			score -= 30;
		}
	}
	return score;
}

inline int othelloEvaluateBoardMaterial(const BoardState& board) {
	int aiCount = othelloCountPiecesForSide(board, AI_SIDE);
	int humanCount = othelloCountPiecesForSide(board, HUMAN_SIDE);
	return (aiCount - humanCount) * 8;
}

inline int othelloEvaluatePosition(const BoardState& board) {
	std::vector<Move> aiMoves = othelloGenerateLegalMovesForSide(board, AI_SIDE);
	std::vector<Move> humanMoves = othelloGenerateLegalMovesForSide(board, HUMAN_SIDE);
	int score = othelloEvaluateBoardMaterial(board);
	score += int(aiMoves.size()) * 6;
	score -= int(humanMoves.size()) * 6;
	score += othelloCornerScore(board);
	return score;
}

inline int othelloSearchDepthForDifficulty(int difficulty) {
	switch (std::max(0, std::min(difficulty, 3))) {
		case 0: return 1;
		case 2: return 3;
		case 3: return 4;
		default: return 2;
	}
}

inline int othelloSearchScore(const BoardState& board, int sideToMove, int depth, int alpha, int beta) {
	std::vector<Move> moves = othelloGenerateLegalMovesForSide(board, sideToMove);
	std::vector<Move> opponentMoves = othelloGenerateLegalMovesForSide(board, -sideToMove);
	if (depth <= 0 || (moves.empty() && opponentMoves.empty())) {
		int score = othelloEvaluatePosition(board);
		return (sideToMove == AI_SIDE) ? score : -score;
	}

	if (moves.empty()) {
		return -othelloSearchScore(board, -sideToMove, depth - 1, -beta, -alpha);
	}

	int best = std::numeric_limits<int>::min();
	for (const Move& move : moves) {
		BoardState nextBoard = othelloApplyMoveToBoard(board, move, sideToMove);
		int value = -othelloSearchScore(nextBoard, -sideToMove, depth - 1, -beta, -alpha);
		best = std::max(best, value);
		alpha = std::max(alpha, value);
		if (alpha >= beta) {
			break;
		}
	}
	return best;
}

inline Move othelloChooseAiMove(const BoardState& board, int difficulty) {
	std::vector<Move> moves = othelloGenerateLegalMovesForSide(board, AI_SIDE);
	if (moves.empty()) {
		return Move();
	}
	int depth = othelloSearchDepthForDifficulty(difficulty);
	int bestIndex = 0;
	int bestScore = std::numeric_limits<int>::min();
	for (int i = 0; i < int(moves.size()); ++i) {
		BoardState nextBoard = othelloApplyMoveToBoard(board, moves[size_t(i)], AI_SIDE);
		int score = -othelloSearchScore(
			nextBoard,
			HUMAN_SIDE,
			depth - 1,
			std::numeric_limits<int>::min() / 2,
			std::numeric_limits<int>::max() / 2
		);
		int flipBonus = int(moves[size_t(i)].captured.size());
		if (score > bestScore || (score == bestScore && flipBonus > int(moves[size_t(bestIndex)].captured.size()))) {
			bestScore = score;
			bestIndex = i;
		}
	}
	return moves[size_t(bestIndex)];
}

inline int othelloWinnerForNoLegalMoves(const BoardState& board) {
	int humanCount = othelloCountPiecesForSide(board, HUMAN_SIDE);
	int aiCount = othelloCountPiecesForSide(board, AI_SIDE);
	if (humanCount > aiCount) {
		return HUMAN_SIDE;
	}
	if (aiCount > humanCount) {
		return AI_SIDE;
	}
	return 0;
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

inline int serpentineDiagonalRank(int row, int col, int rowCount, int colCount) {
	int diagonal = row + col;
	int rank = 0;
	for (int d = 0; d < diagonal; ++d) {
		int rowMin = std::max(0, d - (colCount - 1));
		int rowMax = std::min(rowCount - 1, d);
		rank += rowMax - rowMin + 1;
	}
	int rowMin = std::max(0, diagonal - (colCount - 1));
	int rowMax = std::min(rowCount - 1, diagonal);
	int pos = row - rowMin;
	int count = rowMax - rowMin + 1;
	if (diagonal & 1) {
		pos = count - 1 - pos;
	}
	return rank + pos;
}

inline int boardValueForIndex(int boardIndex, int layoutMode) {
	int index = std::max(0, std::min(boardIndex, BOARD_SIZE - 1));
	int mode = std::max(0, std::min(layoutMode, int(BOARD_VALUE_LAYOUT_NAMES.size()) - 1));
	int row = index / 4;
	int posInRow = index % 4;
	switch (mode) {
		case 0: {
			float metric = boardCenterMetric(index);
			int rank = 0;
			for (int i = 0; i < BOARD_SIZE; ++i) {
				if (boardCenterMetric(i) < metric) {
					rank++;
				}
			}
			return rank;
		}
		case 2:
			return posInRow * 8 + row;
		case 3:
			return serpentineDiagonalRank(row, posInRow, 8, 4);
		case 4: {
			int serpentinePos = (row & 1) ? (3 - posInRow) : posInRow;
			return row * 4 + serpentinePos;
		}
		case 5: {
			int serpentineRow = (posInRow & 1) ? (7 - row) : row;
			return posInRow * 8 + serpentineRow;
		}
		case 6:
			return serpentineDiagonalRank(row, posInRow, 8, 4);
		case 1:
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
	return boardValueIndex * pitchDividerForMode(dividerMode);
}

inline float pitchBipolarCenterOffset(int dividerMode, int boardCellCount) {
	return (0.5f * float(std::max(0, boardCellCount - 1))) * pitchDividerForMode(dividerMode);
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

struct IGameRules {
	virtual ~IGameRules() {
	}
	virtual const char* gameId() const = 0;
	virtual int humanSide() const = 0;
	virtual int aiSide() const = 0;
	virtual int boardCellCount() const = 0;
	virtual bool indexToCoord(int index, int* row, int* col) const = 0;
	virtual int coordToIndex(int row, int col) const = 0;
	virtual BoardState makeInitialBoard() const = 0;
	virtual std::vector<Move> generateLegalMovesForSide(const BoardState& sourceBoard, int side) const = 0;
	virtual BoardState applyMoveToBoard(const BoardState& sourceBoard, const Move& move) const = 0;
	virtual Move chooseAiMove(const BoardState& board, int difficulty) const = 0;
	virtual int searchDepthForDifficulty(int difficulty) const = 0;
	virtual int evaluatePosition(const BoardState& sourceBoard) const = 0;
	virtual int evaluateBoardMaterial(const BoardState& sourceBoard) const = 0;
	virtual int winnerForNoLegalMoves(const BoardState& sourceBoard, int sideToMove) const = 0;
};

struct CheckersRules final : IGameRules {
	const char* gameId() const override {
		return "checkers";
	}
	int humanSide() const override {
		return HUMAN_SIDE;
	}
	int aiSide() const override {
		return AI_SIDE;
	}
	int boardCellCount() const override {
		return BOARD_SIZE;
	}
	bool indexToCoord(int index, int* row, int* col) const override {
		return crownstep::indexToCoord(index, row, col);
	}
	int coordToIndex(int row, int col) const override {
		return crownstep::coordToIndex(row, col);
	}
	BoardState makeInitialBoard() const override {
		return crownstep::makeInitialBoard();
	}
	std::vector<Move> generateLegalMovesForSide(const BoardState& sourceBoard, int side) const override {
		return crownstep::generateLegalMovesForSide(sourceBoard, side);
	}
	BoardState applyMoveToBoard(const BoardState& sourceBoard, const Move& move) const override {
		return crownstep::applyMoveToBoard(sourceBoard, move);
	}
	Move chooseAiMove(const BoardState& board, int difficulty) const override {
		return crownstep::chooseAiMove(board, difficulty);
	}
	int searchDepthForDifficulty(int difficulty) const override {
		return crownstep::searchDepthForDifficulty(difficulty);
	}
	int evaluatePosition(const BoardState& sourceBoard) const override {
		return crownstep::evaluatePosition(sourceBoard);
	}
	int evaluateBoardMaterial(const BoardState& sourceBoard) const override {
		return crownstep::evaluateBoardMaterial(sourceBoard);
	}
	int winnerForNoLegalMoves(const BoardState&, int sideToMove) const override {
		return -sideToMove;
	}
};

struct ChessRules final : IGameRules {
	const char* gameId() const override {
		return "chess";
	}
	int humanSide() const override {
		return HUMAN_SIDE;
	}
	int aiSide() const override {
		return AI_SIDE;
	}
	int boardCellCount() const override {
		return CHESS_BOARD_SIZE;
	}
	bool indexToCoord(int index, int* row, int* col) const override {
		return crownstep::chessIndexToCoord(index, row, col);
	}
	int coordToIndex(int row, int col) const override {
		return crownstep::chessCoordToIndex(row, col);
	}
	BoardState makeInitialBoard() const override {
		return crownstep::chessMakeInitialBoard();
	}
	std::vector<Move> generateLegalMovesForSide(const BoardState& sourceBoard, int side) const override {
		return crownstep::chessGenerateLegalMovesForSide(sourceBoard, side);
	}
	BoardState applyMoveToBoard(const BoardState& sourceBoard, const Move& move) const override {
		return crownstep::chessApplyMoveToBoard(sourceBoard, move);
	}
	Move chooseAiMove(const BoardState& board, int difficulty) const override {
		return crownstep::chessChooseAiMove(board, difficulty);
	}
	int searchDepthForDifficulty(int difficulty) const override {
		return crownstep::chessSearchDepthForDifficulty(difficulty);
	}
	int evaluatePosition(const BoardState& sourceBoard) const override {
		return crownstep::chessEvaluatePosition(sourceBoard);
	}
	int evaluateBoardMaterial(const BoardState& sourceBoard) const override {
		return crownstep::chessEvaluateBoardMaterial(sourceBoard);
	}
	int winnerForNoLegalMoves(const BoardState& sourceBoard, int sideToMove) const override {
		return crownstep::chessWinnerForNoLegalMoves(sourceBoard, sideToMove);
	}
};

struct OthelloRules final : IGameRules {
	const char* gameId() const override {
		return "othello";
	}
	int humanSide() const override {
		return HUMAN_SIDE;
	}
	int aiSide() const override {
		return AI_SIDE;
	}
	int boardCellCount() const override {
		return OTHELLO_BOARD_SIZE;
	}
	bool indexToCoord(int index, int* row, int* col) const override {
		return crownstep::othelloIndexToCoord(index, row, col);
	}
	int coordToIndex(int row, int col) const override {
		return crownstep::othelloCoordToIndex(row, col);
	}
	BoardState makeInitialBoard() const override {
		return crownstep::othelloMakeInitialBoard();
	}
	std::vector<Move> generateLegalMovesForSide(const BoardState& sourceBoard, int side) const override {
		return crownstep::othelloGenerateLegalMovesForSide(sourceBoard, side);
	}
	BoardState applyMoveToBoard(const BoardState& sourceBoard, const Move& move) const override {
		return crownstep::othelloApplyMoveToBoard(sourceBoard, move);
	}
	Move chooseAiMove(const BoardState& board, int difficulty) const override {
		return crownstep::othelloChooseAiMove(board, difficulty);
	}
	int searchDepthForDifficulty(int difficulty) const override {
		return crownstep::othelloSearchDepthForDifficulty(difficulty);
	}
	int evaluatePosition(const BoardState& sourceBoard) const override {
		return crownstep::othelloEvaluatePosition(sourceBoard);
	}
	int evaluateBoardMaterial(const BoardState& sourceBoard) const override {
		return crownstep::othelloEvaluateBoardMaterial(sourceBoard);
	}
	int winnerForNoLegalMoves(const BoardState& sourceBoard, int) const override {
		return crownstep::othelloWinnerForNoLegalMoves(sourceBoard);
	}
};

inline const IGameRules& checkersRules() {
	static CheckersRules rules;
	return rules;
}

inline const IGameRules& chessRules() {
	static ChessRules rules;
	return rules;
}

inline const IGameRules& othelloRules() {
	static OthelloRules rules;
	return rules;
}

} // namespace crownstep
