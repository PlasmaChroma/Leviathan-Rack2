/* 08012a7c FUN_08012a7c; analyst naming is provisional. */

undefined4 FUN_08012a7c(int param_1,byte *param_2)

{
  uint uVar1;
  
  uVar1 = (uint)*param_2;
  param_1 = param_1 + uVar1 * 0x20;
  if (param_2[1] != 1) {
    if ((uVar1 != 0) && (-1 < *(int *)(param_1 + 0xb00))) {
      *(uint *)(param_1 + 0xb00) = *(uint *)(param_1 + 0xb00) & 0xbfffffff;
    }
    *(uint *)(param_1 + 0xb00) = *(uint *)(param_1 + 0xb00) | 0x200000;
    return 0;
  }
  if ((*(int *)(param_1 + 0x900) < 0) || (uVar1 == 0)) {
    *(uint *)(param_1 + 0x900) = *(uint *)(param_1 + 0x900) | 0x200000;
  }
  else {
    *(uint *)(param_1 + 0x900) = *(uint *)(param_1 + 0x900) & 0xbfffffff;
    *(uint *)(param_1 + 0x900) = *(uint *)(param_1 + 0x900) | 0x200000;
  }
  return 0;
}


