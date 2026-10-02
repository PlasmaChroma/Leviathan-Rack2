/* 0800c080 FUN_0800c080; analyst naming is provisional. */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_0800c080(uint *param_1,uint *param_2)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  uint uVar7;
  uint uVar8;
  int iVar9;
  uint uVar10;
  uint uVar11;
  
  iVar1 = DAT_0800c2cc;
  uVar4 = *param_2;
  if (uVar4 != 0) {
    uVar10 = 0;
    uVar3 = 0;
    do {
      uVar2 = 1 << (uVar3 & 0xff);
      uVar11 = uVar2 & uVar4;
      if (uVar11 != 0) {
        uVar5 = param_2[1];
        uVar8 = uVar5 & 3;
        if (uVar8 - 1 < 2) {
          uVar7 = 3 << (uVar10 & 0xff);
          param_1[2] = param_2[3] << (uVar10 & 0xff) | param_1[2] & ~uVar7;
          param_1[1] = param_1[1] & ~uVar2 | ((uVar5 << 0x1b) >> 0x1f) << (uVar3 & 0xff);
LAB_0800c210:
          uVar7 = ~uVar7;
          uVar2 = uVar8 << (uVar10 & 0xff);
          param_1[3] = param_2[2] << (uVar10 & 0xff) | param_1[3] & uVar7;
          if (uVar8 == 2) {
            iVar9 = (uVar3 & 7) << 2;
            param_1[(uVar3 >> 3) + 8] =
                 param_2[4] << iVar9 | param_1[(uVar3 >> 3) + 8] & ~(0xf << iVar9);
          }
        }
        else {
          if (uVar8 != 3) {
            uVar7 = 3 << (uVar10 & 0xff);
            goto LAB_0800c210;
          }
          uVar2 = 3 << (uVar10 & 0xff);
          uVar7 = ~uVar2;
        }
        *param_1 = uVar2 | *param_1 & uVar7;
        if ((uVar5 & 0x30000) != 0) {
          iVar9 = (uVar3 & 3) << 2;
          *(uint *)(iVar1 + 0xf4) = *(uint *)(iVar1 + 0xf4) | 2;
          uVar2 = *(uint *)((uVar3 & 0xfffffffc) + 0x58000408) & ~(0xf << iVar9);
          if (param_1 != DAT_0800c2d0) {
            if (param_1 == DAT_0800c2d0 + 0x100) {
              uVar2 = uVar2 | 1 << iVar9;
            }
            else if (param_1 == DAT_0800c2d4) {
              uVar2 = uVar2 | 2 << iVar9;
            }
            else if (param_1 == DAT_0800c2d8) {
              uVar2 = uVar2 | 3 << iVar9;
            }
            else if (param_1 == DAT_0800c2dc) {
              uVar2 = uVar2 | 4 << iVar9;
            }
            else if (param_1 == DAT_0800c2e0) {
              uVar2 = uVar2 | 5 << iVar9;
            }
            else if (param_1 == DAT_0800c2e4) {
              uVar2 = uVar2 | 6 << iVar9;
            }
            else if (param_1 == DAT_0800c2e8) {
              uVar2 = uVar2 | 7 << iVar9;
            }
            else if (param_1 == DAT_0800c2ec) {
              uVar2 = uVar2 | 8 << iVar9;
            }
            else {
              if (param_1 == DAT_0800c2f0) {
                iVar6 = 9;
              }
              else {
                iVar6 = 10;
              }
              uVar2 = uVar2 | iVar6 << iVar9;
            }
          }
          *(uint *)((uVar3 & 0xfffffffc) + 0x58000408) = uVar2;
          uVar2 = ~uVar11;
          if ((int)(uVar5 << 0xb) < 0) {
            _DAT_58000000 = uVar11 | _DAT_58000000;
          }
          else {
            _DAT_58000000 = uVar2 & _DAT_58000000;
          }
          if ((int)(uVar5 << 10) < 0) {
            _DAT_58000004 = uVar11 | _DAT_58000004;
          }
          else {
            _DAT_58000004 = uVar2 & _DAT_58000004;
          }
          if ((int)(uVar5 << 0xe) < 0) {
            _DAT_58000084 = uVar11 | _DAT_58000084;
          }
          else {
            _DAT_58000084 = uVar2 & _DAT_58000084;
          }
          if ((int)(uVar5 << 0xf) < 0) {
            _DAT_58000080 = uVar11 | _DAT_58000080;
          }
          else {
            _DAT_58000080 = uVar2 & _DAT_58000080;
          }
        }
      }
      uVar3 = uVar3 + 1;
      uVar10 = uVar10 + 2;
    } while (uVar4 >> (uVar3 & 0xff) != 0);
  }
  return;
}


