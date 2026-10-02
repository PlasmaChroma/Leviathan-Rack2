/* 080195a8 FUN_080195a8; analyst naming is provisional. */

void FUN_080195a8(void)

{
  int *piVar1;
  int iVar2;
  code *UNRECOVERED_JUMPTABLE;
  undefined8 uVar3;
  
  FUN_080198d8(6);
  UNRECOVERED_JUMPTABLE = (code *)0x80195b7;
  uVar3 = FUN_080199f0(1);
  piVar1 = DAT_080195d4;
  *DAT_080195d4 = 0;
  iVar2 = FUN_08019954((int)((ulonglong)uVar3 >> 0x20));
  if ((iVar2 == -1) && (*piVar1 != 0)) {
    *(int *)uVar3 = *piVar1;
  }
                    /* WARNING: Could not recover jumptable at 0x080195d2. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*UNRECOVERED_JUMPTABLE)();
  return;
}


