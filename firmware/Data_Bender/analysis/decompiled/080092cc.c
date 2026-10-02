/* 080092cc FUN_080092cc; analyst naming is provisional. */

void FUN_080092cc(int *param_1)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  code *UNRECOVERED_JUMPTABLE;
  
  iVar1 = DAT_08009338;
  iVar4 = *param_1;
  if ((iVar4 == DAT_08009330) || (iVar4 == DAT_08009330 + 0x20)) {
    UNRECOVERED_JUMPTABLE = *(code **)(DAT_08009338 + 0x254);
    uVar3 = *(uint *)(DAT_08009338 + 0x250) >> 1;
    *(uint *)(DAT_08009338 + 600) = uVar3;
    if (UNRECOVERED_JUMPTABLE != (code *)0x0) {
      iVar4 = *(int *)(iVar1 + 0x248);
      iVar2 = *(int *)(iVar1 + 0x24c);
LAB_08009320:
                    /* WARNING: Could not recover jumptable at 0x0800932e. Too many branches */
                    /* WARNING: Treating indirect jump as call */
      (*UNRECOVERED_JUMPTABLE)(iVar4 + uVar3 * 4,iVar2 + uVar3 * 4);
      return;
    }
  }
  else if ((iVar4 == DAT_08009334) || (iVar4 == DAT_08009334 + 0x20)) {
    UNRECOVERED_JUMPTABLE = *(code **)(DAT_08009338 + 0x4b0);
    uVar3 = *(uint *)(DAT_08009338 + 0x4ac) >> 1;
    *(uint *)(DAT_08009338 + 0x4b4) = uVar3;
    if (UNRECOVERED_JUMPTABLE != (code *)0x0) {
      iVar2 = *(int *)(iVar1 + 0x4a8);
      iVar4 = *(int *)(iVar1 + 0x4a4);
      goto LAB_08009320;
    }
  }
  return;
}


