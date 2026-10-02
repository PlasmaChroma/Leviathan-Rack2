/* 080150e4 FUN_080150e4; analyst naming is provisional. */

/* WARNING: Removing unreachable block (ram,0x08014bda) */

void FUN_080150e4(int *param_1)

{
  bool bVar1;
  code **ppcVar2;
  byte *pbVar3;
  int iVar4;
  code *pcVar5;
  int *piVar6;
  int iVar7;
  int *piVar8;
  int iVar9;
  int iVar10;
  int iStack_40;
  int local_3c [4];
  undefined4 local_2c;
  int iStack_28;
  int iStack_24;
  undefined4 uStack_20;
  undefined4 local_1c;
  
  pbVar3 = DAT_08014be4;
  ppcVar2 = DAT_08014be0;
  piVar8 = &iStack_40;
  iVar10 = 0;
  local_3c[0] = *DAT_08015148;
  local_3c[1] = DAT_08015148[1];
  local_3c[2] = DAT_08015148[2];
  local_3c[3] = DAT_08015148[3];
  local_2c = DAT_08015148[4];
  iStack_28 = DAT_08015148[5];
  iStack_24 = DAT_08015148[6];
  uStack_20 = DAT_08015148[7];
  local_1c = DAT_08015148[8];
  while (piVar8 = piVar8 + 1, *param_1 != *piVar8) {
    iVar10 = iVar10 + 1;
    if (iVar10 == 9) {
                    /* WARNING: Does not return */
      pcVar5 = (code *)software_udf(0xff,0x8015146);
      (*pcVar5)();
    }
  }
  if (*(char *)(iVar10 * 0x1b8 + DAT_0801514c + 0x14) != '\0') {
    FUN_08014660();
    return;
  }
  iVar10 = 0;
  bVar1 = (bool)isCurrentModePrivileged();
  if (bVar1) {
    iVar10 = isIRQinterruptsEnabled();
  }
  disableIRQinterrupts();
  *DAT_08014be4 = 0xff;
  pcVar5 = *ppcVar2;
  if (pcVar5 != (code *)0x0) {
    *ppcVar2 = (code *)0x0;
    (*pcVar5)(*DAT_08014be8,0);
  }
  piVar8 = DAT_08014bec;
  if ((int)((uint)*pbVar3 << 0x18) < 0) {
    iVar7 = 0;
    piVar6 = DAT_08014bec;
    iVar9 = DAT_08014bf0;
    do {
      if (piVar6[6] == 1) {
        if (piVar6[1] != 0) {
          iStack_24 = piVar6[5];
          iStack_28 = piVar6[4];
          iVar4 = FUN_08014924(iVar9,piVar6[1],*(undefined2 *)(piVar6 + 2),piVar6[3]);
          if (iVar4 == 0) {
            iVar4 = 0;
            goto LAB_08014ba4;
          }
        }
      }
      else if ((piVar6[6] == 0) && (*piVar6 != 0)) {
        iStack_24 = piVar6[5];
        iStack_28 = piVar6[4];
        iVar4 = FUN_08014a2c(iVar9,*piVar6,*(undefined2 *)(piVar6 + 2),piVar6[3]);
        if (iVar4 == 0) {
LAB_08014ba4:
          piVar8[iVar7 * 7] = iVar4;
          piVar8[iVar7 * 7 + 1] = iVar4;
          break;
        }
      }
      iVar7 = iVar7 + 1;
      piVar6 = piVar6 + 7;
      iVar9 = iVar9 + 0x1b8;
    } while (iVar7 != 9);
  }
  if (iVar10 == 0) {
    enableIRQinterrupts();
  }
  return;
}


