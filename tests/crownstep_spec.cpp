#include "../src/CrownstepCore.hpp"

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

namespace {

using crownstep::AI_SIDE;
using crownstep::BOARD_SIZE;
using crownstep::BoardState;
using crownstep::HUMAN_SIDE;
using crownstep::Move;

struct TestResult {
  std::string name;
  bool pass = false;
  std::string detail;
};

BoardState emptyBoard() {
  BoardState board {};
  board.fill(0);
  return board;
}

TestResult testInitialBoardHasExpectedPieceCounts() {
  BoardState board = crownstep::makeInitialBoard();
  int human = 0;
  int ai = 0;
  for (int piece : board) {
    if (piece > 0) {
      human++;
    }
    if (piece < 0) {
      ai++;
    }
  }
  bool pass = human == 12 && ai == 12;
  return {"Initial board has 12 pieces per side", pass,
          "human=" + std::to_string(human) + " ai=" + std::to_string(ai)};
}

TestResult testForcedCaptureSuppressesSimpleMoves() {
  BoardState board = emptyBoard();
  board[size_t(crownstep::coordToIndex(5, 0))] = 1;
  board[size_t(crownstep::coordToIndex(4, 1))] = -1;

  std::vector<Move> moves = crownstep::generateLegalMovesForSide(board, HUMAN_SIDE);
  bool pass = moves.size() == 1 && moves[0].isCapture && moves[0].destinationIndex == crownstep::coordToIndex(3, 2);
  return {"Forced capture removes non-capturing options", pass,
          "moveCount=" + std::to_string(moves.size())};
}

TestResult testMultiCaptureChainIsGeneratedAsSingleMove() {
  BoardState board = emptyBoard();
  board[size_t(crownstep::coordToIndex(5, 0))] = 1;
  board[size_t(crownstep::coordToIndex(4, 1))] = -1;
  board[size_t(crownstep::coordToIndex(2, 3))] = -1;

  std::vector<Move> moves = crownstep::generateLegalMovesForSide(board, HUMAN_SIDE);
  bool pass = moves.size() == 1 && moves[0].isMultiCapture && moves[0].captured.size() == 2 &&
              moves[0].destinationIndex == crownstep::coordToIndex(1, 4);
  return {"Multi-capture is emitted as one compound move", pass,
          "captured=" + std::to_string(moves.empty() ? 0 : moves[0].captured.size())};
}

TestResult testPromotionSetsKingFlagAndBoardState() {
  BoardState board = emptyBoard();
  board[size_t(crownstep::coordToIndex(1, 2))] = 1;

  std::vector<Move> moves = crownstep::generateLegalMovesForSide(board, HUMAN_SIDE);
  bool foundPromotion = false;
  BoardState promotedBoard = board;
  for (const Move& move : moves) {
    if (move.destinationIndex == crownstep::coordToIndex(0, 1)) {
      foundPromotion = move.isKing;
      promotedBoard = crownstep::applyMoveToBoard(board, move);
      break;
    }
  }

  int promotedPiece = promotedBoard[size_t(crownstep::coordToIndex(0, 1))];
  bool pass = foundPromotion && promotedPiece == 2;
  return {"Promotion marks move and upgrades piece", pass,
          "piece=" + std::to_string(promotedPiece)};
}

TestResult testStepMappingMatchesSpecValues() {
  Move move;
  move.originIndex = 10;
  move.destinationIndex = 14;
  move.isCapture = true;
  move.isMultiCapture = false;
  move.isKing = false;

  crownstep::Step step = crownstep::makeStepFromMove(move, 0, 0, 0.f);
  bool pitchOk = std::fabs(step.pitch - 1.4166666f) < 1e-4f;
  bool accentOk = std::fabs(step.accent - 1.f) < 1e-6f;
  bool modOk = std::fabs(step.mod - 0.5f) < 1e-6f;
  bool pass = pitchOk && accentOk && modOk;
  return {"Move-to-step mapping follows pitch/accent/mod spec", pass,
          "pitch=" + std::to_string(step.pitch) + " accent=" + std::to_string(step.accent) +
            " mod=" + std::to_string(step.mod)};
}

TestResult testSequenceWindowUsesRecentHistorySlice() {
  int length = crownstep::activeLength(20, 8);
  int start = crownstep::activeStartIndex(20, 8);
  bool pass = length == 8 && start == 12;
  return {"Active window slices recent history", pass,
          "length=" + std::to_string(length) + " start=" + std::to_string(start)};
}

TestResult testAiChoosesAvailableCapture() {
  BoardState board = emptyBoard();
  board[size_t(crownstep::coordToIndex(2, 1))] = -1;
  board[size_t(crownstep::coordToIndex(3, 2))] = 1;

  Move move = crownstep::chooseAiMove(board, 1);
  bool pass = move.isCapture && move.destinationIndex == crownstep::coordToIndex(4, 3);
  return {"AI respects mandatory capture", pass,
          "dest=" + std::to_string(move.destinationIndex)};
}

TestResult testPitchDividerModesScaleBoardValuesAndCenterOffset() {
  bool pass = true;
  const float sourceValue = 21.f;
  const std::array<float, 4> expectedDivided = {{21.f, 10.5f, 7.f, 5.25f}};
  const std::array<float, 4> expectedCenter = {{15.5f, 7.75f, 5.1666665f, 3.875f}};
  for (int mode = 0; mode < 4; ++mode) {
    float divided = crownstep::applyPitchDividerToBoardValue(sourceValue, mode);
    float center = crownstep::pitchBipolarCenterOffset(mode);
    if (std::fabs(divided - expectedDivided[size_t(mode)]) > 1e-6f ||
        std::fabs(center - expectedCenter[size_t(mode)]) > 1e-6f) {
      pass = false;
      break;
    }
  }
  return {"Pitch divider modes scale value + bipolar center", pass,
          "modes full/half/third/quarter"};
}

TestResult testCheckersRulesAdapterMatchesCoreFunctions() {
  const crownstep::IGameRules& rules = crownstep::checkersRules();
  crownstep::BoardState board = rules.makeInitialBoard();
  std::vector<Move> direct = crownstep::generateLegalMovesForSide(board, crownstep::HUMAN_SIDE);
  std::vector<Move> adapted = rules.generateLegalMovesForSide(board, rules.humanSide());
  bool countMatch = direct.size() == adapted.size();
  bool cellCountMatch = rules.boardCellCount() == crownstep::BOARD_SIZE;
  bool sideMatch = (rules.humanSide() == crownstep::HUMAN_SIDE) && (rules.aiSide() == crownstep::AI_SIDE);
  return {"Checkers adapter parity with core APIs", countMatch && cellCountMatch && sideMatch,
          "moves=" + std::to_string(adapted.size())};
}

TestResult testChessInitialBoardAndMoveCount() {
  const crownstep::IGameRules& rules = crownstep::chessRules();
  BoardState board = rules.makeInitialBoard();
  int white = 0;
  int black = 0;
  for (int i = 0; i < rules.boardCellCount(); ++i) {
    int piece = board[size_t(i)];
    if (piece > 0) {
      white++;
    } else if (piece < 0) {
      black++;
    }
  }
  std::vector<Move> whiteMoves = rules.generateLegalMovesForSide(board, HUMAN_SIDE);
  bool pass = (white == 16) && (black == 16) && (whiteMoves.size() == 20);
  return {"Chess initial board has pieces + 20 opening moves", pass,
          "white=" + std::to_string(white) + " black=" + std::to_string(black) +
            " moves=" + std::to_string(whiteMoves.size())};
}

TestResult testChessPinnedPieceCannotExposeKing() {
  const crownstep::IGameRules& rules = crownstep::chessRules();
  BoardState board {};
  board.fill(0);
  board[size_t(crownstep::chessCoordToIndex(7, 4))] = crownstep::CHESS_KING;
  board[size_t(crownstep::chessCoordToIndex(6, 4))] = crownstep::CHESS_ROOK;
  board[size_t(crownstep::chessCoordToIndex(0, 4))] = -crownstep::CHESS_ROOK;
  board[size_t(crownstep::chessCoordToIndex(0, 0))] = -crownstep::CHESS_KING;

  std::vector<Move> moves = rules.generateLegalMovesForSide(board, HUMAN_SIDE);
  int illegalDestination = crownstep::chessCoordToIndex(6, 5); // e2 -> f2 would expose the king.
  bool foundIllegal = false;
  for (const Move& move : moves) {
    if (move.originIndex == crownstep::chessCoordToIndex(6, 4) && move.destinationIndex == illegalDestination) {
      foundIllegal = true;
      break;
    }
  }
  return {"Chess legal filtering prevents pinned-piece self-check", !foundIllegal,
          "moves=" + std::to_string(moves.size())};
}

TestResult testChessStalemateIsDraw() {
  const crownstep::IGameRules& rules = crownstep::chessRules();
  BoardState board {};
  board.fill(0);
  board[size_t(crownstep::chessCoordToIndex(7, 7))] = crownstep::CHESS_KING;       // Human king h1
  board[size_t(crownstep::chessCoordToIndex(6, 5))] = -crownstep::CHESS_KING;      // AI king f2
  board[size_t(crownstep::chessCoordToIndex(5, 6))] = -crownstep::CHESS_QUEEN;     // AI queen g3

  std::vector<Move> humanMoves = rules.generateLegalMovesForSide(board, HUMAN_SIDE);
  int winner = rules.winnerForNoLegalMoves(board, HUMAN_SIDE);
  bool pass = humanMoves.empty() && winner == 0;
  return {"Chess stalemate reports draw winner=0", pass,
          "moves=" + std::to_string(humanMoves.size()) + " winner=" + std::to_string(winner)};
}

TestResult testChessCheckmateReportsWinner() {
  const crownstep::IGameRules& rules = crownstep::chessRules();
  BoardState board {};
  board.fill(0);
  board[size_t(crownstep::chessCoordToIndex(7, 7))] = crownstep::CHESS_KING;       // Human king h1
  board[size_t(crownstep::chessCoordToIndex(5, 5))] = -crownstep::CHESS_KING;      // AI king f3
  board[size_t(crownstep::chessCoordToIndex(6, 6))] = -crownstep::CHESS_QUEEN;     // AI queen g2

  std::vector<Move> humanMoves = rules.generateLegalMovesForSide(board, HUMAN_SIDE);
  int winner = rules.winnerForNoLegalMoves(board, HUMAN_SIDE);
  bool pass = humanMoves.empty() && winner == AI_SIDE;
  return {"Chess checkmate reports opposing winner", pass,
          "moves=" + std::to_string(humanMoves.size()) + " winner=" + std::to_string(winner)};
}

TestResult testChessPromotionAutoQueensPawn() {
  const crownstep::IGameRules& rules = crownstep::chessRules();
  BoardState board {};
  board.fill(0);
  board[size_t(crownstep::chessCoordToIndex(7, 4))] = crownstep::CHESS_KING;
  board[size_t(crownstep::chessCoordToIndex(0, 4))] = -crownstep::CHESS_KING;
  board[size_t(crownstep::chessCoordToIndex(1, 0))] = crownstep::CHESS_PAWN;

  std::vector<Move> moves = rules.generateLegalMovesForSide(board, HUMAN_SIDE);
  Move promotionMove;
  bool found = false;
  for (const Move& move : moves) {
    if (move.originIndex == crownstep::chessCoordToIndex(1, 0) &&
        move.destinationIndex == crownstep::chessCoordToIndex(0, 0)) {
      promotionMove = move;
      found = true;
      break;
    }
  }
  BoardState promoted = found ? rules.applyMoveToBoard(board, promotionMove) : board;
  int promotedPiece = promoted[size_t(crownstep::chessCoordToIndex(0, 0))];
  bool pass = found && promotedPiece == crownstep::CHESS_QUEEN;
  return {"Chess pawn promotion auto-queens", pass,
          "found=" + std::to_string(found ? 1 : 0) + " piece=" + std::to_string(promotedPiece)};
}

TestResult testChessKingCannotCaptureEnemyKing() {
  BoardState board {};
  board.fill(0);
  board[size_t(crownstep::chessCoordToIndex(7, 4))] = crownstep::CHESS_KING;
  board[size_t(crownstep::chessCoordToIndex(6, 4))] = -crownstep::CHESS_KING;
  crownstep::ChessState state = crownstep::chessInitialState();

  std::vector<Move> moves = crownstep::chessGenerateLegalMovesForSide(board, HUMAN_SIDE, state);
  bool canCaptureEnemyKing = false;
  for (const Move& move : moves) {
    if (move.originIndex == crownstep::chessCoordToIndex(7, 4) &&
        move.destinationIndex == crownstep::chessCoordToIndex(6, 4)) {
      canCaptureEnemyKing = true;
      break;
    }
  }
  return {"Chess king cannot capture enemy king", !canCaptureEnemyKing,
          "moves=" + std::to_string(moves.size())};
}

TestResult testChessCastlingMovesAndRookShift() {
  BoardState board {};
  board.fill(0);
  board[size_t(crownstep::chessCoordToIndex(7, 4))] = crownstep::CHESS_KING;
  board[size_t(crownstep::chessCoordToIndex(7, 0))] = crownstep::CHESS_ROOK;
  board[size_t(crownstep::chessCoordToIndex(7, 7))] = crownstep::CHESS_ROOK;
  board[size_t(crownstep::chessCoordToIndex(0, 4))] = -crownstep::CHESS_KING;
  crownstep::ChessState state = crownstep::chessInitialState();

  std::vector<Move> moves = crownstep::chessGenerateLegalMovesForSide(board, HUMAN_SIDE, state);
  Move kingSideCastle;
  bool foundKingSide = false;
  bool foundQueenSide = false;
  for (const Move& move : moves) {
    if (move.originIndex == crownstep::chessCoordToIndex(7, 4) &&
        move.destinationIndex == crownstep::chessCoordToIndex(7, 6)) {
      kingSideCastle = move;
      foundKingSide = true;
    }
    if (move.originIndex == crownstep::chessCoordToIndex(7, 4) &&
        move.destinationIndex == crownstep::chessCoordToIndex(7, 2)) {
      foundQueenSide = true;
    }
  }

  crownstep::ChessState nextState = state;
  BoardState nextBoard = foundKingSide
    ? crownstep::chessApplyMoveToBoard(board, kingSideCastle, state, &nextState)
    : board;
  bool rookShifted = nextBoard[size_t(crownstep::chessCoordToIndex(7, 5))] == crownstep::CHESS_ROOK &&
                     nextBoard[size_t(crownstep::chessCoordToIndex(7, 7))] == 0;
  bool kingShifted = nextBoard[size_t(crownstep::chessCoordToIndex(7, 6))] == crownstep::CHESS_KING;
  bool rightsCleared = !nextState.whiteCanCastleKingSide && !nextState.whiteCanCastleQueenSide;
  bool pass = foundKingSide && foundQueenSide && kingShifted && rookShifted && rightsCleared;
  return {"Chess castling legal + rook shifts correctly", pass,
          "k=" + std::to_string(foundKingSide ? 1 : 0) + " q=" + std::to_string(foundQueenSide ? 1 : 0)};
}

TestResult testChessEnPassantImmediateCapture() {
  BoardState board {};
  board.fill(0);
  board[size_t(crownstep::chessCoordToIndex(7, 4))] = crownstep::CHESS_KING;
  board[size_t(crownstep::chessCoordToIndex(0, 4))] = -crownstep::CHESS_KING;
  board[size_t(crownstep::chessCoordToIndex(6, 4))] = crownstep::CHESS_PAWN;   // White pawn e2
  board[size_t(crownstep::chessCoordToIndex(4, 3))] = -crownstep::CHESS_PAWN;  // Black pawn d4
  crownstep::ChessState state = crownstep::chessInitialState();

  std::vector<Move> whiteMoves = crownstep::chessGenerateLegalMovesForSide(board, HUMAN_SIDE, state);
  Move whiteDoublePush;
  bool foundDoublePush = false;
  for (const Move& move : whiteMoves) {
    if (move.originIndex == crownstep::chessCoordToIndex(6, 4) &&
        move.destinationIndex == crownstep::chessCoordToIndex(4, 4)) {
      whiteDoublePush = move;
      foundDoublePush = true;
      break;
    }
  }

  crownstep::ChessState afterWhite = state;
  BoardState boardAfterWhite = foundDoublePush
    ? crownstep::chessApplyMoveToBoard(board, whiteDoublePush, state, &afterWhite)
    : board;

  std::vector<Move> blackMoves = crownstep::chessGenerateLegalMovesForSide(boardAfterWhite, AI_SIDE, afterWhite);
  Move enPassantMove;
  bool foundEnPassant = false;
  int capturedIndex = -1;
  for (const Move& move : blackMoves) {
    if (move.originIndex == crownstep::chessCoordToIndex(4, 3) &&
        move.destinationIndex == crownstep::chessCoordToIndex(5, 4) &&
        !move.captured.empty()) {
      enPassantMove = move;
      capturedIndex = move.captured[0];
      foundEnPassant = true;
      break;
    }
  }

  crownstep::ChessState afterBlack = afterWhite;
  BoardState boardAfterBlack = foundEnPassant
    ? crownstep::chessApplyMoveToBoard(boardAfterWhite, enPassantMove, afterWhite, &afterBlack)
    : boardAfterWhite;
  bool destinationHasBlackPawn =
    boardAfterBlack[size_t(crownstep::chessCoordToIndex(5, 4))] == -crownstep::CHESS_PAWN;
  bool whitePawnRemoved = boardAfterBlack[size_t(crownstep::chessCoordToIndex(4, 4))] == 0;
  bool pass = foundDoublePush && foundEnPassant &&
              capturedIndex == crownstep::chessCoordToIndex(4, 4) &&
              destinationHasBlackPawn && whitePawnRemoved;
  return {"Chess en passant is generated + applied immediately", pass,
          "doublePush=" + std::to_string(foundDoublePush ? 1 : 0) +
            " ep=" + std::to_string(foundEnPassant ? 1 : 0)};
}

TestResult testOthelloInitialBoardAndMoves() {
  const crownstep::IGameRules& rules = crownstep::othelloRules();
  BoardState board = rules.makeInitialBoard();
  int human = 0;
  int ai = 0;
  for (int i = 0; i < rules.boardCellCount(); ++i) {
    int piece = board[size_t(i)];
    if (piece == HUMAN_SIDE) {
      human++;
    } else if (piece == AI_SIDE) {
      ai++;
    }
  }
  std::vector<Move> moves = rules.generateLegalMovesForSide(board, HUMAN_SIDE);
  bool pass = (human == 2) && (ai == 2) && (moves.size() == 4);
  return {"Othello initial board has 2 discs per side + 4 legal moves", pass,
          "human=" + std::to_string(human) + " ai=" + std::to_string(ai) +
            " moves=" + std::to_string(moves.size())};
}

TestResult testOthelloMoveFlipsBracketedDiscs() {
  const crownstep::IGameRules& rules = crownstep::othelloRules();
  BoardState board = rules.makeInitialBoard();
  std::vector<Move> moves = rules.generateLegalMovesForSide(board, HUMAN_SIDE);
  int target = crownstep::othelloCoordToIndex(2, 3);
  Move chosen;
  bool found = false;
  for (const Move& move : moves) {
    if (move.destinationIndex == target) {
      chosen = move;
      found = true;
      break;
    }
  }
  BoardState after = found ? crownstep::othelloApplyMoveToBoard(board, chosen, HUMAN_SIDE) : board;
  bool placed = after[size_t(target)] == HUMAN_SIDE;
  bool flipped = after[size_t(crownstep::othelloCoordToIndex(3, 3))] == HUMAN_SIDE;
  bool pass = found && placed && flipped && chosen.isCapture && !chosen.captured.empty();
  return {"Othello move places disc and flips bracketed line", pass,
          "found=" + std::to_string(found ? 1 : 0) +
            " flips=" + std::to_string(chosen.captured.size())};
}

TestResult testOthelloWinnerByDiscCount() {
  BoardState board {};
  board.fill(0);
  board[size_t(crownstep::othelloCoordToIndex(0, 0))] = HUMAN_SIDE;
  board[size_t(crownstep::othelloCoordToIndex(0, 1))] = HUMAN_SIDE;
  board[size_t(crownstep::othelloCoordToIndex(0, 2))] = AI_SIDE;
  int winner = crownstep::othelloWinnerForNoLegalMoves(board);
  return {"Othello winner resolution uses disc majority", winner == HUMAN_SIDE,
          "winner=" + std::to_string(winner)};
}

} // namespace

