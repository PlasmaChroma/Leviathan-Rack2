/* 0800d534 FUN_0800d534; analyst naming is provisional. */

undefined4 FUN_0800d534(uint **param_1,uint param_2,uint *param_3,undefined2 param_4)

{
  short sVar1;
  uint uVar2;
  int iVar3;
  uint *puVar4;
  uint *puVar5;
  uint uVar6;
  
  if (((*(char *)((int)param_1 + 0x41) != ' ') || (puVar4 = *param_1, (puVar4[6] & 0x8000) != 0)) ||
     (*(char *)(param_1 + 0x10) == '\x01')) {
    return 2;
  }
  param_1[9] = param_3;
  *(undefined *)((int)param_1 + 0x41) = 0x22;
  *(undefined *)((int)param_1 + 0x42) = 0x10;
  param_1[0x11] = (uint *)0x0;
  *(undefined2 *)((int)param_1 + 0x2a) = param_4;
  puVar5 = DAT_0800d65c;
  *(undefined *)(param_1 + 0x10) = 1;
  param_1[0xb] = puVar5;
  param_1[0xd] = DAT_0800d660;
  if (*(ushort *)((int)param_1 + 0x2a) < 0x100) {
    *(short *)(param_1 + 10) = *(short *)((int)param_1 + 0x2a);
    uVar6 = DAT_0800d668;
    if (*(short *)((int)param_1 + 0x2a) == 0) {
      param_1[0xd] = DAT_0800d664;
      puVar4[1] = DAT_0800d66c | param_2 & 0x3ff | puVar4[1] & uVar6;
      *(undefined *)(param_1 + 0x10) = 0;
      *puVar4 = *puVar4 | 0xf4;
      return 0;
    }
    uVar6 = 0x2000000;
  }
  else {
    uVar6 = 0x1000000;
    *(undefined2 *)(param_1 + 10) = 0xff;
  }
  puVar5 = param_1[0xf];
  if (puVar5 == (uint *)0x0) {
    *(undefined *)(param_1 + 0x10) = 0;
    *(undefined *)((int)param_1 + 0x41) = 0x20;
    *(undefined *)((int)param_1 + 0x42) = 0;
    param_1[0x11] = (uint *)((uint)param_1[0x11] | 0x80);
    return 1;
  }
  puVar5[0xf] = DAT_0800d670;
  uVar2 = DAT_0800d674;
  puVar5[0x10] = 0;
  puVar5[0x13] = uVar2;
  puVar5[0x14] = 0;
  iVar3 = FUN_0800b3a0(puVar5,puVar4 + 9);
  if (iVar3 != 0) {
    *(undefined *)(param_1 + 0x10) = 0;
    *(undefined *)((int)param_1 + 0x41) = 0x20;
    *(undefined *)((int)param_1 + 0x42) = 0;
    param_1[0x11] = (uint *)((uint)param_1[0x11] | 0x10);
    return 1;
  }
  puVar4 = *param_1;
  sVar1 = *(short *)(param_1 + 10);
  puVar4[1] = param_2 & 0x3ff | uVar6 | puVar4[1] & DAT_0800d668 | (uint)(byte)sVar1 << 0x10 |
              0x2400;
  *(undefined *)(param_1 + 0x10) = 0;
  *(short *)((int)param_1 + 0x2a) = *(short *)((int)param_1 + 0x2a) - sVar1;
  *puVar4 = *puVar4 | 0x90;
  *puVar4 = *puVar4 | 0x8000;
  return 0;
}


