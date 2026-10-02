/* 0801765c DaisySP_Svf_Process; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DaisySP_Svf_Process(float param_1,int param_2)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  
  fVar6 = *(float *)(param_2 + 0x24);
  fVar3 = *(float *)(param_2 + 0x10);
  fVar1 = param_1 + -*(float *)(param_2 + 0x14) * fVar6;
  fVar4 = *(float *)(param_2 + 0x1c) + fVar6 * fVar3;
  *(float *)(param_2 + 0x2c) = param_1;
  fVar2 = fVar1 - fVar4;
  fVar5 = fVar6 + fVar3 * fVar2 + -fVar6 * fVar6 * *(float *)(param_2 + 0xc) * fVar6;
  param_1 = param_1 + -*(float *)(param_2 + 0x14) * fVar5;
  fVar7 = fVar4 + fVar3 * fVar5;
  *(float *)(param_2 + 0x18) = param_1;
  fVar6 = param_1 - fVar7;
  *(float *)(param_2 + 0x1c) = fVar7;
  *(float *)(param_2 + 0x20) = fVar6;
  *(float *)(param_2 + 0x30) = fVar7 * 0.5 + fVar4 * 0.5;
  fVar3 = fVar5 + fVar3 * fVar6 + -fVar5 * *(float *)(param_2 + 0xc) * fVar5 * fVar5;
  *(float *)(param_2 + 0x40) = param_1 * 0.5 + fVar1 * 0.5;
  *(float *)(param_2 + 0x3c) = (fVar7 - fVar6) * 0.5 + (fVar4 - fVar2) * 0.5;
  *(float *)(param_2 + 0x24) = fVar3;
  *(float *)(param_2 + 0x34) = fVar6 * 0.5 + fVar2 * 0.5;
  *(float *)(param_2 + 0x38) = fVar3 * 0.5 + fVar5 * 0.5;
  return;
}


