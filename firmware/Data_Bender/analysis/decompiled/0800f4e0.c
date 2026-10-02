/* 0800f4e0 FUN_0800f4e0; analyst naming is provisional. */

undefined FUN_0800f4e0(int *param_1,int param_2,uint param_3)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  undefined uVar5;
  
  iVar1 = FUN_08009ce8();
  iVar4 = *param_1;
  uVar2 = *(undefined4 *)(iVar4 + 0x18);
  if (*(char *)(param_1 + 0x10) == '\x01') {
    return 2;
  }
  *(undefined *)(param_1 + 0x10) = 1;
  if (*(char *)((int)param_1 + 0x41) == '\x01') {
    param_1[0x11] = 0;
    uVar5 = 1;
    if (param_2 == 0) {
      param_1[0x11] = param_1[0x11] | 8;
    }
    else {
      *(undefined *)((int)param_1 + 0x41) = 0x22;
      param_1[0xe] = *(int *)(iVar4 + 0x10) + 1;
      iVar3 = *(int *)(iVar4 + 0x10);
      param_1[0xc] = param_2;
      param_1[0xd] = iVar3 + 1;
      *(uint *)(iVar4 + 0x14) = *(uint *)(iVar4 + 0x14) & 0xf3ffffff | 0x4000000;
      *(undefined4 *)(iVar4 + 0x18) = uVar2;
      iVar3 = iVar4;
      if (param_1[0xe] == 0) goto LAB_0800f580;
LAB_0800f558:
      if ((*(uint *)(iVar3 + 8) & 6) == 0) goto LAB_0800f554;
      *(undefined *)param_1[0xc] = *(undefined *)(iVar4 + 0x20);
      param_1[0xe] = param_1[0xe] + -1;
      param_1[0xc] = param_1[0xc] + 1;
      if (param_1[0xe] != 0) goto LAB_0800f578;
      do {
        iVar4 = *param_1;
LAB_0800f580:
        do {
          if (*(int *)(iVar4 + 8) << 0x1e < 0) {
            uVar5 = 0;
            *(undefined4 *)(iVar4 + 0xc) = 2;
            goto LAB_0800f58e;
          }
        } while (param_3 == 0xffffffff);
        iVar4 = FUN_08009ce8();
      } while (((uint)(iVar4 - iVar1) <= param_3) && (param_3 != 0));
LAB_0800f5b4:
      *(undefined *)((int)param_1 + 0x41) = 4;
      param_1[0x11] = param_1[0x11] | 1;
LAB_0800f58e:
      *(undefined *)((int)param_1 + 0x41) = 1;
    }
  }
  else {
    uVar5 = 2;
  }
  *(undefined *)(param_1 + 0x10) = 0;
  return uVar5;
LAB_0800f554:
  if (param_3 != 0xffffffff) {
    iVar3 = FUN_08009ce8();
    if ((param_3 < (uint)(iVar3 - iVar1)) || (param_3 == 0)) goto LAB_0800f5b4;
LAB_0800f578:
    iVar3 = *param_1;
  }
  goto LAB_0800f558;
}


