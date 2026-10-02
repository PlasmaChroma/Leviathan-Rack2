/* 08009c70 FUN_08009c70; analyst naming is provisional. */

undefined4 FUN_08009c70(void)

{
  byte bVar1;
  uint *puVar2;
  uint uVar3;
  int iVar4;
  
  puVar2 = DAT_08009cc0;
  FUN_0800a944(3);
  uVar3 = FUN_080100f4();
  bVar1 = *(byte *)(DAT_08009cc8 + (*(uint *)(DAT_08009cc4 + 0x18) & 0xf));
  uVar3 = uVar3 >> (*(byte *)(DAT_08009cc8 + ((uint)(*(int *)(DAT_08009cc4 + 0x18) << 0x14) >> 0x1c)
                             ) & 0x1f);
  *DAT_08009ccc = uVar3;
  *puVar2 = uVar3 >> (bVar1 & 0x1f);
  iVar4 = FUN_08009c24(0xe);
  if (iVar4 != 0) {
    return 1;
  }
  FUN_08009c20();
  return 0;
}


