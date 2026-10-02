/* 080139ac FUN_080139ac; analyst naming is provisional. */

void FUN_080139ac(int param_1)

{
  undefined4 uVar1;
  int iVar2;
  
  uVar1 = DAT_08013a40;
  *(undefined4 *)(param_1 + 0x134) = 0;
  *(undefined4 *)(param_1 + 0xb0) = uVar1;
  *(undefined4 *)(param_1 + 0x138) = 0x400;
  *(undefined4 *)(param_1 + 0xcc) = 0;
  *(undefined4 *)(param_1 + 0xd0) = 0x30000;
  *(undefined4 *)(param_1 + 0x144) = 0;
  *(undefined4 *)(param_1 + 0x148) = 0x30000;
  *(undefined4 *)(param_1 + 0xd4) = 4;
  *(undefined4 *)(param_1 + 0x14c) = 4;
  *(undefined4 *)(param_1 + 0xd8) = 3;
  *(undefined4 *)(param_1 + 0x150) = 3;
  uVar1 = DAT_08013a44;
  *(undefined4 *)(param_1 + 0xbc) = 0;
  *(undefined4 *)(param_1 + 0xc0) = 0x400;
  *(undefined4 *)(param_1 + 0x128) = uVar1;
  *(undefined4 *)(param_1 + 0xc4) = 0;
  *(undefined4 *)(param_1 + 200) = 0;
  *(undefined4 *)(param_1 + 0xdc) = 0;
  *(undefined4 *)(param_1 + 0xe0) = 0;
  *(undefined4 *)(param_1 + 0x13c) = 0;
  *(undefined4 *)(param_1 + 0x140) = 0;
  *(undefined4 *)(param_1 + 0x154) = 0;
  *(undefined4 *)(param_1 + 0x158) = 0;
  FUN_08013940();
  *(undefined4 *)(param_1 + 0xb8) = 0;
  *(undefined4 *)(param_1 + 0x130) = 0x40;
  iVar2 = FUN_0800af40(param_1 + 0xb0);
  if (iVar2 != 0) {
    software_bkpt(0xff);
    do {
                    /* WARNING: Do nothing block with infinite loop */
    } while( true );
  }
  iVar2 = FUN_0800af40(param_1 + 0x128);
  if (iVar2 != 0) {
    software_bkpt(0xff);
    do {
                    /* WARNING: Do nothing block with infinite loop */
    } while( true );
  }
  *(int *)(param_1 + 0xa0) = param_1 + 0x128;
  *(int *)(param_1 + 0xa4) = param_1 + 0xb0;
  *(int *)(param_1 + 0xe8) = param_1 + 0x28;
  *(int *)(param_1 + 0x160) = param_1 + 0x28;
  return;
}


