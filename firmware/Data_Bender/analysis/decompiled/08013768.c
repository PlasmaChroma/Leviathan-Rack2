/* 08013768 FUN_08013768; analyst naming is provisional. */

void FUN_08013768(void)

{
  uint *puVar1;
  
  puVar1 = DAT_080137a0;
  *(uint *)(DAT_0801379c + 0xdc) = *(uint *)(DAT_0801379c + 0xdc) | 0x40;
  *puVar1 = *puVar1 & 0xffffffdf;
  *puVar1 = *puVar1 | 4;
  return;
}


