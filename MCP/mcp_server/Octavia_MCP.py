#!/usr/bin/env python3
"""
VCV Rack MCP Server — Leviathan Octavia
Connects MCP-compatible agents to VCV Rack through the Octavia module.
"""

import json
import os
import httpx
from typing import Optional, Literal
from pydantic import BaseModel, Field, ConfigDict
from mcp.server.fastmcp import FastMCP

mcp = FastMCP("vcv_rack_mcp")


def _read_bridge_port() -> int:
    raw = os.environ.get("OCTAVIA_PORT", "7777")
    try:
        port = int(raw)
    except ValueError as exc:
        raise RuntimeError(f"OCTAVIA_PORT must be an integer, got {raw!r}") from exc
    if not 1 <= port <= 65535:
        raise RuntimeError(f"OCTAVIA_PORT must be between 1 and 65535, got {port}")
    return port


BRIDGE_PORT = _read_bridge_port()
BRIDGE_URL = f"http://127.0.0.1:{BRIDGE_PORT}"
BRIDGE_TOKEN = os.environ.get("OCTAVIA_TOKEN", "")
BRIDGE_HEADERS = {"X-Octavia-Token": BRIDGE_TOKEN} if BRIDGE_TOKEN else {}


# ── Shared helpers ────────────────────────────────────────────────────────────

async def _call(endpoint: str, method: str = "GET", data: dict = None) -> dict:
    async with httpx.AsyncClient(timeout=5.0) as client:
        if method == "GET":
            r = await client.get(f"{BRIDGE_URL}/{endpoint}", headers=BRIDGE_HEADERS)
        elif method == "DELETE":
            r = await client.delete(f"{BRIDGE_URL}/{endpoint}", headers=BRIDGE_HEADERS)
        else:
            r = await client.post(f"{BRIDGE_URL}/{endpoint}", json=data or {}, headers=BRIDGE_HEADERS)
        r.raise_for_status()
        payload = r.json()
        if isinstance(payload, dict) and "error" in payload:
            raise OctaviaBridgeError(str(payload["error"]))
        return payload


async def _resolve_port(module_id: int, port_kind: str,
                        port_id: Optional[int], port_name: Optional[str]) -> int:
    """Resolve an input/output port by index or case-insensitive display name."""
    if port_id is not None:
        return port_id
    if not port_name:
        raise ValueError(f"Provide {port_kind}_port_id or {port_kind}_port_name")

    module = await _call(f"modules/{module_id}")
    ports = module.get(f"{port_kind}s", [])
    wanted = port_name.casefold()
    matches = [p for p in ports if str(p.get("name", "")).casefold() == wanted]
    if not matches:
        available = ", ".join(f'{p.get("id")}: {p.get("name")}' for p in ports)
        raise ValueError(
            f"No {port_kind} named {port_name!r} on module {module_id}. "
            f"Available: {available or 'none'}"
        )
    return int(matches[0]["id"])


class OctaviaBridgeError(RuntimeError):
    """A failure reported by Octavia's local HTTP bridge."""


def _error_message(e: Exception) -> str:
    if isinstance(e, OctaviaBridgeError):
        return f"Error: {e}"
    if isinstance(e, (httpx.ConnectError, httpx.ConnectTimeout)):
        return (
            "Error: Cannot reach the Octavia module. "
            "Make sure VCV Rack is running and press START on the Octavia module."
        )
    if isinstance(e, httpx.TimeoutException):
        return "Error: VCV Rack timed out. Check that the Octavia module is active."
    if isinstance(e, httpx.HTTPStatusError):
        return f"Error: VCV Rack returned HTTP {e.response.status_code}: {e.response.text}"
    return f"Error: {type(e).__name__}: {e}"


def _err(e: Exception) -> None:
    """Raise a tool failure so MCP clients receive an error result, not success text."""
    raise RuntimeError(_error_message(e)) from e


# ── Status & Performance Tools ────────────────────────────────────────────────

