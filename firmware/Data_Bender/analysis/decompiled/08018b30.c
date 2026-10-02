/* 08018b30 FUN_08018b30; analyst naming is provisional. */

int * FUN_08018b30(undefined4 *param_1)

{
  int *piVar1;
  int *piVar2;
  int iVar3;
  int **ppiVar4;
  
  FUN_08018a90();
  iVar3 = *DAT_08018bb4;
  if (*(int *)(iVar3 + 0x18) == 0) {
    FUN_08018ac0(iVar3);
  }
  ppiVar4 = (int **)(iVar3 + 0x48);
  do {
    piVar1 = ppiVar4[1];
    piVar2 = ppiVar4[2];
    while (piVar1 = (int *)((int)piVar1 + -1), -1 < (int)piVar1) {
      if (*(short *)(piVar2 + 3) == 0) {
        piVar2[3] = DAT_08018bb8;
        piVar2[0x19] = 0;
        FUN_08018c1e(piVar2 + 0x16);
        FUN_08018a9c();
        piVar2[1] = 0;
        piVar2[2] = 0;
        piVar2[4] = 0;
        piVar2[5] = 0;
        *piVar2 = 0;
        piVar2[6] = 0;
        memset(piVar2 + 0x17,0,8);
        piVar2[0xd] = 0;
        piVar2[0xe] = 0;
        piVar2[0x12] = 0;
        piVar2[0x13] = 0;
        return piVar2;
      }
      piVar2 = piVar2 + 0x1a;
    }
    if (*ppiVar4 == (int *)0x0) {
      piVar1 = (int *)FUN_08018a64(param_1,4);
      *ppiVar4 = piVar1;
      if (piVar1 == (int *)0x0) {
        FUN_08018a9c();
        *param_1 = 0xc;
        return (int *)0x0;
      }
    }
    ppiVar4 = (int **)*ppiVar4;
  } while( true );
}


