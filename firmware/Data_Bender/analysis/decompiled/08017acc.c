/* 08017acc DaisySP_Tone_CalculateCoefficients; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DaisySP_Tone_CalculateCoefficients(int param_1)

{
  float fVar1;
  float fVar2;
  
  fVar1 = (float)libm_cosf((*(float *)(param_1 + 0xc) * DAT_08017b34) / *(float *)(param_1 + 0x18));
  fVar1 = 2.0 - fVar1;
  fVar2 = fVar1 * fVar1 + -1.0;
  if ((int)((uint)(fVar2 < 0.0) << 0x1f) < 0) {
    fVar2 = (float)FUN_080184c8();
  }
  else {
    fVar2 = SQRT(fVar2);
  }
  *(float *)(param_1 + 0x14) = fVar1 - fVar2;
  *(float *)(param_1 + 0x10) = 1.0 - (fVar1 - fVar2);
  return;
}