@mcp.tool(
    name="vcv_get_status",
    annotations={"title": "Get Octavia Status", "readOnlyHint": True, "destructiveHint": False}
)
async def vcv_get_status() -> str:
    """Get the current status of the Octavia bridge in VCV Rack and the current patch file info.

    Returns whether the bridge is running, its configured port, API version, and patch file path.
    Use this first to verify the connection before calling other tools.

    Returns:
        str: JSON with running (bool), port (int), version (str), patch {path, hasSavePath}
    """
    try:
        status = await _call("status")
        try:
            status["patch"] = await _call("patch")
        except Exception:
            pass
        return json.dumps(status, indent=2)
    except Exception as e:
        return _err(e)


@mcp.tool(
    name="vcv_get_perf",
    annotations={"title": "Get Performance Stats", "readOnlyHint": True, "destructiveHint": False}
)
async def vcv_get_perf() -> str:
    """Get VCV Rack engine performance and complexity statistics.

    Returns sample rate, block size, block duration, module/cable counts, and process CPU time.
    """
    try:
        return json.dumps(await _call("perf"), indent=2)
    except Exception as e:
        return _err(e)


# ── Inspection Tools ──────────────────────────────────────────────────────────

@mcp.tool(
    name="vcv_list_modules",
    annotations={"title": "List Modules", "readOnlyHint": True, "destructiveHint": False}
)
async def vcv_list_modules() -> str:
    """List all modules currently loaded in the patch (compact summary: id, plugin, model).

    Use vcv_get_module(module_id) to inspect parameters and ports for a specific module.
    """
    try:
        return json.dumps(await _call("modules/summary"), indent=2)
    except Exception as e:
        return _err(e)


class GetModuleInput(BaseModel):
    model_config = ConfigDict(str_strip_whitespace=True, extra="forbid")
    module_id: int = Field(..., description="Module ID from vcv_list_modules", ge=0)


@mcp.tool(
    name="vcv_get_module",
    annotations={"title": "Get Module Detail", "readOnlyHint": True, "destructiveHint": False}
)
async def vcv_get_module(params: GetModuleInput) -> str:
    """Get full details for a single module: all parameters (id, name, value, min, max, unit),
    input ports (id, name, voltage, connected), and output ports.
    """
    try:
        return json.dumps(await _call(f"modules/{params.module_id}"), indent=2)
    except Exception as e:
        return _err(e)


class ListLibraryInput(BaseModel):
    model_config = ConfigDict(str_strip_whitespace=True, extra="forbid")
    plugin: Optional[str] = Field(None, description="Filter by exact plugin slug (e.g. 'Fundamental')")
    q: Optional[str] = Field(None, description="Search query matching module slug or name")


@mcp.tool(
    name="vcv_list_library",
    annotations={"title": "List Library", "readOnlyHint": True, "destructiveHint": False}
)
async def vcv_list_library(params: ListLibraryInput) -> str:
    """List installed VCV Rack plugins and modules. Always filter by plugin or search term."""
    try:
        if not params.plugin and not params.q:
            raise ValueError("Provide at least one of 'plugin' or 'q' to avoid an unbounded library response")
        query_params = {}
        if params.plugin:
            query_params["plugin"] = params.plugin
        if params.q:
            query_params["q"] = params.q
        async with httpx.AsyncClient(timeout=10.0) as client:
            r = await client.get(f"{BRIDGE_URL}/library", params=query_params, headers=BRIDGE_HEADERS)
            r.raise_for_status()
            payload = r.json()
            if isinstance(payload, dict) and "error" in payload:
                raise OctaviaBridgeError(str(payload["error"]))
            return json.dumps(payload, indent=2)
    except Exception as e:
        return _err(e)


class ListCablesInput(BaseModel):
    model_config = ConfigDict(str_strip_whitespace=True, extra="forbid")
    module_id: Optional[int] = Field(None, description="Filter to cables connected to this module ID")


