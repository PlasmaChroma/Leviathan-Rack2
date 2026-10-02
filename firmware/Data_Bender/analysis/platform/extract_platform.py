"""Recover immutable platform metadata from supplied Data Bender image."""
from pathlib import Path
import struct,json,csv,hashlib
ROOT=Path(__file__).resolve().parents[2]
b=(ROOT/'upload/Data_Bender_v1_4_7.bin').read_bytes()
buttons=['Shift','Clock','Mode','Freeze','Bend','Break','Corrupt']
params=['Time','Repeats','Mix','Bend','Break','Corrupt']
gates=['Freeze','Bend','Break','Corrupt','Clock']
rows=[]
for kind,off,n,names in [('button',0x19c20,7,buttons),('gate',0x19c3c,5,gates),('knob',0x19c50,6,params),('cv',0x19c68,6,params)]:
 for idx,d in enumerate(struct.unpack_from('<'+str(n)+'I',b,off)):
  port,pin=struct.unpack_from('<BB',b,0x19e08+2*d)
  rows.append(dict(kind=kind,index=idx,name=names[idx],daisy_pin='D'+str(d),mcu_pin=f'P{chr(65+port)}{pin}',table_va=hex(0x8000000+off+idx*4),name_evidence='cross-referenced UI polling by controls agent'))
for role,d in [('LED I2C SCL',11),('LED I2C SDA',12)]:
 port,pin=struct.unpack_from('<BB',b,0x19e08+2*d)
 rows.append(dict(kind='bus',index='',name=role,daisy_pin='D'+str(d),mcu_pin=f'P{chr(65+port)}{pin}',table_va='instruction immediates',name_evidence='hardware initialization'))
p=ROOT/'analysis/platform/pin_map.csv'
with p.open('w') as f:
 w=csv.DictWriter(f,fieldnames=rows[0]);w.writeheader();w.writerows(rows)
labels={
0x080009fc:'Default_Handler',0x08000a08:'Reset_Handler',0x080032fc:'DataBender_AudioCallback_interleaved',0x08003878:'main',0x0800572c:'DataBenderHardware_Init',0x08005be8:'DaisySeed_Configure',0x08005bec:'DaisySeed_GetPin',0x08005c18:'DaisySeed_StartAudio_interleaved',0x08005c20:'DaisySeed_AudioSampleRate',0x08005c28:'DaisySeed_SetAudioBlockSize',0x08005c60:'DaisySeed_AudioBlockSize',0x08005c6c:'DaisySeed_CheckBoardVersion',0x08005cd8:'DaisySeed_ConfigureAudio',0x08005e00:'DaisySeed_Init',0x08005ef4:'Ak4556_Init',0x08005f38:'Wm8731_WriteRegister',0x08005f74:'Wm8731_Init',0x08006168:'SdramHandle_ConfigureController',0x080061d0:'SdramHandle_InitSequence',0x08006260:'SdramHandle_Init',0x08006414:'AudioHandle_Impl_InternalCallback',0x08006ebc:'AudioHandle_Init_single_SAI',0x08006f2c:'AudioHandle_GetConfig',0x08006f34:'AudioHandle_SetBlockSize',0x08006f50:'AudioHandle_GetSampleRate',0x08006f58:'AudioHandle_Start_interleaved',0x08006fa8:'AnalogControl_Init',0x0800700c:'AnalogControl_InitBipolarCv',0x08007078:'AnalogControl_Process',0x080070c0:'GateIn_Init',0x080070e4:'GateIn_Trig',0x0800710c:'Switch_Init',0x0800713c:'Switch_Debounce',0x08007938:'GPIO_Init',0x08007950:'GPIO_Read',0x08007968:'GPIO_Write',0x08008034:'I2CHandle_Init',0x08008050:'I2CHandle_TransmitBlocking',0x08008058:'I2CHandle_TransmitDma',0x08008d3c:'SaiHandle_Impl_Init',0x08008f50:'SaiHandle_Impl_StartDma',0x0800933c:'SaiHandle_Init',0x08009354:'SaiHandle_GetConfig',0x08009358:'SaiHandle_StartDma',0x08009360:'SaiHandle_GetSampleRate',0x0800950c:'System_GetNow',0x08009510:'System_GetTick',0x0800951c:'System_Delay',0x08009520:'System_GetBootloaderVersion',0x08009550:'System_ConfigureClocks',0x080096e4:'System_ConfigureMpu',0x08009764:'System_Init',0x0800985c:'System_GetTickFreq',0x08009868:'System_GetProgramMemoryRegion',0x080098fc:'SystemInit_STM32H7',0x08011a68:'HAL_SAI_Init',0x08011e1c:'HAL_SAI_InitProtocol',0x080188e4:'libc_init_array',0x0801892c:'memcpy',0x08018948:'memset'}
(ROOT/'analysis/platform/function_labels.json').write_text(json.dumps({hex(k):v for k,v in labels.items()},indent=2)+'\n')
meta={
 'firmware':{'size':len(b),'sha256':hashlib.sha256(b).hexdigest(),'base':'0x08000000'},
 'startup':{'sp':'0x20020000','reset':'0x08000a09','data_source':'0x0801a570','data_begin':'0x24000000','data_end':'0x240003e4','bss_begin':'0x240003e4','bss_end':'0x240047d4'},
 'audio':{'nominal_SAI_rate_hz':48000,'DSP_design_rate_constant_hz':struct.unpack_from('<f',b,0x3b48)[0],'block_frames':96,'callback_scalar_samples':192,'channels':2,'callback_layout':'interleaved L/R','SAI_bit_depth':24,'floating_process':'IEEE754 binary32','postgain':1.0,'output_compensation':1.0,'SAI_protocol':'MSB-justified','frame_bits':64,'slot_bits':32,'MCKDIV_emulated_with_nominal_PLL3_clock':4,'PLL3_M':6,'PLL3_N':295,'PLL3_P':16,'PLL3_Q':4,'PLL3_R':32,'PLL3_FRACN':0,'HSE_nominal_hz':16000000,'predicted_physical_rate_hz':16000000*295/6/16/(4*256),'physical_rate_status':'inferred from recovered initialization, not measured on device'},
 'sdram':{'base':'0xc0000000','second_base':'0xc1b794d0','element_count_constant':0x6de534,'doubled_count_constant':0xdbca68,'distance_bytes':0x1b794d0,'capacity_status':'per-channel/plane semantics must be resolved in DSP analysis'},
 'dma':{'LED_buffers_base':'0x30000000','audio_tx_base':'0x30000108','audio_rx_base':'0x30002108','audio_dma_capacity_bytes_each':8192},
 'sources':[{'url':'https://github.com/electro-smith/libDaisy/blob/master/src/daisy_seed.cpp','local':'libDaisy/src/daisy_seed.cpp'},{'url':'https://github.com/electro-smith/libDaisy/blob/master/src/hid/audio.cpp','local':'libDaisy/src/hid/audio.cpp'},{'url':'https://github.com/electro-smith/libDaisy/blob/master/src/per/sai.cpp','local':'libDaisy/src/per/sai.cpp'},{'url':'https://github.com/electro-smith/libDaisy/blob/master/src/sys/system.cpp','local':'libDaisy/src/sys/system.cpp'},{'url':'https://www.qubitelectronix.com/faq','role':'official board identification'}]
}
(ROOT/'analysis/platform/platform_metadata.json').write_text(json.dumps(meta,indent=2)+'\n')
print('wrote',p,'and function_labels.json/platform_metadata.json')
