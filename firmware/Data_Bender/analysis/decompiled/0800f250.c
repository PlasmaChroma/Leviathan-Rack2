/* 0800f250 FUN_0800f250; analyst naming is provisional. */

undefined4 FUN_0800f250(uint **param_1)

{
  int iVar1;
  int iVar2;
  uint *puVar3;
  uint *puVar4;
  
  iVar1 = FUN_08009ce8();
  if (param_1 == (uint **)0x0) {
    return 1;
  }
  if (*(char *)((int)param_1 + 0x41) == '\0') {
    FUN_08008af4(param_1);
    puVar4 = (uint *)0x1388;
    param_1[0x12] = (uint *)0x1388;
  }
  else {
    puVar4 = param_1[0x12];
  }
  puVar3 = *param_1;
  *puVar3 = *puVar3 & 0xffffe0ff | ((int)param_1[2] + -1) * 0x100;
  while( true ) {
    do {
      if ((puVar3[2] & 0x20) == 0) {
        puVar4 = param_1[5];
        *puVar3 = (uint)param_1[3] | (uint)param_1[7] | (uint)param_1[8] | (int)param_1[1] << 0x18 |
                  DAT_0800f300 & *puVar3;
        puVar3[1] = (uint)param_1[6] | (uint)puVar4 | (int)param_1[4] << 0x10 |
                    DAT_0800f304 & puVar3[1];
        *puVar3 = *puVar3 | 1;
        param_1[0x11] = (uint *)0x0;
        *(undefined *)((int)param_1 + 0x41) = 1;
        return 0;
      }
    } while (puVar4 == (uint *)0xffffffff);
    iVar2 = FUN_08009ce8();
    if ((puVar4 < (uint *)(iVar2 - iVar1)) || (puVar4 == (uint *)0x0)) break;
    puVar3 = *param_1;
  }
  *(undefined *)((int)param_1 + 0x41) = 4;
  param_1[0x11] = (uint *)((uint)param_1[0x11] | 1);
  return 1;
}


