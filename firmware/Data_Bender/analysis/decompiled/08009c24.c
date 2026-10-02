/* 08009c24 FUN_08009c24; analyst naming is provisional. */

undefined4 FUN_08009c24(uint param_1)

{
  int iVar1;
  
  if (*DAT_08009c64 == 0) {
    return 1;
  }
  iVar1 = FUN_0800aa28(*DAT_08009c68 / (1000 / *DAT_08009c64));
  if ((param_1 < 0x10) && (iVar1 == 0)) {
    FUN_0800a968(0xffffffff,param_1,0);
    *DAT_08009c6c = param_1;
    return 0;
  }
  return 1;
}


