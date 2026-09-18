#pragma once
#include "SibylEdit.hpp"
#include <utility>
namespace sibyl {
// Control-side JSON helpers shared by edit, preview, and targeted inspection.
bool isNoteOperation(const std::string& name);
bool applyNoteOperation(json_t* working, json_t* operation, size_t index, EditResult& result);
bool selectNotes(json_t* pattern, json_t* selector, std::vector<json_t*>& selected,
                 EditResult& result, const std::string& path);
std::string serializeNotesView(const Composition& composition, json_t* request);
json_t* editChangesJson(const EditResult& result);
}
