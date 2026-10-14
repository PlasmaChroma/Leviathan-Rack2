"""Instruction-scheduled SP67 DSP translations; see docs/RACK_RECONSTRUCTION.md.

Inputs are already normalized DSP controls, NOT panel voltages. This is an
executable specification, not optimized real-time code or a complete emulator.
Finite normal float32 values are the supported comparison domain.
"""
import math
import struct
from sp67_reference import f32, fma32 as fma, fma64, from_bits, table, bits, active_terms


def mul(a, b): return f32(a * b)
def add(a, b): return f32(a + b)
def sub(a, b): return f32(a - b)
def wrap(x): return sub(x, math.trunc(x))


def calibration_ring_step(samples, sums, history, index):
    """One six-channel calibration bank update; history is six 64-word rows.

    Values use the firmware's calibration polarity (audio inputs negated after
    signed >>15). Caller owns the shared slow/fast ring index. Ordinary sums
    fitting signed int32 are supported; this is not an ADC/voltage model.
    """
    if len(samples)!=6 or len(sums)!=6 or len(history)!=6 or not 0<=index<64:
        raise ValueError('Expected six channels and a 0..63 ring index')
    if any(len(row)!=64 for row in history):raise ValueError('Expected 64-word rows')
    rows=[list(row) for row in history]
    updated=[total-row[index]+value for total,row,value in zip(sums,rows,samples)]
    for row,value in zip(rows,samples):row[index]=value
    return updated,rows,(index+1)&63


def calibration_endpoints(stage, current, sums, high, offsets, gains):
    """Stages 1/2 for a six-channel bank; returns high, offsets, gains, ready.

    Current and summed samples already have calibration polarity. The update
    thresholds are strict while the readiness comparisons include equality.
    Division follows float32; a zero endpoint span yields positive infinity.
    No persistence or GPIO writes occur in this helper.
    """
    if stage not in (1,2) or any(len(v)!=6 for v in (current,sums,high,offsets,gains)):
        raise ValueError('Expected stage 1/2 and six channels')
    high=list(high);offsets=list(offsets);gains=list(gains)
    for i,value in enumerate(current):
        if stage==1 and value>60000:high[i]=sums[i]>>6
        elif stage==2 and value<8000:
            offsets[i]=sums[i]>>6
            span=f32(high[i]-offsets[i])
            gains[i]=f32(1/span) if span else float('inf')
    ready=all(value>=60000 for value in current) if stage==1 else all(value<=8000 for value in current)
    return high,offsets,gains,ready


def calibration_pitch_tables(stage, sums, offsets, breakpoints, slopes):
    """Stages 3..11: A/B averaged pitch measurements and literal table fill.

    Two 16-entry tables per kind; ordinary signed int32 arithmetic without
    overflow is supported. Returns replacement breakpoints, slopes and the
    two accepted-measurement flags (which select low auxiliary GPIO writes).
    Even a rejected measurement extrapolates the tail in stages 4..11.
    """
    if not 3<=stage<=11 or len(sums)!=2 or len(offsets)!=2:
        raise ValueError('Expected stage 3..11 and two pitch channels')
    if any(len(tables)!=2 or any(len(row)!=16 for row in tables) for tables in (breakpoints,slopes)):
        raise ValueError('Expected two 16-entry tables of each kind')
    points=[list(row) for row in breakpoints];rates=[list(row) for row in slopes]
    accepted=[];j=stage-3
    for side in range(2):
        value=(sums[side]>>6)-offsets[side]
        valid=26<=value<=1999 if stage==3 else 8100*j-1500<value<8100*j+1500
        accepted.append(valid)
        if valid:
            points[side][j]=value
            if stage>3:
                span=f32(value-points[side][j-1])
                rates[side][j-1]=f32(2048/span) if span else float('inf')
        if stage>3:
            step=points[side][j]-points[side][j-1]
            for k in range(j+1,16):
                points[side][k]=points[side][k-1]+step
                rates[side][k-1]=rates[side][j-1]
    return points,rates,tuple(accepted)


def auxiliary_clock_rate(period, previous):
    """Admitted live clock period in 64-frame blocks; initialized table matches division."""
    return f32((1/64)/period) if 1 <= period <= 5999 else previous


def pending_operation_step(value):
    """Quiet-UI callback prefix: next byte and whether recovery is requested.

    The byte increments before the UI handler; this models its quiet-input path.
    Recovery clears it after the original transport calls regardless of their
    checked busy returns, then continues into the normal callback body.
    """
    if not isinstance(value,int) or not 0<=value<=255:raise ValueError('Expected a byte')
    advanced=(value+1)&255 if value else 0
    recover=advanced>2
    return (0 if recover else advanced),recover


def array_save_frame(frame, tracked_peak):
    """One save-loop frame: replacement RAM floats and literal PCM16 bytes.

    The caller must apply the returned floats to its Array to reproduce the
    firmware's mutation. No media I/O or marker changes occur here. Finite
    normal values with an int32-representable conversion are supported;
    narrowing to the stored halfword wraps rather than saturates.
    """
    if len(frame)!=64:raise ValueError('Expected 64 coefficients')
    peak=f32(tracked_peak)
    if not math.isfinite(peak):raise ValueError('Expected finite peak')
    gain=f32(1/peak) if 0<peak<1 else 1.
    scaled=[mul(value,gain) for value in frame]
    samples=[]
    for value in scaled:
        if not math.isfinite(value):raise ValueError('Expected finite coefficient')
        integer=math.trunc(value*32767.)  # exact float64 product of float32 and uint15
        if not -(1<<31)<=integer<(1<<31):raise ValueError('Conversion exceeds int32 domain')
        samples.append(integer&0xffff)
    return scaled,struct.pack('<64H',*samples)


