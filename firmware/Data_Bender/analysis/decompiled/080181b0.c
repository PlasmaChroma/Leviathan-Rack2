/* 080181b0 libm_tanhf; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

float libm_tanhf(float param_1)

{
  undefined *puVar1;
  float fVar2;
  
  puVar1 = (undefined *)((uint)param_1 & 0x7fffffff);
  if ((undefined *)0x7f7fffff < puVar1) {
    if (-1 < (int)param_1) {
      return 1.0 / param_1 + 1.0;
    }
    return 1.0 / param_1 - 1.0;
  }
  if (DAT_08018264 < (int)puVar1) {
    fVar2 = 1.0;
  }
  else {
    if (puVar1 < &DAT_24000000) {
      return (param_1 + 1.0) * param_1;
    }
    if (puVar1 < (undefined *)0x3f800000) {
      fVar2 = (float)FUN_080188ac();
      fVar2 = (float)FUN_08018640(fVar2 * -2.0);
      fVar2 = -fVar2 / (fVar2 + 2.0);
    }
    else {
      fVar2 = (float)FUN_080188ac();
      fVar2 = (float)FUN_08018640(fVar2 + fVar2);
      fVar2 = 1.0 - 2.0 / (fVar2 + 2.0);
    }
  }
  if ((int)param_1 < 0) {
    fVar2 = -fVar2;
  }
  return fVar2;
}


