/* 08000d28 DB_Vinyl_ProcessStereo; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DB_Vinyl_ProcessStereo(float param_1,float param_2,int param_3,float *param_4,float *param_5)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  float fVar4;
  float fVar7;
  double dVar5;
  double dVar6;
  float fVar8;
  float fVar9;
  float fVar10;
  float fVar11;
  float fVar12;
  float fVar13;
  float fVar14;
  float local_30;
  float local_2c;
  float local_28;
  float local_24;
  
  if (*(char *)(param_3 + 0x16c) == '\0') {
    *param_4 = param_1;
    *param_5 = param_2;
    return;
  }
  iVar2 = *(int *)(param_3 + 0x120) + 1;
  iVar1 = *(int *)(param_3 + 0x128) + 1;
  iVar2 = iVar2 - *(int *)(param_3 + 0x124) * (iVar2 / *(int *)(param_3 + 0x124));
  iVar1 = iVar1 - *(int *)(param_3 + 300) * (iVar1 / *(int *)(param_3 + 300));
  *(int *)(param_3 + 0x120) = iVar2;
  *(int *)(param_3 + 0x128) = iVar1;
  if (iVar1 == 0) {
    fVar9 = (float)FixedToFP(*(undefined4 *)(param_3 + 0x118),0x20,0x20,0x1f,0,0);
    *(float *)(param_3 + 0x130) = fVar9 * DAT_080010c8;
  }
  if (iVar2 == 0) {
    iVar1 = *(int *)(param_3 + 0x118) * 0x41a7;
    *(int *)(param_3 + 0x118) = iVar1;
    fVar9 = (float)FixedToFP(iVar1,0x20,0x20,0x1f,0,0);
    fVar9 = fVar9 * *(float *)(param_3 + 0x114) * *(float *)(param_3 + 0x130);
    *(float *)(param_3 + 0x11c) = fVar9;
  }
  else {
    fVar9 = *(float *)(param_3 + 0x11c);
  }
  fVar8 = DAT_080010bc;
  iVar1 = *(int *)(param_3 + 0x138) * 0x41a7;
  iVar2 = *(int *)(param_3 + 8) + 1;
  *(int *)(param_3 + 0x138) = iVar1;
  iVar2 = iVar2 - *(int *)(param_3 + 0xc) * (iVar2 / *(int *)(param_3 + 0xc));
  *(int *)(param_3 + 8) = iVar2;
  fVar14 = (float)(longlong)iVar1 * fVar8 * *(float *)(param_3 + 0x134);
  if (iVar2 == 0) {
    fVar10 = *(float *)(param_3 + 0x14);
    fVar7 = *(float *)(param_3 + 0x1c);
    uVar3 = DAT_080010c4 * *DAT_080010c0 + 0x3039 & 0x7fffffff;
    *DAT_080010c0 = uVar3;
    fVar8 = (float)(longlong)(int)uVar3 * fVar8;
    if (fVar10 == fVar7) {
      fVar7 = *(float *)(param_3 + 0x24);
      fVar10 = *(float *)(param_3 + 0x20);
      if (*(int *)(param_3 + 0x2c) == 0) goto LAB_08001278;
LAB_08000e44:
      dVar5 = (double)*(float *)(param_3 + 0x18);
      dVar6 = DAT_08001378;
      if ((int)((uint)(fVar8 < fVar10) << 0x1f) < 0) {
        dVar6 = (double)(fVar7 * fVar8) - 1.0;
      }
    }
    else {
      fVar10 = fVar10 * *(float *)(param_3 + 0x28);
      *(float *)(param_3 + 0x20) = fVar10;
      fVar7 = DAT_08001380;
      if (*(int *)(param_3 + 0x2c) != 0) {
        if (fVar10 != 0.0 && fVar10 < 0.0 == NAN(fVar10)) {
          fVar7 = 2.0 / fVar10;
        }
        *(float *)(param_3 + 0x24) = fVar7;
        goto LAB_08000e44;
      }
      if (fVar10 != 0.0 && fVar10 < 0.0 == NAN(fVar10)) {
        fVar7 = 1.0 / fVar10;
      }
      *(float *)(param_3 + 0x24) = fVar7;
LAB_08001278:
      dVar5 = (double)*(float *)(param_3 + 0x18);
      dVar6 = DAT_08001378;
      if ((int)((uint)(fVar8 < fVar10) << 0x1f) < 0) {
        dVar6 = (double)(fVar8 * fVar7);
      }
    }
    fVar8 = (float)(dVar5 * dVar6);
    *(float *)(param_3 + 0x10) = fVar8;
  }
  else {
    fVar8 = *(float *)(param_3 + 0x10);
  }
  local_28 = fVar8 + fVar9 + fVar14;
  local_30 = param_2;
  local_2c = param_1;
  fVar8 = (float)DaisySP_ATone_Process(param_3 + 0xe4,&local_28);
  iVar1 = *(int *)(param_3 + 0x34) + 1;
  iVar1 = iVar1 - *(int *)(param_3 + 0x38) * (iVar1 / *(int *)(param_3 + 0x38));
  *(int *)(param_3 + 0x34) = iVar1;
  if (iVar1 == 0) {
    fVar10 = *(float *)(param_3 + 0x40);
    fVar11 = *(float *)(param_3 + 0x48);
    uVar3 = DAT_080010c4 * *DAT_080010c0 + 0x3039 & 0x7fffffff;
    *DAT_080010c0 = uVar3;
    fVar7 = (float)FixedToFP(uVar3,0x20,0x20,0x1f,0,0);
    if (fVar10 == fVar11) {
      fVar11 = *(float *)(param_3 + 0x50);
      fVar10 = *(float *)(param_3 + 0x4c);
      if (*(int *)(param_3 + 0x58) != 0) goto LAB_08001230;
LAB_080012ea:
      dVar5 = (double)*(float *)(param_3 + 0x44);
      dVar6 = DAT_08001378;
      if ((int)((uint)(fVar7 < fVar10) << 0x1f) < 0) {
        dVar6 = (double)(fVar7 * fVar11);
      }
    }
    else {
      fVar10 = fVar10 * *(float *)(param_3 + 0x54);
      *(float *)(param_3 + 0x4c) = fVar10;
      fVar11 = DAT_08001380;
      if (*(int *)(param_3 + 0x58) == 0) {
        if (fVar10 != 0.0 && fVar10 < 0.0 == NAN(fVar10)) {
          fVar11 = 1.0 / fVar10;
        }
        *(float *)(param_3 + 0x50) = fVar11;
        goto LAB_080012ea;
      }
      if (fVar10 != 0.0 && fVar10 < 0.0 == NAN(fVar10)) {
        fVar11 = 2.0 / fVar10;
      }
      *(float *)(param_3 + 0x50) = fVar11;
LAB_08001230:
      dVar5 = (double)*(float *)(param_3 + 0x44);
      dVar6 = DAT_08001378;
      if ((int)((uint)(fVar7 < fVar10) << 0x1f) < 0) {
        dVar6 = (double)(fVar11 * fVar7) - 1.0;
      }
    }
    fVar7 = (float)(dVar5 * dVar6);
    *(float *)(param_3 + 0x3c) = fVar7;
  }
  else {
    fVar7 = *(float *)(param_3 + 0x3c);
  }
  local_24 = fVar7 + fVar9 + fVar14;
  fVar9 = (float)DaisySP_ATone_Process(param_3 + 0xfc,&local_24);
  iVar1 = *(int *)(param_3 + 0x8c) + 1;
  iVar1 = iVar1 - *(int *)(param_3 + 0x90) * (iVar1 / *(int *)(param_3 + 0x90));
  *(int *)(param_3 + 0x8c) = iVar1;
  if (iVar1 == 0) {
    fVar10 = *(float *)(param_3 + 0x98);
    fVar14 = *(float *)(param_3 + 0xa0);
    uVar3 = DAT_080010c4 * *DAT_080010c0 + 0x3039 & 0x7fffffff;
    *DAT_080010c0 = uVar3;
    fVar7 = (float)FixedToFP(uVar3,0x20,0x20,0x1f,0,0);
    if (fVar10 == fVar14) {
      fVar14 = *(float *)(param_3 + 0xa8);
      fVar10 = *(float *)(param_3 + 0xa4);
      if (*(int *)(param_3 + 0xb0) != 0) goto LAB_08001148;
LAB_080011d0:
      dVar5 = (double)*(float *)(param_3 + 0x9c);
      dVar6 = DAT_08001378;
      if ((int)((uint)(fVar7 < fVar10) << 0x1f) < 0) {
        dVar6 = (double)(fVar7 * fVar14);
      }
    }
    else {
      fVar10 = fVar10 * *(float *)(param_3 + 0xac);
      *(float *)(param_3 + 0xa4) = fVar10;
      fVar14 = DAT_08001380;
      if (*(int *)(param_3 + 0xb0) == 0) {
        if (fVar10 != 0.0 && fVar10 < 0.0 == NAN(fVar10)) {
          fVar14 = 1.0 / fVar10;
        }
        *(float *)(param_3 + 0xa8) = fVar14;
        goto LAB_080011d0;
      }
      if (fVar10 != 0.0 && fVar10 < 0.0 == NAN(fVar10)) {
        fVar14 = 2.0 / fVar10;
      }
      *(float *)(param_3 + 0xa8) = fVar14;
LAB_08001148:
      dVar5 = (double)*(float *)(param_3 + 0x9c);
      dVar6 = DAT_08001378;
      if ((int)((uint)(fVar7 < fVar10) << 0x1f) < 0) {
        dVar6 = (double)(fVar14 * fVar7) - 1.0;
      }
    }
    fVar14 = (float)(dVar5 * dVar6);
    *(float *)(param_3 + 0x94) = fVar14;
  }
  else {
    fVar14 = *(float *)(param_3 + 0x94);
  }
  iVar1 = *(int *)(param_3 + 0x60) + 1;
  iVar1 = iVar1 - *(int *)(param_3 + 100) * (iVar1 / *(int *)(param_3 + 100));
  *(int *)(param_3 + 0x60) = iVar1;
  if (iVar1 == 0) {
    fVar11 = *(float *)(param_3 + 0x6c);
    fVar7 = *(float *)(param_3 + 0x74);
    uVar3 = DAT_080010c4 * *DAT_080010c0 + 0x3039 & 0x7fffffff;
    *DAT_080010c0 = uVar3;
    fVar10 = (float)FixedToFP(uVar3,0x20,0x20,0x1f,0,0);
    if (fVar11 == fVar7) {
      fVar7 = *(float *)(param_3 + 0x7c);
      fVar11 = *(float *)(param_3 + 0x78);
      if (*(int *)(param_3 + 0x84) != 0) goto LAB_080012a2;
LAB_0800131e:
      dVar5 = (double)*(float *)(param_3 + 0x70);
      dVar6 = DAT_08001378;
      if ((int)((uint)(fVar10 < fVar11) << 0x1f) < 0) {
        dVar6 = (double)(fVar10 * fVar7);
      }
    }
    else {
      fVar11 = fVar11 * *(float *)(param_3 + 0x80);
      *(float *)(param_3 + 0x78) = fVar11;
      fVar7 = DAT_08001380;
      if (*(int *)(param_3 + 0x84) == 0) {
        if (fVar11 != 0.0 && fVar11 < 0.0 == NAN(fVar11)) {
          fVar7 = 1.0 / fVar11;
        }
        *(float *)(param_3 + 0x7c) = fVar7;
        goto LAB_0800131e;
      }
      if (fVar11 != 0.0 && fVar11 < 0.0 == NAN(fVar11)) {
        fVar7 = 2.0 / fVar11;
      }
      *(float *)(param_3 + 0x7c) = fVar7;
LAB_080012a2:
      dVar5 = (double)*(float *)(param_3 + 0x70);
      dVar6 = DAT_08001378;
      if ((int)((uint)(fVar10 < fVar11) << 0x1f) < 0) {
        dVar6 = (double)(fVar7 * fVar10) - 1.0;
      }
    }
    fVar7 = (float)(dVar5 * dVar6);
    *(float *)(param_3 + 0x68) = fVar7;
  }
  else {
    fVar7 = *(float *)(param_3 + 0x68);
  }
  iVar1 = *(int *)(param_3 + 0xb8) + 1;
  iVar1 = iVar1 - *(int *)(param_3 + 0xbc) * (iVar1 / *(int *)(param_3 + 0xbc));
  *(int *)(param_3 + 0xb8) = iVar1;
  if (iVar1 != 0) {
    fVar10 = *(float *)(param_3 + 0xc0);
    goto LAB_080010d4;
  }
  fVar11 = *(float *)(param_3 + 0xc4);
  fVar12 = *(float *)(param_3 + 0xcc);
  uVar3 = DAT_080010c4 * *DAT_080010c0 + 0x3039 & 0x7fffffff;
  *DAT_080010c0 = uVar3;
  fVar10 = (float)FixedToFP(uVar3,0x20,0x20,0x1f,0,0);
  if (fVar11 == fVar12) {
    fVar12 = *(float *)(param_3 + 0xd4);
    fVar11 = *(float *)(param_3 + 0xd0);
    if (*(int *)(param_3 + 0xdc) != 0) goto LAB_08001186;
LAB_08001206:
    dVar5 = (double)*(float *)(param_3 + 200);
    dVar6 = DAT_08001378;
    if ((int)((uint)(fVar10 < fVar11) << 0x1f) < 0) {
      dVar6 = (double)(fVar10 * fVar12);
    }
  }
  else {
    fVar11 = fVar11 * *(float *)(param_3 + 0xd8);
    *(float *)(param_3 + 0xd0) = fVar11;
    fVar12 = DAT_08001380;
    if (*(int *)(param_3 + 0xdc) == 0) {
      if (fVar11 != 0.0 && fVar11 < 0.0 == NAN(fVar11)) {
        fVar12 = 1.0 / fVar11;
      }
      *(float *)(param_3 + 0xd4) = fVar12;
      goto LAB_08001206;
    }
    if (fVar11 != 0.0 && fVar11 < 0.0 == NAN(fVar11)) {
      fVar12 = 2.0 / fVar11;
    }
    *(float *)(param_3 + 0xd4) = fVar12;
LAB_08001186:
    dVar5 = (double)*(float *)(param_3 + 200);
    dVar6 = DAT_08001378;
    if ((int)((uint)(fVar10 < fVar11) << 0x1f) < 0) {
      dVar6 = (double)(fVar12 * fVar10) - 1.0;
    }
  }
  fVar10 = (float)(dVar5 * dVar6);
  *(float *)(param_3 + 0xc0) = fVar10;
LAB_080010d4:
  fVar11 = fVar9 * DAT_080010c8;
  fVar12 = fVar8 * DAT_080010c8;
  local_2c = (float)DaisySP_ATone_Process(param_3 + 0x13c,&local_2c);
  fVar4 = (float)DaisySP_ATone_Process(param_3 + 0x154,&local_30);
  fVar13 = *(float *)(param_3 + 4);
  fVar9 = (fVar9 + fVar12 + fVar14 + fVar10) * DAT_080010cc;
  *param_4 = fVar13 * local_2c + (fVar8 + fVar11 + fVar7 + fVar10) * DAT_080010cc;
  *param_5 = fVar4 * fVar13 + fVar9;
  return;
}


