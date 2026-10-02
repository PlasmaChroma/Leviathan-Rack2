/* 08005c60 DaisySeed_AudioBlockSize; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

undefined4 DaisySeed_AudioBlockSize(int param_1)

{
  undefined4 *puVar1;
  
  puVar1 = (undefined4 *)AudioHandle_GetConfig(param_1 + 0x14);
  return *puVar1;
}


