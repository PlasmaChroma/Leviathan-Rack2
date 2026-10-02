/* 08012088 FUN_08012088; analyst naming is provisional. */

uint FUN_08012088(uint **param_1,uint *param_2,int param_3,undefined4 param_4)

{
  uint uVar1;
  uint *puVar2;
  int iVar3;
  uint *puVar4;
  uint uVar5;
  uint *puVar6;
  
  if (param_2 != (uint *)0x0) {
    puVar6 = (uint *)(uint)(param_3 == 0);
    if (param_3 != 0) {
      uVar5 = (uint)*(byte *)((int)param_1 + 0x91);
      if ((*(byte *)((int)param_1 + 0x91) == 1) && (*(char *)(param_1 + 0x24) != '\x01')) {
        *(undefined *)(param_1 + 0x24) = 1;
        param_1[0x1e] = param_2;
        *(short *)(param_1 + 0x1f) = (short)param_3;
        *(short *)((int)param_1 + 0x7e) = (short)param_3;
        param_1[0x25] = puVar6;
        puVar2 = param_1[0x21];
        *(undefined *)((int)param_1 + 0x91) = 0x22;
        puVar4 = *param_1;
        puVar2[0x10] = DAT_08012144;
        uVar1 = DAT_08012148;
        puVar2[0x14] = (uint)puVar6;
        puVar2[0xf] = uVar1;
        puVar2[0x13] = DAT_0801214c;
        iVar3 = FUN_0800b3a0(puVar2,puVar4 + 7,param_2,param_3,param_4);
        if (iVar3 != 0) {
          *(bool *)(param_1 + 0x24) = param_3 == 0;
          return uVar5;
        }
        if (param_1[0x11] == (uint *)&SupervisorCall) {
          if (((uint)param_1[1] & 0xfffffffd) == 1) {
            uVar5 = 0x11;
          }
          else {
            uVar5 = 1;
          }
        }
        puVar6 = *param_1;
        if ((int)param_1[1] - 2U < 2) {
          uVar5 = uVar5 | 0x60;
        }
        else {
          uVar5 = uVar5 | 4;
        }
        puVar6[4] = puVar6[4] | uVar5;
        *puVar6 = *puVar6 | 0x20000;
        if (-1 < (int)(*puVar6 << 0xf)) {
          *puVar6 = *puVar6 | 0x10000;
        }
        *(undefined *)(param_1 + 0x24) = 0;
        return 0;
      }
      return 2;
    }
  }
  return 1;
}


