/* 080070c0 GateIn_Init; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void GateIn_Init(int param_1,undefined2 param_2,undefined param_3)

{
  GPIO_Init(param_1,param_2,0,0,0);
  *(undefined2 *)(param_1 + 0x14) = 0;
  *(undefined *)(param_1 + 0x16) = param_3;
  return;
}


