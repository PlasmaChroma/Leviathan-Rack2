/* 08010544 FUN_08010544; analyst naming is provisional. */

undefined4 FUN_08010544(int *param_1,int param_2)

{
  uint *puVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  
  puVar1 = DAT_08010630;
  if ((DAT_08010630[10] & 3) == 3) {
    return 1;
  }
  *DAT_08010630 = *DAT_08010630 & 0xefffffff;
  iVar2 = FUN_08009ce8();
  while ((int)(*puVar1 << 2) < 0) {
    iVar3 = FUN_08009ce8();
    if (2 < (uint)(iVar3 - iVar2)) {
      return 3;
    }
  }
  puVar1[10] = puVar1[10] & 0xfc0fffff | *param_1 << 0x14;
  puVar1[0x10] = (param_1[2] + -1) * 0x200 & 0xffffU | (param_1[3] + -1) * 0x10000 & 0x7f0000U |
                 param_1[1] - 1U & 0x1ff | (param_1[4] + -1) * 0x1000000 & 0x7f000000U;
  puVar1[0xb] = puVar1[0xb] & 0xfffff3ff | param_1[5];
  uVar4 = DAT_08010634;
  puVar1[0xb] = puVar1[0xb] & 0xfffffdff | param_1[6];
  puVar1[0xb] = puVar1[0xb] & 0xfffffeff;
  puVar1[0x11] = uVar4 & puVar1[0x11] | param_1[7] << 3;
  puVar1[0xb] = puVar1[0xb] | 0x100;
  uVar4 = puVar1[0xb];
  if (param_2 == 0) {
    puVar1[0xb] = uVar4 | 0x400000;
  }
  else {
    if (param_2 == 1) {
      uVar4 = uVar4 | 0x800000;
    }
    else {
      uVar4 = uVar4 | 0x1000000;
    }
    puVar1[0xb] = uVar4;
  }
  puVar1 = DAT_08010630;
  *DAT_08010630 = *DAT_08010630 | 0x10000000;
  iVar2 = FUN_08009ce8();
  do {
    if ((int)(*puVar1 << 2) < 0) {
      return 0;
    }
    iVar3 = FUN_08009ce8();
  } while ((uint)(iVar3 - iVar2) < 3);
  return 3;
}


