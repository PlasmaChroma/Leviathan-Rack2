/* 08007214 FUN_08007214; analyst naming is provisional. */

void FUN_08007214(char *param_1,int param_2,uint param_3,char param_4)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  undefined4 uVar4;
  undefined uVar5;
  int iVar6;
  int iVar7;
  undefined4 *puVar8;
  uint uVar9;
  undefined2 *puVar10;
  char *pcVar11;
  int iVar12;
  undefined4 local_74;
  int local_70;
  undefined4 uStack_6c;
  undefined4 local_68;
  undefined4 local_64;
  undefined4 local_60;
  undefined4 local_5c;
  undefined4 local_58;
  undefined4 uStack_54;
  undefined4 local_50;
  undefined4 uStack_4c;
  undefined4 local_48;
  undefined4 uStack_44;
  undefined4 uStack_40;
  undefined4 uStack_3c;
  undefined4 local_38;
  undefined4 uStack_34;
  undefined4 uStack_30;
  undefined4 uStack_2c;
  
  iVar3 = DAT_08007524;
  puVar10 = DAT_08007520;
  iVar7 = 0;
  *(uint *)(param_1 + 4) = param_3;
  *param_1 = param_4;
  *(undefined2 **)(iVar3 + 0x588) = puVar10 + 0x20;
  *(undefined2 **)(iVar3 + 0x584) = puVar10;
  local_70 = 0;
  uStack_6c = 0;
  local_68 = 0;
  local_64 = 0;
  local_60 = 0;
  local_5c = 0;
  local_58 = 0;
  uStack_54 = 0;
  local_50 = 0;
  uStack_4c = 0;
  memset(puVar10,0,0x20);
  *(undefined4 *)(iVar3 + 0x551) = 0;
  *(undefined4 *)(iVar3 + 0x555) = 0;
  *(undefined4 *)(iVar3 + 0x559) = 0;
  *(undefined4 *)(iVar3 + 0x55d) = 0;
  memset(iVar3 + 0x562,0,0x20);
  *(char *)(iVar3 + 0x550) = (char)param_3;
  iVar6 = *(int *)(param_1 + 4);
  *(undefined *)(iVar3 + 0x668) = 0;
  if (iVar6 == 0) {
    *(undefined4 *)(iVar3 + 0x594) = 0;
    *(undefined *)(iVar3 + 0x5a0) = 0;
    *(undefined4 *)(iVar3 + 0x5b0) = 0;
    *(undefined4 *)(iVar3 + 0x5b4) = 0;
    uVar4 = DAT_08007528;
    *(uint *)(iVar3 + 0x5a4) = param_3 & 0xff;
    *(undefined4 *)(iVar3 + 0x58c) = uVar4;
    *(undefined4 *)(iVar3 + 0x590) = 0x40000;
    *(undefined4 *)(iVar3 + 0x598) = 1;
    *(undefined4 *)(iVar3 + 0x59c) = 8;
  }
  else {
    pcVar11 = (char *)(iVar3 + 0x550);
    uVar9 = 0;
    do {
      uVar9 = uVar9 + 1;
      libc_memcpy(iVar3 + iVar7,param_2 + iVar7,0x52);
      *puVar10 = 0;
      cVar2 = *(char *)(param_2 + 0x50 + iVar7);
      iVar7 = iVar7 + 0x54;
      pcVar11 = pcVar11 + 1;
      *pcVar11 = cVar2;
      if (cVar2 != '\0') {
        *(undefined *)(iVar3 + 0x668) = 1;
      }
      puVar10 = puVar10 + 1;
    } while (uVar9 < *(uint *)(param_1 + 4));
    *(undefined4 *)(iVar3 + 0x58c) = DAT_08007528;
    *(uint *)(iVar3 + 0x5a4) = (uint)*(byte *)(iVar3 + 0x550);
    *(undefined4 *)(iVar3 + 0x590) = 0x40000;
    *(undefined4 *)(iVar3 + 0x59c) = 8;
    *(undefined4 *)(iVar3 + 0x594) = 0;
    *(undefined *)(iVar3 + 0x5a0) = 0;
    *(undefined4 *)(iVar3 + 0x5b0) = 0;
    *(undefined4 *)(iVar3 + 0x5b4) = 0;
    *(undefined4 *)(iVar3 + 0x598) = 1;
    if (*(char *)(iVar3 + 0x668) != '\0') {
      *(undefined4 *)(iVar3 + 0x5b8) = 1;
      *(undefined *)(iVar3 + 0x5a1) = 0;
      *(undefined *)(iVar3 + 0x5a8) = 0;
      goto LAB_08007304;
    }
  }
  *(undefined *)(iVar3 + 0x5a1) = 1;
  *(undefined *)(iVar3 + 0x5a8) = 0;
  *(undefined4 *)(iVar3 + 0x5b8) = 3;
