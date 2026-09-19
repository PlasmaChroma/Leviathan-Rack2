import ast
import re
from pathlib import Path

MCP_FILE = Path("MCP/mcp_server/Octavia_MCP.py")
content = MCP_FILE.read_text(encoding="utf-8")

# 1. Add time to imports
if "import time" not in content:
    content = content.replace("import json\nimport os\n", "import json\nimport os\nimport time\n")

# 2. Insert module caching helpers and _dump_json before _normalize_endpoint
cache_and_dump_helpers = '''# ── Module Inventory Caching ──────────────────────────────────────────────────

_MODULE_CACHE: dict[str, Any] = {"timestamp": 0.0, "modules": []}
_RESOLVED_MODULE_IDS: dict[int, int] = {}
MODULE_CACHE_TTL_SEC = 10.0


def _invalidate_module_cache() -> None:
    global _MODULE_CACHE, _RESOLVED_MODULE_IDS
    _MODULE_CACHE = {"timestamp": 0.0, "modules": []}
    _RESOLVED_MODULE_IDS = {}


async def _get_cached_modules(force_refresh: bool = False) -> list[dict]:
    now = time.monotonic()
    if not force_refresh and _MODULE_CACHE["modules"] and (now - _MODULE_CACHE["timestamp"]) < MODULE_CACHE_TTL_SEC:
        return _MODULE_CACHE["modules"]
    try:
        async with httpx.AsyncClient(timeout=3.0) as client:
            r = await client.get(f"{BRIDGE_URL}/modules/summary", headers=BRIDGE_HEADERS)
            if r.status_code == 200:
                mods = r.json()
                if isinstance(mods, list):
                    _MODULE_CACHE["timestamp"] = now
                    _MODULE_CACHE["modules"] = mods
                    return mods
    except Exception:
        pass
    return _MODULE_CACHE.get("modules", [])


def _dump_json(payload: Any, compact: bool = True) -> str:
    """Format model-visible responses with compact delimiters to avoid whitespace token bloat."""
    if compact and os.environ.get("OCTAVIA_COMPACT_JSON", "1") not in ("0", "false", "False"):
        return json.dumps(payload, separators=(",", ":"))
    return json.dumps(payload, indent=2)


'''

old_normalize = '''async def _normalize_endpoint(endpoint: str) -> str:
    """Resolve module_id in endpoints to protect against client JSON float precision truncation."""
    m = re.match(r"^((?:sibyl|semantic|console|modules|temporal-deck|debug/metrics|debug/capture)/)(\d+)(/.*|\?.*)?$", endpoint)
    if not m:
        return endpoint
    prefix, mid_str, suffix = m.group(1), m.group(2), m.group(3) or ""
    try:
        req_id = int(mid_str)
        async with httpx.AsyncClient(timeout=3.0) as client:
            r = await client.get(f"{BRIDGE_URL}/modules", headers=BRIDGE_HEADERS)
            if r.status_code == 200:
                for mod in r.json():
                    actual_id = mod.get("id")
                    if actual_id is not None and (actual_id == req_id or abs(int(actual_id) - req_id) <= 64):
                        return f"{prefix}{actual_id}{suffix}"
    except Exception:
        pass
    return endpoint'''

new_normalize = '''async def _normalize_endpoint(endpoint: str) -> str:
    """Resolve module_id in endpoints to protect against client JSON float precision truncation."""
    m = re.match(r"^((?:sibyl|semantic|console|modules|temporal-deck|debug/metrics|debug/capture)/)(\d+)(/.*|\?.*)?$", endpoint)
    if not m:
        return endpoint
    prefix, mid_str, suffix = m.group(1), m.group(2), m.group(3) or ""
    try:
        req_id = int(mid_str)
        if req_id in _RESOLVED_MODULE_IDS:
            return f"{prefix}{_RESOLVED_MODULE_IDS[req_id]}{suffix}"
        mods = await _get_cached_modules()
        for mod in mods:
            actual_id = mod.get("id")
            if actual_id is not None and (actual_id == req_id or abs(int(actual_id) - req_id) <= 64):
                _RESOLVED_MODULE_IDS[req_id] = actual_id
                return f"{prefix}{actual_id}{suffix}"
    except Exception:
        pass
    return endpoint'''

