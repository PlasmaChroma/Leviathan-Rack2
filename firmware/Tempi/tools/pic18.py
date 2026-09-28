#!/usr/bin/env python3
"""PIC18 legacy ISA decoder, written for this investigation.
Numeric encoding reference: Microchip DS40001412G, Table 25-2.
It is NOT a decompiler. Bank-dependent operands stay explicit unless supplied.
All program addresses are byte addresses; data registers have a separate space.
"""
from dataclasses import dataclass,asdict
from typing import Optional

# PIC18(L)F2X/4XK22 SFR layout. Device-family hypothesis, not a silicon-ID read.
SFR={}
def regs(base,names):
    for i,n in enumerate(names.split()):
        if n!='-': SFR[base+i]=n
regs(0xf38,'ANSELA ANSELB ANSELC ANSELD ANSELE PMD2 PMD1 PMD0 DACCON1 DACCON0 FVRCON CTMUICON CTMUCON1 CTMUCON0 SRCON1 SRCON0 CCPTMRS1 CCPTMRS0 T6CON PR6 TMR6 T5GCON T5CON TMR5L TMR5H T4CON PR4 TMR4 CCP5CON CCPR5L CCPR5H CCP4CON CCPR4L CCPR4H PSTR3CON CCP3AS PWM3CON CCP3CON CCPR3L CCPR3H SLRCON WPUB IOCB PSTR2CON CCP2AS PWM2CON CCP2CON CCPR2L CCPR2H SSP2CON3 SSP2MSK SSP2CON2 SSP2CON1 SSP2STAT SSP2ADD SSP2BUF BAUDCON2 RCSTA2 TXSTA2 TXREG2 RCREG2 SPBRG2 SPBRGH2 CM2CON1 CM2CON0 CM1CON0 PIE4 PIR4 IPR4 PIE5 PIR5 IPR5 PORTA PORTB PORTC PORTD PORTE')
regs(0xf89,'LATA LATB LATC LATD LATE')
regs(0xf92,'TRISA TRISB TRISC TRISD TRISE')
regs(0xf9b,'OSCTUNE HLVDCON PIE1 PIR1 IPR1 PIE2 PIR2 IPR2 PIE3 PIR3 IPR3 EECON1 EECON2 EEDATA EEADR EEADRH RCSTA1 TXSTA1 TXREG1 RCREG1 SPBRG1 SPBRGH1 T3CON TMR3L TMR3H T3GCON - ECCP1AS PWM1CON BAUDCON1 PSTR1CON T2CON PR2 TMR2 CCP1CON CCPR1L CCPR1H ADCON2 ADCON1 ADCON0 ADRESL ADRESH SSP1CON2 SSP1CON1 SSP1STAT SSP1ADD SSP1BUF SSP1MSK SSP1CON3 T1GCON T1CON TMR1L TMR1H RCON WDTCON OSCCON2 OSCCON - T0CON TMR0L TMR0H STATUS FSR2L FSR2H PLUSW2 PREINC2 POSTDEC2 POSTINC2 INDF2 BSR FSR1L FSR1H PLUSW1 PREINC1 POSTDEC1 POSTINC1 INDF1 WREG FSR0L FSR0H PLUSW0 PREINC0 POSTDEC0 POSTINC0 INDF0 INTCON3 INTCON2 INTCON PRODL PRODH TABLAT TBLPTRL TBLPTRH TBLPTRU PCL PCLATH PCLATU STKPTR TOSL TOSH TOSU')
BYTE_D={0x0400:'DECF',0x1000:'IORWF',0x1400:'ANDWF',0x1800:'XORWF',0x1c00:'COMF',0x2000:'ADDWFC',0x2400:'ADDWF',0x2800:'INCF',0x2c00:'DECFSZ',0x3000:'RRCF',0x3400:'RLCF',0x3800:'SWAPF',0x3c00:'INCFSZ',0x4000:'RRNCF',0x4400:'RLNCF',0x4800:'INFSNZ',0x4c00:'DCFSNZ',0x5000:'MOVF',0x5400:'SUBFWB',0x5800:'SUBWFB',0x5c00:'SUBWF'}
BYTE={0x0200:'MULWF',0x6000:'CPFSLT',0x6200:'CPFSEQ',0x6400:'CPFSGT',0x6600:'TSTFSZ',0x6800:'SETF',0x6a00:'CLRF',0x6c00:'NEGF',0x6e00:'MOVWF'}
LIT={0x0f00:'ADDLW',0x0800:'SUBLW',0x0900:'IORLW',0x0a00:'XORLW',0x0b00:'ANDLW',0x0c00:'RETLW',0x0d00:'MULLW',0x0e00:'MOVLW'}
EXACT={0:'NOP',3:'SLEEP',4:'CLRWDT',5:'PUSH',6:'POP',7:'DAW',8:'TBLRD*',9:'TBLRD*+',10:'TBLRD*-',11:'TBLRD+*',12:'TBLWT*',13:'TBLWT*+',14:'TBLWT*-',15:'TBLWT+*',0xff:'RESET'}
SKIPS={'BTFSC','BTFSS','CPFSEQ','CPFSGT','CPFSLT','TSTFSZ','DECFSZ','DCFSNZ','INCFSZ','INFSNZ'}
CONDS={'BZ','BNZ','BC','BNC','BOV','BNOV','BN','BNN'}
@dataclass
class Ins:
    address:int
    word:int
    size:int=2
    mnemonic:str='DW'
    f:Optional[int]=None
    a:Optional[int]=None
    d:Optional[int]=None
    bit:Optional[int]=None
    literal:Optional[int]=None
    target:Optional[int]=None
    src:Optional[int]=None
    dst:Optional[int]=None
    fast:Optional[int]=None
    word2:Optional[int]=None
    def reg(self,bank=None):
        if self.f is None:return None
        if self.a:return (bank<<8)|self.f if bank is not None else None
        return self.f if self.f<0x60 else self.f+0xf00
    def text(self,bank=None):
        def rn(f):return SFR.get(f,f'ram_{f:03X}')
        m=self.mnemonic;args=[]
        if m=='MOVFF':args=[rn(self.src),rn(self.dst)]
        elif m=='LFSR':args=[str(self.f),f'0x{self.literal:03X}']
        elif self.target is not None:
            args=[f'0x{self.target:06X}']
            if m=='CALL':args.append(str(self.fast))
        elif self.f is not None:
            r=self.reg(bank);args=[rn(r) if r is not None else f'[BSR:0x{self.f:02X}]']
            if self.bit is not None:args.append(str(self.bit))
            if self.d is not None:args.append('F' if self.d else 'W')
            args.append('B' if self.a else 'A')
        elif self.literal is not None:args=[f'0x{self.literal:02X}']
        elif self.fast is not None:args=[str(self.fast)]
        elif m=='DW':args=[f'0x{self.word:04X}']
        return m+(' '+', '.join(args) if args else '')
