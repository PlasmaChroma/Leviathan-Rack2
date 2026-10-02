/* 08008a90 FUN_08008a90; analyst naming is provisional. */

undefined4 FUN_08008a90(int *param_1,uint param_2,uint param_3,int param_4)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  
  iVar2 = *param_1;
  uVar6 = param_2 & 0xff;
  param_2 = param_2 & 0xfffffff;
  uVar7 = param_3 & 0xff;
  if (uVar6 == 0) {
    if (param_3 < 0x100) {
      FUN_080087a0(iVar2,param_2,param_3,param_4,0);
    }
    else {
      uVar6 = param_2;
      do {
        uVar3 = uVar6 + 0x100;
        FUN_080087a0(iVar2,uVar6,0x100,(param_4 - param_2) + uVar6,0);
        uVar6 = uVar3;
      } while (uVar3 != param_2 + (param_3 & 0xffffff00));
      if (uVar7 != 0) {
        FUN_080087a0(iVar2,param_2 + 0x100 + ((param_3 >> 8) - 1) * 0x100,uVar7,
                     (param_3 & 0xffffff00) + param_4,0);
      }
    }
  }
  else {
    uVar3 = 0x100 - uVar6;
    if (param_3 < 0x100) {
      if (uVar3 < uVar7) {
        FUN_080087a0(iVar2,param_2,uVar3,param_4,0);
        FUN_080087a0(iVar2,param_2 + uVar3,uVar6 + (uVar7 - 0x100),param_4 + uVar3,0);
      }
      else {
        FUN_080087a0(iVar2,param_2,param_3,param_4,0);
      }
    }
    else {
      uVar6 = uVar6 + (param_3 - 0x100);
      FUN_080087a0(iVar2,param_2,uVar3,param_4,0);
      param_4 = param_4 + uVar3;
      iVar1 = param_2 + uVar3;
      if (uVar6 >> 8 != 0) {
        iVar4 = iVar1;
        do {
          iVar5 = iVar4 + 0x100;
          FUN_080087a0(iVar2,iVar4,0x100,(param_4 - iVar1) + iVar4,0);
          iVar4 = iVar5;
        } while (iVar5 != iVar1 + (uVar6 & 0xffffff00));
        param_4 = param_4 + (uVar6 & 0xffffff00);
        iVar1 = iVar1 + 0x100 + ((uVar6 >> 8) - 1) * 0x100;
      }
      if ((uVar6 & 0xff) != 0) {
        FUN_080087a0(iVar2,iVar1,uVar6 & 0xff,param_4,0);
      }
    }
  }
  if (*(char *)(iVar2 + 0xd) != '\0') {
    *(undefined *)(iVar2 + 0xd) = 0;
    iVar1 = FUN_080085c8(iVar2,iVar2);
    if (iVar1 != 0) {
      *(undefined *)(iVar2 + 0xd) = 0;
      *(undefined *)(iVar2 + 0x5c) = 2;
      FUN_080085c8(iVar2,iVar2);
      return 1;
    }
  }
  return 0;
}


