#pragma once
#include <jansson.h>
#include <string>

namespace sibyl {
// Control-side projection only. Errors and warnings are never truncated.
inline void projectEditResponse(json_t* response, const std::string& profile) {
    if (profile == "full" || !json_is_true(json_object_get(response, "ok"))) return;
    json_t* changes = json_object_get(response, "changes");
    json_t* totals = json_object();
    json_t* summaries = json_array();
    size_t i; json_t* change;
    json_array_foreach(changes, i, change) {
        json_t* summary = json_object();
        for (const char* key : {"operationIndex", "patternId"})
            if (json_t* value = json_object_get(change, key)) json_object_set(summary, key, value);
        for (const char* key : {"matched", "inserted", "updated", "deleted", "clamped"}) {
            json_t* value = json_object_get(change, key);
            if (json_is_integer(value)) {
                json_object_set(summary, key, value);
                json_int_t count = json_integer_value(value);
                if (count) json_object_set_new(totals, key, json_integer(
                    json_integer_value(json_object_get(totals, key)) + count));
            }
        }
        json_array_append_new(summaries, summary);
    }
    json_object_del(response, "changes");
    if (json_object_size(totals)) json_object_set_new(response, "changes", totals);
    else json_decref(totals);
    if (profile == "summary") json_object_set_new(response, "operationSummaries", summaries);
    else json_decref(summaries);
}
} // namespace sibyl
