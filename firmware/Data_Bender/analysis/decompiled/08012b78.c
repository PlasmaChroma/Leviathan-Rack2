/* 08012b78 FUN_08012b78; analyst naming is provisional. */

uint FUN_08012b78(int param_1)

{
  return (*(uint *)(param_1 + 0x81c) & *(uint *)(param_1 + 0x818)) >> 0x10;
}


