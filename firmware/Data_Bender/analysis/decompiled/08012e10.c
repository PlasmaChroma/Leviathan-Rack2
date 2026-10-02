/* 08012e10 FUN_08012e10; analyst naming is provisional. */

undefined4 FUN_08012e10(int param_1)

{
  undefined4 uVar1;
  
  if (*(code ***)(param_1 + 0x2b8) != (code **)0x0) {
                    /* WARNING: Could not recover jumptable at 0x08012e18. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    uVar1 = (***(code ***)(param_1 + 0x2b8))();
    return uVar1;
  }
  return 0;
}


