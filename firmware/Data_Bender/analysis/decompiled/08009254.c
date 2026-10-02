/* 08009254 FUN_08009254; analyst naming is provisional. */

void FUN_08009254(int *param_1)

{
  code **ppcVar1;
  int iVar2;
  int iVar3;
  
  iVar2 = DAT_080092c8;
  iVar3 = *param_1;
  if ((iVar3 == DAT_080092c0) || (iVar3 == DAT_080092c0 + 0x20)) {
    ppcVar1 = (code **)(DAT_080092c8 + 0x254);
    *(undefined4 *)(DAT_080092c8 + 600) = 0;
    if (*ppcVar1 != (code *)0x0) {
                    /* WARNING: Could not recover jumptable at 0x080092bc. Too many branches */
                    /* WARNING: Treating indirect jump as call */
      (**ppcVar1)(*(undefined4 *)(iVar2 + 0x248),*(undefined4 *)(iVar2 + 0x24c),
                  *(uint *)(iVar2 + 0x250) >> 1);
      return;
    }
  }
  else if ((iVar3 == DAT_080092c4) || (iVar3 == DAT_080092c4 + 0x20)) {
    ppcVar1 = (code **)(DAT_080092c8 + 0x4b0);
    *(undefined4 *)(DAT_080092c8 + 0x4b4) = 0;
    if (*ppcVar1 != (code *)0x0) {
                    /* WARNING: Could not recover jumptable at 0x0800929a. Too many branches */
                    /* WARNING: Treating indirect jump as call */
      (**ppcVar1)(*(undefined4 *)(iVar2 + 0x4a4),*(undefined4 *)(iVar2 + 0x4a8),
                  *(uint *)(iVar2 + 0x4ac) >> 1);
      return;
    }
  }
  return;
}


