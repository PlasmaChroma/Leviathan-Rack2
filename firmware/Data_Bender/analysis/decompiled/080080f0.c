/* 080080f0 FUN_080080f0; analyst naming is provisional. */

undefined4 FUN_080080f0(int param_1)

{
  int iVar1;
  undefined4 local_60;
  undefined4 uStack_5c;
  undefined4 local_58;
  undefined4 uStack_54;
  int local_50;
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
  
  local_34 = 0;
  local_48[0] = 6;
  local_14 = 0;
  local_30 = 0x100;
  uStack_2c = 0;
  local_28 = 0;
  local_24 = 0;
  local_1c = 0;
  uStack_18 = 0;
  iVar1 = System_GetProgramMemoryRegion();
  if (iVar1 == 7) {
    *(undefined *)(param_1 + 0x5c) = 3;
    return 1;
  }
  local_50 = FUN_0800f32c(param_1 + 0x10,local_48,5000);
  if (local_50 == 0) {
    local_60 = 2;
    uStack_5c = 2;
    local_58 = 0x10;
    uStack_54 = 1;
    local_4c = 0x400000;
    local_48[0] = 5;
    local_24 = 0x1000000;
    iVar1 = FUN_0800f5d8(param_1 + 0x10,local_48,&local_60,5000);
    if (iVar1 == 0) {
      return 0;
    }
  }
  *(undefined *)(param_1 + 0x5c) = 1;
  return 1;
}


