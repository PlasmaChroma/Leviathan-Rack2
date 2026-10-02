/* 08018c34 FUN_08018c34; analyst naming is provisional. */

void FUN_08018c34(undefined4 *param_1,int param_2)

{
  int **ppiVar1;
  int iVar2;
  int *piVar3;
  int *piVar4;
  int *piVar5;
  int iVar6;
  int *piVar7;
  bool bVar8;
  
  if (param_2 == 0) {
    return;
  }
  piVar5 = (int *)(param_2 + -4);
  if (*(int *)(param_2 + -4) < 0) {
    piVar5 = (int *)((int)piVar5 + *(int *)(param_2 + -4));
  }
  FUN_0801984c();
  ppiVar1 = DAT_08018cc8;
  piVar3 = *DAT_08018cc8;
  if (piVar3 != (int *)0x0) {
    if (piVar3 <= piVar5) {
      do {
        piVar4 = piVar3;
        piVar3 = (int *)piVar4[1];
        if (piVar3 == (int *)0x0) break;
      } while (piVar3 <= piVar5);
      piVar7 = (int *)((int)piVar4 + *piVar4);
      if (piVar7 == piVar5) {
        iVar2 = *piVar4 + *piVar5;
        *piVar4 = iVar2;
        if (piVar3 == (int *)((int)piVar4 + iVar2)) {
          iVar6 = *piVar3;
          piVar4[1] = piVar3[1];
          *piVar4 = iVar2 + iVar6;
        }
      }
      else if (piVar5 < piVar7) {
        *param_1 = 0xc;
      }
      else {
        piVar7 = (int *)((int)piVar5 + *piVar5);
        bVar8 = piVar3 == piVar7;
        if (bVar8) {
          piVar7 = (int *)*piVar3;
          piVar3 = (int *)piVar3[1];
        }
        piVar5[1] = (int)piVar3;
        if (bVar8) {
          *piVar5 = (int)piVar7 + *piVar5;
        }
        piVar4[1] = (int)piVar5;
      }
      goto LAB_08018c5a;
    }
    if (piVar3 == (int *)((int)piVar5 + *piVar5)) {
      iVar2 = *piVar3;
      piVar3 = (int *)piVar3[1];
      *piVar5 = iVar2 + *piVar5;
    }
  }
  piVar5[1] = (int)piVar3;
  *ppiVar1 = piVar5;
LAB_08018c5a:
  FUN_08019858();
  return;
}


