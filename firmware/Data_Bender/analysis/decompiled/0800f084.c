/* 0800f084 FUN_0800f084; analyst naming is provisional. */

int FUN_0800f084(uint param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar1 = DAT_0800f0c8;
  if ((*(uint *)(DAT_0800f0c8 + 0xc) & 4) == 0) {
    iVar1 = (*(uint *)(DAT_0800f0c8 + 0xc) & 7) - param_1;
    if (iVar1 != 0) {
      iVar1 = 1;
    }
    return iVar1;
  }
  *(uint *)(DAT_0800f0c8 + 0xc) = param_1 | *(uint *)(DAT_0800f0c8 + 0xc) & 0xfffffff8;
  iVar2 = FUN_08009ce8();
  do {
    if (*(int *)(iVar1 + 4) << 0x12 < 0) {
      return 0;
    }
    iVar3 = FUN_08009ce8();
  } while ((uint)(iVar3 - iVar2) < 0x3e9);
  return 1;
}


