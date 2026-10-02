/* 080178a8 DaisySP_Svf_SetDrive; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DaisySP_Svf_SetDrive(float param_1,int param_2)

{
  undefined4 uVar1;
  float fVar2;
  
  uVar1 = FPMaxNum(param_1 * DAT_080178d4,DAT_080178d8);
  fVar2 = (float)FPMinNum(uVar1,0x3f800000);
  *(float *)(param_2 + 0x44) = fVar2;
  *(float *)(param_2 + 0xc) = *(float *)(param_2 + 8) * fVar2;
  return;
}


