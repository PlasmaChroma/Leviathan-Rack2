/* 0800af40 FUN_0800af40; analyst naming is provisional. */

undefined4 FUN_0800af40(uint **param_1)

{
  ulonglong uVar1;
  int iVar2;
  int iVar3;
  uint *puVar4;
  uint *puVar5;
  uint uVar6;
  uint uVar7;
  uint *puVar8;
  uint *puVar9;
  uint uVar10;
  
  iVar2 = FUN_08009ce8();
  uVar7 = DAT_0800b394;
  if (param_1 == (uint **)0x0) {
    return 1;
  }
  puVar8 = *param_1;
  if ((((puVar8 == DAT_0800b19c) || (puVar8 == DAT_0800b19c + 6)) ||
      (puVar8 == DAT_0800b1a0 + 0x112 ||
       (puVar8 == DAT_0800b1a0 + 0x10c ||
       (puVar8 == DAT_0800b1a0 + 0x106 ||
       (puVar8 == DAT_0800b1a0 + 0x100 ||
       (puVar8 == DAT_0800b1a0 + 0xfa ||
       (puVar8 == DAT_0800b1a0 + 0xf4 ||
       (puVar8 == DAT_0800b1a0 + 0xee ||
       (puVar8 == DAT_0800b1a0 + 0x18 ||
       (puVar8 == DAT_0800b1a0 + 0x12 ||
       (puVar8 == DAT_0800b1a0 + 0xc ||
       (puVar8 == DAT_0800b1a0 + 6 || (puVar8 == DAT_0800b1a0 || puVar8 == DAT_0800b19c + 0xc)))))))
       )))))) || (puVar8 == DAT_0800b1a4)) {
    *(undefined *)(param_1 + 0xd) = 0;
    *(undefined *)((int)param_1 + 0x35) = 2;
    *puVar8 = *puVar8 & 0xfffffffe;
    while ((int)(*puVar8 << 0x1f) < 0) {
      iVar3 = FUN_08009ce8();
      if (5 < (uint)(iVar3 - iVar2)) {
        param_1[0x15] = (uint *)0x20;
        *(undefined *)((int)param_1 + 0x35) = 3;
        return 1;
      }
      puVar8 = *param_1;
    }
    puVar5 = param_1[6];
    uVar7 = (uint)param_1[2] | (uint)param_1[3] | (uint)param_1[4] | (uint)param_1[5] | (uint)puVar5
            | (uint)param_1[7] | (uint)param_1[8] | DAT_0800b1a8 & *puVar8;
    puVar4 = param_1[9];
    if (puVar4 == (uint *)&UndefinedInstruction) {
      puVar9 = param_1[0xb];
      uVar7 = uVar7 | (uint)param_1[0xc] | (uint)puVar9;
      if (0x1fffffff < (DAT_0800b380 & *DAT_0800b1ac)) goto LAB_0800b1c8;
      *puVar8 = uVar7;
      uVar7 = puVar8[5] & 0xfffffff8 | 4;
LAB_0800b1f0:
      puVar4 = param_1[10];
      uVar7 = uVar7 | (uint)puVar4;
      if (puVar9 != (uint *)0x0) {
        if (puVar5 == (uint *)0x0) {
          if (puVar4 == (uint *)0x1) {
switchD_0800b28e_caseD_3:
            if (puVar9 == (uint *)0x1800000) {
switchD_0800b28e_caseD_0:
              param_1[0x15] = (uint *)0x40;
              *(undefined *)((int)param_1 + 0x35) = 1;
              return 1;
            }
          }
          else if (((uint)puVar4 & 0xfffffffd) == 0) goto switchD_0800b28e_caseD_1;
        }
        else if (puVar5 == (uint *)0x2000) {
          switch(puVar4) {
          case (uint *)0x0:
          case (uint *)0x2:
            goto switchD_0800b28e_caseD_0;
          case (uint *)0x1:
switchD_0800b28e_caseD_1:
            if ((int)puVar9 << 7 < 0) goto switchD_0800b28e_caseD_0;
            break;
          case (uint *)0x3:
            goto switchD_0800b28e_caseD_3;
          }
        }
        else {
          if (puVar4 < (uint *)0x3) goto switchD_0800b28e_caseD_0;
          if (puVar4 == (uint *)0x3) goto switchD_0800b28e_caseD_1;
        }
      }
    }
    else if ((DAT_0800b1b0 & *DAT_0800b1ac) < 0x20000000) {
      *puVar8 = uVar7;
      uVar7 = puVar8[5] & 0xfffffff8 | (uint)puVar4;
    }
    else {
LAB_0800b1c8:
      uVar10 = (int)param_1[1] - 0x29;
      if (uVar10 < 0x20) {
        if ((int)((DAT_0800b37c >> (uVar10 & 0xff)) << 0x1f) < 0) goto LAB_0800b1d8;
      }
      else if ((int)param_1[1] - 0x4fU < 4) {
LAB_0800b1d8:
        uVar7 = uVar7 | 0x100000;
      }
      *puVar8 = uVar7;
      uVar7 = puVar8[5] & 0xfffffff8 | (uint)puVar4;
      if (puVar4 == (uint *)&UndefinedInstruction) {
        puVar9 = param_1[0xb];
        goto LAB_0800b1f0;
      }
    }
    puVar8[5] = uVar7;
    iVar2 = FUN_0800acd0(param_1);
    *(int *)(iVar2 + 8) = 0x3f << ((uint)param_1[0x17] & 0x1f);
  }
  else {
    if ((puVar8 != DAT_0800b38c + 0x14 &&
         (puVar8 != DAT_0800b388 + 0x14 &&
         (puVar8 != DAT_0800b38c + 10 &&
         (puVar8 != DAT_0800b388 + 10 &&
         (puVar8 != DAT_0800b38c && (puVar8 != DAT_0800b388 && puVar8 != DAT_0800b384)))))) &&
       (puVar8 != DAT_0800b390)) {
      param_1[0x15] = (uint *)0x40;
      *(undefined *)((int)param_1 + 0x35) = 3;
      return 1;
    }
    *(undefined *)((int)param_1 + 0x35) = 2;
    *(undefined *)(param_1 + 0xd) = 0;
    if (param_1[2] == (uint *)0x40) {
      uVar10 = 0x10;
    }
    else if (param_1[2] == (uint *)0x80) {
      uVar10 = 0x4000;
    }
    else {
      uVar10 = 0;
    }
    uVar6 = DAT_0800b398 + (int)puVar8;
    uVar1 = (ulonglong)DAT_0800b39c;
    *puVar8 = ((uint)param_1[4] | (uint)param_1[3] | (uint)param_1[5] | (uint)param_1[6] |
              (uint)param_1[7]) >> 3 | (uint)param_1[8] >> 4 | uVar7 & *puVar8 | uVar10;
    param_1[0x17] = (uint *)((uint)(uVar1 * uVar6 >> 0x24) << 2);
    iVar2 = FUN_0800acd0(param_1);
    *(int *)(iVar2 + 4) = 1 << ((uint)param_1[0x17] & 0x1f);
  }
  puVar8 = *param_1;
  if ((puVar8 == DAT_0800b1bc + 0x1e ||
       (puVar8 == DAT_0800b1c0 + 0x14 ||
       (puVar8 == DAT_0800b1bc + 0x14 ||
       (puVar8 == DAT_0800b1c0 + 10 ||
       (puVar8 == DAT_0800b1bc + 10 ||
       (puVar8 == DAT_0800b1c0 ||
       (puVar8 == DAT_0800b1bc ||
       (puVar8 == DAT_0800b1b4 + 0x124 ||
       (puVar8 == DAT_0800b1b8 + 0x118 ||
       (puVar8 == DAT_0800b1b4 + 0x118 ||
       (puVar8 == DAT_0800b1b8 + 0x10c ||
       (puVar8 == DAT_0800b1b4 + 0x10c ||
       (puVar8 == DAT_0800b1b8 + 0x100 ||
       (puVar8 == DAT_0800b1b4 + 0x100 ||
       (puVar8 == DAT_0800b1b8 + 0xf4 ||
       (puVar8 == DAT_0800b1b4 + 0x24 ||
       (puVar8 == DAT_0800b1b8 + 0x18 ||
       (puVar8 == DAT_0800b1b4 + 0x18 ||
       (puVar8 == DAT_0800b1b8 + 0xc ||
       (puVar8 == DAT_0800b1b4 + 0xc ||
       (puVar8 == DAT_0800b1b8 || (puVar8 == DAT_0800b1b4 || puVar8 == DAT_0800b19c)))))))))))))))))
       ))))) || (puVar8 == DAT_0800b1c4)) {
    FUN_0800adc4(param_1);
    if (param_1[2] == (uint *)0x80) {
      puVar8 = param_1[0x1a];
      puVar4 = param_1[0x19];
      param_1[1] = (uint *)0x0;
      *param_1[0x18] = 0;
      puVar4[1] = (uint)puVar8;
    }
    else {
      puVar8 = param_1[1];
      *param_1[0x18] = (uint)puVar8 & 0xff;
      param_1[0x19][1] = (uint)param_1[0x1a];
      if ((int)puVar8 - 1U < 8) {
        FUN_0800ae9c();
        puVar4 = param_1[0x1c];
        puVar8 = param_1[0x1d];
        *param_1[0x1b] = 0;
        puVar4[1] = (uint)puVar8;
        goto LAB_0800b18e;
      }
    }
    param_1[0x1b] = (uint *)0x0;
    param_1[0x1c] = (uint *)0x0;
    param_1[0x1d] = (uint *)0x0;
  }
LAB_0800b18e:
  param_1[0x15] = (uint *)0x0;
  *(undefined *)((int)param_1 + 0x35) = 1;
  return 0;
}


