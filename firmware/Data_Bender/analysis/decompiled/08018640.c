/* 08018640 FUN_08018640; analyst naming is provisional. */

/* WARNING: Removing unreachable block (ram,0x08017d1e) */

float FUN_08018640(float param_1)

{
  undefined4 *puVar1;
  uint uVar2;
  float fVar3;
  float fVar4;
  float in_s15;
  float fVar5;
  
  uVar2 = (uint)param_1 & 0x7fffffff;
  if (DAT_08018870 < uVar2) {
    if (0x7f800000 < uVar2) {
      return param_1 + param_1;
    }
    if (uVar2 == 0x7f800000) {
      return (float)((uint)(-1 < (int)param_1) * (int)param_1 +
                    (uint)(-1 >= (int)param_1) * -0x40800000);
    }
    if ((int)param_1 < 0) {
      if ((int)((uint)(param_1 + DAT_080188a0 < 0.0) << 0x1f) < 0) {
        return -1.0;
      }
      fVar5 = -0.5;
    }
    else {
      if (DAT_08018874 < uVar2) {
        fVar5 = DAT_08017d5c * DAT_08017d5c;
        puVar1 = (undefined4 *)FUN_080188d8();
        *puVar1 = 0x22;
        return fVar5;
      }
      fVar5 = 0.5;
    }
LAB_08018672:
    uVar2 = (uint)(fVar5 + param_1 * DAT_08018878);
    fVar3 = param_1 + -(float)(longlong)(int)uVar2 * DAT_0801887c;
    fVar5 = (float)(longlong)(int)uVar2 * DAT_08018880;
  }
  else {
    if (uVar2 <= DAT_08018884) {
      if (uVar2 < 0x33000000) {
        return param_1 - ((param_1 + DAT_080188a4) - (param_1 + DAT_080188a4));
      }
      uVar2 = 0;
      goto LAB_080186b4;
    }
    if (DAT_0801889c < uVar2) {
      fVar5 = (float)((uint)(-1 < (int)param_1) * 0x3f000000 +
                     (uint)(-1 >= (int)param_1) * -0x41000000);
      goto LAB_08018672;
    }
    if ((int)param_1 < 0) {
      fVar3 = param_1 + DAT_0801887c;
      uVar2 = 0xffffffff;
      fVar5 = DAT_080188a8;
    }
    else {
      fVar3 = param_1 - DAT_0801887c;
      uVar2 = 1;
      fVar5 = DAT_08018880;
    }
  }
  param_1 = fVar3 - fVar5;
  in_s15 = (fVar3 - param_1) - fVar5;
LAB_080186b4:
  fVar4 = param_1 * param_1 * 0.5;
  fVar3 = (DAT_08018894 +
          (DAT_08018898 + (DAT_08018890 + (DAT_0801888c + fVar4 * DAT_08018888) * fVar4) * fVar4) *
          fVar4) * fVar4 + 1.0;
  fVar5 = -(param_1 * 0.5) * fVar3 + 3.0;
  fVar5 = ((fVar3 - fVar5) / (-param_1 * fVar5 + 6.0)) * fVar4;
  if (uVar2 == 0) {
    return param_1 - (-fVar4 + param_1 * fVar5);
  }
  fVar4 = (-in_s15 + (fVar5 - in_s15) * param_1) - fVar4;
  if (uVar2 == 0xffffffff) {
    return (param_1 - fVar4) * 0.5 + -0.5;
  }
  if (uVar2 == 1) {
    if (-1 < (int)((uint)(param_1 < -0.25) << 0x1f)) {
      return (param_1 - fVar4) * 2.0 + 1.0;
    }
    return (fVar4 - (param_1 + 0.5)) * -2.0;
  }
  if (0x39 < uVar2 + 1) {
    return (float)((int)(1.0 - (fVar4 - param_1)) + uVar2 * 0x800000) - 1.0;
  }
  if (0x16 < (int)uVar2) {
    return (float)((int)((param_1 - (fVar4 + (float)((0x7f - uVar2) * 0x800000))) + 1.0) +
                  uVar2 * 0x800000);
  }
  return (float)((int)((float)(0x3f800000 - (0x1000000 >> (uVar2 & 0xff))) - (fVar4 - param_1)) +
                uVar2 * 0x800000);
}


