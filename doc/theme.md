# Leviathan Theme Module — Implementation Specification v0.2

## Status and normative language

This document defines Theme V1. Sections explicitly marked **Future** are
non-blocking design notes and are not part of V1 acceptance.

The words **must**, **must not**, **should**, and **may** are intentional:

* **must / must not** — required for V1 acceptance
* **should** — expected unless implementation evidence justifies a deviation
* **may** — optional

Where an example conflicts with a stated requirement, the requirement wins.
Provisional UI artwork and future cross-plugin examples are not implementation
contracts.

## 1. Purpose

`Theme` is a Leviathan meta-module for configuring the visual language shared by Leviathan modules.

It provides user control over:

* semantic **Input** color
* semantic **Output** color
* general **Accent** color for explicitly themeable artwork/text
* fractal **Texture Amount**
* factory theme presets
* user-created custom presets

Theme is not an audio-processing module. It acts as a user-facing editor for a plugin-level visual theme service.

The underlying theme system must be designed from the beginning so that future modules contained in a separate Leviathan Premium plugin can consume the same theme.

V1 is complete without a Premium plugin or runtime bridge. V1 must avoid
choices which would prevent such a bridge, but must not carry speculative
cross-plugin complexity into the free-plugin runtime service.

## 1.1 V1 observable contract

Theme V1 is successful when all of the following are true:

* Input, Output, and Accent colors can be edited globally.
* Texture Amount can be edited globally from 0% through 200%.
* Only explicitly tagged SVG artwork participates.
* Generic glass, module titles, logos, and branding retain their authored look.
* The canonical theme causes no intentional visual change in migrated modules.
* Theme changes are visible in existing module widgets without re-adding them.
* Deleting every Theme module does not reset the active theme.
* Adding another Theme module shows the same active state.
* Patch loading never applies a global theme in V1.
* Active theme and user presets survive Rack restart.
* Steady state adds no framebuffer invalidation or DSP work.
* Graphics-context destruction and recreation rebuilds themed resources lazily.

---

# 2. Design Principles

## 2.1 Semantic roles are independent of rendering material

The theming system shall distinguish between:

**What an element represents**

```cpp
enum class ThemeRole : uint8_t {
    None,
    Input,
    Output,
    Accent
};
```

and:

**How an element is rendered**

Examples:

* glass
* text
* raster/vector accent
* jack effect
* display annotation

Therefore:

```text
Input Glass
```

is represented conceptually as:

```text
Material = Glass
Role     = Input
```

rather than as a unique `InputGlass` rendering system.

This prevents the theme architecture from becoming a matrix of specialized element types.

---

# 3. Theme Data Model

The V1 theme state is:

```cpp
struct ThemeColor {
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;

    bool operator==(const ThemeColor& other) const {
        return r == other.r && g == other.g && b == other.b;
    }
};

struct ThemeColors {
    ThemeColor input;
    ThemeColor output;
    ThemeColor accent;
};

struct ThemeSurface {
    // Relative to canonical Leviathan texture intensity.
    // 0.0 = disabled
    // 1.0 = current/default appearance
    // 2.0 = 200% intensity
    float textureAmount = 1.f;
};

struct ThemeSnapshot {
    uint32_t schemaVersion = 1;

    ThemeColors colors;
    ThemeSurface surface;
};
```

`ThemeSnapshot` is the canonical value type. RGB bytes and `textureAmount` are
the only runtime sources of truth. HSV and HEX are editor representations and
must not be stored as additional mutable state.

Before publication, every snapshot must be canonicalized:

* RGB values are already bounded by their byte representation.
* non-finite Texture Amount becomes the canonical default
* Texture Amount is clamped to `[0.0, 2.0]`
* schema version is normalized to the current supported version

Theme equality is equality after canonicalization. Applying an equal snapshot
must not increment a generation or schedule persistence.

Alpha is intentionally excluded from user colors in V1.

Individual rendering systems remain responsible for material opacity, glow strength, reflection opacity, etc.

This prevents a theme color from accidentally making important UI elements invisible.

---

# 4. Canonical Default Theme

The default theme must reproduce the existing Leviathan appearance as closely as practical.

V1 canonical values are:

```text
Input:         #7A5CFF
Output:        #1CCCD9
Accent:        #5740BF
Texture Amount: 1.0
```

These values match colors already used throughout the current panel system.
Phase 0 may change them only if reference-image calibration demonstrates that
another exact value reproduces the existing appearance more faithfully. Once
Phase 0 is accepted, the constants are frozen for Theme schema V1 and the
document must be updated with the accepted values.

Initial semantic convention:

```text
Input  → canonical Leviathan violet
Output → canonical Leviathan cyan
Accent → canonical Leviathan panel accent
Texture → 100%
```

A clean migration must not cause existing users to open Rack and suddenly see a radically altered plugin.

---

# 5. Theme Runtime Service

A new visual subsystem should be introduced:

```text
src/theme/
    ThemeTypes.hpp
    ThemeService.hpp
    ThemeService.cpp
    ThemePersistence.hpp
    ThemePersistence.cpp
```

Required behavioral interface (exact file and function names may vary):

```cpp
namespace leviathan::theme {

enum ThemeChange : uint32_t {
    ChangeNone    = 0,
    ChangeColors  = 1u << 0,
    ChangeSurface = 1u << 1,
    ChangePresets = 1u << 2
};

struct ThemeState {
    ThemeSnapshot snapshot;
    std::string activePreset = "factory:leviathan";
    uint64_t generation = 0;
    uint64_t colorGeneration = 0;
    uint64_t surfaceGeneration = 0;
    uint64_t presetGeneration = 0;
};

ThemeState read();

// Lock-free change probes for widget step() hot paths. A consumer calls read()
// only after its relevant probe changes.
uint64_t generation();
uint64_t colorGeneration();
uint64_t surfaceGeneration();
uint64_t presetGeneration();

ThemeColor color(ThemeRole role);

ThemeChange setColor(ThemeRole role, ThemeColor color);
ThemeChange setTextureAmount(float amount);

ThemeChange apply(const ThemeSnapshot& theme);

ThemeChange applyPreset(const ThemeSnapshot& theme, const char* stableId);

ThemeChange resetToDefault();

void initialize();
void shutdown();

}
```

