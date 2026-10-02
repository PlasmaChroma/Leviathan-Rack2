/* 0800a1f4 FUN_0800a1f4; analyst naming is provisional. */

undefined4 FUN_0800a1f4(int **param_1)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  int *piVar4;
  
  piVar4 = *param_1;
  if (piVar4[2] << 0x1f < 0) {
    return 0;
  }
  if ((piVar4[2] & DAT_0800a288) != 0) {
LAB_0800a266:
    param_1[0x15] = (int *)((uint)param_1[0x15] | 0x10);
    param_1[0x16] = (int *)((uint)param_1[0x16] | 1);
    return 1;
  }
  piVar4[2] = DAT_0800a28c & piVar4[2] | 1;
  iVar2 = FUN_08009ce8();
  uVar1 = DAT_0800a28c;
  piVar4 = *param_1;
  if ((((piVar4 != DAT_0800a290) && (piVar4 != DAT_0800a290 + 0x40)) ||
      ((*(uint *)(DAT_0800a298 + 8) & 0x1f) == 0)) || (piVar4 != DAT_0800a29c)) {
    iVar3 = *piVar4;
    while (-1 < iVar3 << 0x1f) {
      if (-1 < piVar4[2] << 0x1f) {
        piVar4[2] = piVar4[2] & uVar1 | 1;
      }
      iVar3 = FUN_08009ce8();
      piVar4 = *param_1;
      if ((2 < (uint)(iVar3 - iVar2)) && (-1 < *piVar4 << 0x1f)) goto LAB_0800a266;
      iVar3 = *piVar4;
    }
  }
  return 0;
}


