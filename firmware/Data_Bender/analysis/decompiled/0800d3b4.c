/* 0800d3b4 FUN_0800d3b4; analyst naming is provisional. */

undefined4 FUN_0800d3b4(uint **param_1,uint param_2,uint *param_3,undefined2 param_4)

{
  short sVar1;
  ushort uVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  uint *puVar7;
  uint *puVar8;
  
  if (((*(char *)((int)param_1 + 0x41) != ' ') || (puVar7 = *param_1, (puVar7[6] & 0x8000) != 0)) ||
     (*(char *)(param_1 + 0x10) == '\x01')) {
    return 2;
  }
  param_1[9] = param_3;
  *(undefined *)((int)param_1 + 0x41) = 0x21;
  *(undefined *)((int)param_1 + 0x42) = 0x10;
  param_1[0x11] = (uint *)0x0;
  *(undefined2 *)((int)param_1 + 0x2a) = param_4;
  puVar8 = DAT_0800d518;
  *(undefined *)(param_1 + 0x10) = 1;
  param_1[0xb] = puVar8;
  param_1[0xd] = DAT_0800d51c;
  if (*(ushort *)((int)param_1 + 0x2a) < 0x100) {
    sVar1 = *(short *)((int)param_1 + 0x2a);
    *(short *)(param_1 + 10) = sVar1;
    uVar5 = 0;
    if (sVar1 != 0) {
      puVar7[10] = (uint)*(byte *)param_3;
      param_1[9] = (uint *)((int)param_3 + 1);
      *(short *)(param_1 + 10) = sVar1 + -1;
      *(short *)((int)param_1 + 0x2a) = *(short *)((int)param_1 + 0x2a) + -1;
      if (sVar1 != 1) {
        uVar5 = 0x2000000;
        goto LAB_0800d456;
      }
      uVar5 = 0x10000;
    }
    uVar3 = DAT_0800d520;
    uVar6 = puVar7[1];
    param_1[0xd] = DAT_0800d524;
    puVar7[1] = DAT_0800d528 | uVar5 | param_2 & 0x3ff | uVar6 & uVar3;
    *(undefined *)(param_1 + 0x10) = 0;
    *puVar7 = *puVar7 | 0xf2;
    return 0;
  }
  uVar5 = 0x1000000;
  *(undefined2 *)(param_1 + 10) = 0xff;
  puVar7[10] = (uint)*(byte *)param_3;
  param_1[9] = (uint *)((int)param_3 + 1);
  *(short *)((int)param_1 + 0x2a) = *(short *)((int)param_1 + 0x2a) + -1;
  *(undefined2 *)(param_1 + 10) = 0xfe;
LAB_0800d456:
  puVar8 = param_1[0xe];
  if (puVar8 == (uint *)0x0) {
    *(undefined *)(param_1 + 0x10) = 0;
    *(undefined *)((int)param_1 + 0x41) = 0x20;
    *(undefined *)((int)param_1 + 0x42) = 0;
    param_1[0x11] = (uint *)((uint)param_1[0x11] | 0x80);
    return 1;
  }
  puVar8[0xf] = DAT_0800d52c;
  uVar3 = DAT_0800d530;
  puVar8[0x10] = 0;
  puVar8[0x13] = uVar3;
  puVar8[0x14] = 0;
  iVar4 = FUN_0800b3a0(puVar8,(byte *)((int)param_3 + 1),puVar7 + 10);
  if (iVar4 != 0) {
    *(undefined *)(param_1 + 0x10) = 0;
    *(undefined *)((int)param_1 + 0x41) = 0x20;
    *(undefined *)((int)param_1 + 0x42) = 0;
    param_1[0x11] = (uint *)((uint)param_1[0x11] | 0x10);
    return 1;
  }
  puVar7 = *param_1;
  uVar2 = *(ushort *)(param_1 + 10);
  puVar7[1] = param_2 & 0x3ff | uVar5 | puVar7[1] & DAT_0800d520 | (uVar2 + 1 & 0xff) << 0x10 |
              0x2000;
  *(undefined *)(param_1 + 0x10) = 0;
  *(ushort *)((int)param_1 + 0x2a) = *(short *)((int)param_1 + 0x2a) - uVar2;
  *puVar7 = *puVar7 | 0x90;
  *puVar7 = *puVar7 | 0x4000;
  return 0;
}


