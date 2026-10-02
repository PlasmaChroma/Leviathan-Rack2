/* 080194cc FUN_080194cc; analyst naming is provisional. */

uint FUN_080194cc(undefined4 *param_1,undefined4 *param_2)

{
  ushort uVar1;
  ushort uVar2;
  uint uVar3;
  uint uVar4;
  int iVar5;
  
  iVar5 = *DAT_08019598;
  if ((iVar5 != 0) && (*(int *)(iVar5 + 0x18) == 0)) {
    FUN_08018ac0(iVar5);
  }
  if (param_2 == DAT_0801959c) {
    param_2 = *(undefined4 **)(iVar5 + 4);
  }
  else if (param_2 == DAT_080195a0) {
    param_2 = *(undefined4 **)(iVar5 + 8);
  }
  else if (param_2 == DAT_080195a4) {
    param_2 = *(undefined4 **)(iVar5 + 0xc);
  }
  uVar3 = (uint)*(ushort *)(param_2 + 3);
  uVar2 = *(ushort *)(param_2 + 3);
  if (-1 < (int)(uVar3 << 0x1c)) {
    if (-1 < (int)(uVar3 << 0x1b)) {
      *param_1 = 9;
      goto LAB_080194fc;
    }
    if ((int)(uVar3 << 0x1d) < 0) {
      if ((undefined4 *)param_2[0xd] != (undefined4 *)0x0) {
        if ((undefined4 *)param_2[0xd] != param_2 + 0x11) {
          FUN_08018c34(param_1);
        }
        param_2[0xd] = 0;
      }
      *(ushort *)(param_2 + 3) = *(ushort *)(param_2 + 3) & 0xffdb;
      param_2[1] = 0;
      *param_2 = param_2[4];
    }
    *(ushort *)(param_2 + 3) = *(ushort *)(param_2 + 3) | 8;
  }
  if ((param_2[4] == 0) && ((*(ushort *)(param_2 + 3) & 0x280) != 0x200)) {
    FUN_080197cc(param_1,param_2);
  }
  uVar1 = *(ushort *)(param_2 + 3);
  uVar3 = (uint)uVar1;
  uVar2 = *(ushort *)(param_2 + 3);
  uVar4 = uVar3 & 1;
  if ((uVar1 & 1) == 0) {
    if (-1 < (int)(uVar3 << 0x1e)) {
      uVar4 = param_2[5];
    }
    param_2[2] = uVar4;
  }
  else {
    param_2[2] = 0;
    param_2[6] = -param_2[5];
  }
  if (param_2[4] != 0) {
    return 0;
  }
  if ((uVar1 & 0x80) == 0) {
    return uVar3 & 0x80;
  }
LAB_080194fc:
  *(ushort *)(param_2 + 3) = uVar2 | 0x40;
  return 0xffffffff;
}


