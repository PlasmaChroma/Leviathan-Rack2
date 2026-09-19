#include "SibylEdit.hpp"
#include <iostream>
#include <string>

// Each input line is emitted by the real Python toolkit, not a copied fixture.
int main() {
    std::string line;
    size_t cases = 0;
    while (std::getline(std::cin, line)) {
        json_error_t error;
        json_t* fixture = json_loads(line.c_str(), 0, &error);
        if (!fixture) return 1;
        char* baseJson = json_dumps(json_object_get(fixture, "base"), JSON_COMPACT);
        auto base = sibyl::parseCompositionJson(baseJson ? baseJson : "{}", 1);
        free(baseJson);
        if (!base.valid) { json_decref(fixture); return 2; }
        auto edit = sibyl::applyCompositionEdit(*base.composition, json_object_get(fixture,"operations"), 2);
        if (!edit.valid) {
            std::cerr << "Composer fixture " << cases << ": " << edit.errorPath << ": " << edit.errorMessage << "\n";
            json_decref(fixture);
            return 3;
        }
        // Canonical and compact authoring must produce exactly the same score.
        auto exact = sibyl::applyCompositionEdit(*base.composition, json_object_get(fixture,"canonical"), 2);
        if (!exact.valid || sibyl::serializeFullCompositionJson(*edit.composition)
                != sibyl::serializeFullCompositionJson(*exact.composition)) {
            std::cerr << "Composer compact/canonical mismatch in fixture " << cases << "\n";
            json_decref(fixture);
            return 4;
        }
        json_decref(fixture);
        ++cases;
    }
    std::cout << "PASS: " << cases << " Python composer/C++ validator equivalence fixtures\n";
    return cases >= 7 ? 0 : 5;
}
