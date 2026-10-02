/* 0800f5d8 FUN_0800f5d8; analyst naming is provisional. */

undefined FUN_0800f5d8(uint **param_1,int param_2,uint *param_3,uint param_4)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  uint *puVar4;
  undefined uVar5;
  
  iVar1 = FUN_08009ce8();
  if (*(char *)(param_1 + 0x10) == '\x01') {
    return 2;
  }
  *(undefined *)(param_1 + 0x10) = 1;
  if (*(char *)((int)param_1 + 0x41) != '\x01') {
    uVar5 = 2;
    goto LAB_0800f606;
  }
  param_1[0x11] = (uint *)0x0;
  *(undefined *)((int)param_1 + 0x41) = 0x42;
  do {
    puVar4 = *param_1;
    do {
      if (-1 < (int)(puVar4[2] << 0x1a)) {
        puVar4[10] = *param_3;
        puVar4[9] = param_3[1];
        puVar4[0xb] = param_3[2];
        uVar3 = param_3[3];
        *puVar4 = param_3[4] | *puVar4 & 0xff3fffff | 0x400000;
        *(uint *)(param_2 + 0x28) = uVar3;
        FUN_0800f0dc(param_1,param_2,0x8000000);
        goto LAB_0800f65a;
      }
    } while (param_4 == 0xffffffff);
    iVar2 = FUN_08009ce8();
  } while (((uint)(iVar2 - iVar1) <= param_4) && (param_4 != 0));
  goto LAB_0800f6a4;
LAB_0800f65a:
  do {
    do {
      if ((int)((*param_1)[2] << 0x1c) < 0) {
        uVar5 = 0;
        (*param_1)[3] = 8;
        *(undefined *)((int)param_1 + 0x41) = 1;
        goto LAB_0800f606;
      }
    } while (param_4 == 0xffffffff);
    iVar2 = FUN_08009ce8();
  } while (((uint)(iVar2 - iVar1) <= param_4) && (param_4 != 0));
LAB_0800f6a4:
  *(undefined *)((int)param_1 + 0x41) = 4;
  param_1[0x11] = (uint *)((uint)param_1[0x11] | 1);
  uVar5 = 1;
LAB_0800f606:
  *(undefined *)(param_1 + 0x10) = 0;
  return uVar5;
}


