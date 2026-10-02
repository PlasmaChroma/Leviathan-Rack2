/* 08013750 FUN_08013750; analyst naming is provisional. */

undefined4 FUN_08013750(int param_1,uint param_2)

{
  if (param_2 < 0xb) {
    param_1 = param_1 + param_2 * 4;
    *(uint *)(param_1 + 0x388) = *(uint *)(param_1 + 0x388) & 0x7fff;
  }
  return 0;
}


