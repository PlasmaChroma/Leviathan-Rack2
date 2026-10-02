/* 0800822c FUN_0800822c; analyst naming is provisional. */

undefined4 FUN_0800822c(int param_1)

{
  int iVar1;
  undefined4 local_48 [5];
  undefined4 local_34;
  undefined4 local_30;
  undefined4 uStack_2c;
  undefined4 local_28;
  undefined4 uStack_24;
  undefined4 local_1c;
  undefined4 uStack_18;
  undefined4 local_14;
  
  local_48[0] = 0x66;
  local_34 = 0;
  local_14 = 0;
  local_30 = 0x100;
  uStack_2c = 0;
  local_28 = 0;
  uStack_24 = 0;
  local_1c = 0;
  uStack_18 = 0;
  iVar1 = FUN_0800f32c(param_1 + 0x10,local_48,5000);
  if (iVar1 == 0) {
    local_48[0] = 0x99;
    iVar1 = FUN_0800f32c(param_1 + 0x10,local_48,5000);
    if ((iVar1 == 0) && (iVar1 = FUN_080081d8(param_1,5000), iVar1 == 0)) {
      return 0;
    }
  }
  *(undefined *)(param_1 + 0x5c) = 1;
  return 1;
}


