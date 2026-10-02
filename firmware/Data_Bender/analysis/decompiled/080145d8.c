/* 080145d8 FUN_080145d8; analyst naming is provisional. */

int * FUN_080145d8(void)

{
  int *piVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  
  piVar1 = DAT_080145e0;
  iVar3 = *DAT_080145e0;
  iVar5 = *(int *)(iVar3 + 0xc);
  iVar4 = *(int *)(iVar3 + 0x10);
  if ((iVar4 << 0x1e < 0) && (iVar5 << 0x1e < 0)) {
    *(undefined4 *)(iVar3 + 0x10) = 0xfffffffd;
    *(undefined *)(piVar1 + 7) = 1;
    if ((*(uint *)(iVar3 + 0x18) & 3) == 0) {
      FUN_0801626c();
      FUN_08016274(piVar1);
    }
    else {
      FUN_08016270();
    }
    *(undefined *)(piVar1 + 7) = 0;
  }
  if ((iVar4 << 0x1d < 0) && (iVar5 << 0x1d < 0)) {
    iVar3 = *piVar1;
    *(undefined4 *)(iVar3 + 0x10) = 0xfffffffb;
    *(undefined *)(piVar1 + 7) = 2;
    if ((*(uint *)(iVar3 + 0x18) & 0x300) == 0) {
      FUN_0801626c();
      FUN_08016274(piVar1);
    }
    else {
      FUN_08016270(piVar1);
    }
    *(undefined *)(piVar1 + 7) = 0;
  }
  if ((iVar4 << 0x1c < 0) && (iVar5 << 0x1c < 0)) {
    iVar3 = *piVar1;
    *(undefined4 *)(iVar3 + 0x10) = 0xfffffff7;
    *(undefined *)(piVar1 + 7) = 4;
    if ((*(uint *)(iVar3 + 0x1c) & 3) == 0) {
      FUN_0801626c();
      FUN_08016274(piVar1);
    }
    else {
      FUN_08016270(piVar1);
    }
    *(undefined *)(piVar1 + 7) = 0;
  }
  if ((iVar4 << 0x1b < 0) && (iVar5 << 0x1b < 0)) {
    iVar3 = *piVar1;
    *(undefined4 *)(iVar3 + 0x10) = 0xffffffef;
    *(undefined *)(piVar1 + 7) = 8;
    if ((*(uint *)(iVar3 + 0x1c) & 0x300) == 0) {
      FUN_0801626c();
      FUN_08016274(piVar1);
    }
    else {
      FUN_08016270(piVar1);
    }
    *(undefined *)(piVar1 + 7) = 0;
  }
  if ((iVar4 << 0x1f < 0) && (iVar5 << 0x1f < 0)) {
    *(undefined4 *)(*piVar1 + 0x10) = 0xfffffffe;
    FUN_08014594(piVar1);
  }
  if (iVar4 << 0x18 < 0) {
    if (-1 < iVar5 << 0x18) goto LAB_080162b6;
    *(undefined4 *)(*piVar1 + 0x10) = 0xffffff7f;
    FUN_08016604(piVar1);
    iVar3 = iVar4 << 0x17;
  }
  else {
    if (-1 < iVar4 << 0x17) goto LAB_080162b6;
    iVar3 = iVar5 << 0x18;
  }
  if (iVar3 < 0) {
    *(undefined4 *)(*piVar1 + 0x10) = 0xfffffeff;
    FUN_08016608(piVar1);
  }
LAB_080162b6:
  piVar2 = (int *)(iVar4 << 0x19);
  if (((int)piVar2 < 0) && (iVar5 << 0x19 < 0)) {
    *(undefined4 *)(*piVar1 + 0x10) = 0xffffffbf;
    piVar2 = (int *)FUN_08016278(piVar1);
  }
  if ((iVar4 << 0x1a < 0) && (iVar5 << 0x1a < 0)) {
    *(undefined4 *)(*piVar1 + 0x10) = 0xffffffdf;
    return piVar1;
  }
  return piVar2;
}


