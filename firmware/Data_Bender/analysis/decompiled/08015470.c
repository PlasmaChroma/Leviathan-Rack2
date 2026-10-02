/* 08015470 FUN_08015470; analyst naming is provisional. */

void FUN_08015470(uint param_1,int param_2)

{
  int iVar1;
  int iVar2;
  
  iVar1 = DAT_08015498;
  if (0 < param_2 + 0x20) {
    param_1 = param_1 & 0xffffffe0;
    DataSynchronizationBarrier(0xf);
    iVar2 = param_2 + 0x20 + param_1;
    do {
      *(uint *)(iVar1 + 0x25c) = param_1;
      param_1 = param_1 + 0x20;
    } while (0 < (int)(iVar2 - param_1));
    DataSynchronizationBarrier(0xf);
    InstructionSynchronizationBarrier(0xf);
  }
  return;
}


