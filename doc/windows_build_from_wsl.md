# Native Windows Builds from a WSL Codex Terminal

## Purpose

The repository is stored under MSYS2 on the Windows filesystem, while some
Codex terminals run inside WSL. A normal `make` issued directly by that WSL
shell uses the Linux compiler and is not authoritative for the Windows Rack
plugin. The Windows Codex application does not have this boundary because it
runs on the Windows side.

The WSL sandbox also blocks Windows-process interoperability. An approved
out-of-sandbox invocation can enter the installed MSYS2 environment and run the
native MINGW64 compiler non-interactively.

## Authoritative invocation

Use MSYS2's Bash, explicitly select MINGW64, and put the MinGW tools first on
`PATH`:

```sh
/mnt/c/msys64/usr/bin/bash.exe -lc \
  'export MSYSTEM=MINGW64; \
   export PATH=/mingw64/bin:/usr/bin; \
   hash -r; \
   cd /home/Plasm/Leviathan && \
   make -j10 plugin.dll'
```

The expected compiler identity is:

```text
/mingw64/bin/g++
x86_64-w64-mingw32
```

The generic MSYS environment is insufficient: it reports `MSYSTEM=MSYS` and
does not expose `g++` until MINGW64 and its `PATH` are selected.

## Incremental versus clean builds

Normal development builds should preserve the object cache:

```sh
make -j10 plugin.dll
```

Do not put `make clean` in the routine build path. The plugin currently has
roughly one hundred translation units, so a wrapper such as
`make clean && make -j10 install` guarantees a complete rebuild every time.
Reserve a clean build for explicit release verification, toolchain changes, or
suspected dependency corruption.

Theme headers do contribute rebuild fan-out, but they are not the main cause of
routine full rebuilds. At the time of investigation, `ThemeTypes.hpp` appeared
in 36 generated dependency files, mostly through `PanelSvgUtils.hpp`, while
`ThemeService.hpp` appeared in only five. Editing the shared theme types or
panel helper therefore causes a legitimate partial rebuild; ordinary module or
theme implementation edits should remain incremental.

## Rack-installable artifacts

`plugin.dll` is the native Windows plugin binary. A complete Rack installation
also needs `plugin.json` and the declared resources. The normal distributable is
therefore the packaged `.vcvplugin` file:

```sh
/mnt/c/msys64/usr/bin/bash.exe -lc \
  'export MSYSTEM=MINGW64; \
   export PATH=/mingw64/bin:/usr/bin; \
   hash -r; \
   cd /home/Plasm/Leviathan && \
   make -j10 dist'
```

This produces a platform-qualified archive under `dist/`, for example:

```text
dist/Leviathan-2.9.1-win-x64.vcvplugin
```

That archive is an actual Windows Rack plugin package. `make install` first
builds the same package and then copies it to Rack's Windows user plugin
directory, normally `%LOCALAPPDATA%/Rack2/plugins-win-x64`. Because that
overwrites or supplements the user's installed development package, run it only
when installation is intended:

```sh
/mnt/c/msys64/usr/bin/bash.exe -lc \
  'export MSYSTEM=MINGW64; \
   export PATH=/mingw64/bin:/usr/bin; \
   hash -r; \
   cd /home/Plasm/Leviathan && \
   make -j10 install'
```

## Validation record

On 2026-08-21, the native invocation verified:

- MINGW64 compiler target `x86_64-w64-mingw32`;
- an up-to-date 64-bit PE `plugin.dll` containing the current Sibyl changes;
- successful packaging of `dist/Leviathan-2.9.1-win-x64.vcvplugin`.

The package was created but was not copied into Rack's user plugin directory.
Loading it in Rack remains the final runtime smoke test.

## Native fast tests

`test-fast` can also be compiled and executed as native Windows binaries. Tests
that link Rack must load the DLLs from the installed Rack application, not only
from the SDK. On this machine the verified command is:

```sh
/mnt/c/msys64/usr/bin/bash.exe -lc \
  'export MSYSTEM=MINGW64; \
   export PATH=/mingw64/bin:/usr/bin; \
   hash -r; \
   cd /home/Plasm/Leviathan && \
   make -j10 test-fast \
     RACK_APP_RUNTIME_DIR="/c/Program Files/VCV/Rack2Pro"'
```

The Rack application directory must take precedence over compiler runtime
directories while running Rack-linked tests, so `libRack.dll` and its matching
C++ runtime DLLs are loaded as one set. The Makefile's Rack-aware test runner
enforces that ordering.

MinGW requires `_USE_MATH_DEFINES` for standalone tests that use `M_PI`, either
directly or through Rack SDK DSP headers. The Makefile adds that definition only
for the affected native test builds.

On 2026-08-21, the complete native `test-fast` target built and passed under
MINGW64 using the installed Rack 2 Pro runtime.

Native test binaries use the `.exe` form under `build/tests/`. A successful
native run validates the Windows compiler and runtime behavior of the focused
harnesses; it does not replace loading the packaged plugin in Rack for the final
UI, graphics-context, audio-device, and patch-integration smoke test.
