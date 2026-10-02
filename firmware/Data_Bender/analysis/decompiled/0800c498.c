/* 0800c498 FUN_0800c498; analyst naming is provisional. */

void FUN_0800c498(int *param_1)

{
  char cVar1;
  byte bVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  undefined4 uVar6;
  int iVar7;
  int *piVar8;
  int iVar9;
  uint uVar10;
  int iVar11;
  uint uVar12;
  uint local_2c;
  
  iVar7 = *param_1;
  iVar3 = FUN_08012bcc(iVar7);
  if ((iVar3 == 1) && (iVar3 = FUN_08012b60(*param_1), iVar3 != 0)) {
    uVar4 = FUN_08012b60(*param_1);
    if ((uVar4 & 0x200000) != 0) {
      *(undefined4 *)(*param_1 + 0x14) = 0x200000;
    }
    uVar4 = FUN_08012b60();
    if ((uVar4 & 0x100000) != 0) {
      *(undefined4 *)(*param_1 + 0x14) = 0x100000;
    }
    uVar4 = FUN_08012b60();
    if ((uVar4 & 0x4000000) != 0) {
      *(undefined4 *)(*param_1 + 0x14) = 0x4000000;
    }
    uVar4 = FUN_08012b60();
    if ((uVar4 & 2) != 0) {
      *(undefined4 *)(*param_1 + 0x14) = 2;
    }
    uVar4 = FUN_08012b60();
    iVar3 = *param_1;
    if (((uVar4 & 0x20000000) != 0) &&
       (*(undefined4 *)(iVar3 + 0x14) = 0x20000000, -1 < *(int *)(iVar7 + 0x440) << 0x1f)) {
      FUN_080125b4(iVar7,0x10);
      FUN_08012608(iVar7);
      if (param_1[6] == 2) {
        FUN_08012c58(*param_1,1);
      }
      FUN_08009b38(param_1);
      iVar3 = *param_1;
    }
    iVar3 = FUN_08012b60(iVar3);
    if (iVar3 << 7 < 0) {
      iVar3 = *param_1;
      uVar4 = *(uint *)(iVar3 + 0x440);
      local_2c = *(uint *)(iVar3 + 0x440) & 0xffffffd1;
      if ((int)(uVar4 << 0x1e) < 0) {
        if ((int)(uVar4 << 0x1f) < 0) {
          FUN_08009b30(param_1);
        }
        local_2c = local_2c | 2;
      }
      if ((int)(uVar4 << 0x1c) < 0) {
        local_2c = local_2c | 8;
        if ((int)(uVar4 << 0x1d) < 0) {
          if (param_1[6] == 2) {
            uVar6 = 2;
            if ((uVar4 & 0x60000) != 0x40000) {
              uVar6 = 1;
            }
            FUN_08012c58(*param_1,uVar6);
          }
          else if (param_1[4] == 1) {
            *(undefined4 *)(iVar3 + 0x404) = 60000;
          }
          FUN_08009b44(param_1);
        }
        else {
          FUN_08009b4c(param_1);
        }
      }
      if ((int)(uVar4 << 0x1a) < 0) {
        local_2c = local_2c | 0x20;
      }
      *(uint *)(iVar3 + 0x440) = local_2c;
    }
    iVar3 = FUN_08012b60(*param_1);
    if (iVar3 << 0x1c < 0) {
      FUN_08009b28(param_1);
      iVar3 = *param_1;
      *(undefined4 *)(iVar3 + 0x14) = 8;
    }
    else {
      iVar3 = *param_1;
    }
    iVar3 = FUN_08012b60(iVar3);
    iVar9 = *param_1;
    if (iVar3 << 0x1b < 0) {
      *(uint *)(iVar9 + 0x18) = *(uint *)(iVar9 + 0x18) & 0xffffffef;
      uVar10 = *(uint *)(iVar9 + 0x20);
      uVar4 = (uVar10 << 0x11) >> 0x15;
      if (((uVar10 << 0xb) >> 0x1c == 2) && (uVar4 != 0)) {
        uVar10 = uVar10 & 0xf;
        if (param_1[uVar10 * 0xb + 0x11] != 0) {
          if ((uint)param_1[uVar10 * 0xb + 0x13] < param_1[uVar10 * 0xb + 0x14] + uVar4) {
            *(undefined *)(param_1 + uVar10 * 0xb + 0x18) = 4;
          }
          else {
            iVar11 = iVar9 + 0x500;
            FUN_08012a34(iVar9);
            iVar3 = *(int *)(iVar11 + uVar10 * 0x20 + 0x10);
            param_1[uVar10 * 0xb + 0x11] = param_1[uVar10 * 0xb + 0x11] + uVar4;
            param_1[uVar10 * 0xb + 0x14] = param_1[uVar10 * 0xb + 0x14] + uVar4;
            if (((uint)(iVar3 << 3) >> 0x16 != 0) &&
               (*(ushort *)(param_1 + uVar10 * 0xb + 0x10) == uVar4)) {
              *(uint *)(uVar10 * 0x20 + iVar11) =
                   *(uint *)(uVar10 * 0x20 + iVar11) & 0xbfffffff | 0x80000000;
              *(byte *)(param_1 + uVar10 * 0xb + 0x15) =
                   *(byte *)(param_1 + uVar10 * 0xb + 0x15) ^ 1;
            }
            iVar9 = *param_1;
          }
        }
      }
      *(uint *)(iVar9 + 0x18) = *(uint *)(iVar9 + 0x18) | 0x10;
    }
    iVar3 = FUN_08012b60(iVar9);
    if (iVar3 << 6 < 0) {
      uVar4 = FUN_08012c9c(*param_1);
      if (param_1[2] != 0) {
        uVar10 = 0;
        piVar8 = (int *)(iVar7 + 0x500);
        do {
          if (-1 < (int)((uVar4 >> (uVar10 & 0xf)) << 0x1f)) goto LAB_0800c5a8;
          uVar12 = uVar10 & 0xff;
          iVar3 = *param_1;
          if (*piVar8 << 0x10 < 0) {
            iVar7 = FUN_08012b68(iVar3,uVar12);
            if (iVar7 << 0x1d < 0) {
              uVar6 = 4;
              iVar7 = iVar3 + 0x500 + uVar12 * 0x20;
LAB_0800c56c:
              *(undefined4 *)(iVar7 + 8) = uVar6;
              *(undefined *)((int)param_1 + uVar12 * 0x2c + 0x61) = 7;
              FUN_08012ca4(*param_1,uVar12);
            }
            else {
              iVar7 = FUN_08012b68(*param_1,uVar12);
              if (iVar7 << 0x17 < 0) {
                *(undefined4 *)(iVar3 + uVar12 * 0x20 + 0x508) = 0x100;
                *(undefined *)((int)param_1 + uVar12 * 0x2c + 0x61) = 8;
                FUN_08012ca4(*param_1,uVar12);
              }
              else {
                iVar7 = FUN_08012b68(*param_1,uVar12);
                if (iVar7 << 0x1c < 0) {
                  *(undefined4 *)(iVar3 + uVar12 * 0x20 + 0x508) = 8;
                  *(undefined *)((int)param_1 + uVar12 * 0x2c + 0x61) = 6;
                  FUN_08012ca4(*param_1,uVar12);
                }
                else {
                  iVar7 = FUN_08012b68(*param_1,uVar12);
                  if (iVar7 << 0x15 < 0) {
                    *(undefined4 *)(iVar3 + uVar12 * 0x20 + 0x508) = 0x400;
                    *(undefined *)((int)param_1 + uVar12 * 0x2c + 0x61) = 9;
                    FUN_08012ca4(*param_1,uVar12);
                  }
                  else {
                    iVar7 = FUN_08012b68(*param_1,uVar12);
                    if (iVar7 << 0x18 < 0) {
                      uVar6 = 0x80;
                      iVar7 = iVar3 + 0x500 + uVar12 * 0x20;
                      goto LAB_0800c56c;
                    }
                  }
                }
              }
            }
            uVar5 = FUN_08012b68(*param_1,uVar12);
            if ((uVar5 & 0x200) == 0) {
              iVar7 = FUN_08012b68(*param_1,uVar12);
              if (iVar7 << 0x1f < 0) {
                iVar3 = iVar3 + 0x500;
                iVar7 = param_1[3];
                iVar9 = iVar3 + uVar12 * 0x20;
                *(undefined4 *)(iVar9 + 8) = 0x20;
                if (iVar7 != 0) {
                  param_1[uVar12 * 0xb + 0x14] =
                       param_1[uVar12 * 0xb + 0x12] - (*(uint *)(iVar9 + 0x10) & 0x7ffff);
                }
                param_1[uVar12 * 0xb + 0x17] = 0;
                *(undefined *)((int)param_1 + uVar12 * 0x2c + 0x61) = 1;
                *(undefined4 *)(iVar9 + 8) = 1;
                bVar2 = *(byte *)((int)param_1 + uVar12 * 0x2c + 0x3f);
                if ((bVar2 & 0xfd) == 0) {
                  FUN_08012ca4(*param_1,uVar12);
                  iVar7 = param_1[3];
                  *(undefined4 *)(iVar9 + 8) = 0x10;
                }
                else if ((bVar2 & 0xfd) == 1) {
                  *(uint *)(iVar3 + uVar12 * 0x20) = *(uint *)(iVar3 + uVar12 * 0x20) | 0x20000000;
                  *(undefined *)(param_1 + uVar12 * 0xb + 0x18) = 1;
                  FUN_08009b40(param_1,uVar12);
                  iVar7 = param_1[3];
                }
                if ((iVar7 != 1) ||
                   ((int)((param_1[uVar12 * 0xb + 0x14] + -1 +
                          (uint)*(ushort *)(param_1 + uVar12 * 0xb + 0x10)) /
                          (uint)*(ushort *)(param_1 + uVar12 * 0xb + 0x10) << 0x1f) < 0)) {
                  *(byte *)(param_1 + uVar12 * 0xb + 0x15) =
                       *(byte *)(param_1 + uVar12 * 0xb + 0x15) ^ 1;
                }
              }
              else {
                iVar7 = FUN_08012b68(*param_1,uVar12);
                if (iVar7 << 0x1a < 0) {
                  *(undefined4 *)(iVar3 + uVar12 * 0x20 + 0x508) = 0x20;
                }
                else {
                  uVar5 = FUN_08012b68(*param_1,uVar12);
                  if ((uVar5 & 2) == 0) {
                    uVar5 = FUN_08012b68(*param_1,uVar12);
                    if ((uVar5 & 0x40) == 0) {
                      iVar7 = FUN_08012b68(*param_1,uVar12);
                      if (iVar7 << 0x1b < 0) {
                        bVar2 = *(byte *)((int)param_1 + uVar12 * 0x2c + 0x3f);
                        if (bVar2 == 3) {
                          param_1[uVar12 * 0xb + 0x17] = 0;
LAB_0800cb2e:
                          *(undefined *)((int)param_1 + uVar12 * 0x2c + 0x61) = 4;
                          goto LAB_0800cae8;
                        }
                        if (((bVar2 & 0xfd) == 0) &&
                           (iVar7 = param_1[3], param_1[uVar12 * 0xb + 0x17] = bVar2 & 0xfd,
                           iVar7 == 0)) goto LAB_0800cb2e;
LAB_0800caf0:
                        *(undefined4 *)(iVar3 + uVar12 * 0x20 + 0x508) = 0x10;
                      }
                    }
                    else {
                      *(undefined4 *)(iVar3 + uVar12 * 0x20 + 0x508) = 0x40;
                      param_1[uVar12 * 0xb + 0x17] = 0;
                      *(undefined *)((int)param_1 + uVar12 * 0x2c + 0x61) = 5;
                      FUN_08012ca4(*param_1,uVar12);
                    }
                  }
                  else {
                    *(undefined4 *)(iVar3 + uVar12 * 0x20 + 0x508) = 2;
                    cVar1 = *(char *)((int)param_1 + uVar12 * 0x2c + 0x61);
                    if (cVar1 == '\x01') {
                      *(undefined2 *)(param_1 + uVar12 * 0xb + 0x18) = 0x201;
                    }
                    else if (cVar1 == '\x06') {
                      *(undefined2 *)(param_1 + uVar12 * 0xb + 0x18) = 0x205;
                    }
                    else if ((cVar1 == '\a') || (cVar1 == '\t')) {
LAB_0800cb82:
                      *(undefined *)((int)param_1 + uVar12 * 0x2c + 0x61) = 2;
                      iVar7 = param_1[uVar12 * 0xb + 0x17];
                      param_1[uVar12 * 0xb + 0x17] = iVar7 + 1U;
                      if (iVar7 + 1U < 3) {
                        *(undefined *)(param_1 + uVar12 * 0xb + 0x18) = 2;
LAB_0800cbb6:
                        *(uint *)(iVar3 + 0x500 + uVar12 * 0x20) =
                             *(uint *)(iVar3 + 0x500 + uVar12 * 0x20) & 0xbfffffff | 0x80000000;
                      }
                      else {
                        param_1[uVar12 * 0xb + 0x17] = 0;
                        *(undefined *)(param_1 + uVar12 * 0xb + 0x18) = 4;
                      }
                    }
                    else if (cVar1 == '\x05') {
                      *(undefined *)((int)param_1 + uVar12 * 0x2c + 0x61) = 2;
                    }
                    else if (cVar1 == '\x03') {
                      *(undefined *)((int)param_1 + uVar12 * 0x2c + 0x61) = 2;
                    }
                    else if (cVar1 == '\x04') {
                      *(undefined2 *)(param_1 + uVar12 * 0xb + 0x18) = 0x202;
                      if ((*(byte *)((int)param_1 + uVar12 * 0x2c + 0x3f) & 0xfd) == 0)
                      goto LAB_0800cbb6;
                    }
                    else if (cVar1 == '\b') {
                      *(undefined2 *)(param_1 + uVar12 * 0xb + 0x18) = 0x204;
                      param_1[uVar12 * 0xb + 0x17] = param_1[uVar12 * 0xb + 0x17] + 1;
                    }
                    else if (cVar1 == '\x02') goto LAB_0800c5a8;
LAB_0800c9b8:
                    FUN_08009b40(param_1,uVar12,*(undefined *)(param_1 + uVar12 * 0xb + 0x18));
                  }
                }
              }
            }
            else {
              FUN_08012ca4();
              *(undefined4 *)(iVar3 + uVar12 * 0x20 + 0x508) = 0x200;
            }
          }
          else {
            uVar5 = FUN_08012b68(iVar3,uVar12);
            if ((uVar5 & 4) == 0) {
              iVar7 = FUN_08012b68(*param_1,uVar12);
              if (iVar7 << 0x1a < 0) {
                *(undefined4 *)(iVar3 + uVar12 * 0x20 + 0x508) = 0x20;
                if (*(char *)((int)param_1 + uVar12 * 0x2c + 0x3d) == '\x01') {
                  *(undefined *)((int)param_1 + uVar12 * 0x2c + 0x3d) = 0;
                  *(undefined2 *)(param_1 + uVar12 * 0xb + 0x18) = 0x302;
                  FUN_08012ca4(*param_1,uVar12);
                }
              }
              else {
                uVar5 = FUN_08012b68(*param_1,uVar12);
                if ((uVar5 & 0x200) == 0) {
                  uVar5 = FUN_08012b68(*param_1,uVar12);
                  if ((uVar5 & 1) == 0) {
                    iVar7 = FUN_08012b68(*param_1,uVar12);
                    if (iVar7 << 0x19 < 0) {
                      param_1[uVar12 * 0xb + 0x17] = 0;
                      *(undefined *)((int)param_1 + uVar12 * 0x2c + 0x61) = 5;
                      *(undefined *)((int)param_1 + uVar12 * 0x2c + 0x3d) = 1;
                      FUN_08012ca4(*param_1,uVar12);
                      *(undefined4 *)(iVar3 + uVar12 * 0x20 + 0x508) = 0x40;
                    }
                    else {
                      uVar5 = FUN_08012b68(*param_1,uVar12);
                      if ((uVar5 & 8) == 0) {
                        iVar7 = FUN_08012b68(*param_1,uVar12);
                        if (iVar7 << 0x1b < 0) {
                          param_1[uVar12 * 0xb + 0x17] = 0;
                          *(undefined *)((int)param_1 + uVar12 * 0x2c + 0x61) = 4;
                          if ((*(char *)((int)param_1 + uVar12 * 0x2c + 0x3d) == '\0') &&
                             (*(char *)(param_1 + uVar12 * 0xb + 0xf) == '\0')) {
                            *(undefined *)((int)param_1 + uVar12 * 0x2c + 0x3d) = 1;
                          }
LAB_0800cae8:
                          FUN_08012ca4(*param_1,uVar12);
                          goto LAB_0800caf0;
                        }
                        iVar7 = FUN_08012b68(*param_1,uVar12);
                        if (iVar7 << 0x18 < 0) {
                          if (param_1[3] == 0) {
                            *(undefined *)((int)param_1 + uVar12 * 0x2c + 0x61) = 7;
                            FUN_08012ca4(*param_1,uVar12);
                          }
                          else {
                            iVar7 = param_1[uVar12 * 0xb + 0x17];
                            param_1[uVar12 * 0xb + 0x17] = iVar7 + 1U;
                            if (iVar7 + 1U < 3) {
                              *(undefined *)(param_1 + uVar12 * 0xb + 0x18) = 2;
                            }
                            else {
                              param_1[uVar12 * 0xb + 0x17] = 0;
                              *(undefined *)(param_1 + uVar12 * 0xb + 0x18) = 4;
                              FUN_08009b40(param_1,uVar12);
                            }
                          }
                          *(undefined4 *)(iVar3 + uVar12 * 0x20 + 0x508) = 0x80;
                        }
                        else {
                          iVar7 = FUN_08012b68(*param_1,uVar12);
                          if (iVar7 << 0x15 < 0) {
                            *(undefined *)((int)param_1 + uVar12 * 0x2c + 0x61) = 9;
                            FUN_08012ca4(*param_1,uVar12);
                            *(undefined4 *)(iVar3 + uVar12 * 0x20 + 0x508) = 0x400;
                          }
                          else {
                            iVar7 = FUN_08012b68(*param_1,uVar12);
                            if (iVar7 << 0x1e < 0) {
                              *(undefined4 *)(iVar3 + uVar12 * 0x20 + 0x508) = 2;
                              cVar1 = *(char *)((int)param_1 + uVar12 * 0x2c + 0x61);
                              if (cVar1 == '\x01') {
                                bVar2 = *(byte *)((int)param_1 + uVar12 * 0x2c + 0x3f);
                                *(undefined2 *)(param_1 + uVar12 * 0xb + 0x18) = 0x201;
                                if ((bVar2 - 2 < 2) &&
                                   ((param_1[3] == 0 ||
                                    (((param_1[3] == 1 && (param_1[uVar12 * 0xb + 0x13] != 0)) &&
                                     ((int)((param_1[uVar12 * 0xb + 0x13] + -1 +
                                            (uint)*(ushort *)(param_1 + uVar12 * 0xb + 0x10)) /
                                            (uint)*(ushort *)(param_1 + uVar12 * 0xb + 0x10) << 0x1f
                                           ) < 0)))))) {
                                  *(byte *)((int)param_1 + uVar12 * 0x2c + 0x55) =
                                       *(byte *)((int)param_1 + uVar12 * 0x2c + 0x55) ^ 1;
                                }
                              }
                              else if (cVar1 == '\x03') {
                                *(undefined *)((int)param_1 + uVar12 * 0x2c + 0x61) = 2;
                              }
                              else if ((cVar1 == '\x04') || (cVar1 == '\x05')) {
                                *(undefined2 *)(param_1 + uVar12 * 0xb + 0x18) = 0x202;
                              }
                              else {
                                if (cVar1 != '\x06') {
                                  if ((cVar1 != '\a') && (cVar1 != '\t')) goto LAB_0800c5a8;
                                  goto LAB_0800cb82;
                                }
                                *(undefined2 *)(param_1 + uVar12 * 0xb + 0x18) = 0x205;
                              }
                              goto LAB_0800c9b8;
                            }
                          }
                        }
                      }
                      else {
                        *(undefined4 *)(iVar3 + uVar12 * 0x20 + 0x508) = 8;
                        *(undefined *)((int)param_1 + uVar12 * 0x2c + 0x61) = 6;
                        FUN_08012ca4(*param_1,uVar12);
                      }
                    }
                  }
                  else {
                    iVar7 = *param_1;
                    param_1[uVar12 * 0xb + 0x17] = 0;
                    uVar5 = FUN_08012b68(iVar7,uVar12);
                    iVar3 = iVar3 + 0x500 + uVar12 * 0x20;
                    if ((uVar5 & 0x40) != 0) {
                      *(undefined *)((int)param_1 + uVar12 * 0x2c + 0x3d) = 1;
                      *(undefined4 *)(iVar3 + 8) = 0x40;
                    }
                    *(undefined4 *)(iVar3 + 8) = 1;
                    *(undefined *)((int)param_1 + uVar12 * 0x2c + 0x61) = 1;
                    FUN_08012ca4(*param_1,uVar12);
                  }
                }
                else {
                  iVar7 = *param_1;
                  *(undefined4 *)(iVar3 + uVar12 * 0x20 + 0x508) = 0x200;
                  FUN_08012ca4(iVar7,uVar12);
                }
              }
            }
            else {
              *(undefined4 *)(iVar3 + uVar12 * 0x20 + 0x508) = 4;
              *(undefined *)((int)param_1 + uVar12 * 0x2c + 0x61) = 7;
              FUN_08012ca4(*param_1,uVar12);
            }
          }
LAB_0800c5a8:
          uVar10 = uVar10 + 1;
          piVar8 = piVar8 + 8;
        } while (uVar10 < (uint)param_1[2]);
      }
      *(undefined4 *)(*param_1 + 0x14) = 0x2000000;
    }
  }
  return;
}


