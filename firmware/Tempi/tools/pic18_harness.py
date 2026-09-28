#!/usr/bin/env python3
"""Small instruction-level PIC18 test harness for this image, not a hardware emulator.
Implements the legacy instructions encountered here. EEPROM read/write completes
immediately; ADC, timer clocks, interrupts and analog circuitry are NOT simulated.
Used to cross-check decoded routines and derive tables. No external dependencies.
"""
from pic18 import decode, read_hex, SKIPS
from pathlib import Path
class Machine:
    def __init__(self, flash):
        self.flash=flash;self.ram=bytearray(4096);self.eeprom=bytearray([255])*1024
        self.pc=0;self.stack=[];self.steps=0;self.cycles=0;self.ee_trace=[];self.hooks={};self.visited=set();self.decoded={}
    @property
    def w(self):return self.ram[0xfe8]
    @w.setter
    def w(self,v):self.ram[0xfe8]=v&255
    def u(self,a,n=2):return int.from_bytes(self.ram[a:a+n],'little')
    def put(self,a,v,n=2):self.ram[a:a+n]=(v&((1<<(n*8))-1)).to_bytes(n,'little')
    def flag(self,b):return (self.ram[0xfd8]>>b)&1
    def sf(self,b,v):self.ram[0xfd8]=(self.ram[0xfd8]&~(1<<b))|((bool(v))<<b)
    def zn(self,v):self.sf(2,(v&255)==0);self.sf(4,v&128)
    def add(self,a,b,c=0):
        v=a+b+c;self.sf(0,v>255);self.sf(1,(a&15)+(b&15)+c>15);self.sf(3,~(a^b)&(a^v)&128);self.zn(v);return v&255
    def sub(self,a,b,c=0):
        v=a-b-c;self.sf(0,v>=0);self.sf(1,(a&15)-(b&15)-c>=0);self.sf(3,(a^b)&(a^v)&128);self.zn(v);return v&255
    def addr(self,f):
        # Returns target address and deferred post-index update.
        for base in [0xfd9,0xfe1,0xfe9]:
            mode=f-base
            if 2<=mode<=6:
                ptr=self.u(base)&0xfff
                if mode==2:ptr=(ptr+(self.w if self.w<128 else self.w-256))&0xfff
                elif mode==3:ptr=(ptr+1)&0xfff;self.put(base,ptr)
                post=(base,(ptr+(-1 if mode==4 else 1))&0xfff) if mode in [4,5] else None
                if any(b+2<=ptr<=b+6 for b in [0xfd9,0xfe1,0xfe9]):return None,post
                return ptr,post
        return f,None
    def read(self,f):
        if f is None:return 0
        return self.ram[f]
    def write(self,f,v):
        if f is None:return
        self.ram[f]=v&255
        if f==0xfa6:
            a=self.u(0xfa9)&1023
            if v&1:
                self.ram[0xfa8]=self.eeprom[a];self.ram[f]&=~1;self.ee_trace.append(('read',a,self.ram[0xfa8],self.pc))
            if v&2:
                self.eeprom[a]=self.ram[0xfa8];self.ram[f]&=~2;self.ee_trace.append(('write',a,self.ram[0xfa8],self.pc))
    def finish(self,post):
        if post:self.put(*post)
    def fetch(self,f):
        a,p=self.addr(f);v=self.read(a);self.finish(p);return v
    def store(self,f,v):
        a,p=self.addr(f);self.write(a,v);self.finish(p)
    def ret(self):
        self.pc=self.stack.pop() if self.stack else -1
    def step(self):
        self.visited.add(self.pc)
        if self.pc in self.hooks:
            self.hooks[self.pc](self);self.ret();self.steps+=1;return
        i=self.decoded.get(self.pc)
        if i is None:i=decode(self.flash,self.pc);self.decoded[self.pc]=i
        m=i.mnemonic;n=self.pc+i.size
        self.steps+=1;self.cycles+=2 if i.size==4 or m in {'BRA','RCALL','RETURN','RETLW','RETFIE'} or m.startswith('TBL') else 1
        if m=='MOVLW':self.w=i.literal
        elif m=='MOVLB':self.ram[0xfe0]=i.literal
        elif m=='MOVFF':self.store(i.dst,self.fetch(i.src))
        elif m=='LFSR':self.put([0xfe9,0xfe1,0xfd9][i.f],i.literal)
        elif m=='MULLW':self.put(0xff3,self.w*i.literal)
        elif m in {'ADDLW','SUBLW','ANDLW','IORLW','XORLW'}:
            if m=='ADDLW':self.w=self.add(self.w,i.literal)
            elif m=='SUBLW':self.w=self.sub(i.literal,self.w)
            else:
                self.w={'ANDLW':self.w&i.literal,'IORLW':self.w|i.literal,'XORLW':self.w^i.literal}[m];self.zn(self.w)
        elif m in {'GOTO','BRA'}:n=i.target
        elif m in {'CALL','RCALL'}:self.stack.append(n);n=i.target
        elif m in {'RETURN','RETLW','RETFIE'}:
            if m=='RETLW':self.w=i.literal
            self.ret();return
        elif m in {'BZ','BNZ','BC','BNC','BN','BNN','BOV','BNOV'}:
            b={'BZ':2,'BNZ':2,'BC':0,'BNC':0,'BN':4,'BNN':4,'BOV':3,'BNOV':3}[m]
            if self.flag(b)==(m in {'BZ','BC','BN','BOV'}):n=i.target;self.cycles+=1
        elif m.startswith('TBLRD'):
            p=self.u(0xff6,3)&0x1fffff
            if m=='TBLRD+*':p=(p+1)&0x1fffff
            if p not in self.flash:raise RuntimeError(f'Table read from unrecovered address {p:#x} at {self.pc:#x}')
            self.ram[0xff5]=self.flash[p]
            if m=='TBLRD*+':p+=1
            if m=='TBLRD*-':p-=1
            self.put(0xff6,p,3)
        elif m in {'NOP','NOP_CONT','CLRWDT'}:pass
        elif i.f is not None:
            f=i.reg(self.ram[0xfe0]&15);a,p=self.addr(f);v=self.read(a);w=self.w;c=self.flag(0);r=None;dest=i.d;skip=False
            if m=='MOVF':r=v;self.zn(r)
            elif m=='MOVWF':r=w;dest=1
            elif m=='CLRF':r=0;dest=1;self.sf(2,1)
            elif m=='SETF':r=255;dest=1
            elif m=='NEGF':r=self.sub(0,v);dest=1
            elif m=='COMF':r=v^255;self.zn(r)
            elif m=='INCF':r=self.add(v,1)
            elif m=='DECF':r=self.sub(v,1)
            elif m=='ADDWF':r=self.add(v,w)
            elif m=='ADDWFC':r=self.add(v,w,c)
            elif m=='SUBWF':r=self.sub(v,w)
            elif m=='SUBWFB':r=self.sub(v,w,1-c)
            elif m=='SUBFWB':r=self.sub(w,v,1-c)
            elif m in {'ANDWF','IORWF','XORWF'}:
                r={'ANDWF':v&w,'IORWF':v|w,'XORWF':v^w}[m];self.zn(r)
            elif m in {'RLCF','RRCF','RLNCF','RRNCF'}:
                if m in {'RLCF','RLNCF'}:r=((v<<1)|(c if m=='RLCF' else v>>7))&255
                else:r=(v>>1)|((c if m=='RRCF' else v&1)<<7)
                if m=='RLCF':self.sf(0,v>>7)
                if m=='RRCF':self.sf(0,v&1)
                self.zn(r)
            elif m=='SWAPF':r=((v<<4)|(v>>4))&255
            elif m in {'BSF','BCF','BTG'}:
                r={'BSF':v|(1<<i.bit),'BCF':v&~(1<<i.bit),'BTG':v^(1<<i.bit)}[m];dest=1
            elif m in {'BTFSC','BTFSS'}:skip=((v>>i.bit)&1)==(m=='BTFSS')
            elif m in {'CPFSEQ','CPFSGT','CPFSLT','TSTFSZ'}:skip={'CPFSEQ':v==w,'CPFSGT':v>w,'CPFSLT':v<w,'TSTFSZ':v==0}[m]
            elif m in {'DECFSZ','DCFSNZ','INCFSZ','INFSNZ'}:
                r=(v+(-1 if m in {'DECFSZ','DCFSNZ'} else 1))&255;skip=(r==0) if m in {'DECFSZ','INCFSZ'} else (r!=0)
            elif m=='MULWF':self.put(0xff3,v*w)
            else:raise NotImplementedError((hex(self.pc),i.text(self.ram[0xfe0])))
            if r is not None:
                if dest:self.write(a,r)
                else:self.w=r
            self.finish(p)
            if skip:
                sz=decode(self.flash,n).size;n+=sz;self.cycles+=sz//2
        else:raise NotImplementedError((hex(self.pc),i.text()))
        self.pc=n
    def run(self,start,w=None,stop=None,max_steps=2000000):
        self.pc=start;self.stack=[]
        if w is not None:self.w=w
        before=self.steps
        while self.pc!=-1 and self.pc!=stop:
            if self.steps-before>=max_steps:raise RuntimeError(f'Step limit at {self.pc:#x}')
            self.step()
        return self.w

def initialized():
    root=Path(__file__).resolve().parents[1];m=Machine(read_hex(root/'firmware/tempi71_recovered.hex'))
    m.run(0x779a,stop=0x8998)
    return m
if __name__=='__main__':
    m=initialized();print('Runtime initialization executed:',m.steps,'instructions')
    print('Select-bus header:',m.ram[0x5ee:0x5f2].hex(' '))
    for s in [0,1,63]:
        m.ee_trace=[];m.run(0x87fa,w=s)
        print('State',s,'EEPROM reads:',[hex(x[1]) for x in m.ee_trace])
