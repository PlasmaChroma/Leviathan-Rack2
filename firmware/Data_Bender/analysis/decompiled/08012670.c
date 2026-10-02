/* 08012670 FUN_08012670; analyst naming is provisional. */

undefined4 FUN_08012670(int param_1,byte *param_2)

{
  uint uVar1;
  int iVar2;
  
  uVar1 = (uint)*param_2;
  if (param_2[1] == 1) {
    iVar2 = param_1 + uVar1 * 0x20;
    *(uint *)(param_1 + 0x81c) = 1 << (uVar1 & 0xf) | *(uint *)(param_1 + 0x81c);
    if (-1 < *(int *)(iVar2 + 0x900) << 0x10) {
      *(uint *)(iVar2 + 0x900) =
           DAT_080126fc |
           *(uint *)(param_2 + 8) & 0x7ff | *(uint *)(iVar2 + 0x900) | (uint)param_2[4] << 0x12 |
           uVar1 << 0x16;
      return 0;
    }
  }
  else {
    iVar2 = param_1 + uVar1 * 0x20;
    *(uint *)(param_1 + 0x81c) = 0x10000 << (uVar1 & 0xf) | *(uint *)(param_1 + 0x81c);
    if (-1 < *(int *)(iVar2 + 0xb00) << 0x10) {
      *(uint *)(iVar2 + 0xb00) =
           DAT_080126fc |
           *(uint *)(param_2 + 8) & 0x7ff | *(uint *)(iVar2 + 0xb00) | (uint)param_2[4] << 0x12;
    }
  }
  return 0;
}


