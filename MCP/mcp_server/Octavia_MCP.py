#!/usr/bin/env python3
"""
VCV Rack MCP Server — Leviathan Octavia
Connects MCP-compatible agents to VCV Rack through the Octavia module.
"""

import json
import os
import httpx
from typing import Optional
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



# ── Tools ─────────────────────────────────────────────────────────────────────

@mcp.tool(
    name="vcv_get_status",
    annotations={"title": "Get Octavia Status", "readOnlyHint": True, "destructiveHint": False}
)
async def vcv_get_status() -> str:
    """Get the current status of the Octavia MCP server running inside VCV Rack.

    Returns whether the server is running, its configured port, and the Octavia API version.
    Use this first to verify the connection before calling other tools.

    Returns:
        str: JSON with keys: running (bool), port (int), version (str)
        Raises a tool error with instructions for restoring the connection.
    """
    try:
        return json.dumps(await _call("status"), indent=2)
    except Exception as e:
        return _err(e)


@mcp.tool(
    name="vcv_list_modules",
    annotations={"title": "List Modules", "readOnlyHint": True, "destructiveHint": False}
)
async def vcv_list_modules() -> str:
    """List all modules currently loaded in the VCV Rack patch (summary: id, plugin, model only).

    Returns a compact list suitable for large patches. Use vcv_get_module(module_id) to get
    full parameter and port details for a specific module.

    Returns:
        str: JSON array of modules, each with: id (int), plugin (str), model (str)
        Raises a tool error if Octavia cannot provide the module list.
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
    """Get full details for a single module: all parameters (id, name, value, min, max),
    all input ports (id, name, voltage, connected), and all output ports.

    Use vcv_list_modules first to find the module_id, then call this for the module
    you want to inspect or modify.

    Args:
        params.module_id (int): Module ID from vcv_list_modules

    Returns:
        str: JSON object with id, plugin, model, params[], inputs[], outputs[]
        Raises a tool error if the module cannot be created.
    """
    try:
        return json.dumps(await _call(f"modules/{params.module_id}"), indent=2)
    except Exception as e:
        return _err(e)


class AddModuleInput(BaseModel):
    model_config = ConfigDict(str_strip_whitespace=True, extra="forbid")
    plugin: str = Field(..., description="Plugin slug, e.g. 'Fundamental' or 'ImpromptuModular'")
    model: str = Field(..., description="Model slug within the plugin, e.g. 'VCF' or 'Clocked'")


@mcp.tool(
    name="vcv_add_module",
    annotations={"title": "Add Module", "readOnlyHint": False, "destructiveHint": False}
)
async def vcv_add_module(params: AddModuleInput) -> str:
    """Add an installed module to the current patch.

    Search with vcv_list_library first to obtain exact plugin and model slugs. The
    module is placed near the existing patch and its new module ID is returned.
    This operation can be reverted with vcv_undo.
    """
    try:
        return json.dumps(await _call("modules", "POST", {
            "plugin": params.plugin, "model": params.model
        }), indent=2)
    except Exception as e:
        return _err(e)



class GetParamInput(BaseModel):
    model_config = ConfigDict(str_strip_whitespace=True, extra="forbid")
    module_id: int = Field(..., description="Module ID from vcv_list_modules", ge=0)
    param_id: int = Field(..., description="Parameter index on the module", ge=0)


@mcp.tool(
    name="vcv_get_parameter",
    annotations={"title": "Get Parameter", "readOnlyHint": True, "destructiveHint": False}
)
async def vcv_get_parameter(params: GetParamInput) -> str:
    """Get the current value of a knob or switch on a VCV Rack module.

    Args:
        params.module_id (int): Module ID from vcv_list_modules
        params.param_id (int): Parameter index (0 = first knob)

    Returns:
        str: JSON with: module_id, param_id, name, value, min, max, default, unit
    """
    try:
        return json.dumps(
            await _call(f"modules/{params.module_id}/params/{params.param_id}"), indent=2
        )
    except Exception as e:
        return _err(e)


class SetParamInput(BaseModel):
    model_config = ConfigDict(str_strip_whitespace=True, extra="forbid")
    module_id: int = Field(..., description="Module ID from vcv_list_modules", ge=0)
    param_id: int = Field(..., description="Parameter index on the module", ge=0)
    value: float = Field(..., description="New value in the parameter's own range "
                                          "(e.g. BPM 30-300, V/oct volts, frequency). "
                                          "Clamped to the parameter's min/max by Octavia.")


@mcp.tool(
    name="vcv_set_parameter",
    annotations={"title": "Set Parameter", "readOnlyHint": False, "destructiveHint": False}
)
async def vcv_set_parameter(params: SetParamInput) -> str:
    """Set one knob or switch using its native value range.

    Read the module or parameter first instead of guessing IDs or ranges. The
    Bridge clamps out-of-range values. Revert the change with vcv_undo.
    """
    try:
        return json.dumps(await _call(
            f"modules/{params.module_id}/params/{params.param_id}", "POST",
            {"value": params.value}
        ), indent=2)
    except Exception as e:
        return _err(e)



class ConnectCableInput(BaseModel):
    model_config = ConfigDict(str_strip_whitespace=True, extra="forbid")
    output_module_id: int = Field(..., description="Module ID of the signal source", ge=0)
    output_port_id: Optional[int] = Field(None, description="Output port index (0-based). Use this OR output_port_name.")
    output_port_name: Optional[str] = Field(None, description="Output port name (case-insensitive, e.g. 'Saw', 'Clock 1'). Used if output_port_id not provided.")
    input_module_id: int = Field(..., description="Module ID of the signal destination", ge=0)
    input_port_id: Optional[int] = Field(None, description="Input port index (0-based). Use this OR input_port_name.")
    input_port_name: Optional[str] = Field(None, description="Input port name (case-insensitive, e.g. 'Gate', 'V/Oct'). Used if input_port_id not provided.")



class DisconnectCableInput(BaseModel):
    model_config = ConfigDict(str_strip_whitespace=True, extra="forbid")
    input_module_id: int = Field(..., description="Module ID of the cable destination", ge=0)
    input_port_id: int = Field(..., description="Input port index to disconnect", ge=0)



class DeleteModuleInput(BaseModel):
    model_config = ConfigDict(str_strip_whitespace=True, extra="forbid")
    module_id: int = Field(..., description="Module ID to delete (from vcv_list_modules)", ge=0)



class MoveModuleInput(BaseModel):
    model_config = ConfigDict(str_strip_whitespace=True, extra="forbid")
    module_id: int = Field(..., description="Module ID to move (from vcv_list_modules)", ge=0)
    hp: float = Field(..., description=(
        "Horizontal position in HP (rack units). 1 HP = 5.08mm. "
        "Modules snap to the nearest valid HP grid position. "
        "Use this to arrange modules in signal flow order (left to right)."
    ))



class DisconnectOutputInput(BaseModel):
    model_config = ConfigDict(str_strip_whitespace=True, extra="forbid")
    output_module_id: int = Field(..., description="Module ID of the signal source", ge=0)
    output_port_id: int = Field(..., description="Output port index to disconnect all cables from", ge=0)


@mcp.tool(
    name="vcv_connect_cable",
    annotations={"title": "Connect Cable", "readOnlyHint": False, "destructiveHint": False}
)
async def vcv_connect_cable(params: ConnectCableInput) -> str:
    """Connect one output to one input, using port IDs or exact port names.

    Inspect both modules first when the intended ports are uncertain. Connecting
    to an occupied monophonic input may alter the signal path. Revert with vcv_undo.
    """
    try:
        out_port = await _resolve_port(params.output_module_id, "output",
                                       params.output_port_id, params.output_port_name)
        in_port = await _resolve_port(params.input_module_id, "input",
                                      params.input_port_id, params.input_port_name)
        return json.dumps(await _call("cables", "POST", {
            "outputModuleId": params.output_module_id,
            "outputPortId": out_port,
            "inputModuleId": params.input_module_id,
            "inputPortId": in_port,
        }), indent=2)
    except Exception as e:
        return _err(e)


@mcp.tool(
    name="vcv_disconnect_cable",
    annotations={"title": "Disconnect Input", "readOnlyHint": False, "destructiveHint": False}
)
async def vcv_disconnect_cable(params: DisconnectCableInput) -> str:
    """Disconnect all cables from one input port. Revert with vcv_undo."""
    try:
        return json.dumps(await _call("cables/disconnect", "POST", {
            "inputModuleId": params.input_module_id,
            "inputPortId": params.input_port_id,
        }), indent=2)
    except Exception as e:
        return _err(e)


@mcp.tool(
    name="vcv_disconnect_output",
    annotations={"title": "Disconnect Output", "readOnlyHint": False, "destructiveHint": False}
)
async def vcv_disconnect_output(params: DisconnectOutputInput) -> str:
    """Disconnect every cable leaving one output port. Revert with vcv_undo."""
    try:
        return json.dumps(await _call("cables/disconnect-output", "POST", {
            "outputModuleId": params.output_module_id,
            "outputPortId": params.output_port_id,
        }), indent=2)
    except Exception as e:
        return _err(e)


@mcp.tool(
    name="vcv_delete_module",
    annotations={"title": "Delete Module", "readOnlyHint": False, "destructiveHint": True}
)
async def vcv_delete_module(params: DeleteModuleInput) -> str:
    """Permanently delete a module and its cables from the patch.

    This operation is not covered by the Octavia undo stack. Confirm the target
    module with vcv_get_module and get explicit user approval before calling it.
    """
    try:
        return json.dumps(await _call(f"modules/{params.module_id}", "DELETE"), indent=2)
    except Exception as e:
        return _err(e)


@mcp.tool(
    name="vcv_move_module",
    annotations={"title": "Move Module", "readOnlyHint": False, "destructiveHint": False}
)
async def vcv_move_module(params: MoveModuleInput) -> str:
    """Move a module horizontally to an HP position. Revert with vcv_undo."""
    try:
        return json.dumps(await _call(
            f"modules/{params.module_id}/position", "POST", {"hp": params.hp}
        ), indent=2)
    except Exception as e:
        return _err(e)


@mcp.tool(
    name="vcv_get_perf",
    annotations={"title": "Get Performance Stats", "readOnlyHint": True, "destructiveHint": False}
)
async def vcv_get_perf() -> str:
    """Get VCV Rack performance and system statistics.

    Returns engine timing (sample rate, block size, block duration),
    patch complexity (module and cable count), and process CPU time.
    Use this to diagnose performance issues or monitor patch complexity.

    Note: Per-module CPU usage is not available via the VCV Rack SDK.
    processCpuUserSec is cumulative CPU time for the entire VCV Rack process.

    Returns:
        str: JSON with sampleRate, blockFrames, blockDurationMs,
             moduleCount, cableCount, processCpuUserSec, processCpuSysSec
    """
    try:
        return json.dumps(await _call("perf"), indent=2)
    except Exception as e:
        return _err(e)



class BypassModuleInput(BaseModel):
    model_config = ConfigDict(str_strip_whitespace=True, extra="forbid")
    module_id: int = Field(..., description="Module ID from vcv_list_modules", ge=0)
    bypassed: bool = Field(..., description="True to bypass (module passes signal through), False to re-enable")


@mcp.tool(
    name="vcv_set_bypass",
    annotations={"title": "Set Module Bypass", "readOnlyHint": False, "destructiveHint": False}
)
async def vcv_set_bypass(params: BypassModuleInput) -> str:
    """Bypass or re-enable a module. Revert with vcv_undo."""
    try:
        return json.dumps(await _call(
            f"modules/{params.module_id}/bypass", "POST", {"bypassed": params.bypassed}
        ), indent=2)
    except Exception as e:
        return _err(e)



class ModuleStateInput(BaseModel):
    model_config = ConfigDict(str_strip_whitespace=True, extra="forbid")
    module_id: int = Field(..., description="Module ID from vcv_list_modules", ge=0)


@mcp.tool(
    name="vcv_get_module_state",
    annotations={"title": "Get Module State", "readOnlyHint": True, "destructiveHint": False}
)
async def vcv_get_module_state(params: ModuleStateInput) -> str:
    """Get the full internal state (preset) of a module as JSON.

    Returns the complete module state including EQ curves, wavetable positions,
    compressor settings, and all internal DSP state. Use this to save a processing
    chain before changing it, then restore it with vcv_set_module_state.

    Args:
        params.module_id (int): Module ID to snapshot

    Returns:
        str: Raw JSON state of the module (pass this directly to vcv_set_module_state)
    """
    try:
        async with httpx.AsyncClient(timeout=5.0) as client:
            r = await client.get(f"{BRIDGE_URL}/modules/{params.module_id}/state", headers=BRIDGE_HEADERS)
            r.raise_for_status()
            payload = r.json()
            if isinstance(payload, dict) and "error" in payload:
                raise OctaviaBridgeError(str(payload["error"]))
            return r.text  # raw JSON state
    except Exception as e:
        return _err(e)


class SetModuleStateInput(BaseModel):
    model_config = ConfigDict(str_strip_whitespace=True, extra="forbid")
    module_id: int = Field(..., description="Module ID to restore state into", ge=0)
    state_json: str = Field(..., description="Raw JSON state string from vcv_get_module_state")


@mcp.tool(
    name="vcv_set_module_state",
    annotations={"title": "Restore Module State", "readOnlyHint": False, "destructiveHint": False}
)
async def vcv_set_module_state(params: SetModuleStateInput) -> str:
    """Replace a module's internal state with JSON from vcv_get_module_state.

    Use this for exact preset restoration, not routine knob changes. The JSON is
    validated locally before it is sent. Revert with vcv_undo.
    """
    try:
        state = json.loads(params.state_json)
        if not isinstance(state, dict):
            raise ValueError("state_json must contain a JSON object")
        return json.dumps(await _call(
            f"modules/{params.module_id}/state", "POST", state
        ), indent=2)
    except Exception as e:
        return _err(e)



class ListLibraryInput(BaseModel):
    model_config = ConfigDict(str_strip_whitespace=True, extra="forbid")
    plugin: Optional[str] = Field(None, description="Filter by exact plugin slug (e.g. 'Fundamental', 'ImpromptuModular')")
    q: Optional[str] = Field(None, description="Search query — filters by module slug or name (case-insensitive)")


@mcp.tool(
    name="vcv_list_library",
    annotations={"title": "List Library", "readOnlyHint": True, "destructiveHint": False}
)
async def vcv_list_library(params: ListLibraryInput) -> str:
    """List installed VCV Rack plugins and their modules. Always use filters to avoid huge responses.

    Use plugin= to get all modules for one plugin, or q= to search across all plugins.
    Without filters the full library (200+ plugins) is returned — avoid this.

    Args:
        params.plugin (str, optional): Exact plugin slug to filter by (e.g. 'Fundamental')
        params.q (str, optional): Search term matched against module slug and name

    Returns:
        str: JSON array of plugins, each with: slug, name, models[{slug, name}]
    """
    try:
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
    module_id: Optional[int] = Field(None, description="Filter cables by module ID — returns only cables where this module is source or destination")


@mcp.tool(
    name="vcv_list_cables",
    annotations={"title": "List Cables", "readOnlyHint": True, "destructiveHint": False}
)
async def vcv_list_cables(params: ListCablesInput) -> str:
    """List cables currently connected in the VCV Rack patch.

    Returns every cable with output/input module IDs, port IDs, port names,
    and a human-readable label (e.g. "VCO:SAW → VCF:In").
    Use module_id to filter to only cables touching a specific module.

    Args:
        params.module_id (int, optional): Module ID to filter by (from vcv_list_modules)

    Returns:
        str: JSON array of cables, each with: id, outputModuleId, outputModule,
             outputPortId, outputPortName, inputModuleId, inputModule,
             inputPortId, inputPortName, label
    """
    try:
        cables = await _call("cables")
        if params.module_id is not None:
            mid = params.module_id
            cables = [c for c in cables
                      if c.get("outputModuleId") == mid or c.get("inputModuleId") == mid]
        return json.dumps(cables, indent=2)
    except Exception as e:
        return _err(e)


@mcp.tool(
    annotations={"title": "Get Patch Info", "readOnlyHint": True, "destructiveHint": False}
)
async def vcv_get_patch_info() -> str:
    """Get information about the current VCV Rack patch file.

    Returns the current save path and whether the patch has ever been saved.
    Use this before vcv_save_patch to check if a path exists.

    Returns:
        str: JSON with path (str) and hasSavePath (bool)
    """
    try:
        return json.dumps(await _call("patch"), indent=2)
    except Exception as e:
        return _err(e)



class BulkConnection(BaseModel):
    model_config = ConfigDict(str_strip_whitespace=True, extra="forbid")
    output_module_id: int = Field(..., description="Source module ID", ge=0)
    output_port_id: Optional[int] = Field(None, description="Output port index (0-based). Use this OR output_port_name.")
    output_port_name: Optional[str] = Field(None, description="Output port name (case-insensitive). Used if output_port_id not provided.")
    input_module_id: int = Field(..., description="Destination module ID", ge=0)
    input_port_id: Optional[int] = Field(None, description="Input port index (0-based). Use this OR input_port_name.")
    input_port_name: Optional[str] = Field(None, description="Input port name (case-insensitive). Used if input_port_id not provided.")


class BulkConnectInput(BaseModel):
    model_config = ConfigDict(str_strip_whitespace=True, extra="forbid")
    connections: list[BulkConnection] = Field(
        ..., description="List of cables to connect. Each has output_module_id, output_port_id, input_module_id, input_port_id."
    )


class ParamChange(BaseModel):
    model_config = ConfigDict(extra="forbid")
    module_id: int = Field(..., description="Module ID", ge=0)
    param_id: int = Field(..., description="Parameter index", ge=0)
    value: float = Field(..., description="New native-range value")


class BulkSetParamsInput(BaseModel):
    model_config = ConfigDict(extra="forbid")
    changes: list[ParamChange] = Field(..., min_length=1,
        description="Parameter changes applied as one undoable operation")


@mcp.tool(
    name="vcv_set_parameters",
    annotations={"title": "Set Multiple Parameters", "readOnlyHint": False, "destructiveHint": False}
)
async def vcv_set_parameters(params: BulkSetParamsInput) -> str:
    """Set several parameters in one operation and one undo step.

    Read the affected modules first. The result reports any failed change indices;
    always inspect that field before claiming the entire edit succeeded.
    """
    try:
        changes = [{"moduleId": c.module_id, "paramId": c.param_id, "value": c.value}
                   for c in params.changes]
        return json.dumps(await _call("params/bulk", "POST", {"changes": changes}), indent=2)
    except Exception as e:
        return _err(e)


@mcp.tool(
    name="vcv_connect_cables",
    annotations={"title": "Connect Multiple Cables", "readOnlyHint": False, "destructiveHint": False}
)
async def vcv_connect_cables(params: BulkConnectInput) -> str:
    """Connect several cables in order, resolving optional port names.

    Each successful cable is a separate undo step. Processing stops at the first
    error and returns the successful count so partial application is explicit.
    """
    applied = []
    try:
        for index, c in enumerate(params.connections):
            out_port = await _resolve_port(c.output_module_id, "output",
                                           c.output_port_id, c.output_port_name)
            in_port = await _resolve_port(c.input_module_id, "input",
                                          c.input_port_id, c.input_port_name)
            result = await _call("cables", "POST", {
                "outputModuleId": c.output_module_id, "outputPortId": out_port,
                "inputModuleId": c.input_module_id, "inputPortId": in_port,
            })
            if "error" in result:
                return json.dumps({"ok": False, "applied": len(applied),
                                   "failedIndex": index, "error": result["error"]}, indent=2)
            applied.append(index)
        return json.dumps({"ok": True, "applied": len(applied)}, indent=2)
    except Exception as e:
        return json.dumps({"ok": False, "applied": len(applied), "error": _error_message(e)}, indent=2)


@mcp.tool(
    name="vcv_get_undo_status",
    annotations={"title": "Get Undo Status", "readOnlyHint": True, "destructiveHint": False}
)
async def vcv_get_undo_status() -> str:
    """List Octavia write operations that can currently be undone, newest first."""
    try:
        return json.dumps(await _call("undo/status"), indent=2)
    except Exception as e:
        return _err(e)


@mcp.tool(
    name="vcv_undo",
    annotations={"title": "Undo Last Change", "readOnlyHint": False, "destructiveHint": False}
)
async def vcv_undo() -> str:
    """Undo the most recent reversible Octavia write operation."""
    try:
        return json.dumps(await _call("undo", "POST"), indent=2)
    except Exception as e:
        return _err(e)


@mcp.tool(
    name="vcv_save_patch",
    annotations={"title": "Save Patch", "readOnlyHint": False, "destructiveHint": True}
)
async def vcv_save_patch() -> str:
    """Save the current patch to its existing file path.

    Check vcv_get_patch_info first. This overwrites the saved patch file and cannot
    be reverted through vcv_undo, so obtain explicit user approval before calling.
    """
    try:
        return json.dumps(await _call("patch/save", "POST"), indent=2)
    except Exception as e:
        return _err(e)


@mcp.tool(
    name="vcv_get_signal_levels",
    annotations={"title": "Get Signal Levels", "readOnlyHint": True, "destructiveHint": False}
)
async def vcv_get_signal_levels() -> str:
    """Get a compact voltage and peak snapshot for all module outputs.

    Much lighter than calling vcv_get_module for each module. Use this to quickly
    identify which parts of the signal chain are active, where signal is clipping
    (peak > 10V), or where signal is unexpectedly silent (voltage ≈ 0, peak ≈ 0).

    Returns:
        str: JSON array per module: id, model, outputs[{id, name, ch, v, peak, connected}]
             ch = polyphony channel count, v = current voltage (ch 0), peak = peak voltage seen
    """
    try:
        return json.dumps(await _call("modules/voltages"), indent=2)
    except Exception as e:
        return _err(e)


@mcp.tool(
    name="vcv_find_unpatched",
    annotations={"title": "Find Unpatched Ports", "readOnlyHint": True, "destructiveHint": False}
)
async def vcv_find_unpatched() -> str:
    """Find modules with unconnected ports, with port names and smart source filtering.

    Uses the full module list (port names included) and smart filtering:
    - Pure sources (no audio inputs, e.g. VCO, LFO, Noise): only flagged if ALL their
      outputs are unconnected — having unused waveform outputs is normal and expected.
    - Processors and utilities: all unconnected inputs AND outputs are flagged.

    Results include port names (not just indices) so you can act immediately without
    a follow-up vcv_get_module call.

    Returns:
        str: JSON array of modules with unpatched ports, each with:
             id, plugin, model,
             unconnectedInputs  [{id, name}]  — inputs with no cable,
             unconnectedOutputs [{id, name}]  — outputs with no cable
    """
    try:
        modules_full = await _call("modules")   # full detail — has port names
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
            is_pure_source = len(inputs) == 0  # generators: VCO, LFO, Noise, etc.

            uncon_in  = [{"id": p["id"], "name": p["name"]}
                         for p in inputs  if (mid, p["id"]) not in connected_in]
            uncon_out = [{"id": p["id"], "name": p["name"]}
                         for p in outputs if (mid, p["id"]) not in connected_out]

            # Smart filter: pure sources are OK with some unused outputs
            # Only flag if EVERY output is unconnected (truly orphaned module)
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

if __name__ == "__main__":
    mcp.run()
