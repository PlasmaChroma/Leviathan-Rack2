/* 08005f74 Wm8731_Init; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

undefined4 Wm8731_Init(undefined4 *param_1,undefined4 *param_2,undefined4 param_3)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined uVar4;
  undefined local_1c;
  byte local_1b;
  
  *param_1 = param_3;
  uVar2 = param_2[1];
  uVar3 = param_2[2];
  param_1[1] = *param_2;
  param_1[2] = uVar2;
  param_1[3] = uVar3;
  if (*(char *)((int)param_1 + 6) == '\0') {
    uVar2 = 0x1a;
    uVar4 = 0x1a;
  }
  else {
    uVar2 = 0x1b;
    uVar4 = 0x1b;
  }
  *(undefined *)(param_1 + 4) = uVar4;
  local_1c = 0x1e;
  local_1b = 0;
  iVar1 = I2CHandle_TransmitBlocking(param_1,uVar2,&local_1c,2,0xfa);
  if (iVar1 == 0) {
    System_Delay(10);
    local_1c = 0;
    local_1b = 0x17;
    iVar1 = I2CHandle_TransmitBlocking(param_1,*(undefined *)(param_1 + 4),&local_1c,2,0xfa);
    if (iVar1 == 0) {
      System_Delay(10);
      local_1b = 0x17;
      local_1c = 2;
      iVar1 = I2CHandle_TransmitBlocking(param_1,*(undefined *)(param_1 + 4),&local_1c,2,0xfa);
      if (iVar1 == 0) {
        System_Delay(10);
        local_1c = 4;
        local_1b = 0;
        iVar1 = I2CHandle_TransmitBlocking(param_1,*(undefined *)(param_1 + 4),&local_1c,2,0xfa);
        if (iVar1 == 0) {
          System_Delay(10);
          local_1c = 6;
          local_1b = 0;
          iVar1 = I2CHandle_TransmitBlocking(param_1,*(undefined *)(param_1 + 4),&local_1c,2,0xfa);
          if (iVar1 == 0) {
            System_Delay(10);
            local_1c = 8;
            local_1b = 0x12;
            iVar1 = I2CHandle_TransmitBlocking(param_1,*(undefined *)(param_1 + 4),&local_1c,2,0xfa)
            ;
            if (iVar1 == 0) {
              System_Delay(10);
              local_1b = 0;
              local_1c = 10;
              iVar1 = I2CHandle_TransmitBlocking
                                (param_1,*(undefined *)(param_1 + 4),&local_1c,2,0xfa);
              if (iVar1 == 0) {
                System_Delay(10);
                local_1c = 0xc;
                if (*(char *)(param_1 + 1) == '\0') {
                  local_1b = 0x42;
                }
                else {
                  local_1b = 0x62;
                }
                iVar1 = I2CHandle_TransmitBlocking
                                  (param_1,*(undefined *)(param_1 + 4),&local_1c,2,0xfa);
                if (iVar1 == 0) {
                  System_Delay(10);
                  local_1b = (byte)param_1[2] | (byte)param_1[3];
                  if (*(char *)(param_1 + 1) == '\0') {
                    local_1b = local_1b | 0x40;
                  }
                  if (*(char *)((int)param_1 + 5) != '\0') {
                    local_1b = local_1b | 0x20;
                  }
                  local_1c = 0xe;
                  iVar1 = I2CHandle_TransmitBlocking
                                    (param_1,*(undefined *)(param_1 + 4),&local_1c,2,0xfa);
                  if (iVar1 == 0) {
                    System_Delay(10);
                    local_1c = 0x10;
                    local_1b = 0;
                    iVar1 = I2CHandle_TransmitBlocking
                                      (param_1,*(undefined *)(param_1 + 4),&local_1c,2,0xfa);
                    if (iVar1 == 0) {
                      System_Delay(10);
                      local_1c = 0x12;
                      local_1b = 0;
                      iVar1 = I2CHandle_TransmitBlocking
                                        (param_1,*(undefined *)(param_1 + 4),&local_1c,2,0xfa);
                      if (iVar1 == 0) {
                        System_Delay(10);
                        iVar1 = Wm8731_WriteRegister(param_1,9,1);
                        if (iVar1 == 0) {
                          return 0;
                        }
                        return 1;
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return 1;
}


