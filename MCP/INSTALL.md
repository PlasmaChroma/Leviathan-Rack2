# Octavia MCP installation

Octavia connects VCV Rack to any MCP-capable coding agent. The included Python
server translates MCP tool calls into requests to the Octavia module at
`localhost:7777`.

The MCP server uses standard **stdio transport** and works with clients that
support local MCP servers. This guide gives verified Codex instructions plus
common configuration examples for other clients. MCP does not standardize how
each client stores server configuration, so consult that client's current
documentation when its setup differs.

## Requirements

- VCV Rack 2.x with the Leviathan plugin
- Python 3.10 or newer
- An MCP-capable client (Codex, Claude Desktop, Claude Code, Gemini CLI,
  Antigravity, or any tool that supports stdio MCP servers)

---

## 1. Start Octavia in VCV Rack

1. Install or build the Leviathan plugin and restart VCV Rack.
2. Add **Leviathan > Octavia** to the patch.
3. Press **Start**. The Octopus artwork becomes full brightness when the HTTP
   bridge is listening.

Octavia listens only on `127.0.0.1`. The default port is `7777`; set
`OCTAVIA_PORT` in VCV Rack's environment before launch to use another port.

---

## 2. Install the MCP server

Create an isolated Python environment and install the runtime dependencies.
Run the following commands from the repository's `MCP/` directory, which
contains `requirements.txt` and `mcp_server/Octavia_MCP.py`.

### macOS and Linux

```sh
cd MCP
python3 -m venv ~/.octavia-mcp
~/.octavia-mcp/bin/python -m pip install -r requirements.txt
cp mcp_server/Octavia_MCP.py ~/.octavia-mcp/Octavia_MCP.py
```

### Windows PowerShell

```powershell
Set-Location MCP
py -m venv "$env:USERPROFILE\.octavia-mcp"
& "$env:USERPROFILE\.octavia-mcp\Scripts\python.exe" -m pip install -r requirements.txt
Copy-Item mcp_server\Octavia_MCP.py "$env:USERPROFILE\.octavia-mcp\Octavia_MCP.py"
```

> **Tip:** Write down the absolute paths to the Python interpreter and
> `Octavia_MCP.py` inside the venv — you will need them for registration below.

---

## 3. Register the MCP server with your client

Pick the section that matches your coding agent. Every section registers the
same stdio server; only the configuration surface differs. Third-party client
paths and commands can change between releases, so treat those as examples and
check the client documentation when they do not work.

### 3a. Codex

```sh
codex mcp add vcv-rack -- ~/.octavia-mcp/bin/python ~/.octavia-mcp/Octavia_MCP.py
```

```powershell
codex mcp add vcv-rack -- "$env:USERPROFILE\.octavia-mcp\Scripts\python.exe" "$env:USERPROFILE\.octavia-mcp\Octavia_MCP.py"
```

Confirm with `codex mcp get vcv-rack`, then restart any existing Codex session.

### 3b. Claude Code

```sh
# macOS / Linux — project scope (saved to .mcp.json)
claude mcp add vcv-rack -- ~/.octavia-mcp/bin/python ~/.octavia-mcp/Octavia_MCP.py

# macOS / Linux — user scope (available in all projects)
claude mcp add -s user vcv-rack -- ~/.octavia-mcp/bin/python ~/.octavia-mcp/Octavia_MCP.py
```

```powershell
claude mcp add vcv-rack -- "$env:USERPROFILE\.octavia-mcp\Scripts\python.exe" "$env:USERPROFILE\.octavia-mcp\Octavia_MCP.py"
```

Verify with `claude mcp list`, or type `/mcp` inside a Claude Code session.

### 3c. Claude Desktop

Edit `claude_desktop_config.json`:

- **macOS:** `~/Library/Application Support/Claude/claude_desktop_config.json`
- **Windows:** `%APPDATA%\Claude\claude_desktop_config.json`

Add the server inside the `mcpServers` object:

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

Fully quit and restart Claude Desktop after saving.

### 3d. Gemini CLI example

Edit `~/.gemini/settings.json` (global) or `.gemini/settings.json` (project):

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

Restart the Gemini CLI or run `/mcp` inside a session to verify.

### 3e. Antigravity example

Edit `~/.gemini/config/mcp_config.json` (global) or
`.agents/mcp_config.json` (workspace):

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

Restart the Antigravity session after saving.

### 3f. Other MCP clients

Register `Octavia_MCP.py` as a local stdio MCP server using the Python
interpreter from the virtual environment. Many clients use a JSON object
similar to this one:

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

The exact configuration schema, filename, and restart procedure depend on the
client.

---

## 4. Install the optional skill

The skill teaches your agent efficient VCV Rack workflows and safety rules.
Install it into the skill directory that matches your client.

### Codex

```sh
# macOS / Linux
mkdir -p ~/.codex/skills
cp -R skill/octavia ~/.codex/skills/octavia

# Windows PowerShell
New-Item -ItemType Directory -Force "$env:USERPROFILE\.codex\skills" | Out-Null
Copy-Item -Recurse -Force skill\octavia "$env:USERPROFILE\.codex\skills\octavia"
```

### Other clients

`skill/octavia/` is written in the Codex skill format. If another client
supports compatible skills, install the entire directory according to that
client's current documentation. Otherwise, use `SKILL.md` as project guidance
and keep its `references/` directory alongside it.

Restart the agent session after installing or updating any skill or project
guidance.

---

## Configuration and authentication

The defaults work without configuration. Optional environment variables are:

- `OCTAVIA_PORT`: HTTP port used by both the Rack module and MCP server; default `7777`.
- `OCTAVIA_TOKEN`: shared secret sent as `X-Octavia-Token` by the MCP server.

When using a token or non-default port, set the same values in VCV Rack's launch
environment **and** in the MCP server registration. For CLI-based clients, pass
the variables at registration time:

```sh
# Codex
codex mcp remove vcv-rack
codex mcp add --env OCTAVIA_PORT=7777 --env OCTAVIA_TOKEN=replace-me vcv-rack -- ~/.octavia-mcp/bin/python ~/.octavia-mcp/Octavia_MCP.py

# Claude Code
claude mcp add -e OCTAVIA_PORT=7777 -e OCTAVIA_TOKEN=replace-me vcv-rack -- ~/.octavia-mcp/bin/python ~/.octavia-mcp/Octavia_MCP.py
```

If a JSON-configured client accepts an `env` block, add one like this:

```json
{
  "mcpServers": {
    "vcv-rack": {
      "command": "/absolute/path/to/.octavia-mcp/bin/python",
      "args": ["/absolute/path/to/.octavia-mcp/Octavia_MCP.py"],
      "env": {
        "OCTAVIA_PORT": "7777",
        "OCTAVIA_TOKEN": "replace-me"
      }
    }
  }
}
```

Do not put a real token in this repository.

---

## Verify and troubleshoot

Ask the client to call `vcv_get_status`. A working connection returns JSON
containing `"running": true` and the configured port.

| Symptom | Cause | Fix |
|---|---|---|
| Connection refused | VCV Rack is not running, Octavia is absent, or **Start** was not pressed | Launch VCV Rack and press Start on the Octavia module |
| Unauthorized response | `OCTAVIA_TOKEN` differs between VCV Rack and the MCP process | Set the same token in both environments |
| Tools are missing | Server not registered or session not restarted | Re-check registration (see section 3), then restart the agent session |
| Wrong port | `OCTAVIA_PORT` mismatch | Set the same port for VCV Rack and the MCP registration |
| Server startup failure | Missing Python dependencies | Run the registered Python command directly in a terminal to see errors |
