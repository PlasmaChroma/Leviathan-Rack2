/* 0800c490 FUN_0800c490; analyst naming is provisional. */

void FUN_0800c490(int param_1,int param_2,int param_3)

{
  if (param_3 == 0) {
    param_2 = param_2 << 0x10;
  }
  *(int *)(param_1 + 0x18) = param_2;
  return;
}


