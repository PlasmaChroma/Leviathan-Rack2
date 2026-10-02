/* 08012c00 FUN_08012c00; analyst naming is provisional. */

undefined4 FUN_08012c00(int param_1,int param_2,undefined4 param_3)

{
  if ((DAT_08012c54 < *(uint *)(param_1 + 0x40)) && (*(int *)(param_1 + 0xb00) < 0)) {
    return 0;
  }
  *(undefined4 *)(param_1 + 0xb10) = 0;
  *(uint *)(param_1 + 0xb10) = *(uint *)(param_1 + 0xb10) | 0x80000;
  *(uint *)(param_1 + 0xb10) = *(uint *)(param_1 + 0xb10) | 0x18;
  *(uint *)(param_1 + 0xb10) = *(uint *)(param_1 + 0xb10) | 0x60000000;
  if (param_2 == 1) {
    *(undefined4 *)(param_1 + 0xb14) = param_3;
    *(uint *)(param_1 + 0xb00) = *(uint *)(param_1 + 0xb00) | 0x80008000;
  }
  return 0;
}


