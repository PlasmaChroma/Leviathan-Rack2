/* 080122f0 FUN_080122f0; analyst naming is provisional. */

byte FUN_080122f0(undefined4 *param_1,int *param_2)

{
  byte bVar1;
  
  bVar1 = *(byte *)(param_1 + 0xb);
  if (bVar1 != 2) {
    if ((bVar1 & 0xfb) == 1) {
      *(undefined *)(param_1 + 0xb) = 2;
      FUN_08012474(*param_1);
      if (*param_2 != 2) {
        *(undefined *)(param_1 + 0xb) = 1;
        return 0;
      }
      *(undefined *)(param_1 + 0xb) = 5;
      return 0;
    }
    bVar1 = 1;
  }
  return bVar1;
}


