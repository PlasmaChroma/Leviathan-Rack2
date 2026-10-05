"""Instruction-derived UI actions, separate from GPIO sampling and persistence.

These are dispatch and action-boundary definitions, not a complete UI handler.
Compose in firmware order; each helper documents its input boundary. Array and
shared actions use the hold counters resulting from Shift/clock processing.
"""
from dataclasses import dataclass, replace
from sp67_reference import f32, from_bits


def shift_release_finish(side, elapsed, display, other_gesture):
    """Common release completion after normal admission/selection effects.

    Reset this side's elapsed counter. Preserve the opposite display only
    when its gesture is consumed (4). Selection has just consumed that
    opposite gesture, so its display survives this completion as well.
    Own hold, gesture and display are untouched by this operation.
    """
    if side not in (0,1) or len(elapsed)!=2 or len(display)!=2:
        raise ValueError('side and paired elapsed/display state required')
    elapsed=list(elapsed);display=list(display)
    elapsed[side]=0
    if other_gesture!=4:display[1-side]=0
    return tuple(elapsed),tuple(display)


@dataclass(frozen=True)
class PassiveShiftState:
    hold: int = 0
    auxiliary: int = 0
    phase: float = 0.0
    linear_setting: int = 0
    linear_runtime: int = 0
    scan: int = 0
    dimension1: int = 8
    dimension2: int = 8
    countdown: int = 0


def shift_press_action(state):
    """Press tail: reset hold and non-scan auxiliary phase, retain linear state."""
    return replace(state,hold=0,phase=state.phase if state.auxiliary in (0,5) else 1.0)


def shift_idle_action(side,state):
    """Idle tail after dispatch; countdown is already ticked/clamped.

    A tests its supplied countdown register for nonzero; B tests signed >0.
    Both update runtime linear state by sign-extending the saved halfword.
    """
    if side not in (0,1):raise ValueError('side must be 0 or 1')
    def signed(v):
        v&=0xffffffff
        return v-0x100000000 if v&0x80000000 else v
    active=(state.countdown&0xffffffff)!=0 if side==0 else signed(state.countdown)>0
    scan=state.scan
    if active and state.auxiliary in (0,5):
        scan=(scan+1)&0xffffffff
        if signed(scan)>=signed(state.dimension1*state.dimension2):scan=0
    linear=state.linear_setting&0xffff
    if linear&0x8000:linear-=0x10000
    return replace(state,hold=0,scan=scan,linear_runtime=linear&0xffffffff)


@dataclass(frozen=True)
class ConsumedShiftRoute:
    previous: int
    gesture: int
    pending_clock: int
    route: str


def shift_dispatch(side, current, previous, gesture, pending_clock):
    """Shift edge/clock dispatch, before the selected branch's effects.

    The returned record has the same fields as consumed_shift_route. The
    release route includes clock-triggered synthetic releases; calibration
    and hold-dependent admission are evaluated by the subsequent branch.
    """
    if side not in (0,1) or len(current)!=5 or any(x not in (0,1) for x in current) or previous not in (0,1):
        raise ValueError('side, previous Shift and five boolean inputs required')
    if gesture==4:
        return consumed_shift_route(side,current,pending_clock)
    now=int(current[side])
    clock=pending_clock==1
    pending=0 if clock else pending_clock
    if now!=previous:
        return ConsumedShiftRoute(now,3 if now else 2,pending,'press' if now else 'release')
    if clock and (side==0 or now):
        return ConsumedShiftRoute(now,2,pending,'release')
    return ConsumedShiftRoute(now,now,pending,'hold' if now else 'idle')


def consumed_shift_route(side, current, pending_clock):
    """Dispatch for gesture==4, before hold/release/idle effects.

    current is five boolean panel inputs: Shift A/B, Array A/B, shared.
    Only a pending clock word exactly equal to one is consumed.
    """
    if side not in (0,1) or len(current)!=5 or any(x not in (0,1) for x in current):
        raise ValueError('side and five boolean panel inputs required')
    clock=pending_clock==1
    pending=0 if clock else pending_clock
    if any(current):
        return ConsumedShiftRoute(int(current[side]),4,pending,'hold')
    if side==0 and clock:
        return ConsumedShiftRoute(0,2,pending,'release')
    return ConsumedShiftRoute(0,0,pending,'idle')


