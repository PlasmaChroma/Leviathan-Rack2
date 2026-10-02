/* 0800797c FUN_0800797c; analyst naming is provisional. */

void FUN_0800797c(void)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  
  puVar1 = DAT_080079c0;
  puVar3 = DAT_080079c0 + 0x12;
  *DAT_080079c4 = 0xff;
  do {
    puVar2 = puVar1 + 6;
    *puVar1 = 0x10;
    puVar1[1] = 0;
    puVar1[2] = 0;
    puVar1[3] = 0;
    puVar1[4] = 0;
    puVar1[5] = 0;
    puVar1 = puVar2;
  } while (puVar2 != puVar3);
  return;
}


