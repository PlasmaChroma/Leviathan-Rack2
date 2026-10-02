/* 08009998 FUN_08009998; analyst naming is provisional. */

uint FUN_08009998(int param_1,undefined4 param_2,undefined4 param_3,uint param_4)

{
  char cVar1;
  ushort uVar2;
  byte *pbVar3;
  int iVar4;
  uint uVar5;
  undefined4 *puVar6;
  uint *puVar7;
  byte bVar8;
  uint uVar9;
  code *UNRECOVERED_JUMPTABLE;
  int iVar10;
  byte *pbVar11;
  ushort local_1a [3];
  uint local_14;
  uint uStack_10;
  
  iVar4 = *(int *)(param_1 + 0x508);
  pbVar11 = (byte *)(iVar4 + 0x2aa);
  uStack_10 = param_4;
  FUN_08013650(pbVar11,param_1 + 0x4c4);
  bVar8 = *(byte *)(iVar4 + 0x2aa);
  puVar7 = (uint *)0x1;
  *(uint *)(iVar4 + 0x298) = (uint)*(ushort *)(iVar4 + 0x2b0);
  *(undefined4 *)(iVar4 + 0x294) = 1;
  pbVar3 = DAT_08013458;
  if ((bVar8 & 0x1f) == 1) {
    if (((*pbVar11 & 0x60) == 0x40) || (-1 < (int)((uint)*pbVar11 << 0x19))) {
      if ((2 < *(byte *)(iVar4 + 0x29c) - 1) || (1 < *(byte *)(iVar4 + 0x2ae))) {
        FUN_08009a74(iVar4,0x80);
        FUN_08009a74(iVar4);
        return 0;
      }
      iVar10 = FUN_08013104(iVar4);
      if ((iVar10 != 0) ||
         (UNRECOVERED_JUMPTABLE = *(code **)(*(int *)(iVar4 + 0x2b8) + 8),
         UNRECOVERED_JUMPTABLE == (code *)0x0)) {
        return 3;
      }
      *(undefined4 *)(iVar4 + 0x2d4) = 0;
      uVar5 = (*UNRECOVERED_JUMPTABLE)(iVar4,pbVar11);
      if ((*(short *)(iVar4 + 0x2b0) == 0) && (uVar5 == 0)) {
        FUN_080136ac(iVar4);
      }
    }
    else {
      uVar5 = 0;
      FUN_08009a74(iVar4,0x80);
      FUN_08009a74(iVar4,0);
    }
    return uVar5;
  }
  if ((bVar8 & 0x1f) != 2) {
    if ((bVar8 & 0x1f) != 0) {
      uVar5 = FUN_08009a74(iVar4,bVar8 & 0x80,1,uStack_10);
      return uVar5;
    }
    uVar5 = *pbVar11 & 0x60;
    if ((uVar5 == 0x20) || (uVar5 == 0x40)) {
                    /* WARNING: Could not recover jumptable at 0x08013152. Too many branches */
                    /* WARNING: Treating indirect jump as call */
      uVar5 = (**(code **)(*(int *)(iVar4 + (*(int *)(iVar4 + 0x2d4) + 0xae) * 4) + 8))
                        (iVar4,pbVar11);
      return uVar5;
    }
    uVar9 = uStack_10;
    if ((*pbVar11 & 0x60) == 0) {
      uVar9 = (uint)*(byte *)(iVar4 + 0x2ab);
      puVar7 = &switchD_0801315c::switchdataD_08013160;
      switch(uVar9) {
      case 0:
        if ((*(byte *)(iVar4 + 0x29c) - 1 < 3) && (*(short *)(iVar4 + 0x2b0) == 2)) {
          *(undefined4 *)(iVar4 + 0xc) = 1;
          if (*(int *)(iVar4 + 0x2a4) != 0) {
            *(undefined4 *)(iVar4 + 0xc) = 3;
          }
          FUN_08013668(iVar4,iVar4 + 0xc,2);
          return uVar9;
        }
        break;
      case 1:
        if (*(byte *)(iVar4 + 0x29c) - 1 < 3) {
          if (*(short *)(iVar4 + 0x2ac) != 1) {
            return uVar5;
          }
          uVar9 = 0;
LAB_08013200:
          *(uint *)(iVar4 + 0x2a4) = uVar9;
          FUN_080136ac(iVar4);
          return uVar5;
        }
        break;
      default:
        goto switchD_0801315c_caseD_2;
      case 3:
        uVar9 = (uint)*(ushort *)(iVar4 + 0x2ac);
        if (uVar9 == 1) goto LAB_08013200;
        if (uVar9 == 2) {
          *(char *)(iVar4 + 0x2a0) = (char)((ushort)*(undefined2 *)(iVar4 + 0x2ae) >> 8);
          FUN_080136ac();
          return uVar5;
        }
        break;
      case 5:
        if (((*(short *)(iVar4 + 0x2ae) == 0) && (*(short *)(iVar4 + 0x2b0) == 0)) &&
           ((uVar2 = *(ushort *)(iVar4 + 0x2ac), uVar2 < 0x80 &&
            (*(char *)(iVar4 + 0x29c) != '\x03')))) {
          *(char *)(iVar4 + 0x29e) = (char)uVar2;
          FUN_08009ad4();
          FUN_080136ac(iVar4);
          if (uVar2 == 0) {
            *(undefined *)(iVar4 + 0x29c) = 1;
            return uVar5;
          }
LAB_0801341e:
          *(undefined *)(iVar4 + 0x29c) = 2;
          return uVar5;
        }
        break;
      case 6:
        local_1a[0] = 0;
        switch(*(ushort *)(iVar4 + 0x2ac) >> 8) {
        case 1:
          iVar10 = (***(code ***)(iVar4 + 0x2b4))(*(undefined *)(iVar4 + 0x10),local_1a);
          break;
        case 2:
          if (*(char *)(iVar4 + 0x10) == '\0') {
            iVar10 = (**(code **)(*(int *)(iVar4 + 0x2b8) + 0x28))(local_1a);
            *(undefined *)(iVar10 + 1) = 2;
          }
          else {
            iVar10 = (**(code **)(*(int *)(iVar4 + 0x2b8) + 0x2c))(local_1a);
            *(undefined *)(iVar10 + 1) = 2;
          }
          break;
        case 3:
          switch((char)*(ushort *)(iVar4 + 0x2ac)) {
          case '\0':
            UNRECOVERED_JUMPTABLE = *(code **)(*(int *)(iVar4 + 0x2b4) + 4);
            break;
          case '\x01':
            UNRECOVERED_JUMPTABLE = *(code **)(*(int *)(iVar4 + 0x2b4) + 8);
            break;
          case '\x02':
            UNRECOVERED_JUMPTABLE = *(code **)(*(int *)(iVar4 + 0x2b4) + 0xc);
            break;
          case '\x03':
            UNRECOVERED_JUMPTABLE = *(code **)(*(int *)(iVar4 + 0x2b4) + 0x10);
            break;
          case '\x04':
            UNRECOVERED_JUMPTABLE = *(code **)(*(int *)(iVar4 + 0x2b4) + 0x14);
            break;
          case '\x05':
            UNRECOVERED_JUMPTABLE = *(code **)(*(int *)(iVar4 + 0x2b4) + 0x18);
            break;
          default:
            goto switchD_08013278_caseD_4;
          }
          if (UNRECOVERED_JUMPTABLE == (code *)0x0) goto switchD_08013278_caseD_4;
          iVar10 = (*UNRECOVERED_JUMPTABLE)(*(undefined *)(iVar4 + 0x10),local_1a);
          break;
        default:
          goto switchD_08013278_caseD_4;
        case 6:
          if (*(char *)(iVar4 + 0x10) != '\0') goto switchD_08013278_caseD_4;
          iVar10 = (**(code **)(*(int *)(iVar4 + 0x2b8) + 0x34))(local_1a);
          break;
        case 7:
          if (*(char *)(iVar4 + 0x10) != '\0') goto switchD_08013278_caseD_4;
          iVar10 = (**(code **)(*(int *)(iVar4 + 0x2b8) + 0x30))(local_1a);
          *(undefined *)(iVar10 + 1) = 7;
        }
        uVar2 = *(ushort *)(iVar4 + 0x2b0);
        if (uVar2 == 0) goto LAB_080133a6;
        if (local_1a[0] != 0) {
          if (local_1a[0] <= uVar2) {
            uVar2 = local_1a[0];
          }
          local_1a[0] = uVar2;
          FUN_08013668(iVar4,iVar10);
          return uVar5;
        }
        break;
      case 8:
        if (*(short *)(iVar4 + 0x2b0) == 1) {
          uVar9 = (uint)*(byte *)(iVar4 + 0x29c);
          puVar7 = (uint *)0x1;
          if (*(byte *)(iVar4 + 0x29c) < 3) {
            if (uVar9 != 0) {
              *(undefined4 *)(iVar4 + 8) = 0;
              FUN_08013668(iVar4,(undefined4 *)(iVar4 + 8));
              return uVar5;
            }
          }
          else if (uVar9 == 3) {
            FUN_08013668(iVar4,iVar4 + 4);
            return uVar5;
          }
          goto switchD_0801315c_caseD_2;
        }
        break;
      case 9:
        bVar8 = *(byte *)(iVar4 + 0x2ac);
        *DAT_08013458 = bVar8;
        if (1 < bVar8) {
          FUN_08009a74(iVar4,0x80);
          FUN_08009a74(iVar4,0);
          return 3;
        }
        if (*(char *)(iVar4 + 0x29c) == '\x02') {
          if (bVar8 != 0) {
            *(undefined4 *)(iVar4 + 4) = 1;
            uVar5 = FUN_08012e10();
            if (uVar5 != 0) {
              FUN_08009a74(iVar4,0x80);
              FUN_08009a74(iVar4,0);
              *(undefined *)(iVar4 + 0x29c) = 2;
              return uVar5;
            }
            FUN_080136ac(iVar4);
            *(undefined *)(iVar4 + 0x29c) = 3;
            return 0;
          }
        }
        else {
          if (*(char *)(iVar4 + 0x29c) != '\x03') {
            FUN_08009a74(iVar4,0x80);
            FUN_08009a74(iVar4,0);
            FUN_08012e20(iVar4,*pbVar3);
            return 3;
          }
          if (bVar8 == 0) {
            *(undefined4 *)(iVar4 + 4) = 0;
            *(undefined *)(iVar4 + 0x29c) = 2;
            FUN_08012e20();
            uVar5 = 0;
          }
          else if (*(uint *)(iVar4 + 4) != 1) {
            FUN_08012e20(iVar4,*(uint *)(iVar4 + 4) & 0xff);
            *(uint *)(iVar4 + 4) = (uint)*pbVar3;
            uVar9 = FUN_08012e10(iVar4);
            if (uVar9 != 0) {
              FUN_08009a74(iVar4,0x80);
              FUN_08009a74(iVar4,0);
              FUN_08012e20(iVar4,*(undefined *)(iVar4 + 4));
              uVar5 = uVar9;
              goto LAB_0801341e;
            }
          }
        }
LAB_080133a6:
        FUN_080136ac(iVar4);
        return uVar5;
      }
switchD_08013278_caseD_4:
      FUN_08009a74(iVar4,0x80);
      FUN_08009a74(iVar4,0);
      return uVar5;
    }
switchD_0801315c_caseD_2:
    FUN_08009a74(iVar4,0x80,puVar7,uVar9);
    FUN_08009a74(iVar4);
    return 0;
  }
  uVar2 = *(ushort *)(iVar4 + 0x2ae);
  uVar5 = (uint)(byte)uVar2;
  bVar8 = *pbVar11 & 0x60;
  if ((bVar8 == 0x20) || (bVar8 == 0x40)) {
LAB_08013544:
    iVar10 = FUN_08013108(iVar4,uVar5);
    if (iVar10 != 0) {
      return 0;
    }
    *(undefined4 *)(iVar4 + 0x2d4) = 0;
    UNRECOVERED_JUMPTABLE = *(code **)(*(int *)(iVar4 + 0x2b8) + 8);
    if (UNRECOVERED_JUMPTABLE == (code *)0x0) {
      return 0;
    }
                    /* WARNING: Could not recover jumptable at 0x08013566. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    uVar5 = (*UNRECOVERED_JUMPTABLE)(iVar4,pbVar11);
    return uVar5;
  }
  if ((*pbVar11 & 0x60) != 0) goto LAB_0801350c;
  cVar1 = *(char *)(iVar4 + 0x2ab);
  if (cVar1 == '\x01') {
    if (*(char *)(iVar4 + 0x29c) != '\x02') {
      if (*(char *)(iVar4 + 0x29c) == '\x03') {
        if (*(short *)(iVar4 + 0x2ac) != 0) {
          return 0;
        }
        local_14 = uVar5;
        if ((uVar2 & 0x7f) != 0) {
          FUN_08009a90();
        }
        FUN_080136ac(iVar4);
        uVar5 = local_14;
        goto LAB_08013544;
      }
      goto LAB_0801350c;
    }
  }
  else {
    if (cVar1 != '\x03') {
      if (cVar1 == '\0') {
        if (*(char *)(iVar4 + 0x29c) == '\x02') {
          if ((uVar2 & 0x7f) == 0) {
            if ((int)((uint)uVar2 << 0x18) < 0) {
              puVar6 = (undefined4 *)(iVar4 + 0x14);
            }
            else {
              puVar6 = (undefined4 *)(iVar4 + 0x154);
            }
            *puVar6 = 0;
            FUN_08013668(iVar4,puVar6,2);
            return 0;
          }
          goto LAB_0801350c;
        }
        if (*(char *)(iVar4 + 0x29c) != '\x03') goto LAB_0801350c;
        iVar10 = iVar4 + (uVar5 & 0xf) * 0x14;
        if ((int)((uint)uVar2 << 0x18) < 0) {
          if (*(short *)(iVar10 + 0x24) == 0) goto LAB_0801350c;
          puVar7 = (uint *)(iVar4 + ((uVar5 & 0x7f) + 1) * 0x14);
        }
        else {
          if (*(short *)(iVar10 + 0x164) == 0) goto LAB_0801350c;
          puVar7 = (uint *)((uVar5 & 0x7f) * 0x14 + iVar4 + 0x154);
        }
        uVar5 = uVar5 & 0x7f;
        if ((uVar2 & 0x7f) != 0) {
          iVar10 = FUN_08009aac(iVar4);
          if (iVar10 == 0) {
            *puVar7 = 0;
            goto LAB_080135b0;
          }
          uVar5 = 1;
        }
        *puVar7 = uVar5;
LAB_080135b0:
        FUN_08013668(iVar4,puVar7,2);
        return 0;
      }
      goto LAB_0801350c;
    }
    if (*(char *)(iVar4 + 0x29c) != '\x02') {
      if (*(char *)(iVar4 + 0x29c) == '\x03') {
        if (((*(short *)(iVar4 + 0x2ac) == 0) && ((uVar2 & 0x7f) != 0)) &&
           (*(short *)(iVar4 + 0x2b0) == 0)) {
          FUN_08009a74(iVar4);
        }
        FUN_080136ac(iVar4);
        return 0;
      }
      goto LAB_0801350c;
    }
  }
  if ((uVar2 & 0x7f) != 0) {
    FUN_08009a74();
    FUN_08009a74(iVar4,0x80);
    return 0;
  }
LAB_0801350c:
  FUN_08009a74(iVar4,0x80);
  FUN_08009a74(iVar4,0);
  return 0;
}


