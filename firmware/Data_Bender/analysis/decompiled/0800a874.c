/* 0800a874 FUN_0800a874; analyst naming is provisional. */

undefined4 FUN_0800a874(int *param_1,uint *param_2)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  uint uVar5;
  bool bVar6;
  
  uVar5 = *param_2;
  if (*(char *)(param_1 + 0x14) == '\x01') {
    return 2;
  }
  iVar4 = *param_1;
  bVar6 = iVar4 != DAT_0800a934;
  *(undefined *)(param_1 + 0x14) = 1;
  iVar2 = DAT_0800a940;
  iVar1 = DAT_0800a938;
  if (bVar6) {
    *(undefined *)(param_1 + 0x14) = 0;
    param_1[0x15] = param_1[0x15] | 0x20;
    return 1;
  }
  if ((*(int *)(DAT_0800a938 + 8) << 0x1d < 0) || ((*(uint *)(iVar4 + 8) & 4) != 0)) {
    uVar3 = 1;
    param_1[0x15] = param_1[0x15] | 0x20;
  }
  else {
    if (uVar5 == 0) {
      *(uint *)(DAT_0800a940 + 8) = *(uint *)(DAT_0800a940 + 8) & 0xffff3fff;
      if (-1 < (int)((*(uint *)(iVar1 + 8) | *(uint *)(iVar4 + 8)) << 0x1f)) {
        *(uint *)(iVar2 + 8) = DAT_0800a93c & *(uint *)(iVar2 + 8);
        uVar3 = 0;
        goto LAB_0800a8be;
      }
    }
    else {
      *(uint *)(DAT_0800a940 + 8) = *(uint *)(DAT_0800a940 + 8) & 0xffff3fff | param_2[1];
      if (-1 < (int)((*(uint *)(iVar1 + 8) | *(uint *)(iVar4 + 8)) << 0x1f)) {
        *(uint *)(iVar2 + 8) = uVar5 | param_2[2] | DAT_0800a93c & *(uint *)(iVar2 + 8);
        uVar3 = 0;
        goto LAB_0800a8be;
      }
    }
    uVar3 = 0;
  }
LAB_0800a8be:
  *(undefined *)(param_1 + 0x14) = 0;
  return uVar3;
}