`read()` returns a value copy. The service must not expose a mutable reference,
pointer, `NVGcolor`, widget, framebuffer, or graphics-context resource.

The generation probe functions are atomic loads and must not take the snapshot
mutex. Publishing a changed snapshot happens-before publishing its new domain
generation. This keeps unchanged module widgets off the mutex in steady state.

The service owns monotonically increasing runtime generation counters:

* `generation` changes for any published theme or preset-state change.
* `colorGeneration` changes only when Input, Output, or Accent changes.
* `surfaceGeneration` changes only when Texture Amount changes.
* `presetGeneration` changes only when the user-preset collection or active
  preset identity changes.

An actual change increments the applicable domain generation and the overall
generation exactly once:

```cpp
ThemeChange changed = apply(canonicalize(candidate));
```

No-op writes do nothing. Generation wrap is permitted; consumers compare for
inequality rather than ordering.

Rendering widgets remember only the generation relevant to their framebuffer.

When it changes:

```cpp
ThemeState state = theme::read();
if (cachedColorGeneration != state.colorGeneration) {
    cachedColorGeneration = state.colorGeneration;
    framebuffer->setDirty();
}
```

Fractal composition observes `surfaceGeneration`. Overlays containing semantic
glass also observe `colorGeneration` to re-palette cached source fields as
specified in Section 14; that color invalidation must not regenerate iteration.

Theme mutation occurs from UI-side code. Consumers may read from UI widgets,
module-browser preview creation, and future bridge code. Publication must
therefore produce a coherent snapshot and generations. A small mutex around
copy/mutation is the preferred V1 implementation; lock-free publication is not
required. No ThemeService operation may be called from a module's audio
`process()` method.

No continuous theme processing occurs after relevant framebuffers have redrawn.

---

# 6. Theme Module Ownership Model

The `Theme` module does **not** own the theme.

It is a controller/view onto `ThemeService`.

Therefore:

* deleting Theme does not reset the theme
* adding Theme displays the currently active theme
* multiple Theme modules may coexist
* all Theme modules display/edit the same state
* editing one Theme instance immediately updates the others

This avoids the concept of a particular rack module instance becoming the hidden owner of global Leviathan rendering state.

The module itself should store little or no theme state in patch JSON.

It may store harmless UI state such as:

```text
currently selected editor role
currently selected preset browser page
```

but global theme state belongs to the theme service.

## 6.1 Rack module and control contract

Theme V1 must not expose its global controls as Rack engine parameters, inputs,
outputs, or lights. Its module enum tails are therefore empty in V1:

```cpp
enum ParamId  { PARAMS_LEN };
enum InputId  { INPUTS_LEN };
enum OutputId { OUTPUTS_LEN };
enum LightId  { LIGHTS_LEN };
```

The picker, fields, slider, and preset controls are UI widgets which call
`ThemeService` directly. This prevents each Theme instance and each saved patch
from owning a conflicting copy of global values. It also makes clear that V1
does not provide automation, CV, MIDI mapping, or audio-rate theme modulation.

The Theme model must be appended to `plugin.json`, `plugin.hpp`, and the model
registration sequence. Existing released-module enum ordering must not change.

## 6.2 Multiple-editor synchronization

Each Theme widget observes the overall generation during `step()` and refreshes
non-focused controls from the copied service state. A numeric or HEX field with
keyboard focus retains a local draft until Enter, focus loss, or Escape:

* Enter or focus loss validates and commits the draft.
* Escape discards the draft and reloads the current global value.
* External changes do not rewrite a focused draft.
* If two editors commit competing values, the last committed value wins.

A single drag gesture should be coalesced to at most one accepted update per UI
frame. It may redraw live, but it must become one logical edit for any local
undo/revert affordance. Global Theme edits are user preferences and must not be
placed in Rack's patch history in V1.

---

# 7. Patch Loading Behavior

V1 should **not** automatically restore a global theme merely because a saved patch contains a Theme module.

Opening an old patch should not unexpectedly alter every Leviathan module in the user's entire Rack environment.

Therefore:

```text
Patch JSON → Theme UI state only
Global Theme → plugin/user settings
```

A future explicit feature could support:

```text
"Store theme with patch"
"Apply patch theme on load"
```

but this is out of scope for V1.

---

# 8. SVG Semantic Convention — Glass

Existing generic `glass` artwork remains valid.

New semantic groups are introduced:

```xml
<g id="glass">
    ...
</g>

<g id="glass_input">
    ...
</g>

<g id="glass_output">
    ...
</g>

<g id="glass_accent">
    ...
</g>
```

Meaning:

```text
glass
    authored/default color; not semantic

glass_input
    Input theme color

glass_output
    Output theme color

glass_accent
    Accent theme color
```

Existing modules can therefore migrate progressively.

No existing generic glass must become theme-controlled unless intentionally retagged.

These identifiers are exact, case-sensitive semantic tokens. Runtime semantic
matching must not use substring matching. In particular, `glass_input` is not
both `glass` and `glass_input`.

Only recognized ancestor groups assign a role. A child element whose own ID
happens to contain `glass_input` does not opt in. This makes the SVG group
hierarchy, rather than arbitrary editor-generated element names, the contract.

For split-panel modules, semantic groups must be edited in the master
`res/<Module>.svg`. Generated `.panel.svg` and `.labels.svg` files must never be
edited directly. After semantic artwork changes:

```text
python3 tools/split_svg_labels.py res/<Module>.svg --overwrite
make generate-panel-anchor-atlas
```

The splitter must preserve semantic group IDs through label outlining. A
tooling test must verify that nested `theme_text*` groups survive extraction and
text-to-path conversion.

---

# 9. SVG Parser Changes

The current panel parser can identify elements contained inside groups matching
a substring, but its returned match structures only retain the child element ID
rather than the semantic identity of the matching ancestor group. The generic
substring APIs may remain for existing callers; Theme parsing must use a new
exact semantic-role path.

The parser should be extended to retain semantic ancestor information.

Possible representation:

```cpp
struct SvgThemeMetadata {
    ThemeRole role = ThemeRole::None;
};
```

or directly:

```cpp
struct SvgPathMatch {
    ...
    ThemeRole themeRole = ThemeRole::None;
};
```

Semantic group inheritance should use the nearest recognized theme ancestor.

Recognized glass groups:

