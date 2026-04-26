# Display Challenges (Current)

## Scope
This document summarizes active technical challenges in Bifurx preview/display behavior.

## 1) Bifurx peak marker semantics vs visual expectations

### Problem
The two marker dots are intended to represent the two core center frequencies, but users visually interpret them as "top-of-peak" markers.

### Why this is hard
- In several modes (especially dual-notch), `freqA` / `freqB` are not local maxima in the magnitude curve.
- A marker tied to center frequency can be technically correct but visually surprising when the local response at that frequency is a dip.
- A marker tied to local maxima can look smoother in some modes but becomes unstable/snappy in notch-heavy modes.

### Current status
- Marker X is tied to core center frequencies for stability.
- Marker Y is evaluated from the preview transfer at that same frequency, then clamped to visible bounds.
- Marker drawing is suppressed if computed point is effectively below plot floor.

## 2) Dual-notch instability in perceived smoothness

### Problem
Dual-notch mode appears less smooth during frequency changes than other modes.

### Why this is hard
- Notch responses produce steep local geometry and non-intuitive "feature motion" in the displayed curve.
- Small parameter changes can yield disproportionate visual movement when looking at one or two highlighted points.
- The curve display is a sampled approximation (`kCurvePointCount`) and the visual path is smoothed over time, which can differ from how users expect "continuous" motion.

### Current status
- Marker lookup no longer hunts nearby local maxima (which caused snapping).
- Remaining roughness is mostly semantic/perceptual, not a single math bug.

## 3) Bottom strip layout tension (plot height vs labels)

### Problem
We need enough vertical room for frequency labels while maximizing visible graph area.

### Why this is hard
- Plot and label strip share a fixed-height widget.
- If label band is too tall, graph looks compressed.
- If label band is too short, labels clip/overlap and guide lines become cramped.

### Current status
- Label strip was reduced and graph bottom brought down.
- Plot rendering is clipped above the label strip so curve/fills do not intrude into text area.

## 4) Core tradeoff summary

- "Physically/algorithmically accurate marker semantics" and "intuitively peak-looking visuals" are not always the same in complex modes.
- The biggest remaining challenge is not raw performance; it is choosing a marker semantics that reads correctly across all modes, especially dual-notch.
- Current implementation is stable and bounded, but may still require mode-specific marker presentation if user expectation remains "always on visible peak tops."
