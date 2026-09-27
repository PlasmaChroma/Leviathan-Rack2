"""Offline binary32 reference calculations. No claim of full CPU emulation.

fma32 uses rational arithmetic to select the nearest binary32 result (ties even).
This deliberately slow helper is for finite, bounded test vectors, never audio DSP.
"""
from fractions import Fraction
import struct
from common import ROOT,BASE,flash,f32,bits,frombits,put_csv,put_json
B=flash()
def table(a,n,fmt='f'):return struct.unpack_from('<'+fmt*n,B,a-BASE)
GENE=table(0x08044d10,1024)
LAUNCH=table(0x08047630,34);DENSITY=table(0x080475a8,34);CYCLE=table(0x08047740,34,'I')
def fma32(a,b,c):
    exact=Fraction(a)*Fraction(b)+Fraction(c)
    candidate=bits(f32(float(exact)))
    neighbors=[frombits(k) for k in range(max(0,candidate-2),min(0xffffffff,candidate+2)+1)]
    return min(neighbors,key=lambda x:(abs(Fraction(x)-exact),bits(x)&1))
def morph_stage(adc):return min(21,int(f32(f32(adc/4096)*frombits(0x4207f5c3))))
def sos(adc):
    scaled=f32(adc*frombits(0x398a26fe))
    return 1. if scaled>frombits(0x3f864064) else max(0.,f32(scaled-frombits(0x3d480c74)))
def gene(adc,splice):
    if adc<=199:return None
    b=f32(splice)
    while b>576000:b=f32(b*.5)
    x=GENE[1073-(adc>>2)]
    return max(8.,f32(f32(f32(x*x)*x)*b))
def clock(preliminary,splice):
    if preliminary<=8:return preliminary
    two=frombits(0x3f2aaa9f);current=f32(f32(splice)*two)
    if preliminary>=current:return f32(splice)
    multiplier=.75
    while True:
        boundary=f32(current*multiplier)
        if preliminary>=boundary:return current
        current=boundary;multiplier=two if multiplier==.75 else .75
def lcg(n):return (n*0x0bb38435+0x3619636b)&0xffffffff
def choice(m,state):
    state=lcg(state);test=f32(f32(state)*2**-33)
    state=lcg(state);u=f32(state);p=0.
    if f32(m-.5)>=test:
        p=f32(u*2**-32);state=lcg(state);u=f32(state)
    depth=f32(f32(m-frombits(0x3f19999a))*f32(2**-32*frombits(0x411ffbe7)))
    return p,int(f32(depth*u)) if m>frombits(0x3f19999a) else 0,state
def sparse(a,b,c,d,t):
    # Register-operation trace at 0x08028bca..0x08028c2e.
    s20=f32(d+a);s9=f32(s20-b);s20=f32(a+c);s9=f32(s9-c)
    s11=f32(c-a);s30=f32(t*.5);s14=fma32(s20,.5,b)
    s11=fma32(s9,s30,s11)
    return fma32(s11,t,s14)
def run():
    put_csv(ROOT/'analysis/control_curves.csv',['adc','morph_stage','sos_target_bits','gene_10s_bits','clocked_gene_10s_bits'],
       [(a,morph_stage(a),f'{bits(sos(a)):08x}',f'{bits(gene(a,480000)):08x}' if a>199 else '',
         f'{bits(max(8.,clock(gene(a,480000),480000))):08x}' if a>199 else '') for a in range(4096)])
    rational=[(2,1),(3,2),(4,3),(1,1),(4,5),(3,4),(2,3),(3,5),(4,7),(1,2),(4,9),(3,7),(2,5),(3,8),(4,11),(1,3),(4,13),(3,10),(2,7),(3,11),(4,15),(1,4)]
    rows=[]
    for s,(n,d) in enumerate(rational):
        aa=[a for a in range(4096) if morph_stage(a)==s]
        rows.append((s,min(aa),max(aa),f'{n}/{d}',repr(LAUNCH[s]),f'{bits(LAUNCH[s]):08x}',
                     repr(DENSITY[s]),CYCLE[s],bits(LAUNCH[s])==bits(n/d)))
    put_csv(ROOT/'tables/morph_active_stages.csv',['stage','adc_min','adc_max','nominal_launch_ratio','stored_launch','launch_bits',
            'stored_density','cycle_denominator','equals_rounded_rational'],rows)
    cases=[]
    values=[(-32768.,32767.,-123.,30000.),(100.,100.,100.,100.),(0.,1.,2.,3.),(0.,0.,1.,0.),(1.,0.,0.,0.),(0.,0.,0.,1.)]
    for vs in values:
        for t in [0.,.125,.25,.5,.75,.999,1.]:
            t=f32(t);cases.append((*vs,repr(t),f'{bits(sparse(*vs,t)):08x}'))
    put_csv(ROOT/'tests/sparse_read_vectors.csv',['a','b','c','d','fraction','result_bits'],cases)
    choices=[]
    for m in [.0,.5,.6,.7001,.8,.9,4095/4096]:
        for seed in [0,1,0xffffffff,1234567]:
            p,i,state=choice(f32(m),seed)
            choices.append((repr(f32(m)),seed,f'{bits(p):08x}',i,state))
    put_csv(ROOT/'tests/morph_choice_vectors.csv',['morph','seed','crossmix_bits','rate_index','next_state'],choices)
    put_json(ROOT/'analysis/reconstruction_scope.json',dict(
        components=['Gene Size ordinary curve','clock quantizer','Morph stage map','S.O.S. target/mix',
         'calibrated Vari-Speed target','LCG and one launch choice','sparse read kernel','dense linear kernel',
         'stereo crossmix','gain target/smoother','conditional record filter'],
        excluded=['whole DSP scheduling','peripheral execution','full envelope state machine','calibration-to-voltage transfer',
          'SD card and splice transactions','all RNG call timing','hardware output golden recordings'],
        reference='Finite-input arithmetic fixtures; no instruction-accurate emulator or hardware oracle'))
    print('Exported 4096 control cases, 22 stage ranges, 42 read-kernel and 28 launch-choice fixtures')
if __name__=='__main__':run()