assert old_normalize in content, "old_normalize not found"
content = content.replace(old_normalize, cache_and_dump_helpers + new_normalize)

# 3. Invalidate module cache on add/delete module
old_add = '''async def vcv_add_module(params: AddModuleInput) -> str:
    """Add a module to the current patch. Search with vcv_list_library first for exact slugs."""
    try:
        return json.dumps(await _call("modules", "POST", {
            "plugin": params.plugin, "model": params.model
        }), indent=2)'''

new_add = '''async def vcv_add_module(params: AddModuleInput) -> str:
    """Add a module to the current patch. Search with vcv_list_library first for exact slugs."""
    try:
        _invalidate_module_cache()
        return _dump_json(await _call("modules", "POST", {
            "plugin": params.plugin, "model": params.model
        }))'''

assert old_add in content, "old_add not found"
content = content.replace(old_add, new_add)

old_delete = '''async def vcv_delete_module(params: DeleteModuleInput) -> str:
    """Permanently delete a module and its cables. Obtain explicit user confirmation first."""
    try:
        return json.dumps(await _call(f"modules/{params.module_id}", "DELETE"), indent=2)'''

new_delete = '''async def vcv_delete_module(params: DeleteModuleInput) -> str:
    """Permanently delete a module and its cables. Obtain explicit user confirmation first."""
    try:
        _invalidate_module_cache()
        return _dump_json(await _call(f"modules/{params.module_id}", "DELETE"))'''

assert old_delete in content, "old_delete not found"
content = content.replace(old_delete, new_delete)

# 4. Update SibylCapabilitiesInput
old_caps_def = '''@mcp.tool(name="vcv_sibyl_get_capabilities",
          annotations={"title": "Get Sibyl Capabilities", "readOnlyHint": True, "destructiveHint": False})
async def vcv_sibyl_get_capabilities(params: SibylModuleInput) -> str:
    """Discover the Sibyl API/schema versions and semantic operations supported by a module."""
    try:
        return json.dumps(await _sibyl_call(f"sibyl/{params.module_id}/capabilities"), indent=2)'''

new_caps_def = '''class SibylCapabilitiesInput(SibylModuleInput):
    compact: bool = Field(False, description="Return compact feature manifest (<300B) instead of full contract schema")


@mcp.tool(name="vcv_sibyl_get_capabilities",
          annotations={"title": "Get Sibyl Capabilities", "readOnlyHint": True, "destructiveHint": False})
async def vcv_sibyl_get_capabilities(params: SibylCapabilitiesInput) -> str:
    """Discover the Sibyl API/schema versions and semantic operations supported by a module."""
    try:
        caps = await _sibyl_call(f"sibyl/{params.module_id}/capabilities")
        if getattr(params, "compact", False) and isinstance(caps, dict) and "capabilities" in caps:
            sib = caps["capabilities"].get("sibyl", {})
            manifest = {
                "ok": True,
                "apiVersion": sib.get("apiVersion"),
                "schemaVersion": sib.get("schemaVersion"),
                "revision": sib.get("revision"),
                "pitchStage": sib.get("pitchSystems", {}).get("stage"),
                "views": sib.get("views", []),
                "operations": sib.get("operations", []),
                "features": {
                    "harmony": sib.get("harmony", {}).get("version"),
                    "automation": sib.get("automation", {}).get("version"),
                    "voicing": sib.get("voicing", {}).get("version"),
                    "conditions": sib.get("conditions", {}).get("version"),
                    "repeatEvolution": sib.get("repeatEvolution", {}).get("version")
                }
            }
            return _dump_json(manifest)
        return _dump_json(caps)'''

assert old_caps_def in content, "old_caps_def not found"
content = content.replace(old_caps_def, new_caps_def)

