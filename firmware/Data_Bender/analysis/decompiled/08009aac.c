/* 08009aac FUN_08009aac; analyst naming is provisional. */

undefined FUN_08009aac(int param_1,uint param_2)

{
  if (-1 < (int)(param_2 << 0x18)) {
    return *(undefined *)(*(int *)(param_1 + 0x2c8) + param_2 * 0x24 + 0x27e);
  }
  return *(undefined *)(*(int *)(param_1 + 0x2c8) + (param_2 & 0x7f) * 0x24 + 0x3e);
}