def array_decode_sample_buffer(payload, sample_format):
    """Post-read sample conversion, excluding file I/O and alignment shifting.

    Returns (all converted values, reported sample count). Literal PCM24 code
    reports only complete groups of four, despite writing a partial group.
    Incomplete trailing bytes are ignored; IEEE input is restricted to finite
    values. This is not a WAV parser or validation of a complete Array file.
    """
    widths={'pcm8':1,'pcm16':2,'pcm24':3,'pcm32':4,'float32':4}
    if sample_format not in widths:raise ValueError('Unsupported sample format')
    width=widths[sample_format];count=len(payload)//width;values=[]
    for index in range(count):
        sample=payload[index*width:(index+1)*width]
        if sample_format=='float32':
            value=struct.unpack('<f',sample)[0]
            if not math.isfinite(value):raise ValueError('Expected finite float32')
        elif sample_format=='pcm8':value=f32((sample[0]-128)/128)
        else:
            integer=int.from_bytes(sample,'little',signed=True)
            if sample_format=='pcm16':value=f32(integer/32768)
            else:
                if sample_format=='pcm24':integer<<=8
                value=mul(f32(integer),from_bits(0x2ffffff6))
        values.append(value)
    return values,count//4*4 if sample_format=='pcm24' else count


def tuning_beacon_action(numerator, denominator):
    """GPIO action for ordinary finite ratios, not a stateless LED color.

    'green'/'red' pull only that pin low, preserving the other pin. 'off'
    drives both high. The caller must preserve pin state across updates.
    Physical brightness and polarity are outside this numeric helper. The
    comparison contract excludes subnormal inputs/results and exceptional FP.
    """
    numerator,denominator=map(f32,(numerator,denominator))
    if not math.isfinite(numerator) or not math.isfinite(denominator) or numerator<0 or denominator<=0:
        raise ValueError('Expected finite nonnegative numerator and positive denominator')
    ratio=f32(numerator/denominator)
    if not math.isfinite(ratio):raise ValueError('Ratio overflow')
    if ratio==0:return 'green'
    while ratio<1:ratio=add(ratio,ratio)
    while ratio>=2:ratio=mul(ratio,.5)
    bands=((.99,1.01,'green'),(1.24,1.26,'red'),(1.323,1.343,'green'),
           (1.49,1.51,'green'),(1.59,1.61,'red'),(1.656,1.676,'red'))
    for low,high,color in bands:
        if f32(low)<ratio<f32(high):return color
    return 'off'


def indicator_register_values(sample_counter, display_mode=0, selection_flags=(0,0),
                              slots=(0,0), engines=(0,0), sao=(0,0), capture=(0,0),
                              stored_linear=(0,0), live_linear=(0,0), shift_states=(0,0),
                              lf=(0,0), countdown=(0,0)):
    """Display PWM words, override flag, and A/B mode/aux GPIO high levels.

    Positive display override has priority over A selection, then B selection,
    then engine bits. Auxiliary None means no pin write, preserving prior state.
    Register polarity is defined here; physical LED colors/brightness are not.
    The contract uses 0..15 override/slot values, engines 0..2 and uint32 counter.
    Other paired values are the signed counters or raw flag values at the call.
    """
    pairs=(selection_flags,slots,engines,sao,capture,stored_linear,live_linear,shift_states,lf,countdown)
    if any(len(pair)!=2 for pair in pairs) or not 0<=sample_counter<=0xffffffff:
        raise ValueError('Expected A/B pairs and uint32 sample counter')
    if not 0<=display_mode<=15 or any(not 0<=slot<=15 for slot in slots) or any(not 0<=engine<=2 for engine in engines):
        raise ValueError('Expected display/slots 0..15 and engines 0..2')
    if display_mode:
        pattern=0 if sample_counter&32768 else display_mode
        override=1
    elif selection_flags[0]>0:
        pattern=slots[0];override=1
    elif selection_flags[1]>0:
        pattern=slots[1];override=1
    else:
        pattern=(engines[0]<<2)|engines[1];override=0
    pwm=tuple(0 if pattern&(1<<bit) else 127 for bit in (3,2,1,0))
    mode_high=tuple(not (sao[i]!=0 or (capture[i]!=0 and sample_counter&4096)) for i in range(2))
    aux_high=tuple(None if display_mode else bool(sample_counter&8192) if stored_linear[i]!=live_linear[i]
                   else ((shift_states[i]==1 or lf[i]==1)==(countdown[i]>0)) for i in range(2))
    return pwm,override,mode_high,aux_high


SETTINGS_FIELDS=('a_mode','b_mode','a_linear','b_linear','a_aux','b_aux',
                 'a_engine','b_engine','a_period','b_period','a_lf','b_lf',
                 'a_array','b_array','interaction')


