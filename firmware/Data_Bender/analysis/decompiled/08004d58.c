/* 08004d58 DB_Buffer_ProcessBlock; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DB_Buffer_ProcessBlock(int param_1,undefined4 *param_2,undefined4 *param_3,uint param_4)

{
  bool bVar1;
  char cVar2;
  int iVar3;
  float fVar4;
  uint uVar5;
  uint uVar6;
  undefined4 uVar7;
  float *pfVar8;
  undefined4 *puVar9;
  int iVar10;
  int iVar11;
  undefined4 **ppuVar12;
  undefined4 **ppuVar13;
  undefined *puVar14;
  float fVar15;
  float fVar16;
  float fVar17;
  float fVar19;
  float fVar20;
  double dVar18;
  float fVar21;
  float fVar22;
  float fVar23;
  float fVar24;
  undefined4 *puVar25;
  undefined4 *local_68;
  int local_64;
  uint local_60;
  undefined4 **local_5c;
  uint local_58;
  int local_54;
  uint local_50;
  undefined4 **local_4c;
  uint local_48;
  undefined4 **local_44;
  char local_39 [5];
  
  ppuVar13 = &local_68 + param_4 * -2;
  local_5c = ppuVar13 + param_4 * -2;
  local_68 = param_3;
  local_50 = param_4;
  if (param_4 == 0) {
    local_60 = 0;
  }
  else {
    uVar5 = 0;
    local_60 = param_4 & 0x3fffffff;
    do {
      uVar6 = uVar5 >> 1;
      uVar5 = uVar5 + 2;
      puVar9 = (undefined4 *)param_2[1];
      ppuVar13[uVar6] = (undefined4 *)*param_2;
      ppuVar13[(param_4 & 0x3fffffff) + uVar6] = puVar9;
      param_2 = param_2 + 2;
    } while (uVar5 < param_4);
  }
  if (*(char *)(param_1 + 0x110) == '\0') {
    *(undefined4 *)(param_1 + 0x114) = 0;
    *(undefined4 *)(param_1 + 0x58) = *(undefined4 *)(param_1 + 0x50);
    if (*(char *)(param_1 + 0x119) != '\0') {
      *(undefined *)(param_1 + 0x119) = 0;
      *(undefined2 *)(param_1 + 0x234) = 0x101;
    }
  }
  else {
    fVar15 = *(float *)(param_1 + 0x50);
    fVar19 = *(float *)(param_1 + 0x58);
    *(undefined2 *)(param_1 + 0x118) = 0;
    uVar7 = 1;
    *(undefined2 *)(param_1 + 0x236) = 0x101;
    *(undefined *)(param_1 + 0x111) = 1;
    if (fVar15 == fVar19 || fVar15 < fVar19 != (NAN(fVar15) || NAN(fVar19))) {
      uVar7 = 0xffffffff;
    }
    *(undefined4 *)(param_1 + 0x114) = uVar7;
  }
  puVar14 = (undefined *)(param_1 + 0x22a);
  local_58 = local_50 >> 1;
  pfVar8 = (float *)(param_1 + 0x70);
  iVar11 = 0;
  local_54 = local_60 << 2;
  local_64 = local_58 * -4;
  local_44 = local_5c + local_58;
  iVar10 = param_1;
  local_4c = ppuVar13;
  do {
    if (local_58 != 0) {
      ppuVar12 = (undefined4 **)((int)local_44 + local_64);
      ppuVar13 = local_4c;
      do {
        fVar15 = *(float *)(param_1 + 0x50);
        if (iVar11 == 0) {
          fVar15 = fVar15 + (*(float *)(param_1 + 0x54) - fVar15) * DAT_08005378;
          fVar19 = 1.0 / fVar15;
          *(float *)(param_1 + 0x50) = fVar15;
          *(float *)(param_1 + 0x5c) = fVar19;
        }
        else {
          fVar19 = *(float *)(param_1 + 0x5c);
        }
        fVar24 = *(float *)(param_1 + 0x4c);
        fVar15 = fVar24 / fVar15;
        fVar16 = pfVar8[0x33];
        fVar4 = (float)(*(uint *)(param_1 + 8) >> 1);
        fVar20 = (float)(ulonglong)((int)pfVar8[0x3d] - 1);
        fVar15 = (float)((uint)(0.0 < fVar15) * (int)fVar15);
        if ((int)fVar4 - 1U < (uint)fVar15) {
          fVar15 = fVar4;
        }
        if ((fVar16 == fVar20 || fVar16 < fVar20 != (NAN(fVar16) || NAN(fVar20))) &&
           (-1 < (int)((uint)(fVar16 < (float)(ulonglong)(uint)pfVar8[0x3b]) << 0x1f))) {
          fVar4 = pfVar8[0x22];
LAB_08004e98:
          if (puVar14[0xc] == '\0') goto LAB_0800535a;
LAB_08004ea2:
          fVar16 = pfVar8[0x60];
          if (((fVar16 < 0.0 == NAN(fVar16)) && (pfVar8[0x62] <= 0.0)) ||
             ((fVar16 <= 0.0 && (pfVar8[0x62] < 0.0 == NAN(pfVar8[0x62]))))) {
            pfVar8[0x39] = fVar15;
          }
          *puVar14 = 0;
        }
        else {
          fVar4 = pfVar8[0x22];
          if (fVar4 == 0.0 || fVar4 < 0.0 != NAN(fVar4)) {
            pfVar8[0x33] = (float)(ulonglong)((int)pfVar8[0x3f] - 1);
            goto LAB_08004e98;
          }
          cVar2 = puVar14[0xc];
          pfVar8[0x33] = (float)(ulonglong)(uint)pfVar8[0x3b];
          if (cVar2 != '\0') goto LAB_08004ea2;
LAB_0800535a:
          pfVar8[0x39] = fVar15;
        }
        fVar24 = fVar24 * fVar19;
        uVar7 = FPMaxNum(pfVar8[7] + *(float *)(param_1 + 0x88),*(undefined4 *)(param_1 + 0x9c));
        fVar19 = (float)FPMinNum(uVar7,*(undefined4 *)(param_1 + 0xa0));
        fVar15 = fVar24 - (float)(ulonglong)(uint)pfVar8[0x37];
        fVar16 = (float)(ulonglong)(uint)pfVar8[0x45];
        fVar19 = (1.0 - fVar19) * (float)(ulonglong)(uint)pfVar8[0x37];
        pfVar8[0x47] = (float)((uint)(0.0 < fVar19) * (int)fVar19);
        if (fVar16 != fVar15 && fVar16 < fVar15 == (NAN(fVar16) || NAN(fVar15))) {
          pfVar8[0x45] = 0.0;
        }
        local_39[0] = '\0';
        if (fVar4 == 0.0 || fVar4 < 0.0 != NAN(fVar4)) {
          puVar9 = (undefined4 *)DB_Buffer_ReadLinear(param_1,iVar11,local_39);
          puVar25 = *ppuVar13;
          DB_Buffer_WriteSample(puVar25,param_1,iVar11,param_1 + 0x238);
        }
        else {
          puVar25 = *ppuVar13;
          DB_Buffer_WriteSample(puVar25,param_1,iVar11,param_1 + 0x238);
          puVar9 = (undefined4 *)DB_Buffer_ReadLinear(param_1,iVar11,local_39);
        }
        fVar15 = pfVar8[0x39];
        uVar6 = *(uint *)(iVar10 + 0x1cc);
        uVar5 = *(uint *)(iVar10 + 0x1c0);
        if ((uint)((int)fVar15 << 1) <= *(uint *)(iVar10 + 0x1c0)) {
          uVar5 = (int)fVar15 << 1;
        }
        *(uint *)(iVar10 + 0x1c4) = uVar5;
        if ((uVar6 <= *(int *)(iVar10 + 0x1c8) - 1U) && (uVar5 < uVar6)) {
          uVar5 = uVar6;
        }
        *(undefined4 **)(*(int *)(iVar10 + 0x1bc) + uVar5 * 4) = puVar25;
        *(uint *)(iVar10 + 0x1cc) = uVar5 + 1;
        fVar4 = pfVar8[0x47];
        *(float *)(iVar10 + 0x200) = fVar4;
        fVar20 = *(float *)(param_1 + 0x10c);
        fVar19 = (float)(ulonglong)(uint)fVar4 * fVar20 * 0.5;
        dVar18 = (double)FPMaxNum((double)(ulonglong)((uint)(0.0 < fVar19) * (int)fVar19),
                                  0x4038000000000000);
        fVar21 = (float)((uint)(0.0 < dVar18) * (int)(longlong)dVar18);
        *(float *)(iVar10 + 0x204) = fVar21;
        fVar17 = pfVar8[0x33];
        fVar16 = (float)(ulonglong)(uint)pfVar8[0x3b];
        fVar22 = (float)((uint)(0.0 < fVar17 - fVar16) * (int)(fVar17 - fVar16));
        *(float *)(iVar10 + 0x208) = fVar22;
        fVar19 = DAT_08005360;
        fVar23 = (float)(ulonglong)((int)pfVar8[0x3f] - 1);
        puVar25 = DAT_08005390;
        if ((fVar17 == fVar23 || fVar17 < fVar23 != (NAN(fVar17) || NAN(fVar23))) &&
           (puVar25 = puVar9, (int)((uint)(fVar17 < fVar16) << 0x1f) < 0)) {
          puVar25 = DAT_08005390;
        }
        *ppuVar12 = puVar25;
        if (fVar20 != fVar19 && fVar20 < fVar19 == (NAN(fVar20) || NAN(fVar19))) {
          puVar9 = puVar25;
          if (((uint)fVar22 < (uint)fVar21) ||
             ((uint)((int)fVar4 - (int)fVar21) <= (uint)fVar22 &&
              (int)fVar22 - ((int)fVar4 - (int)fVar21) != 0)) {
            if ((uint)fVar22 < (uint)fVar4 >> 1) {
              fVar19 = (float)(longlong)(int)fVar22 / (float)(ulonglong)(uint)fVar21;
              *(float *)(iVar10 + 0x20c) = fVar19;
              puVar9 = (undefined4 *)((float)puVar25 * fVar19);
            }
            else {
              fVar19 = (float)(ulonglong)(uint)((int)fVar4 - (int)fVar22) /
                       (float)(ulonglong)(uint)fVar21;
              if (fVar19 == 1.0 || fVar19 < 1.0 != NAN(fVar19)) {
                puVar9 = (undefined4 *)((float)puVar25 * fVar19);
                *(float *)(iVar10 + 0x20c) = fVar19;
              }
              else {
                *(undefined4 *)(iVar10 + 0x20c) = 0x3f800000;
              }
            }
          }
          else if ((uint)fVar22 < (uint)fVar4) {
            *(undefined4 *)(iVar10 + 0x20c) = 0x3f800000;
          }
          else {
            puVar9 = (undefined4 *)((float)puVar25 * DAT_08005728);
            *(float *)(iVar10 + 0x20c) = DAT_08005728;
          }
          fVar19 = pfVar8[0x35];
          if ((int)((uint)(fVar19 < DAT_0800537c) << 0x1f) < 0) {
            puVar9 = (undefined4 *)((float)puVar9 * (fVar19 / DAT_0800537c));
          }
          else {
            fVar4 = (float)(ulonglong)(uint)fVar15 - DAT_0800538c;
            if (fVar19 != fVar4 && fVar19 < fVar4 == (NAN(fVar19) || NAN(fVar4))) {
              uVar7 = FPMaxNum(((float)(ulonglong)(uint)fVar15 - (fVar19 + DAT_0800537c)) /
                               DAT_0800537c,DAT_08005390);
              fVar19 = (float)FPMinNum(uVar7,0x3f800000);
              puVar9 = (undefined4 *)((float)puVar9 * fVar19);
            }
          }
          *ppuVar12 = puVar9;
        }
        fVar19 = pfVar8[0x60];
        pfVar8[0x60] = (float)puVar25;
        pfVar8[0x62] = fVar19;
        if (local_39[0] != '\0') {
          pfVar8[0x4f] = (float)((int)pfVar8[0x4f] + 1);
        }
        if (puVar14[10] == '\0') {
joined_r0x0800532e:
          if (iVar11 == 0) {
LAB_080051fe:
            uVar5 = *(uint *)(param_1 + 0x1b4);
            puVar9 = DAT_08005390;
            if ((uVar5 < 2) ||
               (puVar9 = (undefined4 *)((float)(ulonglong)(uVar5 - 1) * *(float *)(param_1 + 0xc0)),
               uVar5 < 5)) {
              bVar1 = (float)puVar9 - *(float *)(param_1 + 0x1ec) != 0.0;
              fVar15 = (float)((uint)bVar1 * DAT_08005384 + (uint)!bVar1 * DAT_08005388);
            }
            else {
              bVar1 = (float)puVar9 - *(float *)(param_1 + 0x1ec) != 0.0;
              fVar15 = (float)((uint)bVar1 * DAT_08005370 + (uint)!bVar1 * DAT_08005374);
            }
            *(float *)(param_1 + 0x1ec) = (float)(longlong)(int)(fVar15 + (float)puVar9 + 0.5);
          }
          if (local_39[0] != '\0') {
            fVar15 = pfVar8[0x51];
            uVar5 = (uint)(0.0 < *(float *)(param_1 + 0x1ec)) * (int)*(float *)(param_1 + 0x1ec);
            if (1 < (uint)fVar15) {
              if ((iVar11 == 0) || (*(int *)(param_1 + 0x68) != 1)) {
                local_48 = uVar5;
                iVar3 = newlib_rand_LCG64();
                fVar19 = DAT_08005368;
                fVar15 = (float)(longlong)
                                (iVar3 + (((int)((ulonglong)
                                                 ((longlong)DAT_08005364 * (longlong)iVar3) >> 0x20)
                                           + iVar3 >> 7) - (iVar3 >> 0x1f)) * -0xff) / DAT_08005368;
                if ((fVar15 == 0.75 || fVar15 < 0.75 != NAN(fVar15)) ||
                   (fVar15 = *(float *)(param_1 + 0x108),
                   fVar15 == DAT_0800536c ||
                   fVar15 < DAT_0800536c != (NAN(fVar15) || NAN(DAT_0800536c)))) {
                  fVar19 = pfVar8[0x15];
                  fVar15 = pfVar8[0x51];
                  uVar5 = local_48;
                }
                else {
                  iVar3 = newlib_rand_LCG64();
                  fVar15 = pfVar8[0x51];
                  fVar19 = (float)(longlong)
                                  (iVar3 + (((int)((ulonglong)
                                                   ((longlong)DAT_08005724 * (longlong)iVar3) >>
                                                  0x20) + iVar3 >> 7) - (iVar3 >> 0x1f)) * -0xff) /
                           fVar19;
                  pfVar8[0x15] = fVar19;
                  uVar5 = local_48;
                }
              }
              else {
                *(undefined4 *)(param_1 + 200) = *(undefined4 *)(param_1 + 0xc4);
                fVar19 = pfVar8[0x15];
              }
              uVar5 = (int)((float)(ulonglong)(uint)fVar15 * fVar19) + uVar5;
              if ((int)fVar15 - 1U <= uVar5) {
                uVar5 = (int)fVar15 - 1U;
              }
            }
            fVar19 = (float)((int)pfVar8[0x37] * uVar5);
            fVar15 = (float)((int)pfVar8[0x39] - (int)pfVar8[0x37]);
            if ((uint)fVar19 <= (uint)fVar15) {
              fVar15 = fVar19;
            }
            pfVar8[0x45] = fVar15;
          }
          if (iVar11 != 0) goto LAB_080054f6;
          if (puVar14[0xe] != '\0') {
            DB_MacroBreak_ChooseRepeatSilencePosition(param_1,0);
            goto LAB_08005546;
          }
        }
        else {
          if (*(int *)(param_1 + 0x120) != 0) {
            if ((((float)puVar25 < 0.0 == NAN((float)puVar25)) && (fVar19 <= 0.0)) ||
               (((float)puVar25 <= 0.0 && (fVar19 < 0.0 == NAN(fVar19))))) goto LAB_08005262;
            goto joined_r0x0800532e;
          }
          if (((((float)puVar25 < 0.0 == NAN((float)puVar25)) && (fVar19 <= 0.0)) ||
              (((float)puVar25 <= 0.0 && (fVar19 < 0.0 == NAN(fVar19))))) || (local_39[0] != '\0'))
          {
LAB_08005262:
            cVar2 = puVar14[0xc];
            *(undefined4 *)(param_1 + 0x120) = 0;
            if (cVar2 != '\0') {
              uVar5 = *(uint *)(param_1 + 8);
              fVar4 = *(float *)(param_1 + 0x4c) / *(float *)(param_1 + 0x50);
              pfVar8[0x43] = fVar15;
              fVar19 = (float)(uVar5 >> 1);
              fVar15 = (float)((uint)(0.0 < fVar4) * (int)fVar4);
              if ((int)fVar19 - 1U < (uint)fVar15) {
                fVar15 = fVar19;
              }
              pfVar8[0x39] = fVar15;
              puVar14[0xc] = 0;
            }
            fVar15 = pfVar8[0x22];
            pfVar8[0x35] = 0.0;
            pfVar8[0x33] = (float)((uint)(fVar15 != 0.0) * (int)fVar16 +
                                  (uint)(fVar15 == 0.0) * (int)fVar23);
            *(undefined4 *)(iVar10 + 0x208) = 0;
            pfVar8[0x4f] = 0.0;
            puVar14[10] = 0;
            *puVar14 = 0;
            puVar14[0xe] = 1;
            pfVar8[0x6f] = *(float *)(param_1 + 0x124);
            *(char *)(param_1 + 0x11a) = *(char *)(param_1 + 0x11b);
            if ((*(char *)(param_1 + 0x11b) == '\0') && (*(char *)(param_1 + 0x110) == '\0')) {
              fVar19 = pfVar8[0x39];
              if ((uint)fVar19 >> 2 < (uint)((int)pfVar8[0x2e] - (int)pfVar8[0x4d])) {
                pfVar8[0x4d] = pfVar8[0x2e];
                fVar4 = fVar19;
                if (pfVar8[0x49] != 0.0) {
                  fVar4 = 0.0;
                }
                pfVar8[0x49] = fVar4;
              }
              if (fVar15 == 0.0 || fVar15 < 0.0 != NAN(fVar15)) {
                if (pfVar8[0x49] != 0.0) {
                  fVar19 = 0.0;
                }
                pfVar8[0x4b] = fVar19;
              }
              else {
                pfVar8[0x4b] = pfVar8[0x49];
              }
            }
            goto joined_r0x0800532e;
          }
          if (iVar11 == 0) goto LAB_080051fe;
LAB_080054f6:
          if (*(int *)(param_1 + 0x120) != 0) {
            *(int *)(param_1 + 0x120) = *(int *)(param_1 + 0x120) + -1;
          }
          if (puVar14[0xe] != '\0') {
            DB_MacroBreak_ChooseRepeatSilencePosition(param_1,1);
            if (*(int *)(param_1 + 0x68) == 1) {
              *(undefined4 *)(param_1 + 0xac) = *(undefined4 *)(param_1 + 0xa8);
              *(undefined4 *)(param_1 + 0xe4) = *(undefined4 *)(param_1 + 0xe0);
            }
            else {
LAB_08005546:
              fVar15 = *(float *)(param_1 + 0x104);
              if (fVar15 == DAT_08005380 ||
                  fVar15 < DAT_08005380 != (NAN(fVar15) || NAN(DAT_08005380))) {
                pfVar8[0xe] = 1.0;
                pfVar8[0x1c] = 1.0;
              }
              else {
                DB_MacroBend_ChooseRateAndSlew(param_1,iVar11);
              }
            }
            puVar14[0xe] = 0;
          }
        }
        if ((int)((uint)(pfVar8[0x33] < (float)(ulonglong)(uint)pfVar8[0x3f]) << 0x1f) < 0) {
          uVar7 = FPMaxNum(*(float *)(param_1 + 0x88) + pfVar8[7],*(undefined4 *)(param_1 + 0x9c));
          fVar15 = (float)FPMinNum(uVar7,*(undefined4 *)(param_1 + 0xa0));
          pfVar8[0x5d] = fVar15;
          if (local_39[0] != '\0') goto LAB_080053be;
LAB_08005194:
          fVar15 = pfVar8[0x60];
          if (((fVar15 < 0.0 == NAN(fVar15)) && (pfVar8[0x62] <= 0.0)) ||
             ((fVar15 <= 0.0 && (pfVar8[0x62] < 0.0 == NAN(pfVar8[0x62]))))) goto LAB_080053be;
        }
        else {
          if (local_39[0] == '\0') goto LAB_08005194;
          uVar7 = FPMaxNum(*(float *)(param_1 + 0x88) + pfVar8[7],*(undefined4 *)(param_1 + 0x9c));
          fVar15 = (float)FPMinNum(uVar7,*(undefined4 *)(param_1 + 0xa0));
          pfVar8[0x5d] = fVar15;
LAB_080053be:
          uVar7 = FPMaxNum(*(float *)(param_1 + 0x6c) + *pfVar8,(int)*(undefined8 *)(param_1 + 0x80)
                          );
          uVar7 = FPMinNum(uVar7,(int)((ulonglong)*(undefined8 *)(param_1 + 0x80) >> 0x20));
          fVar15 = (float)libm_powf(0x40000000,uVar7);
          fVar15 = (float)((uint)(0.0 < fVar15) * (int)fVar15);
          fVar19 = pfVar8[0x45];
          pfVar8[0x51] = fVar15;
          pfVar8[0x3b] = fVar19;
          fVar15 = fVar24 / (float)(ulonglong)(uint)fVar15 + 1.0;
          fVar15 = (float)((uint)(0.0 < fVar15) * (int)fVar15);
          if ((uint)pfVar8[0x39] <= (uint)fVar15) {
            fVar15 = pfVar8[0x39];
          }
          pfVar8[0x37] = fVar15;
          pfVar8[0x3d] = (float)((int)fVar15 + (int)fVar19);
          pfVar8[0x3f] = (float)((int)pfVar8[0x47] + (int)fVar19);
        }
        ppuVar12 = ppuVar12 + 1;
        ppuVar13 = ppuVar13 + 1;
        pfVar8[0x2e] = (float)((int)pfVar8[0x2e] + 1);
      } while (local_44 != ppuVar12);
    }
    pfVar8 = pfVar8 + 1;
    puVar14 = puVar14 + 1;
    iVar10 = iVar10 + 0x14;
    local_44 = (undefined4 **)((int)local_44 + local_54);
    local_4c = (undefined4 **)((int)local_4c + local_54);
    if (iVar11 != 0) {
      if (local_50 != 0) {
        uVar5 = 0;
        do {
          uVar6 = uVar5 >> 1;
          uVar5 = uVar5 + 2;
          puVar9 = local_5c[uVar6 + local_60];
          *local_68 = local_5c[uVar6];
          local_68[1] = puVar9;
          local_68 = local_68 + 2;
        } while (uVar5 < local_50);
      }
      return;
    }
    iVar11 = 1;
  } while( true );
}


