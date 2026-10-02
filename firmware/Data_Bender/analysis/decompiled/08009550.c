/* 08009550 System_ConfigureClocks; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void System_ConfigureClocks(int *param_1)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 local_14c;
  undefined4 uStack_148;
  int local_144;
  undefined4 local_140;
  undefined4 local_13c;
  undefined4 uStack_138;
  undefined4 local_134;
  undefined4 local_130;
  undefined4 local_12c;
  undefined4 local_128;
  undefined4 local_114;
  undefined4 local_108;
  undefined4 uStack_104;
  undefined4 local_100;
  undefined4 local_fc;
  undefined4 local_f8;
  undefined4 local_f4;
  undefined4 local_f0;
  undefined4 local_ec;
  undefined4 local_e8;
  undefined4 uStack_e4;
  undefined4 local_e0;
  undefined4 uStack_dc;
  undefined4 local_d8;
  undefined4 local_d4;
  undefined4 local_d0;
  undefined4 local_cc;
  undefined4 local_c8;
  undefined4 local_c4;
  int local_c0;
  undefined4 local_bc;
  undefined4 local_b8;
  undefined4 uStack_b4;
  undefined4 local_b0;
  undefined4 local_ac;
  undefined4 local_a8;
  undefined4 local_a4;
  int local_a0;
  int local_9c;
  undefined4 uStack_98;
  int local_94;
  undefined4 uStack_90;
  undefined4 local_88;
  undefined4 local_84;
  undefined4 local_80;
  int local_68;
  int iStack_64;
  int local_5c;
  undefined4 local_58;
  undefined4 local_48;
  undefined4 local_3c;
  
  memset(&local_12c,0,0x4c);
  memset(&local_14c,0,0x20);
  memset(&local_e0,0,0xc0);
  FUN_0800f084(2);
  iVar1 = DAT_080096dc;
  iVar2 = DAT_080096d8;
  if (*param_1 == 1) {
    uVar3 = 4;
    local_fc = 0xf0;
    *(uint *)(DAT_080096dc + 0x18) = *(uint *)(DAT_080096dc + 0x18) | 0xc000;
    *(uint *)(iVar2 + 0x2c) = *(uint *)(iVar2 + 0x2c) | 1;
  }
  else {
    uVar3 = 2;
    local_fc = 200;
    *(uint *)(DAT_080096d8 + 0x2c) = *(uint *)(DAT_080096d8 + 0x2c) & 0xfffffffe;
    *(uint *)(iVar1 + 0x18) = *(uint *)(iVar1 + 0x18) | 0xc000;
  }
  do {
  } while (-1 < *(int *)(DAT_080096dc + 0x18) << 0x12);
  *(uint *)(DAT_080096e0 + 0x28) = *(uint *)(DAT_080096e0 + 0x28) & 0xfffffffc | 2;
  local_12c = 0x21;
  local_f8 = 2;
  local_f0 = 2;
  local_128 = 0x10000;
  local_114 = 1;
  local_100 = 4;
  local_f4 = 5;
  local_ec = 8;
  local_108 = 2;
  uStack_104 = 2;
  local_e8 = 0;
  uStack_e4 = 0;
  local_144 = FUN_0800fb00(&local_12c);
  if (local_144 != 0) {
    do {
                    /* WARNING: Do nothing block with infinite loop */
    } while( true );
  }
  local_130 = 0x40;
  local_140 = 8;
  local_134 = 0x400;
  local_13c = 0x40;
  uStack_138 = 0x40;
  local_14c = 0x3f;
  uStack_148 = 3;
  local_c0 = FUN_08010138(&local_14c,uVar3);
  if (local_c0 != 0) {
    do {
                    /* WARNING: Do nothing block with infinite loop */
    } while( true );
  }
  local_cc = 2;
  local_88 = 2;
  local_bc = 0x1000;
  local_80 = 0x1000;
  local_58 = 0x300000;
  local_d0 = 8;
  local_ac = 4;
  local_a4 = 0x400;
  local_d8 = 1;
  local_c8 = 1;
  local_3c = 0x10000;
  local_b0 = 0x10;
  uStack_98 = 2;
  uStack_90 = 0x10000;
  local_48 = 0x100;
  local_c4 = 0x80;
  local_84 = 0x80;
  local_b8 = 6;
  uStack_b4 = 0x127;
  local_d4 = 0xc;
  local_a8 = 0x20;
  local_e0 = DAT_080096d0;
  uStack_dc = DAT_080096d4;
  local_a0 = local_c0;
  local_9c = local_c0;
  local_94 = local_c0;
  local_68 = local_c0;
  iStack_64 = local_c0;
  local_5c = local_c0;
  iVar2 = FUN_08010638(&local_e0);
  if (iVar2 == 0) {
    FUN_0800f0cc();
    return;
  }
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}