def pack_settings(config):
    """Eight flash-record words, with literal ORs and 32-bit truncation.

    The first four RAM fields are halfwords; the remaining fields are words.
    No field validation/clamping is added. In particular, a period above
    65535 can overlap the saved LF bits. This is a codec, not a flash writer.
    """
    c={key:int(config[key])&0xffffffff for key in SETTINGS_FIELDS}
    flags=sum(int(c[key]&0xffff==1)<<bit for key,bit in
              (('a_mode',16),('b_mode',0),('a_linear',20),('b_linear',4)))
    words=[flags,c['a_aux']|(c['a_engine']<<16),c['b_aux']|(c['b_engine']<<16),
           c['a_period']|(c['a_lf']<<16),c['b_period']|(c['b_lf']<<16),
           c['a_array'],c['b_array'],c['interaction']]
    return [word&0xffffffff for word in words]


def unpack_settings(words):
    """Literal flash-record decoder, including signed high-half fields."""
    if len(words)!=8:raise ValueError('Expected eight settings words')
    w=[int(word)&0xffffffff for word in words]
    def signed16(x):
        x&=0xffff
        return x-65536 if x&0x8000 else x
    # A linear mode is ASR #20 then STRH, not the B-side four-bit mask.
    signed_flags=w[0]-0x100000000 if w[0]&0x80000000 else w[0]
    values=((w[0]>>16)&15,w[0]&15,signed16(signed_flags>>20),(w[0]>>4)&15,
            w[1]&0xffff,w[2]&0xffff,signed16(w[1]>>16),signed16(w[2]>>16),
            w[3]&0xffff,w[4]&0xffff,signed16(w[3]>>16),signed16(w[4]>>16),
            w[5],w[6],w[7])
    return dict(zip(SETTINGS_FIELDS,values))


def smoothed_control(previous, target):
    return fma(sub(f32(target),f32(previous)),from_bits(0x3c23d70a),f32(previous))


def partials_control(raw_code, offset, gain, previous):
    """Fast Partials path: lower clamp only, then per-sample 1% smoothing."""
    return smoothed_control(previous,max(0.,mul(f32(raw_code-offset),f32(gain))))


def analysis_references(phase):
    """64 sine/cosine references with the firmware's float32 recurrence."""
    position=mul(f32(phase),8192.)
    c=interpolated_sine(add(position,2048.));s=interpolated_sine(position)
    twice=add(c,c)
    cosines=[c,fma(c,twice,-1.)];sines=[s,mul(s,twice)]
    for _ in range(2,64):
        cosines.append(fma(twice,cosines[-1],-cosines[-2]))
        sines.append(fma(twice,sines[-1],-sines[-2]))
    return cosines,sines


def analysis_parameters_exact(slide,focus):
    """Instruction-scheduled SAM rate/pole setup, including the float64 FMA."""
    if not 0<=slide<=1 or not 0<=focus<=1:raise ValueError('Expected normalized controls')
    q=math.trunc(mul(f32(slide),8191.))
    inc=mul(1<<(q>>11),mul(table('exp2_2048')[q&2047],from_bits(0x39da740e)))
    width=fma(-f32(focus),from_bits(0x3f7ae148),1.)
    curvature=(width*width)*(-3.14)
    pole=min(f32(fma64(inc,curvature,1.)),from_bits(0x3f7ffeb0))
    return inc,pole


def sam_detector_step(previous_input, previous_conditioned, cosine_states, sine_states,
                      magnitudes, input_sample, phase, increment, pole):
    """Exact ordinary-float SAM bank schedule; inactive harmonics retain state.

    Each quadrature state array contains 64 interleaved [stage1,stage2] pairs.
    Magnitudes retain the literal bit-square-root behavior, including energy=0.
    """
    u=mul(f32(input_sample),12.)
    conditioned=add(u,fma(f32(previous_conditioned),from_bits(0x3f7eb852),-f32(previous_input)))
    cs=list(cosine_states);ss=list(sine_states);amps=list(magnitudes)
    cosines,sines=analysis_references(phase)
    for i in range(active_terms(f32(increment))):
        second=[]
        for states,reference in ((cs,cosines[i]),(ss,sines[i])):
            intermediate=fma(reference,-conditioned,f32(states[2*i]))
            first=fma(conditioned,reference,mul(f32(pole),intermediate))
            last=fma(f32(pole),sub(states[2*i+1],first),first)
            states[2*i:2*i+2]=[first,last];second.append(last)
        energy=fma(second[0],second[0],mul(second[1],second[1]))
        encoded=((((bits(energy)-0x00800000)&0xffffffff)>>1)+0x20000000)&0xffffffff
        amps[i]=from_bits(encoded)
    return u,conditioned,cs,ss,amps


def array_load_count(data_bytes, depth, channels):
    """Loader count for ordinary nonnegative sizes and positive PCM dimensions.

    Channels affect the total count but not the later scalar conversion loop.
    Excludes signed overflow, divide-by-zero and corrupt parser fields.
    """
    if not (0 <= data_bytes <= 0x7fffffff and depth > 0 and channels > 0
            and 8 <= depth * channels <= 0x7fffffff):
        raise ValueError('Expected ordinary nonnegative payload and positive byte frame size')
    return data_bytes // (depth * channels // 8)


