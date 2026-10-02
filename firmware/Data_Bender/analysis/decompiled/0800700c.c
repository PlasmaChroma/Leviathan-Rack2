/* 0800700c AnalogControl_InitBipolarCv; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void AnalogControl_InitBipolarCv(float param_1,undefined4 *param_2,undefined4 param_3)

{
  float fVar1;
  float fVar2;
  
  fVar1 = DAT_08007074;
  fVar2 = param_1 * DAT_08007070;
  *param_2 = param_3;
  param_2[3] = fVar1;
  param_2[2] = param_1;
  fVar2 = 1.0 / (fVar2 * 0.5);
  if (fVar2 == 1.0 || fVar2 < 1.0 != NAN(fVar2)) {
    if ((int)((uint)(fVar2 < fVar1) << 0x1f) < 0) {
      fVar2 = fVar1;
    }
  }
  else {
    fVar2 = 1.0;
  }
  param_2[1] = fVar2;
  param_2[4] = 0x40000000;
  param_2[5] = 0x3f000000;
  *(undefined2 *)(param_2 + 6) = 0x100;
  *(undefined *)((int)param_2 + 0x1a) = 1;
  return;
}


