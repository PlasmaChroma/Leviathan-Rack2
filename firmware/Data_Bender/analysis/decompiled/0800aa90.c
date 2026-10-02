/* 0800aa90 FUN_0800aa90; analyst naming is provisional. */

void FUN_0800aa90(byte *param_1)

{
  byte bVar1;
  int iVar2;
  
  iVar2 = DAT_0800aaf0;
  *(uint *)(DAT_0800aaf0 + 0x98) = (uint)param_1[1];
  bVar1 = *param_1;
  if (bVar1 != 0) {
    *(undefined4 *)(iVar2 + 0x9c) = *(undefined4 *)(param_1 + 4);
    *(uint *)(iVar2 + 0xa0) =
         (uint)param_1[0xb] << 0x18 | (uint)param_1[0xc] << 0x1c | (uint)bVar1 |
         (uint)param_1[10] << 0x13 | (uint)param_1[0xd] << 0x12 | (uint)param_1[0xe] << 0x11 |
         (uint)param_1[0xf] << 0x10 | (uint)param_1[9] << 8 | (uint)param_1[8] << 1;
    return;
  }
  *(undefined4 *)(iVar2 + 0x9c) = 0;
  *(undefined4 *)(iVar2 + 0xa0) = 0;
  return;
}


