/* 08013f2c FUN_08013f2c; analyst naming is provisional. */

void FUN_08013f2c(uint *param_1)

{
  code *pcVar1;
  int iVar2;
  int iVar3;
  uint *puVar4;
  uint local_28 [4];
  uint local_18;
  uint uStack_14;
  
  iVar3 = 0;
  local_28[0] = *DAT_080140a4;
  local_28[1] = DAT_080140a4[1];
  local_28[2] = DAT_080140a4[2];
  local_28[3] = DAT_080140a4[3];
  local_18 = DAT_080140a4[4];
  uStack_14 = DAT_080140a4[5];
  puVar4 = local_28;
  while (*param_1 != *puVar4) {
    iVar3 = iVar3 + 1;
    puVar4 = puVar4 + 1;
    if (iVar3 == 6) {
                    /* WARNING: Does not return */
      pcVar1 = (code *)software_udf(0xff,0x80140a2);
      (*pcVar1)();
    }
  }
  iVar2 = DAT_080140a8 + iVar3 * 0x1a0;
  FUN_080137a4(*(undefined *)(DAT_080140a8 + iVar3 * 0x1a0));
  FUN_080137a4(*(undefined *)(iVar2 + 2));
  FUN_080137a4(*(undefined *)(iVar2 + 4));
  FUN_080137a4(*(undefined *)(iVar2 + 6));
  iVar3 = DAT_080140ac;
  switch(*(undefined4 *)(iVar2 + 8)) {
  case 0:
    *(uint *)(DAT_080140ac + 0xf0) = *(uint *)(DAT_080140ac + 0xf0) | 0x1000;
    FUN_0800a968(0x23,0,0,*(uint *)(iVar3 + 0xf0) & 0x1000);
    FUN_0800a9e4(0x23);
    break;
  case 1:
    *(uint *)(DAT_080140ac + 0xe8) = *(uint *)(DAT_080140ac + 0xe8) | 0x4000;
    FUN_0800a968(0x24,0,0,*(uint *)(iVar3 + 0xe8) & 0x4000);
    FUN_0800a9e4(0x24);
    break;
  case 2:
    *(uint *)(DAT_080140ac + 0xe8) = *(uint *)(DAT_080140ac + 0xe8) | 0x8000;
    FUN_0800a968(0x33,0,0,*(uint *)(iVar3 + 0xe8) & 0x8000);
    FUN_0800a9e4(0x33);
    break;
  case 3:
    *(uint *)(DAT_080140ac + 0xf0) = *(uint *)(DAT_080140ac + 0xf0) | 0x2000;
    FUN_0800a968(0x54,0,0,*(uint *)(iVar3 + 0xf0) & 0x2000);
    FUN_0800a9e4(0x54);
    break;
  case 4:
    *(uint *)(DAT_080140ac + 0xf0) = *(uint *)(DAT_080140ac + 0xf0) | 0x100000;
    FUN_0800a968(0x55,0,0,*(uint *)(iVar3 + 0xf0) & 0x100000);
    FUN_0800a9e4(0x55);
    break;
  case 5:
    *(uint *)(DAT_080140ac + 0xf4) = *(uint *)(DAT_080140ac + 0xf4) | 0x20;
    local_28[0] = *(uint *)(iVar3 + 0xf4) & 0x20;
  }
  iVar3 = FUN_08013dc4(iVar2);
  if (iVar3 == 1) {
    software_bkpt(0xff);
    do {
                    /* WARNING: Do nothing block with infinite loop */
    } while( true );
  }
  return;
}


