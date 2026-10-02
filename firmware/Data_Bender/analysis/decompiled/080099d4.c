/* 080099d4 FUN_080099d4; analyst naming is provisional. */

undefined4 FUN_080099d4(int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  int iVar2;
  code *pcVar3;
  undefined4 unaff_r4;
  undefined4 uVar4;
  
  iVar2 = *(int *)(param_1 + 0x10);
  if (iVar2 != 0) {
    if (iVar2 != 2) {
      do {
                    /* WARNING: Do nothing block with infinite loop */
      } while( true );
    }
    iVar2 = 1;
  }
  FUN_08013024(*(undefined4 *)(param_1 + 0x508),iVar2);
  iVar2 = *(int *)(param_1 + 0x508);
  *(undefined *)(iVar2 + 0x29c) = 1;
  *(undefined4 *)(iVar2 + 4) = 0;
  *(undefined4 *)(iVar2 + 0x294) = 0;
  *(undefined4 *)(iVar2 + 0x2a4) = 0;
  *(undefined *)(iVar2 + 0x2a0) = 0;
  if (((*(int *)(iVar2 + 0x2b8) == 0) ||
      (pcVar3 = *(code **)(*(int *)(iVar2 + 0x2b8) + 4), pcVar3 == (code *)0x0)) ||
     (iVar1 = (*pcVar3)(), iVar1 == 0)) {
    uVar4 = 0;
  }
  else {
    uVar4 = 3;
  }
  FUN_08009a50(iVar2,0,0,0x40,param_4,unaff_r4);
  *(undefined2 *)(iVar2 + 0x164) = 1;
  *(undefined4 *)(iVar2 + 0x160) = 0x40;
  FUN_08009a50(iVar2,0x80,0,0x40);
  *(undefined2 *)(iVar2 + 0x24) = 1;
  *(undefined4 *)(iVar2 + 0x20) = 0x40;
  return uVar4;
}


