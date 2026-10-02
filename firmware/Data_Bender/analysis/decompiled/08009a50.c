/* 08009a50 FUN_08009a50; analyst naming is provisional. */

undefined FUN_08009a50(int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  uint uVar1;
  
  uVar1 = FUN_0800ee7c(*(undefined4 *)(param_1 + 0x2c8),param_2,param_4,param_3,param_4);
  if (uVar1 < 4) {
    return *(undefined *)(DAT_08009a70 + uVar1);
  }
  return 3;
}


