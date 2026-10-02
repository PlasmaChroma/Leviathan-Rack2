/* 080075cc FUN_080075cc; analyst naming is provisional. */

int FUN_080075cc(undefined4 param_1,uint param_2)

{
  int iVar1;
  
  iVar1 = *(int *)(DAT_080075dc + 0x584);
  if (param_2 < 0x10) {
    iVar1 = iVar1 + param_2 * 2;
  }
  return iVar1;
}


