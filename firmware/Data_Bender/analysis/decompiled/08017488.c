/* 08017488 DaisySP_CrossFade_Process; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

float DaisySP_CrossFade_Process(float *param_1,float *param_2,float *param_3)

{
  float fVar1;
  float fVar2;
  float fVar3;
  
  fVar1 = DAT_08017554;
  fVar2 = DAT_08017558;
  switch(*(undefined *)(param_1 + 1)) {
  case 0:
    fVar1 = *param_1;
    break;
  case 1:
    fVar3 = *param_1;
    fVar2 = (float)libm_sinf(fVar3 * DAT_08017554);
    fVar1 = (float)libm_sinf((1.0 - fVar3) * fVar1);
    return fVar2 * *param_3 + *param_2 * fVar1;
  case 2:
    fVar1 = (float)libm_expf(DAT_08017550 + *param_1 * DAT_0801754c);
    break;
  case 3:
    fVar1 = *param_1 * *param_1;
    fVar2 = fVar1 * *param_3 + *param_2 * (1.0 - fVar1);
  default:
    return fVar2;
  }
  return fVar1 * *param_3 + *param_2 * (1.0 - fVar1);
}


