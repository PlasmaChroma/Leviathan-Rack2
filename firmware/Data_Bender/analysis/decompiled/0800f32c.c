/* 0800f32c FUN_0800f32c; analyst naming is provisional. */

undefined FUN_0800f32c(int *param_1,int param_2,uint param_3)

{
  int iVar1;
  int iVar2;
  undefined uVar3;
  
  iVar1 = FUN_08009ce8();
  if (*(char *)(param_1 + 0x10) == '\x01') {
    return 2;
  }
  *(undefined *)(param_1 + 0x10) = 1;
  if (*(char *)((int)param_1 + 0x41) != '\x01') {
    uVar3 = 2;
    goto LAB_0800f358;
  }
  param_1[0x11] = 0;
  *(undefined *)((int)param_1 + 0x41) = 2;
  do {
    do {
      if ((*(uint *)(*param_1 + 8) & 0x20) == 0) {
        FUN_0800f0dc(param_1,param_2,0);
        if (*(int *)(param_2 + 0x24) == 0) goto LAB_0800f3d0;
        *(undefined *)((int)param_1 + 0x41) = 1;
        uVar3 = 0;
        goto LAB_0800f358;
      }
    } while (param_3 == 0xffffffff);
    iVar2 = FUN_08009ce8();
  } while (((uint)(iVar2 - iVar1) <= param_3) && (param_3 != 0));
  goto LAB_0800f3a8;
LAB_0800f3d0:
  do {
    do {
      if (*(int *)(*param_1 + 8) << 0x1e < 0) {
        uVar3 = 0;
        *(undefined4 *)(*param_1 + 0xc) = 2;
        *(undefined *)((int)param_1 + 0x41) = 1;
        goto LAB_0800f358;
      }
    } while (param_3 == 0xffffffff);
    iVar2 = FUN_08009ce8();
  } while (((uint)(iVar2 - iVar1) <= param_3) && (param_3 != 0));
LAB_0800f3a8:
  *(undefined *)((int)param_1 + 0x41) = 4;
  param_1[0x11] = param_1[0x11] | 1;
  uVar3 = 1;
LAB_0800f358:
  *(undefined *)(param_1 + 0x10) = 0;
  return uVar3;
}


