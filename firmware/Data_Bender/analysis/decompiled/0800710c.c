/* 0800710c Switch_Init; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void Switch_Init(undefined4 *param_1,undefined2 param_2)

{
  undefined4 uVar1;
  
  uVar1 = System_GetNow();
  *param_1 = uVar1;
  *(undefined2 *)(param_1 + 1) = 0x100;
  *(undefined2 *)(param_1 + 7) = 0x100;
  GPIO_Init(param_1 + 2,param_2,0,1,0);
  return;
}