# 5. Update SibylEditInput and vcv_sibyl_edit for response_profile
old_edit_input = '''class SibylEditInput(SibylModuleInput):
    expected_revision: int = Field(..., description="Last accepted revision read from Sibyl", ge=0)
    operations: list[dict] = Field(..., description="Atomic semantic edit operations", min_length=1, max_length=256)
    apply_at: Optional[Literal["immediate", "nextStep", "nextBeat", "nextScene"]] = Field(
        None, description="Optional adoption boundary; otherwise use the composition default"
    )
    phase_policy: Literal["preserve", "restartChanged", "restartAll"] = Field(
        "preserve", description="How pattern phases respond when the revision becomes active"
    )'''

new_edit_input = '''class SibylEditInput(SibylModuleInput):
    expected_revision: int = Field(..., description="Last accepted revision read from Sibyl", ge=0)
    operations: list[dict] = Field(..., description="Atomic semantic edit operations", min_length=1, max_length=256)
    apply_at: Optional[Literal["immediate", "nextStep", "nextBeat", "nextScene"]] = Field(
        None, description="Optional adoption boundary; otherwise use the composition default"
    )
    phase_policy: Literal["preserve", "restartChanged", "restartAll"] = Field(
        "preserve", description="How pattern phases respond when the revision becomes active"
    )
    response_profile: Optional[Literal["receipt", "summary", "full"]] = Field(
        "full", description="Response profile: receipt returns compact confirmation (<150B); full returns complete change details"
    )'''

assert old_edit_input in content, "old_edit_input not found"
content = content.replace(old_edit_input, new_edit_input)

old_edit_tool = '''@mcp.tool(name="vcv_sibyl_edit",
          annotations={"title": "Edit Sibyl Composition", "readOnlyHint": False, "destructiveHint": False})
async def vcv_sibyl_edit(params: SibylEditInput) -> str:
    """Apply semantic operations atomically. A successful transaction creates one vcv_undo entry."""
    try:
        payload = {"expected_revision": params.expected_revision,
                   "phase_policy": params.phase_policy,
                   "operations": params.operations}
        if params.apply_at is not None:
            payload["apply_at"] = params.apply_at
        return json.dumps(await _sibyl_call(f"sibyl/{params.module_id}/edit", "POST", payload), indent=2)'''

new_edit_tool = '''@mcp.tool(name="vcv_sibyl_edit",
          annotations={"title": "Edit Sibyl Composition", "readOnlyHint": False, "destructiveHint": False})
async def vcv_sibyl_edit(params: SibylEditInput) -> str:
    """Apply semantic operations atomically. A successful transaction creates one vcv_undo entry."""
    try:
        payload = {"expected_revision": params.expected_revision,
                   "phase_policy": params.phase_policy,
                   "operations": params.operations}
        if params.apply_at is not None:
            payload["apply_at"] = params.apply_at
        res = await _sibyl_call(f"sibyl/{params.module_id}/edit", "POST", payload)
        if params.response_profile == "receipt" and isinstance(res, dict) and res.get("ok"):
            receipt = {
                "ok": True,
                "revision": res.get("revision"),
                "activeRevision": res.get("activeRevision"),
                "appliedOperations": len(params.operations),
                "warnings": res.get("warnings", [])
            }
            if "changes" in res and isinstance(res["changes"], dict):
                receipt["changes"] = {k: v for k, v in res["changes"].items() if v}
            return _dump_json(receipt)
        return _dump_json(res)'''

assert old_edit_tool in content, "old_edit_tool not found"
content = content.replace(old_edit_tool, new_edit_tool)

# 6. Replace remaining json.dumps(..., indent=2) with _dump_json(...)
# Use regex substitution
pattern = re.compile(r'json\.dumps\((.*?),\s*indent=2\)', re.DOTALL)
content, count = pattern.subn(r'_dump_json(\1)', content)
print(f"Replaced {count} json.dumps(..., indent=2) calls with _dump_json(...)")

# Validate AST
ast.parse(content)
print("AST parsed successfully!")

# Write back
MCP_FILE.write_text(content, encoding="utf-8")
print(f"Successfully updated {MCP_FILE}")
