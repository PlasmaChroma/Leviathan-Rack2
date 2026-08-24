#!/usr/bin/env python3
"""Background Octavia Console worker backed by Codex app-server.

This is an optional host adapter. Rack owns the durable-in-process mailbox and
claims; this process owns only Codex thread identity and transport lifecycle.
"""

import argparse
import asyncio
import contextlib
import json
import os
from pathlib import Path
import sys
import uuid

import httpx


class AppServer:
    def __init__(self, cwd: Path):
        self.cwd = cwd
        self.process = None
        self.reader_task = None
        self.next_id = 1
        self.pending = {}
        self.notifications = asyncio.Queue()

    async def start(self):
        self.process = await asyncio.create_subprocess_exec(
            "codex", "app-server", "--stdio",
            stdin=asyncio.subprocess.PIPE,
            stdout=asyncio.subprocess.PIPE,
            stderr=sys.stderr,
        )
        self.reader_task = asyncio.create_task(self._read_loop())
        await self.request("initialize", {
            "clientInfo": {"name": "octavia-console-worker", "version": "0.1"},
            "capabilities": {"experimentalApi": True},
        })
        await self.notify("initialized")

    async def _read_loop(self):
        while True:
            line = await self.process.stdout.readline()
            if not line:
                error = RuntimeError("Codex app-server closed its output")
                for future in self.pending.values():
                    if not future.done():
                        future.set_exception(error)
                return
            message = json.loads(line)
            request_id = message.get("id")
            if request_id is not None and request_id in self.pending:
                future = self.pending.pop(request_id)
                if "error" in message:
                    future.set_exception(RuntimeError(str(message["error"])))
                else:
                    future.set_result(message.get("result", {}))
            elif "method" in message:
                await self.notifications.put(message)

    async def _send(self, message):
        self.process.stdin.write((json.dumps(message) + "\n").encode())
        await self.process.stdin.drain()

    async def request(self, method, params):
        request_id = self.next_id
        self.next_id += 1
        future = asyncio.get_running_loop().create_future()
        self.pending[request_id] = future
        await self._send({"id": request_id, "method": method, "params": params})
        return await future

    async def notify(self, method, params=None):
        message = {"method": method}
        if params is not None:
            message["params"] = params
        await self._send(message)

    async def open_thread(self, saved_thread_id=None):
        if saved_thread_id:
            try:
                result = await self.request("thread/resume", {
                    "threadId": saved_thread_id,
                    "cwd": str(self.cwd),
                    "approvalPolicy": "on-request",
                    "approvalsReviewer": "auto_review",
                    "sandbox": "workspace-write",
                })
                return result["thread"]["id"]
            except Exception as exc:
                print(f"Octavia worker: could not resume {saved_thread_id}: {exc}", file=sys.stderr)
        result = await self.request("thread/start", {
            "cwd": str(self.cwd),
            "approvalPolicy": "on-request",
            "approvalsReviewer": "auto_review",
            "sandbox": "workspace-write",
            "ephemeral": False,
            "developerInstructions": (
                "You are the dedicated background worker for the in-Rack Octavia Console. "
                "Use the installed Octavia skill and vcv-rack MCP tools for every Console request. "
                "Ordinary reversible Rack edits explicitly requested by the user are authorized under "
                "the skill rules. Preserve normal safety rules for destructive actions and never claim "
                "they were authorized merely because a request arrived from Rack. Return a useful "
                "terminal answer; the host supervisor delivers it to the Console."
            ),
        })
        return result["thread"]["id"]

    async def run_turn(self, thread_id, module_id, prompt_id, text):
        envelope = (
            "Source: Octavia Console background worker\n"
            f"Console module ID: {module_id}\nPrompt ID: {prompt_id}\n\n"
            "Treat the following as the user's request from the VCV Rack Console. "
            "Use the Octavia skill and its normal permission rules.\n\n"
            f"User request:\n{text}"
        )
        result = await self.request("turn/start", {
            "threadId": thread_id,
            "input": [{"type": "text", "text": envelope}],
            "responsesapiClientMetadata": {
                "source": "octavia-console",
                "octavia_module_id": str(module_id),
                "octavia_prompt_id": str(prompt_id),
            },
        })
        turn_id = result["turn"]["id"]
        answer = ""
        while True:
            message = await self.notifications.get()
            params = message.get("params", {})
            if params.get("threadId") != thread_id or params.get("turnId", turn_id) != turn_id:
                continue
            if message["method"] == "item/completed":
                item = params.get("item", {})
                if item.get("type") == "agentMessage" and item.get("phase") == "final_answer":
                    answer = item.get("text", "")
            if message["method"] == "turn/completed":
                turn = params.get("turn", {})
                if turn.get("status") != "completed":
                    raise RuntimeError(str(turn.get("error") or f"turn ended as {turn.get('status')}"))
                if not answer:
                    for item in turn.get("items", []):
                        if item.get("type") == "agentMessage":
                            answer = item.get("text", "")
                if not answer:
                    raise RuntimeError("Codex turn completed without a final agent message")
                return answer

    async def close(self):
        if self.process and self.process.returncode is None:
            self.process.terminate()
            with contextlib.suppress(asyncio.TimeoutError):
                await asyncio.wait_for(self.process.wait(), 5)
        if self.reader_task:
            self.reader_task.cancel()


