/* 0800b550 FUN_0800b550; analyst naming is provisional. */

undefined4 FUN_0800b550(uint **param_1)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  uint *puVar4;
  uint *puVar5;
  uint *puVar6;
  bool bVar7;
  
  iVar2 = FUN_08009ce8();
  puVar5 = DAT_0800b7c0;
  if (param_1 == (uint **)0x0) {
    return 1;
  }
  if (*(char *)((int)param_1 + 0x35) != '\x02') {
    param_1[0x15] = (uint *)0x80;
    *(undefined *)(param_1 + 0xd) = 0;
    return 1;
  }
  puVar6 = *param_1;
  if (puVar6 == DAT_0800b7b8 || puVar6 == DAT_0800b7b4) {
    bVar1 = true;
LAB_0800b610:
    *puVar6 = *puVar6 & 0xffffffe1;
    puVar6[5] = puVar6[5] & 0xffffff7f;
    if (bVar1) {
      *param_1[0x18] = *param_1[0x18] & 0xfffffeff;
    }
    else {
LAB_0800b786:
      *param_1[0x18] = *param_1[0x18] & 0xfffffeff;
    }
  }
  else {
    bVar1 = false;
    if (puVar6 == DAT_0800b7b0 + 0x11e ||
        (puVar6 == DAT_0800b7b0 + 0x118 ||
        (puVar6 == DAT_0800b7b0 + 0x112 ||
        (puVar6 == DAT_0800b7b0 + 0x10c ||
        (puVar6 == DAT_0800b7b0 + 0x106 ||
        (puVar6 == DAT_0800b7b0 + 0x100 ||
        (puVar6 == DAT_0800b7b0 + 0xfa ||
        (puVar6 == DAT_0800b7b0 + 0xf4 ||
        (puVar6 == DAT_0800b7b0 + 0x1e ||
        (puVar6 == DAT_0800b7b0 + 0x18 ||
        (puVar6 == DAT_0800b7b0 + 0x12 ||
        (puVar6 == DAT_0800b7b0 + 0xc || (puVar6 == DAT_0800b7b0 || puVar6 == DAT_0800b7ac))))))))))
        ))) goto LAB_0800b610;
    *puVar6 = *puVar6 & 0xfffffff1;
    if ((puVar6 == puVar5 + 0x1e ||
         (puVar6 == puVar5 + 0x19 ||
         (puVar6 == puVar5 + 0x14 ||
         (puVar6 == puVar5 + 0xf ||
         (puVar6 == puVar5 + 10 || (puVar6 == puVar5 || puVar6 == DAT_0800b7c4)))))) ||
       (puVar6 == DAT_0800b7c8)) goto LAB_0800b786;
  }
  *puVar6 = *puVar6 & 0xfffffffe;
  while ((int)(*puVar6 << 0x1f) < 0) {
    iVar3 = FUN_08009ce8();
    if (5 < (uint)(iVar3 - iVar2)) {
      param_1[0x15] = (uint *)0x20;
      *(undefined *)(param_1 + 0xd) = 0;
      *(undefined *)((int)param_1 + 0x35) = 3;
      return 1;
    }
  }
  puVar5 = *param_1;
  if ((puVar5 == DAT_0800b7bc + 0x112 ||
       (puVar5 == DAT_0800b7bc + 0x10c ||
       (puVar5 == DAT_0800b7b0 + 0x112 ||
       (puVar5 == DAT_0800b7bc + 0x100 ||
       (puVar5 == DAT_0800b7b0 + 0x106 ||
       (puVar5 == DAT_0800b7bc + 0xf4 ||
       (puVar5 == DAT_0800b7b0 + 0xfa ||
       (puVar5 == DAT_0800b7bc + 0xe8 ||
       (puVar5 == DAT_0800b7b0 + 0x1e ||
       (puVar5 == DAT_0800b7bc + 0xc ||
       (puVar5 == DAT_0800b7b0 + 0x12 ||
       (puVar5 == DAT_0800b7bc || (puVar5 == DAT_0800b7b0 || puVar5 == DAT_0800b7ac))))))))))))) ||
     (puVar5 == DAT_0800b7b8 || puVar5 == DAT_0800b7bc + -0x18)) {
    param_1[0x16][2] = 0x3f << ((uint)param_1[0x17] & 0x1f);
  }
  else {
    bVar7 = puVar5 != DAT_0800b830;
    bVar1 = puVar5 != DAT_0800b82c;
    param_1[0x16][1] = 1 << ((uint)param_1[0x17] & 0x1f);
    if ((puVar5 != DAT_0800b838 + 0xf &&
         (puVar5 != DAT_0800b838 + 10 &&
         (puVar5 != DAT_0800b834 + 10 &&
         (puVar5 != DAT_0800b838 && (puVar5 != DAT_0800b834 && (bVar7 && bVar1)))))) &&
       (puVar5 != DAT_0800b83c)) goto LAB_0800b712;
  }
  puVar5 = param_1[0x1b];
  param_1[0x19][1] = (uint)param_1[0x1a];
  if (puVar5 != (uint *)0x0) {
    puVar4 = param_1[0x1c];
    puVar6 = param_1[0x1d];
    *puVar5 = *puVar5 & 0xfffffeff;
    puVar4[1] = (uint)puVar6;
  }
LAB_0800b712:
  *(undefined *)((int)param_1 + 0x35) = 1;
  *(undefined *)(param_1 + 0xd) = 0;
  return 0;
}


