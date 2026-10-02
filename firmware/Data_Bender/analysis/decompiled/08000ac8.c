/* 08000ac8 FUN_08000ac8; analyst naming is provisional. */

undefined4 *
FUN_08000ac8(undefined4 *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  undefined4 uVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  
  *(undefined *)(param_1 + 10) = 0xb;
  *(undefined *)(param_1 + 0xf) = 0xb;
  *(undefined *)(param_1 + 0x16) = 0xb;
  param_1[5] = 0;
  param_1[8] = 0;
  *(undefined *)((int)param_1 + 0x29) = 0xff;
  param_1[0xd] = 0;
  *(undefined *)((int)param_1 + 0x3d) = 0xff;
  param_1[0x12] = 0;
  *(undefined *)((int)param_1 + 0x59) = 0xff;
  param_1[0x19] = 0;
  param_1[0x1c] = 0;
  *param_1 = 0;
  param_1[1] = 0xff0bff0b;
  param_1[2] = 0xff0bff0b;
  param_1[3] = 0xff0bff0b;
  param_1[0xb] = 0;
  param_1[0xc] = 0;
  param_1[0x10] = 0;
  param_1[0x11] = 0;
  param_1[0x17] = 0;
  param_1[0x18] = 0;
  puVar2 = param_1 + 0x1d;
  do {
    puVar3 = puVar2 + 9;
    puVar2[3] = 0;
    *(undefined *)(puVar2 + 2) = 0xb;
    *(undefined *)((int)puVar2 + 9) = 0xff;
    puVar2[4] = 0;
    puVar2[5] = 0;
    puVar2 = puVar3;
  } while (puVar3 != param_1 + 0x5c);
  do {
    puVar2 = puVar3 + 6;
    puVar3[1] = 0;
    *(undefined *)puVar3 = 0xb;
    *(undefined *)((int)puVar3 + 1) = 0xff;
    puVar3[2] = 0;
    puVar3[3] = 0;
    uVar1 = DAT_08000ba8;
    puVar3 = puVar2;
  } while (puVar2 != param_1 + 0x74);
  param_1[0xd4] = 0;
  *(undefined *)((int)param_1 + 0x35e) = 0xb;
  *(undefined *)((int)param_1 + 0x35f) = 0xff;
  *(undefined *)(param_1 + 0xd8) = 0xb;
  *(undefined *)((int)param_1 + 0x361) = 0xff;
  param_1[0xdb] = 0;
  param_1[0xd9] = 0;
  param_1[0xda] = 0;
  libc_memcpy((int)param_1 + 0x376,uVar1,0x200,puVar2,param_4);
  *(undefined *)(param_1 + 0x188) = 0xb;
  *(undefined *)((int)param_1 + 0x621) = 0xff;
  param_1[0x189] = 0;
  param_1[0x18a] = 0;
  param_1[0x18b] = 0;
  *(undefined *)(param_1 + 0x18d) = 0xb;
  *(undefined *)((int)param_1 + 0x635) = 0xff;
  param_1[0x18e] = 0;
  param_1[399] = 0;
  param_1[400] = 0;
  return param_1;
}