```text
glass         → ThemeRole::None
glass_input   → ThemeRole::Input
glass_output  → ThemeRole::Output
glass_accent  → ThemeRole::Accent
```

Recognized label groups:

```text
theme_text        → ThemeRole::Accent
theme_text_input  → ThemeRole::Input
theme_text_output → ThemeRole::Output
```

`SvgRectMatch` and `SvgPathMatch` must both retain the resolved role. The
semantic parser must retain transforms, authored fill information, bounds, and
path commands exactly as existing themed renderers require.

Theme groups should not be nested. If they are, the nearest recognized ancestor
wins deterministically so release behavior is stable.

When `isDragonKingDebugEnabled()` is true, contradictory nesting and unknown
names beginning with `glass_` or `theme_text_` should emit a warning containing
the asset path and element/group ID. Release builds must remain quiet.

---

# 10. Glass Rendering Integration

Current glass art stores its authored base color and passes it through a global
tint operation. V1 replaces that operation with explicit role resolution.

The renderer must resolve a material palette rather than a single ambiguous
color:

```cpp
struct GlassPalette {
    NVGcolor pigment;
    NVGcolor shadow;
    NVGcolor highlight;
    NVGcolor edgeStart;
    NVGcolor edgeEnd;
};

GlassPalette resolveGlassPalette(const GlassArt& glass,
                                 const ThemeSnapshot& theme);
```

Resolution rules:

```text
Role None
    pigment = authored base color

Role Input / Output / Accent
    pigment = matching theme color

shadow and highlight
    derived from pigment by canonical shared glass-material functions

edgeStart and edgeEnd
    retain the existing canonical violet-to-cyan dual-color treatment in V1
    for both generic and semantic glass
```

The initial derivation must preserve the existing opacity, small-region boost,
glare, reflection, and geometry-specific behavior. Exact derivation constants
must live in one shared helper; modules must not derive their own theme palettes.

Keeping the dual-color edge treatment canonical is the Phase 0 material
decision: semantic Theme color controls the body pigment, glow, and pigment-based
shading while the violet/cyan edge remains part of Leviathan's authored glass
identity. Making edge colors semantic is a possible later extension, not a V1
requirement.

The glass renderer remains responsible for:

* body transparency
* dimensional shading
* bloom
* edge highlights
* glare
* reflection
* depth

Theme therefore changes color without destroying Leviathan's material rendering.

The legacy `PanelGlassTintState` wash and animated color progression must not be
composed on top of semantic colors. Phase 0 removes that composition from the
representative vertical slice. Final removal occurs after legacy persistence
migration and retirement of the Integral Flux crystal controller.

---

# 11. Existing Special Glass Geometry

Existing module-specific glare treatments are not required to disappear during this project.

The current renderer contains special glare handling for elements in Temporal Deck, Bifurx, Wyrm, and Iris.

Those treatments may remain geometry-specific.

This project only removes the need for those elements to have module-specific **color semantics**.

Thus:

```text
geometry specialization → allowed
theme/color specialization → discouraged
```

---

# 12. Themeable Label/Text Artwork

The labels SVG gains explicit opt-in groups:

```xml
<g id="theme_text">
    ...
</g>

<g id="theme_text_input">
    ...
</g>

<g id="theme_text_output">
    ...
</g>
```

Mapping:

```text
theme_text        → Accent
theme_text_input  → Input
theme_text_output → Output
```

Artwork outside these groups is unchanged.

This deliberately protects:

* module titles
* logos
* branding
* carefully authored decorative typography

without requiring special knowledge of what an element called `title` means.

A title remains unchanged simply because it is not placed inside a theme group.

Semantic label artwork should use ordinary fills, strokes, gradients, opacity,
and transforms which the filtered renderer can preserve. Filters, masks, or
paint servers which cannot be transformed safely remain authored and emit a
debug-gated warning. They must not disappear silently.

---

# 13. Themed Label Renderer

The current labels system renders the entire labels SVG into a cached framebuffer as one ordinary `SvgWidget`.

That widget should evolve into a layered renderer approximately like:

```text
CachedThemedPanelLabels
    ├── static labels
    ├── Accent tint layer
    ├── Input tint layer
    └── Output tint layer
```

The SVG remains the artistic source of truth. The implementation must retain a
static layer plus one layer for each semantic role. An element appears in
exactly one output layer; role groups must not be duplicated through substring
matching.

Theme groups behave as authored-color layers. Recoloring must preserve authored
alpha, opacity, gradients, relative shading, and the canonical default image.

Conceptually:

```text
authored geometry and alpha
+ authored color relative to the canonical role color
+ active role color
=
themed label
```

The V1 color transform is channel scaling in linear RGB. For every fill, stroke,
and gradient stop in a semantic role:

```text
linearOut[c] = clamp(
    linearAuthored[c] * linearActiveRole[c] / linearCanonicalRole[c],
    0, 1)
```

All canonical role channels are nonzero in V1. Alpha and explicit opacity are
unchanged. When the active role equals its canonical value, the transformed SVG
must reproduce the authored layer within normal rasterization tolerance. This
identity property is more important than preserving a particular abstract color
space.

The preferred implementation should avoid rendering the original neutral artwork underneath an opaque recolored duplicate if that causes washed-out colors.

Rack's `window::Svg` can load SVG markup directly from a string. V1 should use
cached filtered SVG variants rather than reconstructing arbitrary label paths in
custom NanoVG code:

```text
source labels SVG
    → parse/filter once into static and per-role templates
    → materialize a role variant when that role color changes
    → render each variant in its own cached framebuffer
```

Filtering templates is once per source SVG. A colored role variant is rebuilt
only when its relevant color changes, never every frame. Static labels never
redraw for a theme change.

Phase 0 must prove this approach on a representative labels asset containing
paths, strokes, gradients, nested transforms, and outlined text. If Rack's SVG
loader makes faithful filtering impractical, Phase 0 may select another layered
implementation, but the identity, exclusivity, caching, and lifecycle contracts
above remain mandatory and the document must record the selected design.

---

# 14. Fractal Texture Integration

The current fractal glass overlay composites its generated texture using a fixed opacity of:

```cpp
0.22f
```

at final image-pattern composition.

Theme shall expose:

```text
TEXTURE
0% ---------------- 200%
```

with:

