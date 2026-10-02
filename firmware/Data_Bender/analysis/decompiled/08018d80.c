/* 08018d80 FUN_08018d80; analyst naming is provisional. */

uint FUN_08018d80(int param_1,uint param_2,int *param_3)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  undefined *puVar4;
  
  iVar2 = param_3[2] + -1;
  param_3[2] = iVar2;
  if ((-1 < iVar2) || ((param_3[6] <= iVar2 && (param_2 != 10)))) {
    puVar4 = (undefined *)*param_3;
    *param_3 = (int)(puVar4 + 1);
    *puVar4 = (char)param_2;
    return param_2;
  }
  if ((param_1 != 0) && (*(int *)(param_1 + 0x18) == 0)) {
    FUN_08018ac0();
  }
  if (param_3 == DAT_0801949c) {
    param_3 = *(int **)(param_1 + 4);
  }
  else if (param_3 == DAT_080194a0) {
    param_3 = *(int **)(param_1 + 8);
  }
  else if (param_3 == DAT_080194a4) {
    param_3 = *(int **)(param_1 + 0xc);
  }
  param_3[2] = param_3[6];
  uVar3 = (uint)*(ushort *)(param_3 + 3);
  iVar1 = uVar3 << 0x1c;
  if (((iVar1 < 0) && (uVar3 = param_3[4], uVar3 != 0)) ||
     (iVar2 = FUN_080194cc(param_1,param_3,iVar1,uVar3,iVar2), iVar2 == 0)) {
    iVar2 = *param_3 - param_3[4];
    uVar3 = param_2 & 0xff;
    if ((iVar2 < param_3[5]) || (iVar2 = FUN_080196e4(param_1,param_3), iVar2 == 0)) {
      param_3[2] = param_3[2] + -1;
      puVar4 = (undefined *)*param_3;
      *param_3 = (int)(puVar4 + 1);
      *puVar4 = (char)param_2;
      if (param_3[5] != iVar2 + 1) {
        if (-1 < (int)((uint)*(ushort *)(param_3 + 3) << 0x1f)) {
          return uVar3;
        }
        if (uVar3 != 10) {
          return uVar3;
        }
      }
      iVar2 = FUN_080196e4(param_1,param_3);
      if (iVar2 == 0) {
        return uVar3;
      }
    }
  }
  return 0xffffffff;
}


