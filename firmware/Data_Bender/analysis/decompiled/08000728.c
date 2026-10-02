/* 08000728 FUN_08000728; analyst naming is provisional. */

/* WARNING: Removing unreachable block (ram,0x080009c8) */

ulonglong FUN_08000728(uint param_1,uint param_2,uint param_3,uint param_4,uint *param_5)

{
  code *pcVar1;
  ulonglong uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  uint uVar10;
  uint uVar11;
  uint uVar12;
  bool bVar13;
  
  if (param_4 == 0) {
    if (param_2 < param_3) {
      iVar6 = LZCOUNT(param_3);
      uVar11 = param_3;
      if (iVar6 != 0) {
        uVar11 = param_3 << iVar6;
        param_2 = param_1 >> (0x20U - iVar6 & 0xff) | param_2 << iVar6;
        param_1 = param_1 << iVar6;
      }
      uVar10 = uVar11 >> 0x10;
      uVar8 = param_2 / uVar10;
      uVar7 = param_1 >> 0x10 | (param_2 - uVar10 * uVar8) * 0x10000;
      uVar5 = uVar8 * (uVar11 & 0xffff);
      uVar3 = uVar8;
      if (uVar7 <= uVar5 && uVar5 - uVar7 != 0) {
        bVar13 = CARRY4(uVar11,uVar7);
        uVar7 = uVar11 + uVar7;
        uVar3 = uVar8 - 1;
        if ((bVar13 == false) && (uVar7 <= uVar5 && uVar5 - uVar7 != 0)) {
          uVar3 = uVar8 - 2;
          uVar7 = uVar7 + uVar11;
        }
      }
      uVar4 = (uVar7 - uVar5) / uVar10;
      uVar8 = param_1 & 0xffff | ((uVar7 - uVar5) - uVar10 * uVar4) * 0x10000;
      uVar7 = uVar4 * (uVar11 & 0xffff);
      uVar5 = uVar4;
      if (uVar8 <= uVar7 && uVar7 - uVar8 != 0) {
        bVar13 = CARRY4(uVar11,uVar8);
        uVar8 = uVar11 + uVar8;
        uVar5 = uVar4 - 1;
        if ((bVar13 == false) && (uVar8 <= uVar7 && uVar7 - uVar8 != 0)) {
          uVar8 = uVar8 + uVar11;
          uVar5 = uVar4 - 2;
        }
      }
      uVar5 = uVar5 | uVar3 << 0x10;
      uVar8 = uVar8 - uVar7;
      uVar11 = 0;
    }
    else {
      if (param_3 == 0) {
                    /* WARNING: Does not return */
        pcVar1 = (code *)software_udf(0xff,0x8000812);
        (*pcVar1)();
      }
      iVar6 = LZCOUNT(param_3);
      if (iVar6 == 0) {
        param_2 = param_2 - param_3;
        uVar4 = param_3 >> 0x10;
        uVar12 = param_3 & 0xffff;
        uVar11 = 1;
        uVar3 = param_3;
      }
      else {
        uVar3 = param_3 << iVar6;
        uVar5 = param_2 >> (0x20U - iVar6 & 0xff);
        uVar7 = param_2 << iVar6 | param_1 >> (0x20U - iVar6 & 0xff);
        uVar4 = uVar3 >> 0x10;
        uVar12 = uVar3 & 0xffff;
        uVar11 = uVar5 / uVar4;
        uVar8 = uVar7 >> 0x10 | (uVar5 - uVar4 * uVar11) * 0x10000;
        uVar10 = uVar11 * uVar12;
        param_1 = param_1 << iVar6;
        uVar5 = uVar11;
        if (uVar8 <= uVar10 && uVar10 - uVar8 != 0) {
          bVar13 = CARRY4(uVar3,uVar8);
          uVar8 = uVar3 + uVar8;
          uVar5 = uVar11 - 1;
          if ((bVar13 == false) && (uVar8 <= uVar10 && uVar10 - uVar8 != 0)) {
            uVar5 = uVar11 - 2;
            uVar8 = uVar8 + uVar3;
          }
        }
        uVar9 = (uVar8 - uVar10) / uVar4;
        param_2 = uVar7 & 0xffff | ((uVar8 - uVar10) - uVar4 * uVar9) * 0x10000;
        uVar7 = uVar9 * uVar12;
        uVar11 = uVar9;
        if (param_2 <= uVar7 && uVar7 - param_2 != 0) {
          bVar13 = CARRY4(uVar3,param_2);
          param_2 = uVar3 + param_2;
          uVar11 = uVar9 - 1;
          if ((bVar13 == false) && (param_2 <= uVar7 && uVar7 - param_2 != 0)) {
            uVar11 = uVar9 - 2;
            param_2 = param_2 + uVar3;
          }
        }
        param_2 = param_2 - uVar7;
        uVar11 = uVar11 | uVar5 << 0x10;
      }
      uVar10 = param_2 / uVar4;
      uVar8 = param_1 >> 0x10 | (param_2 - uVar4 * uVar10) * 0x10000;
      uVar5 = uVar12 * uVar10;
      uVar7 = uVar10;
      if (uVar8 <= uVar5 && uVar5 - uVar8 != 0) {
        bVar13 = CARRY4(uVar3,uVar8);
        uVar8 = uVar3 + uVar8;
        uVar7 = uVar10 - 1;
        if ((bVar13 == false) && (uVar8 <= uVar5 && uVar5 - uVar8 != 0)) {
          uVar7 = uVar10 - 2;
          uVar8 = uVar8 + uVar3;
        }
      }
      uVar10 = (uVar8 - uVar5) / uVar4;
      uVar8 = param_1 & 0xffff | ((uVar8 - uVar5) - uVar4 * uVar10) * 0x10000;
      uVar12 = uVar12 * uVar10;
      uVar5 = uVar10;
      if (uVar8 <= uVar12 && uVar12 - uVar8 != 0) {
        bVar13 = CARRY4(uVar3,uVar8);
        uVar8 = uVar3 + uVar8;
        uVar5 = uVar10 - 1;
        if ((bVar13 == false) && (uVar8 <= uVar12 && uVar12 - uVar8 != 0)) {
          uVar8 = uVar8 + uVar3;
          uVar5 = uVar10 - 2;
        }
      }
      uVar8 = uVar8 - uVar12;
      uVar5 = uVar5 | uVar7 << 0x10;
    }
    if (param_5 != (uint *)0x0) {
      *param_5 = uVar8 >> LZCOUNT(param_3);
      param_5[1] = 0;
    }
  }
  else if (param_2 < param_4) {
    if (param_5 != (uint *)0x0) {
      *param_5 = param_1;
      param_5[1] = param_2;
      return 0;
    }
    uVar5 = 0;
    uVar11 = 0;
  }
  else {
    iVar6 = LZCOUNT(param_4);
    if (iVar6 != 0) {
      uVar8 = 0x20 - iVar6;
      uVar12 = param_3 >> (uVar8 & 0xff) | param_4 << iVar6;
      uVar7 = param_1 >> (uVar8 & 0xff) | param_2 << iVar6;
      param_2 = param_2 >> (uVar8 & 0xff);
      uVar4 = uVar12 >> 0x10;
      param_1 = param_1 << iVar6;
      uVar10 = param_2 / uVar4;
      uVar3 = uVar7 >> 0x10 | (param_2 - uVar4 * uVar10) * 0x10000;
      uVar11 = uVar10 * (uVar12 & 0xffff);
      uVar5 = uVar10;
      if (uVar3 <= uVar11 && uVar11 - uVar3 != 0) {
        bVar13 = CARRY4(uVar12,uVar3);
        uVar3 = uVar12 + uVar3;
        uVar5 = uVar10 - 1;
        if ((bVar13 == false) && (uVar3 <= uVar11 && uVar11 - uVar3 != 0)) {
          uVar5 = uVar10 - 2;
          uVar3 = uVar3 + uVar12;
        }
      }
      uVar10 = (uVar3 - uVar11) / uVar4;
      uVar3 = uVar7 & 0xffff | ((uVar3 - uVar11) - uVar4 * uVar10) * 0x10000;
      uVar7 = uVar10 * (uVar12 & 0xffff);
      uVar11 = uVar10;
      if (uVar3 <= uVar7 && uVar7 - uVar3 != 0) {
        bVar13 = CARRY4(uVar12,uVar3);
        uVar3 = uVar12 + uVar3;
        uVar11 = uVar10 - 1;
        if ((bVar13 == false) && (uVar3 <= uVar7 && uVar7 - uVar3 != 0)) {
          uVar11 = uVar10 - 2;
          uVar3 = uVar3 + uVar12;
        }
      }
      uVar11 = uVar11 | uVar5 << 0x10;
      uVar2 = (ulonglong)uVar11 * (ulonglong)(param_3 << iVar6);
      if (CONCAT44(uVar3 - uVar7,param_1) < uVar2) {
        uVar2 = uVar2 - CONCAT44(uVar12,param_3 << iVar6);
        uVar11 = uVar11 - 1;
      }
      if (param_5 != (uint *)0x0) {
        uVar5 = ((uVar3 - uVar7) - (int)(uVar2 >> 0x20)) - (uint)(param_1 < (uint)uVar2);
        *param_5 = uVar5 << (uVar8 & 0xff) | param_1 - (uint)uVar2 >> iVar6;
        param_5[1] = uVar5 >> iVar6;
      }
      return (ulonglong)uVar11;
    }
    if ((param_4 < param_2) || (param_3 <= param_1)) {
      bVar13 = param_1 < param_3;
      param_1 = param_1 - param_3;
      param_2 = (param_2 - param_4) - (uint)bVar13;
      uVar5 = 1;
    }
    else {
      uVar5 = 0;
    }
    uVar11 = 0;
    if (param_5 != (uint *)0x0) {
      *param_5 = param_1;
      param_5[1] = param_2;
    }
  }
  return CONCAT44(uVar11,uVar5);
}


