/* 08009cf4 FUN_08009cf4; analyst naming is provisional. */

void FUN_08009cf4(uint param_1)

{
  int iVar1;
  int iVar2;
  
  iVar1 = FUN_08009ce8();
  if (param_1 != 0xffffffff) {
    param_1 = param_1 + *DAT_08009d14;
  }
  do {
    iVar2 = FUN_08009ce8();
  } while ((uint)(iVar2 - iVar1) < param_1);
  return;
}


