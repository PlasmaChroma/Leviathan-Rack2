"""Readable models of selected recovered routines, not a complete Tempi emulator."""
def signed32(x):
    x &= 0xffffffff
    return x if x<0x80000000 else x-0x100000000

def trunc_div(n,d):
    if not d: return 0
    return (abs(n)//abs(d)) * (-1 if (n<0) != (d<0) else 1)

def ratio_halfperiod(r, halfperiod):
    """7E54: signed 16-bit code r, signed 32-bit halfperiod; 32-bit wrap."""
    h=signed32(halfperiod)
    if r>0:h=trunc_div(signed32(h*4),r+4)
    elif r<0:h=trunc_div(signed32(h*(4-r)),4)
    if h>=0x1000000:h=0xffffff
    if h<200:h=200
    return h

def phase_offset(r, phase, master_halfperiod):
    """65DE: phase byte; divider phase uses master, multiplier uses lane period.
    This is the computed offset, not the complete resynchronization scheduler.
    """
    base=master_halfperiod if r<0 else ratio_halfperiod(r,master_halfperiod)
    return trunc_div(signed32(base*(phase&255)*2),4)

def next_random(state):
    state=(state*6364136223846793005+1)&0xffffffffffffffff
    return state,(state>>49)&255

def adc_state(adc, previous, thresholds):
    """78FC hysteresis; thresholds must be 16 ascending uint16 values."""
    if adc==65535:return previous
    upper=next((j for j,t in enumerate(thresholds) if adc<t),16)
    if upper==16:return 15
    if upper==0:return 0
    width=(thresholds[upper]-thresholds[upper-1])//3
    if previous==upper and adc<=thresholds[upper-1]+width:return upper-1
    if previous==upper-1 and adc<=thresholds[upper]-width:return upper-1
    return upper

def decode_state(eeprom, state):
    if len(eeprom)!=1024:raise ValueError('Expected exactly 1024 bytes')
    if not 0<=state<64:raise ValueError('State must be 0..63')
    ratios=[int.from_bytes(eeprom[64*c+state:64*c+state+1],'little',signed=True) for c in range(6)]
    return dict(state=state,bank=state//16,slot=state%16,ratio_codes=ratios,
        phase_codes=[eeprom[0x180+64*c+state] for c in range(6)],
        enable_mask=eeprom[0x300+state],mod_mask=eeprom[0x340+state])
