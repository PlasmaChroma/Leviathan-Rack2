/* 080113d0 FUN_080113d0; analyst naming is provisional. */

void FUN_080113d0(uint *param_1)

{
  int *piVar1;
  uint uVar2;
  uint uVar3;
  float fVar4;
  float fVar5;
  
  piVar1 = DAT_08011534;
  uVar3 = (uint)(DAT_08011534[10] << 6) >> 0x1a;
  if ((DAT_08011534[10] & 0x3f00000U) != 0) {
    uVar2 = DAT_08011534[10] & 3;
    fVar5 = (float)(longlong)
                   (int)(-((DAT_08011534[0xb] << 0x17) >> 0x1f) *
                        ((uint)(DAT_08011534[0x11] << 0x10) >> 0x13));
    fVar4 = DAT_08011538;
    if (((uVar2 == 1) || (fVar4 = DAT_08011544, uVar2 == 2)) || (fVar4 = DAT_08011538, uVar2 != 0))
    {
      fVar4 = (fVar4 / (float)(longlong)(int)uVar3) *
              ((float)(longlong)(int)(DAT_08011534[0x10] & 0x1ff) + fVar5 * DAT_0801153c + 1.0);
    }
    else if (*DAT_08011534 << 0x1a < 0) {
      fVar4 = ((float)(longlong)(int)(DAT_08011534[0x10] & 0x1ff) + fVar5 * DAT_0801153c + 1.0) *
              ((float)(longlong)(int)(DAT_08011540 >> ((uint)(*DAT_08011534 << 0x1b) >> 0x1e)) /
              (float)(longlong)(int)uVar3);
    }
    else {
      fVar4 = (DAT_08011548 / (float)(longlong)(int)uVar3) *
              ((float)(longlong)(int)(DAT_08011534[0x10] & 0x1ff) + fVar5 * DAT_0801153c + 1.0);
    }
    fVar5 = fVar4 / ((float)(longlong)(int)((uint)(DAT_08011534[0x10] << 0x10) >> 0x19) + 1.0);
    *param_1 = (uint)(0.0 < fVar5) * (int)fVar5;
    fVar5 = fVar4 / ((float)(longlong)(int)((uint)(piVar1[0x10] << 9) >> 0x19) + 1.0);
    param_1[1] = (uint)(0.0 < fVar5) * (int)fVar5;
    fVar4 = fVar4 / ((float)(longlong)(int)((uint)(piVar1[0x10] << 1) >> 0x19) + 1.0);
    param_1[2] = (uint)(0.0 < fVar4) * (int)fVar4;
    return;
  }
  *param_1 = uVar3;
  param_1[1] = uVar3;
  param_1[2] = uVar3;
  return;
}


