/* 080075a0 FUN_080075a0; analyst naming is provisional. */

int FUN_080075a0(void)

{
  undefined uVar1;
  undefined4 *puVar2;
  int iVar3;
  int iVar4;
  undefined4 uVar5;
  undefined4 in_r3;
  uint uVar6;
  undefined4 unaff_r4;
  undefined4 unaff_r5;
  int *piVar7;
  
  iVar4 = DAT_080075c8;
  piVar7 = (int *)(DAT_080075c8 + 0x58c);
  FUN_0800a7d0(piVar7,0x10001,0x7ff,in_r3,in_r3);
  uVar1 = *(undefined *)(iVar4 + 0x550);
  uVar5 = *(undefined4 *)(iVar4 + 0x584);
  puVar2 = (undefined4 *)*piVar7;
  if ((puVar2 == DAT_0800a3a0) || (puVar2 == DAT_0800a3a4)) {
    uVar6 = *(uint *)(DAT_0800a3ac + 8);
    iVar3 = puVar2[2];
  }
  else {
    uVar6 = *(uint *)(DAT_0800a3a8 + 8);
    iVar3 = puVar2[2];
  }
  if ((-1 < iVar3 << 0x1d) && (*(char *)(iVar4 + 0x5dc) != '\x01')) {
    uVar6 = uVar6 & 0x1f;
    *(undefined *)(iVar4 + 0x5dc) = 1;
    if ((uVar6 < 10) && ((~(0x221U >> uVar6) & 1) == 0)) {
      iVar3 = FUN_0800a1f4(piVar7);
      if (iVar3 != 0) {
        *(undefined *)(iVar4 + 0x5dc) = 0;
        return iVar3;
      }
      puVar2 = (undefined4 *)*piVar7;
      *(uint *)(iVar4 + 0x5e0) = DAT_0800a3b0 & *(uint *)(iVar4 + 0x5e0) | 0x100;
      if ((uVar6 == 0) || (puVar2 != DAT_0800a3a4)) {
        *(uint *)(iVar4 + 0x5e0) = *(uint *)(iVar4 + 0x5e0) & 0xffefffff;
      }
      if ((*(uint *)(iVar4 + 0x5e0) & 0x1000) == 0) {
        *(undefined4 *)(iVar4 + 0x5e4) = 0;
      }
      else {
        *(uint *)(iVar4 + 0x5e4) = *(uint *)(iVar4 + 0x5e4) & 0xfffffff9;
      }
      iVar3 = *(int *)(iVar4 + 0x5d8);
      uVar6 = *(uint *)(iVar4 + 0x5b8);
      *(undefined4 *)(iVar3 + 0x3c) = DAT_0800a3b4;
      *(undefined4 *)(iVar3 + 0x40) = DAT_0800a3b8;
      *(undefined4 *)(iVar3 + 0x4c) = DAT_0800a3bc;
      *puVar2 = 0x1c;
      *(undefined *)(iVar4 + 0x5dc) = 0;
      puVar2[1] = puVar2[1] | 0x10;
      puVar2[3] = puVar2[3] & 0xfffffffc | uVar6;
      iVar4 = FUN_0800b3a0(iVar3,puVar2 + 0x10,uVar5,uVar1,unaff_r4,unaff_r5);
      *(uint *)(*piVar7 + 8) = DAT_0800a3c0 & *(uint *)(*piVar7 + 8) | 4;
      return iVar4;
    }
    *(undefined *)(iVar4 + 0x5dc) = 0;
    return 1;
  }
  return 2;
}


