/* 08015a48 FUN_08015a48; analyst naming is provisional. */

undefined FUN_08015a48(uint **param_1,uint *param_2,uint *param_3,uint param_4)

{
  undefined2 uVar1;
  uint uVar2;
  int iVar3;
  uint *puVar4;
  uint *puVar5;
  uint uVar6;
  uint *puVar7;
  undefined uVar8;
  
  if (*(char *)(param_1 + 0x20) == '\x01') {
    return 2;
  }
  *(undefined *)(param_1 + 0x20) = 1;
  if (*(char *)((int)param_1 + 0x81) != '\x01') {
    *(undefined *)(param_1 + 0x20) = 0;
    return 2;
  }
  uVar8 = 1;
  if ((param_4 == 0 || param_3 == (uint *)0x0) ||
     (puVar7 = (uint *)(uint)(param_2 == (uint *)0x0), param_2 == (uint *)0x0)) {
LAB_08015b02:
    *(undefined *)(param_1 + 0x20) = 0;
  }
  else {
    param_1[0x17] = param_2;
    uVar1 = (undefined2)param_4;
    *(undefined2 *)(param_1 + 0x18) = uVar1;
    puVar4 = *param_1;
    param_1[0x19] = param_3;
    *(undefined2 *)(param_1 + 0x1a) = uVar1;
    param_1[0x1c] = puVar7;
    param_1[0x1d] = puVar7;
    *(undefined *)((int)param_1 + 0x81) = 5;
    param_1[0x21] = puVar7;
    *(undefined2 *)((int)param_1 + 0x62) = uVar1;
    *(undefined2 *)((int)param_1 + 0x6a) = uVar1;
    puVar4[3] = puVar4[3] & 0xfff9ffff;
    puVar4[2] = puVar4[2] & 0xffff3fff;
    if (param_1[3] < &DataAbort) {
      if (param_1[3] < (uint *)0x8) {
        if (param_1[0x1e][6] == 0x2000) {
          *(short *)((int)param_1 + 0x62) = (short)(*(ushort *)((int)param_1 + 0x62) + 1 >> 1);
        }
        else if (param_1[0x1e][6] == 0x4000) {
          *(short *)((int)param_1 + 0x62) = (short)(*(ushort *)((int)param_1 + 0x62) + 3 >> 2);
        }
        puVar7 = param_1[0x1f];
        if (puVar7[6] == 0x2000) {
LAB_08015c0c:
          *(short *)((int)param_1 + 0x6a) = (short)(*(ushort *)((int)param_1 + 0x6a) + 1 >> 1);
        }
        else if (puVar7[6] == 0x4000) {
          *(short *)((int)param_1 + 0x6a) = (short)(*(ushort *)((int)param_1 + 0x6a) + 3 >> 2);
        }
      }
      else {
        puVar7 = param_1[0x1f];
        uVar6 = puVar7[6];
        if ((uVar6 != 0x4000) && (uVar6 != 0x2000)) goto LAB_08015b02;
        if (param_1[0x1e][6] == 0x4000) {
          *(short *)((int)param_1 + 0x62) = (short)(*(ushort *)((int)param_1 + 0x62) + 1 >> 1);
        }
        if (uVar6 == 0x4000) goto LAB_08015c0c;
      }
    }
    else {
      puVar7 = param_1[0x1f];
      if (puVar7[6] != 0x4000) goto LAB_08015b02;
    }
    uVar1 = *(undefined2 *)((int)param_1 + 0x6a);
    puVar7[0x10] = DAT_08015c4c;
    uVar2 = DAT_08015c58;
    uVar6 = DAT_08015c50;
    puVar7[0x13] = DAT_08015c58;
    puVar7[0xf] = uVar6;
    puVar7[0x14] = 0;
    uVar6 = param_4;
    iVar3 = FUN_0800b3a0(puVar7,puVar4 + 0xc,param_3,uVar1,param_4);
    if (iVar3 == 0) {
      puVar5 = *param_1;
      puVar7 = param_1[0x1e];
      puVar4 = param_1[0x17];
      puVar5[2] = puVar5[2] | 0x4000;
      uVar1 = *(undefined2 *)((int)param_1 + 0x62);
      puVar7[0x13] = uVar2;
      puVar7[0x14] = 0;
      puVar7[0xf] = 0;
      puVar7[0x10] = 0;
      iVar3 = FUN_0800b3a0(puVar7,puVar4,puVar5 + 8,uVar1,uVar6);
      if (iVar3 == 0) {
        puVar7 = *param_1;
        uVar6 = DAT_08015c54 & puVar7[1];
        if (param_1[0x1e][7] != 0x100) {
          uVar6 = uVar6 | param_4;
        }
        puVar7[1] = uVar6;
        puVar4 = param_1[1];
        puVar7[2] = puVar7[2] | 0x8000;
        puVar7[4] = puVar7[4] | 0x360;
        *puVar7 = *puVar7 | 1;
        if (puVar4 == (uint *)0x400000) {
          *puVar7 = *puVar7 | 0x200;
        }
        uVar8 = 0;
        *(undefined *)(param_1 + 0x20) = 0;
      }
      else {
        FUN_0800b550(param_1[0x1f]);
        *(undefined *)(param_1 + 0x20) = 0;
        param_1[0x21] = (uint *)((uint)param_1[0x21] | 0x10);
        *(undefined *)((int)param_1 + 0x81) = 1;
      }
    }
    else {
      *(undefined *)(param_1 + 0x20) = 0;
      param_1[0x21] = (uint *)((uint)param_1[0x21] | 0x10);
      *(undefined *)((int)param_1 + 0x81) = 1;
    }
  }
  return uVar8;
}


