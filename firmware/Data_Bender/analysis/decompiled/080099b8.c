/* 080099b8 FUN_080099b8; analyst naming is provisional. */

undefined4 FUN_080099b8(int param_1,int param_2)

{
  char cVar1;
  int iVar2;
  undefined4 uVar3;
  uint uVar4;
  code *UNRECOVERED_JUMPTABLE;
  uint uVar5;
  
  iVar2 = *(int *)(param_1 + 0x508);
  uVar3 = *(undefined4 *)(param_1 + param_2 * 0x24 + 0x48);
  if (param_2 != 0) {
    if (*(char *)(iVar2 + 0x29c) != '\x03') {
      return 0;
    }
    UNRECOVERED_JUMPTABLE = *(code **)(*(int *)(iVar2 + 0x2b8) + 0x14);
    if (UNRECOVERED_JUMPTABLE == (code *)0x0) {
      return 0;
    }
    *(undefined4 *)(iVar2 + 0x2d4) = 0;
                    /* WARNING: Could not recover jumptable at 0x08012f42. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    uVar3 = (*UNRECOVERED_JUMPTABLE)();
    return uVar3;
  }
  if (*(int *)(iVar2 + 0x294) == 2) {
    uVar4 = *(uint *)(iVar2 + 0x1c);
    uVar5 = *(uint *)(iVar2 + 0x20);
    if (uVar5 < uVar4) {
      *(uint *)(iVar2 + 0x1c) = uVar4 - uVar5;
      FUN_08013684(iVar2,uVar3,uVar4 - uVar5);
      FUN_08009b0c(iVar2,0,0,0);
    }
    else {
      if (((uVar4 != uVar5) || (*(uint *)(iVar2 + 0x18) < uVar4)) ||
         (*(uint *)(iVar2 + 0x298) <= *(uint *)(iVar2 + 0x18))) {
        if ((*(char *)(iVar2 + 0x29c) == '\x03') &&
           (UNRECOVERED_JUMPTABLE = *(code **)(*(int *)(iVar2 + 0x2b8) + 0xc),
           UNRECOVERED_JUMPTABLE != (code *)0x0)) {
          *(undefined4 *)(iVar2 + 0x2d4) = 0;
          (*UNRECOVERED_JUMPTABLE)(iVar2);
        }
        FUN_08009a74(iVar2,0x80);
        FUN_080136c4(iVar2);
        cVar1 = *(char *)(iVar2 + 0x2a0);
        goto joined_r0x08012f6c;
      }
      FUN_08013684(iVar2,0,0);
      *(undefined4 *)(iVar2 + 0x298) = 0;
      FUN_08009b0c(iVar2,0,0,0);
    }
  }
  cVar1 = *(char *)(iVar2 + 0x2a0);
joined_r0x08012f6c:
  if (cVar1 != '\0') {
    *(undefined *)(iVar2 + 0x2a0) = 0;
  }
  return 0;
}


