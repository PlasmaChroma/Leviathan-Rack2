/* 0801660c FUN_0801660c; analyst naming is provisional. */

void FUN_0801660c(uint **param_1)

{
  bool bVar1;
  uint *puVar2;
  
  puVar2 = *param_1;
  do {
    ExclusiveAccess(puVar2);
    bVar1 = (bool)hasExclusiveAccess(puVar2);
  } while (!bVar1);
  *puVar2 = *puVar2 & 0xfffffedf;
  do {
    ExclusiveAccess(puVar2 + 2);
    bVar1 = (bool)hasExclusiveAccess(puVar2 + 2);
  } while (!bVar1);
  puVar2[2] = puVar2[2] & DAT_08016674;
  if (param_1[0x1b] == (uint *)0x1) {
    do {
      ExclusiveAccess(puVar2);
      bVar1 = (bool)hasExclusiveAccess(puVar2);
      if (bVar1) {
        *puVar2 = *puVar2 & 0xffffffef;
        goto LAB_0801663e;
      }
      ExclusiveAccess(puVar2);
      bVar1 = (bool)hasExclusiveAccess(puVar2);
    } while (!bVar1);
    *puVar2 = *puVar2 & 0xffffffef;
  }
LAB_0801663e:
  param_1[0x23] = (uint *)0x20;
  param_1[0x1d] = (uint *)0x0;
  param_1[0x1b] = (uint *)0x0;
  return;
}


