/* 0800cec4 FUN_0800cec4; analyst naming is provisional. */

undefined4 FUN_0800cec4(int *param_1,uint param_2,int param_3)

{
  int iVar1;
  int iVar2;
  
  iVar2 = *param_1;
  while( true ) {
    if (*(int *)(iVar2 + 0x18) << 0x1e < 0) {
      return 0;
    }
    iVar2 = FUN_0800cdb8(param_1,param_2,param_3);
    if (iVar2 != 0) break;
    if (param_2 == 0xffffffff) {
      iVar2 = *param_1;
    }
    else {
      iVar1 = FUN_08009ce8();
      iVar2 = *param_1;
      if (((param_2 < (uint)(iVar1 - param_3)) || (param_2 == 0)) &&
         ((*(uint *)(iVar2 + 0x18) & 2) == 0)) {
        *(undefined *)(param_1 + 0x10) = 0;
        param_1[0x11] = param_1[0x11] | 0x20;
        *(undefined *)((int)param_1 + 0x41) = 0x20;
        *(undefined *)((int)param_1 + 0x42) = 0;
        return 1;
      }
    }
  }
  return 1;
}


