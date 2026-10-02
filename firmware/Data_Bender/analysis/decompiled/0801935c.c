/* 0801935c FUN_0801935c; analyst naming is provisional. */

void FUN_0801935c(int *param_1,undefined4 param_2)

{
  int *piVar1;
  int iVar2;
  
  piVar1 = DAT_08019378;
  *DAT_08019378 = 0;
  iVar2 = FUN_080199c4(param_2);
  if ((iVar2 == -1) && (*piVar1 != 0)) {
    *param_1 = *piVar1;
  }
  return;
}


