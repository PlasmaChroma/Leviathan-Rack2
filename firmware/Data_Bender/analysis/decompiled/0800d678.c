/* 0800d678 FUN_0800d678; analyst naming is provisional. */

undefined4 FUN_0800d678(uint **param_1,uint *param_2,int param_3,undefined4 param_4)

{
  short sVar1;
  int iVar2;
  uint *puVar3;
  uint *puVar4;
  
  if (*(char *)((int)param_1 + 0x41) == ' ') {
    if ((param_2 == (uint *)0x0) || (param_3 == 0)) {
      param_1[0x11] = (uint *)0x200;
      return 1;
    }
    if (*(char *)(param_1 + 0x10) != '\x01') {
      param_1[9] = param_2;
      *(undefined *)((int)param_1 + 0x41) = 0x21;
      *(undefined *)((int)param_1 + 0x42) = 0x20;
      param_1[0x11] = (uint *)(uint)(param_3 == 0);
      *(short *)((int)param_1 + 0x2a) = (short)param_3;
      puVar4 = DAT_0800d78c;
      sVar1 = *(short *)((int)param_1 + 0x2a);
      param_1[0xd] = DAT_0800d788;
      param_1[0xb] = puVar4;
      *(short *)(param_1 + 10) = sVar1;
      *(undefined *)(param_1 + 0x10) = 1;
      if (param_1[8] == (uint *)0x20000) {
        (*param_1)[10] = (uint)*(byte *)param_2;
        *(short *)(param_1 + 10) = sVar1 + -1;
        param_1[9] = (uint *)((int)param_2 + 1);
        *(short *)((int)param_1 + 0x2a) = *(short *)((int)param_1 + 0x2a) + -1;
      }
      if (*(short *)((int)param_1 + 0x2a) == 0) {
        puVar4 = *param_1;
        puVar4[1] = puVar4[1] & 0xffff7fff;
        *(undefined *)(param_1 + 0x10) = 0;
        *puVar4 = *puVar4 | 0xb8;
        return 0;
      }
      puVar4 = param_1[0xe];
      if (puVar4 == (uint *)0x0) {
        *(undefined *)(param_1 + 0x10) = 0;
        *(undefined *)((int)param_1 + 0x41) = 0x28;
        *(undefined *)((int)param_1 + 0x42) = 0;
        param_1[0x11] = (uint *)((uint)param_1[0x11] | 0x80);
        return 1;
      }
      puVar3 = *param_1;
      puVar4[0xf] = DAT_0800d790;
      puVar4[0x10] = 0;
      puVar4[0x13] = DAT_0800d794;
      puVar4[0x14] = 0;
      iVar2 = FUN_0800b3a0(puVar4,param_1[9],puVar3 + 10,*(undefined2 *)(param_1 + 10),param_4);
      if (iVar2 == 0) {
        puVar4 = *param_1;
        puVar4[1] = puVar4[1] & 0xffff7fff;
        *(undefined *)(param_1 + 0x10) = 0;
        *puVar4 = *puVar4 | 0xb8;
        *puVar4 = *puVar4 | 0x4000;
        return 0;
      }
      *(undefined *)(param_1 + 0x10) = 0;
      *(undefined *)((int)param_1 + 0x41) = 0x28;
      *(undefined *)((int)param_1 + 0x42) = 0;
      param_1[0x11] = (uint *)((uint)param_1[0x11] | 0x10);
      return 1;
    }
  }
  return 2;
}


