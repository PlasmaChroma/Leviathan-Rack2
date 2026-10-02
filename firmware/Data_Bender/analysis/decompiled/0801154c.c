/* 0801154c FUN_0801154c; analyst naming is provisional. */

void FUN_0801154c(uint *param_1)

{
  int *piVar1;
  uint uVar2;
  uint uVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  
  piVar1 = DAT_080116c0;
  uVar3 = (uint)(DAT_080116c0[10] << 0x16) >> 0x1a;
  if ((DAT_080116c0[10] & 0x3f0U) == 0) {
    *param_1 = uVar3;
    param_1[1] = uVar3;
    param_1[2] = uVar3;
    return;
  }
  uVar2 = DAT_080116c0[10] & 3;
  fVar4 = (float)(longlong)
                 (int)((DAT_080116c0[0xb] & 1U) * ((uint)(DAT_080116c0[0xd] << 0x10) >> 0x13));
  if (uVar2 == 1) {
    fVar5 = (float)(longlong)(int)uVar3;
    fVar6 = DAT_080116d4;
  }
  else if (uVar2 == 2) {
    fVar5 = (float)(longlong)(int)uVar3;
    fVar6 = DAT_080116d0;
  }
  else {
    if (uVar2 == 0) {
      if (*DAT_080116c0 << 0x1a < 0) {
        fVar4 = ((float)(longlong)(int)(DAT_080116c0[0xc] & 0x1ff) + fVar4 * DAT_080116c8 + 1.0) *
                ((float)(longlong)(int)(DAT_080116cc >> ((uint)(*DAT_080116c0 << 0x1b) >> 0x1e)) /
                (float)(longlong)(int)uVar3);
      }
      else {
        fVar4 = (DAT_080116c4 / (float)(longlong)(int)uVar3) *
                ((float)(longlong)(int)(DAT_080116c0[0xc] & 0x1ff) + fVar4 * DAT_080116c8 + 1.0);
      }
      goto LAB_080115b8;
    }
    fVar5 = (float)(longlong)(int)uVar3;
    fVar6 = DAT_080116c4;
  }
  fVar4 = (fVar6 / fVar5) *
          ((float)(longlong)(int)(DAT_080116c0[0xc] & 0x1ff) + fVar4 * DAT_080116c8 + 1.0);
LAB_080115b8:
  fVar6 = fVar4 / ((float)(longlong)(int)((uint)(DAT_080116c0[0xc] << 0x10) >> 0x19) + 1.0);
  *param_1 = (uint)(0.0 < fVar6) * (int)fVar6;
  fVar6 = fVar4 / ((float)(longlong)(int)((uint)(piVar1[0xc] << 9) >> 0x19) + 1.0);
  param_1[1] = (uint)(0.0 < fVar6) * (int)fVar6;
  fVar4 = fVar4 / ((float)(longlong)(int)((uint)(piVar1[0xc] << 1) >> 0x19) + 1.0);
  param_1[2] = (uint)(0.0 < fVar4) * (int)fVar4;
  return;
}


