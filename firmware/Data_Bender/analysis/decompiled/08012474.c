/* 08012474 FUN_08012474; analyst naming is provisional. */

undefined4 FUN_08012474(int param_1,uint *param_2)

{
  *(uint *)(param_1 + 0x10) =
       *param_2 | param_2[1] | param_2[3] << 9 | DAT_080124a0 & *(uint *)(param_1 + 0x10) |
       (param_2[2] - 1) * 0x20;
  return 0;
}


