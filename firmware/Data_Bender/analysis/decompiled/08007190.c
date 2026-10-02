/* 08007190 FUN_08007190; analyst naming is provisional. */

void FUN_08007190(void)

{
  char cVar1;
  int iVar2;
  undefined4 uVar3;
  
  uVar3 = DAT_080071f0;
  iVar2 = DAT_080071ec;
  cVar1 = *(char *)(DAT_080071ec + 0x668);
  *(undefined4 *)(DAT_080071ec + 0x600) = 0x400;
  *(undefined4 *)(iVar2 + 0x5f0) = uVar3;
  *(undefined4 *)(iVar2 + 0x604) = 0x800;
  if (cVar1 == '\0') {
    uVar3 = 0x100;
  }
  else {
    uVar3 = 0;
  }
  *(undefined4 *)(iVar2 + 0x5f4) = 9;
  *(undefined4 *)(iVar2 + 0x5f8) = 0;
  *(undefined4 *)(iVar2 + 0x5fc) = 0;
  *(undefined4 *)(iVar2 + 0x610) = 0;
  *(undefined4 *)(iVar2 + 0x608) = 0x2000;
  *(undefined4 *)(iVar2 + 0x60c) = uVar3;
  *(undefined4 *)(iVar2 + 0x614) = 0;
  iVar2 = FUN_0800af40(iVar2 + 0x5f0);
  if (iVar2 == 0) {
    return;
  }
  software_bkpt(0xff);
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}


