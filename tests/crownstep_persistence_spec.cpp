#include "../src/CrownstepShared.hpp"

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

namespace {

using crownstep::Move;
using crownstep::Step;

struct TestResult {
  std::string name;
  bool pass = false;
  std::string detail;
};

bool nearlyEqual(float a, float b, float eps = 1e-6f) {
  return std::fabs(a - b) <= eps;
}

TestResult testCrownstepStateJsonRoundTrip() {
  Crownstep source;
  source.setGameMode(Crownstep::GAME_MODE_CHESS, true);

  source.turnSide = source.aiSide();
  source.winnerSide = 0;
  source.selectedSquare = crownstep::chessCoordToIndex(6, 4);
  source.aiDifficulty = 2;
  source.quantizationEnabled = false;
  source.pitchBipolarEnabled = true;
  source.melodicBiasEnabled = true;
  source.pitchInterpretationMode = 2;
  source.boardValueLayoutMode = 1;
  source.pitchDividerMode = 3;
  source.boardTextureMode = Crownstep::BOARD_TEXTURE_MARBLE;
  source.playhead = 7;
  source.gameOver = false;
  source.lastMoveSide = source.aiSide();

  source.board.fill(0);
  source.board[size_t(crownstep::chessCoordToIndex(7, 4))] = crownstep::CHESS_KING;
  source.board[size_t(crownstep::chessCoordToIndex(0, 4))] = -crownstep::CHESS_KING;
  source.board[size_t(crownstep::chessCoordToIndex(6, 4))] = crownstep::CHESS_PAWN;
  source.board[size_t(crownstep::chessCoordToIndex(4, 4))] = -crownstep::CHESS_PAWN;
  source.board[size_t(crownstep::chessCoordToIndex(0, 0))] = -crownstep::CHESS_ROOK;

  source.chessState.whiteCanCastleKingSide = false;
  source.chessState.whiteCanCastleQueenSide = true;
  source.chessState.blackCanCastleKingSide = false;
  source.chessState.blackCanCastleQueenSide = true;
  source.chessState.enPassantTargetIndex = crownstep::chessCoordToIndex(5, 4);

  Move move;
  move.originIndex = crownstep::chessCoordToIndex(6, 4);
  move.destinationIndex = crownstep::chessCoordToIndex(4, 4);
  move.path = {move.destinationIndex};
  move.captured = {};
  move.isCapture = false;
  move.isMultiCapture = false;
  move.isKing = false;

  Step step;
  step.pitch = 2.5f;
  step.gate = true;
  step.accent = 1.25f;
  step.mod = 0.42f;

  {
    std::lock_guard<std::recursive_mutex> lock(source.sequenceMutex);
    source.history.clear();
    source.moveHistory.clear();
    source.history.push_back(step);
    source.moveHistory.push_back(move);
    source.lastMove = move;
  }

  json_t* serialized = source.dataToJson();
  if (!serialized) {
    return {"Crownstep JSON round-trip", false, "dataToJson returned null"};
  }

  Crownstep loaded;
  loaded.dataFromJson(serialized);
  json_decref(serialized);

  bool gameModeOk = loaded.gameMode == Crownstep::GAME_MODE_CHESS;
  bool turnSideOk = loaded.turnSide == loaded.aiSide();
  bool winnerSideOk = loaded.winnerSide == source.winnerSide;
  bool selectedSquareOk = loaded.selectedSquare == source.selectedSquare;
  bool aiDifficultyOk = loaded.aiDifficulty == source.aiDifficulty;
  bool quantizationOk = loaded.quantizationEnabled == source.quantizationEnabled;
  bool pitchBipolarOk = loaded.pitchBipolarEnabled == source.pitchBipolarEnabled;
  bool melodicBiasOk = loaded.melodicBiasEnabled == source.melodicBiasEnabled;
  bool pitchInterpretationOk = loaded.pitchInterpretationMode == source.pitchInterpretationMode;
  bool boardValueLayoutOk = loaded.boardValueLayoutMode == source.boardValueLayoutMode;
  bool pitchDividerOk = loaded.pitchDividerMode == source.pitchDividerMode;
  bool boardTextureOk = loaded.boardTextureMode == source.boardTextureMode;
  bool playheadOk = loaded.playhead == source.playhead;
  bool gameOverOk = loaded.gameOver == source.gameOver;
  bool lastMoveSideOk = loaded.lastMoveSide == loaded.aiSide();
  bool scalarOk =
    gameModeOk &&
    turnSideOk &&
    winnerSideOk &&
    selectedSquareOk &&
    aiDifficultyOk &&
    quantizationOk &&
    pitchBipolarOk &&
    melodicBiasOk &&
    pitchInterpretationOk &&
    boardValueLayoutOk &&
    pitchDividerOk &&
    boardTextureOk &&
    playheadOk &&
    gameOverOk &&
    lastMoveSideOk;

  bool boardOk =
    loaded.board[size_t(crownstep::chessCoordToIndex(7, 4))] == crownstep::CHESS_KING &&
    loaded.board[size_t(crownstep::chessCoordToIndex(0, 4))] == -crownstep::CHESS_KING &&
    loaded.board[size_t(crownstep::chessCoordToIndex(6, 4))] == crownstep::CHESS_PAWN &&
    loaded.board[size_t(crownstep::chessCoordToIndex(4, 4))] == -crownstep::CHESS_PAWN &&
    loaded.board[size_t(crownstep::chessCoordToIndex(0, 0))] == -crownstep::CHESS_ROOK;

  bool chessStateOk =
    loaded.chessState.whiteCanCastleKingSide == source.chessState.whiteCanCastleKingSide &&
    loaded.chessState.whiteCanCastleQueenSide == source.chessState.whiteCanCastleQueenSide &&
    loaded.chessState.blackCanCastleKingSide == source.chessState.blackCanCastleKingSide &&
    loaded.chessState.blackCanCastleQueenSide == source.chessState.blackCanCastleQueenSide &&
    loaded.chessState.enPassantTargetIndex == source.chessState.enPassantTargetIndex;

  bool moveHistoryOk = false;
  bool historyOk = false;
  {
    std::lock_guard<std::recursive_mutex> lock(loaded.sequenceMutex);
    moveHistoryOk = loaded.moveHistory.size() == 1 &&
      loaded.moveHistory[0].originIndex == move.originIndex &&
      loaded.moveHistory[0].destinationIndex == move.destinationIndex &&
      loaded.moveHistory[0].path == move.path &&
      loaded.moveHistory[0].captured == move.captured &&
      loaded.lastMove.originIndex == move.originIndex &&
      loaded.lastMove.destinationIndex == move.destinationIndex;
    historyOk = loaded.history.size() == 1 &&
      nearlyEqual(loaded.history[0].pitch, step.pitch) &&
      loaded.history[0].gate == step.gate &&
      nearlyEqual(loaded.history[0].accent, step.accent) &&
      nearlyEqual(loaded.history[0].mod, step.mod);
  }

  bool pass = scalarOk && boardOk && chessStateOk && moveHistoryOk && historyOk;
  return {"Crownstep JSON round-trip preserves key state", pass,
          "scalarOk=" + std::to_string(scalarOk ? 1 : 0) +
            " gameModeOk=" + std::to_string(gameModeOk ? 1 : 0) +
            " turnSideOk=" + std::to_string(turnSideOk ? 1 : 0) +
            " winnerSideOk=" + std::to_string(winnerSideOk ? 1 : 0) +
            " selectedSquareOk=" + std::to_string(selectedSquareOk ? 1 : 0) +
            " aiDifficultyOk=" + std::to_string(aiDifficultyOk ? 1 : 0) +
            " quantizationOk=" + std::to_string(quantizationOk ? 1 : 0) +
            " pitchBipolarOk=" + std::to_string(pitchBipolarOk ? 1 : 0) +
            " melodicBiasOk=" + std::to_string(melodicBiasOk ? 1 : 0) +
            " pitchInterpretationOk=" + std::to_string(pitchInterpretationOk ? 1 : 0) +
            " boardValueLayoutOk=" + std::to_string(boardValueLayoutOk ? 1 : 0) +
            " pitchDividerOk=" + std::to_string(pitchDividerOk ? 1 : 0) +
            " boardTextureOk=" + std::to_string(boardTextureOk ? 1 : 0) +
            " playheadOk=" + std::to_string(playheadOk ? 1 : 0) +
            " gameOverOk=" + std::to_string(gameOverOk ? 1 : 0) +
            " lastMoveSideOk=" + std::to_string(lastMoveSideOk ? 1 : 0) +
            " boardOk=" + std::to_string(boardOk ? 1 : 0) +
            " chessStateOk=" + std::to_string(chessStateOk ? 1 : 0) +
            " moveHistoryOk=" + std::to_string(moveHistoryOk ? 1 : 0) +
            " historyOk=" + std::to_string(historyOk ? 1 : 0)};
}

} // namespace

int main() {
  std::vector<TestResult> tests;
  tests.push_back(testCrownstepStateJsonRoundTrip());

  int failed = 0;
  std::cout << "Crownstep Persistence Spec\n";
  std::cout << "--------------------------\n";
  for (const auto& t : tests) {
    std::cout << (t.pass ? "[PASS] " : "[FAIL] ") << t.name << " :: " << t.detail << "\n";
    if (!t.pass) {
      failed++;
    }
  }
  std::cout << "--------------------------\n";
  std::cout << "Summary: " << (tests.size() - failed) << "/" << tests.size() << " passed\n";
  return failed == 0 ? 0 : 1;
}
