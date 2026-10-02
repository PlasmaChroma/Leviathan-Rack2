/* 08007078 AnalogControl_Process; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void AnalogControl_Process(ushort **param_1)

{
  float fVar1;
  
  fVar1 = (float)FixedToFP((uint)**param_1,0x20,0x20,0x10,1,0);
  if (*(char *)(param_1 + 6) != '\0') {
    fVar1 = 1.0 - fVar1;
  }
  fVar1 = (fVar1 - (float)param_1[5]) * (float)param_1[4];
  if (*(char *)((int)param_1 + 0x19) != '\0') {
    fVar1 = -fVar1;
  }
  param_1[3] = (ushort *)((float)param_1[3] + (float)param_1[1] * (fVar1 - (float)param_1[3]));
  return;
}


