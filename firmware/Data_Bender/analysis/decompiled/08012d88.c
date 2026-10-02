/* 08012d88 FUN_08012d88; analyst naming is provisional. */

byte FUN_08012d88(int param_1)

{
  byte bVar1;
  byte bVar2;
  uint *puVar3;
  uint *puVar4;
  uint *puVar5;
  uint local_14;
  
  local_14 = 0;
  *(uint *)(param_1 + 8) = *(uint *)(param_1 + 8) & 0xfffffffe;
  bVar1 = FUN_080125b4(param_1,0x10);
  bVar2 = FUN_08012608(param_1);
  bVar2 = bVar2 | bVar1;
  puVar3 = (uint *)(param_1 + 0x500);
  puVar4 = puVar3;
  if (bVar2 != 0) {
    bVar2 = 1;
  }
  do {
    puVar5 = puVar4 + 8;
    *puVar4 = *puVar4 & 0x7fff7fff | 0x40000000;
    puVar4 = puVar5;
  } while (puVar5 != (uint *)(param_1 + 0x700));
  do {
    *puVar3 = *puVar3 & 0xffff7fff | 0xc0000000;
    do {
      local_14 = local_14 + 1;
      if (1000 < local_14) break;
    } while ((int)*puVar3 < 0);
    puVar3 = puVar3 + 8;
    if (puVar3 == (uint *)(param_1 + 0x700)) {
      *(undefined4 *)(param_1 + 0x414) = 0xffffffff;
      *(undefined4 *)(param_1 + 0x14) = 0xffffffff;
      *(uint *)(param_1 + 8) = *(uint *)(param_1 + 8) | 1;
      return bVar2;
    }
  } while( true );
}


