/* 08008a98 FUN_08008a98; analyst naming is provisional. */

undefined4 FUN_08008a98(int *param_1,uint param_2,uint param_3)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  
  iVar3 = *param_1;
  param_2 = DAT_08008af0 & param_2;
  do {
    if (param_3 <= param_2) {
      return 0;
    }
    uVar2 = param_2 & 0xfffffff;
    param_2 = param_2 + 0x1000;
    iVar1 = FUN_0800895c(iVar3,uVar2);
  } while (iVar1 == 0);
  if (*(char *)(iVar3 + 0xd) != '\0') {
    *(undefined *)(iVar3 + 0xd) = 0;
    iVar1 = FUN_080085c8(iVar3,iVar3);
    if (iVar1 != 0) {
      *(undefined *)(iVar3 + 0xd) = 0;
      *(undefined *)(iVar3 + 0x5c) = 2;
      FUN_080085c8(iVar3,iVar3);
    }
  }
  *(undefined *)(iVar3 + 0x5c) = 1;
  return 1;
}


