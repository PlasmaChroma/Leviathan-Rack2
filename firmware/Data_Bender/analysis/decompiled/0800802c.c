/* 0800802c FUN_0800802c; analyst naming is provisional. */

void FUN_0800802c(void)

{
  bool bVar1;
  code **ppcVar2;
  byte *pbVar3;
  undefined2 *puVar4;
  int iVar5;
  code *pcVar6;
  undefined2 *puVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  
  iVar10 = 0;
  bVar1 = (bool)isCurrentModePrivileged();
  if (bVar1) {
    iVar10 = isIRQinterruptsEnabled();
  }
  disableIRQinterrupts();
  FUN_0800cf80();
  pbVar3 = DAT_08007d7c;
  ppcVar2 = DAT_08007d78;
  *DAT_08007d7c = 0xff;
  pcVar6 = *ppcVar2;
  if (pcVar6 != (code *)0x0) {
    *ppcVar2 = (code *)0x0;
    (*pcVar6)(*DAT_08007d80,1);
  }
  puVar4 = DAT_08007d88;
  if ((int)((uint)*pbVar3 << 0x18) < 0) {
    iVar8 = 0;
    puVar7 = DAT_08007d88;
    iVar9 = DAT_08007d84;
    do {
      iVar5 = *(int *)(puVar7 + 2);
      if (iVar5 != 0) {
        if (*(int *)(puVar7 + 10) == 0) {
          iVar5 = FUN_08007b1c(iVar9,*puVar7,iVar5,puVar7[4],*(undefined4 *)(puVar7 + 6),
                               *(undefined4 *)(puVar7 + 8));
        }
        else {
          iVar5 = FUN_08007c04(iVar9,*puVar7,iVar5,puVar7[4],*(undefined4 *)(puVar7 + 6),
                               *(undefined4 *)(puVar7 + 8));
        }
        if (iVar5 == 0) {
          *(undefined4 *)(puVar4 + iVar8 * 0xc + 2) = 0;
          break;
        }
      }
      iVar8 = iVar8 + 1;
      puVar7 = puVar7 + 0xc;
      iVar9 = iVar9 + 0xe0;
    } while (iVar8 != 3);
  }
  if (iVar10 == 0) {
    enableIRQinterrupts();
  }
  return;
}


