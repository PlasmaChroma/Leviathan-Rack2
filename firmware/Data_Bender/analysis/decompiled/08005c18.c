/* 08005c18 DaisySeed_StartAudio_interleaved; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DaisySeed_StartAudio_interleaved(int param_1,undefined4 param_2)

{
  undefined4 *puVar1;
  
  puVar1 = *(undefined4 **)(param_1 + 0x14);
  SaiHandle_StartDma(puVar1 + 6,puVar1[8],puVar1[10],puVar1[2] << 2,DAT_08006f80);
  *puVar1 = 0;
  puVar1[1] = param_2;
  return;
}


