/* 0800cdb8 FUN_0800cdb8; analyst naming is provisional. */

undefined4 FUN_0800cdb8(int *param_1,uint param_2,int param_3)

{
  bool bVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  
  iVar5 = *param_1;
  if ((*(uint *)(iVar5 + 0x18) & 0x10) != 0) {
    *(undefined4 *)(iVar5 + 0x1c) = 0x10;
    while (-1 < *(int *)(iVar5 + 0x18) << 0x1a) {
      if (param_2 == 0xffffffff) goto LAB_0800cdd8;
      iVar4 = FUN_08009ce8();
      iVar5 = *param_1;
      if ((param_2 < (uint)(iVar4 - param_3)) || (param_2 == 0)) {
        if ((*(int *)(iVar5 + 0x18) << 0x10 < 0) &&
           ((-1 < *(int *)(iVar5 + 4) << 0x11 && (*(char *)((int)param_1 + 0x42) != ' ')))) {
          *(uint *)(iVar5 + 4) = *(uint *)(iVar5 + 4) | 0x4000;
          param_3 = FUN_08009ce8();
          iVar5 = *param_1;
        }
        while (-1 < *(int *)(iVar5 + 0x18) << 0x1a) {
          iVar4 = FUN_08009ce8();
          iVar5 = *param_1;
          if (0x19 < (uint)(iVar4 - param_3)) {
            uVar2 = 0x20;
            goto LAB_0800cde4;
          }
        }
      }
    }
    goto LAB_0800cdde;
  }
  iVar4 = *(int *)(iVar5 + 0x18);
  bVar1 = false;
  uVar2 = 0;
  uVar3 = 0;
  if (iVar4 << 0x17 < 0) goto LAB_0800ce4e;
LAB_0800cdf0:
  if (iVar4 << 0x15 < 0) {
    uVar3 = uVar3 | 8;
    *(undefined4 *)(iVar5 + 0x1c) = 0x400;
    goto LAB_0800cdfe;
  }
  if (-1 < iVar4 << 0x16) {
    if (!bVar1) {
      return 0;
    }
    goto LAB_0800ce0c;
  }
  goto LAB_0800ce02;
LAB_0800cdd8:
  do {
  } while (-1 < *(int *)(iVar5 + 0x18) << 0x1a);
LAB_0800cdde:
  uVar2 = 0;
  *(undefined4 *)(iVar5 + 0x1c) = 0x20;
LAB_0800cde4:
  iVar4 = *(int *)(iVar5 + 0x18);
  uVar2 = uVar2 | 4;
  bVar1 = true;
  uVar3 = uVar2;
  if (-1 < iVar4 << 0x17) goto LAB_0800cdf0;
LAB_0800ce4e:
  uVar3 = uVar2 | 1;
  *(undefined4 *)(iVar5 + 0x1c) = 0x100;
  if (iVar4 << 0x15 < 0) {
    uVar3 = uVar2 | 9;
    *(undefined4 *)(iVar5 + 0x1c) = 0x400;
  }
LAB_0800cdfe:
  if (-1 < iVar4 << 0x16) goto LAB_0800ce0c;
LAB_0800ce02:
  uVar3 = uVar3 | 2;
  *(undefined4 *)(iVar5 + 0x1c) = 0x200;
LAB_0800ce0c:
  if (*(int *)(iVar5 + 0x18) << 0x1e < 0) {
    *(undefined4 *)(iVar5 + 0x28) = 0;
  }
  if (-1 < *(int *)(iVar5 + 0x18) << 0x1f) {
    *(uint *)(iVar5 + 0x18) = *(uint *)(iVar5 + 0x18) | 1;
  }
  *(uint *)(iVar5 + 4) = *(uint *)(iVar5 + 4) & DAT_0800cec0;
  *(undefined *)(param_1 + 0x10) = 0;
  param_1[0x11] = uVar3 | param_1[0x11];
  *(undefined *)((int)param_1 + 0x41) = 0x20;
  *(undefined *)((int)param_1 + 0x42) = 0;
  return 1;
}