def decode(mem,pc):
    w=mem.get(pc,0xff)|(mem.get(pc+1,0xff)<<8); q=mem.get(pc+2,0xff)|(mem.get(pc+3,0xff)<<8)
    i=Ins(pc,w)
    if w&0xf000==0xc000:
        if q&0xf000!=0xf000:return i
        i.mnemonic='MOVFF';i.src=w&0xfff;i.dst=q&0xfff;i.size=4;i.word2=q
    elif w&0xfe00==0xec00 or w&0xff00==0xef00:
        if q&0xf000!=0xf000:return i
        i.mnemonic='GOTO' if w&0xff00==0xef00 else 'CALL';i.target=(((q&0xfff)<<8)|(w&255))*2;i.size=4;i.word2=q
        if i.mnemonic=='CALL':i.fast=(w>>8)&1
    elif w&0xffc0==0xee00:
        if q&0xff00!=0xf000:return i
        i.mnemonic='LFSR';i.f=(w>>4)&3;i.literal=((w&15)<<8)|(q&255);i.size=4;i.word2=q
    elif w&0xf800 in [0xd000,0xd800]:
        i.mnemonic='BRA' if w&0xf800==0xd000 else 'RCALL';v=w&0x7ff;v=v-0x800 if v&0x400 else v;i.target=pc+2+2*v
    elif w&0xf800==0xe000:
        i.mnemonic=['BZ','BNZ','BC','BNC','BOV','BNOV','BN','BNN'][(w>>8)&7];v=w&255;v=v-256 if v&128 else v;i.target=pc+2+2*v
    elif w&0xf000 in [0x7000,0x8000,0x9000,0xa000,0xb000]:
        i.mnemonic={7:'BTG',8:'BSF',9:'BCF',10:'BTFSS',11:'BTFSC'}[w>>12];i.f=w&255;i.a=(w>>8)&1;i.bit=(w>>9)&7
    elif w&0xfc00 in BYTE_D:
        i.mnemonic=BYTE_D[w&0xfc00];i.f=w&255;i.a=(w>>8)&1;i.d=(w>>9)&1
    elif w&0xfe00 in BYTE:
        i.mnemonic=BYTE[w&0xfe00];i.f=w&255;i.a=(w>>8)&1
    elif w&0xff00 in LIT:i.mnemonic=LIT[w&0xff00];i.literal=w&255
    elif w&0xfff0==0x0100:i.mnemonic='MOVLB';i.literal=w&15
    elif w&0xfffe in [0x10,0x12]:i.mnemonic='RETFIE' if w&0xfffe==0x10 else 'RETURN';i.fast=w&1
    elif w in EXACT:i.mnemonic=EXACT[w]
    elif w&0xf000==0xf000:i.mnemonic='NOP_CONT'
    return i

def read_hex(path):
    mem={};base=0
    for line in path.read_text().splitlines():
        b=bytes.fromhex(line[1:]);assert sum(b)%256==0
        n=b[0];a=int.from_bytes(b[1:3],'big');t=b[3];data=b[4:-1]
        if t==4:base=int.from_bytes(data,'big')<<16
        elif t==2:base=int.from_bytes(data,'big')<<4
        elif t==0:
            for j,x in enumerate(data):mem[base+a+j]=x
    return mem

if __name__=='__main__':
    import argparse
    from pathlib import Path
    ap=argparse.ArgumentParser();ap.add_argument('hexfile',type=Path);ap.add_argument('--start',type=lambda s:int(s,0),default=0x800);ap.add_argument('--end',type=lambda s:int(s,0),default=0x97e0);a=ap.parse_args();mem=read_hex(a.hexfile);pc=a.start;bank=None
    while pc<a.end:
        i=decode(mem,pc)
        if i.mnemonic=='MOVLB':bank=i.literal
        print(f'{pc:06X}: {i.word:04X}'+(f' {i.word2:04X}' if i.size==4 else '     ')+f'  {i.text(bank)}')
        if i.mnemonic in {'CALL','RCALL','GOTO','RETURN','RETFIE','BRA','RETLW'}:bank=None
        pc+=i.size
