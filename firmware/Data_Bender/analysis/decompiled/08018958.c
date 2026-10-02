/* 08018958 newlib_rand_LCG64; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

uint newlib_rand_LCG64(void)

{
  longlong lVar1;
  undefined4 uVar2;
  undefined4 *puVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  
  iVar6 = *DAT_080189b4;
  if (*(int *)(iVar6 + 0x38) == 0) {
    puVar3 = (undefined4 *)FUN_08018c24(0x18);
    *(undefined4 **)(iVar6 + 0x38) = puVar3;
    if (puVar3 == (undefined4 *)0x0) {
      puVar3 = (undefined4 *)FUN_080189d4(DAT_080189bc,0x4e,0,DAT_080189b8);
    }
    uVar2 = DAT_080189c4;
    *puVar3 = DAT_080189c0;
    puVar3[1] = uVar2;
    puVar3[2] = DAT_080189c8;
    *(undefined2 *)(puVar3 + 3) = 0xb;
    puVar3[4] = 1;
    puVar3[5] = 0;
  }
  iVar6 = *(int *)(iVar6 + 0x38);
  lVar1 = (ulonglong)*(uint *)(iVar6 + 0x10) * (ulonglong)DAT_080189d0;
  uVar4 = (uint)lVar1;
  uVar5 = DAT_080189d0 * *(int *)(iVar6 + 0x14) + *(uint *)(iVar6 + 0x10) * DAT_080189cc +
          (int)((ulonglong)lVar1 >> 0x20) + (uint)(0xfffffffe < uVar4);
  *(uint *)(iVar6 + 0x10) = uVar4 + 1;
  *(uint *)(iVar6 + 0x14) = uVar5;
  return uVar5 & 0x7fffffff;
}


