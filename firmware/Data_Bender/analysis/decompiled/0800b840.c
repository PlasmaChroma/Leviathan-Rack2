/* 0800b840 FUN_0800b840; analyst naming is provisional. */

undefined4 FUN_0800b840(uint **param_1)

{
  bool bVar1;
  uint *puVar2;
  uint *puVar3;
  uint *puVar4;
  bool bVar5;
  
  puVar2 = DAT_0800b9c8;
  if (param_1 == (uint **)0x0) {
    return 1;
  }
  if (*(char *)((int)param_1 + 0x35) != '\x02') {
    param_1[0x15] = (uint *)0x80;
    return 1;
  }
  puVar3 = *param_1;
  if ((((puVar3 == DAT_0800b9b0) || (puVar3 == DAT_0800b9b0 + 6)) ||
      (puVar3 == DAT_0800b9b8 + 0x10c ||
       (puVar3 == DAT_0800b9b4 + 0x112 ||
       (puVar3 == DAT_0800b9b8 + 0x100 ||
       (puVar3 == DAT_0800b9b4 + 0x106 ||
       (puVar3 == DAT_0800b9b8 + 0xf4 ||
       (puVar3 == DAT_0800b9b4 + 0xfa ||
       (puVar3 == DAT_0800b9b8 + 0xe8 ||
       (puVar3 == DAT_0800b9b4 + 0x1e ||
       (puVar3 == DAT_0800b9b8 + 0xc ||
       (puVar3 == DAT_0800b9b4 + 0x12 ||
       (puVar3 == DAT_0800b9b8 || (puVar3 == DAT_0800b9b4 || puVar3 == DAT_0800b9b0 + 0x12))))))))))
       ))) || (puVar3 == DAT_0800b9bc)) {
    *(undefined *)((int)param_1 + 0x35) = 4;
    *puVar3 = *puVar3 & 0xfffffffe;
    return 0;
  }
  bVar5 = puVar3 == DAT_0800b9c4;
  bVar1 = puVar3 == DAT_0800b9c0;
  puVar4 = DAT_0800b9c4 + 0xf;
  *puVar3 = *puVar3 & 0xfffffff1;
  *puVar3 = *puVar3 & 0xfffffffe;
  if ((puVar3 == DAT_0800b9cc ||
       (puVar3 == puVar2 + 0xf ||
       (puVar3 == puVar2 + 10 || (puVar3 == puVar4 || (puVar3 == puVar2 || (bVar5 || bVar1)))))) ||
     (puVar3 == DAT_0800b9d0)) {
    puVar3 = param_1[0x16];
    puVar2 = param_1[0x17];
    *param_1[0x18] = *param_1[0x18] & 0xfffffeff;
    puVar3[1] = 1 << ((uint)puVar2 & 0x1f);
    puVar2 = param_1[0x1b];
    param_1[0x19][1] = (uint)param_1[0x1a];
    if (puVar2 != (uint *)0x0) {
      puVar3 = param_1[0x1c];
      puVar4 = param_1[0x1d];
      *puVar2 = *puVar2 & 0xfffffeff;
      puVar3[1] = (uint)puVar4;
    }
  }
  *(undefined *)((int)param_1 + 0x35) = 1;
  *(undefined *)(param_1 + 0xd) = 0;
  if (param_1[0x14] == (uint *)0x0) {
    return 0;
  }
  (*(code *)param_1[0x14])(param_1);
  return 0;
}


