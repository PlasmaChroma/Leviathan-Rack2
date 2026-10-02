/* 08011e1c HAL_SAI_InitProtocol; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

undefined4 HAL_SAI_InitProtocol(uint **param_1,uint param_2,undefined4 param_3,uint *param_4)

{
  char cVar1;
  uint uVar2;
  uint *puVar3;
  uint uVar4;
  uint uVar5;
  uint *puVar6;
  int iVar7;
  uint *puVar8;
  undefined4 unaff_r4;
  uint *puVar9;
  undefined4 unaff_r5;
  uint *puVar10;
  uint uVar11;
  bool bVar12;
  
  if (param_2 < 3) {
    param_1[0x1c] = param_4;
    param_1[0x11] = (uint *)0x0;
    param_1[0x13] = (uint *)0x0;
    param_1[0x1a] = (uint *)0x0;
    param_1[0x14] = (uint *)(uint)(((uint)param_1[1] & 0xfffffffd) != 0);
    param_1[0x17] = (uint *)0x10000;
    param_1[0x1d] = (uint *)0xffff;
    if (((uint)param_4 & 1) != 0) {
      return 1;
    }
    if (param_2 != 0) {
      param_1[0x19] = (uint *)0x0;
      param_1[0x18] = (uint *)0x20000;
      switch(param_3) {
      case 0:
        goto switchD_08011eac_caseD_0;
      case 1:
        goto switchD_08011eac_caseD_1;
      case 2:
        goto switchD_08011eac_caseD_2;
      case 3:
        goto switchD_08011eac_caseD_3;
      default:
        goto switchD_08011e66_caseD_4;
      }
    }
    param_1[0x18] = (uint *)0x0;
    param_1[0x19] = (uint *)0x40000;
    switch(param_3) {
    case 0:
switchD_08011eac_caseD_0:
      param_1[0x12] = (uint *)0x80;
      puVar9 = (uint *)(((uint)param_4 >> 1) << 4);
      param_1[0x1b] = (uint *)0x40;
      param_1[0x15] = (uint *)(((uint)param_4 >> 1) << 5);
      param_1[0x16] = puVar9;
      break;
    case 1:
switchD_08011eac_caseD_1:
      param_1[0x12] = (uint *)0x80;
      puVar9 = (uint *)(((uint)param_4 >> 1) << 5);
      param_1[0x1b] = (uint *)0x80;
      param_1[0x15] = (uint *)(((uint)param_4 >> 1) << 6);
      param_1[0x16] = puVar9;
      if (param_2 == 2) {
        puVar9 = (uint *)&DataAbort;
        param_1[0x1a] = (uint *)&DataAbort;
      }
      break;
    case 2:
switchD_08011eac_caseD_2:
      param_1[0x12] = (uint *)0xc0;
      puVar9 = (uint *)(((uint)param_4 >> 1) << 5);
      param_1[0x1b] = (uint *)0x80;
      param_1[0x15] = (uint *)(((uint)param_4 >> 1) << 6);
      param_1[0x16] = puVar9;
      if (param_2 == 2) {
        puVar9 = (uint *)&SupervisorCall;
        param_1[0x1a] = (uint *)&SupervisorCall;
      }
      break;
    case 3:
switchD_08011eac_caseD_3:
      param_1[0x12] = (uint *)0xe0;
      puVar9 = (uint *)(((uint)param_4 >> 1) << 5);
      param_1[0x1b] = (uint *)0x80;
      param_1[0x15] = (uint *)(((uint)param_4 >> 1) << 6);
      param_1[0x16] = puVar9;
      break;
    default:
      goto switchD_08011e66_caseD_4;
    }
    goto HAL_SAI_Init;
  }
  if (1 < param_2 - 3) {
switchD_08011e66_caseD_4:
    return 1;
  }
  param_1[0x1c] = param_4;
  param_1[0x11] = (uint *)0x0;
  param_1[0x13] = (uint *)0x0;
  param_1[0x17] = (uint *)0x0;
  param_1[0x1a] = (uint *)0x0;
  param_1[0x1d] = (uint *)0xffff;
  param_1[0x14] = (uint *)(uint)(((uint)param_1[1] & 0xfffffffd) == 0);
  if (param_2 == 4) {
    puVar9 = (uint *)0x1;
  }
  else {
    puVar9 = (uint *)0xd;
  }
  param_1[0x18] = (uint *)0x20000;
  param_1[0x16] = puVar9;
  param_1[0x19] = (uint *)0x40000;
  switch(param_3) {
  case 0:
    puVar9 = (uint *)((int)param_4 << 4);
    param_1[0x12] = (uint *)0x80;
    param_1[0x15] = puVar9;
    param_1[0x1b] = (uint *)0x40;
    break;
  case 1:
    puVar9 = (uint *)((int)param_4 << 5);
    param_1[0x12] = (uint *)0x80;
    param_1[0x15] = puVar9;
    param_1[0x1b] = (uint *)0x80;
    break;
  case 2:
    puVar3 = (uint *)0xc0;
    goto LAB_08011ee8;
  case 3:
    puVar3 = (uint *)0xe0;
LAB_08011ee8:
    puVar9 = (uint *)((int)param_4 << 5);
    param_1[0x12] = puVar3;
    param_1[0x15] = puVar9;
    param_1[0x1b] = (uint *)0x80;
    break;
  default:
    goto switchD_08011e66_caseD_4;
  }
HAL_SAI_Init:
  if (param_1 == (uint **)0x0) {
    return 1;
  }
  FUN_08009d18();
  puVar3 = *param_1;
  if ((*(char *)(param_1 + 0xe) == '\x01') &&
     ((((puVar3 != DAT_08011d40 && (puVar3 != DAT_08011d40 + 0x5ffbf00)) ||
       (param_1[1] != (uint *)0x1)) || (param_1[0x11] != (uint *)0x0)))) {
    return 1;
  }
  puVar10 = DAT_08011d4c;
  if ((puVar3 != DAT_08011d40) && (puVar3 != DAT_08011d40 + 8)) {
    if ((puVar3 == DAT_08011d40 + 0x100) || (puVar3 == DAT_08011d40 + 0x108)) {
      cVar1 = *(char *)((int)param_1 + 0x91);
      puVar10 = DAT_08011d78;
      goto joined_r0x08011aee;
    }
    puVar10 = DAT_08011e18;
    if (((puVar3 != DAT_08011d40 + 0x200) && (puVar3 != DAT_08011d40 + 0x208)) &&
       ((puVar10 = DAT_08011d48, puVar3 != DAT_08011d44 && (puVar3 != DAT_08011d44 + 8)))) {
      return 1;
    }
  }
  cVar1 = *(char *)((int)param_1 + 0x91);
joined_r0x08011aee:
  if (cVar1 == '\0') {
    *(undefined *)(param_1 + 0x24) = 0;
    FUN_080090c0(param_1);
    puVar3 = *param_1;
  }
  iVar7 = (uint)((ulonglong)DAT_08011d54 * (ulonglong)*DAT_08011d50 >> 0x2c) << 2;
  *puVar3 = *puVar3 & 0xfffeffff;
  do {
    if (iVar7 == 0) {
      param_1[0x25] = (uint *)((uint)param_1[0x25] | 0x40);
      return 1;
    }
    iVar7 = iVar7 + -1;
  } while ((*puVar3 & 0x10000) != 0);
  *(undefined *)((int)param_1 + 0x91) = 2;
  if (param_1[3] == (uint *)0x1) {
    uVar4 = 0x10;
  }
  else if (param_1[3] == (uint *)0x2) {
    uVar4 = 0x20;
  }
  else {
    uVar4 = 0;
  }
  switch(param_1[2]) {
  case (uint *)0x1:
    uVar11 = 0x400;
    break;
  case (uint *)0x3:
    uVar4 = uVar4 | 1;
    uVar11 = 0x800;
    break;
  case (uint *)0x4:
    uVar4 = uVar4 | 2;
  case (uint *)0x2:
    uVar11 = 0x800;
    break;
  case (uint *)0x5:
    uVar4 = uVar4 | 3;
    uVar11 = 0x800;
    break;
  default:
    uVar11 = 0;
  }
  puVar8 = param_1[8];
  *puVar10 = uVar4;
  if (puVar8 != (uint *)0x0) {
    if ((puVar3 == DAT_08011d40) || (uVar2 = (uint)(puVar3 == DAT_08011d58), uVar2 != 0)) {
      uVar2 = FUN_080116d8(0x100,0,uVar4,DAT_08011d40,puVar9,unaff_r4,unaff_r5);
      puVar3 = *param_1;
    }
    if ((puVar3 == DAT_08011d5c) || (puVar3 == DAT_08011d5c + 8)) {
      uVar2 = FUN_080116d8(0x200,0);
      puVar3 = *param_1;
    }
    if ((puVar3 == DAT_08011d60) || (puVar3 == DAT_08011d60 + 8)) {
      uVar2 = FUN_080116d8(0x200,0);
      puVar3 = *param_1;
    }
    if (puVar3 == DAT_08011d44) {
      uVar2 = FUN_080116d8(0x400,0);
      puVar3 = *param_1;
    }
    if (puVar3 == DAT_08011d64) {
      uVar2 = FUN_080116d8(0x800,0);
    }
    if (param_1[6] == (uint *)0x80000) {
      puVar9 = param_1[0x11];
      if (puVar9 == (uint *)&UndefinedInstruction) {
        puVar3 = (uint *)0x40;
      }
      else if (puVar9 == (uint *)&SupervisorCall) {
        puVar3 = (uint *)0x100;
      }
      else {
        puVar3 = param_1[0x15];
      }
      uVar4 = (uVar2 * 10) / (uint)((int)param_1[8] * (int)puVar3);
    }
    else {
      puVar9 = param_1[0x11];
      if (param_1[10] == (uint *)0x4000000) {
        iVar7 = 2;
      }
      else {
        iVar7 = 1;
      }
      uVar4 = (uVar2 * 10) / (uint)((int)param_1[8] * iVar7 * 0x100);
    }
    puVar3 = (uint *)((ulonglong)DAT_08011d68 * (ulonglong)uVar4 >> 0x23);
    if (uVar4 + (int)puVar3 * -10 == 9) {
      puVar3 = (uint *)((int)puVar3 + 1);
    }
    param_1[9] = puVar3;
    if (puVar9 == (uint *)&UndefinedInstruction) {
      param_1[9] = (uint *)((uint)param_1[9] >> 1);
    }
  }
  if (((uint)param_1[1] & 0xfffffffd) == 0) {
    if (param_1[0x14] == (uint *)0x1) {
      uVar4 = 0;
    }
    else {
      uVar4 = 0x200;
    }
  }
  else {
    uVar4 = 0;
    if (param_1[0x14] == (uint *)0x1) {
      uVar4 = 0x200;
    }
  }
  uVar2 = FUN_08009d18();
  puVar9 = *param_1;
  uVar5 = (uint)param_1[1] | (uint)param_1[0x11] | (uint)param_1[0x12] | (uint)param_1[0x13] |
          (uint)param_1[0xb] | (uint)param_1[5];
  if (uVar2 < 0x2000) {
    uVar5 = uVar5 | (uint)param_1[6];
    *puVar9 = DAT_08011e14 & *puVar9;
    puVar3 = param_1[10];
  }
  else {
    uVar5 = uVar5 | (uint)param_1[6] | (uint)param_1[10];
    *puVar9 = DAT_08011d6c & *puVar9;
    puVar3 = param_1[4];
  }
  uVar2 = DAT_08011d70;
  *puVar9 = uVar5 | (uint)puVar3 | *puVar9 | (int)param_1[9] << 0x14 | uVar11 | uVar4;
  puVar3 = param_1[0xc];
  puVar6 = param_1[7];
  puVar8 = param_1[0xd];
  puVar9[1] = uVar2 & puVar9[1];
  uVar4 = DAT_08011d74;
  puVar9[1] = (uint)puVar6 | (uint)puVar3 | (uint)puVar8 | puVar9[1];
  puVar3 = param_1[0x19];
  puVar9[2] = uVar4 & puVar9[2];
  puVar9[2] = (uint)puVar3 | (uint)param_1[0x17] | (uint)param_1[0x18] | (int)param_1[0x15] - 1U |
              ((int)param_1[0x16] + -1) * 0x100 | puVar9[2];
  puVar3 = param_1[0x1a];
  puVar9[3] = puVar9[3] & 0xf020;
  bVar12 = puVar9 == DAT_08011d40;
  puVar9[3] = (uint)puVar3 | (uint)param_1[0x1b] | (int)param_1[0x1d] << 0x10 |
              ((int)param_1[0x1c] + -1) * 0x100 | puVar9[3];
  if (((bVar12) || (puVar9 == DAT_08011d44)) &&
     (puVar10[0x11] = puVar10[0x11] & 0xfffffffe, *(char *)(param_1 + 0xe) == '\x01')) {
    puVar10[0x11] = (uint)param_1[0x10] | ((int)param_1[0xf] + -1) * 0x10;
    puVar10[0x11] = puVar10[0x11] | 1;
  }
  param_1[0x25] = (uint *)0x0;
  *(undefined *)(param_1 + 0x24) = 0;
  *(undefined *)((int)param_1 + 0x91) = 1;
  return 0;
}


