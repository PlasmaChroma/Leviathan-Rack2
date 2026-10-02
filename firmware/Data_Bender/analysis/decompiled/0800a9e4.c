/* 0800a9e4 FUN_0800a9e4; analyst naming is provisional. */

void FUN_0800a9e4(uint param_1)

{
  if (-1 < (int)param_1) {
    *(int *)(DAT_0800a9fc + (param_1 >> 5) * 4) = 1 << (param_1 & 0x1f);
  }
  return;
}


