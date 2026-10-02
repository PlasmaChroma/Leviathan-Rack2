/* 0800d798 FUN_0800d798; analyst naming is provisional. */

undefined4 FUN_0800d798(uint **param_1,uint *param_2,int param_3,undefined4 param_4)

{
  undefined2 uVar1;
  bool bVar2;
  uint uVar3;
  int iVar4;
  uint *puVar5;
  uint *puVar6;
  uint *puVar7;
  
  if (*(char *)((int)param_1 + 0x41) != ' ') {
    return 2;
  }
  if (param_2 != (uint *)0x0) {
    bVar2 = param_3 == 0;
    puVar7 = (uint *)(uint)bVar2;
    if (param_3 != 0) {
      if (*(char *)(param_1 + 0x10) == '\x01') {
        return 2;
      }
      puVar6 = param_1[0xf];
      *(undefined *)((int)param_1 + 0x41) = 0x22;
      *(undefined *)((int)param_1 + 0x42) = 0x20;
      param_1[0x11] = puVar7;
      *(short *)((int)param_1 + 0x2a) = (short)param_3;
      uVar1 = *(undefined2 *)((int)param_1 + 0x2a);
      param_1[0xb] = DAT_0800d864;
      puVar5 = DAT_0800d868;
      param_1[9] = param_2;
      *(undefined2 *)(param_1 + 10) = uVar1;
      param_1[0xd] = puVar5;
      *(undefined *)(param_1 + 0x10) = 1;
      if (puVar6 != (uint *)0x0) {
        puVar5 = *param_1;
        puVar6[0xf] = DAT_0800d86c;
        uVar3 = DAT_0800d870;
        puVar6[0x10] = (uint)puVar7;
        puVar6[0x13] = uVar3;
        puVar6[0x14] = (uint)puVar7;
        iVar4 = FUN_0800b3a0(puVar6,puVar5 + 9,param_2,uVar1,param_4);
        if (iVar4 != 0) {
          *(bool *)(param_1 + 0x10) = bVar2;
          *(undefined *)((int)param_1 + 0x41) = 0x28;
          *(bool *)((int)param_1 + 0x42) = bVar2;
          param_1[0x11] = (uint *)((uint)param_1[0x11] | 0x10);
          return 1;
        }
        puVar7 = *param_1;
        puVar7[1] = puVar7[1] & 0xffff7fff;
        *(undefined *)(param_1 + 0x10) = 0;
        *puVar7 = *puVar7 | 0xb8;
        *puVar7 = *puVar7 | 0x8000;
        return 0;
      }
      *(undefined *)(param_1 + 0x10) = 0;
      *(undefined *)((int)param_1 + 0x41) = 0x28;
      *(undefined *)((int)param_1 + 0x42) = 0;
      param_1[0x11] = (uint *)((uint)param_1[0x11] | 0x80);
      return 1;
    }
  }
  param_1[0x11] = (uint *)0x200;
  return 1;
}


