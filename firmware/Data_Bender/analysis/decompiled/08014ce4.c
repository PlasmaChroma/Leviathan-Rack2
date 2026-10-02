/* 08014ce4 FUN_08014ce4; analyst naming is provisional. */

void FUN_08014ce4(uint *param_1)

{
  code *pcVar1;
  int iVar2;
  int extraout_r1;
  int extraout_r1_00;
  int iVar3;
  int iVar4;
  uint *puVar5;
  uint local_40;
  uint local_3c [4];
  undefined4 local_2c;
  uint uStack_28;
  uint uStack_24;
  uint uStack_20;
  uint local_1c;
  
  iVar4 = DAT_08014e94;
  iVar3 = 0;
  puVar5 = &local_40;
  local_3c[0] = *DAT_08014e90;
  local_3c[1] = DAT_08014e90[1];
  local_3c[2] = DAT_08014e90[2];
  local_3c[3] = DAT_08014e90[3];
  local_2c = DAT_08014e90[4];
  uStack_28 = DAT_08014e90[5];
  uStack_24 = DAT_08014e90[6];
  uStack_20 = DAT_08014e90[7];
  local_1c = DAT_08014e90[8];
  while (puVar5 = puVar5 + 1, *param_1 != *puVar5) {
    iVar3 = iVar3 + 1;
    if (iVar3 == 9) {
                    /* WARNING: Does not return */
      pcVar1 = (code *)software_udf(0xff,0x8014e8e);
      (*pcVar1)();
    }
  }
  FUN_080146e4(*(undefined *)(iVar3 * 0x1b8 + DAT_08014e94 + 0x1a));
  FUN_080146e4(*(undefined *)(extraout_r1 + 0x18));
  iVar2 = DAT_08014e98;
  switch(*(undefined4 *)(extraout_r1_00 + 0x1c)) {
  case 0:
    *(uint *)(DAT_08014e98 + 0xf0) = *(uint *)(DAT_08014e98 + 0xf0) | 0x10;
    break;
  case 1:
    *(uint *)(DAT_08014e98 + 0xe8) = *(uint *)(DAT_08014e98 + 0xe8) | 0x20000;
    break;
  case 2:
    *(uint *)(DAT_08014e98 + 0xe8) = *(uint *)(DAT_08014e98 + 0xe8) | 0x40000;
    break;
  case 3:
    *(uint *)(DAT_08014e98 + 0xe8) = *(uint *)(DAT_08014e98 + 0xe8) | 0x80000;
    break;
  case 4:
    *(uint *)(DAT_08014e98 + 0xe8) = *(uint *)(DAT_08014e98 + 0xe8) | 0x100000;
    break;
  case 5:
    *(uint *)(DAT_08014e98 + 0xf0) = *(uint *)(DAT_08014e98 + 0xf0) | 0x20;
    break;
  case 6:
    *(uint *)(DAT_08014e98 + 0xe8) = *(uint *)(DAT_08014e98 + 0xe8) | 0x40000000;
    break;
  case 7:
    *(uint *)(DAT_08014e98 + 0xe8) = *(uint *)(DAT_08014e98 + 0xe8) | 0x80000000;
    break;
  case 8:
    *(uint *)(DAT_08014e98 + 0xf4) = *(uint *)(DAT_08014e98 + 0xf4) | 8;
    local_40 = *(uint *)(iVar2 + 0xf4) & 8;
  }
  iVar2 = FUN_08014c50(extraout_r1_00);
  if (iVar2 != 1) {
    iVar4 = iVar3 * 0x1b8 + iVar4;
    local_3c[0] = *DAT_08014e9c;
    local_3c[1] = DAT_08014e9c[1];
    local_3c[2] = DAT_08014e9c[2];
    local_3c[3] = DAT_08014e9c[3];
    local_2c = CONCAT22(local_2c._2_2_,(short)DAT_08014e9c[4]);
    FUN_0800a968((int)*(short *)((int)local_3c + *(int *)(iVar4 + 0x1c) * 2),0);
    FUN_0800a9e4((int)*(short *)((int)local_3c + *(int *)(iVar4 + 0x1c) * 2));
    return;
  }
  software_bkpt(0xff);
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}


