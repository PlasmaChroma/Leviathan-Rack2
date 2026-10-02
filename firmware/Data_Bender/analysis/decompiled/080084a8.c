/* 080084a8 FUN_080084a8; analyst naming is provisional. */

undefined4 FUN_080084a8(int param_1,undefined4 param_2)

{
  byte bVar1;
  byte bVar2;
  byte bVar3;
  byte bVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  undefined4 uVar8;
  undefined4 local_2c;
  undefined4 uStack_28;
  undefined4 local_24;
  undefined4 uStack_20;
  undefined4 local_1c;
  
  FUN_0800c490(DAT_080085b8,0xc0,1);
  iVar5 = DAT_080085bc;
  uVar8 = DAT_080085b8;
  local_1c = 0;
  bVar2 = (byte)param_2;
  bVar3 = (byte)((uint)param_2 >> 8);
  bVar4 = (byte)((uint)param_2 >> 0x10);
  bVar1 = (byte)((uint)param_2 >> 0x18);
  uVar7 = (uint)(byte)((((((((bVar2 & 1) << 1 | bVar2 >> 1 & 1) << 1 | bVar2 >> 2 & 1) << 1 |
                          bVar2 >> 3 & 1) << 1 | bVar2 >> 4 & 1) << 1 | bVar2 >> 5 & 1) << 1 |
                       bVar2 >> 6 & 1) << 1 | bVar2 >> 7) << 0x18 |
          (uint)(byte)((((((((bVar3 & 1) << 1 | bVar3 >> 1 & 1) << 1 | bVar3 >> 2 & 1) << 1 |
                          bVar3 >> 3 & 1) << 1 | bVar3 >> 4 & 1) << 1 | bVar3 >> 5 & 1) << 1 |
                       bVar3 >> 6 & 1) << 1 | bVar3 >> 7) << 0x10 |
          (uint)(byte)((((((((bVar4 & 1) << 1 | bVar4 >> 1 & 1) << 1 | bVar4 >> 2 & 1) << 1 |
                          bVar4 >> 3 & 1) << 1 | bVar4 >> 4 & 1) << 1 | bVar4 >> 5 & 1) << 1 |
                       bVar4 >> 6 & 1) << 1 | bVar4 >> 7) << 8 |
          (uint)(byte)((((((((bVar1 & 1) << 1 | bVar1 >> 1 & 1) << 1 | bVar1 >> 2 & 1) << 1 |
                          bVar1 >> 3 & 1) << 1 | bVar1 >> 4 & 1) << 1 | bVar1 >> 5 & 1) << 1 |
                       bVar1 >> 6 & 1) << 1 | bVar1 >> 7);
  local_24 = 0;
  uStack_20 = 0;
  uVar6 = *(uint *)(DAT_080085bc + 0xe0) | 0x20;
  *(uint *)(DAT_080085bc + 0xe0) = uVar6;
  local_2c = 0xc0;
  uStack_28 = 1;
  FUN_0800c080(uVar8,&local_2c,uVar6,*(uint *)(iVar5 + 0xe0) & 0x20);
  FUN_08009cf4(0x14);
  FUN_0800c490(DAT_080085b8,0xc0,0);
  FUN_08009cf4(0x14);
  FUN_0800c490(DAT_080085b8,0xc0,1);
  FUN_08009cf4(0x14);
  uVar8 = DAT_080085c0;
  *(undefined4 *)(param_1 + 0x1c) = 0;
  *(undefined4 *)(param_1 + 0x10) = uVar8;
  *(undefined4 *)(param_1 + 0x14) = 1;
  *(undefined4 *)(param_1 + 0x18) = 1;
  if (uVar7 == 0) {
    iVar5 = 0x1f;
  }
  else {
    iVar5 = LZCOUNT(uVar7) + -1;
  }
  *(undefined4 *)(param_1 + 0x2c) = 0;
  *(undefined4 *)(param_1 + 0x30) = 0;
  *(int *)(param_1 + 0x20) = iVar5;
  *(undefined4 *)(param_1 + 0x24) = 0x100;
  iVar5 = FUN_0800f250(param_1 + 0x10);
  if ((((iVar5 == 0) && (iVar5 = FUN_0800822c(param_1), iVar5 == 0)) &&
      (iVar5 = FUN_08008400(param_1), iVar5 == 0)) &&
     (iVar5 = FUN_0800f308(param_1 + 0x10), iVar5 == 0)) {
    iVar5 = 0;
    if (*(byte *)(param_1 + 4) < 0xb) {
      iVar5 = DAT_080085c4 + (uint)*(byte *)(param_1 + 4) * 0x400;
    }
    FUN_0800c2f4(iVar5,1 << (uint)*(byte *)(param_1 + 5));
    if (*(byte *)(param_1 + 6) < 0xb) {
      iVar5 = DAT_080085c4 + (uint)*(byte *)(param_1 + 6) * 0x400;
    }
    else {
      iVar5 = 0;
    }
    FUN_0800c2f4(iVar5,1 << *(sbyte *)(param_1 + 7));
    *(undefined4 *)(param_1 + 0x60) = 1;
    *(undefined *)(param_1 + 100) = 1;
    uVar8 = 0;
  }
  else {
    uVar8 = 1;
    *(undefined *)(param_1 + 0x5c) = 1;
  }
  return uVar8;
}


