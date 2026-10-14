#include "OctaviaConsoleMailbox.hpp"
#include <algorithm>

namespace octavia_console {
namespace {
std::mutex registryMutex;
std::unordered_map<int64_t, std::weak_ptr<Mailbox>> registry;
constexpr std::size_t kMaxCompletions = 32;
constexpr std::size_t kMaxEvents = 64;
}

const char* agentStateName(AgentState state) {
	switch (state) {
		case AgentState::OFFLINE: return "OFFLINE";
		case AgentState::READY: return "READY";
		case AgentState::ARMED: return "ARMED";
		case AgentState::QUEUED: return "QUEUED";
		case AgentState::WORKING: return "WORKING";
		case AgentState::REPLY: return "REPLY";
		case AgentState::ERROR: return "ERROR";
	}
	return "OFFLINE";
}

Mailbox::Mailbox(Milliseconds workerLease, Milliseconds claimLease, Milliseconds legacyClaimLease)
	: workerLease_(workerLease), claimLease_(claimLease), legacyClaimLease_(legacyClaimLease) {}

void Mailbox::appendTranscript(const char* speaker, const std::string& text) {
	if (!transcript_.empty()) transcript_ += "\n\n";
	transcript_ += speaker; transcript_ += "\n"; transcript_ += text;
	if (transcript_.size() > kMaxTranscriptChars) {
		auto keep = transcript_.size() - kMaxTranscriptChars;
		auto boundary = transcript_.find("\n\n", keep);
		transcript_.erase(0, boundary == std::string::npos ? keep : boundary + 2);
	}
}

void Mailbox::emitEventLocked(std::string type, uint64_t promptId) {
	Event event;
	event.id = nextEventId_++;
	event.type = std::move(type);
	event.promptId = promptId;
	events_.push_back(std::move(event));
	while (events_.size() > kMaxEvents) events_.pop_front();
	eventReady_.notify_all();
}

void Mailbox::expireLocked(Clock::time_point now) {
	for (auto it = workers_.begin(); it != workers_.end();)
		it = it->second.expiry <= now ? workers_.erase(it) : std::next(it);
	for (auto& p : prompts_) if (p.kind != ClaimKind::NONE && p.expiry <= now) {
		p.kind = ClaimKind::NONE; p.owner.clear(); p.token.clear();
		emitEventLocked("prompt.available", p.prompt.id);
	}
}

bool Mailbox::submitPrompt(std::string text, uint64_t* promptId, std::string* error) {
	if (text.empty() || text.size() > kMaxPromptChars) {
		if (error) *error = text.empty() ? "prompt must not be empty" : "prompt exceeds 4096 characters";
		return false;
	}
	std::lock_guard<std::mutex> lock(mutex_); expireLocked(Clock::now());
	if (prompts_.size() >= kMaxPendingPrompts) { if (error) *error = "prompt queue is full"; return false; }
	Pending p;
	p.prompt.id = nextPromptId_++;
	p.prompt.text = std::move(text);
	if (promptId) *promptId = p.prompt.id;
	prompts_.push_back(std::move(p)); appendTranscript("YOU", prompts_.back().prompt.text);
	terminalState_ = AgentState::READY; error_.clear();
	emitEventLocked("prompt.available", prompts_.back().prompt.id);
	promptReady_.notify_all(); return true;
}

bool Mailbox::waitForPrompt(uint64_t afterId, int waitMs, Prompt* prompt) {
	std::unique_lock<std::mutex> lock(mutex_);
	auto available = [&] { expireLocked(Clock::now()); return std::any_of(prompts_.begin(), prompts_.end(), [&](const Pending& p) { return p.prompt.id > afterId && p.kind == ClaimKind::NONE; }); };
	if (!available() && waitMs > 0) promptReady_.wait_for(lock, Milliseconds(std::min(waitMs, 25000)), available);
	auto it = std::find_if(prompts_.begin(), prompts_.end(), [&](const Pending& p) { return p.prompt.id > afterId && p.kind == ClaimKind::NONE; });
	if (it == prompts_.end()) return false;
	it->kind = ClaimKind::LEGACY; it->owner = "legacy"; it->token = "legacy-" + std::to_string(it->prompt.id); it->expiry = Clock::now() + legacyClaimLease_;
	if (prompt) *prompt = it->prompt;
	return true;
}

bool Mailbox::completeLocked(Pending& p, std::string text, bool isError, const std::string& operationId, std::string* error) {
	if (text.empty() || text.size() > kMaxResponseChars) { if (error) *error = text.empty() ? "response must not be empty" : "response exceeds 16384 characters"; return false; }
	Completion c{p.prompt.id, std::move(text), isError, operationId}; latestResponsePromptId_ = c.promptId;
	if (isError) { error_ = c.text; response_.clear(); terminalState_ = AgentState::ERROR; appendTranscript("OCTAVIA — ERROR", c.text); }
	else { response_ = c.text; error_.clear(); terminalState_ = AgentState::REPLY; appendTranscript("OCTAVIA", c.text); }
	completions_.push_back(std::move(c)); while (completions_.size() > kMaxCompletions) completions_.pop_front(); return true;
}

bool Mailbox::postResponse(uint64_t id, std::string text, bool isError, std::string* error) {
	std::lock_guard<std::mutex> lock(mutex_); expireLocked(Clock::now());
	auto done = std::find_if(completions_.begin(), completions_.end(), [&](const Completion& c) { return c.promptId == id; });
	if (done != completions_.end()) { if (done->text == text && done->isError == isError) return true; if (error) *error = "prompt already completed with a different response"; return false; }
	auto it = std::find_if(prompts_.begin(), prompts_.end(), [&](const Pending& p) { return p.prompt.id == id; });
	if (it == prompts_.end()) { if (error) *error = "promptId is not pending"; return false; }
	if (it->kind == ClaimKind::WORKER) { if (error) *error = "prompt is claimed by a background worker"; return false; }
	if (!completeLocked(*it, std::move(text), isError, "legacy", error)) return false;
	prompts_.erase(it); return true;
}

void Mailbox::setBackgroundWorkerEnabled(bool enabled) {
	std::lock_guard<std::mutex> lock(mutex_);
	if (backgroundWorkerEnabled_ == enabled) return;
	backgroundWorkerEnabled_ = enabled;
	if (!enabled) { workers_.clear(); for (auto& p : prompts_) if (p.kind == ClaimKind::WORKER) { p.kind = ClaimKind::NONE; p.owner.clear(); p.token.clear(); emitEventLocked("prompt.available", p.prompt.id); } emitEventLocked("worker.revoked"); promptReady_.notify_all(); }
}
bool Mailbox::backgroundWorkerEnabled() const { std::lock_guard<std::mutex> lock(mutex_); return backgroundWorkerEnabled_; }

bool Mailbox::registerWorker(std::string name, std::string* workerId, std::string* error) {
	std::lock_guard<std::mutex> lock(mutex_); expireLocked(Clock::now());
	if (!backgroundWorkerEnabled_) { if (error) *error = "background worker is not enabled"; return false; }
	std::string id = "worker-" + std::to_string(nextWorkerId_++); workers_[id] = {name.empty() ? "worker" : std::move(name), Clock::now() + workerLease_}; if (workerId) *workerId = id; return true;
}
bool Mailbox::heartbeatWorker(const std::string& id, std::string* error) {
	std::lock_guard<std::mutex> lock(mutex_); expireLocked(Clock::now()); auto it = workers_.find(id);
	if (it == workers_.end()) { if (error) *error = "worker lease is missing or expired"; return false; } it->second.expiry = Clock::now() + workerLease_; return true;
}
bool Mailbox::unregisterWorker(const std::string& id, std::string* error) {
	std::lock_guard<std::mutex> lock(mutex_); if (!workers_.erase(id)) { if (error) *error = "worker lease is missing or expired"; return false; }
	for (auto& p : prompts_) {
		if (p.kind == ClaimKind::WORKER && p.owner == id) {
			p.kind = ClaimKind::NONE; p.owner.clear(); p.token.clear(); emitEventLocked("prompt.available", p.prompt.id);
		}
	}
	promptReady_.notify_all();
	return true;
}

bool Mailbox::claimPrompt(const std::string& workerId, uint64_t id, Prompt* prompt, std::string* token, std::string* error) {
	std::lock_guard<std::mutex> lock(mutex_); auto now = Clock::now(); expireLocked(now);
	if (!backgroundWorkerEnabled_) { if (error) *error = "background worker is not enabled"; return false; }
	if (workers_.find(workerId) == workers_.end()) { if (error) *error = "worker lease is missing or expired"; return false; }
	auto it = std::find_if(prompts_.begin(), prompts_.end(), [&](const Pending& p) { return p.prompt.id == id; });
	if (it == prompts_.end()) { if (error) *error = "promptId is not pending"; return false; }
	if (it->kind != ClaimKind::NONE) { if (error) *error = "prompt is already claimed"; return false; }
	it->kind = ClaimKind::WORKER; it->owner = workerId; it->token = "claim-" + std::to_string(nextClaimToken_++); it->expiry = now + claimLease_;
	if (prompt) *prompt = it->prompt;
	if (token) *token = it->token;
	return true;
}
bool Mailbox::claimNextPrompt(const std::string& workerId, Prompt* prompt, std::string* token, std::string* error) {
	std::lock_guard<std::mutex> lock(mutex_); auto now = Clock::now(); expireLocked(now);
	if (!backgroundWorkerEnabled_) { if (error) *error = "background worker is not enabled"; return false; }
	if (workers_.find(workerId) == workers_.end()) { if (error) *error = "worker lease is missing or expired"; return false; }
	auto it = std::find_if(prompts_.begin(), prompts_.end(), [](const Pending& p) { return p.kind == ClaimKind::NONE; });
	if (it == prompts_.end()) { if (error) *error = "no queued prompt"; return false; }
	it->kind = ClaimKind::WORKER; it->owner = workerId; it->token = "claim-" + std::to_string(nextClaimToken_++); it->expiry = now + claimLease_;
	if (prompt) *prompt = it->prompt;
	if (token) *token = it->token;
	return true;
}
bool Mailbox::renewClaim(uint64_t id, const std::string& token, std::string* error) {
	std::lock_guard<std::mutex> lock(mutex_); expireLocked(Clock::now()); auto it = std::find_if(prompts_.begin(), prompts_.end(), [&](const Pending& p) { return p.prompt.id == id; });
	if (it == prompts_.end() || it->kind != ClaimKind::WORKER || it->token != token) { if (error) *error = "claim is missing, expired, or owned by another attempt"; return false; } it->expiry = Clock::now() + claimLease_; return true;
}
bool Mailbox::releaseClaim(uint64_t id, const std::string& token, std::string* error) {
	std::lock_guard<std::mutex> lock(mutex_); expireLocked(Clock::now()); auto it = std::find_if(prompts_.begin(), prompts_.end(), [&](const Pending& p) { return p.prompt.id == id; });
	if (it == prompts_.end() || it->kind != ClaimKind::WORKER || it->token != token) { if (error) *error = "claim is missing, expired, or owned by another attempt"; return false; } it->kind = ClaimKind::NONE; it->owner.clear(); it->token.clear(); emitEventLocked("prompt.available", id); promptReady_.notify_all(); return true;
}
bool Mailbox::completeClaim(uint64_t id, const std::string& token, std::string text, bool isError, const std::string& operationId, std::string* error) {
	std::lock_guard<std::mutex> lock(mutex_); expireLocked(Clock::now()); auto done = std::find_if(completions_.begin(), completions_.end(), [&](const Completion& c) { return c.promptId == id; });
	if (done != completions_.end()) { if (!operationId.empty() && done->operationId == operationId && done->text == text && done->isError == isError) return true; if (error) *error = "prompt already completed by another operation"; return false; }
	auto it = std::find_if(prompts_.begin(), prompts_.end(), [&](const Pending& p) { return p.prompt.id == id; });
	if (it == prompts_.end() || it->kind != ClaimKind::WORKER || it->token != token) { if (error) *error = "claim is missing, expired, or owned by another attempt"; return false; }
	if (!completeLocked(*it, std::move(text), isError, operationId, error)) return false;
	prompts_.erase(it);
	return true;
}

bool Mailbox::waitForEvent(uint64_t afterId, int waitMs, Event* event) {
	std::unique_lock<std::mutex> lock(mutex_);
	auto available = [&] { return !events_.empty() && events_.back().id > afterId; };
	if (!available() && waitMs > 0) eventReady_.wait_for(lock, Milliseconds(waitMs), available);
	if (!available()) return false;
	if (afterId + 1 < events_.front().id) {
		if (event) {
			event->id = events_.back().id;
			event->type = "resync";
			event->promptId = 0;
		}
		return true;
	}
	auto it = std::find_if(events_.begin(), events_.end(), [&](const Event& candidate) { return candidate.id > afterId; });
	if (it == events_.end()) return false;
	if (event) *event = *it;
	return true;
}

uint64_t Mailbox::latestEventId() const {
	std::lock_guard<std::mutex> lock(mutex_);
	return nextEventId_ - 1;
}

AgentState Mailbox::derivedStateLocked() const {
	if (terminalState_ == AgentState::ERROR) return AgentState::ERROR;
	if (std::any_of(prompts_.begin(), prompts_.end(), [](const Pending& p) { return p.kind != ClaimKind::NONE; })) return AgentState::WORKING;
	if (!prompts_.empty()) return AgentState::QUEUED;
	if (terminalState_ == AgentState::REPLY) return AgentState::REPLY;
	if (!workers_.empty()) return AgentState::ARMED;
	return AgentState::READY;
}
void Mailbox::setAgentState(AgentState state) { std::lock_guard<std::mutex> lock(mutex_); terminalState_ = state; }
void Mailbox::setError(const std::string& error) { std::lock_guard<std::mutex> lock(mutex_); error_ = error; response_.clear(); terminalState_ = AgentState::ERROR; appendTranscript("CONSOLE — ERROR", error); }
Snapshot Mailbox::snapshot() const {
	std::lock_guard<std::mutex> lock(mutex_); const_cast<Mailbox*>(this)->expireLocked(Clock::now()); Snapshot s;
	s.state = derivedStateLocked(); s.latestPromptId = nextPromptId_ - 1; s.latestResponsePromptId = latestResponsePromptId_; s.pendingCount = prompts_.size();
	s.queuedCount = std::count_if(prompts_.begin(), prompts_.end(), [](const Pending& p) { return p.kind == ClaimKind::NONE; }); s.claimedCount = s.pendingCount - s.queuedCount;
	s.liveWorkerCount = workers_.size(); s.backgroundWorkerEnabled = backgroundWorkerEnabled_; s.response = response_; s.error = error_; s.transcript = transcript_; return s;
}

void registerMailbox(int64_t id, const std::shared_ptr<Mailbox>& mailbox) { if (id < 0 || !mailbox) return; std::lock_guard<std::mutex> lock(registryMutex); registry[id] = mailbox; }
void unregisterMailbox(int64_t id, const std::shared_ptr<Mailbox>& mailbox) { if (id < 0) return; std::lock_guard<std::mutex> lock(registryMutex); auto it = registry.find(id); if (it != registry.end() && (!mailbox || it->second.lock() == mailbox)) registry.erase(it); }
std::shared_ptr<Mailbox> findMailbox(int64_t id) { std::lock_guard<std::mutex> lock(registryMutex); auto it = registry.find(id); if (it == registry.end()) return {}; auto mailbox = it->second.lock(); if (!mailbox) registry.erase(it); return mailbox; }
std::vector<int64_t> listMailboxIds() {
	std::lock_guard<std::mutex> lock(registryMutex);
	std::vector<int64_t> ids;
	for (auto it = registry.begin(); it != registry.end();) {
		if (it->second.expired()) it = registry.erase(it);
		else { ids.push_back(it->first); ++it; }
	}
	std::sort(ids.begin(), ids.end());
	return ids;
}

} // namespace octavia_console
