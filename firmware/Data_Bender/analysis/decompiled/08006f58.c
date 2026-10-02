/* 08006f58 AudioHandle_Start_interleaved; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void AudioHandle_Start_interleaved(undefined4 *param_1,undefined4 param_2)

{
  param_1 = (undefined4 *)*param_1;
  SaiHandle_StartDma(param_1 + 6,param_1[8],param_1[10],param_1[2] << 2,DAT_08006f80);
  *param_1 = 0;
  param_1[1] = param_2;
  return;
}


