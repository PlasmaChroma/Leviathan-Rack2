# Bifurx Premium development

The main Leviathan repository remains the source of Bifurx and shared rendering
code. The normal build has **no DRM dependency**: `BifurxLicense.hpp` is empty
unless `LEVIATHAN_PRO_DRM=1`. Audio checks, the bypass override, and the VCV
license overlay are compiled only in Premium builds.

Leviathan Pro owns its plugin bootstrap, manifest, Makefile, and the unmodified
vendored `DRM/drm.hpp`. Its build always enables DRM. The context is created
after registering Bifurx and deleted during plugin shutdown. It checks plugin
`Leviathan-Pro` and model `Bifurx`. Unlicensed processing and bypass explicitly
clear output, including voltages left over from a polyphonic bypass.

## Build and test from the main repository

Run these inside the native MSYS2 MINGW64 environment on Windows:

```sh
make -j10 plugin.dll                  # normal Leviathan, no DRM needed
make -j10 premium-build              # isolated, real DRM enabled
make test-premium RACK_APP_RUNTIME_DIR="/c/Program Files/VCV/Rack2Pro"
make -j10 premium-dist               # stripped Rack-installable Premium archive
```

The Premium targets stage the **current main-repo Bifurx sources**, paired with
Pro's bootstrap/manifest, into `build/premium-validation/`. The DLL, object files,
and `dist/` archive stay there. These targets neither synchronize Pro's shared
sources nor install the plugin in Rack. Unchanged files retain their timestamps
for incremental builds; changing the DRM header/configuration invalidates only
Premium objects. `DRM_DISABLE` is rejected in Premium validation/release code.

Dependencies are configurable:

```sh
make -j10 premium-build \
  PREMIUM_PRO_DIR=../Leviathan-Pro \
  PREMIUM_DRM_DIR=../Leviathan-Pro/DRM \
  RACK_DIR=../Rack-SDK
```

There is no second copy of the DRM header in the main repository. Missing
Premium dependencies fail preparation before changing an existing snapshot.
The normal `test-fast` suite does not require the DRM header or a Pro checkout.

`test-premium` uses the real header with a temporary profile and an empty account
token. It checks missing-context/missing-license silence and bypass gating,
without touching installed licenses or making network requests. It does not
prove successful activation; that requires a valid VCV development license.

## Test activation in Rack

Use the archive from `build/premium-validation/dist/` when ready to test in Rack.
It has the real `Leviathan-Pro` identity and replaces an installed Pro build;
it is not a second plugin that can coexist under the same slug.

This vendored revision reads `licenses/Leviathan-Pro.vcvkey` beneath Rack's user
directory and can download the license using the logged-in VCV account. The
upstream README's older `.key` examples do not match this header. Use VCV's
instructions when they provide the development license. Check the locked
overlay and silence without a license, then successful unlock and normal audio
with the supplied license, including restart, bypass, and both panel modes.

## Sync validated changes into Pro

```sh
python3 tools/sync_bifurx_to_pro.py --dry-run
python3 tools/sync_bifurx_to_pro.py
```

The managed list includes `BifurxLicense.hpp` and Bifurx's shared UI dependencies.
It never overwrites Pro's DRM directory or its existing plugin bootstrap. After
sync, Pro builds the same licensing integration with its local header:

```sh
make -C ../Leviathan-Pro -j10 dist
```

VCV's header and license material stay in the Pro repository; no license files
are included in generated snapshots or plugin archives.
