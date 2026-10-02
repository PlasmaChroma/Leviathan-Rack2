/* 0800c2f4 FUN_0800c2f4; analyst naming is provisional. */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_0800c2f4(uint *param_1,uint param_2)

{
  int iVar1;
  uint *puVar2;
  uint *puVar3;
  uint *puVar4;
  uint uVar5;
  uint uVar6;
  int iVar7;
  uint uVar8;
  int iVar9;
  uint uVar10;
  int iVar11;
  
  puVar4 = DAT_0800c478;
  puVar3 = DAT_0800c474;
  puVar2 = DAT_0800c470;
  iVar1 = DAT_0800c46c;
  if (param_2 == 0) {
    return;
  }
  uVar6 = 0;
  do {
    iVar11 = 1;
    uVar5 = 1 << (uVar6 & 0xff);
    uVar10 = uVar5 & param_2;
    if (uVar10 != 0) {
      iVar9 = (uVar6 & 0xfffffffc) + iVar1;
      iVar7 = (uVar6 & 3) << 2;
      if (param_1 == puVar2) {
        uVar8 = 0;
      }
      else if (param_1 == puVar3) {
LAB_0800c412:
        uVar8 = iVar11 << iVar7;
      }
      else if (param_1 == puVar4) {
        uVar8 = 2 << iVar7;
      }
      else if (param_1 == DAT_0800c458) {
        uVar8 = 3 << iVar7;
      }
      else if (param_1 == DAT_0800c45c) {
        uVar8 = 4 << iVar7;
      }
      else if (param_1 == DAT_0800c460) {
        uVar8 = 5 << iVar7;
      }
      else {
        if (param_1 == DAT_0800c464) {
          iVar11 = 6;
          goto LAB_0800c412;
        }
        if (param_1 == DAT_0800c468) {
          uVar8 = 7 << iVar7;
        }
        else if (param_1 == DAT_0800c47c) {
          uVar8 = 8 << iVar7;
        }
        else {
          if (param_1 == DAT_0800c480) {
            iVar11 = 9;
          }
          else {
            iVar11 = 10;
          }
          uVar8 = iVar11 << iVar7;
        }
      }
      if ((*(uint *)(iVar9 + 8) & 0xf << iVar7) == uVar8) {
        _DAT_58000080 = _DAT_58000080 & ~uVar10;
        _DAT_58000084 = _DAT_58000084 & ~uVar10;
        _DAT_58000004 = _DAT_58000004 & ~uVar10;
        _DAT_58000000 = _DAT_58000000 & ~uVar10;
        *(uint *)(iVar9 + 8) = *(uint *)(iVar9 + 8) & ~(0xf << iVar7);
      }
      uVar10 = 3 << ((uVar6 & 0x7f) << 1);
      *param_1 = *param_1 | uVar10;
      param_1[(uVar6 >> 3) + 8] = param_1[(uVar6 >> 3) + 8] & ~(0xf << ((uVar6 & 7) << 2));
      param_1[3] = param_1[3] & ~uVar10;
      param_1[1] = param_1[1] & ~uVar5;
      param_1[2] = param_1[2] & ~uVar10;
    }
    uVar6 = uVar6 + 1;
    if (param_2 >> (uVar6 & 0xff) == 0) {
      return;
    }
  } while( true );
}


