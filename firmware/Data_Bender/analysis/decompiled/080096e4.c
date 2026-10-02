/* 080096e4 System_ConfigureMpu; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void System_ConfigureMpu(void)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined2 local_20 [2];
  undefined4 local_1c;
  undefined4 local_18;
  uint local_14;
  
  FUN_0800aa54();
  local_20[0] = 1;
  local_14 = 0x100;
  local_1c = 0x30000000;
  local_18 = DAT_08009760;
  FUN_0800aa90(local_20);
  uVar1 = local_18;
  local_18 = CONCAT31(local_18._1_3_,0x19);
  uVar2 = local_18;
  local_1c = 0xc0000000;
  local_14._0_2_ = (ushort)local_14 & 0xff;
  local_14 = CONCAT22(0x101,(ushort)local_14);
  local_20[0] = CONCAT11(1,(undefined)local_20[0]);
  local_18._3_1_ = SUB41(uVar1,3);
  local_18._0_2_ = (ushort)uVar2;
  local_18._0_3_ = (uint3)(ushort)local_18;
  FUN_0800aa90(local_20);
  uVar1 = local_18;
  local_14._0_2_ = CONCAT11(1,(undefined)local_14);
  local_20[0] = CONCAT11(2,(undefined)local_20[0]);
  local_18 = CONCAT31(local_18._1_3_,0xb);
  uVar2 = local_18;
  local_1c = 0x38800000;
  local_14 = (uint)(ushort)local_14;
  local_18._3_1_ = SUB41(uVar1,3);
  local_18._0_2_ = (ushort)uVar2;
  local_18._0_3_ = CONCAT12(1,(ushort)local_18);
  FUN_0800aa90(local_20);
  FUN_0800aa70(4);
  return;
}


