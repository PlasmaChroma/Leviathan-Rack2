/* 0801781c DaisySP_Svf_SetRes; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DaisySP_Svf_SetRes(undefined4 param_1,int param_2)

{
  float fVar1;
  float fVar2;
  undefined4 uVar3;
  float fVar4;
  
  uVar3 = FPMaxNum(param_1,DAT_080178a4);
  fVar4 = (float)FPMinNum(uVar3,0x3f800000);
  *(float *)(param_2 + 8) = fVar4;
  fVar1 = (float)libm_powf(fVar4,0x3e800000);
  fVar2 = 2.0 / *(float *)(param_2 + 0x10) + -*(float *)(param_2 + 0x10) * 0.5;
  fVar2 = (float)((uint)(fVar2 != 2.0) * 0x40000000 + (uint)(fVar2 == 2.0) * (int)fVar2);
  if ((int)((uint)((1.0 - fVar1) + (1.0 - fVar1) < fVar2) << 0x1f) < 0) {
    fVar1 = (float)libm_powf(fVar4,0x3e800000);
    fVar2 = (1.0 - fVar1) + (1.0 - fVar1);
  }
  *(float *)(param_2 + 0x14) = fVar2;
  *(float *)(param_2 + 0xc) = *(float *)(param_2 + 0x44) * fVar4;
  return;
}