def wav_format_classification(tag, pcm_depth, previous_code):
    """Post-read parser classification; None means rejected format tag.

    This does not validate a WAV or perform I/O. Both halfwords are interpreted
    as signed, as in the original parser. Unknown PCM depth preserves the caller's
    prior code, whereas recognized non-PCM tags supply a fixed depth/code.
    """
    signed16 = lambda value: ((value & 0xffff) ^ 0x8000) - 0x8000
    tag = signed16(tag)
    if tag == 1:
        depth = signed16(pcm_depth)
        return depth, {8: 0x75733038, 16: 0x73693136, 24: 0x73693234,
                       32: 0x73693332}.get(depth, previous_code)
    return {2: (4, 0x6d736164), 3: (32, 0x666c3332),
            17: (4, 0x696d6164), 257: (8, 0x6d753038),
            258: (8, 0x616c3038), 259: (4, 0x69626164)}.get(tag)


def slide_clock_tracking(side, normalized_slide, previous_slide, anchor, clock_offset):
    """Slow-control handoff to manual Slide; returns slide, anchor, movement, offset.

    Ordinary finite float32 controls, nonnegative integer clock offset. A measures
    unclamped movement; B clamps first. An existing offset freezes the anchor
    until movement strictly exceeds 0.005; its following zero-offset call then
    refreshes the anchor from the previous Slide value.
    """
    if side not in ('A','B') or not isinstance(clock_offset,int) or clock_offset<0:
        raise ValueError('Expected side A/B and nonnegative clock offset')
    raw,previous,anchor=map(f32,(normalized_slide,previous_slide,anchor))
    if not all(math.isfinite(v) for v in (raw,previous,anchor)):raise ValueError('Expected finite controls')
    if clock_offset==0:anchor=previous
    clamped=min(1.,max(0.,raw))
    movement=abs(sub(raw if side=='A' else clamped,anchor))
    return clamped,anchor,movement,0 if movement>f32(.005) else clock_offset


def array_reader_addresses(dimension1, dimension2, descriptor_offset, bank,
                           slide, focus, clock_offset=0, linear=False):
    """Raw 32-bit reader addresses; never dereference these unchecked.

    Includes large capture-stop lengths, signed comparisons and word/byte
    overflow. This diagnostic reference does not expand the storage adapters'
    0..1024-frame contract or prescribe a Rack invalid-descriptor policy.
    Excludes planar grid-search overflow and saturated float-to-int conversion.
    """
    words=(dimension1,dimension2,descriptor_offset,bank)
    if any(not isinstance(x,int) or not 0<=x<=0xffffffff for x in words):
        raise ValueError('Expected unsigned descriptor and bank words')
    if not 0<=slide<=1 or not 0<=focus<=1 or not isinstance(clock_offset,int) or not 0<=clock_offset<=1023:
        raise ValueError('Expected normalized controls and scan offset 0..1023')
    slide,focus=f32(slide),f32(focus)
    def signed(x):
        x &= 0xffffffff
        return x-0x100000000 if x&0x80000000 else x
    n=signed(dimension1*dimension2)
    span=signed(n<<6)
    grid=None
    if linear:
        position=fma(f32(signed(n-1)),f32(slide),f32(clock_offset))
        if not -(1<<31)<=position<(1<<31):
            raise ValueError('Position requires saturated float-to-int conversion')
        index=math.trunc(position)
        offsets=[signed((index+i)<<6) for i in (0,1)]
        offsets=[signed(x-span) if n<=signed(index+i) else x for i,x in enumerate(offsets)]
        fractions=(sub(position,f32(math.trunc(position))),)
    else:
        if n>46340**2:
            raise ValueError('Planar grid search exceeds non-overflowing signed-square domain')
        grid=math.isqrt(n-1)+1 if n>0 else 2
        y=mul(slide,grid-1) if n>0 else f32(slide)
        x=add(mul(focus,grid-1) if n>0 else f32(focus),f32(clock_offset))
        xi,yi=math.trunc(x),math.trunc(y)
        row0=yi-(grid-1) if yi>=grid else yi
        row1=yi+1-(grid-1) if yi>=grid-1 else yi+1
        offsets=[signed((row*grid+col)<<6) for row,col in
                 ((row0,xi),(row0,xi+1),(row1,xi),(row1,xi+1))]
        offsets=[signed(x-span) if span<=x else x for x in offsets]
        fractions=(sub(x,f32(xi)),sub(y,f32(yi)))
    addresses=[(bank+((descriptor_offset+x)<<2))&0xffffffff for x in offsets]
    return addresses,fractions,grid,n


def linear_coordinates(frame_count, slide, clock_offset=0):
    """Original signed position arithmetic, including empty-descriptor reads."""
    if not isinstance(frame_count,int) or not 0<=frame_count<=1024 or not 0<=slide<=1:
        raise ValueError('Expected 0..1024 frames and normalized Slide')
    if not isinstance(clock_offset,int) or not 0<=clock_offset<=(1<<31)-1:
        raise ValueError('Expected a nonnegative signed int32 clock offset')
    position=fma(f32(frame_count-1),f32(slide),f32(clock_offset))
    index=math.trunc(position)
    return [i-frame_count if i>=frame_count else i for i in (index,index+1)],sub(position,index)


def linear_deltas(frames, current, slide, focus, clock_offset=0):
    """Exact linear-reader Focus transform and fused coefficient-ramp delta.

    Valid nonempty Arrays and normalized controls only. This differs from
    rounding a transformed target first and then subtracting the current bank.
    """
    n=len(frames)
    if not 1<=n<=1024 or len(current)!=64 or any(len(row)!=64 for row in frames):
        raise ValueError('Expected 1..1024 spectra of 64 coefficients')
    if not 0<=slide<=1 or not 0<=focus<=1 or not isinstance(clock_offset,int) or clock_offset<0:
        raise ValueError('Expected normalized controls and nonnegative integer offset')
    indices,fraction=linear_coordinates(n,slide,clock_offset)
    if any(i<0 or i>=n for i in indices):raise ValueError('Firmware addressing leaves the logical Array')
    return _linear_source_deltas([frames[i] for i in indices],current,fraction,focus)


