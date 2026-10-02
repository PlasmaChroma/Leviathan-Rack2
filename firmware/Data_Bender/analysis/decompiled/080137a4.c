/* 080137a4 FUN_080137a4; analyst naming is provisional. */

void FUN_080137a4(undefined4 param_1)

{
  switch(param_1) {
  case 0:
    *(uint *)(DAT_080138f0 + 0xe0) = *(uint *)(DAT_080138f0 + 0xe0) | 1;
    break;
  case 1:
    *(uint *)(DAT_080138f0 + 0xe0) = *(uint *)(DAT_080138f0 + 0xe0) | 2;
    break;
  case 2:
    *(uint *)(DAT_080138f0 + 0xe0) = *(uint *)(DAT_080138f0 + 0xe0) | 4;
    break;
  case 3:
    *(uint *)(DAT_080138f0 + 0xe0) = *(uint *)(DAT_080138f0 + 0xe0) | 8;
    break;
  case 4:
    *(uint *)(DAT_080138f0 + 0xe0) = *(uint *)(DAT_080138f0 + 0xe0) | 0x10;
    break;
  case 5:
    *(uint *)(DAT_080138f0 + 0xe0) = *(uint *)(DAT_080138f0 + 0xe0) | 0x20;
    break;
  case 6:
    *(uint *)(DAT_080138f0 + 0xe0) = *(uint *)(DAT_080138f0 + 0xe0) | 0x40;
    break;
  case 7:
    *(uint *)(DAT_080138f0 + 0xe0) = *(uint *)(DAT_080138f0 + 0xe0) | 0x80;
    break;
  case 8:
    *(uint *)(DAT_080138f0 + 0xe0) = *(uint *)(DAT_080138f0 + 0xe0) | 0x100;
    break;
  case 9:
    *(uint *)(DAT_080138f0 + 0xe0) = *(uint *)(DAT_080138f0 + 0xe0) | 0x200;
    break;
  case 10:
    *(uint *)(DAT_080138f0 + 0xe0) = *(uint *)(DAT_080138f0 + 0xe0) | 0x400;
  }
  return;
}


