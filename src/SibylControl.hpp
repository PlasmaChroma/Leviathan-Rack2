#pragma once

#include "OctaviaSemanticControl.hpp"

#include <string>

// Optional in-process reference adapter implemented by Sibyl for Octavia.
// Sibyl's documented JSON state and semantic protocol remain usable by other
// bridges; this C++ RTTI interface is not intended as a cross-plugin ABI.
// Octavia treats JSON as opaque: Sibyl owns its schema, validation, revisions,
// compilation, and transport semantics.
struct SibylControl : OctaviaSemanticControl {
    enum class Operation {
        CAPABILITIES,
        GET_COMPOSITION,
        VALIDATE,
        EDIT,
        GET_STATUS,
        TRANSPORT,
        DEBUG_CAPTURE,
    };

    virtual ~SibylControl() = default;

    // Called on Rack's UI thread. On success, responseJson must contain one JSON
    // object following Sibyl's public response contract. A rejected request may
    // put its structured {ok:false,error:{...}} envelope in responseJson; error
    // is the fallback diagnostic when no structured response is available.
    // EDIT must return true only after accepting a mutation; Octavia interprets
    // that value as the commit point and records exactly one undo snapshot.
    // The opaque v1 request contract includes EDIT phasePolicy and TRANSPORT
    // restart target/phaseMode fields; Sibyl validates their combinations.
    virtual bool handleSibylRequest(Operation operation,
                                    const std::string& requestJson,
                                    std::string& responseJson,
                                    std::string& error) = 0;

    const char* semanticCapabilityId() const noexcept override {
        return "leviathan.sibyl.composition";
    }

    bool handleSemanticRequest(OctaviaSemanticControl::Operation operation,
                               const std::string& requestJson,
                               std::string& responseJson,
                               std::string& error) override {
        Operation sibylOperation = Operation::GET_STATUS;
        switch (operation) {
            case OctaviaSemanticControl::Operation::CAPABILITIES:
                sibylOperation = Operation::CAPABILITIES; break;
            case OctaviaSemanticControl::Operation::GET_DOCUMENT:
                sibylOperation = Operation::GET_COMPOSITION; break;
            case OctaviaSemanticControl::Operation::VALIDATE:
                sibylOperation = Operation::VALIDATE; break;
            case OctaviaSemanticControl::Operation::EDIT:
                sibylOperation = Operation::EDIT; break;
            case OctaviaSemanticControl::Operation::GET_STATUS:
                sibylOperation = Operation::GET_STATUS; break;
            case OctaviaSemanticControl::Operation::COMMAND:
                sibylOperation = Operation::TRANSPORT; break;
        }
        const bool handled = handleSibylRequest(
            sibylOperation, requestJson, responseJson, error);
        if (handled && operation == OctaviaSemanticControl::Operation::CAPABILITIES
                && !responseJson.empty() && responseJson.front() == '{') {
            responseJson.insert(1,
                "\"capabilityId\":\"leviathan.sibyl.composition\",");
        }
        return handled;
    }
};
