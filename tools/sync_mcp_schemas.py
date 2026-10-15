#!/usr/bin/env python3
"""
tools/sync_mcp_schemas.py
=========================
Reproducible schema export and publication tool for Octavia MCP.
Uses the public async mcp.list_tools() interface.

Supports staging under repository build directories, non-writing drift checks,
deterministic JSON serialization, atomic file updates, and ownership manifests.
"""

from __future__ import annotations

import argparse
import asyncio
import copy
import hashlib
import json
import os
import sys
import tempfile
from pathlib import Path
from typing import Any, Dict, List, Optional, Tuple

_repo_root = Path(__file__).resolve().parents[1]
if str(_repo_root / "MCP") not in sys.path:
    sys.path.insert(0, str(_repo_root / "MCP"))

from mcp_server import Octavia_MCP as server

EXPORTER_VERSION = "1.0"
MANIFEST_FILENAME = "manifest.json"


def compute_sha256(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def format_tool_schema(tool: Any, fmt: str = "antigravity") -> str:
    """Format an MCP Tool into deterministic JSON."""
    if fmt == "antigravity":
        obj = {
            "name": tool.name,
            "description": tool.description or "",
            "parameters": copy.deepcopy(tool.inputSchema),
        }
    elif fmt == "mcp":
        obj = {
            "name": tool.name,
            "description": tool.description or "",
            "inputSchema": copy.deepcopy(tool.inputSchema),
        }
        if getattr(tool, "annotations", None):
            obj["annotations"] = tool.annotations
    else:
        raise ValueError(f"Unknown schema format: {fmt!r}")

    # Deterministic compact JSON with sorted keys
    return json.dumps(obj, indent=2, sort_keys=True) + "\n"


async def export_tools(toolset: str, fmt: str) -> Dict[str, Tuple[str, bytes]]:
    """Export tool schemas without running server or contacting Rack."""
    server.configure_sibyl_toolset(toolset)
    tools = await server.mcp.list_tools()

    # Sort tools by name for determinism
    tools_sorted = sorted(tools, key=lambda t: t.name)
    results: Dict[str, Tuple[str, bytes]] = {}

    for tool in tools_sorted:
        filename = f"{tool.name}.json"
        content_str = format_tool_schema(tool, fmt)
        content_bytes = content_str.encode("utf-8")
        results[filename] = (content_str, content_bytes)

    return results


def read_manifest(directory: Path) -> Optional[Dict[str, Any]]:
    manifest_path = directory / MANIFEST_FILENAME
    if not manifest_path.is_file():
        return None
    try:
        return json.loads(manifest_path.read_text(encoding="utf-8"))
    except Exception:
        return None


def run_check(
    output_dir: Path,
    expected_files: Dict[str, Tuple[str, bytes]],
    toolset: str,
    fmt: str
) -> Tuple[bool, List[str]]:
    """Check for drift without modifying any files. Returns (has_drift, report_lines)."""
    lines: List[str] = []
    has_drift = False

    if not output_dir.exists():
        lines.append(f"Output directory does not exist: {output_dir}")
        return True, lines

    manifest = read_manifest(output_dir)
    if manifest is None:
        lines.append(f"No existing {MANIFEST_FILENAME} found in {output_dir}")
        has_drift = True
    else:
        if manifest.get("toolset") != toolset:
            lines.append(f"Manifest toolset mismatch: expected {toolset!r}, found {manifest.get('toolset')!r}")
            has_drift = True
        if manifest.get("format") != fmt:
            lines.append(f"Manifest format mismatch: expected {fmt!r}, found {manifest.get('format')!r}")
            has_drift = True

    # Check each expected tool file
    for filename, (_, expected_bytes) in expected_files.items():
        file_path = output_dir / filename
        if not file_path.is_file():
            lines.append(f"Missing file: {filename}")
            has_drift = True
        else:
            actual_bytes = file_path.read_bytes()
            if actual_bytes != expected_bytes:
                lines.append(f"Changed content: {filename} (hash mismatch)")
                has_drift = True

    # Check for stale or extra files
    managed_filenames = set(expected_files.keys())
    existing_files = {p.name for p in output_dir.glob("*.json") if p.name != MANIFEST_FILENAME}
    extra_files = existing_files - managed_filenames

    if extra_files:
        if manifest and "files" in manifest:
            prior_managed = set(manifest["files"].keys())
            stale_files = extra_files & prior_managed
            unmanaged_files = extra_files - prior_managed
            if stale_files:
                lines.append(f"Stale managed files (previously owned): {sorted(stale_files)}")
                has_drift = True
            if unmanaged_files:
                lines.append(f"Unmanaged extra files: {sorted(unmanaged_files)}")
        else:
            lines.append(f"Extra files in unmanaged directory: {sorted(extra_files)}")

    return has_drift, lines


def apply_export(
    output_dir: Path,
    expected_files: Dict[str, Tuple[str, bytes]],
    toolset: str,
    fmt: str,
    prune: bool = False
) -> List[str]:
    """Atomically write files and update manifest last."""
    output_dir.mkdir(parents=True, exist_ok=True)
    report: List[str] = []

    prior_manifest = read_manifest(output_dir)
    prior_managed = set(prior_manifest.get("files", {}).keys()) if prior_manifest else set()

    manifest_files_entry: Dict[str, Dict[str, Any]] = {}

    # Write each tool schema atomically
    for filename, (content_str, content_bytes) in expected_files.items():
        target_path = output_dir / filename
        content_hash = compute_sha256(content_bytes)

        manifest_files_entry[filename] = {
            "sha256": content_hash,
            "bytes": len(content_bytes),
        }

        # Check if already identical to avoid unnecessary disk writes
        if target_path.is_file() and target_path.read_bytes() == content_bytes:
            continue

        # Atomic replacement: write to temp file in same directory then replace
        fd, tmp_path_str = tempfile.mkstemp(dir=output_dir, prefix=f".{filename}.", suffix=".tmp")
        tmp_path = Path(tmp_path_str)
        try:
            with os.fdopen(fd, "wb") as f:
                f.write(content_bytes)
            os.replace(tmp_path, target_path)
            report.append(f"Wrote {filename}")
        except Exception:
            if tmp_path.exists():
                tmp_path.unlink()
            raise

    # Handle stale files
    existing_files = {p.name for p in output_dir.glob("*.json") if p.name != MANIFEST_FILENAME}
    managed_filenames = set(expected_files.keys())
    extra_files = existing_files - managed_filenames

    if extra_files:
        if prior_manifest and prior_managed:
            stale_files = extra_files & prior_managed
            unmanaged_files = extra_files - prior_managed
            if prune and stale_files:
                for sf in stale_files:
                    (output_dir / sf).unlink()
                    report.append(f"Pruned stale managed file: {sf}")
            elif stale_files:
                report.append(f"Notice: {len(stale_files)} stale files retained (use --prune to remove): {sorted(stale_files)}")
            if unmanaged_files:
                report.append(f"Notice: {len(unmanaged_files)} unmanaged files retained: {sorted(unmanaged_files)}")
        else:
            report.append(f"Notice: {len(extra_files)} unmanaged files retained in directory without manifest: {sorted(extra_files)}")

    # Write manifest last atomically
    manifest = {
        "exporterVersion": EXPORTER_VERSION,
        "toolset": toolset,
        "format": fmt,
        "toolCount": len(expected_files),
        "files": manifest_files_entry,
    }
    manifest_bytes = (json.dumps(manifest, indent=2, sort_keys=True) + "\n").encode("utf-8")
    manifest_path = output_dir / MANIFEST_FILENAME

    fd, tmp_path_str = tempfile.mkstemp(dir=output_dir, prefix=".manifest.", suffix=".tmp")
    tmp_path = Path(tmp_path_str)
    try:
        with os.fdopen(fd, "wb") as f:
            f.write(manifest_bytes)
        os.replace(tmp_path, manifest_path)
        report.append(f"Updated {MANIFEST_FILENAME} (tools: {len(expected_files)})")
    except Exception:
        if tmp_path.exists():
            tmp_path.unlink()
        raise

    return report


def main() -> int:
    parser = argparse.ArgumentParser(description="Reproducible schema export and publication tool for Octavia MCP.")
    parser.add_argument("--toolset", choices=["full", "compact"], required=True,
                        help="Explicit toolset mode: 'full' or 'compact'")
    parser.add_argument("--output", type=Path, required=True,
                        help="Target output directory (e.g. build/mcp-schemas/full)")
    parser.add_argument("--format", choices=["antigravity", "mcp"], default="antigravity",
                        help="Schema wrapper format: 'antigravity' (default) or 'mcp'")
    parser.add_argument("--check", action="store_true",
                        help="Non-writing drift detection; exit nonzero if missing or changed files exist")
    parser.add_argument("--prune", action="store_true",
                        help="Prune stale files that were previously owned by exporter manifest")

    args = parser.parse_args()

    expected_files = asyncio.run(export_tools(args.toolset, args.format))

    if args.check:
        has_drift, report_lines = run_check(args.output, expected_files, args.toolset, args.format)
        for line in report_lines:
            print(f"CHECK: {line}")
        if has_drift:
            print(f"FAILED: Drift detected in {args.output}")
            return 1
        print(f"OK: All {len(expected_files)} tool schemas in {args.output} match registered {args.toolset} toolset.")
        return 0

    reports = apply_export(args.output, expected_files, args.toolset, args.format, prune=args.prune)
    for r in reports:
        print(r)
    print(f"SUCCESS: Exported {len(expected_files)} {args.toolset} schemas to {args.output}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
