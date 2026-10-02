/* 08006168 SdramHandle_ConfigureController; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

int SdramHandle_ConfigureController(void)

{
  undefined4 uVar1;
  int iVar2;
  undefined4 local_24;
  undefined4 uStack_20;
  undefined4 local_1c;
  undefined4 uStack_18;
  undefined4 local_14;
  undefined4 uStack_10;
  undefined4 local_c;
  
  uVar1 = DAT_080061cc;
  iVar2 = DAT_080061c8;
  *(undefined4 *)(DAT_080061c8 + 0x20) = 0;
  *(undefined4 *)(iVar2 + 0x2c) = 0;
  *(undefined4 *)(iVar2 + 0xc) = 1;
  *(undefined4 *)(iVar2 + 0x10) = 8;
  *(undefined4 *)(iVar2 + 4) = uVar1;
  *(undefined4 *)(iVar2 + 8) = 0;
  *(undefined4 *)(iVar2 + 0x14) = 0x20;
  *(undefined4 *)(iVar2 + 0x18) = 0x40;
  local_24 = 2;
  uStack_20 = 7;
  *(undefined4 *)(iVar2 + 0x1c) = 0x180;
  local_1c = 4;
  uStack_18 = 8;
  *(undefined4 *)(iVar2 + 0x24) = 0x800;
  local_c = 10;
  *(undefined4 *)(iVar2 + 0x28) = 0x1000;
  local_14 = 3;
  uStack_10 = 0x10;
  iVar2 = FUN_0801229c(iVar2 + 4,&local_24);
  if (iVar2 != 0) {
    iVar2 = 1;
  }
  return iVar2;
}


