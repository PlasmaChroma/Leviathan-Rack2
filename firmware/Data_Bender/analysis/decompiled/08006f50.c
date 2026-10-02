/* 08006f50 AudioHandle_GetSampleRate; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

undefined4 AudioHandle_GetSampleRate(int *param_1)

{
  uint uVar1;
  
  uVar1 = *(uint *)(*(int *)(*param_1 + 0x18) + 0x10);
  if (uVar1 < 5) {
    return *(undefined4 *)(DAT_0800937c + uVar1 * 4);
  }
  return DAT_08009380;
}