@mcp.tool(
    name="vcv_list_cables",
    annotations={"title": "List Cables", "readOnlyHint": True, "destructiveHint": False}
)
async def vcv_list_cables(params: ListCablesInput) -> str:
    """List cables currently patched, with source/destination module IDs, port IDs, and names."""
    try:
        cables = await _call("cables")
        if params.module_id is not None:
            mid = params.module_id
            cables = [c for c in cables
                      if c.get("outputModuleId") == mid or c.get("inputModuleId") == mid]
        return json.dumps(cables, indent=2)
    except Exception as e:
        return _err(e)


# ── Signal & Patch Diagnostics ────────────────────────────────────────────────

@mcp.tool(
    name="vcv_get_signal_levels",
    annotations={"title": "Get Signal Levels", "readOnlyHint": True, "destructiveHint": False}
)
async def vcv_get_signal_levels() -> str:
    """Get a voltage and peak snapshot for all module outputs in the patch.

    Use this to identify active signal chains, Octavia full-scale overrange
    (peak >= 5V), or silent sections.
    """
    try:
        return json.dumps(await _call("modules/voltages"), indent=2)
    except Exception as e:
        return _err(e)


class AnalyzeAudioInput(BaseModel):
    model_config = ConfigDict(str_strip_whitespace=True, extra="forbid")
    mode: Literal["spectrum", "loudness"] = Field(
        "spectrum",
        description="'spectrum' for real-time frequency bands/resonances/issues; 'loudness' for EBU R128-style momentary, short-term, and integrated LUFS plus crest factor and stereo correlation."
    )
    port: int = Field(0, description="For spectrum mode: 0 for Left input, 1 for Right input. Defaults to 0.", ge=0, le=1)
    include_spectrum: bool = Field(False, description="For spectrum mode: include raw 1/12-octave frequency bins. This produces a much larger response.")


@mcp.tool(
    name="vcv_analyze_audio",
    annotations={"title": "Analyze Audio", "readOnlyHint": True, "destructiveHint": False}
)
async def vcv_analyze_audio(params: AnalyzeAudioInput = AnalyzeAudioInput()) -> str:
    """Analyze real-time audio fed into Octavia's Audio Analyze inputs.

    Supports:
    - 'spectrum': real-time snapshot of frequency bands (sub/bass/mid/air), standing resonances, DC offset, hum, and feedback.
    - 'loudness': momentary (400 ms), short-term (3 s), and dual-gated integrated LUFS, plus L/R sample peak/crest factor, stereo phase correlation, and Mid/Side balance.
    """
    try:
        if params.mode == "loudness":
            return json.dumps(await _call("audio/loudness"), indent=2)
        query = "?spectrum=1" if params.include_spectrum else ""
        return json.dumps(await _call(f"audio/{params.port}/analyze{query}"), indent=2)
    except Exception as e:
        return _err(e)


@mcp.tool(
    name="vcv_reset_loudness",
    annotations={"title": "Reset Loudness Meter", "readOnlyHint": False, "destructiveHint": False}
)
async def vcv_reset_loudness() -> str:
    """Start a fresh loudness measurement window for Octavia's Analyze L/R inputs."""
    try:
        return json.dumps(await _call("audio/loudness/reset", "POST"), indent=2)
    except Exception as e:
        return _err(e)


class TemporalDeckTransportInput(BaseModel):
    model_config = ConfigDict(str_strip_whitespace=True, extra="forbid")
    module_id: int
    action: Literal["load", "play", "stop_rewind", "seek", "set_loop", "status"]
    path: Optional[str] = Field(None, description="Required for load: absolute local path to a supported audio file.")
    position: Optional[float] = Field(None, ge=0, le=1, description="Required for seek: normalized position, 0=start and 1=end.")
    enabled: Optional[bool] = Field(None, description="Required for set_loop.")