class OctaviaWorker:
    def __init__(self, args):
        self.args = args
        self.base_url = f"http://127.0.0.1:{args.port}"
        self.headers = {"X-Octavia-Token": args.token} if args.token else {}
        self.client = httpx.AsyncClient(headers=self.headers, timeout=10)
        self.worker_id = None
        self.app = AppServer(args.cwd)
        self.thread_id = None

    async def post(self, path, payload):
        response = await self.client.post(self.base_url + path, json=payload)
        response.raise_for_status()
        return response.json()

    async def resolve_module_id(self):
        if self.args.module_id is not None:
            return
        response = await self.client.get(self.base_url + "/console")
        response.raise_for_status()
        module_ids = response.json().get("moduleIds", [])
        if len(module_ids) != 1:
            raise RuntimeError(
                "Octavia worker auto-discovery requires exactly one Console; "
                f"found {len(module_ids)}"
            )
        self.args.module_id = int(module_ids[0])
        print(f"Octavia worker: discovered Console {self.args.module_id}", file=sys.stderr)
        if self.args.state is None:
            self.args.state = (Path.home() / ".config" / "leviathan" /
                               f"octavia-codex-worker-{self.args.module_id}.json")

    def load_thread_id(self):
        try:
            payload = json.loads(self.args.state.read_text(encoding="utf-8"))
            return payload.get("threadId")
        except (FileNotFoundError, json.JSONDecodeError, OSError):
            return None

    def save_thread_id(self):
        self.args.state.parent.mkdir(parents=True, exist_ok=True)
        temporary = self.args.state.with_suffix(self.args.state.suffix + ".tmp")
        temporary.write_text(json.dumps({"threadId": self.thread_id}) + "\n", encoding="utf-8")
        temporary.replace(self.args.state)

    async def renew_claim(self, prompt_id, claim_token):
        while True:
            await asyncio.sleep(20)
            await self.post(
                f"/console/{self.args.module_id}/prompts/{prompt_id}/renew",
                {"claimToken": claim_token},
            )

    async def handle_next(self):
        try:
            claimed = await self.post(
                f"/console/{self.args.module_id}/prompts/claim-next",
                {"workerId": self.worker_id},
            )
        except httpx.HTTPStatusError as exc:
            if exc.response.status_code == 409:
                return False
            raise
        prompt = claimed["prompt"]
        claim_token = claimed["claimToken"]
        renewer = asyncio.create_task(self.renew_claim(prompt["id"], claim_token))
        try:
            answer = await self.app.run_turn(
                self.thread_id, self.args.module_id, prompt["id"], prompt["text"]
            )
            await self.post(
                f"/console/{self.args.module_id}/prompts/{prompt['id']}/complete",
                {"claimToken": claim_token, "operationId": str(uuid.uuid4()),
                 "text": answer, "error": False},
            )
        except Exception as exc:
            message = f"Background worker failed: {exc}"
            with contextlib.suppress(Exception):
                await self.post(
                    f"/console/{self.args.module_id}/prompts/{prompt['id']}/complete",
                    {"claimToken": claim_token, "operationId": str(uuid.uuid4()),
                     "text": message, "error": True},
                )
            print(message, file=sys.stderr)
        finally:
            renewer.cancel()
        return True

    async def drain(self):
        while await self.handle_next():
            pass

    async def run(self):
        await self.resolve_module_id()
        if self.args.state is None:
            self.args.state = (Path.home() / ".config" / "leviathan" /
                               f"octavia-codex-worker-{self.args.module_id}.json")
        await self.app.start()
        self.thread_id = await self.app.open_thread(self.load_thread_id())
        self.save_thread_id()
        await self.register()
        await self.drain()
        event_url = self.base_url + f"/console/{self.args.module_id}/events"
        last_event_id = 0
        while True:
            try:
                async with self.client.stream(
                    "GET", event_url,
                    params={"workerId": self.worker_id, "after": last_event_id},
                    timeout=httpx.Timeout(10, read=None),
                ) as response:
                    response.raise_for_status()
                    async for line in response.aiter_lines():
                        if line.startswith("id: "):
                            last_event_id = int(line[4:])
                        elif line.startswith("event: ") and line[7:] in {
                            "prompt.available", "resync"
                        }:
                            await self.drain()
            except httpx.HTTPStatusError as exc:
                if exc.response.status_code != 410:
                    raise
                await self.register()
                await self.drain()
            except (httpx.TransportError, ValueError) as exc:
                print(f"Octavia worker: event stream reconnecting after {exc}", file=sys.stderr)
                await asyncio.sleep(2)

    async def register(self):
        registration = await self.post(
            f"/console/{self.args.module_id}/workers", {"name": "codex-app-server"}
        )
        self.worker_id = registration["workerId"]

    async def close(self):
        if self.worker_id:
            with contextlib.suppress(Exception):
                await self.post(
                    f"/console/{self.args.module_id}/workers/{self.worker_id}/unregister", {}
                )
        await self.client.aclose()
        await self.app.close()


def parse_args():
    parser = argparse.ArgumentParser(description="Run a Codex-backed Octavia Console worker")
    parser.add_argument("module_id", type=int, nargs="?",
                        help="Octavia Console module ID (auto-discovered when omitted)")
    parser.add_argument("--port", type=int, default=int(os.environ.get("OCTAVIA_PORT", "34570")))
    parser.add_argument("--token", default=os.environ.get("OCTAVIA_TOKEN", ""))
    parser.add_argument("--cwd", type=Path, default=Path.cwd())
    parser.add_argument("--state", type=Path)
    args = parser.parse_args()
    if args.state is None and args.module_id is not None:
        args.state = (Path.home() / ".config" / "leviathan" /
                      f"octavia-codex-worker-{args.module_id}.json")
    return args


async def async_main():
    worker = OctaviaWorker(parse_args())
    try:
        await worker.run()
    finally:
        await worker.close()


if __name__ == "__main__":
    asyncio.run(async_main())
