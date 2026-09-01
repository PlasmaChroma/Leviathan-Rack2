#include "Phonex.hpp"

#include <array>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

namespace {

std::string dumpJson(json_t* root) {
	char* text = json_dumps(root, JSON_COMPACT);
	std::string result = text ? text : "{}";
	if (text)
		free(text);
	return result;
}

json_t* stringArray(std::initializer_list<const char*> values) {
	json_t* result = json_array();
	for (const char* value : values)
		json_array_append_new(result, json_string(value));
	return result;
}

json_t* semanticOperationSchema() {
	json_t* alternatives = json_array();
	auto addAlternative = [&](const char* op, bool needsSlot, bool needsText,
			const char* description) {
		json_t* schema = json_object();
		json_object_set_new(schema, "type", json_string("object"));
		json_object_set_new(schema, "description", json_string(description));
		json_object_set_new(schema, "additionalProperties", json_false());
		json_t* properties = json_object();
		json_t* opSchema = json_object();
		json_object_set_new(opSchema, "const", json_string(op));
		json_object_set_new(properties, "op", opSchema);
		if (needsSlot) {
			json_t* slot = json_object();
			json_object_set_new(slot, "type", json_string("integer"));
			json_object_set_new(slot, "minimum", json_integer(0));
			json_object_set_new(slot, "maximum", json_integer(Phonex::kUserBankSize - 1));
			json_object_set_new(slot, "description", json_string("Zero-based user-bank slot"));
			json_object_set_new(properties, "slot", slot);
		}
		if (needsText) {
			json_t* text = json_object();
			json_object_set_new(text, "type", json_string("string"));
			json_object_set_new(text, "x-maxUtf8Bytes", json_integer(phonex::kMaxSubmittedTextBytes));
			json_object_set_new(text, "description", json_string(
				"Text or bracketed Phonex phonemes; empty text clears the slot"));
			json_object_set_new(properties, "text", text);
		}
		json_object_set_new(schema, "properties", properties);
		if (needsSlot && needsText)
			json_object_set_new(schema, "required", stringArray({"op", "slot", "text"}));
		else if (needsSlot)
			json_object_set_new(schema, "required", stringArray({"op", "slot"}));
		else
			json_object_set_new(schema, "required", stringArray({"op"}));
		json_array_append_new(alternatives, schema);
	};
	addAlternative("set_slot", true, true,
		"Compile and replace one user-bank slot");
	addAlternative("clear_slot", true, false,
		"Clear one user-bank slot");
	addAlternative("clear_bank", false, false,
		"Clear all user-bank slots before later operations in the same transaction");
	json_t* result = json_object();
	json_object_set_new(result, "oneOf", alternatives);
	return result;
}

json_t* semanticTransactionSchema(bool requireRevision) {
	json_t* schema = json_object();
	json_object_set_new(schema, "$schema", json_string("https://json-schema.org/draft/2020-12/schema"));
	json_object_set_new(schema, "type", json_string("object"));
	json_object_set_new(schema, "additionalProperties", json_false());
	json_object_set_new(schema, "description", json_string(requireRevision
		? "Ordered atomic word-bank edit guarded by the last observed revision"
		: "Candidate word-bank operations compiled without changing module state"));
	json_t* properties = json_object();
	json_t* revision = json_object();
	json_object_set_new(revision, "type", json_string("integer"));
	json_object_set_new(revision, "minimum", json_integer(0));
	json_object_set_new(revision, "description", json_string(
		"Revision returned by the most recent status or document read"));
	json_object_set_new(properties, "expectedRevision", revision);
	json_t* operations = json_object();
	json_object_set_new(operations, "type", json_string("array"));
	json_object_set_new(operations, "minItems", json_integer(1));
	json_object_set_new(operations, "description", json_string(
		"Operations are applied in order and commit only if every resulting entry compiles"));
	json_object_set_new(operations, "items", semanticOperationSchema());
	json_object_set_new(properties, "operations", operations);
	json_object_set_new(schema, "properties", properties);
	json_object_set_new(schema, "required", requireRevision
		? stringArray({"expectedRevision", "operations"})
		: stringArray({"operations"}));
	json_t* examples = json_array();
	json_t* example = json_object();
	if (requireRevision)
		json_object_set_new(example, "expectedRevision", json_integer(3));
	json_t* exampleOperations = json_array();
	json_array_append_new(exampleOperations, json_pack("{s:s}", "op", "clear_bank"));
	json_array_append_new(exampleOperations, json_pack("{s:s,s:i,s:s}",
		"op", "set_slot", "slot", 0, "text", "test phrase"));
	json_object_set_new(example, "operations", exampleOperations);
	json_array_append_new(examples, example);
	json_object_set_new(schema, "examples", examples);
	return schema;
}

json_t* semanticDocumentRequestSchema() {
	json_t* schema = json_object();
	json_object_set_new(schema, "$schema", json_string("https://json-schema.org/draft/2020-12/schema"));
	json_object_set_new(schema, "type", json_string("object"));
	json_object_set_new(schema, "additionalProperties", json_false());
	json_t* properties = json_object();
	json_t* view = json_object();
	json_object_set_new(view, "type", json_string("string"));
	json_object_set_new(view, "enum", stringArray({"summary", "full", "slot"}));
	json_object_set_new(view, "default", json_string("summary"));
	json_object_set_new(properties, "view", view);
	json_t* id = json_object();
	json_object_set_new(id, "type", json_string("string"));
	json_object_set_new(id, "pattern", json_string("^(?:[0-9]|[1-5][0-9]|6[0-3])$"));
	json_object_set_new(id, "description", json_string(
		"Zero-based slot ID; required only when view is slot"));
	json_object_set_new(properties, "id", id);
	json_object_set_new(schema, "properties", properties);
	json_t* examples = json_array();
	json_array_append_new(examples, json_pack("{s:s}", "view", "summary"));
	json_array_append_new(examples, json_pack("{s:s,s:s}", "view", "slot", "id", "12"));
	json_object_set_new(schema, "examples", examples);
	return schema;
}

struct CompiledSlot {
	int slot = 0;
	std::unique_ptr<phonex::LpcSequence> sequence;
	bool unsupportedUnicode = false;
};

struct PreparedEdit {
	bool valid = false;
	std::string code;
	std::string path;
	std::string message;
	std::array<std::string, Phonex::kUserBankSize> texts;
	std::array<bool, Phonex::kUserBankSize> changed{};
	std::vector<CompiledSlot> compiled;
};

void reject(PreparedEdit& result, const char* code,
		const std::string& path, const char* message) {
	result.valid = false;
	result.code = code;
	result.path = path;
	result.message = message;
}

bool readSlot(json_t* operation, int operationIndex, PreparedEdit& result, int& slot) {
	json_t* slotJ = json_object_get(operation, "slot");
	if (!json_is_integer(slotJ)) {
		reject(result, "invalid_field", "/operations/" + std::to_string(operationIndex) + "/slot",
			"slot must be an integer");
		return false;
	}
	const json_int_t value = json_integer_value(slotJ);
	if (value < 0 || value >= Phonex::kUserBankSize) {
		reject(result, "out_of_range", "/operations/" + std::to_string(operationIndex) + "/slot",
			"slot must be between 0 and 63");
		return false;
	}
	slot = int(value);
	return true;
}

PreparedEdit prepareEdit(json_t* request,
		const std::array<std::string, Phonex::kUserBankSize>& current,
		std::uint32_t currentRevision, bool requireRevision) {
	PreparedEdit result;
	result.texts = current;
	if (!json_is_object(request)) {
		reject(result, "invalid_request", "", "request must be an object");
		return result;
	}
	json_t* revisionJ = json_object_get(request, "expectedRevision");
	if (requireRevision) {
		if (!json_is_integer(revisionJ) || json_integer_value(revisionJ) < 0) {
			reject(result, "invalid_field", "/expectedRevision",
				"expectedRevision must be a non-negative integer");
			return result;
		}
		if (static_cast<std::uint32_t>(json_integer_value(revisionJ)) != currentRevision) {
			reject(result, "revision_conflict", "/expectedRevision",
				"word bank changed since it was read");
			return result;
		}
	}
	json_t* operations = json_object_get(request, "operations");
	if (!json_is_array(operations) || json_array_size(operations) == 0) {
		reject(result, "invalid_field", "/operations",
			"operations must be a non-empty array");
		return result;
	}
	const std::size_t operationCount = json_array_size(operations);
	for (std::size_t index = 0; index < operationCount; ++index) {
		json_t* operation = json_array_get(operations, index);
		json_t* opJ = json_is_object(operation) ? json_object_get(operation, "op") : nullptr;
		if (!json_is_string(opJ)) {
			reject(result, "invalid_operation", "/operations/" + std::to_string(index),
				"operation requires a string op");
			return result;
		}
		const std::string op = json_string_value(opJ);
		if (op == "clear_bank") {
			for (int slot = 0; slot < Phonex::kUserBankSize; ++slot) {
				result.texts[slot].clear();
				result.changed[slot] = true;
			}
			continue;
		}
		int slot = 0;
		if ((op == "set_slot" || op == "clear_slot")
				&& !readSlot(operation, int(index), result, slot))
			return result;
		if (op == "clear_slot") {
			result.texts[slot].clear();
			result.changed[slot] = true;
			continue;
		}
		if (op == "set_slot") {
			json_t* textJ = json_object_get(operation, "text");
			if (!json_is_string(textJ)) {
				reject(result, "invalid_field", "/operations/" + std::to_string(index) + "/text",
					"text must be a string");
				return result;
			}
			const char* data = json_string_value(textJ);
			const std::size_t size = json_string_length(textJ);
			if (size && std::memchr(data, '\0', size)) {
				reject(result, "invalid_field", "/operations/" + std::to_string(index) + "/text",
					"text cannot contain a null character");
				return result;
			}
			result.texts[slot].assign(data, size);
			result.changed[slot] = true;
			continue;
		}
		reject(result, "unknown_operation", "/operations/" + std::to_string(index) + "/op",
			"supported operations are set_slot, clear_slot, and clear_bank");
		return result;
	}

	for (int slot = 0; slot < Phonex::kUserBankSize; ++slot) {
		if (!result.changed[slot] || result.texts[slot].empty())
			continue;
		CompiledSlot compiled;
		compiled.slot = slot;
		compiled.sequence.reset(new phonex::LpcSequence());
		const phonex::TextCompileResult compileResult = phonex::compileText(
			phonex::StringView(result.texts[slot]), *compiled.sequence);
		if (compileResult.status != phonex::CompileStatus::Ok) {
			reject(result, "compile_failed", "/slots/" + std::to_string(slot),
				phonex::compileStatusText(compileResult.status));
			return result;
		}
		compiled.unsupportedUnicode = compileResult.unsupportedUnicode;
		result.compiled.push_back(std::move(compiled));
	}
	result.valid = true;
	return result;
}

void appendWarnings(json_t* response, const PreparedEdit& edit) {
	json_t* warnings = json_array();
	for (const CompiledSlot& compiled : edit.compiled) {
		if (!compiled.unsupportedUnicode)
			continue;
		json_t* warning = json_object();
		json_object_set_new(warning, "code", json_string("unicode_boundary"));
		json_object_set_new(warning, "path", json_string(
			("/slots/" + std::to_string(compiled.slot)).c_str()));
		json_object_set_new(warning, "message", json_string(
			"unsupported Unicode was treated as a word boundary"));
		json_array_append_new(warnings, warning);
	}
	json_object_set_new(response, "warnings", warnings);
}

json_t* editErrorResponse(const PreparedEdit& edit, std::uint32_t revision) {
	json_t* response = json_object();
	json_object_set_new(response, "ok", json_false());
	json_t* issue = json_object();
	json_object_set_new(issue, "code", json_string(edit.code.c_str()));
	json_object_set_new(issue, "path", json_string(edit.path.c_str()));
	json_object_set_new(issue, "message", json_string(edit.message.c_str()));
	json_object_set_new(response, "error", issue);
	json_object_set_new(response, "currentRevision", json_integer(revision));
	return response;
}

int populatedCount(const Phonex& module) {
	int result = 0;
	for (int slot = 0; slot < Phonex::kUserBankSize; ++slot)
		result += module.userSlotPopulated(slot) ? 1 : 0;
	return result;
}

} // namespace

