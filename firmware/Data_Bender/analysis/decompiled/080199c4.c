/* 080199c4 FUN_080199c4; analyst naming is provisional. */

int FUN_080199c4(int param_1)

{
  int iVar1;
  
  iVar1 = *DAT_080199d8;
  if (*DAT_080199d8 == 0) {
    iVar1 = DAT_080199dc;
  }
  *DAT_080199d8 = param_1 + iVar1;
  return iVar1;
}


