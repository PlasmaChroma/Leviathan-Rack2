#pragma once

#include <string>

// Optional in-process semantic adapter for modules controlled through Octavia.
// This is a local RTTI interface, not a cross-plugin ABI. Implementations own
// their JSON schema, revisions, validation, compilation, and command semantics.
struct OctaviaSemanticControl {
    enum class Operation {
        CAPABILITIES,
        GET_DOCUMENT,
        VALIDATE,
        EDIT,
        GET_STATUS,
        COMMAND,
    };

    virtual ~OctaviaSemanticControl() = default;
    virtual const char* semanticCapabilityId() const noexcept = 0;
    virtual bool handleSemanticRequest(Operation operation,
                                       const std::string& requestJson,
                                       std::string& responseJson,
                                       std::string& error) = 0;
};
