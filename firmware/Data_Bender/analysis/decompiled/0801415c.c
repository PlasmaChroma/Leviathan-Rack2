/* 0801415c FUN_0801415c; analyst naming is provisional. */

/* WARNING: Removing unreachable block (ram,0x08013d32) */

void FUN_0801415c(void)

{
  bool bVar1;
  code **ppcVar2;
  byte *pbVar3;
  int *piVar4;
  int iVar5;
  int iVar6;
  code *pcVar7;
  int *piVar8;
  int iVar9;
  int iVar10;
  int iVar11;
  
  pbVar3 = DAT_08013d64;
  ppcVar2 = DAT_08013d60;
  iVar11 = 0;
  bVar1 = (bool)isCurrentModePrivileged();
  if (bVar1) {
    iVar11 = isIRQinterruptsEnabled();
  }
  disableIRQinterrupts();
  *DAT_08013d64 = 0xff;
  pcVar7 = *ppcVar2;
  if (pcVar7 != (code *)0x0) {
    *ppcVar2 = (code *)0x0;
    (*pcVar7)(*DAT_08013d68,0);
  }
  piVar4 = DAT_08013d70;
  if ((int)((uint)*pbVar3 << 0x18) < 0) {
    iVar9 = 0;
    piVar8 = DAT_08013d70;
    iVar10 = DAT_08013d6c;
    do {
      iVar5 = *piVar8;
      if ((iVar5 != 0) && (iVar6 = piVar8[1], iVar6 != 0)) {
        if (piVar8[6] == 1) {
          iVar5 = FUN_08013a48(iVar10,iVar6,*(undefined2 *)(piVar8 + 2),piVar8[3],piVar8[4],
                               piVar8[5]);
joined_r0x08013d4e:
          if (iVar5 != 0) goto LAB_08013d02;
        }
        else {
          if (piVar8[6] == 0) {
            iVar5 = FUN_08013b10(iVar10,iVar5,*(undefined2 *)(piVar8 + 2),piVar8[3],piVar8[4],
                                 piVar8[5]);
            goto joined_r0x08013d4e;
          }
          iVar5 = FUN_08013bd8(iVar10,iVar5,iVar6,*(undefined2 *)(piVar8 + 2),piVar8[3],piVar8[4],
                               piVar8[5]);
          if (iVar5 != 0) goto LAB_08013d02;
          iVar5 = 0;
        }
        piVar4[iVar9 * 7] = iVar5;
        piVar4[iVar9 * 7 + 1] = iVar5;
        break;
      }
LAB_08013d02:
      iVar9 = iVar9 + 1;
      piVar8 = piVar8 + 7;
      iVar10 = iVar10 + 0x1a0;
    } while (iVar9 != 4);
  }
  if (iVar11 == 0) {
    enableIRQinterrupts();
  }
  return;
}


