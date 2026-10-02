/* 0800895c FUN_0800895c; analyst naming is provisional. */

undefined4 FUN_0800895c(int param_1,undefined4 param_2)

{
  char cVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 local_50;
  undefined4 local_4c;
  undefined4 local_44;
  undefined4 local_3c;
  undefined4 local_38;
  undefined4 local_34;
  undefined4 local_30;
  undefined4 local_2c;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 uStack_20;
  undefined4 local_1c;
  
  local_38 = 0x100;
  local_50 = 0xd7;
  local_30 = 0;
  local_2c = 0;
  local_34 = 0x400;
  local_3c = 0;
  local_44 = 0x2000;
  local_28 = 1;
  local_1c = 0;
  local_24 = 0;
  uStack_20 = 0;
  local_4c = param_2;
  iVar2 = System_GetProgramMemoryRegion();
  if (iVar2 == 7) {
    *(undefined *)(param_1 + 0x5c) = 3;
    return 1;
  }
  if (*(char *)(param_1 + 0xd) != '\x01') {
    *(undefined *)(param_1 + 0xd) = 1;
    iVar2 = FUN_080085c8(param_1,param_1);
    if (iVar2 != 0) {
      *(undefined *)(param_1 + 0xd) = 0;
      *(undefined *)(param_1 + 0x5c) = 2;
      FUN_080085c8(param_1,param_1);
      return 1;
    }
  }
  iVar2 = FUN_080080f0(param_1);
  if (iVar2 == 0) {
    iVar2 = FUN_0800f32c(param_1 + 0x10,&local_50,5000);
    if (iVar2 == 0) {
      iVar2 = FUN_080081d8(param_1,5000);
      if (iVar2 == 0) {
        uVar3 = 0;
        if (*(char *)(param_1 + 0xd) != '\0') {
          *(undefined *)(param_1 + 0xd) = 0;
          iVar2 = FUN_080085c8(param_1,param_1);
          if (iVar2 != 0) {
            *(undefined *)(param_1 + 0xd) = 0;
            *(undefined *)(param_1 + 0x5c) = 2;
            uVar3 = 1;
            FUN_080085c8(param_1,param_1);
          }
        }
        return uVar3;
      }
      if (*(char *)(param_1 + 0xd) != '\0') {
        *(undefined *)(param_1 + 0xd) = 0;
        iVar2 = FUN_080085c8(param_1,param_1);
        if (iVar2 != 0) {
          *(undefined *)(param_1 + 0xd) = 0;
          *(undefined *)(param_1 + 0x5c) = 2;
          FUN_080085c8(param_1,param_1);
        }
      }
      goto LAB_080089f6;
    }
    cVar1 = *(char *)(param_1 + 0xd);
  }
  else {
    cVar1 = *(char *)(param_1 + 0xd);
  }
  if (cVar1 != '\0') {
    *(undefined *)(param_1 + 0xd) = 0;
    iVar2 = FUN_080085c8(param_1,param_1);
    if (iVar2 != 0) {
      *(undefined *)(param_1 + 0xd) = 0;
      *(undefined *)(param_1 + 0x5c) = 2;
      FUN_080085c8(param_1,param_1);
    }
  }
LAB_080089f6:
  *(undefined *)(param_1 + 0x5c) = 1;
  return 1;
}


