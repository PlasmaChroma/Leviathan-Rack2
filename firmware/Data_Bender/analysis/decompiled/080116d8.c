/* 080116d8 FUN_080116d8; analyst naming is provisional. */

uint FUN_080116d8(int param_1,uint param_2)

{
  uint uVar1;
  uint uVar2;
  uint local_14;
  uint local_10;
  uint local_c;
  
  if ((param_1 - 0x100U | param_2) == 0) {
    uVar2 = DAT_080119ac;
    switch(DAT_080119a4[0x14] & 7) {
    case 0:
LAB_080118a4:
      uVar2 = *DAT_080119a4;
      goto joined_r0x080118aa;
    case 1:
switchD_080117fc_caseD_1:
      uVar2 = *DAT_080119a4;
      goto joined_r0x08011886;
    case 2:
      uVar1 = *DAT_080119a4;
      break;
    case 3:
      goto LAB_080117e8;
    case 4:
LAB_0801177c:
      uVar2 = DAT_080119a4[0x13] & 0x30000000;
      if (((int)(*DAT_080119a4 << 0x1d) < 0) && (uVar2 == 0)) {
LAB_08011910:
        return DAT_080119b0 >> ((*DAT_080119a4 << 0x1b) >> 0x1e);
      }
LAB_0801178e:
      if (((int)(*DAT_080119a4 << 0x17) < 0) && (uVar2 == 0x10000000)) {
        return DAT_080119b4;
      }
      if (((int)(*DAT_080119a4 << 0xe) < 0) && (uVar2 == 0x20000000)) {
        return DAT_080119a8;
      }
switchD_080117fc_caseD_5:
      return 0;
    default:
      goto switchD_080117fc_caseD_5;
    }
  }
  else {
    if ((param_1 - 0x200U | param_2) == 0) {
      uVar2 = DAT_080119a4[0x14] & 0x1c0;
      if (uVar2 != 0x80) {
        if (0x80 < uVar2) {
          if (uVar2 == 0xc0) {
            return DAT_080119ac;
          }
          if (uVar2 != 0x100) {
            return 0;
          }
          goto LAB_0801177c;
        }
        if (uVar2 != 0) {
          if (uVar2 != 0x40) {
            return 0;
          }
LAB_080117bc:
          uVar2 = *DAT_080119a4;
joined_r0x08011886:
          if ((uVar2 & 0x8000000) != 0) {
            FUN_08011254(&local_14);
            return local_14;
          }
          return 0;
        }
LAB_08011852:
        uVar2 = *DAT_080119a4;
joined_r0x080118aa:
        if ((uVar2 & 0x2000000) != 0) {
          FUN_0801154c(&local_14);
          return local_10;
        }
        return 0;
      }
    }
    else if ((param_1 - 0x400U | param_2) == 0) {
      uVar2 = DAT_080119a4[0x16] & 0xe00000;
      if (uVar2 != 0x400000) {
        if (0x400000 < uVar2) {
          if (uVar2 == 0x600000) {
            return DAT_080119ac;
          }
          if (uVar2 != 0x800000) {
            return 0;
          }
          goto LAB_0801177c;
        }
        if (uVar2 != 0) {
          if (uVar2 != 0x200000) {
            return 0;
          }
          goto LAB_080117bc;
        }
        goto LAB_08011852;
      }
    }
    else if ((param_1 - 0x800U | param_2) == 0) {
      uVar2 = DAT_080119a4[0x16] & 0x7000000;
      if (uVar2 != 0x2000000) {
        if (uVar2 < 0x2000001) {
          if (uVar2 != 0) {
            if (uVar2 != 0x1000000) {
              return 0;
            }
            goto LAB_080117bc;
          }
          goto LAB_08011852;
        }
        if (uVar2 == 0x3000000) {
          return DAT_080119ac;
        }
        if (uVar2 != 0x4000000) {
          return 0;
        }
LAB_08011830:
        uVar2 = DAT_080119a4[0x13] & 0x30000000;
        if (((int)(*DAT_080119a4 << 0x1d) < 0) && (uVar2 == 0)) {
          return DAT_080119b0 >> ((*DAT_080119a4 << 0x1b) >> 0x1e);
        }
        goto LAB_0801178e;
      }
    }
    else {
      if ((param_1 - 0x1000U | param_2) != 0) {
        if ((param_1 - 0x2000U | param_2) == 0) {
          uVar2 = DAT_080119a4[0x14] & 0x70000;
          if (uVar2 == 0x30000) {
            if ((*DAT_080119a4 & 4) == 0) {
              return 0;
            }
            goto LAB_08011910;
          }
          if (uVar2 < 0x30001) {
            if (uVar2 != 0x10000) {
              if (uVar2 != 0x20000) {
                if (uVar2 == 0) {
                  uVar2 = FUN_08010408();
                  return uVar2;
                }
                return 0;
              }
LAB_08011954:
              if ((*DAT_080119a4 & 0x20000000) != 0) {
                FUN_080113d0(&local_14);
                return local_10;
              }
              return 0;
            }
LAB_08011a00:
            uVar2 = *DAT_080119a4;
joined_r0x08011a56:
            if ((uVar2 & 0x8000000) != 0) {
              FUN_08011254(&local_14);
              return local_10;
            }
            return 0;
          }
          if (uVar2 == 0x40000) {
            uVar2 = *DAT_080119a4;
joined_r0x08011a30:
            if ((uVar2 & 0x100) != 0) {
              return DAT_080119b4;
            }
            return 0;
          }
          if (uVar2 != 0x50000) {
            return 0;
          }
        }
        else {
          if ((param_1 - 0x80000U | param_2) == 0) {
            uVar2 = DAT_080119a4[0x16] & 0x30000;
            if (uVar2 == 0x10000) {
              if ((*DAT_080119a4 & 0x20000000) != 0) {
                FUN_080113d0(&local_14);
                return local_c;
              }
              return 0;
            }
            if (uVar2 != 0x20000) {
              if (uVar2 != 0) {
                return 0;
              }
              goto switchD_080117fc_caseD_1;
            }
            goto LAB_08011830;
          }
          if ((param_1 - 0x10000U | param_2) == 0) {
            if ((int)(DAT_080119a4[0x13] << 0xf) < 0) {
              if ((*DAT_080119a4 & 0x8000000) != 0) {
                FUN_08011254(&local_14);
                return local_c;
              }
              return 0;
            }
            goto LAB_080118a4;
          }
          if ((param_1 - 0x4000U | param_2) != 0) {
            if ((param_1 - 0x8000U | param_2) != 0) {
              return 0;
            }
            uVar2 = DAT_080119a4[0x14] & 0x30000000;
            if (uVar2 == 0x10000000) goto LAB_08011852;
            if (uVar2 != 0x20000000) {
              if (uVar2 != 0) {
                return 0;
              }
              uVar2 = *DAT_080119a4;
              goto joined_r0x0801175c;
            }
            goto LAB_08011a00;
          }
          uVar2 = DAT_08011a5c[0x16] & 0x70000000;
          if (uVar2 == 0x30000000) {
            if ((*DAT_08011a5c & 4) != 0) {
              return DAT_08011a64 >> ((*DAT_08011a5c << 0x1b) >> 0x1e);
            }
            return 0;
          }
          if (uVar2 < 0x30000001) {
            if (uVar2 != 0x10000000) {
              if (uVar2 != 0x20000000) {
                if (uVar2 != 0) {
                  return 0;
                }
                uVar2 = FUN_08010388();
                return uVar2 >> (*(byte *)(DAT_08011a60 + ((DAT_08011a5c[8] << 0x19) >> 0x1d)) &
                                0x1f);
              }
              goto LAB_08011954;
            }
            uVar2 = *DAT_08011a5c;
            goto joined_r0x08011a56;
          }
          if (uVar2 == 0x40000000) {
            uVar2 = *DAT_08011a5c;
            goto joined_r0x08011a30;
          }
          if (uVar2 != 0x50000000) {
            return 0;
          }
        }
        uVar2 = *DAT_080119a4;
joined_r0x0801175c:
        if ((uVar2 & 0x20000) != 0) {
          return DAT_080119a8;
        }
        return 0;
      }
      uVar2 = DAT_080119a4[0x14] & 0x7000;
      if (uVar2 != 0x2000) {
        if (uVar2 < 0x2001) {
          if (uVar2 != 0) {
            if (uVar2 != 0x1000) {
              return 0;
            }
            goto LAB_080117bc;
          }
          goto LAB_08011852;
        }
        if (uVar2 == 0x3000) {
          return DAT_080119ac;
        }
        if (uVar2 != 0x4000) {
          return 0;
        }
        goto LAB_08011830;
      }
    }
    uVar1 = *DAT_080119a4;
  }
  uVar2 = uVar1 & 0x20000000;
  if ((uVar1 & 0x20000000) != 0) {
    FUN_080113d0(&local_14);
    uVar2 = local_14;
  }
LAB_080117e8:
  return uVar2;
}


