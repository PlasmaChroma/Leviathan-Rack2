#include "OctaviaSemanticControl.hpp"
#include "SibylControl.hpp"

#include <iostream>
#include <string>

namespace {

struct MockSibyl final : SibylControl {
    Operation lastOperation = Operation::DEBUG_CAPTURE;
    std::string lastRequest;

    bool handleSibylRequest(Operation operation, const std::string& requestJson,
                            std::string& responseJson, std::string&) override {
        lastOperation = operation;
        lastRequest = requestJson;
        responseJson = operation == Operation::CAPABILITIES
            ? "{\"ok\":true}" : "{\"ok\":true,\"forwarded\":true}";
        return true;
    }
};

bool checkMapping(MockSibyl& mock, OctaviaSemanticControl::Operation operation,
                  SibylControl::Operation expected) {
    std::string response;
    std::string error;
    const bool handled = mock.handleSemanticRequest(
        operation, "{\"probe\":1}", response, error);
    return handled && error.empty() && mock.lastOperation == expected
        && mock.lastRequest == "{\"probe\":1}" && !response.empty();
}

} // namespace

int main() {
    MockSibyl mock;
    int failures = 0;
    const auto check = [&](bool pass, const char* name) {
        std::cout << (pass ? "[PASS] " : "[FAIL] ") << name << "\n";
        if (!pass) ++failures;
    };

    check(std::string(mock.semanticCapabilityId()) ==
        "leviathan.sibyl.composition", "Sibyl advertises its generic capability id");
    check(checkMapping(mock, OctaviaSemanticControl::Operation::GET_DOCUMENT,
        SibylControl::Operation::GET_COMPOSITION), "GET_DOCUMENT maps to GET_COMPOSITION");
    check(checkMapping(mock, OctaviaSemanticControl::Operation::VALIDATE,
        SibylControl::Operation::VALIDATE), "VALIDATE maps without schema interpretation");
    check(checkMapping(mock, OctaviaSemanticControl::Operation::EDIT,
        SibylControl::Operation::EDIT), "EDIT maps to the existing commit point");
    check(checkMapping(mock, OctaviaSemanticControl::Operation::GET_STATUS,
        SibylControl::Operation::GET_STATUS), "GET_STATUS maps unchanged");
    check(checkMapping(mock, OctaviaSemanticControl::Operation::COMMAND,
        SibylControl::Operation::TRANSPORT), "COMMAND maps to TRANSPORT");

    std::string response;
    std::string error;
    const bool capabilityHandled = mock.handleSemanticRequest(
        OctaviaSemanticControl::Operation::CAPABILITIES, "{}", response, error);
    check(capabilityHandled && mock.lastOperation == SibylControl::Operation::CAPABILITIES
        && response.find("\"capabilityId\":\"leviathan.sibyl.composition\"") !=
            std::string::npos,
        "generic capabilities include a dispatcher-verifiable capability id");
    return failures == 0 ? 0 : 1;
}
