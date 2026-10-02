/* 0800c484 FUN_0800c484; analyst naming is provisional. */

bool FUN_0800c484(int param_1,uint param_2)

{
  return (param_2 & *(uint *)(param_1 + 0x10)) != 0;
}


