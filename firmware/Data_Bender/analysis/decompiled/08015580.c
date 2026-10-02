/* 08015580 FUN_08015580; analyst naming is provisional. */

undefined4 FUN_08015580(uint **param_1)

{
  uint *puVar1;
  uint *puVar2;
  uint *puVar3;
  uint *puVar4;
  uint *puVar5;
  uint *puVar6;
  uint *puVar7;
  uint *puVar8;
  uint *puVar9;
  uint *puVar10;
  uint *puVar11;
  uint uVar12;
  uint *puVar13;
  uint *puVar14;
  uint *puVar15;
  uint *puVar16;
  uint *puVar17;
  
  puVar13 = DAT_0801572c;
  if (param_1 == (uint **)0x0) {
    return 1;
  }
  puVar3 = *param_1;
  param_1[10] = (uint *)0x0;
  if ((puVar3 == puVar13) || (puVar3 == puVar13 + -0x3e00)) {
    puVar2 = param_1[3];
    puVar13 = param_1[0xf];
    uVar12 = ((uint)puVar13 >> 5) * ((uint)(puVar2 + 2) >> 3) + ((uint)(puVar2 + 2) >> 3);
    if ((puVar3 == DAT_08015730 || puVar3 == DAT_0801572c) || (puVar3 == DAT_08015730 + 0x100))
    goto LAB_080155ec;
  }
  else {
    puVar2 = param_1[3];
    if (puVar3 == puVar13 + -0x3d00) {
      puVar13 = param_1[0xf];
      uVar12 = ((uint)puVar13 >> 5) * ((uint)(puVar2 + 2) >> 3) + ((uint)(puVar2 + 2) >> 3);
LAB_080155ec:
      if (0x10 < uVar12) {
        return 1;
      }
    }
    else {
      if ((uint *)0xf < puVar2) {
        return 1;
      }
      puVar13 = param_1[0xf];
      if (8 < ((uint)puVar13 >> 5) * ((uint)(puVar2 + 2) >> 3) + ((uint)(puVar2 + 2) >> 3)) {
        return 1;
      }
    }
  }
  if (*(char *)((int)param_1 + 0x81) == '\0') {
    *(undefined *)(param_1 + 0x20) = 0;
    FUN_08013f2c(param_1);
    puVar3 = *param_1;
    puVar16 = param_1[10];
    puVar2 = param_1[3];
    puVar13 = param_1[0xf];
  }
  else {
    puVar16 = (uint *)0x0;
  }
  puVar15 = param_1[6];
  puVar1 = param_1[1];
  *(undefined *)((int)param_1 + 0x81) = 2;
  puVar14 = param_1[0xe];
  *puVar3 = *puVar3 & 0xfffffffe;
  if (puVar15 == (uint *)0x4000000) {
    puVar17 = puVar1;
    if (puVar1 == (uint *)0x400000) {
      if (puVar14 == (uint *)0x0) {
LAB_080156f2:
        *puVar3 = *puVar3 | 0x1000;
        goto LAB_08015622;
      }
      goto LAB_08015628;
    }
    if (puVar1 != (uint *)0x0) goto LAB_08015622;
    if (puVar14 == (uint *)0x10000000) goto LAB_080156f2;
  }
  else {
LAB_08015622:
    puVar17 = (uint *)((uint)puVar1 & 0x400000);
    if (puVar17 != (uint *)0x0) {
LAB_08015628:
      if ((uint *)0x6 < puVar2) {
        *puVar3 = *puVar3 & 0xfffffeff | (uint)param_1[0x14];
        goto LAB_08015634;
      }
    }
  }
  *puVar3 = *puVar3 & 0xfffffeff;
LAB_08015634:
  puVar4 = param_1[0xd];
  puVar5 = param_1[9];
  puVar6 = param_1[4];
  puVar7 = param_1[5];
  puVar8 = param_1[8];
  puVar9 = param_1[0x13];
  puVar10 = param_1[2];
  puVar11 = param_1[0x12];
  puVar3[2] = (uint)puVar16 | (uint)param_1[7] | (uint)puVar13 | puVar3[2] & 0x1f0000 | (uint)puVar2
  ;
  puVar3[3] = (uint)puVar14 |
              (uint)puVar15 | (uint)puVar4 | (uint)puVar5 | (uint)puVar6 | (uint)puVar7 |
              (uint)puVar8 | (uint)puVar9 | (uint)puVar10 | (uint)puVar11 | (uint)param_1[0x16] |
              (uint)puVar1;
  if (puVar1 == (uint *)0x0) {
    puVar3[2] = puVar3[2] & 0xffffe7ff | 0x800;
    puVar3[2] = puVar3[2] & 0xfffff9ff | 0x400;
    puVar3[0x14] = puVar3[0x14] & 0xfffffffe;
  }
  else {
    puVar3[0x14] = puVar3[0x14] & 0xfffffffe;
    if (puVar17 != (uint *)0x0) {
      puVar3[3] = puVar3[3] & 0x7fffffff | (uint)param_1[0x15];
    }
  }
  param_1[0x21] = (uint *)0x0;
  *(undefined *)((int)param_1 + 0x81) = 1;
  return 0;
}


