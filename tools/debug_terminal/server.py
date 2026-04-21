#!/usr/bin/env python3
import argparse
import socket
import threading

from model import DebugState
from protocol import normalize_event
from render import run_live_renderer


class DebugServer(object):
    def __init__(self, host, port, state):
        self.host = host
        self.port = port
        self.state = state
        self.stop_event = threading.Event()
        self._client_lock = threading.Lock()
        self._clients = set()
        self._threads = []

    def _track_client(self, conn):
        with self._client_lock:
            self._clients.add(conn)
            self.state.set_client_count(len(self._clients))

    def _untrack_client(self, conn):
        with self._client_lock:
            self._clients.discard(conn)
            self.state.set_client_count(len(self._clients))

    def _handle_client(self, conn, addr):
        self._track_client(conn)
        try:
            with conn:
                file_obj = conn.makefile("r", encoding="utf-8", newline="\n")
                for raw_line in file_obj:
                    if self.stop_event.is_set():
                        break
                    try:
                        event = normalize_event(raw_line)
                    except Exception:
                        self.state.record_parse_error()
                        continue
                    if event is not None:
                        self.state.ingest_event(event)
        finally:
            self._untrack_client(conn)

    def serve_forever(self):
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_sock:
            server_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            server_sock.bind((self.host, self.port))
            server_sock.listen()
            server_sock.settimeout(0.5)
            while not self.stop_event.is_set():
                try:
                    conn, addr = server_sock.accept()
                except socket.timeout:
                    continue
                thread = threading.Thread(target=self._handle_client, args=(conn, addr), daemon=True)
                thread.start()
                self._threads.append(thread)

    def shutdown(self):
        self.stop_event.set()
        with self._client_lock:
            clients = list(self._clients)
        for conn in clients:
            try:
                conn.close()
            except OSError:
                pass


def parse_args():
    parser = argparse.ArgumentParser(description="Live debug terminal for Rack telemetry")
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--port", type=int, default=8765)
    parser.add_argument("--refresh-hz", type=float, default=8.0)
    return parser.parse_args()


def main():
    args = parse_args()
    state = DebugState()
    server = DebugServer(args.host, args.port, state)

    server_thread = threading.Thread(target=server.serve_forever, daemon=True)
    server_thread.start()
    try:
        run_live_renderer(state, args.host, args.port, args.refresh_hz, server.stop_event)
    except KeyboardInterrupt:
        pass
    finally:
        server.shutdown()
        server_thread.join(timeout=1.0)


if __name__ == "__main__":
    main()
