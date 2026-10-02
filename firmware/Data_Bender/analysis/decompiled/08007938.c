/* 08007938 GPIO_Init; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void GPIO_Init(undefined2 *param_1,undefined2 param_2,undefined4 param_3,undefined4 param_4,
              undefined4 param_5)

{
  *(undefined4 *)(param_1 + 4) = param_4;
  *(undefined4 *)(param_1 + 2) = param_3;
  *param_1 = param_2;
  *(undefined4 *)(param_1 + 6) = param_5;
  FUN_0800776c();
  return;
}


