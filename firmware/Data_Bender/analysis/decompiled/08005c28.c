/* 08005c28 DaisySeed_SetAudioBlockSize; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DaisySeed_SetAudioBlockSize(int param_1)

{
  uint *puVar1;
  int iVar2;
  float fVar3;
  
  iVar2 = param_1 + 0x14;
  AudioHandle_SetBlockSize(iVar2);
  fVar3 = (float)AudioHandle_GetSampleRate(iVar2);
  puVar1 = (uint *)AudioHandle_GetConfig(iVar2);
  *(float *)(param_1 + 0x6c) = fVar3 / (float)(ulonglong)*puVar1;
  return;
}


