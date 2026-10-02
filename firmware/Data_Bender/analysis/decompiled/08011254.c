/* 08011254 FUN_08011254; analyst naming is provisional. */

void FUN_08011254(uint *param_1)

{
  int *piVar1;
  uint uVar2;
  uint uVar3;
  float fVar4;
  float fVar5;
  
  piVar1 = DAT_080113b8;
  uVar3 = (uint)(DAT_080113b8[10] << 0xe) >> 0x1a;
  if ((DAT_080113b8[10] & 0x3f000U) != 0) {
    uVar2 = DAT_080113b8[10] & 3;
    fVar5 = (float)(longlong)
                   (int)(-((DAT_080113b8[0xb] << 0x1b) >> 0x1f) *
                        ((uint)(DAT_080113b8[0xf] << 0x10) >> 0x13));
    fVar4 = DAT_080113bc;
    if (((uVar2 == 1) || (fVar4 = DAT_080113c8, uVar2 == 2)) || (fVar4 = DAT_080113bc, uVar2 != 0))
    {
      fVar4 = (fVar4 / (float)(longlong)(int)uVar3) *
              ((float)(longlong)(int)(DAT_080113b8[0xe] & 0x1ff) + fVar5 * DAT_080113c0 + 1.0);
    }
    else if (*DAT_080113b8 << 0x1a < 0) {
      fVar4 = ((float)(longlong)(int)(DAT_080113b8[0xe] & 0x1ff) + fVar5 * DAT_080113c0 + 1.0) *
              ((float)(longlong)(int)(DAT_080113c4 >> ((uint)(*DAT_080113b8 << 0x1b) >> 0x1e)) /
              (float)(longlong)(int)uVar3);
    }
    else {
      fVar4 = (DAT_080113cc / (float)(longlong)(int)uVar3) *
              ((float)(longlong)(int)(DAT_080113b8[0xe] & 0x1ff) + fVar5 * DAT_080113c0 + 1.0);
    }
    fVar5 = fVar4 / ((float)(longlong)(int)((uint)(DAT_080113b8[0xe] << 0x10) >> 0x19) + 1.0);
    *param_1 = (uint)(0.0 < fVar5) * (int)fVar5;
    fVar5 = fVar4 / ((float)(longlong)(int)((uint)(piVar1[0xe] << 9) >> 0x19) + 1.0);
    param_1[1] = (uint)(0.0 < fVar5) * (int)fVar5;
    fVar4 = fVar4 / ((float)(longlong)(int)((uint)(piVar1[0xe] << 1) >> 0x19) + 1.0);
    param_1[2] = (uint)(0.0 < fVar4) * (int)fVar4;
    return;
  }
  *param_1 = uVar3;
  param_1[1] = uVar3;
  param_1[2] = uVar3;
  return;
}


