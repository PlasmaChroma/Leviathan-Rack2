/* 0800f9d0 FUN_0800f9d0; analyst naming is provisional. */

uint FUN_0800f9d0(void)

{
  uint uVar1;
  uint uVar2;
  float fVar3;
  float fVar4;
  
  uVar1 = (uint)(DAT_0800fae8[10] << 0x16) >> 0x1a;
  if ((DAT_0800fae8[10] & 0x3f0U) != 0) {
    uVar2 = DAT_0800fae8[10] & 3;
    fVar4 = (float)(longlong)
                   (int)((DAT_0800fae8[0xb] & 1U) * ((uint)(DAT_0800fae8[0xd] << 0x10) >> 0x13));
    fVar3 = DAT_0800faec;
    if (((uVar2 == 1) || (fVar3 = DAT_0800faf8, uVar2 == 2)) || (fVar3 = DAT_0800faec, uVar2 != 0))
    {
      fVar3 = (fVar3 / (float)(longlong)(int)uVar1) *
              ((float)(longlong)(int)(DAT_0800fae8[0xc] & 0x1ff) + fVar4 * DAT_0800faf0 + 1.0);
    }
    else if (*DAT_0800fae8 << 0x1a < 0) {
      fVar3 = ((float)(longlong)(int)(DAT_0800fae8[0xc] & 0x1ff) + fVar4 * DAT_0800faf0 + 1.0) *
              ((float)(longlong)(int)(DAT_0800faf4 >> ((uint)(*DAT_0800fae8 << 0x1b) >> 0x1e)) /
              (float)(longlong)(int)uVar1);
    }
    else {
      fVar3 = (DAT_0800fafc / (float)(longlong)(int)uVar1) *
              ((float)(longlong)(int)(DAT_0800fae8[0xc] & 0x1ff) + fVar4 * DAT_0800faf0 + 1.0);
    }
    fVar3 = fVar3 / (float)(longlong)(int)(((uint)(DAT_0800fae8[0xc] << 0x10) >> 0x19) + 1);
    uVar1 = (uint)(0.0 < fVar3) * (int)fVar3;
  }
  return uVar1;
}


