/* 08012b88 FUN_08012b88; analyst naming is provisional. */

uint FUN_08012b88(int param_1)

{
  return *(uint *)(param_1 + 0x81c) & *(uint *)(param_1 + 0x818) & 0xffff;
}


