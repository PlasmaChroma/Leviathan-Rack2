/* 08015150 FUN_08015150; analyst naming is provisional. */

void FUN_08015150(int *param_1)

{
  code *pcVar1;
  int *piVar2;
  int iVar3;
  int iStack_38;
  int local_34 [4];
  undefined4 local_24;
  undefined4 uStack_20;
  undefined4 uStack_1c;
  undefined4 uStack_18;
  undefined4 local_14;
  
  piVar2 = &iStack_38;
  iVar3 = 0;
  local_34[0] = *DAT_080151ac;
  local_34[1] = DAT_080151ac[1];
  local_34[2] = DAT_080151ac[2];
  local_34[3] = DAT_080151ac[3];
  local_24 = DAT_080151ac[4];
  uStack_20 = DAT_080151ac[5];
  uStack_1c = DAT_080151ac[6];
  uStack_18 = DAT_080151ac[7];
  local_14 = DAT_080151ac[8];
  while (piVar2 = piVar2 + 1, *param_1 != *piVar2) {
    iVar3 = iVar3 + 1;
    if (iVar3 == 9) {
                    /* WARNING: Does not return */
      pcVar1 = (code *)software_udf(0xff,0x80151a8);
      (*pcVar1)();
    }
  }
  if (*(char *)(iVar3 * 0x1b8 + DAT_080151b0 + 0x14) == '\0') {
    return;
  }
  FUN_08014660();
  return;
}


