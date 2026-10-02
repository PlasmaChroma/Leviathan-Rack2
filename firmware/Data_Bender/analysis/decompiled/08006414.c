/* 08006414 AudioHandle_Impl_InternalCallback; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void AudioHandle_Impl_InternalCallback(int param_1,int param_2,uint param_3)

{
  short sVar1;
  float fVar2;
  undefined4 uVar3;
  code **ppcVar4;
  undefined4 uVar5;
  float fVar6;
  int *piVar7;
  float **ppfVar8;
  int iVar9;
  float **ppfVar10;
  int iVar11;
  float **ppfVar12;
  int iVar13;
  int iVar14;
  uint uVar15;
  uint uVar16;
  int iVar17;
  uint uVar18;
  float *pfVar19;
  uint uVar20;
  code *pcVar21;
  float **ppfVar22;
  float **ppfVar23;
  float **ppfVar24;
  int iVar25;
  undefined4 uVar26;
  code *pcVar27;
  float fVar28;
  float fVar29;
  undefined auStack_58 [4];
  int iStack_54;
  float *apfStack_50 [2];
  float *pfStack_48;
  int local_44;
  float **local_40;
  float **local_3c;
  code *local_38;
  float **local_34;
  
  ppcVar4 = DAT_08006818;
  iVar9 = SaiHandle_GetConfig(DAT_08006818 + 6);
  if (ppcVar4[6] == (code *)0x0) {
    if (ppcVar4[7] == (code *)0x0) {
      return;
    }
    pcVar21 = ppcVar4[1];
    uVar20 = 2;
    local_34 = *(float ***)(iVar9 + 0x14);
  }
  else {
    pcVar21 = ppcVar4[1];
    local_34 = *(float ***)(iVar9 + 0x14);
    if (ppcVar4[7] == (code *)0x0) {
      uVar20 = 2;
    }
    else {
      uVar20 = 4;
    }
  }
  if (pcVar21 != (code *)0x0) {
    uVar20 = param_3 * 4 + 7 & 0xfffffff8;
    iVar9 = -uVar20;
    if (local_34 == (float **)0x1) {
      if (param_3 != 0) {
        pcVar27 = ppcVar4[0xc];
        uVar15 = 0;
        pfVar19 = (float *)((int)&pfStack_48 + iVar9);
        do {
          iVar17 = uVar15 * 4;
          uVar16 = *(uint *)(param_1 + 4 + uVar15 * 4);
          uVar15 = uVar15 + 2;
          *pfVar19 = (float)(longlong)(int)((*(uint *)(param_1 + iVar17) ^ 0x800000) - 0x800000) *
                     DAT_08006824 * (float)pcVar27;
          pfVar19[1] = (float)(longlong)(int)((uVar16 ^ 0x800000) - 0x800000) * DAT_08006824 *
                       (float)pcVar27;
          pfVar19 = pfVar19 + 2;
        } while (uVar15 < param_3);
        local_34 = (float **)((int)&pfStack_48 + uVar20 * -2);
        (*pcVar21)((int)&pfStack_48 + iVar9,(int)&pfStack_48 + uVar20 * -2,param_3);
        fVar6 = DAT_0800682c;
        fVar2 = DAT_08006828;
        uVar5 = DAT_08006820;
        uVar3 = DAT_08006814;
        pcVar21 = ppcVar4[0xd];
        uVar20 = 0;
        do {
          fVar29 = (float)pcVar21 * (float)*local_34;
          uVar26 = uVar5;
          if ((fVar2 < fVar29) && (uVar26 = uVar3, fVar29 < fVar6 != (NAN(fVar29) || NAN(fVar6)))) {
            uVar26 = FPToFixed(fVar29,0x20,0x20,0x17,0,3);
          }
          fVar29 = ((float *)local_34)[1];
          *(undefined4 *)(param_2 + uVar20 * 4) = uVar26;
          fVar29 = (float)pcVar21 * fVar29;
          uVar26 = uVar5;
          if ((fVar2 < fVar29) && (uVar26 = uVar3, fVar29 < fVar6 != (NAN(fVar29) || NAN(fVar6)))) {
            uVar26 = FPToFixed(fVar29,0x20,0x20,0x17,0,3);
          }
          *(undefined4 *)(param_2 + 4 + uVar20 * 4) = uVar26;
          uVar20 = uVar20 + 2;
          local_34 = (float **)((float *)local_34 + 2);
        } while (uVar20 < param_3);
        return;
      }
    }
    else if (local_34 == (float **)0x2) {
      if (param_3 != 0) {
        pcVar27 = ppcVar4[0xc];
        uVar15 = 0;
        pfVar19 = (float *)((int)&pfStack_48 + iVar9);
        do {
          iVar17 = uVar15 * 4;
          iVar13 = *(int *)(param_1 + 4 + uVar15 * 4);
          uVar15 = uVar15 + 2;
          *pfVar19 = (float)(longlong)*(int *)(param_1 + iVar17) * DAT_0800680c * (float)pcVar27;
          pfVar19[1] = (float)(longlong)iVar13 * DAT_0800680c * (float)pcVar27;
          pfVar19 = pfVar19 + 2;
        } while (uVar15 < param_3);
        local_34 = (float **)((int)&pfStack_48 + uVar20 * -2);
        (*pcVar21)((int)&pfStack_48 + iVar9,(int)&pfStack_48 + uVar20 * -2,param_3);
        fVar6 = DAT_0800682c;
        fVar2 = DAT_08006828;
        uVar5 = DAT_0800681c;
        uVar3 = DAT_08006810;
        pcVar21 = ppcVar4[0xd];
        uVar20 = 0;
        do {
          fVar29 = (float)pcVar21 * (float)*local_34;
          uVar26 = uVar5;
          if ((fVar2 < fVar29) && (uVar26 = uVar3, fVar29 < fVar6 != (NAN(fVar29) || NAN(fVar6)))) {
            uVar26 = FPToFixed(fVar29,0x20,0x20,0x1f,0,3);
          }
          fVar29 = ((float *)local_34)[1];
          *(undefined4 *)(param_2 + uVar20 * 4) = uVar26;
          fVar29 = (float)pcVar21 * fVar29;
          uVar26 = uVar5;
          if ((fVar2 < fVar29) && (uVar26 = uVar3, fVar29 < fVar6 != (NAN(fVar29) || NAN(fVar6)))) {
            uVar26 = FPToFixed(fVar29,0x20,0x20,0x1f,0,3);
          }
          *(undefined4 *)(param_2 + 4 + uVar20 * 4) = uVar26;
          uVar20 = uVar20 + 2;
          local_34 = (float **)((float *)local_34 + 2);
        } while (uVar20 < param_3);
        return;
      }
    }
    else if ((local_34 == (float **)0x0) && (param_3 != 0)) {
      pcVar27 = ppcVar4[0xc];
      uVar15 = 0;
      pfVar19 = (float *)((int)&pfStack_48 + iVar9);
      do {
        iVar17 = uVar15 * 4;
        sVar1 = *(short *)(param_1 + 4 + uVar15 * 4);
        uVar15 = uVar15 + 2;
        *pfVar19 = (float)(longlong)(int)*(short *)(param_1 + iVar17) * DAT_08006800 *
                   (float)pcVar27;
        pfVar19[1] = (float)(longlong)(int)sVar1 * DAT_08006800 * (float)pcVar27;
        pfVar19 = pfVar19 + 2;
      } while (uVar15 < param_3);
      local_34 = (float **)((int)&pfStack_48 + uVar20 * -2);
      (*pcVar21)((int)&pfStack_48 + iVar9,(int)&pfStack_48 + uVar20 * -2,param_3);
      fVar29 = DAT_0800682c;
      fVar6 = DAT_08006828;
      fVar2 = DAT_08006808;
      iVar9 = DAT_08006804;
      pcVar21 = ppcVar4[0xd];
      uVar20 = 0;
      do {
        fVar28 = (float)pcVar21 * (float)*local_34;
        iVar17 = iVar9;
        if (fVar6 < fVar28) {
          if (fVar28 < fVar29 == (NAN(fVar28) || NAN(fVar29))) {
            iVar17 = 0x7ffe;
          }
          else {
            iVar17 = (int)(short)(int)(fVar28 * fVar2);
          }
        }
        fVar28 = ((float *)local_34)[1];
        *(int *)(param_2 + uVar20 * 4) = iVar17;
        fVar28 = (float)pcVar21 * fVar28;
        iVar17 = iVar9;
        if (fVar6 < fVar28) {
          if (fVar28 < fVar29 == (NAN(fVar28) || NAN(fVar29))) {
            iVar17 = 0x7ffe;
          }
          else {
            iVar17 = (int)(short)(int)(fVar28 * fVar2);
          }
        }
        *(int *)(param_2 + 4 + uVar20 * 4) = iVar17;
        uVar20 = uVar20 + 2;
        local_34 = (float **)((float *)local_34 + 2);
      } while (uVar20 < param_3);
      return;
    }
    (*pcVar21)((int)&pfStack_48 + iVar9,(int)&pfStack_48 + uVar20 * -2,param_3);
    return;
  }
  local_38 = *ppcVar4;
  if (local_38 == (code *)0x0) {
    return;
  }
  iVar9 = FUN_08009384(DAT_080067fc);
  ppfVar8 = local_34;
  fVar29 = DAT_08006bdc;
  fVar6 = DAT_08006bcc;
  fVar2 = DAT_08006824;
  if (uVar20 == 2) {
    uVar15 = param_3 * 4 + 7 & 0xfffffff8;
    iVar17 = -uVar15;
    ppfVar24 = (float **)((int)&pfStack_48 + iVar17);
    ppfVar10 = (float **)((int)apfStack_50 + uVar15 * -2);
    ppfVar22 = (float **)((int)&pfStack_48 + (param_3 >> 1) * 4 + iVar17);
    ppfVar23 = (float **)(auStack_58 + uVar15 * -2);
    *(int *)((int)apfStack_50 + uVar15 * -2) = (int)&pfStack_48 + iVar17;
    *(float ***)((int)apfStack_50 + uVar15 * -2 + 4) = ppfVar22;
    *(uint *)(auStack_58 + uVar15 * -2) = (int)&pfStack_48 + uVar15 * -2;
    *(uint *)((int)&iStack_54 + uVar15 * -2) = (int)&pfStack_48 + (param_3 >> 1) * 4 + uVar15 * -2;
  }
  else {
    ppfVar24 = &pfStack_48 + param_3 * -2;
    uVar15 = (param_3 << 1) / uVar20;
    local_3c = ppfVar24 + param_3 * -2;
    ppfVar22 = ppfVar24 + uVar15;
    local_34 = local_3c + uVar15;
    ppfVar10 = local_3c + -uVar20;
    ppfVar10[2] = (float *)(ppfVar22 + uVar15);
    ppfVar10[3] = (float *)(ppfVar22 + uVar15 + uVar15);
    ppfVar12 = local_34 + uVar15;
    ppfVar23 = ppfVar10 + -uVar20;
    local_40 = ppfVar12 + uVar15;
    *ppfVar23 = (float *)local_3c;
    ppfVar23[1] = (float *)local_34;
    ppfVar23[2] = (float *)ppfVar12;
    ppfVar12 = local_40;
    *ppfVar10 = (float *)ppfVar24;
    ppfVar10[1] = (float *)ppfVar22;
    ppfVar23[3] = (float *)ppfVar12;
  }
  if (ppfVar8 == (float **)0x1) {
    if (param_3 != 0) {
      pcVar21 = ppcVar4[9];
      uVar15 = 0;
      local_40 = ppfVar23;
      local_3c = (float **)(iVar9 << 2);
      local_34 = (float **)param_2;
      do {
        uVar18 = uVar15 >> 1;
        pcVar27 = ppcVar4[0xc];
        uVar16 = *(uint *)(param_1 + 4 + uVar15 * 4);
        ppfVar24[uVar18] =
             (float *)((float)(longlong)
                              (int)((*(uint *)(param_1 + uVar15 * 4) ^ 0x800000) - 0x800000) * fVar2
                      * (float)pcVar27);
        ppfVar22[uVar18] =
             (float *)((float)(longlong)(int)((uVar16 ^ 0x800000) - 0x800000) * fVar2 *
                      (float)pcVar27);
        iVar17 = (int)local_34;
        piVar7 = (int *)local_40;
        if (uVar20 != 2) {
          uVar16 = *(uint *)(pcVar21 + uVar15 * 4 + iVar9 * 4 + 4);
          ppfVar10[2][uVar18] =
               (float)(longlong)
                      (int)((*(uint *)(pcVar21 + uVar15 * 4 + iVar9 * 4) ^ 0x800000) - 0x800000) *
               fVar2 * (float)pcVar27;
          ppfVar10[3][uVar18] =
               (float)(longlong)(int)((uVar16 ^ 0x800000) - 0x800000) * fVar2 * (float)ppcVar4[0xc];
        }
        uVar15 = uVar15 + 2;
      } while (uVar15 < param_3);
      (*local_38)(ppfVar10,local_40,param_3 >> 1);
      uVar5 = DAT_08006bd8;
      uVar3 = DAT_08006bc8;
      fVar6 = DAT_0800682c;
      fVar2 = DAT_08006828;
      pcVar21 = ppcVar4[0xb];
      pcVar27 = ppcVar4[0xd];
      uVar15 = 0;
      iVar9 = *piVar7;
      iVar13 = piVar7[1];
      do {
        iVar25 = (uVar15 >> 1) * 4;
        fVar29 = (float)pcVar27 * *(float *)(iVar9 + (uVar15 >> 1) * 4);
        uVar26 = uVar5;
        if ((fVar2 < fVar29) && (uVar26 = uVar3, fVar29 < fVar6 != (NAN(fVar29) || NAN(fVar6)))) {
          uVar26 = FPToFixed(fVar29,0x20,0x20,0x17,0,3);
        }
        *(undefined4 *)(iVar17 + uVar15 * 4) = uVar26;
        fVar29 = (float)pcVar27 * *(float *)(iVar13 + iVar25);
        uVar26 = uVar5;
        if ((fVar2 < fVar29) && (uVar26 = uVar3, fVar29 < fVar6 != (NAN(fVar29) || NAN(fVar6)))) {
          uVar26 = FPToFixed(fVar29,0x20,0x20,0x17,0,3);
        }
        *(undefined4 *)(iVar17 + 4 + uVar15 * 4) = uVar26;
        if (uVar20 != 2) {
          fVar29 = (float)pcVar27 * *(float *)(piVar7[2] + iVar25);
          uVar26 = uVar5;
          if ((fVar2 < fVar29) && (uVar26 = uVar3, fVar29 < fVar6 != (NAN(fVar29) || NAN(fVar6)))) {
            uVar26 = FPToFixed(fVar29,0x20,0x20,0x17,0,3);
          }
          *(undefined4 *)(pcVar21 + uVar15 * 4 + (int)local_3c) = uVar26;
          fVar29 = (float)pcVar27 * *(float *)(iVar25 + piVar7[3]);
          uVar26 = uVar5;
          if ((fVar2 < fVar29) && (uVar26 = uVar3, fVar29 < fVar6 != (NAN(fVar29) || NAN(fVar6)))) {
            uVar26 = FPToFixed(fVar29,0x20,0x20,0x17,0,3);
          }
          *(undefined4 *)(pcVar21 + uVar15 * 4 + (int)local_3c + 4) = uVar26;
        }
        uVar15 = uVar15 + 2;
      } while (uVar15 < param_3);
      return;
    }
  }
  else if (ppfVar8 == (float **)0x2) {
    if (param_3 != 0) {
      pcVar21 = ppcVar4[9];
      uVar15 = 0;
      local_40 = ppfVar23;
      local_3c = (float **)(iVar9 << 2);
      local_34 = (float **)param_2;
      do {
        pcVar27 = ppcVar4[0xc];
        iVar17 = *(int *)(param_1 + 4 + uVar15 * 4);
        uVar16 = uVar15 >> 1;
        ppfVar24[uVar16] =
             (float *)((float)(longlong)*(int *)(param_1 + uVar15 * 4) * fVar29 * (float)pcVar27);
        ppfVar22[uVar16] = (float *)((float)(longlong)iVar17 * fVar29 * (float)pcVar27);
        iVar17 = (int)local_34;
        piVar7 = (int *)local_40;
        if (uVar20 != 2) {
          iVar13 = *(int *)(pcVar21 + uVar15 * 4 + iVar9 * 4 + 4);
          ppfVar10[2][uVar16] =
               (float)(longlong)*(int *)(pcVar21 + uVar15 * 4 + iVar9 * 4) * fVar29 * (float)pcVar27
          ;
          ppfVar10[3][uVar16] = (float)(longlong)iVar13 * fVar29 * (float)ppcVar4[0xc];
        }
        uVar15 = uVar15 + 2;
      } while (uVar15 < param_3);
      (*local_38)(ppfVar10,local_40,param_3 >> 1);
      uVar5 = DAT_08006eb8;
      uVar3 = DAT_08006eb4;
      fVar6 = DAT_08006be4;
      fVar2 = DAT_08006be0;
      pcVar21 = ppcVar4[0xb];
      pcVar27 = ppcVar4[0xd];
      uVar15 = 0;
      iVar9 = *piVar7;
      iVar13 = piVar7[1];
      do {
        iVar25 = (uVar15 >> 1) * 4;
        fVar29 = (float)pcVar27 * *(float *)(iVar9 + (uVar15 >> 1) * 4);
        uVar26 = uVar5;
        if ((fVar2 < fVar29) && (uVar26 = uVar3, fVar29 < fVar6 != (NAN(fVar29) || NAN(fVar6)))) {
          uVar26 = FPToFixed(fVar29,0x20,0x20,0x1f,0,3);
        }
        *(undefined4 *)(iVar17 + uVar15 * 4) = uVar26;
        fVar29 = (float)pcVar27 * *(float *)(iVar13 + iVar25);
        uVar26 = uVar5;
        if ((fVar2 < fVar29) && (uVar26 = uVar3, fVar29 < fVar6 != (NAN(fVar29) || NAN(fVar6)))) {
          uVar26 = FPToFixed(fVar29,0x20,0x20,0x1f,0,3);
        }
        *(undefined4 *)(iVar17 + 4 + uVar15 * 4) = uVar26;
        if (uVar20 != 2) {
          fVar29 = (float)pcVar27 * *(float *)(piVar7[2] + iVar25);
          uVar26 = uVar5;
          if ((fVar2 < fVar29) && (uVar26 = uVar3, fVar29 < fVar6 != (NAN(fVar29) || NAN(fVar6)))) {
            uVar26 = FPToFixed(fVar29,0x20,0x20,0x1f,0,3);
          }
          *(undefined4 *)(pcVar21 + uVar15 * 4 + (int)local_3c) = uVar26;
          fVar29 = (float)pcVar27 * *(float *)(iVar25 + piVar7[3]);
          uVar26 = uVar5;
          if ((fVar2 < fVar29) && (uVar26 = uVar3, fVar29 < fVar6 != (NAN(fVar29) || NAN(fVar6)))) {
            uVar26 = FPToFixed(fVar29,0x20,0x20,0x1f,0,3);
          }
          *(undefined4 *)(pcVar21 + uVar15 * 4 + (int)local_3c + 4) = uVar26;
        }
        uVar15 = uVar15 + 2;
      } while (uVar15 < param_3);
      return;
    }
  }
  else {
    if (ppfVar8 != (float **)0x0) {
      (*local_38)(ppfVar10,ppfVar23,param_3 >> 1);
      return;
    }
    if (param_3 != 0) {
      pcVar21 = ppcVar4[9];
      local_40 = (float **)(iVar9 << 2);
      local_44 = param_2;
      local_34 = (float **)(pcVar21 + iVar9 * 4 + 4);
      local_3c = ppfVar23;
      uVar15 = 0;
      do {
        pcVar27 = ppcVar4[0xc];
        sVar1 = *(short *)(param_1 + 4 + uVar15 * 4);
        uVar16 = uVar15 >> 1;
        ppfVar24[uVar16] =
             (float *)((float)(longlong)(int)*(short *)(param_1 + uVar15 * 4) * fVar6 *
                      (float)pcVar27);
        ppfVar22[uVar16] = (float *)((float)(longlong)(int)sVar1 * fVar6 * (float)pcVar27);
        piVar7 = (int *)local_3c;
        iVar17 = local_44;
        if (uVar20 != 2) {
          sVar1 = *(short *)((int)local_34 + uVar15 * 4);
          pfVar19 = ppfVar10[3];
          ppfVar10[2][uVar16] =
               (float)(longlong)(int)*(short *)(pcVar21 + uVar15 * 4 + iVar9 * 4) * fVar6 *
               (float)pcVar27;
          pfVar19[uVar16] = (float)(longlong)(int)sVar1 * fVar6 * (float)ppcVar4[0xc];
        }
        uVar15 = uVar15 + 2;
      } while (uVar15 < param_3);
      (*local_38)(ppfVar10,local_3c,param_3 >> 1);
      fVar29 = DAT_08006be4;
      fVar6 = DAT_08006be0;
      iVar9 = DAT_08006bd4;
      fVar2 = DAT_08006bd0;
      pcVar21 = ppcVar4[0xb];
      pcVar27 = ppcVar4[0xd];
      uVar15 = 0;
      iVar25 = *piVar7;
      iVar13 = piVar7[1];
      do {
        iVar14 = (uVar15 >> 1) * 4;
        fVar28 = (float)pcVar27 * *(float *)(iVar25 + (uVar15 >> 1) * 4);
        iVar11 = iVar9;
        if (fVar6 < fVar28) {
          if (fVar28 < fVar29 == (NAN(fVar28) || NAN(fVar29))) {
            iVar11 = 0x7ffe;
          }
          else {
            iVar11 = (int)(short)(int)(fVar28 * fVar2);
          }
        }
        *(int *)(iVar17 + uVar15 * 4) = iVar11;
        fVar28 = (float)pcVar27 * *(float *)(iVar13 + iVar14);
        iVar11 = iVar9;
        if (fVar6 < fVar28) {
          if (fVar28 < fVar29 == (NAN(fVar28) || NAN(fVar29))) {
            iVar11 = 0x7ffe;
          }
          else {
            iVar11 = (int)(short)(int)(fVar28 * fVar2);
          }
        }
        *(int *)(iVar17 + 4 + uVar15 * 4) = iVar11;
        if (uVar20 != 2) {
          fVar28 = (float)pcVar27 * *(float *)(piVar7[2] + iVar14);
          iVar11 = iVar9;
          if (fVar6 < fVar28) {
            if (fVar28 < fVar29 == (NAN(fVar28) || NAN(fVar29))) {
              iVar11 = 0x7ffe;
            }
            else {
              iVar11 = (int)(short)(int)(fVar28 * fVar2);
            }
          }
          *(int *)(pcVar21 + uVar15 * 4 + (int)local_40) = iVar11;
          fVar28 = (float)pcVar27 * *(float *)(iVar14 + piVar7[3]);
          iVar11 = iVar9;
          if (fVar6 < fVar28) {
            if (fVar28 < fVar29 == (NAN(fVar28) || NAN(fVar29))) {
              iVar11 = 0x7ffe;
            }
            else {
              iVar11 = (int)(short)(int)(fVar28 * fVar2);
            }
          }
          *(int *)(pcVar21 + uVar15 * 4 + (int)local_40 + 4) = iVar11;
        }
        uVar15 = uVar15 + 2;
      } while (uVar15 < param_3);
      return;
    }
  }
  (*local_38)(ppfVar10,ppfVar23,param_3);
  return;
}


