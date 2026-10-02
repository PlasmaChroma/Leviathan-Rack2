/* 080099a4 FUN_080099a4; analyst naming is provisional. */

undefined4 FUN_080099a4(int param_1,int param_2)

{
  int iVar1;
  undefined4 uVar2;
  code *UNRECOVERED_JUMPTABLE;
  uint uVar3;
  uint uVar4;
  
  iVar1 = *(int *)(param_1 + 0x508);
  uVar2 = *(undefined4 *)(param_1 + param_2 * 0x24 + 0x288);
  if (param_2 == 0) {
    if (*(int *)(iVar1 + 0x294) == 3) {
      uVar4 = *(uint *)(iVar1 + 0x160);
      if (*(uint *)(iVar1 + 0x15c) <= uVar4) {
        if ((*(char *)(iVar1 + 0x29c) == '\x03') &&
           (UNRECOVERED_JUMPTABLE = *(code **)(*(int *)(iVar1 + 0x2b8) + 0x10),
           UNRECOVERED_JUMPTABLE != (code *)0x0)) {
          *(undefined4 *)(iVar1 + 0x2d4) = 0;
          (*UNRECOVERED_JUMPTABLE)();
        }
        FUN_080136ac(iVar1);
        return 0;
      }
      uVar3 = *(uint *)(iVar1 + 0x15c) - uVar4;
      *(uint *)(iVar1 + 0x15c) = uVar3;
      if (uVar3 <= uVar4) {
        uVar4 = uVar3;
      }
      FUN_08013698(iVar1,uVar2,uVar4);
    }
  }
  else if ((*(char *)(iVar1 + 0x29c) == '\x03') &&
          (UNRECOVERED_JUMPTABLE = *(code **)(*(int *)(iVar1 + 0x2b8) + 0x18),
          UNRECOVERED_JUMPTABLE != (code *)0x0)) {
    *(undefined4 *)(iVar1 + 0x2d4) = 0;
                    /* WARNING: Could not recover jumptable at 0x08012ec4. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    uVar2 = (*UNRECOVERED_JUMPTABLE)();
    return uVar2;
  }
  return 0;
}


