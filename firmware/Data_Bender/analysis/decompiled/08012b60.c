/* 08012b60 FUN_08012b60; analyst naming is provisional. */

uint FUN_08012b60(int param_1)

{
  return *(uint *)(param_1 + 0x18) & *(uint *)(param_1 + 0x14);
}


