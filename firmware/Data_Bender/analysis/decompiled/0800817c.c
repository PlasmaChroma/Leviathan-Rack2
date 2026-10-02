/* 0800817c FUN_0800817c; analyst naming is provisional. */

int FUN_0800817c(int param_1)

{
  int iVar1;
  undefined4 local_50;
  undefined4 uStack_4c;
  undefined4 local_48 [2];
  undefined4 local_40;
  undefined4 local_3c;
  undefined4 uStack_38;
  undefined4 local_34;
  undefined4 local_30;
  undefined4 local_2c;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_1c;
  undefined4 uStack_18;
  undefined4 local_14;
  
  local_48[0] = 0xeb;
  local_2c = 0xc00;
  local_28 = 0xc000;
  local_34 = 6;
  local_24 = 0x3000000;
  local_30 = 0x100;
  local_3c = 0x2000;
  uStack_38 = 0;
  local_40 = 0xa0;
  local_14 = 0x10000000;
  local_1c = 0;
  uStack_18 = 0;
  local_50 = 0;
  uStack_4c = 0;
  iVar1 = FUN_0800f6b4(param_1 + 0x10,local_48,&local_50);
  if (iVar1 != 0) {
    iVar1 = 1;
    *(undefined *)(param_1 + 0x5c) = 1;
  }
  return iVar1;
}


