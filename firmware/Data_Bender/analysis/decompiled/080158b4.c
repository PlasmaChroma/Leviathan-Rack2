/* 080158b4 FUN_080158b4; analyst naming is provisional. */

undefined FUN_080158b4(uint **param_1,uint *param_2,uint param_3,undefined4 param_4)

{
  undefined2 uVar1;
  ushort uVar2;
  uint *puVar3;
  int iVar4;
  uint uVar5;
  uint *puVar6;
  
  if (*(char *)(param_1 + 0x20) == '\x01') {
    return 2;
  }
  *(undefined *)(param_1 + 0x20) = 1;
  if (*(char *)((int)param_1 + 0x81) != '\x01') {
    *(undefined *)(param_1 + 0x20) = 0;
    return 2;
  }
  if (param_2 != (uint *)0x0) {
    puVar6 = (uint *)(uint)(param_3 == 0);
    if (param_3 != 0) {
      uVar2 = (ushort)(param_3 == 0);
      *(ushort *)(param_1 + 0x18) = uVar2;
      param_1[0x19] = param_2;
      *(undefined *)((int)param_1 + 0x81) = 4;
      param_1[0x21] = puVar6;
      *(short *)((int)param_1 + 0x6a) = (short)param_3;
      *(ushort *)((int)param_1 + 0x62) = uVar2;
      *(short *)(param_1 + 0x1a) = (short)param_3;
      param_1[0x1c] = puVar6;
      param_1[0x1d] = puVar6;
      puVar6 = *param_1;
      if (param_1[2] == (uint *)0x60000) {
        *puVar6 = *puVar6 & 0xfffff7ff;
      }
      else {
        puVar6[3] = puVar6[3] & 0xfff9ffff | 0x40000;
      }
      if (param_1[3] < &DataAbort) {
        if (param_1[3] < (uint *)0x8) {
          puVar3 = param_1[0x1f];
          puVar6[2] = puVar6[2] & 0xffffbfff;
          if (puVar3[6] != 0x2000) {
            if (puVar3[6] == 0x4000) {
              *(short *)((int)param_1 + 0x6a) = (short)(*(ushort *)((int)param_1 + 0x6a) + 3 >> 2);
            }
            goto LAB_08015930;
          }
        }
        else {
          puVar3 = param_1[0x1f];
          uVar5 = puVar3[6];
          if ((uVar5 != 0x4000) && (uVar5 != 0x2000)) goto LAB_080159a0;
          puVar6[2] = puVar6[2] & 0xffffbfff;
          if (uVar5 != 0x4000) goto LAB_08015930;
        }
        *(short *)((int)param_1 + 0x6a) = (short)(*(ushort *)((int)param_1 + 0x6a) + 1 >> 1);
      }
      else {
        puVar3 = param_1[0x1f];
        if (puVar3[6] != 0x4000) goto LAB_080159a0;
        puVar6[2] = puVar6[2] & 0xffffbfff;
      }
LAB_08015930:
      uVar1 = *(undefined2 *)((int)param_1 + 0x6a);
      puVar3[0x10] = DAT_08015a38;
      puVar3[0xf] = DAT_08015a3c;
      puVar3[0x13] = DAT_08015a40;
      puVar3[0x14] = 0;
      iVar4 = FUN_0800b3a0(puVar3,puVar6 + 0xc,param_2,uVar1,param_4);
      if (iVar4 != 0) {
        *(undefined *)(param_1 + 0x20) = 0;
        param_1[0x21] = (uint *)((uint)param_1[0x21] | 0x10);
        *(undefined *)((int)param_1 + 0x81) = 1;
        return 1;
      }
      puVar6 = *param_1;
      uVar5 = DAT_08015a44 & puVar6[1];
      if (param_1[0x1f][7] != 0x100) {
        uVar5 = uVar5 | param_3;
      }
      puVar6[1] = uVar5;
      puVar3 = param_1[1];
      puVar6[2] = puVar6[2] | 0x4000;
      puVar6[4] = puVar6[4] | 0x340;
      *puVar6 = *puVar6 | 1;
      if (puVar3 == (uint *)0x400000) {
        *puVar6 = *puVar6 | 0x200;
      }
      *(undefined *)(param_1 + 0x20) = 0;
      return 0;
    }
  }
LAB_080159a0:
  *(undefined *)(param_1 + 0x20) = 0;
  return 1;
}


