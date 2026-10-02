/* 08013650 FUN_08013650; analyst naming is provisional. */

void FUN_08013650(undefined *param_1,undefined *param_2)

{
  *param_1 = *param_2;
  param_1[1] = param_2[1];
  *(undefined2 *)(param_1 + 2) = *(undefined2 *)(param_2 + 2);
  *(undefined2 *)(param_1 + 4) = *(undefined2 *)(param_2 + 4);
  *(undefined2 *)(param_1 + 6) = *(undefined2 *)(param_2 + 6);
  return;
}


