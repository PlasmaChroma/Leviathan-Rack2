/* 08013668 FUN_08013668; analyst naming is provisional. */

undefined4 FUN_08013668(int param_1,undefined4 param_2,undefined4 param_3)

{
  *(undefined4 *)(param_1 + 0x294) = 2;
  *(undefined4 *)(param_1 + 0x18) = param_3;
  *(undefined4 *)(param_1 + 0x1c) = param_3;
  FUN_08009af0(param_1,0,param_2);
  return 0;
}


