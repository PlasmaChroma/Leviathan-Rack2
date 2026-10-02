/* 08001e4c DB_Engine_MapControlsAndProcess; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DB_Engine_MapControlsAndProcess(int param_1,float *param_2,undefined4 *param_3,uint param_4)

{
  longlong lVar1;
  int iVar2;
  int iVar3;
  char cVar4;
  float *pfVar5;
  uint uVar6;
  byte bVar7;
  char cVar8;
  uint uVar9;
  undefined4 *puVar10;
  float *pfVar11;
  undefined4 *puVar12;
  bool bVar13;
  float fVar14;
  float fVar15;
  float fVar16;
  float fVar17;
  uint uVar18;
  undefined4 uVar19;
  undefined8 uVar20;
  undefined *local_70 [3];
  uint local_64;
  float local_5c [4];
  float local_4c;
  float fStack_48;
  float fStack_44;
  
  fVar14 = DAT_08002050;
  fVar17 = *(float *)(param_1 + 0x6c8) + *(float *)(param_1 + 0x54) * *(float *)(param_1 + 0x6cc);
  fVar15 = *(float *)(param_1 + 0x30);
  uVar18 = FPToFixed(*(undefined4 *)(param_1 + 0x28),0x20,0x20,3,0,3);
  iVar3 = *(int *)(param_1 + 4);
  *(float *)(param_1 + 0x54) = fVar17;
  *(float *)(param_1 + 0x104) = (float)(ulonglong)uVar18;
  uVar19 = FPMaxNum(fVar17 + fVar15,fVar14);
  uVar19 = FPMinNum(uVar19,0x3f800000);
  *(undefined4 *)(param_1 + 0x18) = uVar19;
  if (iVar3 == 0) {
    uVar19 = FPMaxNum(fVar15 + fVar17 * *(float *)(param_1 + 0x3c),fVar14);
    uVar19 = FPMinNum(uVar19,0x3f800000);
    *(undefined4 *)(param_1 + 0x18) = uVar19;
  }
  fVar14 = DAT_08002050;
  uVar19 = FPMaxNum(*(float *)(param_1 + 0x68) + *(float *)(param_1 + 0x50),DAT_08002050);
  fVar16 = (float)FPMinNum(uVar19,0x3f800000);
  bVar13 = fVar16 < DAT_08002034;
  uVar19 = FPMaxNum(*(float *)(param_1 + 0x34) +
                    *(float *)(param_1 + 0x58) * *(float *)(param_1 + 0x40),DAT_08002050);
  fVar17 = (float)FPMinNum(uVar19,0x3f800000);
  uVar19 = FPMaxNum(*(float *)(param_1 + 0x38) +
                    *(float *)(param_1 + 0x5c) * *(float *)(param_1 + 0x44),DAT_08002050);
  *(float *)(param_1 + 0x1c) = fVar17;
  uVar19 = FPMinNum(uVar19,0x3f800000);
  *(undefined4 *)(param_1 + 0x20) = uVar19;
  if ((int)((uint)bVar13 << 0x1f) < 0) {
    *(float *)(param_1 + 0x2c) = fVar14;
  }
  else {
    *(uint *)(param_1 + 0x2c) =
         (uint)(fVar16 != DAT_0800205c) * 0x3f800000 + (uint)(fVar16 == DAT_0800205c) * (int)fVar16;
  }
  cVar4 = *(char *)(param_1 + 0x7c);
  *(undefined *)(param_1 + 0x2e0) = *(undefined *)(param_1 + 0x10);
  *(undefined4 *)(param_1 + 0x2e4) = uVar19;
  fVar14 = DAT_08002050;
  *(char *)(param_1 + 0x1b4) = cVar4;
  uVar19 = FPMaxNum(*(float *)(param_1 + 100) + *(float *)(param_1 + 0x4c),fVar14);
  *(undefined *)(param_1 + 0x75) = 0;
  uVar19 = FPMinNum(uVar19,0x3f800000);
  *(undefined4 *)(param_1 + 0x28) = uVar19;
  pfVar5 = DAT_08002048;
  local_70[0] = (undefined *)param_3;
  local_64 = param_4;
  if (iVar3 != 0) {
    if (iVar3 != 1) goto LAB_08002096;
    *(float *)(param_1 + 0x19c) = fVar14;
    *(float *)(param_1 + 0x1a0) = fVar14;
    fVar16 = DAT_08002038;
    local_5c[0] = *pfVar5;
    local_5c[1] = pfVar5[1];
    local_5c[2] = pfVar5[2];
    local_5c[3] = pfVar5[3];
    local_4c = pfVar5[4];
    fStack_48 = pfVar5[5];
    fStack_44 = pfVar5[6];
    if ((*(char *)(param_1 + 0xe) == '\0') && (*(char *)(param_1 + 0x13) == '\0')) {
      *(float *)(param_1 + 0x158) = fVar17;
    }
    else {
      *(float *)(param_1 + 0x158) = fVar14;
      if (-1 < (int)((uint)(fVar17 < fVar16) << 0x1f)) goto LAB_08001fae;
    }
    fVar17 = DAT_08002394;
LAB_08001fae:
    fVar16 = DAT_08002050;
    fVar14 = DAT_0800203c;
    pfVar11 = local_5c;
    *(float *)(param_1 + 0x120) = fVar17;
    fVar14 = (float)libm_expf(fVar16 + fVar15 * fVar14);
    fVar14 = fVar14 * 0.125;
    iVar3 = 1;
    pfVar5 = pfVar11;
    do {
      fVar15 = *pfVar5;
      pfVar5 = pfVar5 + 1;
      if (iVar3 == 1) {
        if ((int)((uint)(fVar15 - fVar15 * DAT_08002044 < fVar14) << 0x1f) < 0) {
          bVar13 = fVar15 + fVar15 * DAT_08002044 != fVar14;
          fVar14 = (float)((uint)bVar13 * (int)fVar15 + (uint)!bVar13 * (int)fVar14);
        }
      }
      else {
        if ((int)((uint)(fVar15 - fVar15 * DAT_08002040 < fVar14) << 0x1f) < 0) {
          bVar13 = fVar15 + fVar15 * DAT_08002040 != fVar14;
          fVar14 = (float)((uint)bVar13 * (int)fVar15 + (uint)!bVar13 * (int)fVar14);
        }
        if (iVar3 == 7) goto LAB_080022c2;
      }
      iVar3 = iVar3 + 1;
    } while( true );
  }
  if ((*(char *)(param_1 + 0xc) != '\0') || (*(char *)(param_1 + 0x12) != '\0')) {
    fVar14 = *(float *)(param_1 + 0x18);
  }
  *(float *)(param_1 + 0x19c) = fVar14;
  if (*(char *)(param_1 + 0xf) == '\0') {
    bVar13 = *(char *)(param_1 + 0x13) == '\0';
    fVar17 = (float)((uint)bVar13 * (int)DAT_08002394 + (uint)!bVar13 * (int)fVar17);
  }
  *(float *)(param_1 + 0x1a0) = fVar17;
  *(undefined4 *)(param_1 + 0x120) = 0;
  *(undefined4 *)(param_1 + 0x158) = 0;
  goto LAB_08002096;
LAB_080022c2:
  fVar15 = (float)libm_powf(0x40000000,*(float *)(param_1 + 0x54) * 5.0);
  iVar3 = 1;
  fVar14 = fVar14 * fVar15;
  do {
    fVar15 = *pfVar11;
    pfVar11 = pfVar11 + 1;
    if (iVar3 == 1) {
      if (((int)((uint)(fVar15 - fVar15 * DAT_0800239c < fVar14) << 0x1f) < 0) &&
         (fVar17 = fVar15 + fVar15 * DAT_0800239c,
         fVar17 != fVar14 && fVar17 < fVar14 == (NAN(fVar17) || NAN(fVar14)))) {
LAB_080022fe:
        *(undefined *)(param_1 + 0x75) = 1;
        fVar14 = fVar15;
        goto LAB_08002308;
      }
    }
    else {
      if (((int)((uint)(fVar15 - fVar15 * DAT_08002398 < fVar14) << 0x1f) < 0) &&
         (fVar17 = fVar15 + fVar15 * DAT_08002398,
         fVar17 != fVar14 && fVar17 < fVar14 == (NAN(fVar17) || NAN(fVar14)))) goto LAB_080022fe;
LAB_08002308:
      if (iVar3 == 7) break;
    }
    iVar3 = iVar3 + 1;
  } while( true );
  bVar7 = *(byte *)(param_1 + 0x75);
  if (fVar14 != 8.0 && fVar14 < 8.0 == NAN(fVar14)) {
    bVar7 = bVar7 | 1;
  }
  *(byte *)(param_1 + 0x75) = bVar7;
  if (*(int *)(param_1 + 4) == 0) {
    if (*(char *)(param_1 + 0xc) != '\0') goto LAB_0800208a;
LAB_0800237c:
    if (*(char *)(param_1 + 0x12) != '\0') goto LAB_0800208a;
  }
  else {
    if (*(char *)(param_1 + 0xd) == '\0') goto LAB_0800237c;
LAB_0800208a:
    fVar14 = -fVar14;
  }
  cVar4 = *(char *)(param_1 + 0x1b4);
  *(float *)(param_1 + 0x13c) = fVar14;
LAB_08002096:
  *(undefined4 *)(param_1 + 0x2d4) = *(undefined4 *)(param_1 + 0x684);
  cVar8 = *(char *)(param_1 + 0x11);
  if (cVar8 == '\0') {
    cVar8 = *(char *)(param_1 + 0x15);
  }
  *(char *)(param_1 + 0x1b3) = cVar8;
  if (cVar4 != '\0') {
    *(char *)(param_1 + 0x1b2) = cVar8;
  }
  if (*(char *)(param_1 + 0x74) != '\0') {
    uVar20 = newlib_rand_LCG64();
    iVar3 = (int)uVar20;
    lVar1 = (longlong)DAT_08002388;
    iVar2 = iVar3 + ((int)((ulonglong)(lVar1 * iVar3) >> 0x20) - (iVar3 >> 0x1f)) * -3 + 1;
    *(int *)(param_1 + 0x2dc) = iVar2;
    uVar18 = newlib_rand_LCG64(iVar2,(int)((ulonglong)uVar20 >> 0x20),(int)(lVar1 * iVar3));
    fVar15 = *(float *)(param_1 + 0x2e4) * DAT_0800238c;
    *(undefined *)(param_1 + 0x74) = 0;
    *(undefined4 *)(param_1 + 0x1b8) = *(undefined4 *)(param_1 + 0x90);
    fVar14 = DAT_08002390;
    uVar6 = (uint)(0.0 < fVar15) * (int)fVar15;
    *(undefined *)(param_1 + 0x1b1) = 1;
    *(float *)(param_1 + 0x2f0) = (float)(ulonglong)(uVar18 - (uVar18 / uVar6) * uVar6) / fVar14;
  }
  uVar18 = local_64;
  uVar6 = local_64 * 4 + 7 & 0xfffffff8;
  DB_Buffer_ProcessBlock(param_1 + 0x98,param_2,(int)local_70 + -uVar6);
  DB_Corrupt_ProcessInterleaved
            (param_1 + 0x2d4,(int)local_70 + -uVar6,(int)local_70 + uVar6 * -2,uVar18);
  uVar9 = (uVar18 >> 1) * 4 + 7 & 0xfffffff8;
  local_70[2] = (undefined *)((int)local_70 + (uVar6 * -2 - uVar9));
  local_70[1] = (undefined *)((int)local_70 + uVar9 * -2 + uVar6 * -2);
  if (uVar18 != 0) {
    uVar18 = 0;
    pfVar5 = (float *)((int)local_70 + uVar6 * -2);
    do {
      fVar14 = DAT_08002050;
      fVar15 = *(float *)(param_1 + 0x2c);
      if (((int)((uint)(*(float *)(param_1 + 0x2c) < DAT_0800204c) << 0x1f) < 0) &&
         (*(float *)(param_1 + 0x2c) = DAT_08002050, fVar15 = fVar14,
         *(char *)(param_1 + 0x1b3) != '\0')) {
        fVar15 = 1.0;
        *(undefined4 *)(param_1 + 0x2c) = 0x3f800000;
      }
      fVar14 = DAT_08002058;
      uVar6 = uVar18 >> 1;
      puVar12 = (undefined4 *)(local_70[2] + uVar6 * 4);
      fVar17 = *(float *)(param_1 + 0x6c) + (fVar15 - *(float *)(param_1 + 0x6c)) * DAT_08002054;
      uVar18 = uVar18 + 2;
      puVar10 = (undefined4 *)(local_70[1] + uVar6 * 4);
      *(float *)(param_1 + 0x6c) = fVar17;
      fVar15 = (float)libm_sinf((1.0 - fVar17) * fVar14);
      fVar14 = (float)libm_sinf(fVar17 * fVar14);
      uVar19 = DaisySP_Tone_Process(fVar14 * *pfVar5 + *param_2 * fVar15,param_1 + 0x688);
      fVar17 = pfVar5[1];
      *puVar12 = uVar19;
      uVar19 = DaisySP_Tone_Process(fVar14 * fVar17 + param_2[1] * fVar15,param_1 + 0x6a4);
      *puVar10 = uVar19;
      uVar19 = DaisySP_CrossFade_Process(param_1 + 0x6d0,puVar12,puVar10);
      *puVar12 = uVar19;
      uVar19 = DaisySP_CrossFade_Process(param_1 + 0x6d0,puVar10,puVar12);
      *puVar10 = uVar19;
      param_2 = param_2 + 2;
      pfVar5 = pfVar5 + 2;
    } while (uVar18 < local_64);
    uVar18 = 0;
    do {
      uVar6 = uVar18 >> 1;
      uVar18 = uVar18 + 2;
      uVar19 = *(undefined4 *)(local_70[1] + uVar6 * 4);
      *(undefined4 *)local_70[0] = *(undefined4 *)(local_70[2] + uVar6 * 4);
      *(undefined4 *)((int)local_70[0] + 4) = uVar19;
      local_70[0] = (undefined *)((int)local_70[0] + 8);
    } while (uVar18 < local_64);
  }
  return;
}