def shift_release_action(hold, other_hold):
    """Normal (non-calibration) release decision; counters observed now.

    Returns 'select_other', 'clock_self', or 'none'. Enclosing code must
    establish a release and handle consumed gestures before calling this.
    A runs before B; apply A effects before obtaining B's counter inputs.
    """
    hold &= 0xffffffff
    other_hold &= 0xffffffff
    if other_hold > 49:
        return 'select_other' if 51 <= hold <= 1499 else 'none'
    return 'clock_self' if hold < 500 else 'none'


@dataclass(frozen=True)
class LongHoldResult:
    hold: int
    linear: int
    dirty: int
    action: str = 'none'


def long_hold_step(hold, other_hold, linear, dirty=0):
    """Held-Shift increment and long action at 0x0802e0cc / 0x0802dfb8.

    other_hold is the value at this side's processing time (A precedes B).
    'reset' requests factory replacement plus save; this result stops before
    those calls. Firmware writes hold=1601 only after the reset/save returns.
    """
    sampled=(hold+1)&0xffffffff
    if sampled>1600:
        return LongHoldResult(1600,linear,dirty)
    if sampled<=1500 or sampled<=other_hold:
        return LongHoldResult(sampled,linear,dirty)
    if other_hold>300:
        return LongHoldResult(sampled,linear,dirty,'reset')
    return LongHoldResult(1601,(1-linear)&0xffff,1,'toggle')


@dataclass(frozen=True)
class ArraySelectionState:
    slot: int = 0
    scan: int = 0
    slot_mirror: int = 0
    hold: int = 0
    gesture: int = 0
    display: tuple = (0,0)  # selection display flags A, B
    dirty: int = 0


def array_selection_action(side, state):
    """Selection body 0x0802e4ec (A) / 0x0802dc50 (B).

    Admission thresholds and subsequent elapsed/Shift handling are external.
    Scan reset and dirty marking depend on the saved mirror, not the old slot.
    """
    if side not in ('A','B'):
        raise ValueError('side must be A or B')
    slot=(state.slot+1)&15
    changed=slot!=state.slot_mirror
    return replace(state,slot=slot,slot_mirror=slot,hold=1601,gesture=4,
                   display=(1,0) if side=='A' else (0,1),
                   scan=0 if changed else state.scan,
                   dirty=1 if changed else state.dirty)


@dataclass(frozen=True)
class AdmittedClockState:
    dimension1: int = 8
    dimension2: int = 8
    scan: int = 0
    capture: int = 0
    auxiliary: int = 0
    elapsed: int = 0
    rate: float = 0.
    period_mirror: int = 0
    dirty: int = 0
    countdown: int = 0
    policy: int = 0


def admitted_clock_action(state, short_rates=None):
    """Effects after admission, starting at 0x0802e2ca / 0x0802e37e.

    Elapsed already includes this callback's timer tick. Optional short_rates supplies
    actual table contents; omission uses the verified initialized table law.
    Admission, event clearing, display flags and Shift counters remain external.
    """
    s=state
    if not 0<=s.elapsed<=0xffffffff:
        raise ValueError('elapsed must be a uint32 word')
    def signed(value):
        value&=0xffffffff
        return value-0x100000000 if value&0x80000000 else value
    count=1 if s.capture==1 else ((signed(s.dimension1-1)>>6)+1)&0xffffffff
    if s.auxiliary in (0,5):
        scan=(s.scan+1)&0xffffffff
        if signed(scan)>=signed(s.dimension1*s.dimension2):scan=0
        return replace(s,countdown=count,policy=1,scan=scan,elapsed=0)
    rate=s.rate
    if 1<=s.elapsed<=1023:
        rate=f32(short_rates[s.elapsed]) if short_rates is not None else f32((1/64)/s.elapsed)
    elif 1024<=s.elapsed<=5999:
        rate=f32((1/64)/s.elapsed)
    # VCVT.S32.F32 saturates positive overflow at INT32_MAX.
    mirror=min(0x7fffffff,int(f32(f32(s.elapsed)*from_bits(0x3faaaaaa))))
    return replace(s,countdown=count,policy=1,elapsed=0,rate=rate,period_mirror=mirror,
                   dirty=1 if mirror!=s.period_mirror else s.dirty)


@dataclass(frozen=True)
class ClockTimerState:
    elapsed: tuple = (0,0)
    countdown: tuple = (0,0)
    policy: tuple = (0,0)


