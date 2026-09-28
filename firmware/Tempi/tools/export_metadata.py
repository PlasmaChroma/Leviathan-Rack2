#!/usr/bin/env python3
"""Export analyst maps and exact constant bytes. Labels are not recovered symbols."""
from pathlib import Path
import csv,json,re
from pic18_harness import initialized
ROOT=Path(__file__).resolve().parents[1];OUT=ROOT/'analysis'
def csvout(name,rows):
    with (OUT/name).open('w',newline='') as f:
        w=csv.DictWriter(f,fieldnames=list(rows[0]));w.writeheader();w.writerows(rows)
def main():
    m=initialized();io=[]
    def add(signal,pin,direction,evidence,confidence,note=''):
        io.append(dict(signal=signal,mcu_signal=pin,direction=direction,evidence_byte_address=evidence,confidence=confidence,note=note))
    for c,b in enumerate([5,3,1,2,4,0]):add(f'CH{c+1} clock output',f'RB{b}','output',f'0x{0x83c+12*c:06X}..0x000880','High: logical lane to GPIO','Jack wiring and voltage buffer not physically traced')
    for c,(pin,pc) in enumerate(zip(['RC2','RC5','RD7','RC1','RC6','RD6'],[0x11f2,0x11fa,0x1202,0x120a,0x1212,0x121a])):
        add(f'CH{c+1} button',pin,'input',f'0x{pc:06X}; 0x00FE0D mask table','High','Active-low software interpretation')
    add('PGM_A button','RC0','input','0x001222','High','Active-low')
    add('PGM_B button','RE2','input','0x00122A','High','Active-low')
    add('Leading tempo input','RA4','input','0x000884; 0x004B16','High','Rising-edge interval capture')
    add('MOD gate input','RA0','input','ISR after UART; 0x004F66','High inference','Gate level and rising-edge toggle tracked')
    add('State select gate','RA5','input','0x007A5C..0x007BB6','High inference','State stepping; also read by LED service')
    add('State CV / combo control','RA3 / AN3','analog input','0x008214; 0x008240; 0x0078FC','High inference','Only analog channel selected; external summing/attenuation unknown')
    add('Select Bus serial receive','RC7 / RX1','input','0x008252..0x00826A; 0x0071AA','High inference','USART1 receive enabled; transmit disabled')
    add('LED serial data','RD5','output','0x009742; 0x009756','High','Part number of external LED driver not recovered')
    add('LED serial clock','RD2','output','0x009742; 0x009756','High','Software bit-banged')
    add('LED latch','RD4','output','0x00979E','High','16-bit shift words, LSB-first')
    add('Other LED/UI lines','RC3, RC4, RD0, RD1, RD3','output','0x003F32..0x004668','Medium','Exact lamp/color assignments not fully resolved')
    add('Unresolved configured outputs','RE0, RE1','output configuration','0x008226..0x008228','Unresolved','No explicit LATE write identified; do not infer attached circuit')
    csvout('io_map.csv',io)
    ram=[]
    def r(a,n,name,typ,evidence,confidence='High'):
        ram.append(dict(address=f'0x{a:03X}',bytes=n,analyst_name=name,interpretation=typ,evidence=evidence,confidence=confidence))
    for args in [
        (0x30,12,'channel_source_pointers','6 x uint16 data-memory pointers','ISR; main'),
        (0x3e,1,'current_state','0..63','0x8540; 0x88D4'),
        (0x41,4,'master_current_halfperiod','uint32 ticks','ISR'),
        (0x45,4,'master_requested_halfperiod','uint32 ticks','0x65DE; ISR'),
        (0x49,4,'master_countdown','int32 ticks','ISR'),
        (0x4d,1,'constant_true_source','uint8 = 1','C runtime; main'),
        (0x400,24,'human_elapsed_timers','6 x uint32 ticks','ISR; 0x340A'),
        (0x418,24,'human_captured_intervals','6 x uint32 ticks','ISR; 0x340A'),
        (0x430,6,'human_timer_enables','6 x uint8','ISR'),
        (0x436,6,'channel_wave_levels','6 x uint8','ISR'),
        (0x43c,6,'pending_output_levels','6 x uint8','0x083C..0x0880; ISR'),
        (0x446,4,'external_tempo_elapsed','uint32 ticks','ISR'),
        (0x44e,2,'uart_ring_write_index','uint16; wraps 460','ISR'),
        (0x450,2,'mod_source_pointer','data-memory pointer','0x8DC2; ISR'),
        (0x454,2,'active_low_button_mask','~(PORTC | PORTD<<8) & 0xC066','0x0EA4..0x0EC0'),
        (0x456,2,'master_alignment_counter','uint16','ISR'),
        (0x45d,1,'sysex_payload_index','uint8','0x71AA'),
        (0x45e,1,'serial_status','C0/F0/F4 or zero','0x71AA'),
        (0x47e,1,'master_wave_level','uint8','ISR'),
        (0x47f,1,'global_timing_dirty','boolean','0x8540; 0x65DE'),
        (0x493,1,'follow_enable','boolean','0x7D08; 0x7BBA; 0x8DC2'),
        (0x498,72,'six_channel_timer_records','6 x {uint32 current,uint32 next,int32 remaining}','ISR'),
        (0x4e0,6,'channel_output_enable','6 x uint8','0x8540; ISR'),
        (0x4e6,2,'previous_adc_state','uint16','0x78FC'),
        (0x4f5,1,'mod_toggle_level','uint8','ISR; 0x4F66'),
        (0x4f6,1,'mod_gate_level','uint8','ISR; 0x4F66'),
        (0x500,32,'state_adc_thresholds','16 x uint16','0x5B66; 0x78FC'),
        (0x520,24,'computed_phase_offsets','6 x int32 ticks','0x65DE'),
        (0x538,12,'active_ratio_codes','6 x int16, EEPROM sign-extended','0x8540; 0x7E54'),
        (0x544,8,'mesh_state_bitmap','64 bits','0x7D08'),
        (0x55e,6,'previous_channel_source_levels','6 x uint8','ISR'),
        (0x570,6,'active_phase_codes','6 x uint8','0x8540; 0x65DE'),
        (0x586,4,'captured_external_tempo_interval','uint32','ISR; 0x4B16'),
        (0x595,2,'uart_ring_read_index','uint16; wraps 460','0x71AA'),
        (0x5a3,1,'adc_nonblocking_state','0=start,1=busy,2=read','0x9318'),
        (0x5b6,8,'lcg_state','uint64','0x8446'),
        (0x5be,24,'computed_channel_halfperiods','6 x uint32','0x65DE'),
        (0x5dc,6,'channel_mod_enable','6 x uint8','0x8540; 0x4F66'),
        (0x5e2,6,'per_channel_timing_dirty','6 x uint8','0x65DE'),
        (0x5e8,6,'channel_run_gate','6 x uint8','ISR; 0x4F66'),
        (0x5ee,3,'select_bus_header','00 02 2D','C runtime; 0x71AA'),
        (0x5f1,1,'select_bus_subcommand','uint8','0x71AA'),
        (0x5f3,2,'last_adc_result','uint16','0x9318'),
        (0x5f7,2,'master_alignment_length','uint16 master cycles','0x93FC; ISR'),
        (0x5fa,1,'requested_serial_state','uint8','0x71AA; 0x7A5C'),
        (0x65c,16,'ui_button_debounce','8 x uint16','0x11C0'),
        (0x66c,12,'fast_channel_button_debounce','6 x uint16','0x7336'),
        (0x678,12,'ratio_alignment_factors','6 x uint16','0x8BCC; 0x65DE'),
        (0x6a0,8,'dirty_state_bitmap','64 bits','0x8B16'),
        (0x700,120,'master_history','10 x 12-byte timing snapshots','ISR'),
        (0x800,64,'cached_mod_masks','uint8[64]','0x87FA; 0x8540'),
        (0x840,64,'cached_output_enable_masks','uint8[64]','0x87FA; 0x8540'),
        (0x9ae,64,'tempo_control_table','16 x uint32 copied from 0x9442','C runtime; 0x4B16'),
        (0xa60,460,'uart_receive_ring','uint8[460]','ISR; 0x71AA'),
        (0xc2c,384,'cached_ratio_codes','int8[64][6]','0x87FA; 0x8540'),
        (0xdac,384,'cached_phase_codes','uint8[64][6]','0x87FA; 0x8540'),
    ]:r(*args)
    csvout('ram_symbols.csv',ram)
    tables=[]
    for a,b,label in [(0x92b8,0x9317,'runtime initialization block'),(0x9442,0x9481,'tempo-control integer table'),
        (0x9482,0x94bf,'runtime initialization block'),(0x977e,0x978f,'runtime initialization block'),
        (0x97ac,0x97cd,'runtime initialization blocks'),(0xfd40,0xfd4c,'constant prefix'),
        (0xfd4d,0xfdac,'factory phase bank A'),(0xfdad,0xfe0c,'factory ratio bank A'),
        (0xfe0d,0xfe18,'channel button masks'),(0xfe19,0xfe3f,'LED / bit masks and remaining constants')]:
        tables.append(dict(start=f'0x{a:06X}',end=f'0x{b:06X}',analyst_label=label,hex=bytes(m.flash[x] for x in range(a,b+1)).hex(' ')))
    (OUT/'constant_tables.json').write_text(json.dumps(tables,indent=2)+'\n')
    csvout('tempo_control_table.csv',[dict(index=j,flash_address=f'0x{0x9442+4*j:06X}',runtime_address=f'0x{0x9ae+4*j:03X}',integer_value=m.u(0x9ae+4*j,4)) for j in range(16)])
    (OUT/'channel_button_masks.json').write_text(json.dumps([f'0x{m.flash[0xfe0d+2*c]|m.flash[0xfe0e+2*c]<<8:04X}' for c in range(6)],indent=2)+'\n')
    # Strings are raw candidates: instruction bytes routinely resemble text.
    candidates=[]
    for name in ['tempi71_000800_0097df.bin','tempi71_00fd40_00fe3f.bin']:
        bb=(ROOT/'firmware'/name).read_bytes();base=int(name.split('_')[1],16)
        for match in re.finditer(rb'[ -~]{6,}',bb):candidates.append(f'{name} 0x{base+match.start():06X} {match.group().decode("ascii")}')
    (OUT/'strings_candidates.txt').write_text('Raw ASCII candidates only. NOT authenticated source symbols, messages, versions, or compiler identification.\n'+'\n'.join(candidates)+'\n')
    configs=[('CONFIG1L',0xff,'Unimplemented/reserved byte'),('CONFIG1H',0x32,'HSHP primary oscillator; PLL enabled; primary clock enabled; fail-safe and switchover off'),
        ('CONFIG2L',0x19,'Power-up timer and brown-out reset disabled'),('CONFIG2H',0x3c,'Watchdog disabled; watchdog postscale field 32768'),
        ('CONFIG3L',0xff,'Unimplemented/reserved byte'),('CONFIG3H',0xb5,'MCLR enabled; PORTB analog-at-reset disabled; peripheral pin options'),
        ('CONFIG4L',0x81,'Legacy instruction set; low-voltage programming off; stack reset on'),('CONFIG4H',0xff,'Unimplemented/reserved byte'),
        ('CONFIG5L',0,'Application code protection enabled for blocks 0..3'),('CONFIG5H',0x80,'Boot code protection enabled; EEPROM read protection off'),
        ('CONFIG6L',0x0f,'Application write protection off'),('CONFIG6H',0x80,'Boot/config write protection enabled; EEPROM write protection off'),
        ('CONFIG7L',0x0f,'Application table-read protection off'),('CONFIG7H',0x40,'Boot table-read protection off')]
    csvout('configuration_bytes.csv',[dict(address=f'0x{0x300000+j:06X}',name=n,value=f'{v:02X}',interpretation=interp,
        caveat='Interpretation assumes PIC18(L)F46K22; update records are not hardware fuse readback') for j,(n,v,interp) in enumerate(configs)])
    globals_=[(0x380,'0x495','MOD shift-mode field',0),(0x381,'0x450..451','Select raw MOD level (4F6) versus latched MOD toggle (4F5)',1),
        (0x382,'0x040','MOD run/stop mode field',1),(0x383,'0x4F3','Human resolution field',2),
        (0x384,'Unresolved','Global setting not fully named',0),(0x3b0,'0x4F1','Clock-width / tap-related bitfield; exact bit semantics incomplete',None),
        (0x3b1,'Unresolved','Global setting not fully named',1),(0x3f0,'Unresolved','Factory routine writes 0x3F here',63),
        (0x3f5,'0x493','Select Bus follow',0),(0x3f6,'0x045..048 on load','Three-byte stored leading halfperiod (3F6..3F8)',8000),
        (0x3fd,'0x03E','Stored state index',0),(0x3fe,'0x6FD on startup','Initialization marker / boot check',None)]
    csvout('global_eeprom_partial_map.csv',[dict(address=f'0x{a:03X}',ram=ra,interpretation=desc,factory_value='' if v is None else v) for a,ra,desc,v in globals_])
    print('Exported GPIO, RAM, constants, fuse, strings, and partial global EEPROM maps.')
if __name__=='__main__':main()