@mcp.tool(
    name="vcv_temporal_deck_transport",
    annotations={"title": "Control Temporal Deck Sample Transport", "readOnlyHint": False, "destructiveHint": False}
)
async def vcv_temporal_deck_transport(params: TemporalDeckTransportInput) -> str:
    """Load and control a Temporal Deck sample without changing its user interface.

    Use status after load until loaded=true, then stop_rewind, reset Octavia's
    loudness meter, play, and measure. This is intended for repeatable local
    reference-audio tests as well as normal transport control.
    """
    try:
        if params.action == "load" and not params.path:
            raise ValueError("path is required for action='load'")
        if params.action == "seek" and params.position is None:
            raise ValueError("position is required for action='seek'")
        if params.action == "set_loop" and params.enabled is None:
            raise ValueError("enabled is required for action='set_loop'")
        payload = {"action": params.action}
        if params.path is not None: payload["path"] = params.path
        if params.position is not None: payload["position"] = params.position
        if params.enabled is not None: payload["enabled"] = params.enabled
        return json.dumps(await _call(f"temporal-deck/{params.module_id}/transport", "POST", payload), indent=2)
    except Exception as e:
        return _err(e)


@mcp.tool(
    name="vcv_find_unpatched",
    annotations={"title": "Find Unpatched Ports", "readOnlyHint": True, "destructiveHint": False}
)
async def vcv_find_unpatched() -> str:
    """Find modules with unconnected ports, including port names and smart generator filtering."""
    try:
        modules_full = await _call("modules")
        cables       = await _call("cables")

        connected_in:  set = set()
        connected_out: set = set()
        for c in cables:
            connected_in.add((c["inputModuleId"],  c["inputPortId"]))
            connected_out.add((c["outputModuleId"], c["outputPortId"]))

        result = []
        for m in modules_full:
            if m.get("plugin") == "Leviathan" and m.get("model") == "Octavia":
                continue

            mid     = m["id"]
            inputs  = m.get("inputs",  [])
            outputs = m.get("outputs", [])
            is_pure_source = len(inputs) == 0

            uncon_in  = [{"id": p["id"], "name": p["name"]}
                         for p in inputs  if (mid, p["id"]) not in connected_in]
            uncon_out = [{"id": p["id"], "name": p["name"]}
                         for p in outputs if (mid, p["id"]) not in connected_out]

            if is_pure_source and len(uncon_out) < len(outputs):
                uncon_out = []

            if uncon_in or uncon_out:
                entry: dict = {"id": mid, "plugin": m["plugin"], "model": m["model"]}
                if uncon_in:
                    entry["unconnectedInputs"]  = uncon_in
                if uncon_out:
                    entry["unconnectedOutputs"] = uncon_out
                result.append(entry)

        return json.dumps(result, indent=2)
    except Exception as e:
        return _err(e)


# ── Module Lifecycle & Arrangement ───────────────────────────────────────────

class AddModuleInput(BaseModel):
    model_config = ConfigDict(str_strip_whitespace=True, extra="forbid")
    plugin: str = Field(..., description="Plugin slug, e.g. 'Fundamental' or 'Bogaudio'")
    model: str = Field(..., description="Model slug within the plugin, e.g. 'VCF' or 'VCO'")


@mcp.tool(
    name="vcv_add_module",
    annotations={"title": "Add Module", "readOnlyHint": False, "destructiveHint": False}
)
async def vcv_add_module(params: AddModuleInput) -> str:
    """Add a module to the current patch. Search with vcv_list_library first for exact slugs."""
    try:
        return json.dumps(await _call("modules", "POST", {
            "plugin": params.plugin, "model": params.model
        }), indent=2)
    except Exception as e:
        return _err(e)


class DeleteModuleInput(BaseModel):
    model_config = ConfigDict(str_strip_whitespace=True, extra="forbid")
    module_id: int = Field(..., description="Module ID to delete", ge=0)


@mcp.tool(
    name="vcv_delete_module",
    annotations={"title": "Delete Module", "readOnlyHint": False, "destructiveHint": True}
)
async def vcv_delete_module(params: DeleteModuleInput) -> str:
    """Permanently delete a module and its cables. Obtain explicit user confirmation first."""
    try:
        return json.dumps(await _call(f"modules/{params.module_id}", "DELETE"), indent=2)
    except Exception as e:
        return _err(e)


