/* 08006260 SdramHandle_Init; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

int SdramHandle_Init(undefined4 param_1)

{
  int iVar1;
  
  iVar1 = SdramHandle_ConfigureController();
  if (iVar1 != 0) {
    return 1;
  }
  iVar1 = SdramHandle_InitSequence(param_1);
  if (iVar1 != 0) {
    iVar1 = 1;
  }
  return iVar1;
}