int main() {
  std::vector<TestResult> tests;
  tests.push_back(testInitialBoardHasExpectedPieceCounts());
  tests.push_back(testForcedCaptureSuppressesSimpleMoves());
  tests.push_back(testMultiCaptureChainIsGeneratedAsSingleMove());
  tests.push_back(testPromotionSetsKingFlagAndBoardState());
  tests.push_back(testStepMappingMatchesSpecValues());
  tests.push_back(testSequenceWindowUsesRecentHistorySlice());
  tests.push_back(testAiChoosesAvailableCapture());
  tests.push_back(testPitchDividerModesScaleBoardValuesAndCenterOffset());
  tests.push_back(testCheckersRulesAdapterMatchesCoreFunctions());
  tests.push_back(testChessInitialBoardAndMoveCount());
  tests.push_back(testChessPinnedPieceCannotExposeKing());
  tests.push_back(testChessStalemateIsDraw());
  tests.push_back(testChessCheckmateReportsWinner());
  tests.push_back(testChessPromotionAutoQueensPawn());
  tests.push_back(testChessKingCannotCaptureEnemyKing());
  tests.push_back(testChessCastlingMovesAndRookShift());
  tests.push_back(testChessEnPassantImmediateCapture());
  tests.push_back(testOthelloInitialBoardAndMoves());
  tests.push_back(testOthelloMoveFlipsBracketedDiscs());
  tests.push_back(testOthelloWinnerByDiscCount());

  int failed = 0;
  std::cout << "Crownstep Spec\n";
  std::cout << "--------------\n";
  for (const auto& t : tests) {
    std::cout << (t.pass ? "[PASS] " : "[FAIL] ") << t.name << " :: " << t.detail << "\n";
    if (!t.pass) {
      failed++;
    }
  }
  std::cout << "--------------\n";
  std::cout << "Summary: " << (tests.size() - failed) << "/" << tests.size() << " passed\n";
  return failed == 0 ? 0 : 1;
}
