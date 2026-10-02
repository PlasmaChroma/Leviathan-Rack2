/* 08001868 DB_Corrupt_ProcessInterleaved; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DB_Corrupt_ProcessInterleaved(int *param_1,float *param_2,float *param_3,int param_4)

{
  char cVar1;
  uint uVar2;
  float *pfVar3;
  float *pfVar4;
  int iVar5;
  uint uVar6;
  undefined unaff_r8;
  bool bVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  undefined4 uVar11;
  float fVar12;
  float fVar13;
  float fVar14;
  float fVar15;
  undefined4 uVar16;
  float fVar17;
  
  fVar8 = DAT_08001c70;
  fVar13 = DAT_08001c68;
  iVar5 = *param_1;
  if (*(char *)(param_1 + 3) == '\0') {
    if (iVar5 == 0) {
      libc_memcpy(param_3,param_2,param_4 << 2);
      return;
    }
  }
  else if (iVar5 == 0) {
    iVar5 = param_1[2];
    fVar17 = (float)param_1[7];
    goto LAB_08001894;
  }
  fVar17 = (float)param_1[4];
LAB_08001894:
  switch(iVar5) {
  case 1:
    fVar17 = fVar17 + fVar17;
    fVar13 = (float)libm_fmodf(fVar17);
    fVar13 = (float)FPRoundInt(fVar13 * 16.0,0x20,4,0);
    fVar9 = *(float *)(DAT_08001e40 + (int)fVar13 * 4);
    fVar8 = -fVar9 * 16.0 + 2.0;
    pfVar3 = (float *)(DAT_08001e44 + (int)fVar13 * 4);
    param_1[0xc] = (int)fVar9;
    param_1[0x16] = (int)fVar9;
    uVar6 = (uint)(0.0 < (float)(longlong)(int)(uint)*(byte *)(param_1 + 10) * fVar9) *
            (int)((float)(longlong)(int)(uint)*(byte *)(param_1 + 10) * fVar9);
    param_1[0xd] = uVar6;
    param_1[0x13] = (int)((float)(ulonglong)uVar6 + fVar8);
    fVar9 = (float)(longlong)(int)(uint)*(byte *)(param_1 + 0x14) * fVar9;
    uVar6 = (uint)(0.0 < fVar9) * (int)fVar9;
    param_1[0x17] = uVar6;
    fVar13 = (float)((uint)(fVar17 != 1.0) * 0x3f800000 + (uint)(fVar17 == 1.0) * 0x3e800000) *
             *pfVar3;
    param_1[0x1d] = (int)((float)(ulonglong)uVar6 + fVar8);
    param_1[0xb] = (int)fVar13;
    param_1[0x15] = (int)fVar13;
    if (param_4 != 0) {
      uVar6 = 0;
      do {
        fVar13 = (float)DaisySP_Decimator_Process(*param_2,param_1 + 10);
        *param_3 = fVar13;
        fVar13 = (float)DaisySP_Decimator_Process(param_2[1],param_1 + 0x14);
        bVar7 = param_4 - 1U >> 1 != uVar6;
        param_3[1] = fVar13;
        uVar6 = uVar6 + 1;
        param_2 = param_2 + 2;
        param_3 = param_3 + 2;
      } while (bVar7);
      return;
    }
    break;
  case 2:
    if (fVar17 == DAT_08001ca0 || fVar17 < DAT_08001ca0 != (NAN(fVar17) || NAN(DAT_08001ca0))) {
      *(undefined *)(param_1 + 6) = 1;
    }
    else {
      uVar2 = newlib_rand_LCG64();
      uVar6 = uVar2 & 0x3ff;
      if ((int)uVar2 < 1) {
        uVar6 = -(uVar2 * -0x400000 >> 0x16);
      }
      if ((int)((uint)((float)(longlong)(int)uVar6 < fVar17 * fVar17 * DAT_08001e48) << 0x1f) < 0) {
        *(byte *)(param_1 + 6) = *(byte *)(param_1 + 6) ^ 1;
      }
    }
    if (param_4 != 0) {
      cVar1 = *(char *)(param_1 + 6);
      iVar5 = 4;
      pfVar3 = param_3 + 1;
      do {
        fVar13 = 0.0;
        if (cVar1 == '\0') {
          pfVar3[-1] = 0.0;
        }
        else {
          pfVar3[-1] = *param_2;
          fVar13 = param_2[1];
        }
        iVar5 = iVar5 + 8;
        param_2 = param_2 + 2;
        *pfVar3 = fVar13;
        pfVar3 = pfVar3 + 2;
      } while ((param_4 - 1U >> 1) * 8 + 0xc != iVar5);
    }
    break;
  case 3:
    fVar8 = fVar17 * fVar17 + fVar17 * fVar17;
    uVar11 = FPMaxNum(fVar8,DAT_08001c68);
    fVar13 = (float)FPMinNum(uVar11,0x3f800000);
    fVar9 = fVar13 * DAT_08001c8c + 1.0;
    uVar11 = FPMaxNum(fVar8 - 1.0,DAT_08001c68);
    fVar13 = (float)FPMinNum(uVar11,0x3f800000);
    fVar13 = fVar13 * 8.0 + 1.0;
    if ((int)((uint)(fVar9 < 2.0) << 0x1f) < 0) {
      fVar9 = 2.0;
    }
    if (param_4 != 0) {
      iVar5 = (uint)(fVar17 < DAT_08001c90) << 0x1f;
      pfVar3 = param_2 + (param_4 - 1U & 0xfffffffe) + 2;
      fVar8 = DAT_08001c68;
      if (-1 < iVar5) {
        fVar8 = 1.0;
      }
      if (iVar5 < 0) {
        unaff_r8 = 1;
      }
      if (-1 < iVar5) {
        unaff_r8 = 0;
      }
      do {
        fVar10 = *param_2;
        if (fVar10 == 0.0 || fVar10 < 0.0 != NAN(fVar10)) {
          fVar10 = (float)libm_expf(fVar10 * fVar9);
          fVar10 = fVar10 - 1.0;
          fVar12 = param_2[1];
          if (fVar12 == 0.0 || fVar12 < 0.0 != NAN(fVar12)) goto LAB_08001c4e;
LAB_08001b72:
          fVar12 = (float)libm_expf(-(fVar12 * fVar9));
          fVar12 = 1.0 - fVar12;
        }
        else {
          fVar10 = (float)libm_expf(-(fVar10 * fVar9));
          fVar10 = 1.0 - fVar10;
          fVar12 = param_2[1];
          if (fVar12 != 0.0 && fVar12 < 0.0 == NAN(fVar12)) goto LAB_08001b72;
LAB_08001c4e:
          fVar12 = (float)libm_expf(fVar12 * fVar9);
          fVar12 = fVar12 - 1.0;
        }
        uVar16 = FPMaxNum(fVar13 * fVar12,DAT_08001c94);
        uVar11 = FPMaxNum(fVar13 * fVar10,DAT_08001c94);
        fVar12 = (float)FPMinNum(uVar16,DAT_08001c98);
        fVar10 = (float)FPMinNum(uVar11,DAT_08001c98);
        if (fVar9 != 1.0 && fVar9 < 1.0 == NAN(fVar9)) {
          fVar14 = (float)libm_sinf(fVar17 * DAT_08001c9c);
          fVar14 = -fVar14 * 0.375 + 0.5;
          fVar10 = fVar10 * fVar14;
          fVar12 = fVar12 * fVar14;
        }
        fVar14 = DAT_08001c70;
        pfVar4 = param_2 + 2;
        *(undefined *)(param_1 + 8) = unaff_r8;
        fVar14 = (float)param_1[9] + (fVar8 - (float)param_1[9]) * fVar14;
        param_1[9] = (int)fVar14;
        *param_3 = fVar14 * fVar10 + *param_2 * (1.0 - fVar14);
        param_3[1] = (float)param_1[9] * fVar12 + param_2[1] * (1.0 - (float)param_1[9]);
        param_2 = pfVar4;
        param_3 = param_3 + 2;
      } while (pfVar3 != pfVar4);
    }
    break;
  case 4:
    param_1[0x8f] = (int)fVar17;
    uVar11 = FPMaxNum(fVar17 + fVar17,fVar13);
    fVar8 = (float)FPMinNum(uVar11,0x3f800000);
    uVar11 = libm_expf((float)param_1[0x8b] + fVar8 * ((float)param_1[0x8c] - (float)param_1[0x8b]))
    ;
    DaisySP_Svf_SetFreq(param_1 + 0x3f);
    DaisySP_Svf_SetFreq(uVar11,param_1 + 0x52);
    uVar11 = FPMaxNum((float)param_1[0x8f] * 2.0 + -1.0,fVar13);
    fVar13 = (float)FPMinNum(uVar11,0x3f800000);
    uVar11 = libm_expf((float)param_1[0x8d] + fVar13 * ((float)param_1[0x8e] - (float)param_1[0x8d])
                      );
    DaisySP_Svf_SetFreq(param_1 + 0x65);
    DaisySP_Svf_SetFreq(uVar11,param_1 + 0x78);
    if (param_4 != 0) {
      uVar6 = 0;
      do {
        DaisySP_Svf_Process(*param_2,param_1 + 0x3f);
        pfVar3 = param_2 + 1;
        param_2 = param_2 + 2;
        DaisySP_Svf_Process(*pfVar3,param_1 + 0x52);
        iVar5 = param_1[0x5e];
        DaisySP_Svf_Process(param_1[0x4b],param_1 + 0x65);
        DaisySP_Svf_Process(iVar5,param_1 + 0x78);
        fVar13 = (float)libm_tanhf(param_1[0x72]);
        fVar8 = (float)libm_tanhf(param_1[0x85]);
        param_3[1] = fVar8;
        bVar7 = param_4 - 1U >> 1 != uVar6;
        *param_3 = fVar13;
        uVar6 = uVar6 + 1;
        param_3 = param_3 + 2;
      } while (bVar7);
    }
    break;
  case 5:
    bVar7 = fVar17 < DAT_08001c70;
    *(bool *)(param_1 + 0xeb) =
         fVar17 != DAT_08001c6c && fVar17 < DAT_08001c6c == (NAN(fVar17) || NAN(DAT_08001c6c));
    fVar13 = DAT_08001c74;
    if ((int)((uint)bVar7 << 0x1f) < 0) {
      fVar17 = fVar8;
    }
    fVar8 = (float)libm_expf(fVar17);
    fVar15 = (float)((double)(fVar8 - 1.0) / DAT_08001c60) * 1.25;
    fVar14 = fVar15 * DAT_08001c78 + 0.5;
    fVar8 = DAT_08001c6c + fVar15 * DAT_08001c7c;
    fVar12 = fVar15 * 15.0;
    fVar10 = fVar15 * DAT_08001c84;
    fVar9 = fVar15 * DAT_08001c80;
    fVar17 = fVar15 * DAT_08001ca0;
    param_1[0xab] = (int)fVar12;
    param_1[0xac] = (int)fVar14;
    param_1[0xb7] = (int)fVar14;
    param_1[0xc2] = (int)fVar14;
    param_1[0xb6] = (int)fVar12;
    param_1[0xc1] = (int)fVar12;
    param_1[0x95] = (int)fVar10;
    param_1[0x96] = (int)fVar8;
    param_1[0xa1] = (int)fVar8;
    param_1[0xa0] = (int)fVar10;
    param_1[0xd5] = (int)fVar17;
    param_1[0xdd] = (int)fVar9;
    param_1[0xe2] = (int)(fVar15 * fVar13);
    DaisySP_ATone_CalculateCoefficients(param_1 + 0xdf);
    param_1[0xe8] = (int)(fVar15 * fVar13);
    DaisySP_ATone_CalculateCoefficients(param_1 + 0xe5);
    param_1[0x91] = (int)(-fVar15 * DAT_08001c88 + 1.0);
    if (param_4 != 0) {
      uVar6 = 0;
      do {
        DB_Vinyl_ProcessStereo(*param_2,param_1 + 0x90,param_3,param_3 + 1);
        bVar7 = uVar6 != param_4 - 1U >> 1;
        param_2 = param_2 + 2;
        param_3 = param_3 + 2;
        uVar6 = uVar6 + 1;
      } while (bVar7);
      return;
    }
  }
  return;
}


