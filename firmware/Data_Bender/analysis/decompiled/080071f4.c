/* 080071f4 FUN_080071f4; analyst naming is provisional. */

void FUN_080071f4(undefined2 *param_1,undefined2 param_2,undefined param_3)

{
  *param_1 = param_2;
  *(undefined *)(param_1 + 0x28) = 0;
  *(undefined *)((int)param_1 + 0x51) = param_3;
  *(undefined4 *)(param_1 + 2) = 3;
  *(undefined4 *)(param_1 + 4) = 0;
  return;
}


