/* 0800f3f0 FUN_0800f3f0; analyst naming is provisional. */

undefined FUN_0800f3f0(int *param_1,int param_2,uint param_3)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined uVar4;
  
  iVar1 = FUN_08009ce8();
  iVar3 = *param_1;
  if (*(char *)(param_1 + 0x10) == '\x01') {
    return 2;
  }
  *(undefined *)(param_1 + 0x10) = 1;
  if (*(char *)((int)param_1 + 0x41) == '\x01') {
    param_1[0x11] = 0;
    uVar4 = 1;
    if (param_2 == 0) {
      param_1[0x11] = param_1[0x11] | 8;
    }
    else {
      *(undefined *)((int)param_1 + 0x41) = 0x12;
      param_1[0xb] = *(int *)(iVar3 + 0x10) + 1;
      iVar2 = *(int *)(iVar3 + 0x10);
      param_1[9] = param_2;
      param_1[10] = iVar2 + 1;
      *(uint *)(iVar3 + 0x14) = *(uint *)(iVar3 + 0x14) & 0xf3ffffff;
      iVar2 = iVar3;
      if (param_1[0xb] == 0) goto LAB_0800f486;
LAB_0800f460:
      if (-1 < *(int *)(iVar2 + 8) << 0x1d) goto LAB_0800f45c;
      *(undefined *)(iVar3 + 0x20) = *(undefined *)param_1[9];
      param_1[0xb] = param_1[0xb] + -1;
      param_1[9] = param_1[9] + 1;
      if (param_1[0xb] != 0) goto LAB_0800f47e;
      do {
        iVar3 = *param_1;
LAB_0800f486:
        do {
          if (*(int *)(iVar3 + 8) << 0x1e < 0) {
            uVar4 = 0;
            *(undefined4 *)(iVar3 + 0xc) = 2;
            goto LAB_0800f494;
          }
        } while (param_3 == 0xffffffff);
        iVar3 = FUN_08009ce8();
      } while (((uint)(iVar3 - iVar1) <= param_3) && (param_3 != 0));
LAB_0800f4ba:
      *(undefined *)((int)param_1 + 0x41) = 4;
      param_1[0x11] = param_1[0x11] | 1;
LAB_0800f494:
      *(undefined *)((int)param_1 + 0x41) = 1;
    }
  }
  else {
    uVar4 = 2;
  }
  *(undefined *)(param_1 + 0x10) = 0;
  return uVar4;
LAB_0800f45c:
  if (param_3 != 0xffffffff) {
    iVar2 = FUN_08009ce8();
    if ((param_3 < (uint)(iVar2 - iVar1)) || (param_3 == 0)) goto LAB_0800f4ba;
LAB_0800f47e:
    iVar2 = *param_1;
  }
  goto LAB_0800f460;
}


