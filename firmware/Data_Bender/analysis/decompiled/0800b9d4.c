/* 0800b9d4 FUN_0800b9d4; analyst naming is provisional. */

/* WARNING: Type propagation algorithm not settling */

void FUN_0800b9d4(uint **param_1)

{
  int iVar1;
  uint uVar2;
  uint *puVar3;
  undefined uVar4;
  uint *puVar5;
  uint *puVar6;
  uint uVar7;
  uint *UNRECOVERED_JUMPTABLE_00;
  uint uVar8;
  bool bVar9;
  uint local_24;
  
  uVar7 = *DAT_0800bc4c;
  local_24 = 0;
  puVar5 = *param_1;
  puVar6 = param_1[0x16];
  uVar8 = *puVar6;
  UNRECOVERED_JUMPTABLE_00 = DAT_0800bc50;
  if (puVar5 == DAT_0800bc54 || puVar5 == DAT_0800bc50) {
    UNRECOVERED_JUMPTABLE_00 = (uint *)0x1;
  }
  uVar2 = *puVar6;
  if (((puVar5 != DAT_0800bc54 && puVar5 != DAT_0800bc50) &&
      (UNRECOVERED_JUMPTABLE_00 = (uint *)0x0,
      puVar5 != DAT_0800bc58 + 0x112 &&
      (puVar5 != DAT_0800bc58 + 0x10c &&
      (puVar5 != DAT_0800bc58 + 0x106 &&
      (puVar5 != DAT_0800bc58 + 0x100 &&
      (puVar5 != DAT_0800bc58 + 0xfa &&
      (puVar5 != DAT_0800bc58 + 0xf4 &&
      (puVar5 != DAT_0800bc58 + 0xee &&
      (puVar5 != DAT_0800bc58 + 0x18 &&
      (puVar5 != DAT_0800bc58 + 0x12 &&
      (puVar5 != DAT_0800bc58 + 0xc &&
      (puVar5 != DAT_0800bc58 + 6 && (puVar5 != DAT_0800bc58 && puVar5 != DAT_0800bc54 + 6))))))))))
      ))) && (puVar5 != DAT_0800bc5c)) {
    if ((puVar5 != DAT_0800c06c + 0x19 &&
         (puVar5 != DAT_0800c06c + 0x14 &&
         (puVar5 != DAT_0800c06c + 0xf &&
         (puVar5 != DAT_0800c06c + 10 &&
         (puVar5 != DAT_0800c06c + 5 && (puVar5 != DAT_0800c06c && puVar5 != DAT_0800c068)))))) &&
       (puVar5 != DAT_0800c070)) {
      return;
    }
    uVar7 = *puVar5;
    uVar8 = (uint)param_1[0x17] & 0x1f;
    if (((4 << uVar8 & uVar2) == 0) || (-1 < (int)(uVar7 << 0x1d))) {
      if (((2 << uVar8 & uVar2) == 0) || (-1 < (int)(uVar7 << 0x1e))) {
        if ((8 << uVar8 & uVar2) == 0) {
          return;
        }
        if (-1 < (int)(uVar7 << 0x1c)) {
          return;
        }
        *puVar5 = *puVar5 & 0xfffffff1;
        UNRECOVERED_JUMPTABLE_00 = param_1[0x13];
        puVar6[1] = 1 << uVar8;
        param_1[0x15] = (uint *)0x1;
        *(undefined *)(param_1 + 0xd) = 0;
        *(undefined *)((int)param_1 + 0x35) = 1;
        if (UNRECOVERED_JUMPTABLE_00 == (uint *)0x0) {
          return;
        }
                    /* WARNING: Could not recover jumptable at 0x0800c02a. Too many branches */
                    /* WARNING: Treating indirect jump as call */
        (*(code *)UNRECOVERED_JUMPTABLE_00)(param_1);
        return;
      }
      puVar6[1] = 2 << uVar8;
      if ((int)(uVar7 << 0x10) < 0) {
        if (-1 < (int)(uVar7 << 0xf)) {
          UNRECOVERED_JUMPTABLE_00 = param_1[0x11];
          goto joined_r0x0800bf9e;
        }
      }
      else if ((uVar7 & 0x20) == 0) {
        *puVar5 = *puVar5 & 0xfffffff5;
        *(undefined *)((int)param_1 + 0x35) = 1;
        *(undefined *)(param_1 + 0xd) = 0;
      }
      UNRECOVERED_JUMPTABLE_00 = param_1[0xf];
    }
    else {
      puVar6[1] = 4 << uVar8;
      if ((int)(uVar7 << 0x10) < 0) {
        if (-1 < (int)(uVar7 << 0xf)) {
          UNRECOVERED_JUMPTABLE_00 = param_1[0x12];
          goto joined_r0x0800bf9e;
        }
      }
      else if (-1 < (int)(uVar7 << 0x1a)) {
        *puVar5 = *puVar5 & 0xfffffffb;
      }
      UNRECOVERED_JUMPTABLE_00 = param_1[0x10];
    }
    goto joined_r0x0800bf9e;
  }
  puVar3 = param_1[0x17];
  uVar2 = (uint)puVar3 & 0x1f;
  if ((uVar8 & 8 << uVar2) == 0) {
    if ((int)((uVar8 >> uVar2) << 0x1f) < 0) {
LAB_0800baa8:
      if ((int)(puVar5[5] << 0x18) < 0) {
        puVar6[2] = 1 << uVar2;
        param_1[0x15] = (uint *)((uint)param_1[0x15] | 2);
      }
    }
LAB_0800bac2:
    if ((4 << uVar2 & uVar8) == 0) {
LAB_0800bb86:
      if ((0x10 << uVar2 & uVar8) != 0) {
        if (UNRECOVERED_JUMPTABLE_00 == (uint *)0x0) goto LAB_0800bb96;
        goto LAB_0800bc22;
      }
    }
    else {
      if (((UNRECOVERED_JUMPTABLE_00 != (uint *)0x0) ||
          (puVar5 == DAT_0800bc60 + 0x118 ||
           (puVar5 == DAT_0800bc60 + 0x112 ||
           (puVar5 == DAT_0800bc60 + 0x10c ||
           (puVar5 == DAT_0800bc60 + 0x106 ||
           (puVar5 == DAT_0800bc60 + 0x100 ||
           (puVar5 == DAT_0800bc60 + 0xfa ||
           (puVar5 == DAT_0800bc60 + 0xf4 ||
           (puVar5 == DAT_0800bc60 + 0x1e ||
           (puVar5 == DAT_0800bc60 + 0x18 ||
           (puVar5 == DAT_0800bc60 + 0x12 ||
           (puVar5 == DAT_0800bc60 + 0xc || (puVar5 == DAT_0800bc60 || puVar5 == DAT_0800bc58)))))))
           )))))) || (puVar5 == DAT_0800bc5c)) {
        if ((int)(*puVar5 << 0x1e) < 0) {
          puVar6[2] = 4 << uVar2;
          param_1[0x15] = (uint *)((uint)param_1[0x15] | 4);
        }
        goto LAB_0800bb86;
      }
LAB_0800bfa4:
      if ((uVar8 & 0x10 << uVar2) != 0) goto LAB_0800bfb4;
    }
  }
  else {
    if ((int)(*puVar5 << 0x1d) < 0) {
      *puVar5 = *puVar5 & 0xfffffffb;
      puVar6[2] = 8 << uVar2;
      param_1[0x15] = (uint *)((uint)param_1[0x15] | 1);
    }
    if (-1 < (int)((uVar8 >> uVar2) << 0x1f)) goto LAB_0800bac2;
    if ((puVar5 == DAT_0800bf28 + 0x11e ||
         (puVar5 == DAT_0800bf28 + 0x118 ||
         (puVar5 == DAT_0800bf28 + 0x112 ||
         (puVar5 == DAT_0800bf28 + 0x10c ||
         (puVar5 == DAT_0800bf28 + 0x106 ||
         (puVar5 == DAT_0800bf28 + 0x100 ||
         (puVar5 == DAT_0800bf28 + 0xfa ||
         (puVar5 == DAT_0800bf28 + 0xf4 ||
         (puVar5 == DAT_0800bf28 + 0x1e ||
         (puVar5 == DAT_0800bf28 + 0x18 ||
         (puVar5 == DAT_0800bf28 + 0x12 ||
         (puVar5 == DAT_0800bf28 + 0xc || (puVar5 == DAT_0800bf28 || puVar5 == DAT_0800bf24)))))))))
         )))) || (UNRECOVERED_JUMPTABLE_00 != (uint *)0x0)) goto LAB_0800baa8;
    if ((4 << uVar2 & uVar8) != 0) goto LAB_0800bfa4;
    if ((uVar8 & 0x10 << uVar2) == 0) goto LAB_0800bc6c;
LAB_0800bb96:
    if ((puVar5 == DAT_0800bc60 + 0x118 ||
         (puVar5 == DAT_0800bc60 + 0x112 ||
         (puVar5 == DAT_0800bc60 + 0x10c ||
         (puVar5 == DAT_0800bc60 + 0x106 ||
         (puVar5 == DAT_0800bc60 + 0x100 ||
         (puVar5 == DAT_0800bc60 + 0xfa ||
         (puVar5 == DAT_0800bc60 + 0xf4 ||
         (puVar5 == DAT_0800bc60 + 0x1e ||
         (puVar5 == DAT_0800bc60 + 0x18 ||
         (puVar5 == DAT_0800bc60 + 0x12 ||
         (puVar5 == DAT_0800bc60 + 0xc || (puVar5 == DAT_0800bc60 || puVar5 == DAT_0800bc58)))))))))
         ))) || (puVar5 == DAT_0800bc5c)) {
LAB_0800bc22:
      iVar1 = *puVar5 << 0x1c;
    }
    else {
LAB_0800bfb4:
      iVar1 = *puVar5 << 0x1d;
    }
    if (iVar1 < 0) {
      puVar6[2] = 0x10 << uVar2;
      if ((int)(*puVar5 << 0xd) < 0) {
        if (-1 < (int)(*puVar5 << 0xc)) goto LAB_0800bc42;
        UNRECOVERED_JUMPTABLE_00 = param_1[0x12];
      }
      else {
        if (-1 < (int)(*puVar5 << 0x17)) {
          *puVar5 = *puVar5 & 0xfffffff7;
        }
LAB_0800bc42:
        UNRECOVERED_JUMPTABLE_00 = param_1[0x10];
      }
      if (UNRECOVERED_JUMPTABLE_00 != (uint *)0x0) {
        (*(code *)UNRECOVERED_JUMPTABLE_00)(param_1);
        puVar3 = param_1[0x17];
      }
    }
  }
LAB_0800bc6c:
  uVar2 = 0x20 << ((uint)puVar3 & 0x1f);
  if ((uVar2 & uVar8) != 0) {
    UNRECOVERED_JUMPTABLE_00 = *param_1;
    if ((UNRECOVERED_JUMPTABLE_00 == DAT_0800bf18 + 0x11e ||
         (UNRECOVERED_JUMPTABLE_00 == DAT_0800bf18 + 0x118 ||
         (UNRECOVERED_JUMPTABLE_00 == DAT_0800bf18 + 0x112 ||
         (UNRECOVERED_JUMPTABLE_00 == DAT_0800bf18 + 0x10c ||
         (UNRECOVERED_JUMPTABLE_00 == DAT_0800bf18 + 0x106 ||
         (UNRECOVERED_JUMPTABLE_00 == DAT_0800bf18 + 0x100 ||
         (UNRECOVERED_JUMPTABLE_00 == DAT_0800bf18 + 0xfa ||
         (UNRECOVERED_JUMPTABLE_00 == DAT_0800bf18 + 0x24 ||
         (UNRECOVERED_JUMPTABLE_00 == DAT_0800bf18 + 0x1e ||
         (UNRECOVERED_JUMPTABLE_00 == DAT_0800bf18 + 0x18 ||
         (UNRECOVERED_JUMPTABLE_00 == DAT_0800bf18 + 0x12 ||
         (UNRECOVERED_JUMPTABLE_00 == DAT_0800bf18 + 0xc ||
         (UNRECOVERED_JUMPTABLE_00 == DAT_0800bf18 + 6 ||
         (UNRECOVERED_JUMPTABLE_00 == DAT_0800bf18 || UNRECOVERED_JUMPTABLE_00 == DAT_0800bf14))))))
         )))))))) || (UNRECOVERED_JUMPTABLE_00 == DAT_0800bf1c)) {
      iVar1 = *UNRECOVERED_JUMPTABLE_00 << 0x1b;
    }
    else {
      iVar1 = *UNRECOVERED_JUMPTABLE_00 << 0x1e;
    }
    if (iVar1 < 0) {
      puVar6[2] = uVar2;
      if (*(char *)((int)param_1 + 0x35) == '\x04') {
        *UNRECOVERED_JUMPTABLE_00 = *UNRECOVERED_JUMPTABLE_00 & 0xffffffe9;
        UNRECOVERED_JUMPTABLE_00[5] = UNRECOVERED_JUMPTABLE_00[5] & 0xffffff7f;
        if ((param_1[0x10] != (uint *)0x0) || (param_1[0x12] != (uint *)0x0)) {
          *UNRECOVERED_JUMPTABLE_00 = *UNRECOVERED_JUMPTABLE_00 & 0xfffffff7;
        }
        UNRECOVERED_JUMPTABLE_00 = param_1[0x14];
        puVar6[2] = 0x3f << ((uint)puVar3 & 0x1f);
        *(undefined *)((int)param_1 + 0x35) = 1;
        *(undefined *)(param_1 + 0xd) = 0;
        goto joined_r0x0800bf9e;
      }
      if ((*UNRECOVERED_JUMPTABLE_00 & 0x40000) == 0) {
        if ((*UNRECOVERED_JUMPTABLE_00 & 0x100) == 0) {
          *UNRECOVERED_JUMPTABLE_00 = *UNRECOVERED_JUMPTABLE_00 & 0xffffffef;
          *(undefined *)(param_1 + 0xd) = 0;
          *(undefined *)((int)param_1 + 0x35) = 1;
        }
LAB_0800bd40:
        UNRECOVERED_JUMPTABLE_00 = param_1[0xf];
      }
      else {
        if ((int)(*UNRECOVERED_JUMPTABLE_00 << 0xc) < 0) goto LAB_0800bd40;
        UNRECOVERED_JUMPTABLE_00 = param_1[0x11];
      }
      if (UNRECOVERED_JUMPTABLE_00 != (uint *)0x0) {
        (*(code *)UNRECOVERED_JUMPTABLE_00)(param_1);
      }
    }
  }
  if (param_1[0x15] == (uint *)0x0) {
    return;
  }
  if ((int)param_1[0x15] << 0x1f < 0) {
    UNRECOVERED_JUMPTABLE_00 = *param_1;
    *(undefined *)((int)param_1 + 0x35) = 4;
    *UNRECOVERED_JUMPTABLE_00 = *UNRECOVERED_JUMPTABLE_00 & 0xfffffffe;
    do {
      local_24 = local_24 + 1;
      if ((uint)((ulonglong)DAT_0800bf20 * (ulonglong)uVar7 >> 0x2a) < local_24) break;
    } while ((int)(*UNRECOVERED_JUMPTABLE_00 << 0x1f) < 0);
    iVar1 = *UNRECOVERED_JUMPTABLE_00 << 0x1f;
    bVar9 = iVar1 < 0;
    if (bVar9) {
      iVar1 = 3;
    }
    uVar4 = (undefined)iVar1;
    if (!bVar9) {
      uVar4 = 1;
    }
    *(undefined *)((int)param_1 + 0x35) = uVar4;
    *(undefined *)(param_1 + 0xd) = 0;
  }
  UNRECOVERED_JUMPTABLE_00 = param_1[0x13];
joined_r0x0800bf9e:
  if (UNRECOVERED_JUMPTABLE_00 == (uint *)0x0) {
    return;
  }
                    /* WARNING: Could not recover jumptable at 0x0800bdae. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)UNRECOVERED_JUMPTABLE_00)(param_1);
  return;
}


