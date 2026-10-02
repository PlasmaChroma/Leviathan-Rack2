/* 0800aa28 FUN_0800aa28; analyst naming is provisional. */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 FUN_0800aa28(int param_1)

{
  if (param_1 - 1U < 0x1000000) {
    _DAT_e000e014 = param_1 - 1U;
    *(undefined *)(DAT_0800aa50 + 0x23) = 0xf0;
    _DAT_e000e018 = 0;
    _DAT_e000e010 = 7;
    return 0;
  }
  return 1;
}


