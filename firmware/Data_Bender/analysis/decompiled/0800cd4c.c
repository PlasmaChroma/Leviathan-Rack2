/* 0800cd4c FUN_0800cd4c; analyst naming is provisional. */

undefined4 FUN_0800cd4c(int *param_1,uint param_2,uint param_3,uint param_4,int param_5)

{
  int iVar1;
  int iVar2;
  
  iVar2 = *param_1;
  do {
    do {
      if (((param_2 & ~*(uint *)(iVar2 + 0x18)) == 0) != param_3) {
        return 0;
      }
    } while (param_4 == 0xffffffff);
    iVar1 = FUN_08009ce8();
    iVar2 = *param_1;
  } while ((((uint)(iVar1 - param_5) <= param_4) && (param_4 != 0)) ||
          (((param_2 & ~*(uint *)(iVar2 + 0x18)) == 0) != param_3));
  *(undefined *)(param_1 + 0x10) = 0;
  param_1[0x11] = param_1[0x11] | 0x20;
  *(undefined *)((int)param_1 + 0x41) = 0x20;
  *(undefined *)((int)param_1 + 0x42) = 0;
  return 1;
}


