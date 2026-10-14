#pragma once

#include "ThemeTypes.hpp"
#include <cctype>
#include <string>

namespace leviathan { namespace theme {

inline std::string trimColorText(const std::string& text) {
	std::size_t first = 0, last = text.size();
	while (first < last && std::isspace(static_cast<unsigned char>(text[first]))) ++first;
	while (last > first && std::isspace(static_cast<unsigned char>(text[last - 1]))) --last;
	return text.substr(first, last - first);
}

// Parse completely before updating the output; rejected clipboard data never
// partially changes a color. RGB channels are decimal integers in [0, 255].
inline bool parseClipboardColor(std::string text, ThemeColor* result) {
	if (!result) return false;
	text = trimColorText(text);
	unsigned channels[3] = {};
	if (text.find(',') != std::string::npos) {
		for (int i = 0; i < 3; ++i) {
			const auto comma = text.find(',');
			if ((i < 2) != (comma != std::string::npos)) return false;
			const auto part = trimColorText(text.substr(0, comma));
			if (part.empty()) return false;
			for (char c : part) {
				if (c < '0' || c > '9') return false;
				channels[i] = channels[i] * 10u + unsigned(c - '0');
				if (channels[i] > 255u) return false;
			}
			if (i < 2) text.erase(0, comma + 1);
		}
	}
	else {
		if (!text.empty() && text.front() == '#') text.erase(0, 1);
		if (text.size() != 3 && text.size() != 6) return false;
		const int digits = text.size() == 3 ? 1 : 2;
		for (int i = 0; i < 3; ++i) {
			for (int j = 0; j < digits; ++j) {
				const char c = text[i * digits + j];
				const int value = c >= '0' && c <= '9' ? c - '0'
					: c >= 'a' && c <= 'f' ? c - 'a' + 10
					: c >= 'A' && c <= 'F' ? c - 'A' + 10 : -1;
				if (value < 0) return false;
				channels[i] = channels[i] * 16u + unsigned(value);
			}
			if (digits == 1) channels[i] *= 17u;
		}
	}
	*result = ThemeColor(channels[0], channels[1], channels[2]);
	return true;
}

}} // namespace leviathan::theme
