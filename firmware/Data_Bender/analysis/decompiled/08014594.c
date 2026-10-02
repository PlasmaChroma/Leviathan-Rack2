/* 08014594 FUN_08014594; analyst naming is provisional. */

void FUN_08014594(int *param_1)

{
  int *piVar1;
  int iVar2;
  code *UNRECOVERED_JUMPTABLE;
  int iVar3;
  
  iVar3 = 0;
  iVar2 = DAT_080145c8;
  do {
    piVar1 = (int *)(iVar2 + 0x10);
    iVar2 = iVar2 + 100;
    if (*param_1 == *piVar1) {
      iVar2 = iVar3 * 100 + DAT_080145c8;
      UNRECOVERED_JUMPTABLE = *(code **)(iVar2 + 0x5c);
      if (UNRECOVERED_JUMPTABLE == (code *)0x0) {
        return;
      }
                    /* WARNING: Could not recover jumptable at 0x080145c4. Too many branches */
                    /* WARNING: Treating indirect jump as call */
      (*UNRECOVERED_JUMPTABLE)(*(undefined4 *)(iVar2 + 0x60));
      return;
    }
    iVar3 = iVar3 + 1;
  } while (iVar3 != 4);
  return;
}