def linear_storage_deltas(storage_frames, first_frame, frame_count, current,
                          slide, focus, clock_offset=0):
    """Bounded physical storage; an empty Array can read its preceding frame.

    first_frame locates the descriptor in the supplied storage window. Check
    absolute indices before indexing so Python's negative indexing cannot hide
    a missing leading frame. Retain neighboring contents without logical wrap.
    """
    if not isinstance(first_frame,int) or first_frame<0 or len(current)!=64 or not 0<=focus<=1:
        raise ValueError('Expected nonnegative physical offset, 64 coefficients and normalized Focus')
    indices,fraction=linear_coordinates(frame_count,slide,clock_offset)
    absolute=[first_frame+i for i in indices]
    if any(i<0 or i>=len(storage_frames) for i in absolute):
        raise ValueError('Firmware addressing leaves supplied physical storage')
    sources=[storage_frames[i] for i in absolute]
    if any(len(row)!=64 for row in sources):raise ValueError('Expected 64 coefficients per physical frame')
    return _linear_source_deltas(sources,current,fraction,focus)


def _linear_source_deltas(sources,current,fraction,focus):
    focus=f32(focus)
    gain=sub(1.25,mul(.75,focus));bypass=mul(.75,focus)==.25
    exponent=table('focus_exponent_1024')[min(math.trunc(focus*1024),1023)]
    result=[]
    for a,b,old in zip(sources[0],sources[1],current):
        value=fma(sub(b,a),fraction,f32(a))
        if value<=0 or bypass:delta=sub(value,old)
        else:
            encoded=(bits(value)+0xc0876c0b)&0xffffffff
            integer=encoded-0x100000000 if encoded&0x80000000 else encoded
            transformed=fma(exponent,f32(integer),f32(1064866816))
            if not -(1<<31)<=transformed<(1<<31):raise ValueError('Bit transform exceeds int32 domain')
            delta=fma(gain,from_bits(math.trunc(transformed)), -f32(old))
        result.append(mul(delta,1/64))
    return result


def planar_coordinates(frame_count, slide, focus, clock_offset=0):
    """Reader addressing, including empty-descriptor fallback; no safety repair.

    Returns four frame indices, x/y fractions and the stored grid size. Indices
    may be outside the logical Array. A Rack adapter must check them before use.
    Controls here are normalized finite values, offsets nonnegative integers.
    """
    if not 0 <= frame_count <= 1024 or not 0 <= slide <= 1 or not 0 <= focus <= 1:
        raise ValueError('Expected 0..1024 frames and normalized controls')
    if not isinstance(clock_offset, int) or clock_offset < 0:
        raise ValueError('Expected a nonnegative integer clock offset')
    slide, focus = f32(slide), f32(focus)
    if frame_count:
        grid = math.isqrt(frame_count - 1) + 1
        x = mul(focus, grid - 1)
        y = mul(slide, grid - 1)
    else:
        grid = 2
        x, y = f32(focus), f32(slide)
    x = add(x, f32(clock_offset))  # separate multiply/add, not an FMA
    xi, yi = math.trunc(x), math.trunc(y)
    row0 = yi - (grid - 1) if yi >= grid else yi
    row1 = yi + 1 - (grid - 1) if yi >= grid - 1 else yi + 1
    indices = [row0*grid+xi, row0*grid+xi+1, row1*grid+xi, row1*grid+xi+1]
    indices = [i-frame_count if i >= frame_count else i for i in indices]
    return indices, sub(x, xi), sub(y, yi), grid


def planar_deltas(frames, current, slide, focus, clock_offset=0):
    """Exact planar coefficient-ramp schedule for valid logical frame accesses."""
    indices, tx, ty, _ = planar_coordinates(len(frames), slide, focus, clock_offset)
    if len(current) != 64 or any(len(row) != 64 for row in frames):
        raise ValueError('Expected 64 coefficients per spectrum')
    if any(i < 0 or i >= len(frames) for i in indices):
        raise ValueError('Firmware addressing leaves the logical Array')
    return _planar_source_deltas([frames[i] for i in indices], current, tx, ty)


def planar_slot_deltas(slot_frames, frame_count, current, slide, focus, clock_offset=0):
    """Planar playback against retained 1024-frame physical slot storage.

    Logical length selects the grid/wrap arithmetic, not the backing allocation.
    Values past the logical length remain from prior writes. This also models
    the empty planar descriptor's reads; it is not an empty linear-reader policy.
    Reject any actual source outside the supplied slot before dereferencing.
    """
    if len(slot_frames) != 1024 or len(current) != 64 or any(len(row) != 64 for row in slot_frames):
        raise ValueError('Expected a full 1024-by-64 slot and 64 working coefficients')
    indices, tx, ty, _ = planar_coordinates(frame_count, slide, focus, clock_offset)
    if any(i < 0 or i >= 1024 for i in indices):
        raise ValueError('Firmware addressing leaves the physical slot')
    return _planar_source_deltas([slot_frames[i] for i in indices], current, tx, ty)


