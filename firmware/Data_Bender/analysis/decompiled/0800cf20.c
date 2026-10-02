/* 0800cf20 FUN_0800cf20; analyst naming is provisional. */

undefined4 FUN_0800cf20(int *param_1,uint param_2,int param_3)

{
  int iVar1;
  
  if (*(int *)(*param_1 + 0x18) << 0x1a < 0) {
    return 0;
  }
  while( true ) {
    iVar1 = FUN_0800cdb8(param_1,param_2,param_3);
    if (iVar1 != 0) {
      return 1;
    }
    iVar1 = FUN_08009ce8();
    if (((param_2 < (uint)(iVar1 - param_3)) || (param_2 == 0)) &&
       ((*(uint *)(*param_1 + 0x18) & 0x20) == 0)) break;
    if (*(int *)(*param_1 + 0x18) << 0x1a < 0) {
      return 0;
    }
  }
  *(undefined *)(param_1 + 0x10) = 0;
  param_1[0x11] = param_1[0x11] | 0x20;
  *(undefined *)((int)param_1 + 0x41) = 0x20;
  *(undefined *)((int)param_1 + 0x42) = 0;
  return 1;
}


