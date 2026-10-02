/* 0800a3c4 FUN_0800a3c4; analyst naming is provisional. */

undefined4 FUN_0800a3c4(int *param_1)

{
  int iVar1;
  undefined4 *puVar2;
  int iVar3;
  
  puVar2 = (undefined4 *)*param_1;
  if ((int)(puVar2[2] << 0x1e) < 0) {
    return 0;
  }
  if ((int)(puVar2[2] << 0x1f) < 0) {
    if ((puVar2[2] & 0xd) != 1) {
LAB_0800a3e6:
      param_1[0x15] = param_1[0x15] | 0x10;
      param_1[0x16] = param_1[0x16] | 1;
      return 1;
    }
    puVar2[2] = DAT_0800a434 & puVar2[2] | 2;
    *puVar2 = 3;
    iVar1 = FUN_08009ce8();
    iVar3 = *(int *)(*param_1 + 8);
    while (iVar3 << 0x1f < 0) {
      iVar3 = FUN_08009ce8();
      if ((2 < (uint)(iVar3 - iVar1)) && (*(int *)(*param_1 + 8) << 0x1f < 0)) goto LAB_0800a3e6;
      iVar3 = *(int *)(*param_1 + 8);
    }
  }
  return 0;
}