def _planar_source_deltas(sources, current, tx, ty):
    a, b, c, d = sources
    result = []
    for aa, bb, cc, dd, old in zip(a, b, c, d, current):
        low = fma(sub(bb, aa), tx, f32(aa))
        high = fma(sub(dd, cc), tx, f32(cc))
        result.append(mul(fma(sub(high, low), ty, sub(low, old)), 1/64))
    return result


def planar_storage_deltas(storage_frames, first_frame, frame_count, current,
                          slide, focus, clock_offset=0):
    """Bounded contiguous-bank variant, including reads into neighboring slots.

    first_frame is the descriptor offset in 64-float frames relative to this
    supplied storage window. Preserve neighboring data and trailing guard RAM;
    do not wrap at a logical or slot boundary. Missing physical storage raises.
    """
    if not isinstance(first_frame,int) or first_frame<0 or len(current)!=64:
        raise ValueError('Expected nonnegative physical frame offset and 64 working coefficients')
    indices,tx,ty,_=planar_coordinates(frame_count,slide,focus,clock_offset)
    absolute=[first_frame+i for i in indices]
    if any(i<0 or i>=len(storage_frames) for i in absolute):
        raise ValueError('Firmware addressing leaves supplied physical storage')
    sources=[storage_frames[i] for i in absolute]
    if any(len(row)!=64 for row in sources):raise ValueError('Expected 64 coefficients per physical frame')
    return _planar_source_deltas(sources,current,tx,ty)


def sine(phase):
    return table('sine_8192')[math.trunc(phase * 8192) & 8191]


def interpolated_sine(position):
    i = math.trunc(position)
    t = table('sine_8192')
    return fma(sub(t[(i + 1) & 8191], t[i & 8191]), sub(position, i), t[i & 8191])


def chaos_step(state, partials, feedback_control, ratio_control, inc_odd, inc_even, fm_index=0., fm=0., side='A'):
    """Return (16-word state, two preclip outputs, A FM increment contribution).

    State layout per side: four 16-byte oscillator records; phase at +0,
    output at +12. Update order is 1,3,2,4. A at 0x200022bc; B at +64.
    A slice 0x0802fa04..fc16; B 0x0802fc96..ff00 (exclusive stops).
    """
    if side not in ('A', 'B'): raise ValueError(side)
    st = list(map(f32, state))
    p, f, s, io, ie, index, fm = map(f32, (partials, feedback_control, ratio_control, inc_odd, inc_even, fm_index, fm))
    depth = mul(p, f32(9.9))
    feedback = mul(mul(f, f32(.2)), depth)
    ratio = fma(s, 21., 1.)
    low = math.trunc(ratio)
    n1, n2 = low + 1, low
    if low & 1: n1, n2 = n2, n1
    blend = mul(add(table('sine_8192')[(math.trunc(ratio * 4096) + 2048) & 8191], 1.), .5)
    fm_delta = mul(mul(index, io), fm)
    do = add(io, fm_delta) if side == 'A' else fma(fm, mul(index, io), io)
    de = fma(fm, mul(index, ie), ie)
    qo, qe = mul(do, .25), mul(de, .25)
    old1, old2, old3, old4 = st[3], st[7], st[11], st[15]

    def morph(phase):
        a = sine(mul(phase, n1))
        b = sine(mul(phase, n2))
        return fma(sub(b, a), blend, a)

    st[0] = wrap(fma(qo, mul(feedback, fma(old3, 2., mul(old2, f32(.1)))), add(st[0], qo)))
    st[3] = morph(st[0])
    st[8] = wrap(fma(fma(st[3], 2., -mul(f32(.1), old4)), mul(depth, do), add(st[8], do)))
    st[11] = sine(st[8])
    st[4] = wrap(fma(fma(old4, 2., mul(st[11], f32(.1))), mul(feedback, qe), add(st[4], qe)))
    st[7] = morph(st[4])
    st[12] = wrap(fma(fma(st[7], 2., -mul(f32(.1), st[3])), mul(depth, de), add(st[12], de)))
    st[15] = sine(st[12])
    return st, (fma(st[11], f32(.6), mul(st[3], f32(.3))),
                fma(st[15], f32(.6), mul(st[7], f32(.3)))), fm_delta


