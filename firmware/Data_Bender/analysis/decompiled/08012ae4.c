/* 08012ae4 FUN_08012ae4; analyst naming is provisional. */

undefined4 FUN_08012ae4(int param_1,byte *param_2)

{
  param_1 = param_1 + (uint)*param_2 * 0x20;
  if (param_2[1] == 1) {
    *(uint *)(param_1 + 0x900) = *(uint *)(param_1 + 0x900) & 0xffdfffff;
    if (param_2[4] - 2 < 2) {
      *(uint *)(param_1 + 0x900) = *(uint *)(param_1 + 0x900) | 0x10000000;
      return 0;
    }
  }
  else {
    *(uint *)(param_1 + 0xb00) = *(uint *)(param_1 + 0xb00) & 0xffdfffff;
    if (param_2[4] - 2 < 2) {
      *(uint *)(param_1 + 0xb00) = *(uint *)(param_1 + 0xb00) | 0x10000000;
      return 0;
    }
  }
  return 0;
}


