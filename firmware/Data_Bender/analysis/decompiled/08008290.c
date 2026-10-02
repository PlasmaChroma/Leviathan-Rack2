/* 08008290 FUN_08008290; analyst naming is provisional. */

undefined4 FUN_08008290(int param_1,int param_2)

{
  int iVar1;
  ushort local_4a;
  undefined4 local_48 [5];
  undefined4 local_34;
  undefined4 local_30;
  undefined4 uStack_2c;
  undefined4 local_28;
  undefined4 uStack_24;
  undefined4 local_20;
  undefined4 uStack_1c;
  undefined4 local_18;
  undefined4 uStack_14;
  
  local_4a = 0;
  local_34 = 0;
  local_28 = 0;
  uStack_24 = 0x1000000;
  local_30 = 0x100;
  uStack_2c = 0;
  local_20 = 1;
  uStack_1c = 0;
  local_18 = 0;
  uStack_14 = 0;
  if (param_2 == 0) {
    local_48[0] = 0x61;
    iVar1 = FUN_0800f32c(param_1 + 0x10,local_48,5000);
    if ((iVar1 != 0) || (iVar1 = FUN_0800f4e0(param_1 + 0x10,&local_4a,5000), iVar1 != 0))
    goto LAB_080082d8;
    local_4a = local_4a & 0xff87 | 0x40;
    iVar1 = FUN_080080f0(param_1);
    if (iVar1 != 0) goto LAB_080082d8;
  }
  else {
    local_4a = 0xf0;
  }
  local_48[0] = 0xc0;
  iVar1 = FUN_0800f32c(param_1 + 0x10,local_48,5000);
  if (((iVar1 == 0) && (iVar1 = FUN_0800f3f0(param_1 + 0x10,&local_4a,5000), iVar1 == 0)) &&
     (iVar1 = FUN_080081d8(param_1,5000), iVar1 == 0)) {
    return 0;
  }
LAB_080082d8:
  *(undefined *)(param_1 + 0x5c) = 1;
  return 1;
}