def noise_step(generator, filters, envelope, seed, partials, slide, focus,
               phase_odd, phase_even, slow=False, side='A', phase_gains=(1., 1.)):
    """Return (generator, filters, envelope, shared RNG seed, preclip outputs).

    generator: six float words [phase, step, speed, start, difference, value].
    filters: twelve words, two six-word filter records. A records at
    0x2000233c and +48; B at +24 and +72. RNG at 0x200023cc is SHARED.
    phase_gains supplies B's raw interaction-mode-2 multipliers; ignored for A.
    Slide sets the first low-pass coefficient; Focus sets the following
    high-pass coefficient relative to it. Their order is intentional.
    """
    if side not in ('A', 'B'): raise ValueError(side)
    g, v = list(map(f32, generator)), list(map(f32, filters))
    p, f, s, po, pe, env = map(f32, (partials, slide, focus, phase_odd, phase_even, envelope))
    pos_o, pos_e = mul(po, 8192.), mul(pe, 16384.)
    if side == 'B':
        pos_o, pos_e = mul(pos_o, f32(phase_gains[0])), mul(pos_e, f32(phase_gains[1]))
    carrier_o, carrier_e = interpolated_sine(pos_o), interpolated_sine(pos_e)
    q = math.trunc(mul(p, 25000.))
    if not 0 <= q <= 25000 or not 0 <= f <= 1:
        raise ValueError('Normalized partials/Slide required')
    rate = mul(mul(table('exp2_2048')[q & 2047], .0625 if slow else 4.), 1 << (q >> 11))
    g[1] = mul(rate, from_bits(0x37aec33e))
    initial = g[0] if g[0] <= 10. else 0.
    g[0] = fma(g[1], g[2], initial) if side == 'A' else add(g[1], initial)
    if g[0] >= 1.:
        seed = (0x0bb38435 * seed + 0x3619636b) & 0xffffffff
        rnd = fma(f32(seed), from_bits(0x2f80000d), -.5)
        g[3] = g[5]
        g[4] = fma(rnd, 2., -g[5])
        g[0] = wrap(g[0])
    weight = add(table('sine_8192')[(math.trunc(g[0] * 4096) - 2048) & 8191], 1.)
    g[5] = fma(weight, mul(g[4], .5), g[3])
    v[0] = mul(mul(table('noise_rate_256')[math.trunc(mul(f, 255.))], from_bits(0x403cff8b)), g[1])
    v[3] = fma(v[0], v[2], v[3])
    v[6] = mul(mul(s, f32(.9)), v[0])
    v[9] = fma(v[6], v[8], v[9])
    v[10] = sub(fma(-v[8], .5, v[3]), v[9])
    v[8] = fma(v[6], v[10], v[8])
    v[11] = add(v[10], v[8])
    v[4] = sub(g[5], fma(v[2], f32(1.2), v[3]))
    v[2] = fma(v[0], v[4], v[2])
    v[5] = add(v[4], v[2])
    level = abs(v[10])
    if env < level: env = fma(level, f32(.1), mul(env, f32(.9)))
    else: env = fma(level, from_bits(0x3851b717), mul(env, from_bits(0x3f7ffcb9)))
    gain = f32(f32(1.1) / env) if env else math.inf
    return g, v, env, seed, (mul(mul(v[10], carrier_o), gain), mul(mul(v[10], carrier_e), gain))


def clip_a(x):
    # VMINNM selects the numeric operand for a quiet NaN; its +1.5 clamp
    # precedes VMAXNM. Python min/max alone has different NaN behavior.
    x = 1.5 if math.isnan(x) else min(1.5, max(-1.5, f32(x)))
    return mul(x, fma(-mul(x, x), from_bits(0x3e17b426), 1.))


def clip_b(x):
    x = 1.5 if math.isnan(x) else min(1.5, max(-1.5, f32(x)))
    return mul(x, sub(1., mul(mul(x, x), from_bits(0x3e17b426))))


def phase_step(state, increments, fm_indices, aux_modes=(0,0), aux_rates=(0.,0.), interaction=0):
    """Phase and internal cross-FM update for raw interaction modes 0, 1, 2.

    state dict keys: a_odd,a_even,b_odd,b_even,a_sine,b_sine,a_sub,b_sub,
    a_analysis,b_analysis,seed,a_random,a_previous,b_random,b_previous,
    a_pulse,b_pulse. increments=(aOdd,aEven,bOdd,bEven,aAnalysis,bAnalysis).
    Mode 2 substitutes A's increments into B's spectral phase update; clean
    sine/Sub rates remain B's. No external CV acquisition is modeled here.
    """
    st=dict(state)
    ao,ae,bo,be,ai,bi=map(f32,increments)
    ix_a,ix_b=map(f32,fm_indices)
    if interaction not in (0,1,2):raise ValueError('Invalid interaction mode')
    # Both sources are evaluated BEFORE either clean sine phase is advanced.
    src_a=interpolated_sine(mul(f32(st['b_sine']),8192.))
    src_b=interpolated_sine(mul(f32(st['a_sine']),8192.))
    delta_a=mul(mul(ao,ix_a),src_a)
    next_ao=add(add(ao,st['a_odd']),delta_a)
    next_ae=add(add(st['a_even'],ae),delta_a)
    if ao==ae:next_ae=fma(sub(next_ao,next_ae),from_bits(0x3a03126f),next_ae)
    fm_b=mul(src_b,ix_b)
    drive_odd,drive_even=(ao,ae) if interaction==2 else (bo,be)
    next_be=fma(drive_odd,fm_b,add(drive_even,st['b_even']))
    next_bo=fma(drive_odd,fm_b,add(drive_odd,st['b_odd']))
    if bo==be:next_be=fma(sub(next_bo,next_be),from_bits(0x3a03126f),next_be)
    for key,val in (('a_odd',next_ao),('a_even',next_ae),('b_odd',next_bo),('b_even',next_be),
                    ('a_sine',add(st['a_sine'],ao)),('b_sine',add(st['b_sine'],bo)),
                    ('a_analysis',add(st['a_analysis'],ai)),('b_analysis',add(st['b_analysis'],bi))):
        st[key]=wrap(val)
    # The sub/CV RNG is separate from Noise's RNG, but shared between sides.
    # B is advanced first, including RNG consumption if both cross together.
    for side,inc,mode,rate in (('b',bo,aux_modes[1],aux_rates[1]),('a',ao,aux_modes[0],aux_rates[0])):
        phase=fma(inc,.5,state[side+'_sub']) if mode in (0,5) else add(state[side+'_sub'],f32(rate))
        if phase>=1.:
            phase=sub(phase,1.)
            if mode not in (0,5):st[side+'_pulse']=20 if side=='b' else 10
            st['seed']=(st['seed']*0x0bb38435+0x3619636b)&0xffffffff
            st[side+'_previous']=state[side+'_random']
            st[side+'_random']=fma(f32(st['seed']),from_bits(0x3000000d),-1.)
        if phase<0.:phase=add(phase,1.)  # no random refresh on negative crossing
        st[side+'_sub']=phase
    return st, (src_a,src_b), delta_a


