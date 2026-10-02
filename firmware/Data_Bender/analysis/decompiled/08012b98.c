/* 08012b98 FUN_08012b98; analyst naming is provisional. */

uint FUN_08012b98(int param_1,int param_2)

{
  return *(uint *)(param_1 + 0x814) & *(uint *)(param_1 + param_2 * 0x20 + 0xb08);
}