LAB_08007304:
  cVar2 = *param_1;
  *(undefined4 *)(iVar3 + 0x5bc) = 0;
  *(undefined4 *)(iVar3 + 0x5c0) = 0;
  if (cVar2 == '\0') {
    *(undefined *)(iVar3 + 0x5c4) = 0;
  }
  else {
    *(undefined4 *)(iVar3 + 0x5d0) = 0;
    *(undefined *)(iVar3 + 0x5c4) = 1;
    *(undefined4 *)(iVar3 + 0x5d4) = 1;
    switch(cVar2) {
    case '\x01':
      *(undefined4 *)(iVar3 + 0x5cc) = 0x40;
      *(undefined4 *)(iVar3 + 0x5c8) = 3;
      break;
    case '\x02':
      *(undefined4 *)(iVar3 + 0x5cc) = 0x60;
      *(undefined4 *)(iVar3 + 0x5c8) = 7;
      break;
    case '\x03':
      *(undefined4 *)(iVar3 + 0x5cc) = 0x80;
      *(undefined4 *)(iVar3 + 0x5c8) = 0xf;
      break;
    case '\x04':
      *(undefined4 *)(iVar3 + 0x5cc) = 0xa0;
      *(undefined4 *)(iVar3 + 0x5c8) = 0x1f;
      break;
    case '\x05':
      *(undefined4 *)(iVar3 + 0x5cc) = 0xc0;
      *(undefined4 *)(iVar3 + 0x5c8) = 0x3f;
      break;
    case '\x06':
      *(undefined4 *)(iVar3 + 0x5cc) = 0xe0;
      *(undefined4 *)(iVar3 + 0x5c8) = 0x7f;
      break;
    case '\a':
      *(undefined4 *)(iVar3 + 0x5cc) = 0x100;
      *(undefined4 *)(iVar3 + 0x5c8) = 0xff;
      break;
    case '\b':
      *(undefined4 *)(iVar3 + 0x5cc) = 0x120;
      *(undefined4 *)(iVar3 + 0x5c8) = 0x1ff;
      break;
    case '\t':
      *(undefined4 *)(iVar3 + 0x5cc) = 0x140;
      *(undefined4 *)(iVar3 + 0x5c8) = 0x3ff;
    }
  }
  local_70 = FUN_0800a5b4(DAT_0800752c);
  if (local_70 != 0) {
    software_bkpt(0xff);
    do {
                    /* WARNING: Do nothing block with infinite loop */
    } while( true );
  }
  iVar6 = FUN_0800a874(DAT_0800752c,&local_70);
  uVar4 = DAT_0800752c;
  if (iVar6 != 0) {
    software_bkpt(0xff);
    do {
                    /* WARNING: Do nothing block with infinite loop */
    } while( true );
  }
  local_58 = 0x7ff;
  uStack_54 = 4;
  if (*(char *)(iVar3 + 0x550) == '\0') {
    return;
  }
  local_74 = 6;
  uVar9 = 0;
  local_50 = 0;
  do {
    switch(*(undefined *)(uVar9 * 0x54 + iVar3 + 0x51)) {
    case 0:
      local_5c = 0;
      break;
    case 1:
      local_5c = 1;
      break;
    case 2:
      local_5c = 2;
      break;
    case 3:
      local_5c = 3;
      break;
    case 4:
      local_5c = 4;
      break;
    case 5:
      local_5c = 5;
      break;
    case 6:
      local_5c = 6;
      break;
    case 7:
      local_5c = 7;
    }
    iVar6 = (int)(short)uVar9;
    FUN_0800776c(iVar3 + iVar6 * 0x54);
    bVar1 = *(byte *)(uVar9 * 0x54 + iVar3 + 0x50);
    if (bVar1 < 5) {
      if (2 < bVar1) {
        uVar5 = 2;
        goto LAB_08007566;
      }
      if (bVar1 == 2) {
        uVar5 = 1;
        goto LAB_08007566;
      }
      *(undefined *)(iVar3 + uVar9 + 0x540) = 0;
    }
    else {
      uVar5 = 3;
LAB_08007566:
      iVar12 = 0;
      iVar7 = iVar3 + iVar6 * 0x54 + 0x14;
      *(undefined *)(iVar3 + uVar9 + 0x540) = uVar5;
      do {
        iVar12 = iVar12 + 1;
        FUN_0800776c(iVar7);
        iVar7 = iVar7 + 0x14;
      } while (iVar12 < (int)(uint)*(byte *)(iVar3 + uVar9 + 0x540));
    }
    puVar8 = &local_48;
    iVar7 = 0;
    local_48 = *DAT_08007530;
    uStack_44 = DAT_08007530[1];
    uStack_40 = DAT_08007530[2];
    uStack_3c = DAT_08007530[3];
    local_38 = DAT_08007530[4];
    uStack_34 = DAT_08007530[5];
    uStack_30 = DAT_08007530[6];
    uStack_2c = DAT_08007530[7];
    do {
      if ((*(char *)puVar8 == *(char *)(iVar3 + iVar6 * 0x54)) &&
         (*(char *)((int)puVar8 + 1) == *(char *)(uVar9 * 0x54 + iVar3 + 1))) {
        local_64 = *(undefined4 *)(DAT_08007534 + iVar7 * 4);
        goto LAB_0800745e;
      }
      iVar7 = iVar7 + 1;
      puVar8 = (undefined4 *)((int)puVar8 + 2);
    } while (iVar7 != 0x10);
    local_64 = 0;
LAB_0800745e:
    local_60 = local_74;
    iVar6 = FUN_08009dbc(uVar4,&local_64);
    if (iVar6 != 0) {
      software_bkpt(0xff);
      do {
                    /* WARNING: Do nothing block with infinite loop */
      } while( true );
    }
    uVar9 = uVar9 + 1 & 0xff;
    if (*(byte *)(iVar3 + 0x550) <= uVar9) {
      return;
    }
    local_74 = *(undefined4 *)(DAT_08007538 + uVar9 * 4);
  } while( true );
}


