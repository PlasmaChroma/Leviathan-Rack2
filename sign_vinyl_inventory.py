#!/usr/bin/env python3
import argparse
import json
import os
import sys
import time
from typing import Dict, List, Tuple


FNV64_OFFSET_BASIS = 1469598103934665603
FNV64_PRIME = 1099511628211
ALGORITHM = "fnv1a64-keyed-v1"
DOMAIN = "TemporalDeckVinylInventorySigned-v1"
INTERNAL_SIGNING_SECRET = "TemporalDeckLocalSigningKey"


def fnv1a64_update(h: int, data: bytes) -> int:
    for b in data:
        h ^= b
        h = (h * FNV64_PRIME) & 0xFFFFFFFFFFFFFFFF
    return h


def fnv1a64_string(h: int, text: str) -> int:
    return fnv1a64_update(h, text.encode("utf-8"))


def hex_u64(value: int) -> str:
    return f"{value & 0xFFFFFFFFFFFFFFFF:016x}"


def is_safe_filename(name: str) -> bool:
    return bool(name) and ".." not in name and "/" not in name and "\\" not in name


def normalize_base_path(path: str) -> str:
    path = path.strip().replace("\\", "/")
    while path.endswith("/"):
        path = path[:-1]
    return path


def is_safe_base_path(base_path: str) -> bool:
    if not base_path:
        return False
    if ".." in base_path:
        return False
    if base_path[0] in ("/", "\\"):
        return False
    return True


def hash_file_fnv64(path: str) -> Tuple[int, int]:
    h = FNV64_OFFSET_BASIS
    size = 0
    with open(path, "rb") as f:
        while True:
            chunk = f.read(32768)
            if not chunk:
                break
            size += len(chunk)
            h = fnv1a64_update(h, chunk)
    return h, size


def parse_inventory(path: str) -> Dict:
    with open(path, "r", encoding="utf-8") as f:
        return json.load(f)


def infer_source_root(inventory_path: str, base_path: str) -> str:
    inventory_path = os.path.abspath(inventory_path)
    inv_dir = os.path.dirname(inventory_path)
    base_path = normalize_base_path(base_path)
    if base_path in ("", "."):
        return inv_dir

    base_parts = [p for p in base_path.split("/") if p and p != "."]
    if not base_parts:
        return inv_dir

    inv_dir_norm = os.path.normpath(inv_dir)
    inv_parts = inv_dir_norm.split(os.sep)
    if len(inv_parts) >= len(base_parts):
        tail = inv_parts[-len(base_parts) :]
        if [p.lower() for p in tail] == [p.lower() for p in base_parts]:
            root = inv_dir_norm
            for _ in base_parts:
                root = os.path.dirname(root)
            return root if root else os.path.abspath(os.sep)
    return os.getcwd()


def collect_records(inventory: Dict) -> Tuple[str, List[Dict]]:
    base_path = normalize_base_path(str(inventory.get("basePath", "") or ""))
    if not is_safe_base_path(base_path):
        raise ValueError("inventory has invalid basePath")

    vinyl = inventory.get("vinyl")
    if not isinstance(vinyl, list) or not vinyl:
        raise ValueError("inventory must contain a non-empty vinyl[] array")

    seen_ids = set()
    seen_visible_menu_ids = set()
    records: List[Dict] = []
    for i, item in enumerate(vinyl):
        if not isinstance(item, dict):
            raise ValueError(f"vinyl[{i}] must be an object")
        raw_id = item.get("id")
        raw_file = item.get("file")
        if not isinstance(raw_id, str) or not isinstance(raw_file, str):
            raise ValueError(f"vinyl[{i}] requires string id and file")
        rec_id = raw_id.strip()
        rec_file = raw_file.strip()
        if not rec_id or not is_safe_filename(rec_file):
            raise ValueError(f"vinyl[{i}] has invalid id/file")
        if rec_id in seen_ids:
            raise ValueError(f"duplicate vinyl id: {rec_id}")
        seen_ids.add(rec_id)

        label = item.get("label", "")
        if not isinstance(label, str):
            label = str(label)
        label = label.strip()

        menu_visible = bool(item.get("menuVisible", True))
        menu_id = -1
        if menu_visible:
            if "menuId" not in item or not isinstance(item.get("menuId"), int):
                raise ValueError(f"vinyl[{i}] requires integer menuId when menuVisible=true")
            menu_id = int(item["menuId"])
            if menu_id < 0:
                raise ValueError(f"vinyl[{i}] has invalid menuId")
            if menu_id in seen_visible_menu_ids:
                raise ValueError(f"duplicate visible menuId: {menu_id}")
            seen_visible_menu_ids.add(menu_id)
        elif isinstance(item.get("menuId"), int):
            menu_id = int(item["menuId"])

        records.append(
            {
                "id": rec_id,
                "label": label,
                "file": rec_file,
                "menuVisible": menu_visible,
                "menuId": menu_id,
            }
        )
    return base_path, records


