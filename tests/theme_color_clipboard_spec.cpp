#include "theme/ThemeColorClipboard.hpp"
#include <iostream>

int main() {
	using namespace leviathan::theme;
	int failures = 0;
	auto valid = [&](const char* text, ThemeColor expected) {
		ThemeColor actual;
		if (!parseClipboardColor(text, &actual) || actual != expected) {
			std::cerr << "Rejected or misparsed: " << text << '\n';
			++failures;
		}
	};
	valid(" \t#1aB2c3\r\n", ThemeColor(26, 178, 195));
	valid("1AB2C3", ThemeColor(26, 178, 195));
	valid("#a3F", ThemeColor(170, 51, 255));
	valid("a3f", ThemeColor(170, 51, 255));
	valid(" 255, 0, 128 \n", ThemeColor(255, 0, 128));
	valid("0,0,0", ThemeColor(0, 0, 0));
	valid("255,255,255", ThemeColor(255, 255, 255));
	for (const char* text : {"", " \n\t", "#", "##123456", "12345", "12345678",
		"#gg0000", "0x123456", "12 3456", "rgb(1,2,3)", "1,2", "1,2,3,4",
		"1,,3", ",2,3", "1,2,", "256,0,0", "-1,0,0", "+1,2,3", "1.5,2,3",
		"1,2,3junk", "999999999999999999999,2,3", "1,2,3,", "1,2,3\nabc"}) {
		ThemeColor actual(4, 5, 6);
		if (parseClipboardColor(text, &actual) || actual != ThemeColor(4, 5, 6)) {
			std::cerr << "Accepted invalid color or modified output: " << text << '\n';
			++failures;
		}
	}
	if (parseClipboardColor("#123456", nullptr)) ++failures;
	std::cout << "Theme clipboard color tests: " << (failures ? "FAIL" : "PASS") << '\n';
	return failures ? 1 : 0;
}
