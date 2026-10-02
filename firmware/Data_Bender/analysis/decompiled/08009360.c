/* 08009360 SaiHandle_GetSampleRate; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

undefined4 SaiHandle_GetSampleRate(int *param_1)

{
  if (*(uint *)(*param_1 + 0x10) < 5) {
    return *(undefined4 *)(DAT_0800937c + *(uint *)(*param_1 + 0x10) * 4);
  }
  return DAT_08009380;
}


