/* 08008400 FUN_08008400; analyst naming is provisional. */

undefined4 FUN_08008400(int param_1)

{
  int iVar1;
  int iVar2;
  byte local_69;
  uint local_68;
  undefined4 local_64;
  undefined4 local_60;
  undefined4 local_5c;
  int local_58;
  undefined4 local_54;
  undefined4 local_50 [5];
  undefined4 local_3c;
  undefined4 local_38;
  undefined4 uStack_34;
  undefined4 local_30;
  undefined4 local_2c;
  undefined4 local_28;
  undefined4 uStack_24;
  undefined4 local_20;
  undefined4 uStack_1c;
  
  local_50[0] = 1;
  local_3c = 0;
  local_38 = 0x100;
  uStack_34 = 0;
  local_30 = 0;
  local_2c = 0x1000000;
  local_28 = 1;
  uStack_24 = 0;
  local_20 = 0;
  uStack_1c = 0;
  iVar1 = FUN_080080f0();
  if (iVar1 == 0) {
    iVar2 = param_1 + 0x10;
    iVar1 = FUN_0800f32c(iVar2,local_50,5000);
    if (iVar1 == 0) {
      local_69 = 0x40;
      local_58 = FUN_0800f3f0(iVar2,&local_69,5000);
      if (local_58 == 0) {
        local_5c = 1;
        local_60 = 0x8000;
        local_2c = 0x1000000;
        local_54 = 0x400000;
        local_50[0] = 5;
        local_68 = (uint)local_69;
        local_64 = 0xff;
        iVar1 = FUN_0800f5d8(iVar2,local_50,&local_68,5000);
        if ((iVar1 == 0) && (iVar1 = FUN_080081d8(param_1,5000), iVar1 == 0)) {
          return 0;
        }
      }
    }
  }
  *(undefined *)(param_1 + 0x5c) = 1;
  return 1;
}


