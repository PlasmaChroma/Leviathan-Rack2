/* 08018ccc FUN_08018ccc; analyst naming is provisional. */

uint FUN_08018ccc(undefined4 *param_1,uint param_2,undefined4 param_3,undefined4 param_4)

{
  int *piVar1;
  int iVar2;
  uint *puVar3;
  uint uVar4;
  uint *puVar5;
  uint *puVar6;
  uint uVar7;
  
  uVar7 = (param_2 + 3 & 0xfffffffc) + 8;
  if (uVar7 < 0xc) {
    uVar7 = 0xc;
  }
  if (((int)uVar7 < 0) || (uVar7 < param_2)) {
    *param_1 = 0xc;
  }
  else {
    FUN_0801984c();
    piVar1 = DAT_08018d7c;
    puVar3 = *DAT_08018d78;
    for (puVar6 = *DAT_08018d78; puVar6 != (uint *)0x0; puVar6 = (uint *)puVar6[1]) {
      uVar4 = *puVar6 - uVar7;
      if (-1 < (int)uVar4) {
        if (0xb < uVar4) {
          *puVar6 = uVar4;
          puVar6 = (uint *)((int)puVar6 + uVar4);
          goto LAB_08018d30;
        }
        puVar5 = (uint *)puVar6[1];
        if (puVar3 == puVar6) {
          *DAT_08018d78 = puVar5;
        }
        if (puVar3 != puVar6) {
          puVar3[1] = (uint)puVar5;
        }
        goto LAB_08018d3e;
      }
      puVar3 = puVar6;
    }
    if (*DAT_08018d7c == 0) {
      iVar2 = FUN_0801935c(param_1,0,puVar3,0,param_4);
      *piVar1 = iVar2;
    }
    puVar3 = (uint *)FUN_0801935c(param_1,uVar7);
    if ((puVar3 != (uint *)0xffffffff) &&
       ((puVar6 = (uint *)((int)puVar3 + 3U & 0xfffffffc), puVar3 == puVar6 ||
        (iVar2 = FUN_0801935c(param_1,(int)puVar6 - (int)puVar3), iVar2 != -1)))) {
LAB_08018d30:
      *puVar6 = uVar7;
LAB_08018d3e:
      FUN_08019858(param_1);
      uVar7 = (int)puVar6 + 0xbU & 0xfffffff8;
      iVar2 = uVar7 - (int)(puVar6 + 1);
      if (iVar2 == 0) {
        return uVar7;
      }
      *(uint *)((int)puVar6 + iVar2) = (int)(puVar6 + 1) - uVar7;
      return uVar7;
    }
    *param_1 = 0xc;
    FUN_08019858(param_1);
  }
  return 0;
}


