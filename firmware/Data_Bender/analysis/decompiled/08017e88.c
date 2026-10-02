/* 08017e88 libm_powf; analyst naming is provisional. */

/* WARNING: Type propagation algorithm not settling */
/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

float libm_powf(void)

{
  undefined4 *puVar1;
  undefined4 uVar2;
  uint uVar3;
  int iVar4;
  double *pdVar5;
  uint uVar6;
  undefined *puVar7;
  float fVar8;
  float fVar9;
  longlong in_d0;
  float fVar10;
  double dVar11;
  double dVar12;
  uint uVar13;
  
  fVar9 = (float)in_d0;
  fVar10 = (float)((ulonglong)in_d0 >> 0x20);
  iVar4 = (int)fVar10 * 2;
  fVar8 = fVar9;
  if ((int)fVar9 - 0x800000U < 0x7f000000) {
    if (iVar4 - 1U < 0xfeffffff) {
      uVar3 = 0;
LAB_08017eba:
      puVar7 = &UNK_c0cd0000 + (int)fVar8;
      pdVar5 = (double *)(DAT_080181a0 + ((uint)((int)puVar7 * 0x200) >> 0x1c) * 0x10);
      dVar11 = *pdVar5 * (double)(float)((int)fVar8 - ((uint)puVar7 & 0xff800000)) + -1.0;
      dVar12 = dVar11 * dVar11;
      dVar11 = (double)fVar10 *
               ((double)(longlong)((int)puVar7 >> 0x17) + pdVar5[1] +
                dVar11 * *(double *)(DAT_080181a0 + 0x120) +
                dVar12 * (*(double *)(DAT_080181a0 + 0x118) +
                         dVar11 * *(double *)(DAT_080181a0 + 0x110)) +
               (*(double *)(DAT_080181a0 + 0x108) + dVar11 * *(double *)(DAT_080181a0 + 0x100)) *
               dVar12 * dVar12);
      if (((uint)((int)((ulonglong)dVar11 >> 0x20) << 1) >> 0x10 < 0x80bf) ||
         (((fVar8 = DAT_08017d5c,
           dVar11 == DAT_08018180 || dVar11 < DAT_08018180 != (NAN(dVar11) || NAN(DAT_08018180)) &&
           (fVar8 = DAT_08017d44, DAT_08018190 < dVar11)) &&
          (fVar8 = DAT_08017d50, -1 < (int)((uint)(dVar11 < DAT_08018198) << 0x1f))))) {
        dVar12 = dVar11 + *(double *)(DAT_080181a4 + 0x100);
        uVar13 = SUB84(dVar12,0);
        uVar6 = uVar13 & 0x1f;
        dVar11 = dVar11 - (dVar12 - *(double *)(DAT_080181a4 + 0x100));
        return (float)((dVar11 * *(double *)(DAT_080181a4 + 0x118) + 1.0 +
                       (*(double *)(DAT_080181a4 + 0x110) +
                       dVar11 * *(double *)(DAT_080181a4 + 0x108)) * dVar11 * dVar11) *
                      (double)CONCAT44(*(int *)(DAT_080181a4 + uVar6 * 8 + 4) +
                                       (uVar3 + uVar13) * 0x8000,
                                       *(undefined4 *)(DAT_080181a4 + uVar6 * 8)));
      }
      if (uVar3 == 0) {
        fVar8 = fVar8 * fVar8;
        uVar2 = 0x22;
      }
      else {
        fVar8 = -fVar8 * fVar8;
        uVar2 = 0x22;
      }
      goto LAB_08017d04;
    }
    if ((iVar4 != 0) && (fVar8 = fVar10, fVar9 != 1.0)) goto LAB_080180b4;
  }
  else {
    if (iVar4 - 1U < 0xfeffffff) {
      if ((int)fVar9 * 2 - 1U < 0xfeffffff) {
        if ((int)fVar9 < 0) {
          uVar3 = (uint)((int)fVar10 << 1) >> 0x18;
          if (uVar3 < 0x7f) {
LAB_08017d84:
            fVar8 = (fVar9 - fVar9) / (fVar9 - fVar9);
            if (NAN(fVar9)) {
              return fVar8;
            }
            uVar2 = 0x21;
            goto LAB_08017d04;
          }
          if (uVar3 < 0x97) {
            uVar3 = 1 << (0x96 - uVar3 & 0xff);
            if ((uVar3 - 1 & (uint)fVar10) != 0) goto LAB_08017d84;
            uVar3 = uVar3 & (uint)fVar10;
            if (uVar3 != 0) {
              uVar3 = 0x10000;
            }
          }
          else {
            uVar3 = 0;
          }
          fVar8 = (float)((uint)fVar9 & 0x7fffffff);
        }
        else {
          uVar3 = 0;
        }
        if ((uint)fVar8 < 0x800000) {
          fVar8 = (float)(((uint)(fVar9 * DAT_080181a8) & 0x7fffffff) + 0xf4800000);
        }
        goto LAB_08017eba;
      }
      fVar9 = fVar9 * fVar9;
      if (((int)fVar9 < 0) && (uVar3 = (uint)((int)fVar10 << 1) >> 0x18, uVar3 - 0x7f < 0x18)) {
        uVar3 = 1 << (0x96 - uVar3 & 0xff);
        if ((uVar3 - 1 & (uint)fVar10) != 0) goto LAB_080180ea;
        uVar3 = uVar3 & (uint)fVar10;
        if (uVar3 != 0) {
          fVar9 = -fVar9;
          uVar3 = 1;
        }
      }
      else {
LAB_080180ea:
        uVar3 = 0;
      }
      if ((int)fVar9 * 2 == 0) {
        if (in_d0 < 0) {
          uVar2 = 0x22;
          fVar8 = (float)((uint)(uVar3 == 0) * 0x3f800000 + (uint)(uVar3 != 0) * -0x40800000) /
                  DAT_08017d80;
LAB_08017d04:
          puVar1 = (undefined4 *)FUN_080188d8();
          *puVar1 = uVar2;
          return fVar8;
        }
      }
      else if (in_d0 < 0) {
        fVar9 = 1.0 / fVar9;
      }
      return fVar9;
    }
    if (iVar4 != 0) {
LAB_080180b4:
      uVar3 = (int)fVar9 * 2;
      if ((uVar3 < 0xff000001) && (iVar4 == -0x1000000)) {
        if (uVar3 == 0x7f000000) {
          return (float)0x3f800000;
        }
        if ((uint)(0x7effffff < uVar3) == -((int)~(uint)fVar10 >> 0x1f)) {
          return fVar10 * fVar10;
        }
        return DAT_080181ac;
      }
      goto LAB_0801811a;
    }
  }
  if (((uint)fVar8 ^ 0x400000) * 2 < 0xff800001) {
    return 1.0;
  }
LAB_0801811a:
  return fVar9 + fVar10;
}


