#pragma once

#include <string>

// Optional in-process reference adapter implemented by Sibyl for Octavia.
// Sibyl's documented JSON state and semantic protocol remain usable by other
// bridges; this C++ RTTI interface is not intended as a cross-plugin ABI.
// Octavia treats JSON as opaque: Sibyl owns its schema, validation, revisions,
// compilation, and transport semantics.
struct SibylControl {
    enum class Operation {
        CAPABILITIES,
        GET_COMPOSITION,
        VALIDATE,
        EDIT,
        GET_STATUS,
        TRANSPORT,
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
};
