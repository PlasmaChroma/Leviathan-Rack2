/* 0800985c System_GetTickFreq; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

int System_GetTickFreq(void)

{
  int iVar1;
  
  iVar1 = FUN_08010408();
  return iVar1 << 1;
}


