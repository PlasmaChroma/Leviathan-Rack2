/* 08006f34 AudioHandle_SetBlockSize; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

bool AudioHandle_SetBlockSize(int *param_1,uint param_2)

{
  uint uVar1;
  
  uVar1 = param_2;
  if (0xff < param_2) {
    uVar1 = 0x100;
  }
  *(uint *)(*param_1 + 8) = uVar1;
  return 0x100 < param_2;
}


