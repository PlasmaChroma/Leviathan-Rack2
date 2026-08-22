#pragma once

#include <condition_variable>
#include <cstdint>
#include <deque>
#include <memory>
#include <mutex>
#include <string>

namespace octavia_console {

constexpr std::size_t kMaxPromptChars = 4096;
constexpr std::size_t kMaxResponseChars = 16384;
constexpr std::size_t kMaxPendingPrompts = 8;

enum class AgentState {
	OFFLINE,
	READY,
	ARMED,
	WORKING,
	REPLY,
	ERROR
};

const char* agentStateName(AgentState state);

struct Prompt {
	uint64_t id = 0;
	std::string text;
};

struct Snapshot {
	AgentState state = AgentState::OFFLINE;
	uint64_t latestPromptId = 0;
	uint64_t latestResponsePromptId = 0;
	std::size_t pendingCount = 0;
	std::string response;
	std::string error;
};

class Mailbox {
public:
	bool submitPrompt(std::string text, uint64_t* promptId = nullptr, std::string* error = nullptr);
	bool waitForPrompt(uint64_t afterId, int waitMs, Prompt* prompt);
	bool postResponse(uint64_t promptId, std::string text, bool isError, std::string* error = nullptr);
	void setAgentState(AgentState state);
	void setError(std::string error);
	Snapshot snapshot() const;

private:
	mutable std::mutex mutex_;
	std::condition_variable promptReady_;
	std::deque<Prompt> prompts_;
	uint64_t nextPromptId_ = 1;
	uint64_t latestResponsePromptId_ = 0;
	AgentState state_ = AgentState::READY;
	std::string response_;
	std::string error_;
};

void registerMailbox(int64_t moduleId, const std::shared_ptr<Mailbox>& mailbox);
void unregisterMailbox(int64_t moduleId, const std::shared_ptr<Mailbox>& mailbox);
std::shared_ptr<Mailbox> findMailbox(int64_t moduleId);

} // namespace octavia_console