def normal_phase_step(state, increments, fm_indices, aux_modes=(0,0), aux_rates=(0.,0.)):
    return phase_step(state,increments,fm_indices,aux_modes,aux_rates,interaction=0)


def interaction_ratios(increments, interaction):
    """Pitch setup ratios, before standard/Noise dispatch may overwrite them."""
    ao,ae,bo,be=map(f32,increments[:4])
    if interaction==0:return 1.,1.
    return f32(bo/ao),f32(be/ae)


def interaction_gate(phase):
    """Mode-2 B amplitude window, evaluated from A's pre-update phase."""
    phase=f32(phase);triangle=add(phase,phase)
    if phase>.5:triangle=sub(2.,triangle)
    return 1. if triangle>f32(.05) else mul(triangle,20.)


def auxiliary_value(side, mode, phase, current_random, previous_random, sao, envelope, raw_b_pitch=65536):
    """Sub/CV value BEFORE final +1 offset for modes 1..4 and output scaling.

    A default uses the shaped table; B uses sine with pitch-dependent drive.
    This asymmetry is literal firmware behavior, not an assumed panel mapping.
    """
    if side not in ('A','B') or mode not in range(6):raise ValueError('Invalid side/mode')
    phase=f32(phase)
    if mode==1:return f32(current_random)
    if mode==2:return fma(sub(f32(current_random),f32(previous_random)),phase,f32(previous_random))
    if mode==0 and not sao:return mul(f32(envelope),4.)
    name='waveform_triangle_1024' if mode==3 else 'waveform_ramp_1024' if mode==4 else \
         'waveform_shaped_1024' if side=='A' else 'waveform_sine_1024'
    t=table(name);position=mul(phase,1024.);i=math.trunc(position)
    value=fma(sub(t[(i+1)&1023],t[i&1023]),sub(position,i),t[i&1023])
    if mode==4:return -value
    if side=='B' and mode in (0,5):
        drive=fma(f32(65536-raw_b_pitch),from_bits(0x38000000),1.5)
        return clip_a(mul(drive,value))
    return value


def even_increment(odd_increment, normalized_control, sao=True):
    """SAO even-rate law after signed input calibration (no input clamping)."""
    odd,x=map(f32,(odd_increment,normalized_control))
    if not sao:return odd
    small=from_bits(0x38d1b717)
    if x<f32(.3):offset=mul(x,small)
    else:offset=fma(sub(odd,small),mul(sub(x,f32(.3)),from_bits(0x3fb6db6e)),small)
    return mul(add(odd,offset),.5)


def b_pitch_increment(coordinate, a_unscaled_hz, linked=False, slow=False):
    """B pitch after piecewise calibration, before even-rate adjustment.

    a_unscaled_hz is A's frequency BEFORE its own /256 LF scaling. Linked
    corresponds to either nonzero interaction mode; exact panel labels and
    subsequent phase/ratio integration are separate from this numeric law.
    """
    if not 0<=coordinate<=32767:raise ValueError('Expected valid internal pitch coordinate')
    fraction=table('exp2_2048')[coordinate&2047];octave=coordinate>>11
    if linked:
        hz=mul(mul(1<<octave,mul(fraction,from_bits(0x3d7ae148))),f32(a_unscaled_hz))
    else:
        hz=mul(1<<(octave+4),mul(fraction,from_bits(0x3f82d013)))
    increment=mul(hz,from_bits(0x37aec33e))
    return mul(increment,1/256) if slow else increment


def default_calibration():
    """Fallback digital calibration values; these are not measured jack volts."""
    return {'offsets':[2000]*12, 'gains':[from_bits(0x37840803)]*12,
            'pitch_breakpoints':[40,8120,16200,24280,32360,40440,48520,56600],
            'pitch_slopes':[from_bits(0x3e808312)]*8}


def pitch_coordinate(raw_code, offset, breakpoints, slopes):
    """Fast pitch ADC to nonnegative exp2 coordinate, A/B compiled schedule.

    Final two regions intentionally reuse slope[5]. This interface requires
    sane monotonic calibration and uint16 input; corrupt saved data is excluded.
    """
    if not 0 <= raw_code <= 65535 or len(breakpoints)!=8 or len(slopes)!=8:
        raise ValueError('Expected uint16 code and eight calibration points/slopes')
    if any(a>=b for a,b in zip(breakpoints,breakpoints[1:])):
        raise ValueError('Expected increasing calibration breakpoints')
    x=raw_code-offset
    region=next((i for i in range(7) if x<breakpoints[i+1]),7)
    delta=f32(x-breakpoints[region])
    gain=f32(slopes[min(region,5)])
    value=mul(delta,gain) if region==0 else fma(delta,gain,float(region*2048))
    return max(0,math.trunc(value))
