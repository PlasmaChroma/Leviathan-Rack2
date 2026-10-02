/* 08009b28 FUN_08009b28; analyst naming is provisional. */

void FUN_08009b28(int param_1)

{
  char *pcVar1;
  
  pcVar1 = *(char **)(param_1 + 0x300);
  *(int *)(pcVar1 + 0x3c8) = *(int *)(pcVar1 + 0x3c8) + 1;
  if ((*pcVar1 == '\v') && (*(int *)(pcVar1 + 0x380) != 0)) {
                    /* WARNING: Could not recover jumptable at 0x080136f8. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(*(int *)(pcVar1 + 0x380) + 0x18))();
    return;
  }
  return;
}


