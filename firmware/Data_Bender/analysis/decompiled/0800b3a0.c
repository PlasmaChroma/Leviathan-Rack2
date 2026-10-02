/* 0800b3a0 FUN_0800b3a0; analyst naming is provisional. */

undefined4 FUN_0800b3a0(uint **param_1)

{
  uint *puVar1;
  uint *puVar2;
  
  if (param_1 == (uint **)0x0) {
    return 1;
  }
  if (*(char *)(param_1 + 0xd) == '\x01') {
    return 2;
  }
  *(undefined *)(param_1 + 0xd) = 1;
  if (*(char *)((int)param_1 + 0x35) != '\x01') {
    param_1[0x15] = (uint *)0x800;
    *(undefined *)(param_1 + 0xd) = 0;
    return 1;
  }
  *(undefined *)((int)param_1 + 0x35) = 2;
  param_1[0x15] = (uint *)0x0;
  **param_1 = **param_1 & 0xfffffffe;
  FUN_0800ab00(param_1);
  puVar2 = *param_1;
  if (((puVar2 == DAT_0800b538 || puVar2 == DAT_0800b534) ||
      (puVar2 == DAT_0800b53c + 0x118 ||
       (puVar2 == DAT_0800b53c + 0x112 ||
       (puVar2 == DAT_0800b53c + 0x10c ||
       (puVar2 == DAT_0800b53c + 0x106 ||
       (puVar2 == DAT_0800b53c + 0x100 ||
       (puVar2 == DAT_0800b53c + 0xfa ||
       (puVar2 == DAT_0800b53c + 0xf4 ||
       (puVar2 == DAT_0800b53c + 0x1e ||
       (puVar2 == DAT_0800b53c + 0x18 ||
       (puVar2 == DAT_0800b53c + 0x12 ||
       (puVar2 == DAT_0800b53c + 0xc || (puVar2 == DAT_0800b53c || puVar2 == DAT_0800b538 + 0xc)))))
       )))))))) || (puVar2 == DAT_0800b540)) {
    puVar1 = param_1[0x10];
    *puVar2 = *puVar2 & 0xffffffe1 | 0x16;
    if (puVar1 != (uint *)0x0) {
      *puVar2 = *puVar2 | 8;
    }
  }
  else {
    puVar1 = param_1[0x10];
    *puVar2 = *puVar2 & 0xfffffff1 | 10;
    if (puVar1 != (uint *)0x0) {
      *puVar2 = *puVar2 | 4;
    }
    if ((puVar2 != DAT_0800b548 + 0x1e &&
         (puVar2 != DAT_0800b548 + 0x19 &&
         (puVar2 != DAT_0800b548 + 0x14 &&
         (puVar2 != DAT_0800b548 + 0xf &&
         (puVar2 != DAT_0800b548 + 10 && (puVar2 != DAT_0800b548 && puVar2 != DAT_0800b544)))))) &&
       (puVar2 != DAT_0800b54c)) goto LAB_0800b4b6;
  }
  puVar1 = param_1[0x18];
  if ((int)(*puVar1 << 0xf) < 0) {
    *puVar1 = *puVar1 | 0x100;
  }
  puVar1 = param_1[0x1b];
  if (puVar1 != (uint *)0x0) {
    *puVar1 = *puVar1 | 0x100;
  }
LAB_0800b4b6:
  *puVar2 = *puVar2 | 1;
  return 0;
}