class UpdateModuleInput(BaseModel):
    model_config = ConfigDict(str_strip_whitespace=True, extra="forbid")
    module_id: int = Field(..., description="Module ID from vcv_list_modules", ge=0)
    hp: Optional[float] = Field(None, description="Horizontal position in HP (rack units, 1 HP = 5.08mm)")
    row: Optional[int] = Field(None, description="Rack row index. Omit to preserve the module's current row")
    bypassed: Optional[bool] = Field(None, description="True to bypass module, False to re-enable")


@mcp.tool(
    name="vcv_update_module",
    annotations={"title": "Update Module", "readOnlyHint": False, "destructiveHint": False}
)
async def vcv_update_module(params: UpdateModuleInput) -> str:
    """Update a module's rack position and/or bypass state.

    Position and bypass are separate writes and therefore separate undo steps. If one
    succeeds before a later write fails, the response reports the applied operation.
    Moving by HP alone preserves the current row; provide row to move between rows.
    """
    if params.hp is None and (params.row is not None or params.bypassed is None):
        if params.row is not None:
            raise ValueError("'row' requires 'hp'")
        raise ValueError("Provide at least one of 'hp' or 'bypassed'")
    results = {}
    applied = []
    if params.hp is not None:
        try:
            payload = {"hp": params.hp}
            if params.row is not None:
                payload["row"] = params.row
            results["position"] = await _call(
                f"modules/{params.module_id}/position", "POST", payload
            )
            applied.append("position")
        except Exception as e:
            return json.dumps({
                "ok": False,
                "applied": applied,
                "failedOperation": "position",
                "error": _error_message(e),
            }, indent=2)
    if params.bypassed is not None:
        try:
            results["bypass"] = await _call(
                f"modules/{params.module_id}/bypass", "POST", {"bypassed": params.bypassed}
            )
            applied.append("bypass")
        except Exception as e:
            return json.dumps({
                "ok": False,
                "applied": applied,
                "failedOperation": "bypass",
                "error": _error_message(e),
            }, indent=2)
    return json.dumps({"ok": True, "applied": applied, "results": results}, indent=2)


class LayoutChange(BaseModel):
    model_config = ConfigDict(extra="forbid")
    module_id: int = Field(..., description="Module ID", ge=0)
    hp: float = Field(..., description="Horizontal position in HP")
    row: int = Field(..., description="Rack row index")


class LayoutModulesInput(BaseModel):
    model_config = ConfigDict(extra="forbid")
    changes: list[LayoutChange] = Field(..., min_length=1, description="Complete set of module positions")


@mcp.tool(
    name="vcv_layout_modules",
    annotations={"title": "Layout Modules", "readOnlyHint": False, "destructiveHint": False}
)
async def vcv_layout_modules(params: LayoutModulesInput) -> str:
    """Atomically arrange modules across rack rows as one undoable operation.

    The bridge validates all modules and target rectangles before moving anything.
    Collisions reject the whole operation. The response reports resolved positions.
    Use separate rows when functional lanes improve readability, such as sequencing,
    drums, melodic voices, and mixing/effects, with signal flow left-to-right per row.
    """
    changes = [
        {"moduleId": change.module_id, "hp": change.hp, "row": change.row}
        for change in params.changes
    ]
    try:
        return json.dumps(await _call("modules/layout", "POST", {"changes": changes}), indent=2)
    except Exception as e:
        return _err(e)


# ── Parameter & Preset Operations ─────────────────────────────────────────────

class ParamChange(BaseModel):
    model_config = ConfigDict(extra="forbid")
    module_id: int = Field(..., description="Module ID", ge=0)
    param_id: int = Field(..., description="Parameter index on module", ge=0)
    value: float = Field(..., description="New native-range value (e.g. BPM, V/oct volts, Hz)")


