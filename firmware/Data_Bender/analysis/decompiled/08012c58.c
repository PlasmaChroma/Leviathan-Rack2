/* 08012c58 FUN_08012c58; analyst naming is provisional. */

undefined4 FUN_08012c58(int param_1,uint param_2)

{
  *(uint *)(param_1 + 0x400) = *(uint *)(param_1 + 0x400) & 0xfffffffc;
  *(uint *)(param_1 + 0x400) = param_2 & 3 | *(uint *)(param_1 + 0x400);
  if (param_2 == 1) {
    *(undefined4 *)(param_1 + 0x404) = 48000;
    return 0;
  }
  if (param_2 == 2) {
    *(undefined4 *)(param_1 + 0x404) = 6000;
    return 0;
  }
  return 1;
}


