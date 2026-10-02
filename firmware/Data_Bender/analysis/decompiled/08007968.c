/* 08007968 GPIO_Write; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void GPIO_Write(int param_1,undefined4 param_2)

{
  FUN_0800c490(*(undefined4 *)(param_1 + 0x10),1 << *(sbyte *)(param_1 + 1) & 0xffff,param_2);
  return;
}


