#include "OctaviaConsoleMailbox.hpp"

#include <algorithm>
#include <chrono>
#include <unordered_map>

namespace octavia_console {
namespace {
std::mutex registryMutex;
std::unordered_map<int64_t, std::weak_ptr<Mailbox>> registry;
}

const char* agentStateName(AgentState state) {
	switch (state) {
		case AgentState::OFFLINE: return "OFFLINE";
		case AgentState::READY: return "READY";
		case AgentState::ARMED: return "ARMED";
		case AgentState::WORKING: return "WORKING";
		case AgentState::REPLY: return "REPLY";
		case AgentState::ERROR: return "ERROR";
	}
	return "OFFLINE";
}

bool Mailbox::submitPrompt(std::string text, uint64_t* promptId, std::string* error) {
	if (text.empty()) {
		if (error) *error = "prompt must not be empty";
		return false;
	}
	if (text.size() > kMaxPromptChars) {
		if (error) *error = "prompt exceeds 4096 characters";
		return false;
	}
	std::lock_guard<std::mutex> lock(mutex_);
	if (prompts_.size() >= kMaxPendingPrompts) {
		if (error) *error = "prompt queue is full";
		return false;
	}
	Prompt prompt;
	prompt.id = nextPromptId_++;
	prompt.text = std::move(text);
	if (promptId) *promptId = prompt.id;
	prompts_.push_back(std::move(prompt));
	state_ = AgentState::WORKING;
	error_.clear();
	promptReady_.notify_all();
	return true;
}

bool Mailbox::waitForPrompt(uint64_t afterId, int waitMs, Prompt* prompt) {
	std::unique_lock<std::mutex> lock(mutex_);
	auto available = [&] {
		return std::any_of(prompts_.begin(), prompts_.end(),
			[&](const Prompt& candidate) { return candidate.id > afterId; });
	};
	state_ = AgentState::ARMED;
	if (!available() && waitMs > 0)
		promptReady_.wait_for(lock, std::chrono::milliseconds(std::min(waitMs, 25000)), available);
	auto found = std::find_if(prompts_.begin(), prompts_.end(),
		[&](const Prompt& candidate) { return candidate.id > afterId; });
	if (found == prompts_.end()) return false;
	if (prompt) *prompt = *found;
	state_ = AgentState::WORKING;
	return true;
}

bool Mailbox::postResponse(uint64_t promptId, std::string text, bool isError, std::string* error) {
	if (text.empty()) {
		if (error) *error = "response must not be empty";
		return false;
	}
	if (text.size() > kMaxResponseChars) {
		if (error) *error = "response exceeds 16384 characters";
		return false;
	}
	std::lock_guard<std::mutex> lock(mutex_);
	auto found = std::find_if(prompts_.begin(), prompts_.end(),
		[&](const Prompt& prompt) { return prompt.id == promptId; });
	if (found == prompts_.end()) {
		if (error) *error = "promptId is not pending";
		return false;
	}
	prompts_.erase(prompts_.begin(), std::next(found));
	latestResponsePromptId_ = promptId;
	if (isError) {
		error_ = std::move(text);
		response_.clear();
		state_ = AgentState::ERROR;
	} else {
		response_ = std::move(text);
		error_.clear();
		state_ = AgentState::REPLY;
	}
	return true;
}

void Mailbox::setAgentState(AgentState state) {
	std::lock_guard<std::mutex> lock(mutex_);
	state_ = state;
}

void Mailbox::setError(std::string error) {
	std::lock_guard<std::mutex> lock(mutex_);
	error_ = std::move(error);
	response_.clear();
	state_ = AgentState::ERROR;
}

Snapshot Mailbox::snapshot() const {
	std::lock_guard<std::mutex> lock(mutex_);
	Snapshot result;
	result.state = state_;
	result.latestPromptId = nextPromptId_ - 1;
	result.latestResponsePromptId = latestResponsePromptId_;
	result.pendingCount = prompts_.size();
	result.response = response_;
	result.error = error_;
	return result;
}

void registerMailbox(int64_t moduleId, const std::shared_ptr<Mailbox>& mailbox) {
	if (moduleId < 0 || !mailbox) return;
	std::lock_guard<std::mutex> lock(registryMutex);
	registry[moduleId] = mailbox;
}

void unregisterMailbox(int64_t moduleId, const std::shared_ptr<Mailbox>& mailbox) {
	if (moduleId < 0) return;
	std::lock_guard<std::mutex> lock(registryMutex);
	auto found = registry.find(moduleId);
	if (found == registry.end()) return;
	if (!mailbox || found->second.lock() == mailbox) registry.erase(found);
}

std::shared_ptr<Mailbox> findMailbox(int64_t moduleId) {
	std::lock_guard<std::mutex> lock(registryMutex);
	auto found = registry.find(moduleId);
	if (found == registry.end()) return {};
	auto mailbox = found->second.lock();
	if (!mailbox) registry.erase(found);
	return mailbox;
}

} // namespace octavia_console
