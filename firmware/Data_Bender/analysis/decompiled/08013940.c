/* 08013940 FUN_08013940; analyst naming is provisional. */

undefined4 FUN_08013940(int param_1)

{
  undefined4 uVar1;
  
  switch(*(undefined4 *)(param_1 + 8)) {
  case 0:
    *(undefined4 *)(param_1 + 0xb4) = 0x25;
    *(undefined4 *)(param_1 + 300) = 0x26;
    return *(undefined4 *)(param_1 + 8);
  case 1:
    *(undefined4 *)(param_1 + 0xb4) = 0x27;
    *(undefined4 *)(param_1 + 300) = 0x28;
    return 0;
  case 2:
    *(undefined4 *)(param_1 + 0xb4) = 0x3d;
    *(undefined4 *)(param_1 + 300) = 0x3e;
    return 0;
  case 3:
    *(undefined4 *)(param_1 + 0xb4) = 0x53;
    *(undefined4 *)(param_1 + 300) = 0x54;
    return 0;
  case 4:
    uVar1 = 0;
    *(undefined4 *)(param_1 + 0xb4) = 0x55;
    *(undefined4 *)(param_1 + 300) = 0x56;
    break;
  default:
    uVar1 = 1;
  }
  return uVar1;
}