```text
0%   = no fractal texture
100% = current Leviathan appearance
200% = twice canonical texture opacity
```

Rendering becomes approximately:

```cpp
float opacity =
    0.22f *
    theme.surface.textureAmount;
```

with appropriate clamping at the final renderer.

The user-facing control is therefore a relative artistic amount rather than the internal NanoVG alpha value.

## 14.1 Fractal palette relationship

The existing fractal overlay is palette-colored from the authored glass fill;
it is not a neutral opacity-only texture. Semantic recoloring must not leave a
violet authored texture visibly sitting on newly cyan, red, or green glass.

For semantic glass, the fractal palette must be derived from the resolved role
pigment. For generic glass, it remains derived from the authored pigment.

The fractal iteration result and the palette application must remain separate:

```text
fractal parameters change
    → regenerate source field

semantic role color changes
    → reuse source field
    → reapply palette
    → upload replacement pixels

Texture Amount changes
    → reuse source field and palette pixels
    → redraw composition only
```

An overlay must retain or reacquire the cached source field needed for
re-palettization. A theme color change may perform palette conversion and image
upload at UI rate, but must not rerun fractal iteration.

---

# 15. Texture Performance

Changing Texture Amount must not regenerate fractals solely because opacity changed.

The current overlay already separates generated/cached fractal imagery from final composition.

A Texture Amount change must therefore:

```text
surfaceGeneration changes
→ fractal framebuffer dirty
→ existing texture recomposited at new opacity
```

not:

```text
Texture Amount change
→ regenerate fractal field
```

At:

```text
textureAmount == 0
```

drawing of the texture should be skipped.

A later optimization may avoid creating/rendering fractal resources entirely while texture is disabled.

That optimization is desirable but not required for first implementation.

---

# 16. Theme Module UI

Recommended initial width:

```text
8–10 HP
```

The editor should be visually spacious enough to make color manipulation pleasant.

Suggested hierarchy:

```text
┌──────────────────────────┐
│          THEME           │
│                          │
│ INPUT  OUTPUT  ACCENT     │
│                          │
│   ┌──────────────────┐   │
│   │                  │   │
│   │   COLOR FIELD    │   │
│   │                  │   │
│   └──────────────────┘   │
│                          │
│   ━━━━━ HUE ━━━━━        │
│                          │
│ HSV          RGB          │
│ H 278        R 153        │
│ S 74         G 74         │
│ V 92         B 235        │
│                          │
│ HEX  #994AEB              │
│                          │
│ ─────── SURFACE ───────   │
│ Texture  ━━━━━●━━━━ 100%  │
│                          │
│ ─────── PRESET ────────   │
│ ‹ Leviathan Default ›     │
│                          │
│ [SAVE] [RESET] [SWAP]     │
└──────────────────────────┘
```

Exact artwork/layout remains open to iteration.

---

# 17. Active Color Role Selection

The user selects which semantic channel is actively edited:

```text
[ INPUT ] [ OUTPUT ] [ ACCENT ]
```

Each selector should display its current color.

The selected role receives clear visual emphasis.

Changing active role does not modify colors.

It only redirects the editor controls to the chosen theme value.

---

# 18. Color Picker

Primary interaction:

```text
HSV saturation/value field
+
Hue strip
```

Dragging should update the theme live.

Theme changes should therefore be visible throughout Leviathan while the selector moves.

The color field itself should remain visually accurate and should not receive decorative tinting that distorts perceived color.

Leviathan styling may surround the picker without altering the actual color field.

---

# 19. Numeric Color Editing

The module should support simultaneous numerical representations:

```text
HSV
H
S
V

RGB
R
G
B

HEX
#RRGGBB
```

Changing any representation updates all others.

Editor conventions are fixed:

```text
H: integer display 0–359 degrees; hue wraps modulo 360 on commit
S: integer display 0–100 percent
V: integer display 0–100 percent
R/G/B: integer display 0–255
HEX: uppercase #RRGGBB after commit
```

Conversion uses floating-point intermediates and the canonical RGB bytes are
rounded to nearest, then clamped. When saturation is zero, the editor retains a
local last meaningful hue for picker continuity, but that hue is not persisted
and does not affect theme equality.

Hex input should accept at least:

```text
RRGGBB
#RRGGBB
```

Alpha editing is not exposed in V1.

Numeric changes should clamp safely rather than produce invalid theme state.

Empty or syntactically incomplete focused text is a draft, not an immediate
zero. Invalid input reverts on focus loss and may show local validation styling;
it must never publish a partial theme value.

---

# 20. Utility Operations

## Swap

```text
SWAP
```

exchanges:

```text
Input ↔ Output
```

Accent remains unchanged.

Swap publishes both changed colors atomically, increments `colorGeneration`
once, and sets the active preset reference to `modified`.

## Reset Role

Reset the currently selected role to its canonical Leviathan default.

Reset Role changes no other value, increments `colorGeneration` only if needed,
and sets the active preset reference to `modified`.

## Reset Theme

Available either on-panel or in context menu.

Restores:

```text
default colors
default texture amount
```

Reset Theme is equivalent to applying `factory:leviathan`; it publishes one
logical change and selects that factory preset.

---

# 21. Factory Presets

The plugin ships with a curated collection of immutable factory presets.

A preset contains the complete theme:

```cpp
struct ThemePreset {
    std::string name;
    ThemeSnapshot theme;
};
```

This means presets control both:

```text
colors
texture amount
```

Factory presets must be deliberately art-directed against Leviathan rendering
rather than generated from arbitrary color formulas. V1 ships these four:

| Stable ID | Display name | Input | Output | Accent | Texture |
|---|---|---:|---:|---:|---:|
| `leviathan` | Leviathan | `#7A5CFF` | `#1CCCD9` | `#5740BF` | 100% |
| `abyssal` | Abyssal | `#3F4C9A` | `#167D8C` | `#2A335F` | 135% |
| `monochrome` | Monochrome | `#A7A9B0` | `#E1E3E8` | `#676A73` | 35% |
| `ultraviolet` | Ultraviolet | `#A44DFF` | `#35D8FF` | `#FF4DFF` | 120% |

`Leviathan` always reproduces canonical defaults. Phase 3 must create reference
captures for all four presets. Palette tuning discovered during that review is
allowed only by updating this table and its golden captures before release.
Additional factory presets may be added later without changing the schema, but
unreviewed presets must not ship merely to fill a list.