class SetParamsInput(BaseModel):
    model_config = ConfigDict(extra="forbid")
    changes: list[ParamChange] = Field(
        ..., min_length=1,
        description="One or more parameter changes applied in a single undo step"
    )


@mcp.tool(
    name="vcv_set_parameters",
    annotations={"title": "Set Parameters", "readOnlyHint": False, "destructiveHint": False}
)
async def vcv_set_parameters(params: SetParamsInput) -> str:
    """Set one or more knob or switch values using native ranges as a single undoable action.

    Read parameter IDs and ranges with vcv_get_module first. Clamped automatically by Octavia.
    """
    try:
        changes = [{"moduleId": c.module_id, "paramId": c.param_id, "value": c.value}
                   for c in params.changes]
        return json.dumps(await _call("params/bulk", "POST", {"changes": changes}), indent=2)
    except Exception as e:
        return _err(e)


class ModuleStateInput(BaseModel):
    model_config = ConfigDict(str_strip_whitespace=True, extra="forbid")
    module_id: int = Field(..., description="Module ID", ge=0)


@mcp.tool(
    name="vcv_get_module_state",
    annotations={"title": "Get Module State", "readOnlyHint": True, "destructiveHint": False}
)
async def vcv_get_module_state(params: ModuleStateInput) -> str:
    """Get the full internal preset state of a module as raw JSON."""
    try:
        async with httpx.AsyncClient(timeout=5.0) as client:
            r = await client.get(f"{BRIDGE_URL}/modules/{params.module_id}/state", headers=BRIDGE_HEADERS)
            r.raise_for_status()
            payload = r.json()
            if isinstance(payload, dict) and "error" in payload:
                raise OctaviaBridgeError(str(payload["error"]))
            return r.text
    except Exception as e:
        return _err(e)


class SetModuleStateInput(BaseModel):
    model_config = ConfigDict(str_strip_whitespace=True, extra="forbid")
    module_id: int = Field(..., description="Module ID to restore state into", ge=0)
    state_json: str = Field(..., description="Raw JSON preset string from vcv_get_module_state")


@mcp.tool(
    name="vcv_set_module_state",
    annotations={"title": "Restore Module State", "readOnlyHint": False, "destructiveHint": False}
)
async def vcv_set_module_state(params: SetModuleStateInput) -> str:
    """Restore a module's internal preset state from JSON. Revert with vcv_undo."""
    try:
        state = json.loads(params.state_json)
        if not isinstance(state, dict):
            raise ValueError("state_json must contain a JSON object")
        return json.dumps(await _call(
            f"modules/{params.module_id}/state", "POST", state
        ), indent=2)
    except Exception as e:
        return _err(e)


# ── Cabling Operations ────────────────────────────────────────────────────────

class CableConnection(BaseModel):
    model_config = ConfigDict(str_strip_whitespace=True, extra="forbid")
    output_module_id: int = Field(..., description="Source module ID", ge=0)
    output_port_id: Optional[int] = Field(None, description="Output port index (0-based). Use this OR output_port_name.", ge=0)
    output_port_name: Optional[str] = Field(None, description="Output port name (case-insensitive, e.g. 'Saw', 'Out').")
    input_module_id: int = Field(..., description="Destination module ID", ge=0)
    input_port_id: Optional[int] = Field(None, description="Input port index (0-based). Use this OR input_port_name.", ge=0)
    input_port_name: Optional[str] = Field(None, description="Input port name (case-insensitive, e.g. 'V/Oct', 'In').")
    color: Optional[str] = Field(None, description="Cable color name ('white', 'red', 'green', 'blue', 'yellow', 'orange', 'purple', 'cyan', 'magenta', 'gray', 'black') or hex ('#ffffff'). Defaults to 'white'.")


class ConnectCablesInput(BaseModel):
    model_config = ConfigDict(str_strip_whitespace=True, extra="forbid")
    connections: list[CableConnection] = Field(
        ..., min_length=1,
        description="List of one or more cables to connect"
    )