def clock_timer_tick(state, capture):
    """0x0802d946..0x0802d99e, before Shift/clock admission.

    Preserve uint32 wrap followed by the firmware's signed countdown clamp.
    Policy expiry is strict >6000 after increment, and only when capture is 0.
    An admitted clock later in the same callback may replace these results.
    """
    elapsed=tuple((v+1)&0xffffffff for v in state.elapsed)
    countdown=tuple((v-1)&0xffffffff for v in state.countdown)
    countdown=tuple(0 if v&0x80000000 else v for v in countdown)
    policy=tuple(0 if elapsed[i]>6000 and capture[i]==0 else state.policy[i]
                 for i in range(2))
    return ClockTimerState(elapsed,countdown,policy)


@dataclass(frozen=True)
class ButtonEdgeState:
    previous: tuple = (0,0,0)  # Array A, Array B, shared
    pending: tuple = (0,0,0)
    held: tuple = (0,0,0)      # independent button counters, not Shift counters


def sample_button_edges(state, current):
    """0x0802dce4..0x0802dd24; boolean snapshots, before action dispatch.

    Changed inputs reset their button counters. Rising inputs set pending=1;
    falling inputs do not cancel an event already pending. Unchanged inputs
    preserve all three fields. Shift processing is outside this function.
    """
    if len(current)!=3 or any(v not in (0,1) for v in current):
        raise ValueError('current must contain three boolean button snapshots')
    previous=list(state.previous);pending=list(state.pending);held=list(state.held)
    for i,value in enumerate(current):
        if value!=previous[i]:
            previous[i]=value;held[i]=0
            if value:pending[i]=1
    return ButtonEdgeState(tuple(previous),tuple(pending),tuple(held))


@dataclass(frozen=True)
class ArrayButtonState:
    raw_sao: int = 0
    cached_sao: int = 0
    engine: int = 0
    capture: int = 0
    hold: int = 0
    gesture: int = 0
    lf: int = 0
    descriptor: tuple = (0, 8, 8, 1)  # coefficient offset, dimensions, save marker
    cursor: int = 0                   # coefficient offset, not bytes
    peak: float = 0.
    policy: int = 0
    engine_mirror: int = 0
    lf_mirror: int = 0
    dirty: int = 0


def array_button_action(side, state):
    """Consume one Array rising edge at 0x0802dd60 (A) / 0x0802ddce (B).

    Returns a new state. Preserves the raw/cached mode distinction and uint32
    stop-length subtraction. The caller owns selection, GPIO/event clearing,
    countdowns, media operations and safe host allocation for invalid lengths.
    Ordinary boolean flags and engine values 0..2 are the tested domain.
    """
    if side not in ('A', 'B'):
        raise ValueError('side must be A or B')
    s=state
    if not s.raw_sao:
        if s.hold<=50:
            return s if s.capture else replace(s,raw_sao=1,engine=0)
        s=replace(s,hold=1601,gesture=4)
        # A checks the cached mode here; B's raw-SAM branch does not.
        if side=='A' and s.cached_sao:
            return s
        offset,d1,d2,marker=s.descriptor
        if s.capture:
            length=((s.cursor-offset)&0xffffffff)>>6
            return replace(s,capture=0,descriptor=(offset,length,d2,marker),
                           policy=0 if side=='A' else s.policy)
        return replace(s,capture=1,cursor=offset,peak=0.,
                       descriptor=(offset,1024,1,0))
    if s.capture:
        return s
    if s.hold>50:
        lf=int(not s.lf)
        s=replace(s,hold=1601,lf=lf,lf_mirror=lf,dirty=1)
    elif s.engine==2:
        s=replace(s,raw_sao=0,engine=0,policy=0)
    else:
        s=replace(s,raw_sao=1,engine=s.engine+1)
    if s.engine_mirror!=s.engine:
        s=replace(s,engine_mirror=s.engine,dirty=1)
    return s


@dataclass(frozen=True)
class SharedButtonState:
    holds: tuple = (0,0)
    gestures: tuple = (0,0)
    cached_sao: tuple = (0,0)
    auxiliary: tuple = (0,0)
    auxiliary_mirrors: tuple = (0,0)
    interaction: int = 0
    interaction_mirror: int = 0
    dirty: int = 0


