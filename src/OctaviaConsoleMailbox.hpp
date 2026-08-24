#pragma once

#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <deque>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace octavia_console {

constexpr std::size_t kMaxPromptChars = 4096;
constexpr std::size_t kMaxResponseChars = 16384;
constexpr std::size_t kMaxPendingPrompts = 8;
constexpr std::size_t kMaxTranscriptChars = 65536;

enum class AgentState { OFFLINE, READY, ARMED, QUEUED, WORKING, REPLY, ERROR };
const char* agentStateName(AgentState state);

struct Prompt { uint64_t id = 0; std::string text; };
struct Event { uint64_t id = 0; std::string type; uint64_t promptId = 0; };
struct Snapshot {
	AgentState state = AgentState::OFFLINE;
	uint64_t latestPromptId = 0;
	uint64_t latestResponsePromptId = 0;
	std::size_t pendingCount = 0;
	std::size_t queuedCount = 0;
	std::size_t claimedCount = 0;
	std::size_t liveWorkerCount = 0;
	bool backgroundWorkerEnabled = false;
	std::string response;
	std::string error;
	std::string transcript;
};

class Mailbox {
public:
	using Milliseconds = std::chrono::milliseconds;
	explicit Mailbox(Milliseconds workerLease = Milliseconds(30000),
		Milliseconds claimLease = Milliseconds(60000),
		Milliseconds legacyClaimLease = Milliseconds(600000));
	bool submitPrompt(std::string text, uint64_t* promptId = nullptr, std::string* error = nullptr);
	bool waitForPrompt(uint64_t afterId, int waitMs, Prompt* prompt);
	bool postResponse(uint64_t promptId, std::string text, bool isError, std::string* error = nullptr);
	void setBackgroundWorkerEnabled(bool enabled);
	bool backgroundWorkerEnabled() const;
	bool registerWorker(std::string name, std::string* workerId, std::string* error = nullptr);
	bool heartbeatWorker(const std::string& workerId, std::string* error = nullptr);
	bool unregisterWorker(const std::string& workerId, std::string* error = nullptr);
	bool claimPrompt(const std::string& workerId, uint64_t promptId, Prompt* prompt,
		std::string* claimToken, std::string* error = nullptr);
	bool claimNextPrompt(const std::string& workerId, Prompt* prompt,
		std::string* claimToken, std::string* error = nullptr);
	bool renewClaim(uint64_t promptId, const std::string& claimToken, std::string* error = nullptr);
	bool releaseClaim(uint64_t promptId, const std::string& claimToken, std::string* error = nullptr);
	bool completeClaim(uint64_t promptId, const std::string& claimToken, std::string text,
		bool isError, const std::string& operationId, std::string* error = nullptr);
	bool waitForEvent(uint64_t afterId, int waitMs, Event* event);
	uint64_t latestEventId() const;
	void setAgentState(AgentState state);
	void setError(std::string error);
	Snapshot snapshot() const;

private:
	using Clock = std::chrono::steady_clock;
	enum class ClaimKind { NONE, LEGACY, WORKER };
	struct Pending { Prompt prompt; ClaimKind kind = ClaimKind::NONE; std::string owner; std::string token; Clock::time_point expiry{}; };
	struct Worker { std::string name; Clock::time_point expiry; };
	struct Completion { uint64_t promptId; std::string text; bool isError; std::string operationId; };
	mutable std::mutex mutex_;
	std::condition_variable promptReady_;
	std::condition_variable eventReady_;
	std::deque<Pending> prompts_;
	std::deque<Completion> completions_;
	std::deque<Event> events_;
	std::unordered_map<std::string, Worker> workers_;
	uint64_t nextPromptId_ = 1, nextWorkerId_ = 1, nextClaimToken_ = 1, nextEventId_ = 1, latestResponsePromptId_ = 0;
	bool backgroundWorkerEnabled_ = false;
	AgentState terminalState_ = AgentState::READY;
	std::string response_, error_, transcript_;
	Milliseconds workerLease_, claimLease_, legacyClaimLease_;
	void appendTranscript(const char* speaker, const std::string& text);
	void emitEventLocked(std::string type, uint64_t promptId = 0);
	void expireLocked(Clock::time_point now);
	bool completeLocked(Pending& pending, std::string text, bool isError,
		const std::string& operationId, std::string* error);
	AgentState derivedStateLocked() const;
};

void registerMailbox(int64_t moduleId, const std::shared_ptr<Mailbox>& mailbox);
void unregisterMailbox(int64_t moduleId, const std::shared_ptr<Mailbox>& mailbox);
std::shared_ptr<Mailbox> findMailbox(int64_t moduleId);
std::vector<int64_t> listMailboxIds();

} // namespace octavia_console
