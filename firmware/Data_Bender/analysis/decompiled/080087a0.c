/* 080087a0 FUN_080087a0; analyst naming is provisional. */

undefined4 FUN_080087a0(int param_1,undefined4 param_2,uint param_3,undefined4 param_4,char param_5)

{
  int iVar1;
  undefined4 local_58;
  undefined4 local_54;
  undefined4 local_4c;
  undefined4 local_44;
  undefined4 local_40;
  undefined4 local_3c;
  undefined4 local_38;
  undefined4 local_34;
  uint local_30;
  undefined4 local_2c;
  undefined4 uStack_28;
  undefined4 local_24;
  
  iVar1 = System_GetProgramMemoryRegion();
  if (iVar1 == 7) {
    *(undefined *)(param_1 + 0x5c) = 3;
    return 1;
  }
  if (*(char *)(param_1 + 0xd) != '\x01') {
    *(undefined *)(param_1 + 0xd) = 1;
    iVar1 = FUN_080085c8(param_1,param_1);
    if (iVar1 != 0) {
      *(undefined *)(param_1 + 0xd) = 0;
      *(undefined *)(param_1 + 0x5c) = 2;
      FUN_080085c8(param_1,param_1);
      return 1;
    }
  }
  local_34 = 0x1000000;
  if (0xff < param_3) {
    param_3 = 0x100;
  }
  local_40 = 0x100;
  local_3c = 0x400;
  local_38 = 0;
  local_44 = 0;
  local_24 = 0;
  local_58 = 2;
  local_4c = 0x2000;
  local_2c = 0;
  uStack_28 = 0;
  local_54 = param_2;
  local_30 = param_3;
  iVar1 = FUN_080080f0();
  if (iVar1 == 0) {
    iVar1 = FUN_0800f32c(param_1 + 0x10,&local_58,5000);
    if (iVar1 == 0) {
      iVar1 = FUN_0800f3f0(param_1 + 0x10,param_4,5000);
      if (iVar1 != 0) goto LAB_0800875e;
      iVar1 = FUN_080081d8(param_1,5000);
      if (iVar1 == 0) {
        if (param_5 == '\0') {
          return 0;
        }
        if (*(char *)(param_1 + 0xd) == '\0') {
          return 0;
        }
        *(undefined *)(param_1 + 0xd) = 0;
        iVar1 = FUN_080085c8(param_1,param_1);
        if (iVar1 == 0) {
          return 0;
        }
        *(undefined *)(param_1 + 0xd) = 0;
        *(undefined *)(param_1 + 0x5c) = 2;
        FUN_080085c8(param_1,param_1);
        return 1;
      }
    }
    if (*(char *)(param_1 + 0xd) != '\0') {
      *(undefined *)(param_1 + 0xd) = 0;
      iVar1 = FUN_080085c8(param_1,param_1);
      if (iVar1 != 0) {
        *(undefined *)(param_1 + 0xd) = 0;
        *(undefined *)(param_1 + 0x5c) = 2;
        FUN_080085c8(param_1,param_1);
      }
    }
  }
  else {
LAB_0800875e:
    if (*(char *)(param_1 + 0xd) != '\0') {
      *(undefined *)(param_1 + 0xd) = 0;
      iVar1 = FUN_080085c8(param_1,param_1);
      if (iVar1 != 0) {
        *(undefined *)(param_1 + 0xd) = 0;
        *(undefined *)(param_1 + 0x5c) = 2;
        FUN_080085c8(param_1,param_1);
      }
    }
  }
  *(undefined *)(param_1 + 0x5c) = 1;
  return 1;
}