---

# 22. User Presets

V1 supports exactly:

```text
8 custom preset slots, numbered 1 through 8
```

Each saves:

```text
name
Input color
Output color
Accent color
Texture Amount
schema version
```

User presets may be:

* saved
* recalled
* renamed
* overwritten
* deleted

A slot has stable identity and is either empty or contains one preset. Deleting
a preset empties its slot; it does not shift later presets. Names are UTF-8,
trimmed on commit, limited to 48 bytes without splitting a code point, and must
not be empty. Duplicate names are allowed because slot number is the identity.

Selecting a factory or user preset applies it immediately.

Manual adjustment after recalling a preset should place the theme into an implicit:

```text
Modified
```

state rather than automatically overwriting the preset.

The service persists an active preset reference of `factory:<stable-id>`,
`user:<1-8>`, or `modified`. Applying a preset publishes its complete snapshot
and active reference in one logical operation. Manual color or surface edits set
the reference to `modified`. Editors synchronize this reference through
`presetGeneration`.

---

# 23. Preset Persistence

Preset data is global user data, not patch data.

Canonical persistent schema:

```json
{
  "schemaVersion": 1,
  "activePreset": "factory:leviathan",
  "active": {
    "input": "#7A5CFF",
    "output": "#1CCCD9",
    "accent": "#5740BF",
    "textureAmount": 1.0
  },
  "userPresets": [
    null,
    {
      "name": "My Theme",
      "input": "#7A5CFF",
      "output": "#1CCCD9",
      "accent": "#5740BF",
      "textureAmount": 1.0
    },
    null,
    null,
    null,
    null,
    null,
    null
  ]
}
```

`userPresets` must contain exactly eight entries after loading normalization.
Missing entries are appended as `null`; extras are ignored. Missing fields use
canonical defaults. Malformed colors, non-numeric or non-finite amounts, and
unknown active preset references are rejected field-by-field without discarding
otherwise valid data. A future schema version must not be overwritten
automatically; load canonical defaults, preserve the unread file, and emit a
debug-gated warning.

The existing plugin already keeps Leviathan-specific visual settings under the
Rack user directory and loads/saves them during plugin lifecycle. Theme uses a
dedicated persistence component and this canonical location:

```text
<Rack user>/Leviathan/theme.json
```

Writes must be debounced during live edits and atomic:

```text
write temporary file
→ flush/close
→ rename
```

to avoid corruption if Rack terminates during a save.

The initial debounce target is 750 ms after the last accepted live edit. Pointer
release after a drag, numeric/HEX field commit, preset apply/save/delete, reset,
Theme-widget destruction with a pending edit, and plugin shutdown force a
flush. Persistence runs on UI/lifecycle code, never the audio thread.

Atomic replacement must work in the Windows/MSYS2 release environment as well
as Linux. The persistence helper writes a same-directory temporary file, flushes
and closes it, then uses an overwrite-capable platform replacement. Startup
ignores incomplete temporary files when the canonical document is valid and may
recover a valid temporary file only when the canonical document is absent or
invalid. Save failures leave the active runtime theme untouched.

---

# 24. Integral Flux Crystal Removal

Integral Flux currently owns an interactive crystal control which directly toggles the plugin-global glass color cycle.

Once Theme becomes functional, the crystal-based global theme behavior must be retired.

Remove:

```text
PanelGlassTintState
panel glass progression
crystal color-cycle toggle
crystal progression preview state
crystal-specific global persistence
IntegralFluxLogoCrystalButton behavior
```

Integral Flux should no longer secretly control the appearance of unrelated modules.

The visual space occupied by the crystal may be repurposed artistically or simply returned to the panel design.

Animated/cycling themes are explicitly out of scope for V1.

They may later reappear as an intentional Theme feature.

## 24.1 Legacy settings migration

Current releases store the crystal cycle in
`<Rack user>/Leviathan/settings.json`. That animation cannot be mapped
deterministically onto three static semantic colors, so V1 uses a conservative
one-time migration:

1. If a valid `theme.json` exists, it always wins and legacy settings are not
   consulted for Theme state.
2. If `theme.json` does not exist, initialize the canonical Leviathan theme and
   eight empty user slots, regardless of whether the legacy cycle was enabled.
3. Write `theme.json` only after successful initialization or the first edit.
4. Do not delete or rewrite the legacy `settings.json` during migration.
5. Once all crystal code has been removed, no runtime code reads or writes the
   legacy crystal keys.

This deliberately preserves the old file for rollback while preventing an
arbitrary animation phase from becoming a permanent static palette. The first
V1 launch therefore retires an enabled crystal cycle and restores the canonical
appearance. This is an intentional, documented compatibility exception because
animated themes are a V1 non-goal.

Integral Flux is released. Removal must not reorder or remove any existing
parameter, input, output, or light ID. The crystal is a UI-only widget and may
be removed without changing those enums. The context-menu `Reset Crystal` entry
must also be removed when the legacy behavior is retired.

---

# 25. Module Integration Contract

A Leviathan module becomes theme-aware primarily through shared infrastructure.

Modules using `SplitPanelRenderer` should automatically receive:

```text
themed glass
themed labels
theme generation invalidation
```

after their SVG assets are semantically tagged.

Module code should generally **not** contain logic such as:

```cpp
if (theme == ...)
```

or:

```cpp
if (moduleName == ...)
```

for color selection.

The preferred contract is:

```text
SVG semantic role
→ shared renderer
→ ThemeService
```

---

# 26. Per-Module Migration

Each existing module requires an asset audit.

For each panel:

1. Identify generic decorative glass.
2. Identify glass representing Inputs.
3. Identify glass representing Outputs.
4. Identify optional Accent glass.
5. Retag appropriate SVG groups.
6. Identify label/glyph artwork which may be themeable.
7. Place such artwork in explicit `theme_text*` groups.
8. Confirm module title and branding remain outside theme groups.
9. Verify fractal texture response.
10. Compare default-theme rendering against pre-migration appearance.

Modules without meaningful Input/Output panel regions may retain generic glass.

The theme system must not force every piece of Leviathan artwork into one of the semantic roles.

## 26.1 Migration inventory

The migration is tracked by integration class rather than assuming every module
already uses identical rendering infrastructure:

| Integration class | Modules |
|---|---|
| Split panel + shared fractal overlay | Integral Flux, Proc, Temporal Deck, Undertow, Puffy, Crownstep, Bifurx, Wyrm, Iris, Nautiloid, Doorstop, Mandelwake |
| Split panel without shared fractal overlay | Deep Cache, Chromatide, Cantor |
| No current `SplitPanelRenderer` integration | TD.Scope, Sil, Chronomaw, Bulkhead, Umi |

Integral Flux, Proc, Temporal Deck, TD.Scope, and Undertow are released. Their
existing parameter/input/output/light ordering, patch serialization, and
canonical-theme behavior are compatibility constraints. Crownstep, Bifurx,
Wyrm, Sil, Chronomaw, and Bulkhead are unreleased and may be migrated more
freely, but still follow the shared rendering contract.

Before Phase 6 begins, this inventory must become a checked migration table with
one row per registered module and these columns:

```text
module
master SVG
runtime panel SVG
runtime labels SVG
split renderer yes/no
fractal overlay yes/no
glass roles selected
label roles selected
released yes/no
canonical reference captured
migration status
```

Modules outside `SplitPanelRenderer` do not block the first vertical slice, but
they do block a claim that the full free plugin has been migrated. A module may
be marked intentionally non-themeable when its artwork has no meaningful V1
semantic regions; that decision must be recorded rather than silently skipped.

---

# 27. Other Visual Elements

V1 scope is:

```text
Glass color
Opt-in labels/text
Fractal texture amount
```

The semantic role architecture intentionally allows later integration of:

```text
Magitek jack animation colors
jack glows
LEDs
signal-flow arrows
display annotations
selected dynamic graphics
```

The current Magitek2 system already distinguishes input and output default animation styles using purple-inward and cyan-outward behavior, so semantic theme integration there is a natural future extension.

These extensions should not block Theme V1.

---

# Future Appendix A — Cross-Plugin Architecture

Sections 28 through 35 are forward-looking constraints, not V1 acceptance
requirements. They exist to prevent V1 from closing off a future Premium
consumer. No bridge, exported symbol, shared file reader, polling loop, or
Premium fallback code should be implemented until a second plugin exists and
its concrete toolchains are known.

## 28. Cross-Plugin Architecture

The theme architecture must assume that a future plugin may exist with a different Rack plugin slug, for example conceptually:

```text
Leviathan
Leviathan Premium
```

No Premium plugin is required for initial implementation.

The base `Leviathan` plugin remains the theme authority.

Premium is a consumer.

---

## 29. Shared Source Contract

Theme data types and consumer logic should be written so they can later be compiled into both plugin binaries.

For example:

```text
theme/
    ThemeContract.hpp
    ThemePersistence.*
    ThemeConsumer.*
```

However:

> compiling the same C++ singleton into two plugin binaries does not create shared runtime state.

Each plugin receives its own static data.

Cross-plugin communication therefore requires an explicit bridge.

---

## 30. Cross-Plugin Runtime ABI — Future Phase

Rack exposes loaded plugins by slug and exposes the OS library handle on the loaded `Plugin` object. On Linux and macOS Rack loads plugins with `RTLD_LOCAL`, so unrelated plugin symbols are not simply merged into one global namespace.

A future Premium integration should therefore use an intentional, versioned C ABI exported by the base Leviathan plugin.

Conceptually:

```cpp
extern "C" {

struct LeviathanThemeSnapshotV1 {
    uint32_t structSize;
    uint32_t schemaVersion;

    // Fourth byte is reserved and must be zero. Four-byte color slots make
    // offsets explicit without exposing user-editable alpha.
    uint8_t inputRgbReserved[4];
    uint8_t outputRgbReserved[4];
    uint8_t accentRgbReserved[4];

    float textureAmount;

    uint64_t generation;
};

struct LeviathanThemeApiV1 {
    uint32_t abiVersion;
    uint32_t structSize;

    // 1 = success, 0 = unavailable/incompatible.
    uint32_t (*readSnapshot)(
        LeviathanThemeSnapshotV1* destination);
};

const LeviathanThemeApiV1*
leviathanThemeGetApiV1();

}
```

Exact names are provisional.

Before this becomes a real ABI, the bridge document must freeze member offsets,
sizes, alignment, byte order, float representation, calling convention, symbol
visibility, and Windows/macOS/Linux lookup behavior. Both producers and
consumers must use compile-time size/offset assertions. C++ `bool` is not used
across the boundary.

---

## 31. ABI Rules

The cross-plugin ABI must use only stable C-compatible data.

Do not expose across the binary boundary:

```text
std::string
std::vector
std::shared_ptr
NVGcolor
C++ classes
virtual interfaces
exceptions
```

Prefer:

```text
fixed-width integers
floats
POD structs
function pointers
explicit structSize
explicit ABI version
```

This minimizes compiler/runtime coupling between the free and Premium plugin packages.

---

## 32. Premium Discovery

Future Premium plugin behavior:

```text
Premium initializes
       ↓
look for loaded plugin slug "Leviathan"
       ↓
if unavailable:
    use local canonical/default snapshot
       ↓
retry discovery lazily from UI-side runtime
       ↓
if available:
    resolve leviathanThemeGetApiV1()
       ↓
read snapshot
       ↓
observe generation
```

This avoids depending on plugin load order.

Premium modules must remain functional if the base Leviathan plugin is absent.

Only customization availability changes.

---

## 33. Cross-Plugin Live Updates

Premium does not need callback subscriptions in the first bridge implementation.

A simpler model is sufficient:

```text
Theme edits
→ base generation increments

Premium UI step
→ compare remote generation
→ copy snapshot only when generation changed
→ dirty Premium framebuffers
```

This is:

* simple
* read-only
* low-risk
* naturally compatible with existing framebuffer-generation logic

No cross-plugin communication should occur on the audio thread.

---

## 34. Cross-Plugin Persistence Fallback

The shared user theme file should remain the durable representation of the currently selected theme.

A future Premium plugin may load this file directly at startup as a fallback.

When the live ABI bridge is available:

```text
ABI snapshot = runtime source of truth
shared file = durable source of truth
```

The Theme UI should not write the shared file for every individual mouse-motion event.

Live dragging is handled through the runtime service.

Persistence may occur:

```text
on edit commit
after a short debounce
on preset apply
on plugin shutdown
```

This keeps live color manipulation responsive without turning a color-picker drag into a stream of filesystem writes.

---

## 35. Plugin Settings Relationship

Rack supports plugin-scope settings via `settingsToJson()` and `settingsFromJson()`.

Those settings are naturally associated with an individual plugin slug.

Because Theme is intended eventually to span multiple Leviathan-family plugins, the dedicated shared Leviathan theme document is the preferred canonical cross-plugin persistence format.

Rack plugin settings may still be used for private plugin-specific preferences unrelated to shared theme state.

---

# 36. Threading

Theme mutation is a UI concern.

Theme controls should not modify state directly from an audio `process()` method.

Consumers need only:

```text
copy coherent current state
compare relevant generation
request framebuffer redraw
```

V1 uses a small mutex around snapshot/generation copy and mutation. Disk I/O,
SVG parsing, framebuffer invalidation, and widget calls must occur after
releasing that mutex. Theme updates occur at human UI rates, so a more complex
lock-free publisher is not justified. If a future bridge requires a different
publication mechanism, it may replace the internal implementation without
changing the copied `ThemeState` contract.

## 36.1 Graphics-context lifecycle

ThemeService is graphics-agnostic. It must never own NanoVG handles,
`NVGcontext*`, GL objects, framebuffers, or `window::Svg` instances.

Themed rendering widgets must follow the repository lifecycle standard:

* use `NvgGraphicsLifecycle.hpp` for NanoVG image ownership and validation
* never delete an image handle from a different `NVGcontext*`
* clear context-bound handles when the context changes
* validate persistent handles before reuse
* lazily rebuild themed SVG/image/framebuffer resources in draw/step-time context
* avoid destructor-time GL cleanup
* keep weak or widget-owned SVG caches so plugin-static destruction cannot
  release Rack window resources after graphics teardown

Context recreation is independent of Theme generations. A widget whose context
resources are invalid must rebuild them even when the theme has not changed.
Conversely, a context change must not mutate or persist ThemeService state.

---

# 37. Performance Requirements

Steady state:

```text
No theme changes
→ no framebuffer invalidation
→ no theme-specific continuous rendering work
```

During color-picker drag:

```text
Theme changes
→ one generation increment per accepted UI update
→ glass and affected semantic label layers redraw
→ semantic fractal palettes reuse source fields and re-palette as needed
```

After drag:

```text
framebuffers cached again
```

Texture amount changes must not regenerate fractal fields merely because compositing opacity changed.

Unrelated domains must remain cached: a Texture Amount edit does not rebuild
glass or labels, a preset rename does not redraw modules, and a color edit does
not regenerate fractal iteration. UI-frame coalescing and the persistence
debounce prevent pointer-motion event rate from becoming redraw or filesystem
write rate.

Cross-plugin theme checks must never occur in DSP processing.

---

# 38. Browser Preview Behavior

Module Browser previews should use the currently active global theme where practical.

This ensures the browser visually matches modules that will be instantiated.

If preview-generation context makes runtime theming unsafe or expensive, canonical defaults are an acceptable temporary fallback during early implementation.

The final desired behavior is global-theme consistency.

At minimum, previews constructed after a theme change must use the current
state. V1 is not required to invalidate Rack's already-captured browser-preview
cache if Rack exposes no safe plugin API for doing so. This limitation must not
affect live module instances.

---

# 39. Multiple Theme Modules

Multiple Theme module instances are explicitly supported.

They behave as synchronized editors of the same global state.

Example:

```text
Theme A selects red Input
        ↓
ThemeService changes
        ↓
All Leviathan modules redraw
        ↓
Theme B UI reflects red Input
```

There is no master/slave election.

There is no conflict because the modules do not own independent themes.

---

# 40. Initial Non-Goals

V1 does not require:

* CV control over theme colors
* audio-rate theme modulation
* per-module independent themes
* per-patch automatic theme takeover
* animated theme cycling
* separate Input/Output texture strengths
* user-editable glass reflection amount
* user-editable glow strength
* arbitrary skin replacement
* Premium plugin implementation
* full recoloring of every Leviathan widget
* alpha editing
* unlimited preset filesystem browsing

These may be revisited after the core system is experienced in use.

---

# 41. Implementation Phases

## Phase 0 — Representative Vertical Slice

Before broad infrastructure or asset migration, select one representative
unreleased module with split panel artwork, semantic Input and Output regions,
outlined labels, gradients/transforms, and the shared fractal overlay. Bifurx or
Wyrm are suitable candidates.

Implement the thinnest production-quality path needed to prove:

* copied ThemeService state and domain generations
* exact semantic ancestor parsing for rects and paths
* canonical-default glass identity
* one semantic label layer using the specified color transform
* Texture Amount composition without fractal iteration
* semantic-color re-palettization from a reused source field
* one-time relevant framebuffer invalidation
* graphics-context destruction and lazy recreation
* master SVG regeneration workflow

Acceptance:

* canonical captures match the pre-theme reference within the chosen tolerance
* Input and Output edits are visibly independent
* generic artwork and titles do not change
* Texture Amount edits never increment fractal-source generation
* closing/reopening the Rack or DAW window restores the themed appearance
* the filtered-label implementation decision and accepted canonical constants
  are recorded in this document

Do not begin full module migration until this phase is accepted. Code from the
vertical slice should become shared infrastructure rather than a module-local
prototype.

## Phase 1 — Theme Foundation

Implement:

* `ThemeRole`
* theme data structures
* `ThemeService`
* generation tracking
* persistence schema
* canonical defaults
* preset data model
* exact legacy settings migration

No Theme module UI is required yet.

Acceptance:

* programmatic color change can recolor theme-aware glass
* changing theme dirties cached surfaces once
* current default appearance remains intact
* equal writes do not increment generations
* color and surface generations invalidate only their consumers

---

## Phase 2 — Semantic Glass

Implement:

* `glass_input`
* `glass_output`
* `glass_accent`
* parser semantic-role support
* theme-aware glass material color resolution

Migrate a small representative group first:

```text
Bifurx
Wyrm
```

Released modules follow only after the shared path passes the vertical-slice
tests.

Acceptance:

* Input and Output glass can receive independent colors
* generic glass remains authored
* existing special glare remains functional

---

