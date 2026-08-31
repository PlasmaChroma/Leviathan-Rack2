#pragma once

#include <string>

namespace iris_worker {

enum class CompletionKind {
  Failure,
  Success,
  PartialSuccess,
};

inline CompletionKind classifyCompletion(bool buildSucceeded, const std::string& diagnostic) {
  if (!buildSucceeded) return CompletionKind::Failure;
  return diagnostic.empty() ? CompletionKind::Success : CompletionKind::PartialSuccess;
}

inline bool shouldPublish(CompletionKind completion) {
  return completion != CompletionKind::Failure;
}

inline bool shouldReportFailure(CompletionKind completion) {
  return completion != CompletionKind::Success;
}

} // namespace iris_worker