def sign_inventory(inventory: Dict, source_root: str, secret: str, generated_unix: int) -> Tuple[Dict, str, int]:
    base_path, records = collect_records(inventory)

    files_signed = []
    for rec in records:
        abs_path = os.path.join(source_root, base_path, rec["file"])
        if not os.path.isfile(abs_path):
            raise ValueError(f"missing vinyl file: {abs_path}")
        content_hash, size_bytes = hash_file_fnv64(abs_path)
        rec["hash"] = content_hash
        rec["size"] = size_bytes
        files_signed.append(
            {
                "id": rec["id"],
                "file": rec["file"],
                "menuId": rec["menuId"],
                "size": size_bytes,
                "hash": hex_u64(content_hash),
            }
        )

    key_id = fnv1a64_string(FNV64_OFFSET_BASIS, secret)
    sig = fnv1a64_string(FNV64_OFFSET_BASIS, DOMAIN)
    sig = fnv1a64_update(sig, b"\x00")
    sig = fnv1a64_string(sig, secret)
    sig = fnv1a64_update(sig, b"\x00")
    sig = fnv1a64_string(sig, base_path)
    sig = fnv1a64_update(sig, b"\x00")
    for rec in records:
        sig = fnv1a64_string(sig, rec["id"])
        sig = fnv1a64_update(sig, b"\x00")
        sig = fnv1a64_string(sig, rec["label"])
        sig = fnv1a64_update(sig, b"\x00")
        sig = fnv1a64_string(sig, rec["file"])
        sig = fnv1a64_update(sig, b"\x00")
        sig = fnv1a64_string(sig, "1" if rec["menuVisible"] else "0")
        sig = fnv1a64_update(sig, b"\x00")
        sig = fnv1a64_string(sig, str(rec["menuId"]))
        sig = fnv1a64_update(sig, b"\x00")
        sig = fnv1a64_string(sig, str(rec["size"]))
        sig = fnv1a64_update(sig, b"\x00")
        sig = fnv1a64_string(sig, hex_u64(rec["hash"]))
        sig = fnv1a64_update(sig, b"\x00")

    out = dict(inventory)
    out["signed"] = {
        "algorithm": ALGORITHM,
        "generatedUnix": int(generated_unix),
        "keyId": hex_u64(key_id),
        "signature": hex_u64(sig),
        "files": files_signed,
    }
    return out, hex_u64(sig), len(records)


def main() -> int:
    parser = argparse.ArgumentParser(description="Sign a TemporalDeck vinyl inventory JSON file.")
    parser.add_argument("inventory", help="Path to inventory JSON (e.g. res/Vinyl/inventory.json)")
    parser.add_argument("--root", default="", help="Source root used to resolve basePath/file (default: auto-detect)")
    parser.add_argument("--output", default="", help="Output path (default: in-place)")
    parser.add_argument("--generated-unix", type=int, default=0, help="Override generatedUnix timestamp")
    args = parser.parse_args()

    try:
        inventory_path = os.path.abspath(args.inventory)
        secret = INTERNAL_SIGNING_SECRET

        inventory = parse_inventory(inventory_path)
        if args.root:
            source_root = os.path.abspath(args.root)
        else:
            base_path = normalize_base_path(str(inventory.get("basePath", "") or ""))
            source_root = infer_source_root(inventory_path, base_path)
        generated_unix = args.generated_unix if args.generated_unix > 0 else int(round(time.time()))
        signed_inventory, signature, file_count = sign_inventory(inventory, source_root, secret, generated_unix)

        out_path = os.path.abspath(args.output) if args.output else inventory_path
        with open(out_path, "w", encoding="utf-8") as f:
            json.dump(signed_inventory, f, indent=2)
            f.write("\n")

        print(f"Signed {file_count} file(s).")
        print(f"Signature: {signature}")
        print(f"Wrote: {out_path}")
        return 0
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