## Phase 3 — Theme Module UI

Implementation checkpoint (2026-08-15): the Rack-visible Phase 3 MVP is in
place. It provides the parameterless global module, role selection, live HSV
picker, Texture Amount, Swap, Reset Theme, the four factory presets, durable
preference writes, and synchronization between multiple editor instances. The
typed HSV/RGB/HEX fields, Reset Role, and user-preset management remain required
before Phase 3 and Theme V1 can be declared complete.

Implement:

* Theme model/module registration
* Input / Output / Accent selector
* HSV picker
* RGB numeric editing
* HEX editing
* Texture slider
* Swap
* Reset
* factory presets
* exactly 8 stable user preset slots
* live synchronization between multiple Theme instances

Acceptance:

* editing Theme visibly updates existing migrated modules live
* removing Theme leaves selected theme active
* re-adding Theme recalls current state
* restart preserves active theme and custom presets
* factory preset colors and reference captures are frozen in this document
* focused drafts and competing Theme instances follow Section 6.2

---

## Phase 4 — Themed Labels

Implement:

* semantic label groups
* layered/cached labels renderer
* role-aware tint masks
* title/branding exclusion-by-default
* module asset migration

Acceptance:

* only explicitly tagged label artwork changes
* module titles remain canonical
* default theme closely matches original label appearance
* theme changes redraw label cache once

---

## Phase 5 — Fractal Surface Control

Implement:

* Texture Amount integration
* 0–200% UI range
* canonical 100% = current 0.22 compositing strength
* zero-texture draw bypass
* semantic palette refresh from cached source fields

Acceptance:

* 0% removes visible texture
* 100% matches existing appearance
* changing texture does not regenerate fractal fields unnecessarily
* changing a semantic color re-palettes but does not rerun fractal iteration

---

## Phase 6 — Full Free-Plugin Migration

Audit all Leviathan modules.

Create and maintain the Section 26.1 migration table before editing assets.

For each:

```text
glass semantics
label semantics
texture behavior
default-theme visual regression
```

Remove remaining Integral Flux crystal behavior.

Acceptance:

* all intended free Leviathan modules participate consistently
* no unintended branding/title tinting
* no meaningful steady-state rendering regression
* every registered module is migrated or explicitly recorded as intentionally
  non-themeable in V1

---

## Future Phase 7 — Premium Bridge

Not part of V1. Deferred until a second Leviathan plugin actually exists.

Implement:

* versioned C ABI
* base-plugin discovery
* exported Theme API
* Premium-side consumer
* load-order-independent lazy connection
* shared persistence fallback
* Premium theme-generation invalidation

Acceptance:

* Theme module in free Leviathan modifies both free and Premium modules live
* Premium remains functional without free Leviathan installed
* ABI version mismatch fails safely
* no direct C++ object sharing between plugin binaries

---

# 42. Testing Requirements

## Unit tests

Theme:

* RGB ↔ HSV conversion
* HEX parsing
* clamping
* preset serialization
* preset migration
* theme serialization
* role resolution
* default restoration
* snapshot canonicalization and equality
* non-finite Texture Amount handling
* no-op writes do not increment generations
* color/surface/preset generation isolation
* eight-slot identity and normalization
* focused editor conversion edge cases

SVG:

* semantic ancestor detection
* generic glass
* Input glass
* Output glass
* Accent glass
* theme text groups
* nested/conflicting group behavior
* exact matching rejects near-miss names
* semantic groups survive label splitting and text outlining
* fill, stroke, gradient, opacity, and transform preservation
* canonical label recolor is an identity transform within tolerance

## Rendering tests

Capture reference screenshots for:

```text
canonical default
custom Input
custom Output
custom Accent
Texture 0%
Texture 100%
Texture 200%
```

Check module titles remain unaffected.

Reference captures must record Rack version, operating system, pixel ratio,
zoom, framebuffer oversampling, module revision, theme JSON, and capture crop.
Phase 0 selects and records a numeric pixel-difference tolerance. A changed
golden image is accepted only with an intentional visual-review note; tests must
not silently regenerate baselines.

## Lifecycle tests

Test:

```text
Rack start
Rack shutdown
module creation
module deletion
multiple Theme instances
module-browser previews
preset save/reload
invalid theme file
future-schema theme file
interrupted atomic save / leftover temporary file
old settings migration
focused edit while a second Theme module commits
graphics context destroy/recreate without a theme edit
```

## Source-level and build validation

Theme unit, parser, persistence, and focused integration tests must be part of
`make test-fast`. In WSL/WSL-like environments, those tests and focused test
binaries are authoritative; full `plugin.so` linking is not. The final release
plugin link is verified in the Windows/MSYS2 VCV Rack toolchain. On real Linux
with a matching Rack SDK, the full plugin build is also required.

## Future Premium tests

Test both load orders:

```text
Leviathan → Premium
Premium → Leviathan
```

and:

```text
Premium without Leviathan
```

---

# 43. Architectural End State

The intended final architecture is:

```text
                         ┌─────────────────┐
                         │  Theme Module   │
                         │   UI / Editor   │
                         └────────┬────────┘
                                  │
                                  ▼
                         ┌─────────────────┐
                         │  ThemeService   │
                         │                 │
                         │ Input           │
                         │ Output          │
                         │ Accent          │
                         │ Texture Amount  │
                         │ Generation      │
                         └────────┬────────┘
                                  │
                  ┌───────────────┼────────────────┐
                  │               │                │
                  ▼               ▼                ▼
               Glass          Labels/Text      Fractal
                  │               │                │
                  └───────────────┼────────────────┘
                                  │
                                  ▼
                         Free Leviathan Modules

                                  │
                         Future C ABI Bridge
                                  │
                                  ▼

                         Premium ThemeConsumer
                                  │
                  ┌───────────────┼────────────────┐
                  ▼               ▼                ▼
               Glass          Labels/Text      Fractal
                                  │
                                  ▼
                       Premium Leviathan Modules
```

The governing principle is:

> **Theme supplies semantic visual intent. Renderers supply material character. SVG assets declare which artwork participates. Plugin binaries communicate through an explicit versioned contract.**

This allows Leviathan to become visually customizable without sacrificing the authored identity of individual modules, and establishes the foundation for Theme to become the first true Leviathan meta-module spanning more than one plugin package.
