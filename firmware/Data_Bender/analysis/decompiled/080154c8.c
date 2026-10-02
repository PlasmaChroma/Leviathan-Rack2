/* 080154c8 FUN_080154c8; analyst naming is provisional. */

void FUN_080154c8(uint **param_1)

{
  uint uVar1;
  uint uVar2;
  uint *puVar3;
  
  uVar1 = DAT_0801557c;
  puVar3 = *param_1;
  uVar2 = puVar3[5];
  puVar3[6] = puVar3[6] | 8;
  puVar3[6] = puVar3[6] | 0x10;
  *puVar3 = *puVar3 & 0xfffffffe;
  puVar3[4] = uVar1 & puVar3[4];
  puVar3[2] = puVar3[2] & 0xffff3fff;
  if ((*(char *)((int)param_1 + 0x81) != '\x04') && ((int)(uVar2 << 0x1a) < 0)) {
    param_1[0x21] = (uint *)((uint)param_1[0x21] | 0x80);
    puVar3[6] = puVar3[6] | 0x20;
  }
  if ((*(char *)((int)param_1 + 0x81) != '\x03') && ((int)(uVar2 << 0x19) < 0)) {
    param_1[0x21] = (uint *)((uint)param_1[0x21] | 4);
    puVar3[6] = puVar3[6] | 0x40;
  }
  if ((int)(uVar2 << 0x16) < 0) {
    param_1[0x21] = (uint *)((uint)param_1[0x21] | 1);
    puVar3[6] = puVar3[6] | 0x200;
  }
  if ((int)(uVar2 << 0x17) < 0) {
    param_1[0x21] = (uint *)((uint)param_1[0x21] | 8);
    puVar3[6] = puVar3[6] | 0x100;
  }
  *(undefined2 *)((int)param_1 + 0x62) = 0;
  *(undefined2 *)((int)param_1 + 0x6a) = 0;
  return;
}


