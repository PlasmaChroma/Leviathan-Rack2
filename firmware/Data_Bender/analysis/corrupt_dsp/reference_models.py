#!/usr/bin/env python3
"""Small readable reconstructions of Data Bender 1.4.7 Corrupt algorithms.

These are independently expressed analysis models, not manufacturer source.
Firmware addresses and qualifications are in recovered_corrupt_dsp.md.
Uses float32 rounding; libm and multiply-add edge rounding may differ by ulps.
All inputs/outputs use the firmware's internal float amplitude domain.
"""
import math, struct

def F(x):return struct.unpack('<f',struct.pack('<f',float(x)))[0]
def add(a,b):return F(float(a)+float(b))
def mul(a,b):return F(float(a)*float(b))
def div(a,b):return F(float(a)/float(b))
def fma(a,b,c):return F(float(a)*float(b)+float(c))
def clamp(x,a=0.,b=1.):return max(a,min(b,x))
def expf(x):return F(math.exp(x))
def sinf(x):return F(math.sin(x))
def cosf(x):return F(math.cos(x))
def s32(n):return ((n+0x80000000)&0xffffffff)-0x80000000

BIT_TABLE=list(map(F,[0.,0.,.58,.60,.17,.68,.69,.63,.43,.64,.49,.52,.54,.87,.74,.77,.80]))
RATE_TABLE=list(map(F,[0.,0.,.33,.51,.47,.49,.42,.82,.28,.73,.64,.26,.52,.17,.22,.96,.60]))

class Decimator:
    def __init__(self):self.counter=0;self.held=0.
    def configure(self,u):
        doubled=add(u,u)
        remainder=F(math.fmod(doubled,F(1.01)))
        self.index=int(math.floor(float(mul(remainder,16.)) + .5))
        self.crushed_bits=int(mul(16.,BIT_TABLE[self.index]))
        self.factor=mul(RATE_TABLE[self.index],1. if doubled>1. else .25)
        self.threshold=int(mul(mul(self.factor,self.factor),96.))
    def process(self,x):
        self.counter+=1
        if self.counter>self.threshold:self.counter=0;self.held=F(x)
        q=int(mul(self.held,65536.))
        q=(q>>self.crushed_bits)<<self.crushed_bits
        return F(q/65536.)

class Destroy:
    def __init__(self):self.blend=0.
    def configure(self,u):
        self.u=F(u)
        t=add(mul(self.u,self.u),mul(self.u,self.u))
        self.exponent=max(2.,fma(clamp(t),86.,1.))
        self.drive=fma(clamp(add(t,-1.)),8.,1.)
        self.attenuation=fma(sinf(mul(self.u,F(math.pi/2))),-.375,.5)
        self.target=1. if self.u>=F(.02) else 0.
    def shape(self,x):
        x=F(x)
        e=expf(mul(-abs(x),self.exponent))
        curved=add(1.,-e) if x>0 else add(e,-1.)
        return mul(clamp(mul(self.drive,curved),F(-.6),F(.6)),self.attenuation)
    def process(self,left,right):
        self.blend=fma(add(self.target,-self.blend),F(.001),self.blend)
        g=add(1.,-self.blend)
        return tuple(fma(F(x),g,mul(self.blend,self.shape(x))) for x in (left,right))

class Svf:
    def __init__(self,fs):
        self.fs=F(fs);self.res=F(.7);self.low=0.;self.band=0.
    def cutoff(self,hz):
        hz=clamp(F(hz),F(.000001),div(self.fs,3.))
        self.freq=mul(2.,sinf(mul(F(math.pi),min(.25,div(hz,add(self.fs,self.fs))))))
        self.damp=min(mul(2.,add(1.,-F(math.pow(self.res,.25)))),min(2.,fma(-.5,self.freq,div(2.,self.freq))))
    def process(self,x,kind):
        # Firmware drive is zero. Two state updates per external sample.
        low1=fma(self.band,self.freq,self.low)
        high1=add(fma(-self.damp,self.band,F(x)),-low1)
        band1=fma(self.freq,high1,self.band)
        low2=fma(self.freq,band1,low1)
        high2=add(fma(-self.damp,band1,F(x)),-low2)
        band2=fma(self.freq,high2,band1)
        self.low,self.band=low2,band2
        return fma(.5,low2,mul(.5,low1)) if kind=='low' else fma(.5,high2,mul(.5,high1))

