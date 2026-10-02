/* 08006fa8 AnalogControl_Init; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void AnalogControl_Init(float param_1,float param_2,undefined4 *param_3,undefined4 param_4,
                       undefined param_5,undefined param_6)

{
  float fVar1;
  float fVar2;
  
  fVar1 = DAT_08007008;
  *param_3 = param_4;
  param_3[3] = fVar1;
  param_3[2] = param_1;
  fVar2 = 1.0 / (param_1 * param_2 * 0.5);
  if (fVar2 == 1.0 || fVar2 < 1.0 != NAN(fVar2)) {
    if ((int)((uint)(fVar2 < fVar1) << 0x1f) < 0) {
      fVar2 = fVar1;
    }
  }
  else {
    fVar2 = 1.0;
  }
  *(undefined *)(param_3 + 6) = param_5;
  param_3[1] = fVar2;
  param_3[5] = 0;
  param_3[4] = 0x3f800000;
  *(undefined *)((int)param_3 + 0x19) = param_6;
  *(undefined *)((int)param_3 + 0x1a) = 0;
  param_3[7] = param_2;
  return;
}


