# SINC Optimization Notes (Future Work)

Date: 2026-04-02
Scope: potential CPU/perf improvements for `TemporalDeck` high-quality scratch interpolation (`SINC` mode).

## Why This Matters

- `SINC` mode is the most expensive interpolation path in scratching.
- Current path computes windowed-sinc weights at runtime and accumulates many tap MACs per sample per channel.
- This can spike CPU under heavy scratch motion.

## Highest-Value Opportunities

### 1) SIMD in SINC Tap Accumulation
- Vectorize tap multiply-accumulate loop in `TemporalDeckBuffer::readSinc`.
- Compute shared weight vectors once, apply to L/R with SIMD accumulators.
- Expected impact: moderate-to-high speedup in the hot loop.

### 2) Polyphase / Precomputed Kernel Table
- Precompute windowed-sinc weights by fractional phase bins.
- Replace per-sample `sin/cos` window math with table lookup + dot product.
- Expected impact: large reduction in runtime math cost.

### 3) SIMD + Polyphase Together
- Best long-term path for making `SINC` practical at high motion and polyphony.
- Table-driven weights + SIMD dot products should reduce both average and worst-case CPU.

### 4) Optional Dynamic Quality Guardrail
- During extreme read velocity, reduce kernel radius or temporarily fall back to a cheaper interpolation mode.
- Purpose: prevent CPU spikes while preserving quality in normal use.
- Should be opt-in or tuned carefully to avoid audible mode churn.

## Implementation Order (Recommended)

1. Instrument current `SINC` cost with basic timing counters.
2. Implement SIMD tap accumulation without changing kernel behavior.
3. Add regression/perceptual checks (A/B against baseline).
4. Introduce polyphase table with strict output comparison tolerance.
5. Optional dynamic guardrail once baseline quality/perf is validated.

## Guardrails

- Keep behavior stable by default; optimize internals first.
- Validate with:
  - existing unit/integration tests
  - scratch-feel manual checks
  - CPU profile before/after in representative patches
- Avoid adding branchy logic in the audio hot loop unless measured beneficial.

## Non-Goals (For Initial Pass)

- Major tonal redesign of interpolation output.
- Changing UI defaults or exposing many new user-facing options up front.