def shared_button_action(state):
    """One shared rising edge at 0x0802de3a, after both Array actions.

    The tested domain is auxiliary values 0..5, interaction 0..2 and boolean
    cached modes. This function does not sample or clear pending GPIO events.
    """
    s=state
    side=0 if s.holds[0]>50 else 1 if s.holds[1]>50 else None
    if side is None:
        value=(s.interaction+1)%3
        return replace(s,interaction=value,interaction_mirror=value,
                       dirty=1 if value!=s.interaction_mirror else s.dirty)
    holds=list(s.holds);gestures=list(s.gestures)
    aux=list(s.auxiliary);mirrors=list(s.auxiliary_mirrors)
    holds[side]=1601;gestures[side]=4
    value=(aux[side]+1)%6
    if s.cached_sao[side]==1 and value==0:value=1
    aux[side]=value
    dirty=1 if mirrors[side]!=value else s.dirty
    mirrors[side]=value
    return replace(s,holds=tuple(holds),gestures=tuple(gestures),
                   auxiliary=tuple(aux),auxiliary_mirrors=tuple(mirrors),dirty=dirty)


def button_actions(a, b, shared, pending):
    """Compose pending A Array, B Array, shared actions in firmware order.

    Returns (a, b, shared). ``pending`` is three booleans. Side states own
    hold/gesture/cached-mode values; ``shared.dirty`` is the authoritative input
    dirty flag. All copies of these overlapping fields are synchronized on
    return. This starts after Shift/clock/selection processing, not at GPIO read.
    """
    if len(pending)!=3:
        raise ValueError('pending must contain A Array, B Array, shared flags')
    sides=[a,b];dirty=shared.dirty
    for side in range(2):
        sides[side]=replace(sides[side],dirty=dirty)
        if pending[side]:sides[side]=array_button_action('AB'[side],sides[side])
        dirty=sides[side].dirty
    shared=replace(shared,holds=tuple(s.hold for s in sides),
                   gestures=tuple(s.gesture for s in sides),
                   cached_sao=tuple(s.cached_sao for s in sides),dirty=dirty)
    if pending[2]:shared=shared_button_action(shared)
    sides=[replace(s,hold=shared.holds[i],gesture=shared.gestures[i],dirty=shared.dirty)
           for i,s in enumerate(sides)]
    return sides[0],sides[1],shared


def button_inputs(a, b, shared, edges, current):
    """Sample Array/shared inputs, dispatch pending actions, clear consumed events.

    This corresponds to the handler tail from 0x0802dce4 to return. The caller
    must already have processed Shift, clocks and selection. Pending flags are
    restricted to 0/1 here, matching the tested reachable event domain.
    """
    if any(v not in (0,1) for v in edges.pending):
        raise ValueError('pending events must be boolean flags')
    sampled=sample_button_edges(edges,current)
    a,b,shared=button_actions(a,b,shared,sampled.pending)
    return a,b,shared,replace(sampled,pending=(0,0,0))


@dataclass(frozen=True)
class ShiftSideState:
    previous: int = 0
    gesture: int = 0
    pending_clock: int = 0
    hold: int = 0
    linear_setting: int = 0
    linear_runtime: int = 0
    slot: int = 0
    slot_mirror: int = 0
    scan: int = 0
    capture: int = 0
    auxiliary: int = 0
    phase: float = 0.0
    elapsed: int = 0
    rate: float = 0.0
    period_mirror: int = 0
    countdown: int = 0
    policy: int = 0
    dimensions: tuple = ((8,8),)*16


@dataclass(frozen=True)
class ShiftStageState:
    sides: tuple = (ShiftSideState(),ShiftSideState())
    display: tuple = (0,0)
    dirty: int = 0


@dataclass(frozen=True)
class ShiftStageResult:
    state: ShiftStageState
    reset_side: object = None  # stop before factory replacement/save on this side


def normal_shift_stage(state, current, short_rates=None):
    """GPIO-sampled normal UI through 0x0802dce4, before Array/shared edges.

    Calibration stage must be inactive (<=0 signed). Timer tick runs first;
    A's mutations are visible to B. reset_side marks a partial result before
    the factory replacement/save call sequence, never a completed UI stage.
    """
    if len(current)!=5 or any(v not in (0,1) for v in current):
        raise ValueError('five boolean panel inputs required')
    sides=list(state.sides);display=state.display;dirty=state.dirty
    timer=clock_timer_tick(ClockTimerState(tuple(x.elapsed for x in sides),
        tuple(x.countdown for x in sides),tuple(x.policy for x in sides)),
        tuple(x.capture for x in sides))
    for i in range(2):
        sides[i]=replace(sides[i],elapsed=timer.elapsed[i],countdown=timer.countdown[i],policy=timer.policy[i])
    if not current[0] and not current[1]:display=(0,0)
    return _normal_shift_sides(ShiftStageState(tuple(sides),display,dirty),current,short_rates,0)


