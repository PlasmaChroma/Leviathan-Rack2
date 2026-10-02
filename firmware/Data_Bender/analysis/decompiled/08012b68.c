/* 08012b68 FUN_08012b68; analyst naming is provisional. */

uint FUN_08012b68(int param_1,int param_2)

{
  param_1 = param_1 + param_2 * 0x20;
  return *(uint *)(param_1 + 0x508) & *(uint *)(param_1 + 0x50c);
}


