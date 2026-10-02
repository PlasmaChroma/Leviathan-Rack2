/* 0800627c FUN_0800627c; analyst naming is provisional. */

void FUN_0800627c(void)

{
  int iVar1;
  undefined4 uVar2;
  uint uVar3;
  int iVar4;
  undefined4 local_2c;
  undefined4 local_28;
  int local_24;
  undefined4 local_20;
  undefined4 local_1c;
  
  iVar1 = DAT_080063f8;
  iVar4 = *DAT_080063f4;
  local_24 = 0;
  if (iVar4 == 0) {
    *DAT_080063f4 = 1;
    uVar2 = DAT_080063fc;
    *(uint *)(iVar1 + 0xd4) = *(uint *)(iVar1 + 0xd4) | 0x1000;
    *(uint *)(iVar1 + 0xe0) = *(uint *)(iVar1 + 0xe0) | 0x10;
    *(uint *)(iVar1 + 0xe0) = *(uint *)(iVar1 + 0xe0) | 0x40;
    *(uint *)(iVar1 + 0xe0) = *(uint *)(iVar1 + 0xe0) | 8;
    *(uint *)(iVar1 + 0xe0) = *(uint *)(iVar1 + 0xe0) | 0x100;
    *(uint *)(iVar1 + 0xe0) = *(uint *)(iVar1 + 0xe0) | 0x80;
    *(uint *)(iVar1 + 0xe0) = *(uint *)(iVar1 + 0xe0) | 0x20;
    uVar3 = *(uint *)(iVar1 + 0xe0) | 4;
    *(uint *)(iVar1 + 0xe0) = uVar3;
    local_2c = 0xff83;
    local_28 = 2;
    local_20 = 3;
    local_1c = 0xc;
    FUN_0800c080(uVar2,&local_2c,uVar3,*(uint *)(iVar1 + 0xe0) & 4);
    local_2c = 0x8137;
    local_28 = 2;
    local_20 = 3;
    local_1c = 0xc;
    local_24 = iVar4;
    FUN_0800c080(DAT_08006400,&local_2c);
    local_2c = 0xc703;
    local_28 = 2;
    local_20 = 3;
    local_1c = 0xc;
    local_24 = iVar4;
    FUN_0800c080(DAT_08006404,&local_2c);
    local_2c = 0x6ff;
    local_28 = 2;
    local_20 = 3;
    local_1c = 0xc;
    local_24 = iVar4;
    FUN_0800c080(DAT_08006408,&local_2c);
    local_2c = 0xff0c;
    local_28 = 2;
    local_20 = 3;
    local_1c = 0xc;
    local_24 = iVar4;
    FUN_0800c080(DAT_0800640c,&local_2c);
    local_2c = 0xf83f;
    local_28 = 2;
    local_20 = 3;
    local_1c = 0xc;
    local_24 = iVar4;
    FUN_0800c080(DAT_08006410,&local_2c);
    local_28 = 2;
    local_1c = 0xc;
    local_2c = 0x20;
    local_20 = 3;
    local_24 = iVar4;
    FUN_0800c080(DAT_0800640c,&local_2c);
  }
  return;
}


