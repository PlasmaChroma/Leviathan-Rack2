/* 0800a7d0 FUN_0800a7d0; analyst naming is provisional. */

int FUN_0800a7d0(int *param_1,uint param_2,uint param_3)

{
  uint uVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  uint local_1c;
  
  local_1c = 0;
  if (*(char *)(param_1 + 0x14) == '\x01') {
    return 2;
  }
  *(undefined *)(param_1 + 0x14) = 1;
  iVar3 = FUN_0800a3c4();
  uVar2 = DAT_0800a870;
  uVar1 = DAT_0800a86c;
  if (iVar3 == 0) {
    param_1[0x15] = DAT_0800a868 & param_1[0x15] | 2;
    iVar4 = *param_1;
    *(uint *)(iVar4 + 8) =
         uVar1 & *(uint *)(iVar4 + 8) | param_3 & 0x40000000 | param_2 & 0x10000 | 0x80000000;
    do {
      if (-1 < *(int *)(iVar4 + 8)) {
        param_1[0x15] = param_1[0x15] & 0xfffffffcU | 1;
        goto LAB_0800a836;
      }
      local_1c = local_1c + 1;
    } while (local_1c < uVar2);
    iVar3 = 1;
    *(undefined *)(param_1 + 0x14) = 0;
    param_1[0x15] = param_1[0x15] & 0xffffffedU | 0x10;
  }
  else {
    param_1[0x15] = param_1[0x15] | 0x10;
LAB_0800a836:
    *(undefined *)(param_1 + 0x14) = 0;
  }
  return iVar3;
}


