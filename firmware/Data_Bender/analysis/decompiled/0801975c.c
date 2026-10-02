/* 0801975c FUN_0801975c; analyst naming is provisional. */

void FUN_0801975c(int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int *piVar1;
  int iVar2;
  
  piVar1 = DAT_0801977c;
  *DAT_0801977c = 0;
  iVar2 = FUN_080199a4(param_2,param_3,param_4,param_4,param_4);
  if ((iVar2 == -1) && (*piVar1 != 0)) {
    *param_1 = *piVar1;
  }
  return;
}


