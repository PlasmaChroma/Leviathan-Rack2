import threading
import time


class DebugState(object):
    def __init__(self):
        self._lock = threading.Lock()
        self._rows = {}
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
        row_key = (event["plugin"], event["module"], event["instance"], event["stream"])
        row = {
            "plugin": event["plugin"],
            "module": event["module"],
            "instance": event["instance"],
            "stream": event["stream"],
            "kind": event["kind"],
            "ts": event["ts"],
            "last_seen_sec": now,
            "data": dict(event["data"]),
        }
        with self._lock:
            self._rows[row_key] = row
            self._events_total += 1

    def snapshot(self):
        now = time.time()
        with self._lock:
            rows = []
            for row in self._rows.values():
                snap = dict(row)
                snap["data"] = dict(row["data"])
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
                "uptime_sec": uptime_sec,
                "events_per_sec": self._events_total / uptime_sec,
            }
