/* 08017aac DaisySP_Tone_Process; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

float DaisySP_Tone_Process(float param_1,int param_2)

{
  float fVar1;
  
  fVar1 = *(float *)(param_2 + 0x14) * *(float *)(param_2 + 4) +
          *(float *)(param_2 + 0x10) * param_1;
  *(float *)(param_2 + 4) = fVar1;
  return fVar1;
}


