/* 0800ab00 FUN_0800ab00; analyst naming is provisional. */

void FUN_0800ab00(uint **param_1,uint param_2,uint param_3,uint param_4)

{
  bool bVar1;
  bool bVar2;
  uint *puVar3;
  uint *puVar4;
  uint *puVar5;
  bool bVar6;
  bool bVar7;
  bool bVar8;
  bool bVar9;
  bool bVar10;
  bool bVar11;
  bool bVar12;
  bool bVar13;
  bool bVar14;
  bool bVar15;
  bool bVar16;
  bool bVar17;
  bool bVar18;
  bool bVar19;
  bool bVar20;
  bool bVar21;
  bool bVar22;
  bool bVar23;
  bool bVar24;
  
  puVar3 = *param_1;
  puVar4 = param_1[0x16];
  if ((puVar3 == DAT_0800ac90) || (puVar3 == DAT_0800ac90 + 6)) {
    param_1[0x19][1] = (uint)param_1[0x1a];
    if (param_1[0x1b] != (uint *)0x0) {
      param_1[0x1c][1] = (uint)param_1[0x1d];
    }
LAB_0800ac32:
    puVar5 = param_1[2];
    puVar4[2] = 0x3f << ((uint)param_1[0x17] & 0x1f);
    *puVar3 = *puVar3 & 0xfffbffff;
    puVar3[1] = param_4;
  }
  else {
    bVar6 = puVar3 != DAT_0800ac94;
    bVar1 = puVar3 != DAT_0800ac90 + 0xc;
    bVar7 = puVar3 != DAT_0800ac94 + 6;
    bVar8 = puVar3 != DAT_0800ac94 + 0xc;
    bVar9 = puVar3 != DAT_0800ac94 + 0x12;
    bVar10 = puVar3 != DAT_0800ac94 + 0x18;
    bVar11 = puVar3 != DAT_0800ac94 + 0xee;
    bVar12 = puVar3 != DAT_0800ac9c;
    bVar2 = puVar3 != DAT_0800ac98;
    bVar13 = puVar3 != DAT_0800aca0;
    bVar14 = puVar3 != DAT_0800aca4;
    bVar15 = puVar3 != DAT_0800aca8;
    bVar16 = puVar3 != DAT_0800acac;
    bVar17 = puVar3 != DAT_0800acb0;
    puVar5 = DAT_0800acb0 + 0x6001400;
    bVar18 = puVar3 != DAT_0800acb4;
    bVar19 = puVar3 != DAT_0800acb8;
    bVar20 = puVar3 != DAT_0800acbc;
    bVar21 = puVar3 != DAT_0800acc0;
    bVar22 = puVar3 != DAT_0800acc4;
    bVar23 = puVar3 != DAT_0800acc8;
    bVar24 = puVar3 != DAT_0800accc;
    if ((bVar23 && (bVar21 &&
                   (bVar19 && (puVar3 != puVar5 && (bVar16 && (bVar14 && (bVar12 && bVar2))))))) &&
       (bVar24 && (bVar22 &&
                  (bVar20 &&
                  (bVar18 &&
                  (bVar17 &&
                  (bVar15 &&
                  (bVar13 &&
                  (bVar11 && (bVar10 && (bVar9 && (bVar8 && (bVar7 && (bVar6 && bVar1))))))))))))))
    {
      return;
    }
    param_1[0x19][1] = (uint)param_1[0x1a];
    if (param_1[0x1b] == (uint *)0x0) {
      if (!bVar24 ||
          (!bVar22 ||
          (!bVar20 ||
          (!bVar18 ||
          (!bVar17 ||
          (!bVar15 ||
          (!bVar13 ||
          (!bVar11 || (!bVar10 || (!bVar9 || (!bVar8 || (!bVar7 || (!bVar6 || !bVar1)))))))))))))
      goto LAB_0800ac32;
      if (bVar23 && (bVar21 &&
                    (bVar19 && (puVar3 != puVar5 && (bVar16 && (bVar14 && (bVar12 && bVar2))))))) {
        return;
      }
    }
    else {
      param_1[0x1c][1] = (uint)param_1[0x1d];
      if (!bVar24 ||
          (!bVar22 ||
          (!bVar20 ||
          (!bVar18 ||
          (!bVar17 ||
          (!bVar15 ||
          (!bVar13 ||
          (!bVar11 || (!bVar10 || (!bVar9 || (!bVar8 || (!bVar7 || (!bVar6 || !bVar1)))))))))))))
      goto LAB_0800ac32;
    }
    puVar5 = param_1[2];
    puVar4[1] = 1 << ((uint)param_1[0x17] & 0x1f);
    puVar3[1] = param_4;
  }
  if (puVar5 != (uint *)0x40) {
    puVar3[2] = param_2;
    puVar3[3] = param_3;
    return;
  }
  puVar3[2] = param_3;
  puVar3[3] = param_2;
  return;
}


