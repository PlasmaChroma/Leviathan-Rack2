/* 08005cd8 DaisySeed_ConfigureAudio; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DaisySeed_ConfigureAudio(int param_1)

{
  int iVar1;
  undefined4 local_78;
  undefined2 local_74;
  undefined local_72;
  int local_70;
  undefined4 local_6c;
  undefined4 local_68;
  undefined4 local_64;
  int iStack_60;
  undefined4 local_5c;
  undefined local_58;
  undefined4 local_54;
  undefined4 local_50;
  undefined4 local_4c;
  undefined4 uStack_48;
  undefined4 local_40;
  int iStack_3c;
  int local_38;
  undefined2 local_34;
  undefined4 local_30;
  undefined4 uStack_2c;
  undefined4 local_28;
  undefined4 uStack_24;
  int local_20;
  undefined4 local_1c;
  
  local_30 = 3;
  uStack_2c = 1;
  local_34 = 0xff0b;
  local_40 = 0;
  iStack_3c = DAT_08005df4;
  local_38 = DAT_08005df4 + -0x4f8fd00;
  local_28 = 0;
  uStack_24 = 1;
  iVar1 = DaisySeed_CheckBoardVersion();
  if (iVar1 == 1) {
    local_38 = CONCAT22(0x604,(undefined2)local_38);
    local_34 = 0x304;
    local_5c = 0;
    local_58 = 0x10;
    local_78 = 0;
    local_64 = DAT_08005dfc;
    local_1c = 0;
    local_68 = iVar1;
    iStack_60 = iVar1;
    local_20 = iVar1;
    I2CHandle_Init(&local_78,&local_68);
    local_74 = 1;
    local_72 = 0;
    local_6c = 8;
    local_54 = 0;
    local_70 = iVar1;
    Wm8731_Init(&local_54,&local_74,local_78);
  }
  else if (iVar1 == 2) {
    local_54 = 0xff0b;
    local_38 = CONCAT22(0x604,(undefined2)local_38);
    local_68 = CONCAT22(local_68._2_2_,0xb01);
    local_34 = 0x304;
    local_50 = 0;
    local_20 = 0;
    local_1c = 1;
    local_4c = 0;
    uStack_48 = 0;
    GPIO_Init(&local_54,local_68,1,0,0);
    GPIO_Write(&local_54,0);
  }
  else {
    local_54 = CONCAT22(local_54._2_2_,0xb01);
    local_20 = 0;
    local_1c = 1;
    local_38 = CONCAT22(0x604,(undefined2)local_38);
    local_34 = 0x304;
    Ak4556_Init(param_1 + 0x58,local_54);
  }
  SaiHandle_Init(param_1 + 0x70,&local_40);
  local_54 = *DAT_08005df8;
  local_50 = DAT_08005df8[1];
  local_4c = DAT_08005df8[2];
  uStack_48 = DAT_08005df8[3];
  AudioHandle_Init_single_SAI(param_1 + 0x14,&local_54,*(undefined4 *)(param_1 + 0x70));
  return;
}


