/* 08011f64 FUN_08011f64; analyst naming is provisional. */

uint FUN_08011f64(uint **param_1,uint *param_2,int param_3,undefined4 param_4)

{
  uint uVar1;
  int iVar2;
  uint *puVar3;
  int iVar4;
  uint *puVar5;
  uint uVar6;
  uint *puVar7;
  
  iVar2 = FUN_08009ce8();
  if (param_2 != (uint *)0x0) {
    puVar7 = (uint *)(uint)(param_3 == 0);
    if (param_3 != 0) {
      uVar6 = (uint)*(byte *)((int)param_1 + 0x91);
      if ((*(byte *)((int)param_1 + 0x91) == 1) && (*(char *)(param_1 + 0x24) != '\x01')) {
        *(short *)(param_1 + 0x1f) = (short)param_3;
        *(short *)((int)param_1 + 0x7e) = (short)param_3;
        param_1[0x25] = puVar7;
        *(undefined *)((int)param_1 + 0x91) = 0x12;
        uVar1 = DAT_0801207c;
        puVar3 = param_1[0x20];
        puVar5 = *param_1;
        *(undefined *)(param_1 + 0x24) = 1;
        param_1[0x1e] = param_2;
        puVar3[0x10] = uVar1;
        uVar1 = DAT_08012080;
        puVar3[0x14] = (uint)puVar7;
        puVar3[0xf] = uVar1;
        puVar3[0x13] = DAT_08012084;
        iVar4 = FUN_0800b3a0(puVar3,param_2,puVar5 + 7,param_3,param_4);
        if (iVar4 != 0) {
          *(bool *)(param_1 + 0x24) = param_3 == 0;
          return uVar6;
        }
        if (param_1[0x11] == (uint *)&SupervisorCall) {
          if (((uint)param_1[1] & 0xfffffffd) == 1) {
            uVar6 = 0x11;
          }
          else {
            uVar6 = 1;
          }
        }
        puVar7 = *param_1;
        if ((int)param_1[1] - 2U < 2) {
          uVar6 = uVar6 | 0x60;
        }
        else {
          uVar6 = uVar6 | 4;
        }
        puVar7[4] = puVar7[4] | uVar6;
        *puVar7 = *puVar7 | 0x20000;
        while( true ) {
          if ((puVar7[5] & 0x70000) != 0) {
            if (-1 < (int)(*puVar7 << 0xf)) {
              *puVar7 = *puVar7 | 0x10000;
            }
            *(undefined *)(param_1 + 0x24) = 0;
            return 0;
          }
          iVar4 = FUN_08009ce8();
          if (1000 < (uint)(iVar4 - iVar2)) break;
          puVar7 = *param_1;
        }
        *(undefined *)(param_1 + 0x24) = 0;
        param_1[0x25] = (uint *)((uint)param_1[0x25] | 0x40);
        return 3;
      }
      return 2;
    }
  }
  return 1;
}


