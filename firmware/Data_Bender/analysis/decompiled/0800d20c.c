/* 0800d20c FUN_0800d20c; analyst naming is provisional. */

undefined4 FUN_0800d20c(int *param_1,int param_2,int param_3,undefined4 param_4)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  byte *pbVar4;
  uint uVar5;
  
  if (*(char *)((int)param_1 + 0x41) != ' ') {
    return 2;
  }
  if ((param_2 == 0) || (uVar5 = (uint)(param_3 == 0), param_3 == 0)) {
    param_1[0x11] = 0x200;
    return 1;
  }
  if (*(char *)(param_1 + 0x10) == '\x01') {
    return 2;
  }
  *(undefined *)(param_1 + 0x10) = 1;
  uVar1 = FUN_08009ce8();
  param_1[9] = param_2;
  param_1[0xd] = uVar5;
  *(undefined *)((int)param_1 + 0x41) = 0x21;
  *(undefined *)((int)param_1 + 0x42) = 0x20;
  param_1[0x11] = uVar5;
  *(short *)((int)param_1 + 0x2a) = (short)param_3;
  *(uint *)(*param_1 + 4) = *(uint *)(*param_1 + 4) & 0xffff7fff;
  iVar2 = FUN_0800cd4c(param_1,8,uVar5,param_4,uVar1);
  if (iVar2 != 0) {
    *(uint *)(*param_1 + 4) = *(uint *)(*param_1 + 4) | 0x8000;
    return 1;
  }
  if (param_1[8] == 0x20000) {
    pbVar4 = (byte *)param_1[9];
    iVar2 = *param_1;
    *(uint *)(iVar2 + 0x28) = (uint)*pbVar4;
    param_1[9] = (int)(pbVar4 + 1);
    *(short *)((int)param_1 + 0x2a) = *(short *)((int)param_1 + 0x2a) + -1;
  }
  else {
    iVar2 = *param_1;
  }
  *(undefined4 *)(iVar2 + 0x1c) = 8;
  if (param_1[3] == 2) {
    iVar2 = FUN_0800cd4c(param_1,8,0,param_4,uVar1);
    if (iVar2 != 0) goto LAB_0800d2e8;
    *(undefined4 *)(*param_1 + 0x1c) = 8;
  }
  iVar2 = FUN_0800cd4c(param_1,0x10000,0,param_4,uVar1);
  if (iVar2 == 0) {
    while (*(short *)((int)param_1 + 0x2a) != 0) {
      iVar2 = FUN_0800cec4(param_1,param_4,uVar1);
      if (iVar2 != 0) goto LAB_0800d2e8;
      pbVar4 = (byte *)param_1[9];
      *(uint *)(*param_1 + 0x28) = (uint)*pbVar4;
      param_1[9] = (int)(pbVar4 + 1);
      *(short *)((int)param_1 + 0x2a) = *(short *)((int)param_1 + 0x2a) + -1;
    }
    iVar2 = FUN_0800cd4c(param_1,0x10,0,param_4,uVar1);
    if (iVar2 == 0) {
      iVar2 = *param_1;
      if (*(int *)(iVar2 + 0x18) << 0x1e < 0) {
        *(undefined4 *)(iVar2 + 0x28) = 0;
      }
      if (-1 < *(int *)(iVar2 + 0x18) << 0x1f) {
        *(uint *)(iVar2 + 0x18) = *(uint *)(iVar2 + 0x18) | 1;
      }
      *(undefined4 *)(iVar2 + 0x1c) = 0x10;
      iVar2 = FUN_0800cf20(param_1,param_4,uVar1);
      iVar3 = *param_1;
      if (iVar2 == 0) {
        *(undefined4 *)(iVar3 + 0x1c) = 0x20;
        iVar2 = FUN_0800cd4c(param_1,0x8000,1,param_4,uVar1);
        iVar3 = *param_1;
        if (iVar2 == 0) {
          *(uint *)(iVar3 + 4) = *(uint *)(iVar3 + 4) | 0x8000;
          *(undefined *)((int)param_1 + 0x41) = 0x20;
          *(undefined *)(param_1 + 0x10) = 0;
          *(undefined *)((int)param_1 + 0x42) = 0;
          return 0;
        }
      }
      *(uint *)(iVar3 + 4) = *(uint *)(iVar3 + 4) | 0x8000;
      return 1;
    }
  }
LAB_0800d2e8:
  *(uint *)(*param_1 + 4) = *(uint *)(*param_1 + 4) | 0x8000;
  return 1;
}


