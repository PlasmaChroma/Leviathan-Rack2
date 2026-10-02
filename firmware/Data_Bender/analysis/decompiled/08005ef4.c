/* 08005ef4 Ak4556_Init; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void Ak4556_Init(undefined4 param_1,undefined2 param_2)

{
  GPIO_Init(param_1,param_2,1,0,0);
  GPIO_Write(param_1,1);
  System_Delay(1);
  GPIO_Write(param_1,0);
  System_Delay(1);
  GPIO_Write(param_1,1);
  return;
}


