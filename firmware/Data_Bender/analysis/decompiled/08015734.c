/* 08015734 FUN_08015734; analyst naming is provisional. */

undefined FUN_08015734(uint **param_1,uint *param_2,uint param_3)

{
  ushort uVar1;
  int iVar2;
  uint *puVar3;
  uint *puVar4;
  uint uVar5;
  
  if (*(char *)(param_1 + 0x20) == '\x01') {
    return 2;
  }
  *(undefined *)(param_1 + 0x20) = 1;
  if (*(char *)((int)param_1 + 0x81) != '\x01') {
    *(undefined *)(param_1 + 0x20) = 0;
    return 2;
  }
  if (param_2 == (uint *)0x0) {
LAB_08015822:
    *(undefined *)(param_1 + 0x20) = 0;
    return 1;
  }
  puVar4 = (uint *)(uint)(param_3 == 0);
  if (param_3 == 0) goto LAB_08015822;
  param_1[0x19] = puVar4;
  uVar1 = (ushort)(param_3 == 0);
  *(ushort *)(param_1 + 0x1a) = uVar1;
  *(undefined *)((int)param_1 + 0x81) = 3;
  param_1[0x21] = puVar4;
  *(short *)((int)param_1 + 0x62) = (short)param_3;
  *(ushort *)((int)param_1 + 0x6a) = uVar1;
  puVar3 = *param_1;
  param_1[0x17] = param_2;
  *(short *)(param_1 + 0x18) = (short)param_3;
  param_1[0x1c] = puVar4;
  param_1[0x1d] = puVar4;
  if (param_1[2] == (uint *)0x60000) {
    *puVar3 = *puVar3 | 0x800;
  }
  else {
    puVar3[3] = puVar3[3] & 0xfff9ffff | 0x20000;
  }
  puVar4 = param_1[0x1e];
  if (&DataAbort <= param_1[3]) {
    if (puVar4[6] == 0x4000) goto LAB_080157aa;
    goto LAB_08015822;
  }
  uVar5 = puVar4[6];
  if (param_1[3] < (uint *)0x8) {
    if (uVar5 != 0x2000) {
      if (uVar5 == 0x4000) {
        *(short *)((int)param_1 + 0x62) = (short)(*(ushort *)((int)param_1 + 0x62) + 3 >> 2);
      }
      goto LAB_080157aa;
    }
  }
  else if (uVar5 != 0x4000) {
    if (uVar5 != 0x2000) goto LAB_08015822;
    goto LAB_080157aa;
  }
  *(short *)((int)param_1 + 0x62) = (short)(*(ushort *)((int)param_1 + 0x62) + 1 >> 1);
LAB_080157aa:
  uVar5 = puVar3[2];
  puVar4[0x10] = DAT_080158a4;
  puVar4[0xf] = DAT_080158a8;
  puVar4[0x13] = DAT_080158ac;
  puVar4[0x14] = 0;
  puVar3[2] = uVar5 & 0xffff7fff;
  iVar2 = FUN_0800b3a0();
  if (iVar2 != 0) {
    *(undefined *)(param_1 + 0x20) = 0;
    param_1[0x21] = (uint *)((uint)param_1[0x21] | 0x10);
    *(undefined *)((int)param_1 + 0x81) = 1;
    return 1;
  }
  puVar4 = *param_1;
  uVar5 = DAT_080158b0 & puVar4[1];
  if (param_1[0x1e][7] != 0x100) {
    uVar5 = uVar5 | param_3;
  }
  puVar4[1] = uVar5;
  puVar3 = param_1[1];
  puVar4[2] = puVar4[2] | 0x8000;
  puVar4[4] = puVar4[4] | 800;
  *puVar4 = *puVar4 | 1;
  if (puVar3 == (uint *)0x400000) {
    *puVar4 = *puVar4 | 0x200;
  }
  *(undefined *)(param_1 + 0x20) = 0;
  return 0;
}