bool Phonex::handleSemanticRequest(OctaviaSemanticControl::Operation operation,
		const std::string& requestJson, std::string& responseJson, std::string& error) {
	const std::uint32_t revision = userBankRevision.load(std::memory_order_relaxed);
	if (operation == OctaviaSemanticControl::Operation::CAPABILITIES) {
		json_t* response = json_object();
		json_object_set_new(response, "ok", json_true());
		json_object_set_new(response, "capabilityId", json_string(semanticCapabilityId()));
		json_t* capability = json_object();
		json_object_set_new(capability, "apiVersion", json_integer(1));
		json_object_set_new(capability, "schemaVersion", json_integer(1));
		json_object_set_new(capability, "documentType", json_string("text-slot-bank"));
		json_object_set_new(capability, "revision", json_integer(revision));
		json_object_set_new(capability, "slotCount", json_integer(kUserBankSize));
		json_object_set_new(capability, "maxTextBytes", json_integer(phonex::kMaxSubmittedTextBytes));
		json_object_set_new(capability, "documentViews", stringArray({"summary", "full", "slot"}));
		json_object_set_new(capability, "operations", stringArray({"document", "validate", "edit", "status"}));
		json_object_set_new(capability, "editOperations", stringArray({"set_slot", "clear_slot", "clear_bank"}));
		json_t* requestSchemas = json_object();
		json_object_set_new(requestSchemas, "document", semanticDocumentRequestSchema());
		json_object_set_new(requestSchemas, "validate", semanticTransactionSchema(false));
		json_object_set_new(requestSchemas, "edit", semanticTransactionSchema(true));
		json_object_set_new(capability, "requestSchemas", requestSchemas);
		json_object_set_new(response, "capabilities", capability);
		responseJson = dumpJson(response);
		json_decref(response);
		return true;
	}

	if (operation == OctaviaSemanticControl::Operation::GET_DOCUMENT) {
		json_error_t parseError{};
		json_t* request = requestJson.empty() ? json_object()
			: json_loads(requestJson.c_str(), 0, &parseError);
		if (!request || !json_is_object(request)) {
			if (request)
				json_decref(request);
			error = "invalid document request";
			return false;
		}
		std::string view = "summary";
		std::string id;
		if (json_t* value = json_object_get(request, "view"))
			if (json_is_string(value)) view = json_string_value(value);
		if (json_t* value = json_object_get(request, "id"))
			if (json_is_string(value)) id = json_string_value(value);
		json_t* response = json_object();
		json_object_set_new(response, "ok", json_true());
		json_object_set_new(response, "revision", json_integer(revision));
		if (view == "summary") {
			json_object_set_new(response, "slotCount", json_integer(kUserBankSize));
			json_object_set_new(response, "populatedCount", json_integer(populatedCount(*this)));
			json_t* entries = json_array();
			for (int slot = 0; slot < kUserBankSize; ++slot) {
				if (userTexts[slot].empty())
					continue;
				json_t* entry = json_object();
				json_object_set_new(entry, "slot", json_integer(slot));
				json_object_set_new(entry, "text", json_stringn(
					userTexts[slot].data(), userTexts[slot].size()));
				json_array_append_new(entries, entry);
			}
			json_object_set_new(response, "entries", entries);
		}
		else if (view == "full") {
			json_t* slots = json_array();
			for (const std::string& text : userTexts)
				json_array_append_new(slots, json_stringn(text.data(), text.size()));
			json_object_set_new(response, "slots", slots);
		}
		else if (view == "slot") {
			char* end = nullptr;
			const long slot = std::strtol(id.c_str(), &end, 10);
			if (id.empty() || !end || *end != '\0' || slot < 0 || slot >= kUserBankSize) {
				json_decref(response);
				json_decref(request);
				error = "slot view requires id from 0 to 63";
				responseJson = "{\"ok\":false,\"error\":{\"code\":\"object_not_found\",\"message\":\"slot view requires id from 0 to 63\"}}";
				return false;
			}
			json_object_set_new(response, "slot", json_integer(slot));
			json_object_set_new(response, "text", json_stringn(
				userTexts[slot].data(), userTexts[slot].size()));
			json_object_set_new(response, "populated", json_boolean(!userTexts[slot].empty()));
		}
		else {
			json_decref(response);
			json_decref(request);
			error = "unsupported document view";
			responseJson = "{\"ok\":false,\"error\":{\"code\":\"unsupported_view\",\"message\":\"supported views are summary, full, and slot\"}}";
			return false;
		}
		json_decref(request);
		responseJson = dumpJson(response);
		json_decref(response);
		return true;
	}

	if (operation == OctaviaSemanticControl::Operation::GET_STATUS) {
		json_t* response = json_object();
		json_object_set_new(response, "ok", json_true());
		json_object_set_new(response, "capabilityId", json_string(semanticCapabilityId()));
		json_object_set_new(response, "revision", json_integer(revision));
		json_object_set_new(response, "slotCount", json_integer(kUserBankSize));
		json_object_set_new(response, "populatedCount", json_integer(populatedCount(*this)));
		json_object_set_new(response, "selectedSlot", json_integer(
			clamp(selectedWord.load(std::memory_order_relaxed), 0, kUserBankSize - 1)));
		json_object_set_new(response, "selectedBank", json_string(
			params[BANK_PARAM].getValue() >= 0.5f ? "user" : "stock"));
		responseJson = dumpJson(response);
		json_decref(response);
		return true;
	}

	if (operation == OctaviaSemanticControl::Operation::VALIDATE
			|| operation == OctaviaSemanticControl::Operation::EDIT) {
		json_error_t parseError{};
		json_t* request = json_loads(requestJson.c_str(), 0, &parseError);
		PreparedEdit prepared = prepareEdit(request, userTexts, revision,
			operation == OctaviaSemanticControl::Operation::EDIT);
		if (request)
			json_decref(request);
		if (operation == OctaviaSemanticControl::Operation::VALIDATE) {
			json_t* response = json_object();
			json_object_set_new(response, "ok", json_true());
			json_object_set_new(response, "valid", json_boolean(prepared.valid));
			json_object_set_new(response, "currentRevision", json_integer(revision));
			json_t* errors = json_array();
			if (!prepared.valid) {
				json_t* issue = json_object();
				json_object_set_new(issue, "code", json_string(prepared.code.c_str()));
				json_object_set_new(issue, "path", json_string(prepared.path.c_str()));
				json_object_set_new(issue, "message", json_string(prepared.message.c_str()));
				json_array_append_new(errors, issue);
			}
			json_object_set_new(response, "errors", errors);
			appendWarnings(response, prepared);
			responseJson = dumpJson(response);
			json_decref(response);
			return true;
		}
		if (!prepared.valid) {
			json_t* response = editErrorResponse(prepared, revision);
			responseJson = dumpJson(response);
			json_decref(response);
			error = prepared.message;
			return false;
		}

		for (int slot = 0; slot < kUserBankSize; ++slot) {
			if (!prepared.changed[slot])
				continue;
			userTexts[slot] = prepared.texts[slot];
			if (prepared.texts[slot].empty())
				userSlotAvailable[slot].store(false, std::memory_order_release);
		}
		for (const CompiledSlot& compiled : prepared.compiled)
			publishUserSequence(compiled.slot, *compiled.sequence);
		const int selected = clamp(selectedWord.load(std::memory_order_relaxed), 0, kUserBankSize - 1);
		if (prepared.changed[selected]) {
			submittedText = prepared.texts[selected];
			textStatus.store(prepared.texts[selected].empty()
				? phonex::CompileStatus::Empty : phonex::CompileStatus::Ok,
				std::memory_order_relaxed);
			bool unicodeWarning = false;
			for (const CompiledSlot& compiled : prepared.compiled)
				if (compiled.slot == selected) unicodeWarning = compiled.unsupportedUnicode;
			unsupportedUnicode.store(unicodeWarning, std::memory_order_relaxed);
		}
		const std::uint32_t nextRevision = revision + 1u;
		userBankRevision.store(nextRevision, std::memory_order_relaxed);
		json_t* response = json_object();
		json_object_set_new(response, "ok", json_true());
		json_object_set_new(response, "revision", json_integer(nextRevision));
		json_object_set_new(response, "populatedCount", json_integer(populatedCount(*this)));
		appendWarnings(response, prepared);
		responseJson = dumpJson(response);
		json_decref(response);
		return true;
	}

	error = "unsupported semantic operation";
	responseJson = "{\"ok\":false,\"error\":{\"code\":\"unsupported_operation\",\"message\":\"operation is not supported by the Phonex word bank\"}}";
	return false;
}
