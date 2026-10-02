/* 08015f6c FUN_08015f6c; analyst naming is provisional. */

bool FUN_08015f6c(uint **param_1)

{
  uint *puVar1;
  bool bVar2;
  uint *puVar3;
  bool bVar4;
  
  puVar1 = DAT_08016008;
  if (*(char *)((int)param_1 + 0x3d) != '\x01') {
    return true;
  }
  puVar3 = *param_1;
  bVar2 = puVar3 == DAT_08016004;
  bVar4 = puVar3 == DAT_0801600c;
  *(undefined *)((int)param_1 + 0x3d) = 2;
  if ((puVar3 == puVar1 + 0x400 ||
       (puVar3 == DAT_08016014 ||
       (puVar3 == DAT_08016010 ||
       (puVar3 == puVar1 || (bVar4 || (puVar3 == (uint *)0x40000000 || bVar2)))))) ||
     (bVar2 = puVar3 == DAT_08016014 + 0xf00, bVar2)) {
    if (((DAT_08016018 & puVar3[2]) == 6) || ((DAT_08016018 & puVar3[2]) == 0x10000)) {
      return false;
    }
    bVar2 = false;
    *puVar3 = *puVar3 | 1;
  }
  else {
    *puVar3 = *puVar3 | 1;
  }
  return bVar2;
}


