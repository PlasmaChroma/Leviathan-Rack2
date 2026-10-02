/* 08008af4 FUN_08008af4; analyst naming is provisional. */

void FUN_08008af4(int *param_1)

{
  byte bVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 uVar4;
  int iVar5;
  byte *pbVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  uint uVar10;
  uint uVar11;
  int iVar12;
  uint uVar13;
  uint uVar14;
  uint uVar15;
  undefined4 local_34;
  undefined2 local_30;
  int local_2c;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  uint local_1c;
  
  iVar5 = DAT_08008c68;
  uVar4 = DAT_08008c64;
  puVar3 = DAT_08008c60;
  iVar2 = DAT_08008c5c;
  iVar12 = DAT_08008c58;
  uVar15 = 0;
  local_2c = 0;
  local_28 = 0;
  local_24 = 0;
  local_20 = 0;
  local_1c = 0;
  if (*param_1 != DAT_08008c54) {
    return;
  }
  *(uint *)(DAT_08008c58 + 0xd4) = *(uint *)(DAT_08008c58 + 0xd4) | 0x4000;
  uVar7 = *(uint *)(iVar12 + 0xd4) & 0x4000;
  *(uint *)(iVar12 + 0x7c) = *(uint *)(iVar12 + 0x7c) | 0x4000;
  *(uint *)(iVar12 + 0x7c) = *(uint *)(iVar12 + 0x7c) & 0xffffbfff;
  *(uint *)(iVar12 + 0xe0) = *(uint *)(iVar12 + 0xe0) | 0x40;
  uVar8 = *(uint *)(iVar12 + 0xe0) & 0x40;
  *(uint *)(iVar12 + 0xe0) = *(uint *)(iVar12 + 0xe0) | 0x20;
  uVar9 = *(uint *)(iVar12 + 0xe0) & 0x20;
  *(uint *)(iVar12 + 0xe0) = *(uint *)(iVar12 + 0xe0) | 0x10;
  uVar10 = *(uint *)(iVar12 + 0xe0) & 0x10;
  *(uint *)(iVar12 + 0xe0) = *(uint *)(iVar12 + 0xe0) | 2;
  uVar13 = *(uint *)(iVar12 + 0xe0) & 2;
  do {
    uVar14 = uVar15 & 0xff;
    if (*(int *)(iVar5 + 0x60) == 0) {
      if ((uVar15 & 0xfc) != 0) goto LAB_08008c3c;
      pbVar6 = *(byte **)(iVar5 + (uVar14 + 0x20) * 4);
      uVar11 = (uint)*pbVar6;
      iVar12 = 0;
      if (uVar11 < 0xb) {
        iVar12 = iVar2 + uVar11 * 0x400;
      }
      local_34 = uVar4;
      local_2c = 1 << pbVar6[1];
      bVar1 = *(byte *)((int)&local_34 + uVar14);
    }
    else {
      if (5 < uVar14) {
LAB_08008c3c:
        FUN_0800a968(0x5c,0,0,uVar14,uVar7,uVar8,uVar9,uVar10,uVar13);
        FUN_0800a9e4(0x5c);
        return;
      }
      pbVar6 = *(byte **)(iVar5 + (uVar14 + 0x1a) * 4);
      uVar11 = (uint)*pbVar6;
      if (uVar11 < 0xb) {
        iVar12 = iVar2 + uVar11 * 0x400;
      }
      else {
        iVar12 = 0;
      }
      local_2c = 1 << pbVar6[1];
      local_28 = 2;
      local_24 = 0;
      local_20 = 3;
      local_34 = *puVar3;
      local_30 = (short)puVar3[1];
      bVar1 = *(byte *)((int)&local_34 + uVar14);
    }
    local_20 = 3;
    local_24 = 0;
    local_28 = 2;
    local_1c = (uint)bVar1;
    uVar15 = uVar15 + 1;
    FUN_0800c080(iVar12,&local_2c);
  } while( true );
}


