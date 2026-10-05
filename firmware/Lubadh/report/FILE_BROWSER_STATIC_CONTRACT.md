# File browser folder provenance and ordering

Static interpretation of audio_load SHA-256
`2dd4523f1c846c14952b301aaacb509ed13125fbbd2e0e49c8a1634a1139301a`.
No shell commands from the firmware were executed and no directory/import
histories were emulated. This closes folder-0 provenance in the normal mapper
construction path; it does not certify the whole browser or filesystem workflow.

## Saved folder occupies index 0

Let G=0x36288, the worker's global state. Global initialization constructs a
string at G+84 from address 0x22f98 (0x1417c–0x14188). Hash-checked ELF
extraction verifies the bytes spell `saved/` followed by zero. See
`tools/verify_import_tag_bytes.ps1` and `evidence/import_tag_byte_checks.json`.

`mapFiles` at 0x152a8 clears the folder vector G+72, preserving its allocation
(0x152b8–0x15338). It builds a directory-list shell command using the same
G+84 string through the literal pointer at 0x15ea4. The string expression is
`cd /media/usb/samples/; ls -p | grep / | grep -v ` plus `saved/` plus
` > .folders` (0x15354–0x15388; literals 0x22c50/0x22c84).
The grep exclusion is a text filter, not exact structured directory matching;
other names containing the pattern can also be excluded.

Before reading `.folders`, the routine copies G+84 into a FolderFileList with
an empty file vector and appends it to G+72 (0x153b4–0x1541c). If the existing
vector has no spare capacity it reaches the equivalent realloc-insert branch
at 0x15be4. Subsequent folder-list lines are appended after this first element
(0x15600–0x15698, with capacity branch at 0x15ab0). This establishes saved/
as folder 0 on the normal mapper path, independent of alphabetic directory order.

It follows that the previously traced no-tag default (folderIndex!=0) means:
untagged saved-folder audio uses no append, while other mapped-folder audio
uses silence padding. Explicit `{n}`, `{s}`, `{c}` tags override that default.
This is a recovered static connection, not an actual save/export round trip.

## File ordering and local capacity

The mapper loops over FolderFileList entries at stride 36 (0x156d4–0x15714).
It compares each folder to the saved-folder string at 0x15718–0x15738 and
0x15b58–0x15b70. For the saved-folder match it constructs the command suffix
`"; ls -p -r | grep -v / > .files` (0x15b84–0x15bbc, literal 0x22cc8).
For other folders it uses
`"; ls -p | grep -v / > .files` (0x1574c–0x15780, literal 0x22cec).
Thus saved files request reverse listing order, other files normal listing order.
The actual sort is supplied by the installed ls and its environment; this is
not proof of numeric, timestamp or locale-independent ordering.

The file-line reader checks the current string-vector count against 11 before
appending (0x158f8–0x1590c), accepting at most twelve filenames per folder
on this path. It appends raw line strings at 0x15974–0x159a0. These slices
show no audio-extension validation; shell filtering removes entries marked as
directories. Do not equate browser membership with a validated audio asset.
Folder-list limits, shared summary publication, blank/unreadable listings and
preview selection need the rest of mapFiles and original execution coverage.

## Native acceptance and design implications

The Rack browser should represent saved recordings and imported audio as
explicit categories, with explicit default tail policy. Preserve the recovered
saved-versus-imported defaults even if the native browser uses stable asset IDs
instead of firmware folder indices. Declare ordering and any expanded file
limits as native decisions; do not silently rely on shell locale or line order.

Required fixtures include an empty samples root, empty saved folder, missing
saved folder, saved/ plus names containing `saved`, more than twelve files,
blank/unreadable listing data, supported/unsupported audio extensions, duplicate
display names, spaces and Unicode, and changed lists while a menu is open.
Join resulting selected indices to the Time menu model, tag selector, worker
read extent, preview and completion. Native stale selections must resolve by
stable asset identity or fail without replacing the tape.
