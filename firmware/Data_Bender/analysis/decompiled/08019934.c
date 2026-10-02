/* 08019934 FUN_08019934; analyst naming is provisional. */

void FUN_08019934(int *param_1,undefined4 param_2)

{
  int *piVar1;
  int iVar2;
  
  piVar1 = DAT_08019950;
  *DAT_08019950 = 0;
  iVar2 = FUN_08019984(param_2);
  if ((iVar2 == -1) && (*piVar1 != 0)) {
    *param_1 = *piVar1;
  }
  return;
}


