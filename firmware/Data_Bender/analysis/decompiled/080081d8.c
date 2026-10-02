/* 080081d8 FUN_080081d8; analyst naming is provisional. */

int FUN_080081d8(int param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 local_60;
  undefined4 local_5c;
  undefined4 local_58;
  undefined4 local_54;
  undefined4 local_50;
  undefined4 local_4c;
  undefined4 local_48 [5];
  undefined4 local_34;
  undefined4 local_30;
  undefined4 uStack_2c;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_1c;
  undefined4 uStack_18;
  undefined4 local_14;
  
  local_48[0] = 5;
  local_28 = 0;
  local_24 = 0x1000000;
  local_34 = 0;
  local_58 = 0x10;
  local_14 = 0;
  local_60 = 0;
  local_50 = 0;
  local_4c = 0x400000;
  local_5c = 1;
  local_54 = 1;
  local_30 = 0x100;
  uStack_2c = 0;
  local_1c = 0;
  uStack_18 = 0;
  iVar1 = FUN_0800f5d8(param_1 + 0x10,local_48,&local_60,param_2);
  if (iVar1 != 0) {
    iVar1 = 1;
    *(undefined *)(param_1 + 0x5c) = 1;
  }
  return iVar1;
}


