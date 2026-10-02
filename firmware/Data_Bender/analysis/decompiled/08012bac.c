/* 08012bac FUN_08012bac; analyst naming is provisional. */

uint FUN_08012bac(int param_1,uint param_2)

{
  return *(uint *)(param_1 + param_2 * 0x20 + 0x908) &
         ((*(uint *)(param_1 + 0x834) >> (param_2 & 0xf) & 1) << 7 | *(uint *)(param_1 + 0x810));
}


