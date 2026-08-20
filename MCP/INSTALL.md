# Octavia MCP installation

Octavia connects VCV Rack to MCP-capable coding agents. The included Python server
translates MCP tool calls into requests to the Octavia module at `localhost:7777`.

This guide uses Codex as the primary client. The MCP server itself uses standard stdio
transport and can also be registered with other MCP clients.

## Requirements

- VCV Rack 2.x with the Leviathan plugin
- Python 3.10 or newer
- Codex CLI or another client that supports local stdio MCP servers

## 1. Start Octavia in VCV Rack

1. Install or build the Leviathan plugin and restart VCV Rack.
2. Add **Leviathan > Octavia** to the patch.
3. Press **Start**. The status orb turns green when the HTTP bridge is listening.

Octavia listens only on `127.0.0.1`. The default port is `7777`; set
`OCTAVIA_PORT` in VCV Rack's environment before launch to use another port.

## 2. Install the MCP server

Create an isolated Python environment and install the runtime dependencies.

### macOS and Linux

```sh
python3 -m venv ~/.octavia-mcp
~/.octavia-mcp/bin/python -m pip install -r requirements.txt
cp mcp_server/Octavia_MCP.py ~/.octavia-mcp/Octavia_MCP.py
```

### Windows PowerShell

```powershell
py -m venv "$env:USERPROFILE\.octavia-mcp"
& "$env:USERPROFILE\.octavia-mcp\Scripts\python.exe" -m pip install -r requirements.txt
Copy-Item mcp_server\Octavia_MCP.py "$env:USERPROFILE\.octavia-mcp\Octavia_MCP.py"
```

## 3. Register it with Codex

### macOS and Linux

```sh
codex mcp add vcv-rack -- ~/.octavia-mcp/bin/python ~/.octavia-mcp/Octavia_MCP.py
```

Use absolute paths if your shell does not expand `~` in command arguments.

### Windows PowerShell

```powershell
codex mcp add vcv-rack -- "$env:USERPROFILE\.octavia-mcp\Scripts\python.exe" "$env:USERPROFILE\.octavia-mcp\Octavia_MCP.py"
```

Confirm the registration with:

```sh
codex mcp get vcv-rack
```

Restart an existing Codex session after adding the server so its tools are discovered.

## 4. Install the optional Codex skill

The skill teaches Codex efficient VCV Rack workflows and safety rules. Copy the complete
skill directory into the Codex skills directory:

### macOS and Linux

```sh
mkdir -p ~/.codex/skills
cp -R skill/octavia ~/.codex/skills/octavia
```

### Windows PowerShell

```powershell
New-Item -ItemType Directory -Force "$env:USERPROFILE\.codex\skills" | Out-Null
Copy-Item -Recurse -Force skill\octavia "$env:USERPROFILE\.codex\skills\octavia"
```

Restart Codex after installing or updating the skill.

## Configuration and authentication

The defaults work without configuration. Optional environment variables are:

- `OCTAVIA_PORT`: HTTP port used by both the Rack module and MCP server; default `7777`.
- `OCTAVIA_TOKEN`: shared secret sent as `X-Octavia-Token` by the MCP server.

When using a token or non-default port, set the same values in VCV Rack's launch
environment and in the Codex MCP registration:

```sh
codex mcp remove vcv-rack
codex mcp add --env OCTAVIA_PORT=7777 --env OCTAVIA_TOKEN=replace-me vcv-rack -- ~/.octavia-mcp/bin/python ~/.octavia-mcp/Octavia_MCP.py
```

Do not put a real token in this repository.

## Other MCP clients

Register `Octavia_MCP.py` as a local stdio MCP server using the Python interpreter from
the virtual environment. A generic configuration looks like this:

```json
{
  "mcpServers": {
    "vcv-rack": {
      "command": "/absolute/path/to/.octavia-mcp/bin/python",
      "args": ["/absolute/path/to/.octavia-mcp/Octavia_MCP.py"]
    }
  }
}
```

The exact configuration filename and restart procedure depend on the client.

## Verify and troubleshoot

Ask the client to call `vcv_get_status`. A working connection returns JSON containing
`"running": true` and the configured port.

- Connection refused: VCV Rack is not running, Octavia is absent, or **Start** was not pressed.
- Unauthorized response: `OCTAVIA_TOKEN` differs between VCV Rack and the MCP process.
- Tools are missing: confirm `codex mcp get vcv-rack`, then restart Codex.
- Wrong port: set the same `OCTAVIA_PORT` for VCV Rack and the MCP registration.
- Server startup failure: run the registered Python command directly to see dependency errors.
