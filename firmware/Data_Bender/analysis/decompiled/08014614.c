/* 08014614 FUN_08014614; analyst naming is provisional. */

int FUN_08014614(int *param_1)

{
  int iVar1;
  
  if (*(char *)(*param_1 + 0xc) != '\0') {
    iVar1 = FUN_0801601c();
    if (iVar1 != 0) {
      iVar1 = 1;
    }
    return iVar1;
  }
  iVar1 = FUN_08015f6c(*param_1 + 0x10);
  if (iVar1 != 0) {
    iVar1 = 1;
  }
  return iVar1;
}


