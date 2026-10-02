/* 08011a68 HAL_SAI_Init; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

undefined4 HAL_SAI_Init(uint **param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  char cVar1;
  uint uVar2;
  uint *puVar3;
  uint *puVar4;
  uint uVar5;
  uint uVar6;
  uint *puVar7;
  int iVar8;
  uint *puVar9;
  uint *puVar10;
  uint uVar11;
  bool bVar12;
  
  if (param_1 == (uint **)0x0) {
    return 1;
  }
  FUN_08009d18();
  puVar4 = *param_1;
  if ((*(char *)(param_1 + 0xe) == '\x01') &&
     ((((puVar4 != DAT_08011d40 && (puVar4 != DAT_08011d40 + 0x5ffbf00)) ||
       (param_1[1] != (uint *)0x1)) || (param_1[0x11] != (uint *)0x0)))) {
    return 1;
  }
  puVar10 = DAT_08011d4c;
  if ((puVar4 != DAT_08011d40) && (puVar4 != DAT_08011d40 + 8)) {
    if ((puVar4 == DAT_08011d40 + 0x100) || (puVar4 == DAT_08011d40 + 0x108)) {
      cVar1 = *(char *)((int)param_1 + 0x91);
      puVar10 = DAT_08011d78;
      goto joined_r0x08011aee;
    }
    puVar10 = DAT_08011e18;
    if (((puVar4 != DAT_08011d40 + 0x200) && (puVar4 != DAT_08011d40 + 0x208)) &&
       ((puVar10 = DAT_08011d48, puVar4 != DAT_08011d44 && (puVar4 != DAT_08011d44 + 8)))) {
      return 1;
    }
  }
  cVar1 = *(char *)((int)param_1 + 0x91);
joined_r0x08011aee:
  if (cVar1 == '\0') {
    *(undefined *)(param_1 + 0x24) = 0;
    FUN_080090c0(param_1);
    puVar4 = *param_1;
  }
  iVar8 = (uint)((ulonglong)DAT_08011d54 * (ulonglong)*DAT_08011d50 >> 0x2c) << 2;
  *puVar4 = *puVar4 & 0xfffeffff;
  do {
    if (iVar8 == 0) {
      param_1[0x25] = (uint *)((uint)param_1[0x25] | 0x40);
      return 1;
    }
    iVar8 = iVar8 + -1;
  } while ((*puVar4 & 0x10000) != 0);
  *(undefined *)((int)param_1 + 0x91) = 2;
  if (param_1[3] == (uint *)0x1) {
    uVar5 = 0x10;
  }
  else if (param_1[3] == (uint *)0x2) {
    uVar5 = 0x20;
  }
  else {
    uVar5 = 0;
  }
  switch(param_1[2]) {
  case (uint *)0x1:
    uVar11 = 0x400;
    break;
  case (uint *)0x3:
    uVar5 = uVar5 | 1;
    uVar11 = 0x800;
    break;
  case (uint *)0x4:
    uVar5 = uVar5 | 2;
  case (uint *)0x2:
    uVar11 = 0x800;
    break;
  case (uint *)0x5:
    uVar5 = uVar5 | 3;
    uVar11 = 0x800;
    break;
  default:
    uVar11 = 0;
  }
  puVar9 = param_1[8];
  *puVar10 = uVar5;
  if (puVar9 != (uint *)0x0) {
    if ((puVar4 == DAT_08011d40) || (uVar2 = (uint)(puVar4 == DAT_08011d58), uVar2 != 0)) {
      uVar2 = FUN_080116d8(0x100,0,uVar5,DAT_08011d40,param_4);
      puVar4 = *param_1;
    }
    if ((puVar4 == DAT_08011d5c) || (puVar4 == DAT_08011d5c + 8)) {
      uVar2 = FUN_080116d8(0x200,0);
      puVar4 = *param_1;
    }
    if ((puVar4 == DAT_08011d60) || (puVar4 == DAT_08011d60 + 8)) {
      uVar2 = FUN_080116d8(0x200,0);
      puVar4 = *param_1;
    }
    if (puVar4 == DAT_08011d44) {
      uVar2 = FUN_080116d8(0x400,0);
      puVar4 = *param_1;
    }
    if (puVar4 == DAT_08011d64) {
      uVar2 = FUN_080116d8(0x800,0);
    }
    if (param_1[6] == (uint *)0x80000) {
      puVar4 = param_1[0x11];
      if (puVar4 == (uint *)&UndefinedInstruction) {
        puVar9 = (uint *)0x40;
      }
      else if (puVar4 == (uint *)&SupervisorCall) {
        puVar9 = (uint *)0x100;
      }
      else {
        puVar9 = param_1[0x15];
      }
      uVar5 = (uVar2 * 10) / (uint)((int)param_1[8] * (int)puVar9);
    }
    else {
      puVar4 = param_1[0x11];
      if (param_1[10] == (uint *)0x4000000) {
        iVar8 = 2;
      }
      else {
        iVar8 = 1;
      }
      uVar5 = (uVar2 * 10) / (uint)((int)param_1[8] * iVar8 * 0x100);
    }
    puVar9 = (uint *)((ulonglong)DAT_08011d68 * (ulonglong)uVar5 >> 0x23);
    if (uVar5 + (int)puVar9 * -10 == 9) {
      puVar9 = (uint *)((int)puVar9 + 1);
    }
    param_1[9] = puVar9;
    if (puVar4 == (uint *)&UndefinedInstruction) {
      param_1[9] = (uint *)((uint)param_1[9] >> 1);
    }
  }
  if (((uint)param_1[1] & 0xfffffffd) == 0) {
    if (param_1[0x14] == (uint *)0x1) {
      uVar5 = 0;
    }
    else {
      uVar5 = 0x200;
    }
  }
  else {
    uVar5 = 0;
    if (param_1[0x14] == (uint *)0x1) {
      uVar5 = 0x200;
    }
  }
  uVar2 = FUN_08009d18();
  puVar4 = *param_1;
  uVar6 = (uint)param_1[1] | (uint)param_1[0x11] | (uint)param_1[0x12] | (uint)param_1[0x13] |
          (uint)param_1[0xb] | (uint)param_1[5];
  if (uVar2 < 0x2000) {
    uVar6 = uVar6 | (uint)param_1[6];
    *puVar4 = DAT_08011e14 & *puVar4;
    puVar9 = param_1[10];
  }
  else {
    uVar6 = uVar6 | (uint)param_1[6] | (uint)param_1[10];
    *puVar4 = DAT_08011d6c & *puVar4;
    puVar9 = param_1[4];
  }
  uVar2 = DAT_08011d70;
  *puVar4 = uVar6 | (uint)puVar9 | *puVar4 | (int)param_1[9] << 0x14 | uVar11 | uVar5;
  puVar9 = param_1[0xc];
  puVar7 = param_1[7];
  puVar3 = param_1[0xd];
  puVar4[1] = uVar2 & puVar4[1];
  uVar5 = DAT_08011d74;
  puVar4[1] = (uint)puVar7 | (uint)puVar9 | (uint)puVar3 | puVar4[1];
  puVar9 = param_1[0x19];
  puVar4[2] = uVar5 & puVar4[2];
  puVar4[2] = (uint)puVar9 | (uint)param_1[0x17] | (uint)param_1[0x18] | (int)param_1[0x15] - 1U |
              ((int)param_1[0x16] + -1) * 0x100 | puVar4[2];
  puVar9 = param_1[0x1a];
  puVar4[3] = puVar4[3] & 0xf020;
  bVar12 = puVar4 == DAT_08011d40;
  puVar4[3] = (uint)puVar9 | (uint)param_1[0x1b] | (int)param_1[0x1d] << 0x10 |
              ((int)param_1[0x1c] + -1) * 0x100 | puVar4[3];
  if (((bVar12) || (puVar4 == DAT_08011d44)) &&
     (puVar10[0x11] = puVar10[0x11] & 0xfffffffe, *(char *)(param_1 + 0xe) == '\x01')) {
    puVar10[0x11] = (uint)param_1[0x10] | ((int)param_1[0xf] + -1) * 0x10;
    puVar10[0x11] = puVar10[0x11] | 1;
  }
  param_1[0x25] = (uint *)0x0;
  *(undefined *)(param_1 + 0x24) = 0;
  *(undefined *)((int)param_1 + 0x91) = 1;
  return 0;
}