class DjFilter:
    def __init__(self,fs):
        self.lp=[Svf(fs),Svf(fs)];self.hp=[Svf(fs),Svf(fs)]
    def configure(self,u):
        u=F(u)
        lo0,lo1=F(4.605170249938965),F(9.798127174377441)
        hi0,hi1=F(3.2188758850097656),F(8.006367683410645)
        self.low_hz=expf(fma(clamp(add(u,u)),add(lo1,-lo0),lo0))
        self.high_hz=expf(fma(clamp(fma(u,2.,-1.)),add(hi1,-hi0),hi0))
        for f in self.lp:f.cutoff(self.low_hz)
        for f in self.hp:f.cutoff(self.high_hz)
    def process(self,left,right):
        return tuple(F(math.tanh(self.hp[i].process(self.lp[i].process(x,'low'),'high'))) for i,x in enumerate((left,right)))

class ATone:
    def __init__(self,fs):self.fs=F(fs);self.prev=0.;self.coef=.5
    def cutoff(self,hz):
        b=add(2.,-cosf(div(mul(F(2*math.pi),hz),self.fs)))
        self.coef=add(b,-F(math.sqrt(fma(b,b,-1.))))
    def process(self,x):
        out=mul(add(x,self.prev),self.coef)
        self.prev=add(out,-x)
        return out

class Dust:
    def __init__(self,period,fs):self.period=period;self.counter=0;self.value=0.;self.dt=div(1.,fs)
    def configure(self,density,amp):self.threshold=mul(density,self.dt);self.scale=div(2.,self.threshold) if self.threshold>0 else 0.;self.amp=amp
    def process(self,rng):
        self.counter=(self.counter+1)%self.period
        if self.counter==0:
            r=rng()
            self.value=F(self.amp*(float(mul(r,self.scale))-1.)) if r<self.threshold else 0.
        return self.value

class Vinyl:
    def __init__(self,fs,seed=1):
        self.fs=F(fs);self.lcg=seed
        # Initializer draws five seeds from the shared LCG; individual seed
        # members aren't read in this processing routine.
        for _ in range(5):self.rng()
        self.dust=[Dust(p,self.fs) for p in (5,5,15,15,15)]
        self.noise_hp=[ATone(fs),ATone(fs)]
        for hp in self.noise_hp:hp.cutoff(3000.)
        self.signal_hp=[ATone(fs),ATone(fs)]
        self.slow_seed=1;self.fast_seed=1
        self.slow_counter=0;self.mod_counter=0;self.mod_period=int(mul(.5,self.fs))
        self.modulation=0.;self.slow_value=0.
    def rng(self):
        self.lcg=(1103515245*self.lcg+12345)&0x7fffffff
        return mul(F(self.lcg),F(2**-31))
    def configure(self,u):
        u=F(u);self.enabled=u>F(.05)
        t=add(expf(max(u,F(.001))),-1.)
        self.a=mul(F(float(t)/1.7183),1.25)
        for d in self.dust[:2]:d.configure(mul(500.,self.a),fma(self.a,F(.07),F(.05)))
        for d in self.dust[2:]:d.configure(mul(15.,self.a),fma(self.a,F(.2),.5))
        self.slow_amp=mul(self.a,F(.03));self.fast_amp=mul(self.a,F(.01))
        for hp in self.signal_hp:hp.cutoff(mul(700.,self.a))
        self.gain=fma(-self.a,F(.15),1.)
    def process(self,left,right):
        if not self.enabled:return F(left),F(right)
        self.slow_counter=(self.slow_counter+1)%10
        self.mod_counter=(self.mod_counter+1)%self.mod_period
        if self.mod_counter==0:self.modulation=mul(F(self.slow_seed/2**31),F(.3))
        if self.slow_counter==0:
            self.slow_seed=s32(self.slow_seed*16807)
            self.slow_value=mul(mul(F(self.slow_seed/2**31),self.slow_amp),self.modulation)
        self.fast_seed=s32(self.fast_seed*16807)
        hiss=mul(F(self.fast_seed/2**31),self.fast_amp)
        h0=self.noise_hp[0].process(add(add(self.dust[0].process(self.rng),self.slow_value),hiss))
        h1=self.noise_hp[1].process(add(add(self.dust[1].process(self.rng),self.slow_value),hiss))
        # Preserve the actual order because the RNG state is shared.
        d3=self.dust[3].process(self.rng)
        d2=self.dust[2].process(self.rng)
        d4=self.dust[4].process(self.rng)
        n0=add(add(fma(h1,F(.3),h0),d2),d4)
        n1=add(add(fma(h0,F(.3),h1),d3),d4)
        y0=fma(n0,F(1.2),mul(self.signal_hp[0].process(F(left)),self.gain))
        y1=fma(n1,F(1.2),mul(self.signal_hp[1].process(F(right)),self.gain))
        return y0,y1
