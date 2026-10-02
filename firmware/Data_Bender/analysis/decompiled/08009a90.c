/* 08009a90 FUN_08009a90; analyst naming is provisional. */

undefined FUN_08009a90(int param_1)

{
  uint uVar1;
  
  uVar1 = FUN_0800f008(*(undefined4 *)(param_1 + 0x2c8));
  if (uVar1 < 4) {
    return *(undefined *)(DAT_08009aa8 + uVar1);
  }
  return 3;
}


