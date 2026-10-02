/* 08014880 FUN_08014880; analyst naming is provisional. */

undefined4 FUN_08014880(int param_1)

{
  undefined4 uVar1;
  
  switch(*(undefined4 *)(param_1 + 0x1c)) {
  case 0:
    *(undefined4 *)(param_1 + 0xcc) = 0x29;
    *(undefined4 *)(param_1 + 0x144) = 0x2a;
    return *(undefined4 *)(param_1 + 0x1c);
  case 1:
    *(undefined4 *)(param_1 + 0xcc) = 0x2b;
    *(undefined4 *)(param_1 + 0x144) = 0x2c;
    return 0;
  case 2:
    *(undefined4 *)(param_1 + 0xcc) = 0x2d;
    *(undefined4 *)(param_1 + 0x144) = 0x2e;
    return 0;
  case 3:
    *(undefined4 *)(param_1 + 0xcc) = 0x3f;
    *(undefined4 *)(param_1 + 0x144) = 0x40;
    return 0;
  case 4:
    *(undefined4 *)(param_1 + 0xcc) = 0x41;
    *(undefined4 *)(param_1 + 0x144) = 0x42;
    return 0;
  case 5:
    *(undefined4 *)(param_1 + 0xcc) = 0x47;
    *(undefined4 *)(param_1 + 0x144) = 0x48;
    return 0;
  case 6:
    *(undefined4 *)(param_1 + 0xcc) = 0x4f;
    *(undefined4 *)(param_1 + 0x144) = 0x50;
    return 0;
  case 7:
    uVar1 = 0;
    *(undefined4 *)(param_1 + 0xcc) = 0x51;
    *(undefined4 *)(param_1 + 0x144) = 0x52;
    break;
  default:
    uVar1 = 1;
  }
  return uVar1;
}