def resume_shift_after_reset(post_io_state, reset_side, current, short_rates=None):
    """Resume after factory copy/save returns, using caller-supplied post-I/O state.

    Does not execute or assume success of persistence. The original caller
    ignores the save return value, consumes this hold, and continues after
    this side. Current inputs must be the same sampled inputs as before I/O.
    """
    if reset_side not in (0,1) or len(current)!=5 or any(v not in (0,1) for v in current):
        raise ValueError('reset side and five sampled boolean inputs required')
    sides=list(post_io_state.sides)
    sides[reset_side]=replace(sides[reset_side],hold=1601)
    return _normal_shift_sides(replace(post_io_state,sides=tuple(sides)),current,short_rates,reset_side+1)


def _normal_shift_sides(state,current,short_rates,start_side):
    sides=list(state.sides);display=state.display;dirty=state.dirty
    for i in range(start_side,2):
        s=sides[i];other=1-i
        route=shift_dispatch(i,current,s.previous,s.gesture,s.pending_clock)
        s=replace(s,previous=route.previous,gesture=route.gesture,pending_clock=route.pending_clock)
        sides[i]=s
        d1,d2=s.dimensions[s.slot]
        if route.route in ('press','idle'):
            passive=PassiveShiftState(s.hold,s.auxiliary,s.phase,s.linear_setting,
                s.linear_runtime,s.scan,d1,d2,s.countdown)
            changed=shift_press_action(passive) if route.route=='press' else shift_idle_action(i,passive)
            sides[i]=replace(s,hold=changed.hold,phase=changed.phase,
                             linear_runtime=changed.linear_runtime,scan=changed.scan)
        elif route.route=='hold':
            changed=long_hold_step(s.hold,sides[other].hold,s.linear_setting,dirty)
            sides[i]=replace(s,hold=changed.hold,linear_setting=changed.linear)
            dirty=changed.dirty
            if changed.action=='reset':
                return ShiftStageResult(ShiftStageState(tuple(sides),display,dirty),i)
        else:
            action=shift_release_action(s.hold,sides[other].hold)
            if action=='select_other':
                o=sides[other]
                selected=array_selection_action('AB'[other],ArraySelectionState(
                    o.slot,o.scan,o.slot_mirror,o.hold,o.gesture,display,dirty))
                sides[other]=replace(o,slot=selected.slot,slot_mirror=selected.slot_mirror,
                    scan=selected.scan,hold=selected.hold,gesture=selected.gesture)
                display=selected.display;dirty=selected.dirty
            elif action=='clock_self':
                changed=admitted_clock_action(AdmittedClockState(d1,d2,s.scan,s.capture,
                    s.auxiliary,s.elapsed,s.rate,s.period_mirror,dirty,s.countdown,s.policy),short_rates)
                sides[i]=replace(s,scan=changed.scan,elapsed=changed.elapsed,rate=changed.rate,
                    period_mirror=changed.period_mirror,countdown=changed.countdown,policy=changed.policy)
                dirty=changed.dirty
            elapsed,display=shift_release_finish(i,tuple(x.elapsed for x in sides),display,sides[other].gesture)
            sides[i]=replace(sides[i],elapsed=elapsed[i])
    return ShiftStageResult(ShiftStageState(tuple(sides),display,dirty))


@dataclass(frozen=True)
class CalibrationReleaseResult:
    stage: int
    clear_ui: bool
    save_pending: bool = False


def calibration_release_action(stage, side, hold):
    """Active stages 1..11, after dispatch reaches release handling.

    clear_ui requests the original five-byte UI/snapshot clearing and pin
    writes, not a reset of hold counters or of all module state. Cancellation
    can then continue into normal release admission with calibration inactive.
    save_pending stops before the calibration save call.
    """
    if not 1<=stage<=11 or side not in (0,1):
        raise ValueError('active stage 1..11 and side 0/1 required')
    if side==0 or (hold&0xffffffff)<=9:
        return CalibrationReleaseResult(0,True)
    stage+=1
    return CalibrationReleaseResult(stage,False,stage in (3,12))


def calibration_save_return(stage, shortcut):
    """Caller continuation after a pending stage-3 or stage-12 save returns.

    The return status is ignored. At stage 3 only shortcut exactly one exits;
    stage 12 always exits. The caller supplies any actual save mutations.
    """
    if stage not in (3,12):raise ValueError('pending save stage must be 3 or 12')
    clear=stage==12 or shortcut==1
    return CalibrationReleaseResult(0 if clear else stage,clear)
