/* 08008050 I2CHandle_TransmitBlocking; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

int I2CHandle_TransmitBlocking
              (int *param_1,uint param_2,undefined4 param_3,undefined4 param_4,undefined4 param_5)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar2 = *param_1;
  iVar3 = iVar2 + 0x8c;
  do {
    iVar1 = FUN_0800e628(iVar3);
  } while (iVar1 != 0x20);
  if (*(int *)(iVar2 + 0xc) == 0) {
    iVar2 = FUN_0800d02c(iVar3,(param_2 & 0x7fff) << 1,param_3,param_4,param_5);
  }
  else {
    iVar2 = FUN_0800d20c(iVar3,param_3,param_4,param_5);
  }
  if (iVar2 != 0) {
    iVar2 = 1;
  }
  return iVar2;
}


