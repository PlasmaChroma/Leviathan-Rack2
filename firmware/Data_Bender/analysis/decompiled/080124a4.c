/* 080124a4 FUN_080124a4; analyst naming is provisional. */

undefined4 FUN_080124a4(int param_1,int param_2)

{
  *(uint *)(param_1 + 0x14) = DAT_080124bc & *(uint *)(param_1 + 0x14) | param_2 << 1;
  return 0;
}


