/* 080099f8 FUN_080099f8; analyst naming is provisional. */

void FUN_080099f8(int *param_1)

{
  FUN_0801302c(param_1[0x142]);
  *(uint *)(*param_1 + 0xe00) = *(uint *)(*param_1 + 0xe00) | 1;
  if (param_1[8] != 0) {
    *(uint *)(DAT_08009a24 + 0x10) = *(uint *)(DAT_08009a24 + 0x10) | 6;
  }
  return;
}


