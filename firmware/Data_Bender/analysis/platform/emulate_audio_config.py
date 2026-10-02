"""Narrow emulation of immutable initialization code; hardware/library calls stubbed explicitly.
This executes local authorized firmware, not device flashing. It validates the config
values passed to libDaisy/HAL, and does not claim full hardware emulation.
"""
from pathlib import Path
import struct,json
from unicorn import Uc, UC_ARCH_ARM, UC_MODE_THUMB, UC_HOOK_CODE
from unicorn.arm_const import *
ROOT=Path(__file__).resolve().parents[2]
b=(ROOT/'upload/Data_Bender_v1_4_7.bin').read_bytes()
u=Uc(UC_ARCH_ARM,UC_MODE_THUMB)
for a,s in [(0x08000000,0x20000),(0x20000000,0x20000),(0x24000000,0x80000),(0x40015000,0x1000),(0x58024000,0x1000),(0x58000000,0x1000),(0xE000E000,0x1000)]:u.mem_map(a,s)
u.mem_write(0x8000000,b)
write=lambda a,*v:u.mem_write(a,struct.pack('<'+'I'*len(v),*v))
read=lambda a,n:list(struct.unpack('<'+'I'*n,u.mem_read(a,n*4)))
reg=lambda n:u.reg_read(n)
log=[]
def ret(v=0):u.reg_write(UC_ARM_REG_R0,v);u.reg_write(UC_ARM_REG_PC,reg(UC_ARM_REG_LR))
def run(addr,r0,r1=0,r2=0,r3=0):
 for n,v in [(UC_ARM_REG_R0,r0),(UC_ARM_REG_R1,r1),(UC_ARM_REG_R2,r2),(UC_ARM_REG_R3,r3),(UC_ARM_REG_SP,0x2001ff00),(UC_ARM_REG_LR,0x8000001)]:u.reg_write(n,v)
 u.emu_start(addr|1,0x8000000,count=100000)

def hook(u,addr,size,_):
 r0,r1,r2,r3=(reg(x) for x in [UC_ARM_REG_R0,UC_ARM_REG_R1,UC_ARM_REG_R2,UC_ARM_REG_R3])
 if addr==0x8005c6c:ret(0) # Original Daisy Seed
 elif addr==0x8005ef4:log.append({'call':'AK4556::Init','reset_pin_u16':r1&65535});ret(0)
 elif addr==0x800933c:
  config=bytes(u.mem_read(r1,40));u.mem_write(0x24005000,config);write(r0,0x24005000)
  log.append({'call':'SaiHandle::Init','words':read(r1,10)});ret(0)
 elif addr==0x8006ebc:
  log.append({'call':'AudioHandle::Init','words':read(r1,4)});ret(0)

h=u.hook_add(UC_HOOK_CODE,hook)
run(0x8005cd8,0x24000468)
u.hook_del(h)
# Replay the captured SAI config in the actual SaiHandle::Impl::Init.
def hook2(u,addr,size,_):
 if addr==0x8011e1c:
  r0=reg(UC_ARM_REG_R0)
  log.append({'call':'HAL_SAI_InitProtocol','handle':hex(r0),'AudioFrequency':read(r0+0x20,1)[0], 'protocol':reg(UC_ARM_REG_R1),'data_size':reg(UC_ARM_REG_R2),'slots':reg(UC_ARM_REG_R3),'handle_words_before_protocol':read(r0,30)})
  ret(0)
h=u.hook_add(UC_HOOK_CODE,hook2)
run(0x8008d3c,0x24005200,0x24005000)
u.hook_del(h)
# Replay HAL protocol setup; stop before full HAL init.
def hook3(u,addr,size,_):
 if addr==0x8011a68:
  r0=reg(UC_ARM_REG_R0)
  log.append({'call':'HAL_SAI_Init','handle':hex(r0),'handle_words_after_protocol':read(r0,30)})
  ret(0)
h=u.hook_add(UC_HOOK_CODE,hook3)
run(0x8011e1c,0x24005228,1,2,2)
u.hook_del(h)
# Replay HAL_SAI_Init with explicit kernel clock injection. This verifies
# the firmware's divider arithmetic, but physical crystal is an assumption.
write(0x24000050,400000000) # HAL timeout SystemCoreClock
write(0x40015804,0)
def hook4(u,addr,size,_):
 if addr==0x8009d18:ret(0x2000) # MCU silicon revision
 elif addr==0x80090c0:ret(0) # HAL MSP peripheral setup
 elif addr==0x80116d8:ret(49166666) # 16MHz * 295 /6 /16, nominal PLL3_P
h=u.hook_add(UC_HOOK_CODE,hook4)
run(0x8011a68,0x24005228)
u.hook_del(h)
log.append({'HAL_SAI_divider':read(0x24005228+0x24,1)[0],'SAI1_BlockA_registers':[hex(v) for v in read(0x40015804,4)]})
# Capture actual system clock setup without running oscillator waits.
write(0x58024818,0x2000)  # PWR voltage-scaling ready status
write(0x24006000,0,0)    # System::Config::Defaults (no boost)
def hook5(u,addr,size,_):
 r0,r1=(reg(x) for x in [UC_ARM_REG_R0,UC_ARM_REG_R1])
 if addr==0x800f084:ret(0) # HAL_PWREx_ConfigSupply
 elif addr==0x800fb00:
  log.append({'call':'HAL_RCC_OscConfig','config_words':read(r0,19)});ret(0)
 elif addr==0x8010138:
  log.append({'call':'HAL_RCC_ClockConfig','config_words':read(r0,8),'flash_latency':r1});ret(0)
 elif addr==0x8010638:
  log.append({'call':'HAL_RCCEx_PeriphCLKConfig','config_words':read(r0,48)});ret(0)
 elif addr==0x800f0cc:ret(0) # HAL_PWREx_EnableUSBVoltageDetector
h=u.hook_add(UC_HOOK_CODE,hook5)
run(0x8009550,0x24006000)
u.hook_del(h)
print(json.dumps(log,indent=2))
(ROOT/'analysis/platform/audio_config_emulation.json').write_text(json.dumps(log,indent=2)+'\n')
