/* 0800cf80 FUN_0800cf80; analyst naming is provisional. */

undefined4 FUN_0800cf80(uint **param_1)

{
  uint *puVar1;
  uint *puVar2;
  uint *puVar3;
  uint *puVar4;
  
  if (param_1 != (uint **)0x0) {
    if (*(char *)((int)param_1 + 0x41) == '\0') {
      *(undefined *)(param_1 + 0x10) = 0;
      FUN_08007dfc();
    }
    puVar4 = *param_1;
    puVar2 = param_1[1];
    *(undefined *)((int)param_1 + 0x41) = 0x24;
    puVar1 = param_1[3];
    *puVar4 = *puVar4 & 0xfffffffe;
    puVar4[4] = (uint)puVar2 & 0xf0ffffff;
    puVar4[2] = puVar4[2] & 0xffff7fff;
    if (puVar1 == (uint *)0x1) {
      puVar4[2] = (uint)param_1[2] | 0x8000;
    }
    else {
      puVar4[2] = (uint)param_1[2] | 0x8400;
      if (puVar1 == (uint *)0x2) {
        puVar4[1] = 0x800;
      }
    }
    puVar3 = param_1[4];
    puVar4[1] = DAT_0800d028 | puVar4[1];
    puVar4[3] = puVar4[3] & 0xffff7fff;
    puVar2 = param_1[7];
    puVar1 = param_1[8];
    puVar4[3] = (uint)puVar3 | (uint)param_1[5] | (int)param_1[6] << 8;
    *puVar4 = (uint)puVar2 | (uint)puVar1;
    *puVar4 = *puVar4 | 1;
    param_1[0x11] = (uint *)0x0;
    *(undefined *)((int)param_1 + 0x41) = 0x20;
    param_1[0xc] = (uint *)0x0;
    *(undefined *)((int)param_1 + 0x42) = 0;
    return 0;
  }
  return 1;
}


