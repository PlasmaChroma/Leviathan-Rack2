/* 08009b90 FUN_08009b90; analyst naming is provisional. */

void FUN_08009b90(float param_1,float param_2,float param_3,int *param_4)

{
  if ((int)((uint)(param_1 < 0.0) << 0x1f) < 0) {
    *param_4 = DAT_08009c1c;
  }
  else {
    *param_4 = (uint)(param_1 != 1.0) * 0x3f800000 + (uint)(param_1 == 1.0) * (int)param_1;
  }
  if ((int)((uint)(param_2 < 0.0) << 0x1f) < 0) {
    param_4[1] = DAT_08009c1c;
  }
  else {
    param_4[1] = (uint)(param_2 != 1.0) * 0x3f800000 + (uint)(param_2 == 1.0) * (int)param_2;
  }
  if (-1 < (int)((uint)(param_3 < 0.0) << 0x1f)) {
    param_4[2] = (uint)(param_3 != 1.0) * 0x3f800000 + (uint)(param_3 == 1.0) * (int)param_3;
    return;
  }
  param_4[2] = DAT_08009c1c;
  return;
}


