#include "../src/CrownstepCore.hpp"

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

namespace {

using crownstep::AI_SIDE;
using crownstep::BOARD_SIZE;
using crownstep::HUMAN_SIDE;
using crownstep::Move;

struct TestResult {
  std::string name;
  bool pass = false;
  std::string detail;
};

std::array<int, BOARD_SIZE> emptyBoard() {
  std::array<int, BOARD_SIZE> board {};
  board.fill(0);
  return board;
}

TestResult testInitialBoardHasExpectedPieceCounts() {
  std::array<int, BOARD_SIZE> board = crownstep::makeInitialBoard();
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
  std::array<int, BOARD_SIZE> board = emptyBoard();
  board[size_t(crownstep::coordToIndex(5, 0))] = 1;
  board[size_t(crownstep::coordToIndex(4, 1))] = -1;

  std::vector<Move> moves = crownstep::generateLegalMovesForSide(board, HUMAN_SIDE);
  bool pass = moves.size() == 1 && moves[0].isCapture && moves[0].destinationIndex == crownstep::coordToIndex(3, 2);
  return {"Forced capture removes non-capturing options", pass,
          "moveCount=" + std::to_string(moves.size())};
}

TestResult testMultiCaptureChainIsGeneratedAsSingleMove() {
  std::array<int, BOARD_SIZE> board = emptyBoard();
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
  std::array<int, BOARD_SIZE> board = emptyBoard();
  board[size_t(crownstep::coordToIndex(1, 2))] = 1;

  std::vector<Move> moves = crownstep::generateLegalMovesForSide(board, HUMAN_SIDE);
  bool foundPromotion = false;
  std::array<int, BOARD_SIZE> promotedBoard = board;
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
  std::array<int, BOARD_SIZE> board = emptyBoard();
  board[size_t(crownstep::coordToIndex(2, 1))] = -1;
  board[size_t(crownstep::coordToIndex(3, 2))] = 1;

  Move move = crownstep::chooseAiMove(board, 1);
  bool pass = move.isCapture && move.destinationIndex == crownstep::coordToIndex(4, 3);
  return {"AI respects mandatory capture", pass,
          "dest=" + std::to_string(move.destinationIndex)};
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
