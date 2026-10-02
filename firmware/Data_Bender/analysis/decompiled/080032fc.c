/* 080032fc DB_AudioCallback; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DB_AudioCallback(undefined4 param_1,int param_2,uint param_3)

{
  char cVar1;
  ulonglong uVar2;
  char *pcVar3;
  uint uVar4;
  int *piVar5;
  char *pcVar6;
  int *piVar7;
  int *piVar8;
  undefined4 uVar9;
  int iVar10;
  int iVar11;
  undefined uVar12;
  int iVar13;
  int iVar14;
  uint uVar15;
  float fVar16;
  float fVar17;
  float fVar18;
  float fVar19;
  float fVar20;
  int iVar21;
  int iVar22;
  float fVar23;
  
  pcVar6 = DAT_080036d4;
  uVar9 = System_GetTick();
  *(undefined4 *)(pcVar6 + 8) = uVar9;
  iVar10 = System_GetNow();
  iVar11 = GateIn_Trig(DAT_08003684);
  if (iVar11 != 0) {
    DB_Clock_CaptureEdgeAndMedian3();
  }
  pcVar3 = DAT_08003688;
  if (*DAT_08003688 != '\0') {
    System_GetTick();
    *pcVar3 = '\0';
  }
  piVar7 = DAT_080036d8;
  DB_Controls_Poll(DAT_0800368c);
  iVar11 = DAT_08003860;
  iVar22 = DAT_08003690;
  if (*piVar7 == 1) {
    fVar20 = (float)piVar7[0x12];
    iVar22 = 1;
    if (fVar20 != 1.0 && fVar20 < 1.0 == NAN(fVar20)) {
      fVar18 = (float)(ulonglong)((uint)(0.0 < fVar20) * (int)fVar20);
      iVar22 = (uint)(0.0 < fVar18) * (int)fVar18;
    }
    uVar15 = piVar7[0xc];
    uVar2 = (ulonglong)DAT_0800385c;
    *(int *)(DAT_08003860 + 0x1bc) = iVar22;
    iVar22 = DAT_08003874;
    fVar18 = (float)(ulonglong)(uint)(iVar10 - *DAT_08003864);
    fVar20 = (float)(longlong)(int)(uint)(uVar2 * uVar15 >> 0x26) * fVar20 * 4.0;
    if (fVar18 != fVar20 && fVar18 < fVar20 == (NAN(fVar18) || NAN(fVar20))) {
      *DAT_08003868 = 1;
      *(undefined *)(iVar22 + 0x210) = 1;
      pcVar3 = DAT_080036dc;
      goto joined_r0x080037da;
    }
    uVar12 = *DAT_08003868;
  }
  else {
    uVar12 = 0;
    *(undefined4 *)(DAT_08003690 + 0x1bc) = 1;
    *DAT_08003694 = 0;
    iVar11 = iVar22;
  }
  iVar22 = DAT_0800368c;
  *(undefined *)(DAT_0800368c + 0x210) = uVar12;
  pcVar3 = DAT_080036dc;
joined_r0x080037da:
  DAT_080036dc = pcVar3;
  if (param_3 == 0) {
    uVar12 = *DAT_0800386c;
    *(undefined *)(iVar11 + 0x7c) = *(undefined *)(iVar22 + 0x228);
    *(undefined *)(iVar11 + 0x94) = uVar12;
    *(undefined *)(iVar11 + 0x1a8) = uVar12;
    DB_Engine_MapControlsAndProcess(DAT_08003860,param_1,param_2,0);
  }
  else {
    uVar15 = 0;
    do {
      while( true ) {
        fVar23 = DAT_080036b0;
        fVar18 = DAT_080036ac;
        fVar20 = DAT_080036a8;
        fVar19 = (float)(ulonglong)(uint)piVar7[0xc] * DAT_080036a8;
        fVar16 = (float)piVar7[0x10];
        fVar17 = (float)piVar7[0x1a] * DAT_080036a4 + fVar19 * DAT_080036ac;
        piVar7[0x19] = (int)DAT_080036a4;
        piVar7[0x18] = (int)fVar18;
        uVar9 = FPMaxNum((float)piVar7[0xe] + (float)piVar7[0xf],fVar23);
        fVar23 = (float)FPMinNum(uVar9,0x3f800000);
        fVar19 = fVar19 - fVar17;
        piVar7[0x1a] = (int)fVar17;
        fVar18 = ABS(fVar19);
        piVar7[0x1b] = (int)fVar19;
        *pcVar3 = fVar18 != 5.0 && fVar18 < 5.0 == NAN(fVar18);
        if (fVar23 != fVar16) {
          piVar7[0x10] = (int)fVar23;
          fVar16 = DAT_080036b4;
          fVar18 = DAT_080036b0;
          fVar17 = (float)piVar7[0x11] + fVar20;
          if ((fVar23 != fVar17 && fVar23 < fVar17 == (NAN(fVar23) || NAN(fVar17))) ||
             ((int)((uint)(fVar23 < (float)piVar7[0x11] - fVar20) << 0x1f) < 0)) {
            piVar7[0x11] = (int)fVar23;
            fVar20 = (float)libm_expf(fVar18 + fVar23 * fVar16);
            iVar10 = DAT_080036bc;
            uVar9 = FPMaxNum(fVar23 * DAT_080036b8,fVar18);
            fVar18 = (float)FPMinNum(uVar9,0x41000000);
            piVar7[0xd] = (int)(fVar20 * DAT_080036c0);
            piVar7[0x12] = *(int *)(iVar10 + (int)fVar18 * 4);
          }
        }
        fVar20 = DAT_08003698;
        if (*piVar7 == 0) break;
        iVar10 = System_GetTick();
        if (*(char *)(piVar7 + 0x17) == '\0') {
          fVar20 = (float)(ulonglong)(uint)piVar7[0x13] * (1.0 / (float)piVar7[0x12]);
          piVar7[0xc] = (uint)(0.0 < fVar20) * (int)fVar20;
LAB_080033b8:
          System_GetNow();
        }
        else {
          fVar20 = (float)piVar7[0x12];
          *(undefined *)(piVar7 + 0x17) = 0;
          if (fVar20 == 1.0) {
            piVar7[10] = iVar10;
            piVar7[0xc] = piVar7[0x13];
            iVar10 = System_GetNow();
            piVar7[9] = iVar10 - piVar7[8];
            iVar10 = System_GetNow();
            piVar7[8] = iVar10;
            goto LAB_08003564;
          }
          fVar18 = 1.0 / fVar20;
          iVar21 = (uint)(0.0 < (float)(ulonglong)(uint)piVar7[0x13] * fVar18) *
                   (int)((float)(ulonglong)(uint)piVar7[0x13] * fVar18);
          piVar7[0xc] = iVar21;
          if ((int)((uint)(fVar20 < 1.0) << 0x1f) < 0) {
            if ((uint)(0.0 < fVar18) * (int)fVar18 <= piVar7[0xb] + 1U) {
              piVar7[0xb] = 0;
              piVar7[10] = iVar21 + 1 +
                           (uint)((ulonglong)DAT_08003870 * (ulonglong)(uint)(iVar10 - piVar7[10])
                                 >> 0x26);
              iVar10 = System_GetNow();
              goto LAB_0800352c;
            }
            piVar7[0xb] = piVar7[0xb] + 1U;
          }
          iVar10 = System_GetNow();
          if (fVar20 < 1.0 == NAN(fVar20)) goto LAB_0800352c;
        }
        *(float *)(iVar11 + 0xec) = 1.0 / ((float)(ulonglong)(uint)piVar7[0xc] * DAT_080036a0);
LAB_080033d8:
        uVar15 = uVar15 + 2;
        if (param_3 <= uVar15) goto LAB_080035c6;
      }
      fVar18 = (float)piVar7[0xd];
      piVar7[2] = (int)fVar18;
      fVar23 = (fVar18 * fVar20) / (float)piVar7[4];
      fVar18 = (1.0 / fVar18) * DAT_0800369c;
      piVar7[0xc] = (uint)(0.0 < fVar18) * (int)fVar18;
      fVar18 = fVar23 + (float)piVar7[3];
      piVar7[5] = (int)fVar23;
      piVar7[3] = (int)fVar18;
      if (fVar18 < fVar20 != (NAN(fVar18) || NAN(fVar20))) goto LAB_080033b8;
      piVar7[3] = (int)(fVar18 - fVar20);
      iVar10 = System_GetTick();
      uVar4 = DAT_080036c4;
      iVar21 = piVar7[6];
      piVar7[6] = iVar10;
      piVar7[7] = (uint)((ulonglong)uVar4 * (ulonglong)(uint)(iVar10 - iVar21) >> 0x26);
      iVar10 = System_GetNow();
LAB_0800352c:
      fVar20 = (float)piVar7[0x12];
      piVar7[9] = iVar10 - piVar7[8];
      if (fVar20 != 1.0 && fVar20 < 1.0 == NAN(fVar20)) {
        fVar20 = (float)(ulonglong)(uint)(iVar10 - piVar7[8]) * (1.0 / fVar20);
        piVar7[9] = (uint)(0.0 < fVar20) * (int)fVar20;
      }
      piVar7[8] = iVar10;
LAB_08003564:
      iVar10 = System_GetTick();
      piVar8 = DAT_080036e0;
      piVar5 = DAT_080036cc;
      fVar20 = DAT_080036c8;
      iVar14 = *DAT_080036e0;
      *DAT_080036e0 = iVar10;
      piVar8[1] = iVar14;
      piVar8 = DAT_080036e4;
      iVar13 = *piVar5;
      iVar21 = *DAT_080036e4;
      piVar5[1] = iVar13;
      piVar8[1] = iVar21;
      cVar1 = *pcVar3;
      iVar10 = (int)((float)(ulonglong)(uint)(iVar10 - iVar14) / fVar20);
      *piVar5 = iVar10;
      *piVar8 = iVar13 - iVar10;
      if (cVar1 != '\0') goto LAB_080033d8;
      uVar15 = uVar15 + 2;
      *(undefined4 *)(iVar11 + 0x90) = 0;
      *(undefined *)(iVar11 + 0x74) = 1;
    } while (uVar15 < param_3);
LAB_080035c6:
    cVar1 = *pcVar3;
    uVar12 = *(undefined *)(iVar22 + 0x228);
    *(char *)(iVar11 + 0x94) = cVar1;
    *(undefined *)(iVar11 + 0x7c) = uVar12;
    *(char *)(iVar11 + 0x1a8) = cVar1;
    DB_Engine_MapControlsAndProcess(DAT_08003690,param_1,param_2,param_3);
    if (*DAT_080036d0 != '\0') {
      iVar10 = param_2;
      do {
        iVar11 = iVar10 + 8;
        *(float *)(iVar10 + 4) = -*(float *)(iVar10 + 4);
        iVar10 = iVar11;
      } while (param_2 + 8 + (param_3 - 1 >> 1) * 8 != iVar11);
    }
  }
  iVar10 = System_GetTick();
  fVar20 = (float)(ulonglong)(uint)(iVar10 - *(int *)(pcVar6 + 8)) * *(float *)(pcVar6 + 4);
  if (*pcVar6 != '\0') {
    *(float *)(pcVar6 + 0x14) = fVar20;
    *(float *)(pcVar6 + 0xc) = fVar20;
    *(float *)(pcVar6 + 0x10) = fVar20;
    *pcVar6 = '\0';
    return;
  }
  fVar18 = *(float *)(pcVar6 + 0x10);
  if (fVar20 != fVar18 && fVar20 < fVar18 == (NAN(fVar20) || NAN(fVar18))) {
    *(float *)(pcVar6 + 0x10) = fVar20;
  }
  if ((int)((uint)(fVar20 < *(float *)(pcVar6 + 0xc)) << 0x1f) < 0) {
    *(float *)(pcVar6 + 0xc) = fVar20;
  }
  *(float *)(pcVar6 + 0x14) =
       (1.0 - *(float *)(pcVar6 + 0x18)) * *(float *)(pcVar6 + 0x14) +
       fVar20 * *(float *)(pcVar6 + 0x18);
  return;
}


