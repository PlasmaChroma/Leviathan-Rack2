/* 08018518 FUN_08018518; analyst naming is provisional. */

float FUN_08018518(float param_1,float param_2)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  
  uVar1 = (uint)param_2 & 0x7fffffff;
  if (((uVar1 == 0) || (uVar5 = (uint)param_1 & 0x7fffffff, 0x7f7fffff < uVar5)) ||
     (0x7f800000 < uVar1)) {
    param_1 = (param_1 * param_2) / (param_1 * param_2);
  }
  else if (uVar1 <= uVar5) {
    uVar6 = (uint)param_1 & 0x80000000;
    if (uVar5 != uVar1) {
      if (((uint)param_1 & 0x7f800000) == 0) {
        iVar4 = -0x7e;
        for (iVar2 = uVar5 * 0x100; 0 < iVar2; iVar2 = iVar2 * 2) {
          iVar4 = iVar4 + -1;
        }
      }
      else {
        iVar4 = ((int)uVar5 >> 0x17) + -0x7f;
      }
      if (((uint)param_2 & 0x7f800000) == 0) {
        iVar2 = -0x7e;
        for (iVar3 = uVar1 << 8; -1 < iVar3; iVar3 = iVar3 << 1) {
          iVar2 = iVar2 + -1;
        }
      }
      else {
        iVar2 = ((int)uVar1 >> 0x17) + -0x7f;
      }
      if (iVar4 + 0x7e < 0 == SCARRY4(iVar4,0x7e)) {
        uVar5 = (uint)param_1 & 0x7fffff | 0x800000;
      }
      else {
        uVar5 = uVar5 << (-iVar4 - 0x7eU & 0xff);
      }
      if (iVar2 + 0x7e < 0 == SCARRY4(iVar2,0x7e)) {
        uVar1 = (uint)param_2 & 0x7fffff | 0x800000;
      }
      else {
        uVar1 = uVar1 << (-iVar2 - 0x7eU & 0xff);
      }
      for (iVar4 = iVar4 - iVar2; iVar4 != 0; iVar4 = iVar4 + -1) {
        iVar3 = uVar5 - uVar1;
        if (iVar3 < 0) {
          uVar5 = uVar5 << 1;
        }
        else {
          if (iVar3 == 0) goto LAB_080185bc;
          uVar5 = iVar3 * 2;
        }
      }
      if (-1 < (int)(uVar5 - uVar1)) {
        uVar5 = uVar5 - uVar1;
      }
      if (uVar5 != 0) {
        for (; (int)uVar5 < 0x800000; uVar5 = uVar5 << 1) {
          iVar2 = iVar2 + -1;
        }
        if (iVar2 + 0x7e < 0 == SCARRY4(iVar2,0x7e)) {
          return (float)(uVar5 - 0x800000 | uVar6 | (iVar2 + 0x7f) * 0x800000);
        }
        return (float)((int)uVar5 >> (-iVar2 - 0x7eU & 0xff) | uVar6);
      }
    }
LAB_080185bc:
    return *(float *)(DAT_08018634 + (uVar6 >> 0x1d));
  }
  return param_1;
}


