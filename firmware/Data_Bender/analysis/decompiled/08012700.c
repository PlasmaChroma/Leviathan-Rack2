/* 08012700 FUN_08012700; analyst naming is provisional. */

undefined4 FUN_08012700(int param_1,byte *param_2,int param_3)

{
  ushort uVar1;
  int iVar2;
  int iVar3;
  uint *puVar4;
  undefined4 *puVar5;
  undefined4 *puVar6;
  uint uVar8;
  uint uVar9;
  uint uVar10;
  uint uVar11;
  byte bVar12;
  undefined4 *puVar7;
  
  uVar11 = DAT_08012978;
  uVar9 = DAT_08012974;
  uVar8 = (uint)*param_2;
  if (param_2[1] != 1) {
    iVar3 = param_1 + uVar8 * 0x20;
    *(uint *)(iVar3 + 0xb10) = DAT_08012974 & *(uint *)(iVar3 + 0xb10);
    *(uint *)(iVar3 + 0xb10) = uVar11 & *(uint *)(iVar3 + 0xb10);
    if (uVar8 == 0) {
      if (*(int *)(param_2 + 0x10) == 0) {
        uVar9 = *(uint *)(param_2 + 8);
      }
      else {
        uVar9 = *(uint *)(param_2 + 8);
        *(uint *)(param_2 + 0x10) = uVar9;
      }
      *(uint *)(param_2 + 0x20) = uVar9;
    }
    else {
      if (*(int *)(param_2 + 0x10) != 0) {
        uVar11 = *(uint *)(param_2 + 8);
        uVar9 = ((*(int *)(param_2 + 0x10) + uVar11) - 1) / uVar11 & 0xffff;
        uVar11 = uVar9 * uVar11;
        uVar9 = DAT_0801297c & uVar9 << 0x13;
        uVar8 = *(uint *)(iVar3 + 0xb10);
        *(uint *)(param_2 + 0x20) = uVar11;
        *(uint *)(iVar3 + 0xb10) = uVar9 | uVar8;
        *(uint *)(iVar3 + 0xb10) = uVar11 & 0x7ffff | *(uint *)(iVar3 + 0xb10);
        goto joined_r0x0801289c;
      }
      uVar9 = *(uint *)(param_2 + 8);
    }
    *(uint *)(iVar3 + 0xb10) = uVar9 & 0x7ffff | *(uint *)(iVar3 + 0xb10);
    *(uint *)(iVar3 + 0xb10) = *(uint *)(iVar3 + 0xb10) | 0x80000;
joined_r0x0801289c:
    if ((param_3 == 1) && (*(int *)(param_2 + 0xc) != 0)) {
      *(int *)(iVar3 + 0xb14) = *(int *)(param_2 + 0xc);
    }
    if (param_2[4] == 1) {
      if ((*(uint *)(param_1 + 0x808) & 0x100) == 0) {
        uVar9 = *(uint *)(iVar3 + 0xb00) | 0x20000000;
      }
      else {
        uVar9 = *(uint *)(iVar3 + 0xb00) | 0x10000000;
      }
      *(uint *)(iVar3 + 0xb00) = uVar9;
    }
    *(uint *)(iVar3 + 0xb00) = *(uint *)(iVar3 + 0xb00) | 0x84000000;
    return 0;
  }
  uVar10 = *(uint *)(param_2 + 0x10);
  if (uVar10 == 0) {
    iVar3 = param_1 + uVar8 * 0x20;
    *(uint *)(iVar3 + 0x910) = DAT_08012978 & *(uint *)(iVar3 + 0x910);
    *(uint *)(iVar3 + 0x910) = *(uint *)(iVar3 + 0x910) | 0x80000;
    *(uint *)(iVar3 + 0x910) = uVar9 & *(uint *)(iVar3 + 0x910);
    puVar4 = (uint *)(iVar3 + 0x900);
    bVar12 = param_2[4];
    if (param_3 != 1) {
      *(uint *)(iVar3 + 0x900) = *(uint *)(iVar3 + 0x900) | 0x84000000;
      if (bVar12 != 1) {
        return 0;
      }
LAB_080128f0:
      if ((*(uint *)(param_1 + 0x808) & 0x100) == 0) {
        uVar9 = *puVar4 | 0x20000000;
      }
      else {
        uVar9 = *puVar4 | 0x10000000;
      }
      *puVar4 = uVar9;
      if ((param_3 == 0) && (uVar1 = *(ushort *)(param_2 + 0x10), uVar1 + 3 >> 2 != 0)) {
        puVar5 = *(undefined4 **)(param_2 + 0xc);
        puVar6 = puVar5;
        do {
          puVar7 = puVar6 + 1;
          *(undefined4 *)(param_1 + uVar8 * 0x1000 + 0x1000) = *puVar6;
          puVar6 = puVar7;
        } while (puVar7 != (undefined4 *)((int)puVar5 + (uVar1 + 3 & 0xfffffffc)));
      }
      return 0;
    }
    uVar9 = *(uint *)(param_2 + 0x1c);
    if (uVar9 != 0) goto LAB_0801294c;
LAB_08012934:
    if (bVar12 != 1) goto LAB_08012938;
  }
  else {
    iVar3 = param_1 + uVar8 * 0x20;
    *(uint *)(iVar3 + 0x910) = *(uint *)(iVar3 + 0x910) & DAT_08012974;
    *(uint *)(iVar3 + 0x910) = uVar11 & *(uint *)(iVar3 + 0x910);
    puVar4 = (uint *)(iVar3 + 0x900);
    if (uVar8 == 0) {
      uVar9 = *(uint *)(param_2 + 8);
      if (uVar9 < uVar10) {
        *(uint *)(param_2 + 0x10) = uVar9;
        uVar10 = uVar9;
      }
      *(uint *)(iVar3 + 0x910) = *(uint *)(iVar3 + 0x910) | 0x80000;
    }
    else {
      *(uint *)(iVar3 + 0x910) =
           DAT_0801297c & ((uVar10 + *(uint *)(param_2 + 8)) - 1) / *(uint *)(param_2 + 8) << 0x13 |
           *(uint *)(iVar3 + 0x910);
    }
    *(uint *)(iVar3 + 0x910) = uVar10 & 0x7ffff | *(uint *)(iVar3 + 0x910);
    bVar12 = param_2[4];
    if (bVar12 != 1) {
      if (param_3 != 1) {
        iVar2 = *(int *)(param_2 + 0x10);
        *(uint *)(iVar3 + 0x900) = *(uint *)(iVar3 + 0x900) | 0x84000000;
        if (iVar2 == 0) {
          return 0;
        }
        *(uint *)(param_1 + 0x834) = 1 << (uVar8 & 0xf) | *(uint *)(param_1 + 0x834);
        return 0;
      }
      uVar9 = *(uint *)(param_2 + 0x1c);
      if (uVar9 == 0) goto LAB_08012938;
LAB_0801294c:
      puVar4[5] = uVar9;
      goto LAB_08012934;
    }
    *(uint *)(iVar3 + 0x910) = *(uint *)(iVar3 + 0x910) & 0x9fffffff;
    *(uint *)(iVar3 + 0x910) = *(uint *)(iVar3 + 0x910) | 0x20000000;
    if (param_3 != 1) {
      *(uint *)(iVar3 + 0x900) = *(uint *)(iVar3 + 0x900) | 0x84000000;
      goto LAB_080128f0;
    }
    uVar9 = *(uint *)(param_2 + 0x1c);
    if (uVar9 != 0) goto LAB_0801294c;
  }
  if ((*(uint *)(param_1 + 0x808) & 0x100) == 0) {
    uVar9 = *puVar4 | 0x20000000;
  }
  else {
    uVar9 = *puVar4 | 0x10000000;
  }
  *puVar4 = uVar9;
LAB_08012938:
  *puVar4 = *puVar4 | 0x84000000;
  return 0;
}


