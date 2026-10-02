/* 08019910 FUN_08019910; analyst naming is provisional. */

void FUN_08019910(int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int *piVar1;
  int iVar2;
  
  piVar1 = DAT_08019930;
  *DAT_08019930 = 0;
  iVar2 = FUN_08019964(param_2,param_3,param_3,0,param_4);
  if ((iVar2 == -1) && (*piVar1 != 0)) {
    *param_1 = *piVar1;
  }
  return;
}


