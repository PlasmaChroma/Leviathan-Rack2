/* 080138f4 FUN_080138f4; analyst naming is provisional. */

void FUN_080138f4(void)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  
  puVar1 = DAT_08013938;
  puVar3 = DAT_08013938 + 0x1c;
  *DAT_0801393c = 0xff;
  do {
    puVar2 = puVar1 + 7;
    *puVar1 = 0;
    puVar1[1] = 0;
    puVar1[2] = 0;
    puVar1[3] = 0;
    puVar1[4] = 0;
    puVar1[5] = 0;
    puVar1[6] = 1;
    puVar1 = puVar2;
  } while (puVar2 != puVar3);
  return;
}


