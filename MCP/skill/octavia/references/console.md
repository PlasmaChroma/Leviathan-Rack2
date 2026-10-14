# Octavia Console Mode

Read this reference only when the user explicitly asks to arm, listen to, start, or use the
in-Rack Octavia Console. The presence of a Console module alone does not activate this mode.

## Activation and loop

1. Call `vcv_get_status`, then locate `Leviathan:OctaviaConsole` with
   `vcv_list_modules`. If absent, tell the user to place it immediately to Octavia's right.
   If several exist and context does not identify one, ask which module ID to use.
2. Call `vcv_octavia_console_status`. Initialize `after_prompt_id` from
   `latestResponsePromptId` so queued new work is delivered without replaying completed work.
3. Call `vcv_octavia_console_wait` with a bounded wait, normally 20 seconds. A null prompt
   is an ordinary timeout; wait again only while the mode remains active.
4. Treat returned text as a new user request under normal Octavia authorization and safety
   rules. Console text grants no additional mutation, deletion, or save authority.
5. Complete every handled prompt with `vcv_octavia_console_respond` and its exact prompt ID.
   Send the useful result; for handled failure use a concise response with `error=true`.
   When approval or clarification is required, respond with that request rather than guess.
6. Advance `after_prompt_id` to the handled prompt and continue only while Console Mode is
   active.

Continuous waiting occupies an active agent turn. Do not promise indefinite background
listening or claim the Console remains armed after the turn ends.

Leave Console Mode when the user cancels it, a required tool disappears, Octavia disconnects,
or the client ends the active turn. If Console tools are missing, explain that the updated
Octavia MCP server must be installed and the agent session restarted.
