from collections import deque
import threading
import time


class DebugState(object):
    def __init__(self):
        self._lock = threading.Lock()
        self._rows = {}
        self._schemas = {}
        self._row_stale_sec = 3.0
        self._client_count = 0
        self._events_total = 0
        self._parse_errors = 0
        self._start_time = time.time()

    def set_client_count(self, count):
        with self._lock:
            self._client_count = max(0, int(count))

    def record_parse_error(self):
        with self._lock:
            self._parse_errors += 1

    def ingest_event(self, event):
        now = time.time()
        schema_key = (event["plugin"], event["module"], event["stream"])
        if event["kind"] == "schema":
            columns = self._normalize_columns(event["data"].get("columns"))
            if not columns:
                return
            schema = {
                "plugin": event["plugin"],
                "module": event["module"],
                "stream": event["stream"],
                "target_kind": str(event["data"].get("target_kind", "metric")),
                "columns": columns,
                "last_seen_sec": now,
                "ts": event["ts"],
            }
            with self._lock:
                self._schemas[schema_key] = schema
                self._events_total += 1
            return

        row_key = (event["plugin"], event["module"], event["instance"], event["stream"])
        data = dict(event["data"])
        row = {
            "plugin": event["plugin"],
            "module": event["module"],
            "instance": event["instance"],
            "stream": event["stream"],
            "kind": event["kind"],
            "ts": event["ts"],
            "last_seen_sec": now,
            "data": data,
        }
        with self._lock:
            schema = self._schemas.get(schema_key)
            if schema is None or schema.get("target_kind") != event["kind"]:
                self._events_total += 1
                return
            prev_row = self._rows.get(row_key)
            cutoff_sec = now - 1.0
            vw_age_value = data.get("vw_age_ms")
            vw_queue_value = data.get("vw_queue_ms")
            vw_age_history = prev_row.get("vw_age_history") if prev_row else None
            if vw_age_history is None:
                vw_age_history = deque()
            if isinstance(vw_age_value, (int, float)):
                vw_age_history.append((now, float(vw_age_value)))
            vw_queue_history = prev_row.get("vw_queue_history") if prev_row else None
            if vw_queue_history is None:
                vw_queue_history = deque()
            if isinstance(vw_queue_value, (int, float)):
                vw_queue_history.append((now, float(vw_queue_value)))
            while vw_age_history and vw_age_history[0][0] < cutoff_sec:
                vw_age_history.popleft()
            while vw_queue_history and vw_queue_history[0][0] < cutoff_sec:
                vw_queue_history.popleft()
            row["vw_age_history"] = vw_age_history
            row["vw_queue_history"] = vw_queue_history
            row["columns"] = list(schema["columns"])
            self._rows[row_key] = row
            self._events_total += 1

    @staticmethod
    def _normalize_columns(raw_columns):
        if not isinstance(raw_columns, list):
            return []
        columns = []
        seen = set()
        for item in raw_columns:
            if not isinstance(item, dict):
                continue
            key = str(item.get("key", "")).strip()
            label = str(item.get("label", key)).strip()
            if not key or key in seen:
                continue
            seen.add(key)
            columns.append({"key": key, "label": label or key})
        return columns

    def snapshot(self):
        now = time.time()
        with self._lock:
            stale_cutoff = now - self._row_stale_sec
            stale_keys = [key for key, row in self._rows.items() if row["last_seen_sec"] < stale_cutoff]
            for key in stale_keys:
                self._rows.pop(key, None)

            rows = []
            for row in self._rows.values():
                schema = self._schemas.get((row["plugin"], row["module"], row["stream"]))
                if schema is None:
                    continue
                snap = dict(row)
                snap["data"] = dict(row["data"])
                snap["columns"] = list(schema["columns"])
                vw_age_history = row.get("vw_age_history")
                if vw_age_history:
                    cutoff_sec = now - 1.0
                    while vw_age_history and vw_age_history[0][0] < cutoff_sec:
                        vw_age_history.popleft()
                    if vw_age_history:
                        vw_age_values = [value for _, value in vw_age_history]
                        snap["data"]["vw_age_ms"] = sum(vw_age_values) / float(len(vw_age_values))
                vw_queue_history = row.get("vw_queue_history")
                if vw_queue_history:
                    cutoff_sec = now - 1.0
                    while vw_queue_history and vw_queue_history[0][0] < cutoff_sec:
                        vw_queue_history.popleft()
                    if vw_queue_history:
                        vw_queue_values = [value for _, value in vw_queue_history]
                        snap["data"]["vw_queue_ms"] = sum(vw_queue_values) / float(len(vw_queue_values))
                # Age in the terminal should reflect how stale the latest row is
                # from the server's point of view, not the producer's timestamp
                # domain, which may be relative uptime instead of wall clock.
                snap["age_sec"] = max(0.0, now - row["last_seen_sec"])
                snap["last_seen_age_sec"] = max(0.0, now - row["last_seen_sec"])
                rows.append(snap)
            rows.sort(key=lambda item: (item["module"], item["instance"], item["stream"]))
            uptime_sec = max(0.001, now - self._start_time)
            return {
                "rows": rows,
                "client_count": self._client_count,
                "events_total": self._events_total,
                "parse_errors": self._parse_errors,
                "schemas": len(self._schemas),
                "uptime_sec": uptime_sec,
                "events_per_sec": self._events_total / uptime_sec,
            }
