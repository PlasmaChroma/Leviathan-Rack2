/* 08010388 FUN_08010388; analyst naming is provisional. */

void FUN_08010388(void)

{
  byte bVar1;
  uint *puVar2;
  uint uVar3;
  uint uVar4;
  
  uVar4 = DAT_080103ec[4] & 0x38;
  uVar3 = DAT_08010400;
  if (uVar4 != 0x10) {
    if (uVar4 == 0x18) {
      uVar3 = FUN_0800f9d0();
    }
    else {
      uVar3 = DAT_080103f0;
      if ((uVar4 == 0) && (uVar3 = DAT_08010404, *DAT_080103ec << 0x1a < 0)) {
        uVar3 = DAT_08010404 >> ((uint)(*DAT_080103ec << 0x1b) >> 0x1e);
      }
    }
  }
  puVar2 = DAT_080103f8;
  bVar1 = *(byte *)(DAT_080103f4 + (DAT_080103ec[6] & 0xfU));
  uVar3 = uVar3 >> (*(byte *)(DAT_080103f4 + ((uint)(DAT_080103ec[6] << 0x14) >> 0x1c)) & 0x1f);
  *DAT_080103fc = uVar3;
  *puVar2 = uVar3 >> (bVar1 & 0x1f);
  return;
}


