/* 0801654c FUN_0801654c; analyst naming is provisional. */

undefined4 FUN_0801654c(int *param_1,uint *param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  bool bVar5;
  
  iVar3 = DAT_080165f0;
  if (*(char *)(param_1 + 0xf) == '\x01') {
    return 2;
  }
  iVar2 = *param_1;
  *(undefined *)((int)param_1 + 0x3d) = 2;
  iVar1 = DAT_080165f8;
  if ((iVar2 == iVar3) || (iVar2 == iVar3 + 0x400)) {
    *(uint *)(iVar2 + 4) = (*(uint *)(iVar2 + 4) & 0xff0fffff | param_2[1]) & 0xffffff8f | *param_2;
  }
  else {
    bVar5 = iVar2 != DAT_080165f4;
    iVar3 = DAT_080165f4 + 0x400;
    iVar4 = DAT_080165f4 + 0x800;
    *(uint *)(iVar2 + 4) = *(uint *)(iVar2 + 4) & 0xffffff8f | *param_2;
    if ((iVar2 != iVar1 && (iVar2 != iVar4 && (iVar2 != iVar3 && (bVar5 && iVar2 != 0x40000000))))
       && (iVar2 != DAT_080165fc)) goto LAB_080165da;
  }
  *(uint *)(iVar2 + 8) = *(uint *)(iVar2 + 8) & 0xffffff7f | param_2[2];
LAB_080165da:
  *(undefined *)((int)param_1 + 0x3d) = 1;
  *(undefined *)(param_1 + 0xf) = 0;
  return 0;
}


