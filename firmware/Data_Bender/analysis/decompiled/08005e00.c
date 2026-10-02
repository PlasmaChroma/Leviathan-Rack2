/* 08005e00 DaisySeed_Init; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DaisySeed_Init(int param_1,int param_2)

{
  undefined4 uVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  uint *puVar5;
  float fVar6;
  uint local_28;
  undefined2 local_24;
  undefined local_22;
  
  uVar2 = DAT_08005ef0;
  uVar1 = DAT_08005eec;
  local_28 = (uint)(param_2 != 0);
  local_22 = 0;
  local_24 = 0x101;
  *(undefined4 *)(param_1 + 4) = DAT_08005ee8;
  *(undefined4 *)(param_1 + 8) = uVar1;
  *(undefined4 *)(param_1 + 0xc) = uVar2;
  *(undefined2 *)(param_1 + 0x28) = 0x702;
  *(undefined2 *)(param_1 + 0x3c) = 0xe06;
  *(undefined2 *)(param_1 + 0x10) = 1;
  *(undefined4 *)(param_1 + 0x2c) = 1;
  *(undefined4 *)(param_1 + 0x40) = 1;
  iVar3 = System_GetProgramMemoryRegion();
  iVar4 = System_GetBootloaderVersion();
  if ((iVar4 == 0) && (iVar3 != 0)) {
    local_22 = 1;
    System_Init(param_1 + 0x50,&local_28);
    if (iVar3 != 7) {
      FUN_08008a80(param_1,param_1 + 4);
    }
  }
  else {
    System_Init(param_1 + 0x50,&local_28);
    if (iVar3 == 7) {
      if (iVar4 == 0) goto LAB_08005e62;
    }
    else {
      FUN_08008a80(param_1,param_1 + 4);
      if ((iVar4 == 0) && (iVar3 != 0)) goto LAB_08005e62;
    }
    FUN_08007928(param_1 + 0x28);
    FUN_08007928(param_1 + 0x3c);
    SdramHandle_Init(param_1 + 0x12);
  }
LAB_08005e62:
  DaisySeed_ConfigureAudio(param_1);
  fVar6 = (float)AudioHandle_GetSampleRate(param_1 + 0x14);
  puVar5 = (uint *)AudioHandle_GetConfig(param_1 + 0x14);
  *(float *)(param_1 + 0x6c) = fVar6 / (float)(ulonglong)*puVar5;
  return;
}


