/* 08019110 FUN_08019110; analyst naming is provisional. */

code * FUN_08019110(undefined4 param_1,uint *param_2,undefined4 param_3,code *param_4,uint **param_5
                   )

{
  bool bVar1;
  byte bVar2;
  int iVar3;
  code *pcVar4;
  int iVar5;
  undefined *puVar6;
  uint uVar7;
  uint *puVar8;
  uint uVar9;
  uint **ppuVar10;
  uint uVar11;
  undefined *puVar12;
  code *local_24;
  
  iVar3 = DAT_08019354;
  bVar2 = *(byte *)(param_2 + 6);
  puVar6 = (undefined *)((int)param_2 + 0x43);
  local_24 = param_4;
  if (0x78 < bVar2) {
switchD_0801914c_caseD_65:
    *(byte *)((int)param_2 + 0x42) = bVar2;
LAB_080191b8:
    puVar6 = (undefined *)((int)param_2 + 0x42);
    uVar7 = 1;
LAB_08019304:
    param_2[4] = uVar7;
    *(undefined *)((int)param_2 + 0x43) = 0;
    goto LAB_08019260;
  }
  if (bVar2 < 99) {
    if (bVar2 == 0) goto LAB_080192e2;
    if (bVar2 == 0x58) {
      *(undefined *)((int)param_2 + 0x45) = 0x58;
      goto LAB_08019284;
    }
    goto switchD_0801914c_caseD_65;
  }
  switch(bVar2) {
  case 99:
    uVar7 = **param_5;
    *param_5 = *param_5 + 1;
    *(char *)((int)param_2 + 0x42) = (char)uVar7;
    goto LAB_080191b8;
  case 100:
  case 0x69:
    uVar9 = *param_2;
    puVar8 = *param_5;
    if ((int)(uVar9 << 0x18) < 0) {
      uVar7 = *puVar8;
      *param_5 = puVar8 + 1;
    }
    else {
      uVar7 = *puVar8;
      *param_5 = puVar8 + 1;
      if ((uVar9 & 0x40) != 0) {
        uVar7 = (uint)(short)uVar7;
      }
    }
    if ((int)uVar7 < 0) {
      uVar7 = -uVar7;
      *(undefined *)((int)param_2 + 0x43) = 0x2d;
    }
    uVar9 = 10;
    iVar3 = DAT_08019354;
    goto LAB_08019212;
  default:
    goto switchD_0801914c_caseD_65;
  case 0x6e:
    ppuVar10 = (uint **)*param_5;
    uVar9 = *param_2;
    uVar7 = param_2[5];
    *param_5 = (uint *)(ppuVar10 + 1);
    puVar8 = *ppuVar10;
    if (((int)(uVar9 << 0x18) < 0) || (-1 < (int)(uVar9 << 0x19))) {
      *puVar8 = uVar7;
    }
    else {
      *(short *)puVar8 = (short)uVar7;
    }
LAB_080192e2:
    param_2[4] = 0;
    goto LAB_08019260;
  case 0x6f:
  case 0x75:
    puVar8 = *param_5;
    uVar7 = *param_2;
    *param_5 = puVar8 + 1;
    if (((int)(uVar7 << 0x18) < 0) || (-1 < (int)(uVar7 << 0x19))) {
      uVar7 = *puVar8;
    }
    else {
      uVar7 = (uint)*(ushort *)puVar8;
    }
    iVar3 = DAT_08019354;
    if (bVar2 == 0x6f) {
      uVar9 = 8;
    }
    else {
      uVar9 = 10;
    }
    break;
  case 0x70:
    *param_2 = *param_2 | 0x20;
  case 0x78:
    iVar3 = DAT_08019358;
    *(undefined *)((int)param_2 + 0x45) = 0x78;
LAB_08019284:
    uVar9 = *param_2;
    uVar7 = **param_5;
    *param_5 = *param_5 + 1;
    if ((-1 < (int)(uVar9 << 0x18)) && ((int)(uVar9 << 0x19) < 0)) {
      uVar7 = uVar7 & 0xffff;
    }
    if ((int)(uVar9 << 0x1f) < 0) {
      *param_2 = uVar9 | 0x20;
    }
    if (uVar7 == 0) {
      *param_2 = *param_2 & 0xffffffdf;
    }
    uVar9 = 0x10;
    break;
  case 0x73:
    puVar8 = *param_5;
    *param_5 = puVar8 + 1;
    puVar6 = (undefined *)*puVar8;
    iVar3 = FUN_080002e0(puVar6,0,param_2[1],puVar8,param_1,param_2,param_3);
    if (iVar3 != 0) {
      param_2[1] = iVar3 - (int)puVar6;
    }
    uVar7 = param_2[1];
    goto LAB_08019304;
  }
  *(undefined *)((int)param_2 + 0x43) = 0;
LAB_08019212:
  uVar11 = param_2[1];
  param_2[2] = uVar11;
  if (-1 < (int)uVar11) {
    *param_2 = *param_2 & 0xfffffffb;
  }
  puVar12 = puVar6;
  if ((uVar7 != 0) || (uVar11 != 0)) {
    do {
      puVar12 = puVar12 + -1;
      *puVar12 = *(undefined *)(iVar3 + (uVar7 - uVar9 * (uVar7 / uVar9)));
      bVar1 = uVar9 <= uVar7;
      uVar7 = uVar7 / uVar9;
    } while (bVar1);
  }
  if (((uVar9 == 8) && ((int)(*param_2 << 0x1f) < 0)) && ((int)param_2[1] <= (int)param_2[4])) {
    puVar12[-1] = 0x30;
    puVar12 = puVar12 + -1;
  }
  param_2[4] = (int)puVar6 - (int)puVar12;
  puVar6 = puVar12;
LAB_08019260:
  iVar3 = FUN_08019034(param_1,param_2,&local_24,param_3,param_4);
  if ((iVar3 == -1) || (iVar3 = (*param_4)(param_1,param_3,puVar6,param_2[4]), iVar3 == -1)) {
LAB_08019274:
    pcVar4 = (code *)0xffffffff;
  }
  else {
    if ((int)(*param_2 << 0x1e) < 0) {
      for (iVar3 = 0; iVar3 < (int)(param_2[3] - (int)local_24); iVar3 = iVar3 + 1) {
        iVar5 = (*param_4)(param_1,param_3,(int)param_2 + 0x19,1);
        if (iVar5 == -1) goto LAB_08019274;
      }
    }
    pcVar4 = (code *)param_2[3];
    if ((int)(code *)param_2[3] < (int)local_24) {
      pcVar4 = local_24;
    }
  }
  return pcVar4;
}


