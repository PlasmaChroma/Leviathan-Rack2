/* 0800d02c FUN_0800d02c; analyst naming is provisional. */

undefined4
FUN_0800d02c(int *param_1,uint param_2,byte *param_3,undefined2 param_4,undefined4 param_5)

{
  ushort uVar1;
  short sVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  byte *pbVar6;
  uint uVar7;
  int iVar8;
  uint uVar9;
  
  if ((*(char *)((int)param_1 + 0x41) != ' ') || (*(char *)(param_1 + 0x10) == '\x01')) {
    return 2;
  }
  *(undefined *)(param_1 + 0x10) = 1;
  iVar3 = FUN_08009ce8();
  iVar8 = *param_1;
  while (*(int *)(iVar8 + 0x18) << 0x10 < 0) {
    iVar4 = FUN_08009ce8();
    iVar8 = *param_1;
    if ((0x19 < (uint)(iVar4 - iVar3)) && (*(int *)(iVar8 + 0x18) << 0x10 < 0)) {
      *(undefined *)(param_1 + 0x10) = 0;
      param_1[0x11] = param_1[0x11] | 0x20;
      *(undefined *)((int)param_1 + 0x41) = 0x20;
      *(undefined *)((int)param_1 + 0x42) = 0;
      return 1;
    }
  }
  param_1[9] = (int)param_3;
  *(undefined *)((int)param_1 + 0x41) = 0x21;
  param_1[0xd] = 0;
  *(undefined *)((int)param_1 + 0x42) = 0x10;
  param_1[0x11] = 0;
  *(undefined2 *)((int)param_1 + 0x2a) = param_4;
  if (*(ushort *)((int)param_1 + 0x2a) < 0x100) {
    uVar1 = *(ushort *)((int)param_1 + 0x2a);
    *(ushort *)(param_1 + 10) = uVar1;
    if (uVar1 == 0) {
      *(uint *)(iVar8 + 4) = DAT_0800d1fc | param_2 & 0x3ff | DAT_0800d1f8 & *(uint *)(iVar8 + 4);
      goto LAB_0800d0da;
    }
    sVar2 = uVar1 - 1;
    uVar9 = 0x2000000;
    uVar5 = (uint)(uVar1 & 0xff) << 0x10;
  }
  else {
    uVar5 = 0xff0000;
    sVar2 = 0xfe;
    uVar9 = 0x1000000;
    *(undefined2 *)(param_1 + 10) = 0xff;
  }
  *(uint *)(iVar8 + 0x28) = (uint)*param_3;
  *(short *)(param_1 + 10) = sVar2;
  param_1[9] = (int)(param_3 + 1);
  *(short *)((int)param_1 + 0x2a) = *(short *)((int)param_1 + 0x2a) + -1;
  *(uint *)(iVar8 + 4) =
       *(uint *)(iVar8 + 4) & DAT_0800d1f8 | uVar5 | param_2 & 0x3ff | uVar9 | 0x2000;
LAB_0800d0da:
  uVar5 = DAT_0800d208;
  uVar9 = DAT_0800d204;
LAB_0800d0e2:
  do {
    sVar2 = *(short *)((int)param_1 + 0x2a);
    while( true ) {
      if (sVar2 == 0) {
        iVar3 = FUN_0800cf20();
        uVar9 = DAT_0800d200;
        if (iVar3 != 0) {
          return 1;
        }
        iVar3 = *param_1;
        *(undefined4 *)(iVar3 + 0x1c) = 0x20;
        *(uint *)(iVar3 + 4) = *(uint *)(iVar3 + 4) & uVar9;
        *(undefined *)((int)param_1 + 0x41) = 0x20;
        *(undefined *)(param_1 + 0x10) = 0;
        *(undefined *)((int)param_1 + 0x42) = 0;
        return 0;
      }
      iVar8 = FUN_0800cec4(param_1,param_5,iVar3);
      if (iVar8 != 0) {
        return 1;
      }
      pbVar6 = (byte *)param_1[9];
      sVar2 = *(short *)(param_1 + 10);
      *(uint *)(*param_1 + 0x28) = (uint)*pbVar6;
      param_1[9] = (int)(pbVar6 + 1);
      *(short *)(param_1 + 10) = sVar2 + -1;
      *(short *)((int)param_1 + 0x2a) = *(short *)((int)param_1 + 0x2a) + -1;
      if (((short)(sVar2 + -1) != 0) || (*(short *)((int)param_1 + 0x2a) == 0)) goto LAB_0800d0e2;
      iVar8 = FUN_0800cd4c(param_1,0x80,0,param_5,iVar3);
      if (iVar8 != 0) {
        return 1;
      }
      if (*(ushort *)((int)param_1 + 0x2a) < 0x100) break;
      *(undefined2 *)(param_1 + 10) = 0xff;
      *(uint *)(*param_1 + 4) = *(uint *)(*param_1 + 4) & uVar9 | param_2 & 0x3ff | uVar5;
      sVar2 = *(short *)((int)param_1 + 0x2a);
    }
    uVar7 = *(uint *)(*param_1 + 4);
    *(undefined2 *)(param_1 + 10) = *(undefined2 *)((int)param_1 + 0x2a);
    *(uint *)(*param_1 + 4) =
         param_2 & 0x3ff | (uint)(byte)*(undefined2 *)((int)param_1 + 0x2a) << 0x10 | uVar7 & uVar9
         | 0x2000000;
  } while( true );
}


