import json
import time


REQUIRED_TOP_LEVEL_FIELDS = ("plugin", "module", "instance", "stream", "kind", "data")


def normalize_event(raw_line):
    line = raw_line.strip()
    if not line:
        return None

    parsed = json.loads(line)
    if not isinstance(parsed, dict):
        raise ValueError("event must be a JSON object")

    for field in REQUIRED_TOP_LEVEL_FIELDS:
        if field not in parsed:
            raise ValueError("missing required field: %s" % field)

    data = parsed.get("data")
    if not isinstance(data, dict):
        raise ValueError("data must be an object")

    event = {
        "plugin": str(parsed["plugin"]),
        "module": str(parsed["module"]),
        "instance": str(parsed["instance"]),
        "stream": str(parsed["stream"]),
        "kind": str(parsed["kind"]),
        "ts": float(parsed.get("ts", time.time())),
        "data": data,
    }
    return event
