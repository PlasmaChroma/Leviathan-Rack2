#include "OctaviaConsoleMailbox.hpp"

#include <chrono>
#include <iostream>
#include <thread>

namespace {
int failures = 0;
void check(bool condition, const char* name) {
	std::cout << (condition ? "[PASS] " : "[FAIL] ") << name << "\n";
	if (!condition) ++failures;
}
}

int main() {
	auto mailbox = std::make_shared<octavia_console::Mailbox>();
	uint64_t promptId = 0;
	std::string error;
	check(mailbox->submitPrompt("Make the bass darker", &promptId, &error) && promptId == 1,
		"panel prompt receives a stable generation");
	octavia_console::Prompt prompt;
	check(mailbox->waitForPrompt(0, 0, &prompt) && prompt.id == promptId &&
		prompt.text == "Make the bass darker", "agent reads the queued prompt");
	check(!mailbox->waitForPrompt(promptId, 0, nullptr), "after ID prevents duplicate delivery");
	check(mailbox->postResponse(promptId, "Reduced the filter cutoff.", false, &error),
		"agent response completes the pending prompt");
	auto snapshot = mailbox->snapshot();
	check(snapshot.state == octavia_console::AgentState::REPLY &&
		snapshot.response == "Reduced the filter cutoff." && snapshot.pendingCount == 0,
		"Rack snapshot publishes the reply and clears the queue");
	check(snapshot.transcript == "YOU\nMake the bass darker\n\nOCTAVIA\nReduced the filter cutoff.",
		"conversation transcript appends both sides in order");
	check(!mailbox->postResponse(promptId, "duplicate", false, &error),
		"duplicate response is rejected");
	check(mailbox->postResponse(promptId, "Reduced the filter cutoff.", false, &error),
		"identical legacy response retry is idempotent");

	auto workers = std::make_shared<octavia_console::Mailbox>(
		std::chrono::milliseconds(100), std::chrono::milliseconds(40),
		std::chrono::milliseconds(100));
	std::string workerId;
	check(!workers->registerWorker("test", &workerId, &error),
		"background registration is rejected by default");
	workers->setBackgroundWorkerEnabled(true);
	check(workers->registerWorker("test", &workerId, &error) && !workerId.empty(),
		"explicit opt-in permits worker registration");
	const uint64_t beforeEvent = workers->latestEventId();
	uint64_t workerPromptId = 0;
	workers->submitPrompt("worker prompt", &workerPromptId);
	octavia_console::Event event;
	check(workers->waitForEvent(beforeEvent, 0, &event) && event.type == "prompt.available" &&
		event.promptId == workerPromptId, "prompt submission emits a replayable wake event");
	std::string claimToken;
	check(workers->claimPrompt(workerId, workerPromptId, &prompt, &claimToken, &error) &&
		prompt.text == "worker prompt" && !claimToken.empty(), "worker atomically claims prompt");
	check(!workers->waitForPrompt(0, 0, nullptr), "legacy wait skips worker claim");
	check(workers->completeClaim(workerPromptId, claimToken, "done", false, "operation-1", &error),
		"worker claim completes prompt");
	check(workers->completeClaim(workerPromptId, claimToken, "done", false, "operation-1", &error),
		"worker completion retry is idempotent");
	uint64_t nextId = 0;
	workers->submitPrompt("claim next", &nextId);
	check(workers->claimNextPrompt(workerId, &prompt, &claimToken, &error) && prompt.id == nextId,
		"worker can resynchronize by claiming the next queued prompt");
	check(workers->completeClaim(nextId, claimToken, "next done", false, "operation-2", &error),
		"resynchronized claim completes normally");

	uint64_t expiringId = 0;
	workers->submitPrompt("retry me", &expiringId);
	check(workers->claimPrompt(workerId, expiringId, &prompt, &claimToken, &error),
		"worker claims prompt before expiry");
	std::this_thread::sleep_for(std::chrono::milliseconds(60));
	check(workers->waitForPrompt(0, 0, &prompt) && prompt.id == expiringId,
		"expired worker claim returns to legacy queue");
	workers->setBackgroundWorkerEnabled(false);
	check(!workers->heartbeatWorker(workerId, &error), "disabling background mode revokes worker");

	auto waited = std::make_shared<octavia_console::Mailbox>();
	bool received = false;
	std::thread waiter([&] {
		octavia_console::Prompt next;
		received = waited->waitForPrompt(0, 1000, &next) && next.text == "wake";
	});
	std::this_thread::sleep_for(std::chrono::milliseconds(20));
	waited->submitPrompt("wake");
	waiter.join();
	check(received, "long poll wakes when Rack submits a prompt");

	octavia_console::registerMailbox(42, mailbox);
	check(octavia_console::findMailbox(42) == mailbox, "module registry resolves a live Console");
	check(octavia_console::listMailboxIds() == std::vector<int64_t>{42},
		"module registry lists the sole live Console for discovery");
	octavia_console::unregisterMailbox(42, mailbox);
	check(!octavia_console::findMailbox(42), "module registry removes a detached Console");

	std::cout << "[SUMMARY] octavia_console_mailbox_spec: "
		<< (failures ? "FAILED" : "passed") << "\n";
	return failures ? 1 : 0;
}
