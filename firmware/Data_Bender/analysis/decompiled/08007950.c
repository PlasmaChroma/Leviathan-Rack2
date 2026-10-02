/* 08007950 GPIO_Read; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

int GPIO_Read(int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  
  iVar1 = FUN_0800c484(*(undefined4 *)(param_1 + 0x10),1 << (uint)*(byte *)(param_1 + 1) & 0xffff,
                       param_3,(uint)*(byte *)(param_1 + 1),param_4);
  if (iVar1 != 0) {
    iVar1 = 1;
  }
  return iVar1;
}


