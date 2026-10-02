/* 08009dbc FUN_08009dbc; analyst naming is provisional. */

uint FUN_08009dbc(int *param_1,uint *param_2)

{
  byte bVar1;
  byte bVar2;
  byte bVar3;
  byte bVar4;
  uint *puVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
  uint uVar9;
  uint uVar10;
  int iVar11;
  uint uVar12;
  
  if (*(char *)(param_1 + 0x14) == '\x01') {
    return 2;
  }
  uVar6 = 1;
  iVar7 = *param_1;
  *(undefined *)(param_1 + 0x14) = 1;
  if (*(int *)(iVar7 + 8) << 0x1d < 0) {
    param_1[0x15] = param_1[0x15] | 0x20;
    goto LAB_08009de6;
  }
  uVar9 = *param_2;
  if (-1 < (int)uVar9) {
    if ((uVar9 & 0xfffff) == 0) {
      uVar6 = 1 << (uVar9 >> 0x1a);
    }
    else {
      bVar2 = (byte)uVar9;
      bVar3 = (byte)(uVar9 >> 8);
      bVar4 = (byte)(uVar9 >> 0x10);
      bVar1 = (byte)(uVar9 >> 0x18);
      uVar10 = (uint)(byte)((((((((bVar2 & 1) << 1 | bVar2 >> 1 & 1) << 1 | bVar2 >> 2 & 1) << 1 |
                               bVar2 >> 3 & 1) << 1 | bVar2 >> 4 & 1) << 1 | bVar2 >> 5 & 1) << 1 |
                            bVar2 >> 6 & 1) << 1 | bVar2 >> 7) << 0x18 |
               (uint)(byte)((((((((bVar3 & 1) << 1 | bVar3 >> 1 & 1) << 1 | bVar3 >> 2 & 1) << 1 |
                               bVar3 >> 3 & 1) << 1 | bVar3 >> 4 & 1) << 1 | bVar3 >> 5 & 1) << 1 |
                            bVar3 >> 6 & 1) << 1 | bVar3 >> 7) << 0x10 |
               (uint)(byte)((((((((bVar4 & 1) << 1 | bVar4 >> 1 & 1) << 1 | bVar4 >> 2 & 1) << 1 |
                               bVar4 >> 3 & 1) << 1 | bVar4 >> 4 & 1) << 1 | bVar4 >> 5 & 1) << 1 |
                            bVar4 >> 6 & 1) << 1 | bVar4 >> 7) << 8 |
               (uint)(byte)((((((((bVar1 & 1) << 1 | bVar1 >> 1 & 1) << 1 | bVar1 >> 2 & 1) << 1 |
                               bVar1 >> 3 & 1) << 1 | bVar1 >> 4 & 1) << 1 | bVar1 >> 5 & 1) << 1 |
                            bVar1 >> 6 & 1) << 1 | bVar1 >> 7);
      if (uVar10 != 0) {
        uVar6 = 1 << LZCOUNT(uVar10);
      }
    }
    *(uint *)(iVar7 + 0x1c) = uVar6 | *(uint *)(iVar7 + 0x1c);
  }
  uVar10 = param_2[1] & 0x1f;
  uVar6 = param_2[1] >> 6 & 0xc;
  *(uint *)(uVar6 + iVar7 + 0x30) =
       (uVar9 >> 0x1a & 0x1f) << uVar10 | *(uint *)(uVar6 + iVar7 + 0x30) & ~(0x1f << uVar10);
  puVar5 = DAT_0800a140;
  if (((*(uint *)(iVar7 + 8) & 4) == 0) && ((*(uint *)(iVar7 + 8) & 8) == 0)) {
    uVar6 = (*param_2 << 7) >> 0x1b;
    uVar9 = *param_2 >> 0x17 & 4;
    *(uint *)(uVar9 + iVar7 + 0x14) =
         *(uint *)(uVar9 + iVar7 + 0x14) & ~(7 << uVar6) | param_2[2] << uVar6;
    if ((*puVar5 & 0xf0000000) == 0x10000000) {
      uVar6 = param_2[5] << (((uint)(*(int *)(iVar7 + 0xc) << 0x1b) >> 0x1d) << 1);
    }
    else if (*(int *)(iVar7 + 0xc) << 0x1b < 0) {
      uVar6 = param_2[5] << (*(uint *)(iVar7 + 0xc) >> 1 & 8);
    }
    else {
      uVar6 = param_2[5] << (((*(uint *)(iVar7 + 0xc) << 0x1b) >> 0x1d) << 1);
    }
    uVar9 = param_2[4];
    if (uVar9 == 4) {
      iVar11 = *param_2 * 0x4000000;
      if ((*(uint *)(iVar7 + 0x60) & 0x7c000000) == *param_2 * 0x4000000) {
        *(uint *)(iVar7 + 0x60) = *(uint *)(iVar7 + 0x60) & 0x7fffffff;
      }
      if (iVar11 - (*(uint *)(iVar7 + 100) & 0x7c000000) == 0) {
        *(uint *)(iVar7 + 100) = *(uint *)(iVar7 + 100) & 0x7fffffff;
      }
      if (iVar11 - (*(uint *)(iVar7 + 0x68) & 0x7c000000) == 0) {
        *(uint *)(iVar7 + 0x68) = *(uint *)(iVar7 + 0x68) & 0x7fffffff;
      }
      if (iVar11 - (*(uint *)(iVar7 + 0x6c) & 0x7c000000) == 0) {
        *(uint *)(iVar7 + 0x6c) = *(uint *)(iVar7 + 0x6c) & 0x7fffffff;
      }
    }
    else {
      iVar11 = iVar7 + 0x60;
      *(uint *)(iVar11 + uVar9 * 4) =
           *param_2 & 0x7c000000 | *(uint *)(iVar11 + uVar9 * 4) & 0x80000000 | uVar6;
      if (*(char *)((int)param_2 + 0x19) == '\x01') {
        uVar6 = 0x80000000;
      }
      else {
        uVar6 = 0;
      }
      *(uint *)(iVar11 + param_2[4] * 4) = *(uint *)(iVar11 + param_2[4] * 4) & 0x7fffffff | uVar6;
      uVar6 = 0;
      if (*(char *)(param_2 + 6) == '\x01') {
        uVar6 = 0x800 << (param_2[4] & 0x1f);
      }
      *(uint *)(iVar7 + 0x10) = uVar6 | *(uint *)(iVar7 + 0x10) & 0xffff87ff;
    }
  }
  if (-1 < *(int *)(iVar7 + 8) << 0x1f) {
    uVar9 = param_2[3];
    uVar6 = *param_2;
    *(uint *)(iVar7 + 0xc0) =
         DAT_0800a144 >> (uVar9 & 0x18) & uVar6 | *(uint *)(iVar7 + 0xc0) & ~(uVar6 & 0xfffff);
    if (uVar9 == DAT_0800a148) {
      if ((uVar6 & 0xfffff) == 0) {
        uVar6 = (uVar6 >> 0x1a) + 1 & 0x1f;
        if (uVar6 < 10) {
          uVar9 = 1 << uVar6;
          uVar6 = uVar6 * 0x300000;
        }
        else {
          uVar9 = 1 << uVar6;
          uVar6 = (uVar6 * 3 + -0x1e) * 0x100000 | 0x2000000;
        }
      }
      else {
        bVar2 = (byte)uVar6;
        bVar3 = (byte)(uVar6 >> 8);
        bVar4 = (byte)(uVar6 >> 0x10);
        bVar1 = (byte)(uVar6 >> 0x18);
        uVar6 = (uint)(byte)((((((((bVar2 & 1) << 1 | bVar2 >> 1 & 1) << 1 | bVar2 >> 2 & 1) << 1 |
                                bVar2 >> 3 & 1) << 1 | bVar2 >> 4 & 1) << 1 | bVar2 >> 5 & 1) << 1 |
                             bVar2 >> 6 & 1) << 1 | bVar2 >> 7) << 0x18 |
                (uint)(byte)((((((((bVar3 & 1) << 1 | bVar3 >> 1 & 1) << 1 | bVar3 >> 2 & 1) << 1 |
                                bVar3 >> 3 & 1) << 1 | bVar3 >> 4 & 1) << 1 | bVar3 >> 5 & 1) << 1 |
                             bVar3 >> 6 & 1) << 1 | bVar3 >> 7) << 0x10 |
                (uint)(byte)((((((((bVar4 & 1) << 1 | bVar4 >> 1 & 1) << 1 | bVar4 >> 2 & 1) << 1 |
                                bVar4 >> 3 & 1) << 1 | bVar4 >> 4 & 1) << 1 | bVar4 >> 5 & 1) << 1 |
                             bVar4 >> 6 & 1) << 1 | bVar4 >> 7) << 8 |
                (uint)(byte)((((((((bVar1 & 1) << 1 | bVar1 >> 1 & 1) << 1 | bVar1 >> 2 & 1) << 1 |
                                bVar1 >> 3 & 1) << 1 | bVar1 >> 4 & 1) << 1 | bVar1 >> 5 & 1) << 1 |
                             bVar1 >> 6 & 1) << 1 | bVar1 >> 7);
        if ((uVar6 == 0) || ((LZCOUNT(uVar6) + 1U & 0x1f) < 10)) {
          uVar6 = (uint)(byte)((((((((bVar2 & 1) << 1 | bVar2 >> 1 & 1) << 1 | bVar2 >> 2 & 1) << 1
                                  | bVar2 >> 3 & 1) << 1 | bVar2 >> 4 & 1) << 1 | bVar2 >> 5 & 1) <<
                                1 | bVar2 >> 6 & 1) << 1 | bVar2 >> 7) << 0x18 |
                  (uint)(byte)((((((((bVar3 & 1) << 1 | bVar3 >> 1 & 1) << 1 | bVar3 >> 2 & 1) << 1
                                  | bVar3 >> 3 & 1) << 1 | bVar3 >> 4 & 1) << 1 | bVar3 >> 5 & 1) <<
                                1 | bVar3 >> 6 & 1) << 1 | bVar3 >> 7) << 0x10 |
                  (uint)(byte)((((((((bVar4 & 1) << 1 | bVar4 >> 1 & 1) << 1 | bVar4 >> 2 & 1) << 1
                                  | bVar4 >> 3 & 1) << 1 | bVar4 >> 4 & 1) << 1 | bVar4 >> 5 & 1) <<
                                1 | bVar4 >> 6 & 1) << 1 | bVar4 >> 7) << 8 |
                  (uint)(byte)((((((((bVar1 & 1) << 1 | bVar1 >> 1 & 1) << 1 | bVar1 >> 2 & 1) << 1
                                  | bVar1 >> 3 & 1) << 1 | bVar1 >> 4 & 1) << 1 | bVar1 >> 5 & 1) <<
                                1 | bVar1 >> 6 & 1) << 1 | bVar1 >> 7);
          if (uVar6 == 0) {
            uVar9 = 0;
          }
          else {
            uVar9 = 1 << (LZCOUNT(uVar6) + 1U & 0x1f);
          }
          uVar6 = (uint)(byte)((((((((bVar2 & 1) << 1 | bVar2 >> 1 & 1) << 1 | bVar2 >> 2 & 1) << 1
                                  | bVar2 >> 3 & 1) << 1 | bVar2 >> 4 & 1) << 1 | bVar2 >> 5 & 1) <<
                                1 | bVar2 >> 6 & 1) << 1 | bVar2 >> 7) << 0x18 |
                  (uint)(byte)((((((((bVar3 & 1) << 1 | bVar3 >> 1 & 1) << 1 | bVar3 >> 2 & 1) << 1
                                  | bVar3 >> 3 & 1) << 1 | bVar3 >> 4 & 1) << 1 | bVar3 >> 5 & 1) <<
                                1 | bVar3 >> 6 & 1) << 1 | bVar3 >> 7) << 0x10 |
                  (uint)(byte)((((((((bVar4 & 1) << 1 | bVar4 >> 1 & 1) << 1 | bVar4 >> 2 & 1) << 1
                                  | bVar4 >> 3 & 1) << 1 | bVar4 >> 4 & 1) << 1 | bVar4 >> 5 & 1) <<
                                1 | bVar4 >> 6 & 1) << 1 | bVar4 >> 7) << 8 |
                  (uint)(byte)((((((((bVar1 & 1) << 1 | bVar1 >> 1 & 1) << 1 | bVar1 >> 2 & 1) << 1
                                  | bVar1 >> 3 & 1) << 1 | bVar1 >> 4 & 1) << 1 | bVar1 >> 5 & 1) <<
                                1 | bVar1 >> 6 & 1) << 1 | bVar1 >> 7);
          if (uVar6 == 0) {
            uVar6 = 0x300000;
          }
          else {
            uVar6 = (LZCOUNT(uVar6) + 1U & 0x1f) * 0x300000;
          }
        }
        else {
          uVar10 = (uint)(byte)((((((((bVar2 & 1) << 1 | bVar2 >> 1 & 1) << 1 | bVar2 >> 2 & 1) << 1
                                   | bVar2 >> 3 & 1) << 1 | bVar2 >> 4 & 1) << 1 | bVar2 >> 5 & 1)
                                 << 1 | bVar2 >> 6 & 1) << 1 | bVar2 >> 7) << 0x18 |
                   (uint)(byte)((((((((bVar3 & 1) << 1 | bVar3 >> 1 & 1) << 1 | bVar3 >> 2 & 1) << 1
                                   | bVar3 >> 3 & 1) << 1 | bVar3 >> 4 & 1) << 1 | bVar3 >> 5 & 1)
                                 << 1 | bVar3 >> 6 & 1) << 1 | bVar3 >> 7) << 0x10 |
                   (uint)(byte)((((((((bVar4 & 1) << 1 | bVar4 >> 1 & 1) << 1 | bVar4 >> 2 & 1) << 1
                                   | bVar4 >> 3 & 1) << 1 | bVar4 >> 4 & 1) << 1 | bVar4 >> 5 & 1)
                                 << 1 | bVar4 >> 6 & 1) << 1 | bVar4 >> 7) << 8 |
                   (uint)(byte)((((((((bVar1 & 1) << 1 | bVar1 >> 1 & 1) << 1 | bVar1 >> 2 & 1) << 1
                                   | bVar1 >> 3 & 1) << 1 | bVar1 >> 4 & 1) << 1 | bVar1 >> 5 & 1)
                                 << 1 | bVar1 >> 6 & 1) << 1 | bVar1 >> 7);
          uVar9 = 1 << (LZCOUNT(uVar6) + 1U & 0x1f);
          uVar6 = DAT_0800a1f0;
          if (uVar10 != 0) {
            uVar6 = ((short)((short)LZCOUNT(uVar10) + 1U & 0x1f) * 3 + -0x1e) * 0x100000 | 0x2000000
            ;
          }
        }
      }
      uVar10 = ((uVar6 | uVar9) << 7) >> 0x1b;
      uVar6 = (uVar6 | uVar9) >> 0x17 & 4;
      *(uint *)(uVar6 + iVar7 + 0x14) =
           param_2[2] << uVar10 | *(uint *)(uVar6 + iVar7 + 0x14) & ~(7 << uVar10);
      uVar6 = *param_2;
    }
    if ((int)uVar6 < 0) {
      if ((iVar7 == DAT_0800a14c) || (iVar7 == DAT_0800a14c + 0x100)) {
        uVar10 = *(uint *)(DAT_0800a164 + 8);
        uVar9 = *(uint *)(DAT_0800a14c + 0x108) | *(uint *)(DAT_0800a14c + 8);
        iVar11 = DAT_0800a164;
      }
      else {
        uVar10 = *(uint *)(DAT_0800a150 + 8);
        uVar9 = *(uint *)(DAT_0800a154 + 8);
        iVar11 = DAT_0800a150;
      }
      uVar12 = uVar10 & 0x1c00000;
      if ((~uVar9 & 1) == 0) {
        uVar6 = 1;
        param_1[0x15] = param_1[0x15] | 0x20;
        goto LAB_08009de6;
      }
      if (uVar6 == DAT_0800a158) {
        if ((-1 < (int)(uVar10 << 8)) && (iVar7 == DAT_0800a1e4)) {
          iVar8 = (uint)((ulonglong)DAT_0800a1ec * (ulonglong)(*DAT_0800a1e8 >> 6) >> 0x26) + 1;
          *(uint *)(iVar11 + 8) = *(uint *)(iVar11 + 8) & 0xfe3fffff | uVar12 | 0x800000;
          iVar7 = iVar8 * 2;
          while (iVar8 != 0) {
            iVar7 = iVar7 + -1;
            iVar8 = iVar7;
          }
        }
      }
      else if (uVar6 == DAT_0800a15c) {
        if (((uVar10 & 0x1000000) == 0) && (iVar7 == DAT_0800a1e4)) {
          *(uint *)(iVar11 + 8) = *(uint *)(iVar11 + 8) & 0xfe3fffff | uVar12 | 0x1000000;
          uVar6 = 0;
          goto LAB_08009de6;
        }
      }
      else if (((uVar6 == DAT_0800a160) && (-1 < (int)(uVar10 << 9))) && (iVar7 == DAT_0800a154)) {
        uVar6 = 0;
        *(uint *)(iVar11 + 8) = *(uint *)(iVar11 + 8) & 0xfe3fffff | uVar12 | 0x400000;
        goto LAB_08009de6;
      }
    }
  }
  uVar6 = 0;
LAB_08009de6:
  *(undefined *)(param_1 + 0x14) = 0;
  return uVar6;
}


