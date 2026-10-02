/* 08009764 System_Init; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void System_Init(undefined4 *param_1,undefined4 *param_2)

{
  uint uVar1;
  int iVar2;
  undefined4 uVar3;
  uint uVar4;
  uint uVar5;
  undefined4 local_20;
  undefined4 uStack_1c;
  undefined4 local_18;
  undefined local_14;
  
  uVar3 = param_2[1];
  *param_1 = *param_2;
  param_1[1] = uVar3;
  FUN_08009c70();
  if (*(char *)((int)param_2 + 6) == '\0') {
    System_ConfigureClocks(param_1);
    System_ConfigureMpu(param_1);
  }
  FUN_08015370();
  thunk_FUN_0800797c();
  thunk_FUN_080138f4();
  thunk_FUN_08014834();
  iVar2 = DAT_08009850;
  if (*(char *)(param_2 + 1) != '\0') {
    if ((*(uint *)(DAT_08009850 + 0x14) & 0x10000) == 0) {
      *(undefined4 *)(DAT_08009850 + 0x84) = 0;
      DataSynchronizationBarrier(0xf);
      uVar5 = ((uint)(*(int *)(iVar2 + 0x80) << 4) >> 0x11) << 5;
      do {
        uVar4 = (uint)(*(int *)(iVar2 + 0x80) << 0x13) >> 0x16;
        do {
          uVar1 = uVar4 << 0x1e;
          uVar4 = uVar4 - 1;
          *(uint *)(iVar2 + 0x260) = uVar5 & 0x3fe0 | uVar1;
        } while (uVar4 != 0xffffffff);
        uVar5 = uVar5 - 0x20;
      } while (uVar5 != 0xffffffe0);
      DataSynchronizationBarrier(0xf);
      *(uint *)(iVar2 + 0x14) = *(uint *)(iVar2 + 0x14) | 0x10000;
      DataSynchronizationBarrier(0xf);
      InstructionSynchronizationBarrier(0xf);
    }
  }
  iVar2 = DAT_08009850;
  if (*(char *)((int)param_2 + 5) != '\0') {
    if ((*(uint *)(DAT_08009850 + 0x14) & 0x20000) == 0) {
      DataSynchronizationBarrier(0xf);
      InstructionSynchronizationBarrier(0xf);
      *(undefined4 *)(DAT_08009850 + 0x250) = 0;
      DataSynchronizationBarrier(0xf);
      InstructionSynchronizationBarrier(0xf);
      *(uint *)(iVar2 + 0x14) = *(uint *)(iVar2 + 0x14) | 0x20000;
      DataSynchronizationBarrier(0xf);
      InstructionSynchronizationBarrier(0xf);
    }
  }
  local_18 = 0xffffffff;
  local_14 = 0;
  local_20 = 0;
  uStack_1c = 0;
  FUN_080145fc(DAT_08009854,&local_20);
  FUN_08014614(DAT_08009854);
  FUN_08013768();
  return;
}


