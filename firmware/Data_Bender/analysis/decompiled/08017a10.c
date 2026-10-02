/* 08017a10 DaisySP_ATone_Process; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DaisySP_ATone_Process(int param_1,float *param_2)

{
  *(float *)(param_1 + 4) =
       (*param_2 + *(float *)(param_1 + 4)) * *(float *)(param_1 + 0x10) - *param_2;
  return;
}


