/* 0800f6b4 FUN_0800f6b4; analyst naming is provisional. */

undefined FUN_0800f6b4(uint **param_1,undefined4 param_2,uint *param_3)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  uint *puVar4;
  uint *puVar5;
  undefined uVar6;
  
  iVar1 = FUN_08009ce8();
  if (*(char *)(param_1 + 0x10) == '\x01') {
    return 2;
  }
  *(undefined *)(param_1 + 0x10) = 1;
  if (*(char *)((int)param_1 + 0x41) == '\x01') {
    puVar5 = param_1[0x12];
    param_1[0x11] = (uint *)0x0;
    *(undefined *)((int)param_1 + 0x41) = 0x82;
    do {
      puVar4 = *param_1;
      do {
        if (-1 < (int)(puVar4[2] << 0x1a)) {
          uVar3 = param_3[1];
          *puVar4 = *puVar4 & 0xfffffff7 | uVar3;
          if (uVar3 == 8) {
            puVar4[0xc] = *param_3;
            puVar4[3] = 0x10;
            *puVar4 = *puVar4 | 0x100000;
          }
          uVar6 = 0;
          FUN_0800f0dc(param_1,param_2,0xc000000);
          goto LAB_0800f6e0;
        }
      } while (puVar5 == (uint *)0xffffffff);
      iVar2 = FUN_08009ce8();
    } while (((uint *)(iVar2 - iVar1) <= puVar5) && (puVar5 != (uint *)0x0));
    *(undefined *)((int)param_1 + 0x41) = 4;
    param_1[0x11] = (uint *)((uint)param_1[0x11] | 1);
    uVar6 = 1;
  }
  else {
    uVar6 = 2;
  }
LAB_0800f6e0:
  *(undefined *)(param_1 + 0x10) = 0;
  return uVar6;
}


