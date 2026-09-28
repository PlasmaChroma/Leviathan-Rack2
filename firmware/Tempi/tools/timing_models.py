"""Recovered digital timing rules; no physical tick-rate or full-board claim.

See docs/TIMING_RECONSTRUCTION.md for domains, addresses and limitations.
"""
from dataclasses import dataclass, replace

TEMPO_TABLE = (200000, 180000, 130000, 80000, 70000, 40000, 12000,
               10000, 8500, 6000, 5000, 3500, 2500, 1750, 1000, 250)


def tempo_control(adc, previous_adc, halfperiod, thresholds):
    """627C: return (candidate full period, retained ADC, movement flag).

    Domain: 10-bit ADC or 0xffff sentinel, ascending 16-entry calibration.
    The low-end doubling and divide-before-multiply are intentional.
    """
    margin = min(10, previous_adc // 50 + 2)
    if adc == 0xffff or max(0, previous_adc-margin) <= adc <= previous_adc+margin:
        return 2*halfperiod, previous_adc, False
    if adc <= thresholds[0] + margin:
        period = 2*TEMPO_TABLE[0]
    elif adc > thresholds[-1]:
        period = TEMPO_TABLE[-1]
    elif adc in thresholds:
        period = TEMPO_TABLE[thresholds.index(adc)]
    else:
        upper = next(i for i, t in enumerate(thresholds) if t > adc)
        lower = upper-1
        slope = ((TEMPO_TABLE[lower]-TEMPO_TABLE[upper]) //
                 (thresholds[upper]-thresholds[lower]))
        period = TEMPO_TABLE[upper] + slope*(thresholds[upper]-adc)
    return period, adc, True


@dataclass
class LeadingState:
    requested: int = 8000
    current: int = 8000
    countdown: int = 4000
    elapsed: int = 0
    measuring: int = 1
    accepted: int = 0
    capture: int = 0
    edge_remaining: int = 0
    edge_level: int = 0
    dirty: int = 0


def leading_capture_service(state):
    """4B16 capture path, Tap enabled and Follow setting unchanged.

    Nonnegative signed-32-bit elapsed/capture/remainder, valid requested H.
    No ADC candidate or concurrent interrupt is injected in this model.
    """
    s = replace(state)
    if s.elapsed >= 0xffffff or (s.accepted and s.elapsed > 15*s.requested):
        s.measuring = s.elapsed = s.accepted = 0
    if not s.capture:
        return s
    interval = s.capture
    s.capture = 0
    if interval < 399:
        s.measuring = s.elapsed = 0
        return s
    s.accepted = 1
    new = min(0xffffff, max(200, interval//2))
    if new != s.requested:
        s.requested = s.current = new
        s.countdown = min(s.countdown, new)
        s.dirty = 1
        forward = new-s.edge_remaining
        while forward > new//2:
            forward //= 2
        backward = s.edge_remaining
        while backward > new//2:
            backward //= 2
        s.current = (s.current + (forward if s.edge_level else -backward)) & 0xffffffff
    s.edge_remaining = 0
    return s
