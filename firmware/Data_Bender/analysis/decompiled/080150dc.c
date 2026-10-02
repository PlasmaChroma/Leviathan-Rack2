/* 080150dc FUN_080150dc; analyst naming is provisional. */

/* WARNING: Removing unreachable block (ram,0x08014bda) */

void FUN_080150dc(void)

{
  bool bVar1;
  code **ppcVar2;
  byte *pbVar3;
  int *piVar4;
  int iVar5;
  code *pcVar6;
  int *piVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  
  pbVar3 = DAT_08014be4;
  ppcVar2 = DAT_08014be0;
  iVar9 = 0;
  bVar1 = (bool)isCurrentModePrivileged();
  if (bVar1) {
    iVar9 = isIRQinterruptsEnabled();
  }
  disableIRQinterrupts();
  *DAT_08014be4 = 0xff;
  pcVar6 = *ppcVar2;
  if (pcVar6 != (code *)0x0) {
    *ppcVar2 = (code *)0x0;
    (*pcVar6)(*DAT_08014be8,0);
  }
  piVar4 = DAT_08014bec;
  if ((int)((uint)*pbVar3 << 0x18) < 0) {
    iVar8 = 0;
    piVar7 = DAT_08014bec;
    iVar10 = DAT_08014bf0;
    do {
      if (piVar7[6] == 1) {
        if ((piVar7[1] != 0) &&
           (iVar5 = FUN_08014924(iVar10,piVar7[1],*(undefined2 *)(piVar7 + 2),piVar7[3],piVar7[4],
                                 piVar7[5]), iVar5 == 0)) {
          iVar5 = 0;
          goto LAB_08014ba4;
        }
      }
      else if (((piVar7[6] == 0) && (*piVar7 != 0)) &&
              (iVar5 = FUN_08014a2c(iVar10,*piVar7,*(undefined2 *)(piVar7 + 2),piVar7[3],piVar7[4],
                                    piVar7[5]), iVar5 == 0)) {
LAB_08014ba4:
        piVar4[iVar8 * 7] = iVar5;
        piVar4[iVar8 * 7 + 1] = iVar5;
        break;
      }
      iVar8 = iVar8 + 1;
      piVar7 = piVar7 + 7;
      iVar10 = iVar10 + 0x1b8;
    } while (iVar8 != 9);
  }
  if (iVar9 == 0) {
    enableIRQinterrupts();
  }
  return;
}


