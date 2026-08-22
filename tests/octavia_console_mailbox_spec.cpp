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
	octavia_console::unregisterMailbox(42, mailbox);
	check(!octavia_console::findMailbox(42), "module registry removes a detached Console");

	std::cout << "[SUMMARY] octavia_console_mailbox_spec: "
		<< (failures ? "FAILED" : "passed") << "\n";
	return failures ? 1 : 0;
}
