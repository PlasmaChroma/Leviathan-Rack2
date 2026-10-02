/* 0801601c FUN_0801601c; analyst naming is provisional. */

bool FUN_0801601c(uint **param_1)

{
  uint *puVar1;
  uint *puVar2;
  bool bVar3;
  uint *puVar4;
  bool bVar5;
  
  puVar1 = DAT_080160c4;
  if (*(char *)((int)param_1 + 0x3d) != '\x01') {
    return true;
  }
  puVar4 = *param_1;
  bVar3 = puVar4 == DAT_080160bc;
  bVar5 = puVar4 == DAT_080160c0;
  *(undefined *)((int)param_1 + 0x3d) = 2;
  puVar2 = DAT_080160c8;
  puVar4[3] = puVar4[3] | 1;
  if ((puVar4 == DAT_080160cc ||
       (puVar4 == puVar2 ||
       (puVar4 == puVar1 + 0x100 ||
       (puVar4 == puVar1 || (bVar5 || (puVar4 == (uint *)0x40000000 || bVar3)))))) ||
     (bVar3 = puVar4 == puVar2 + 0xf00, bVar3)) {
    if (((DAT_080160d0 & puVar4[2]) == 6) || ((DAT_080160d0 & puVar4[2]) == 0x10000)) {
      return false;
    }
    bVar3 = false;
    *puVar4 = *puVar4 | 1;
  }
  else {
    *puVar4 = *puVar4 | 1;
  }
  return bVar3;
}


