/* 0800aa70 FUN_0800aa70; analyst naming is provisional. */

void FUN_0800aa70(uint param_1)

{
  int iVar1;
  
  iVar1 = DAT_0800aa8c;
  *(uint *)(DAT_0800aa8c + 0x94) = param_1 | 1;
  *(uint *)(iVar1 + 0x24) = *(uint *)(iVar1 + 0x24) | 0x10000;
  DataSynchronizationBarrier(0xf);
  InstructionSynchronizationBarrier(0xf);
  return;
}


