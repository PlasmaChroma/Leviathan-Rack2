# Generic Semantic Documents

Use the generic semantic tools for structured authored state that is richer than Rack
parameters but does not have a dedicated agent workflow. The target module owns its
schema, validation, compilation, and revision. Octavia transports requests opaquely and
makes each successful semantic edit one Rack undo action.

## Discovery and editing

1. Connect normally and resolve the exact module ID.
2. Call `vcv_semantic_get_capabilities`. Do not infer support from the module name.
3. Use only document views and operations advertised by that capability response. When
   `requestSchemas` is present, treat its machine-readable schemas and embedded examples
   as the authority for constructing requests; do not rely on module-name knowledge.
4. Read `vcv_semantic_get_status` and the smallest useful document view immediately
   before editing.
5. Validate a proposed request when the capability supports validation.
6. Edit using the last observed revision. On `revision_conflict`, reread, rebase, and
   retry; never increment a stale revision blindly.
7. Verify with a focused document read. Use `vcv_undo` if verification fails.

Do not send `vcv_semantic_command` unless the capability explicitly advertises commands.
Commands are ephemeral and are not Rack undo actions.

## Self-describing capability convention

Modules should return a `requestSchemas` object from their existing capability response,
keyed by the advertised operation name. Each value is a self-contained JSON Schema and
should include concise descriptions plus one or more valid `examples`. Capability-specific
limits that JSON Schema cannot express exactly may use documented `x-` extension fields.

This convention is additive: older capabilities without schemas remain usable through
their documented fields, and the target module's validation remains authoritative even
when a request satisfies its advertised schema. Do not duplicate module request grammars
in this skill; the live capability response is the source of truth.
