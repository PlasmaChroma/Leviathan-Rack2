/* 08017724 DaisySP_Svf_SetFreq; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DaisySP_Svf_SetFreq(undefined4 param_1,float *param_2)

{
  float fVar1;
  float fVar2;
  undefined4 uVar3;
  float fVar4;
  
  uVar3 = FPMaxNum(param_1,DAT_0801780c);
  fVar4 = (float)FPMinNum(uVar3,param_2[0x12]);
  fVar2 = fVar4 / (*param_2 + *param_2);
  param_2[1] = fVar4;
  fVar4 = param_2[2];
  if (fVar2 == 0.25 || fVar2 < 0.25 != NAN(fVar2)) {
    fVar2 = (float)libm_sinf(fVar2 * DAT_08017810);
    fVar2 = fVar2 + fVar2;
    param_2[4] = fVar2;
    fVar1 = (float)libm_powf(fVar4,0x3e800000);
    fVar1 = (1.0 - fVar1) + (1.0 - fVar1);
    fVar2 = 2.0 / fVar2 + -fVar2 * 0.5;
    fVar2 = (float)((uint)(fVar2 != 2.0) * 0x40000000 + (uint)(fVar2 == 2.0) * (int)fVar2);
    if (fVar2 != fVar1 && fVar2 < fVar1 == (NAN(fVar2) || NAN(fVar1))) {
LAB_080177e8:
      fVar2 = (float)libm_powf(fVar4,0x3e800000);
      param_2[5] = (1.0 - fVar2) + (1.0 - fVar2);
      return;
    }
  }
  else {
    param_2[4] = DAT_08017814;
    fVar2 = (float)libm_powf(fVar4);
    fVar1 = (1.0 - fVar2) + (1.0 - fVar2);
    fVar2 = DAT_08017818;
    if (DAT_08017818 != fVar1 && DAT_08017818 < fVar1 == (NAN(DAT_08017818) || NAN(fVar1)))
    goto LAB_080177e8;
  }
  param_2[5] = fVar2;
  return;
}


