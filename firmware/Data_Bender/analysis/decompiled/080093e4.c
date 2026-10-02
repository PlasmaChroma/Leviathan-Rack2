/* 080093e4 FUN_080093e4; analyst naming is provisional. */

void FUN_080093e4(void)

{
  char *pcVar1;
  undefined uVar2;
  int *piVar3;
  int iVar4;
  uint uVar5;
  undefined4 uVar6;
  int iVar7;
  uint *puVar8;
  int iVar9;
  int *piVar10;
  uint uVar11;
  uint uVar12;
  int iVar13;
  uint uVar14;
  int iVar15;
  uint uVar16;
  int *piVar17;
  uint uVar18;
  
  if (*DAT_08009400 != 0) {
    FUN_0800c498();
  }
  piVar3 = DAT_08009404;
  if (*DAT_08009404 == 0) {
    return;
  }
  iVar15 = *DAT_08009404;
  iVar4 = FUN_08012bcc(iVar15);
  if ((iVar4 == 0) && (iVar4 = FUN_08012b60(*piVar3), iVar4 != 0)) {
    piVar3[0x13f] = (uint)(*(int *)(iVar15 + 0x808) << 10) >> 0x12;
    uVar5 = FUN_08012b60(*piVar3);
    if ((uVar5 & 2) != 0) {
      *(uint *)(*piVar3 + 0x14) = *(uint *)(*piVar3 + 0x14) & 2;
    }
    uVar5 = FUN_08012b60();
    iVar4 = *piVar3;
    if ((uVar5 & 0x10) != 0) {
      *(uint *)(iVar4 + 0x18) = *(uint *)(iVar4 + 0x18) & 0xffffffef;
      uVar12 = *(uint *)(iVar15 + 0x20);
      uVar5 = (uVar12 << 0xb) >> 0x1c;
      uVar16 = uVar12 & 0xf;
      if (uVar5 == 2) {
        if ((uVar12 & 0x7ff0) != 0) {
          uVar5 = (uVar12 << 0x11) >> 0x15;
          FUN_08012a34(iVar15,piVar3[uVar16 * 9 + 0xa2]);
          iVar4 = *piVar3;
          piVar3[uVar16 * 9 + 0xa2] = piVar3[uVar16 * 9 + 0xa2] + uVar5;
          piVar3[uVar16 * 9 + 0xa4] = piVar3[uVar16 * 9 + 0xa4] + uVar5;
        }
      }
      else if (uVar5 == 6) {
        FUN_08012a34(iVar15,piVar3 + 0x131,8);
        iVar4 = *piVar3;
        piVar3[uVar16 * 9 + 0xa4] = ((uVar12 << 0x11) >> 0x15) + piVar3[uVar16 * 9 + 0xa4];
      }
      *(uint *)(iVar4 + 0x18) = *(uint *)(iVar4 + 0x18) | 0x10;
    }
    iVar4 = FUN_08012b60();
    if ((iVar4 << 0xc < 0) && (uVar5 = FUN_08012b78(*piVar3), uVar5 != 0)) {
      iVar4 = iVar15 + 0xb00;
      uVar12 = 0;
      piVar10 = piVar3;
      do {
        if ((int)(uVar5 << 0x1f) < 0) {
          uVar11 = uVar12 & 0xff;
          uVar16 = FUN_08012b98(*piVar3,uVar11);
          if ((uVar16 & 1) != 0) {
            iVar13 = *piVar3;
            iVar7 = piVar3[3];
            *(undefined4 *)(iVar4 + 8) = 1;
            uVar18 = *(uint *)(iVar13 + 0x40);
            iVar9 = iVar13 + 0xb00 + uVar12 * 0x20;
            uVar14 = *(uint *)(iVar9 + 8);
            if (iVar7 == 1) {
              if ((int)(uVar14 << 0x1c) < 0) {
                if ((DAT_0800ee48 < uVar18) && ((int)(uVar14 << 0x10) < 0)) {
LAB_0800ed9a:
                  *(undefined4 *)(iVar9 + 8) = 0x8000;
                }
              }
              else if ((int)(uVar14 << 0x1a) < 0) {
                *(undefined4 *)(iVar9 + 8) = 0x20;
              }
              else if ((uVar14 & 0x28) == 0) {
                if ((uVar18 <= DAT_0800ee48) || (-1 < (int)(uVar14 << 0x10))) {
                  iVar7 = piVar10[0xa7] - (*(uint *)(iVar9 + 0x10) & 0x7ffff);
                  piVar10[0xa4] = iVar7;
                  if (uVar12 == 0) {
                    if (piVar3[0xa3] == 0) {
                      FUN_08012c00(iVar13,1,piVar3 + 0x131);
                    }
                    else {
                      piVar3[0xa2] = iVar7 + piVar3[0xa2];
                    }
                  }
                  goto LAB_0800ec4c;
                }
                goto LAB_0800ed9a;
              }
            }
            else {
              if (uVar18 == DAT_0800ee4c) {
                if ((int)(uVar14 << 0x10) < 0) goto LAB_0800ed9a;
                if ((int)(uVar14 << 0x1a) < 0) {
                  *(undefined4 *)(iVar9 + 8) = 0x20;
                }
              }
              else if ((uVar12 == 0) && (piVar3[0xa3] == 0)) {
                FUN_08012c00(iVar13,0,piVar3 + 0x131);
              }
LAB_0800ec4c:
              FUN_080099a4(piVar3,uVar11);
            }
          }
          uVar14 = DAT_0800ee48;
          if ((uVar16 & 8) != 0) {
            iVar13 = *piVar3;
            *(undefined4 *)(iVar4 + 8) = 8;
            iVar7 = iVar13 + 0xb00 + uVar12 * 0x20;
            if (uVar14 < *(uint *)(iVar13 + 0x40)) {
              if (*(int *)(iVar7 + 8) << 0x10 < 0) {
                *(undefined4 *)(iVar7 + 8) = 0x8000;
              }
              FUN_08009998(piVar3);
              if (piVar3[3] == 1) {
                FUN_08012c00(*piVar3,1,piVar3 + 0x131);
              }
            }
            else {
              FUN_08009998(piVar3);
            }
          }
          if ((uVar16 & 0x10) != 0) {
            *(undefined4 *)(iVar4 + 8) = 0x10;
          }
          if ((uVar16 & 2) != 0) {
            if (*(int *)(iVar15 + 0x14) << 0x18 < 0) {
              *(uint *)(iVar15 + 0x804) = *(uint *)(iVar15 + 0x804) | 0x400;
            }
            if (*(char *)((int)piVar10 + 0x27f) == '\x01') {
              *(undefined *)((int)piVar10 + 0x27f) = 0;
              FUN_08009a30(piVar3,uVar11);
            }
            *(undefined4 *)(iVar4 + 8) = 2;
          }
          if ((uVar16 & 0x20) != 0) {
            *(undefined4 *)(iVar4 + 8) = 0x20;
          }
          if ((uVar16 & 0x2000) != 0) {
            *(undefined4 *)(iVar4 + 8) = 0x2000;
          }
        }
        uVar5 = uVar5 >> 1;
        uVar12 = uVar12 + 1;
        iVar4 = iVar4 + 0x20;
        piVar10 = piVar10 + 9;
      } while (uVar5 != 0);
    }
    iVar4 = FUN_08012b60(*piVar3);
    if ((iVar4 << 0xd < 0) && (uVar5 = FUN_08012b88(*piVar3), uVar5 != 0)) {
      iVar13 = iVar15 + 0x900;
      iVar4 = *piVar3;
      uVar12 = 0;
      piVar10 = piVar3;
      do {
        if ((uVar5 & 1) != 0) {
          uVar16 = uVar12 & 0xff;
          iVar4 = FUN_08012bac(iVar4,uVar16);
          if (iVar4 << 0x1f < 0) {
            *(uint *)(iVar15 + 0x834) = *(uint *)(iVar15 + 0x834) & ~(1 << (uVar12 & 0xf));
            iVar7 = piVar3[3];
            *(undefined4 *)(iVar13 + 8) = 1;
            if (((iVar7 == 1) && (piVar10[0x12] = piVar10[0x12] + piVar10[0x11], uVar12 == 0)) &&
               (piVar3[0x13] == 0)) {
              FUN_08012c00(*piVar3,1,piVar3 + 0x131);
            }
            FUN_080099b8(piVar3,uVar16);
          }
          if (iVar4 << 0x1c < 0) {
            *(undefined4 *)(iVar13 + 8) = 8;
          }
          if (iVar4 << 0x1b < 0) {
            *(undefined4 *)(iVar13 + 8) = 0x10;
          }
          if (iVar4 << 0x19 < 0) {
            *(undefined4 *)(iVar13 + 8) = 0x40;
          }
          if (iVar4 << 0x1e < 0) {
            FUN_080125b4(iVar15,uVar12);
            if (*(char *)((int)piVar10 + 0x3f) == '\x01') {
              *(undefined *)((int)piVar10 + 0x3f) = 0;
              FUN_08009a38(piVar3,uVar16);
            }
            *(undefined4 *)(iVar13 + 8) = 2;
          }
          if (iVar4 << 0x18 < 0) {
            uVar14 = piVar10[0x13];
            uVar11 = piVar10[0x14];
            iVar7 = *piVar3;
            iVar4 = iVar7;
            if (uVar11 <= uVar14) {
              iVar9 = iVar7 + 0x900 + uVar12 * 0x20;
              uVar18 = uVar14 - uVar11;
              if ((uint)piVar10[0x11] <= uVar14 - uVar11) {
                uVar18 = piVar10[0x11];
              }
              if ((*(uint *)(iVar9 + 0x18) & 0xffff) < uVar18 + 3 >> 2) {
LAB_0800edb0:
                if (uVar11 < uVar14) goto LAB_0800ea6a;
              }
              else {
                while (uVar11 < uVar14) {
                  uVar18 = uVar14 - uVar11;
                  if ((uint)piVar10[0x11] <= uVar14 - uVar11) {
                    uVar18 = piVar10[0x11];
                  }
                  FUN_08012a08(iVar7,piVar10[0x12],uVar16,uVar18 & 0xffff,*(undefined *)(piVar3 + 3)
                              );
                  uVar14 = *(uint *)(iVar9 + 0x18);
                  piVar10[0x12] = piVar10[0x12] + uVar18;
                  uVar11 = piVar10[0x14] + uVar18;
                  piVar10[0x14] = uVar11;
                  if ((uVar14 & 0xffff) < uVar18 + 3 >> 2) {
                    iVar4 = *piVar3;
                    uVar14 = piVar10[0x13];
                    goto LAB_0800edb0;
                  }
                  uVar14 = piVar10[0x13];
                }
                iVar4 = *piVar3;
              }
              *(uint *)(iVar7 + 0x834) = *(uint *)(iVar7 + 0x834) & ~(1 << (uVar12 & 0xf));
            }
          }
          else {
            iVar4 = *piVar3;
          }
        }
LAB_0800ea6a:
        uVar5 = uVar5 >> 1;
        uVar12 = uVar12 + 1;
        piVar10 = piVar10 + 9;
        iVar13 = iVar13 + 0x20;
      } while (uVar5 != 0);
    }
    else {
      iVar4 = *piVar3;
    }
    iVar4 = FUN_08012b60(iVar4);
    if (iVar4 < 0) {
      *(uint *)(iVar15 + 0x804) = *(uint *)(iVar15 + 0x804) & 0xfffffffe;
      if (*(char *)(piVar3 + 0x13d) == '\x01') {
        *(undefined *)(piVar3 + 0x13d) = 0;
        FUN_0800f080(piVar3);
      }
      else {
        FUN_08009a28(piVar3);
      }
      iVar4 = *piVar3;
      *(uint *)(iVar4 + 0x14) = *(uint *)(iVar4 + 0x14) & 0x80000000;
    }
    else {
      iVar4 = *piVar3;
    }
    iVar4 = FUN_08012b60(iVar4);
    if (iVar4 << 0x14 < 0) {
      if (*(int *)(iVar15 + 0x808) << 0x1f < 0) {
        FUN_080099f8(piVar3);
      }
      iVar4 = *piVar3;
      *(uint *)(iVar4 + 0x14) = *(uint *)(iVar4 + 0x14) & 0x800;
    }
    else {
      iVar4 = *piVar3;
    }
    iVar4 = FUN_08012b60(iVar4);
    if (iVar4 << 4 < 0) {
      iVar4 = *piVar3;
      *(uint *)(iVar4 + 0x14) = *(uint *)(iVar4 + 0x14) & 0x8000000;
      if (*(char *)(piVar3 + 0x13d) == '\0') {
        *(undefined *)(piVar3 + 0x13d) = 1;
        piVar3[0x13e] = (uint)(*(int *)(iVar4 + 0x54) << 0x1a) >> 0x1c;
        FUN_0800f080(piVar3);
      }
      else {
        FUN_080099f8(piVar3);
      }
    }
    iVar4 = FUN_08012b60(*piVar3);
    if (iVar4 << 0x13 < 0) {
      iVar4 = *piVar3;
      *(uint *)(iVar15 + 0x804) = *(uint *)(iVar15 + 0x804) & 0xfffffffe;
      FUN_080125b4(iVar4,0x10);
      iVar4 = piVar3[1];
      if (iVar4 != 0) {
        iVar13 = 0;
        puVar8 = (uint *)(iVar15 + 0x900);
        do {
          puVar8[2] = 0xfb7f;
          iVar13 = iVar13 + 1;
          *puVar8 = *puVar8 & 0xffdfffff;
          puVar8[0x82] = 0xfb7f;
          puVar8[0x80] = puVar8[0x80] & 0xffdfffff;
          puVar8[0x80] = puVar8[0x80] | 0x8000000;
          puVar8 = puVar8 + 8;
        } while (iVar13 != iVar4);
      }
      iVar4 = piVar3[0xc];
      *(uint *)(iVar15 + 0x81c) = *(uint *)(iVar15 + 0x81c) | 0x10001;
      if (iVar4 == 0) {
        *(uint *)(iVar15 + 0x814) = *(uint *)(iVar15 + 0x814) | 0x202b;
        *(uint *)(iVar15 + 0x810) = *(uint *)(iVar15 + 0x810) | 0xb;
      }
      else {
        *(uint *)(iVar15 + 0x884) = *(uint *)(iVar15 + 0x884) | 0xb;
        *(uint *)(iVar15 + 0x844) = *(uint *)(iVar15 + 0x844) | 0xb;
      }
      uVar2 = *(undefined *)(piVar3 + 3);
      iVar4 = *piVar3;
      *(uint *)(iVar15 + 0x800) = *(uint *)(iVar15 + 0x800) & 0xfffff80f;
      FUN_08012c00(iVar4,uVar2,piVar3 + 0x131);
      iVar4 = *piVar3;
      *(uint *)(iVar4 + 0x14) = *(uint *)(iVar4 + 0x14) & 0x1000;
    }
    else {
      iVar4 = *piVar3;
    }
    uVar5 = FUN_08012b60(iVar4);
    if ((uVar5 & 0x2000) != 0) {
      FUN_08012bd4(*piVar3);
      iVar4 = FUN_08012658(*piVar3);
      iVar13 = *piVar3;
      piVar3[4] = iVar4;
      uVar6 = FUN_08010388();
      FUN_080124c0(iVar13,uVar6,*(undefined *)(piVar3 + 4));
      FUN_080099d4(piVar3);
      *(uint *)(*piVar3 + 0x14) = *(uint *)(*piVar3 + 0x14) & 0x2000;
    }
    iVar4 = FUN_08012b60();
    if (iVar4 << 0x1c < 0) {
      FUN_080099cc(piVar3);
      iVar4 = *piVar3;
      *(uint *)(iVar4 + 0x14) = *(uint *)(iVar4 + 0x14) & 8;
    }
    else {
      iVar4 = *piVar3;
    }
    iVar4 = FUN_08012b60(iVar4);
    if (iVar4 << 0x18 < 0) {
      uVar5 = piVar3[1];
      *(uint *)(iVar15 + 0x18) = *(uint *)(iVar15 + 0x18) & 0xffffff7f;
      if (1 < uVar5) {
        uVar12 = 1;
        piVar10 = piVar3;
        do {
          pcVar1 = (char *)((int)piVar10 + 0x2a3);
          piVar10 = piVar10 + 9;
          if (*pcVar1 == '\x01') {
            FUN_08012980(*piVar3,piVar3 + uVar12 * 9 + 0x9f);
            uVar5 = piVar3[1];
          }
          uVar12 = uVar12 + 1;
        } while (uVar12 < uVar5);
      }
    }
    iVar4 = FUN_08012b60(*piVar3);
    if (iVar4 << 0xb < 0) {
      uVar5 = piVar3[1];
      if (1 < uVar5) {
        uVar12 = 1;
        piVar10 = piVar3;
        piVar17 = (int *)(iVar15 + 0x920);
        do {
          if ((*(char *)(piVar10 + 0x19) == '\x01') && (*piVar17 < 0)) {
            *(undefined *)((int)piVar10 + 99) = 1;
            FUN_08012980(*piVar3,piVar3 + (short)((ushort)uVar12 & 0xf) * 9 + 0xf);
            uVar5 = piVar3[1];
          }
          uVar12 = uVar12 + 1;
          piVar10 = piVar10 + 9;
          piVar17 = piVar17 + 8;
        } while (uVar12 < uVar5);
      }
      iVar4 = *piVar3;
      *(uint *)(iVar4 + 0x14) = *(uint *)(iVar4 + 0x14) & 0x100000;
    }
    else {
      iVar4 = *piVar3;
    }
    iVar4 = FUN_08012b60(iVar4);
    if (iVar4 << 10 < 0) {
      uVar5 = piVar3[1];
      if (1 < uVar5) {
        puVar8 = (uint *)(iVar15 + 0xb20);
        uVar12 = 1;
        piVar10 = piVar3;
        do {
          uVar12 = uVar12 + 1;
          uVar16 = *puVar8;
          puVar8 = puVar8 + 8;
          if (((*(char *)(piVar10 + 0xa9) == '\x01') && ((int)uVar16 < 0)) &&
             ((uVar16 & 0x10000) == (piVar3[0x13f] & 1U))) {
            *(undefined *)((int)piVar10 + 0x2a3) = 1;
            *(uint *)(iVar15 + 0x18) = *(uint *)(iVar15 + 0x18) | 0x80;
            if (-1 < *(int *)(iVar15 + 0x14) << 0x18) {
              *(uint *)(iVar15 + 0x804) = *(uint *)(iVar15 + 0x804) | 0x200;
              break;
            }
          }
          piVar10 = piVar10 + 9;
        } while (uVar12 != uVar5);
      }
      iVar4 = *piVar3;
      *(uint *)(iVar4 + 0x14) = *(uint *)(iVar4 + 0x14) & 0x200000;
    }
    else {
      iVar4 = *piVar3;
    }
    iVar4 = FUN_08012b60(iVar4);
    if (iVar4 << 1 < 0) {
      FUN_08009a40(piVar3);
      iVar4 = *piVar3;
      *(uint *)(iVar4 + 0x14) = *(uint *)(iVar4 + 0x14) & 0x40000000;
    }
    else {
      iVar4 = *piVar3;
    }
    iVar4 = FUN_08012b60(iVar4);
    if (iVar4 << 0x1d < 0) {
      iVar4 = *piVar3;
      uVar5 = *(uint *)(iVar4 + 4);
      if ((int)(uVar5 << 0x1d) < 0) {
        FUN_08009a48(piVar3);
        iVar4 = *piVar3;
      }
      *(uint *)(iVar4 + 4) = *(uint *)(iVar4 + 4) | uVar5;
    }
  }
  return;
}


