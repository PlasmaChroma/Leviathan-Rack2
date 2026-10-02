/* 080171c8 FUN_080171c8; analyst naming is provisional. */

undefined4 FUN_080171c8(int **param_1,uint param_2,uint param_3,int param_4,uint param_5)

{
  int iVar1;
  int *piVar2;
  
  piVar2 = *param_1;
  do {
    do {
      do {
        if (((param_2 & ~piVar2[7]) == 0) != param_3) {
          return 0;
        }
      } while (param_5 == 0xffffffff);
      iVar1 = FUN_08009ce8();
      if ((param_5 < (uint)(iVar1 - param_4)) || (param_5 == 0)) {
        return 3;
      }
      piVar2 = *param_1;
    } while (((-1 < *piVar2 << 0x1d) || (param_2 == 0x80)) || (param_2 == 0x40));
    if ((piVar2[7] & 8U) != 0) {
      piVar2[8] = 8;
      FUN_0801660c(param_1);
      param_1[0x24] = (int *)&SupervisorCall;
      *(bool *)(param_1 + 0x21) = param_5 == 0;
      return 1;
    }
  } while (-1 < piVar2[7] << 0x14);
  piVar2[8] = 0x800;
  FUN_0801660c(param_1);
  *(undefined *)(param_1 + 0x21) = 0;
  param_1[0x24] = (int *)0x20;
  return 3;
}