@mcp.tool(
    name="vcv_connect_cables",
    annotations={"title": "Connect Cables", "readOnlyHint": False, "destructiveHint": False}
)
async def vcv_connect_cables(params: ConnectCablesInput) -> str:
    """Connect one or more cables in order, resolving port IDs or names. Revert with vcv_undo.

    Each connection creates an undo step. Stops at first error and reports applied count.
    """
    applied = []
    failed_index = None
    try:
        for index, c in enumerate(params.connections):
            failed_index = index
            out_port = await _resolve_port(c.output_module_id, "output",
                                           c.output_port_id, c.output_port_name)
            in_port = await _resolve_port(c.input_module_id, "input",
                                          c.input_port_id, c.input_port_name)
            payload = {
                "outputModuleId": c.output_module_id, "outputPortId": out_port,
                "inputModuleId": c.input_module_id, "inputPortId": in_port,
            }
            if c.color:
                payload["color"] = c.color
            result = await _call("cables", "POST", payload)
            applied.append(index)
        return json.dumps({"ok": True, "applied": len(applied)}, indent=2)
    except Exception as e:
        return json.dumps({
            "ok": False,
            "applied": len(applied),
            "failedIndex": failed_index,
            "error": _error_message(e),
        }, indent=2)


class DisconnectCableInput(BaseModel):
    model_config = ConfigDict(str_strip_whitespace=True, extra="forbid")
    module_id: int = Field(..., description="Module ID to disconnect from", ge=0)
    port_id: Optional[int] = Field(None, description="Port index (0-based). Use this OR port_name.", ge=0)
    port_name: Optional[str] = Field(None, description="Port name (case-insensitive, e.g. 'V/Oct', 'Out').")
    direction: Literal["input", "output"] = Field(..., description="Disconnect from 'input' port or 'output' port")


@mcp.tool(
    name="vcv_disconnect_cable",
    annotations={"title": "Disconnect Cable", "readOnlyHint": False, "destructiveHint": False}
)
async def vcv_disconnect_cable(params: DisconnectCableInput) -> str:
    """Disconnect cables from a specified input or output port. Revert with vcv_undo."""
    try:
        port = await _resolve_port(params.module_id, params.direction, params.port_id, params.port_name)
        if params.direction == "input":
            return json.dumps(await _call("cables/disconnect", "POST", {
                "inputModuleId": params.module_id,
                "inputPortId": port,
            }), indent=2)
        else:
            return json.dumps(await _call("cables/disconnect-output", "POST", {
                "outputModuleId": params.module_id,
                "outputPortId": port,
            }), indent=2)
    except Exception as e:
        return _err(e)


# ── Undo & Patch Persistence ──────────────────────────────────────────────────

class UndoInput(BaseModel):
    model_config = ConfigDict(str_strip_whitespace=True, extra="forbid")
    status_only: bool = Field(False, description="If True, lists undoable actions without popping the undo stack")


@mcp.tool(
    name="vcv_undo",
    annotations={"title": "Undo / Inspect Undo Stack", "readOnlyHint": False, "destructiveHint": False}
)
async def vcv_undo(params: UndoInput = UndoInput()) -> str:
    """Undo the most recent reversible Octavia write operation, or inspect the undo stack.

    Set status_only=True to view the list of reversible operations without modifying the patch.
    """
    try:
        if params.status_only:
            return json.dumps(await _call("undo/status"), indent=2)
        return json.dumps(await _call("undo", "POST"), indent=2)
    except Exception as e:
        return _err(e)


@mcp.tool(
    name="vcv_save_patch",
    annotations={"title": "Save Patch", "readOnlyHint": False, "destructiveHint": True}
)
async def vcv_save_patch() -> str:
    """Save the current patch to its existing file path. Obtain explicit user confirmation first."""
    try:
        return json.dumps(await _call("patch/save", "POST"), indent=2)
    except Exception as e:
        return _err(e)


if __name__ == "__main__":
    mcp.run()
