/* 08009004 FUN_08009004; analyst naming is provisional. */

void FUN_08009004(int *param_1)

{
  bool bVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  undefined2 *puVar6;
  undefined2 local_40;
  undefined2 local_3e;
  undefined2 local_3c;
  undefined2 local_3a;
  undefined2 local_38;
  byte abStack_36 [2];
  int local_34;
  undefined4 local_30;
  undefined4 uStack_2c;
  undefined4 local_28;
  undefined4 local_24;
  
  iVar2 = DAT_080090bc;
  puVar6 = &local_40;
  local_40 = *(undefined2 *)((int)param_1 + 6);
  local_3e = *(undefined2 *)(param_1 + 1);
  local_3c = *(undefined2 *)(param_1 + 2);
  local_3a = *(undefined2 *)((int)param_1 + 10);
  local_38 = *(undefined2 *)(param_1 + 3);
  if (param_1[6] == 0) {
    bVar1 = true;
  }
  else {
    bVar1 = param_1[7] == 0;
  }
  local_28 = 1;
  local_30 = 2;
  uStack_2c = 1;
  do {
    uVar3 = (uint)*(byte *)puVar6;
    uVar5 = (uint)*(byte *)((int)puVar6 + 1);
    if ((*(byte *)(param_1 + 1) == uVar3) && (*(byte *)((int)param_1 + 5) == uVar5)) {
      if (bVar1) {
        iVar4 = *param_1;
        if (iVar4 == 0) goto LAB_080090a6;
LAB_08009066:
        if (iVar4 == 1) {
          if (uVar3 == 0) {
            if (uVar5 == 2) {
              local_24 = 8;
            }
            else {
              local_24 = 10;
            }
          }
          else {
            local_24 = 10;
          }
        }
        goto LAB_08009070;
      }
    }
    else {
      iVar4 = *param_1;
      if (iVar4 != 0) goto LAB_08009066;
LAB_080090a6:
      local_24 = 6;
LAB_08009070:
      local_34 = 1 << uVar5;
      if (uVar3 < 0xb) {
        iVar4 = iVar2 + uVar3 * 0x400;
      }
      else {
        iVar4 = 0;
      }
      FUN_0800c080(iVar4,&local_34);
    }
    puVar6 = (undefined2 *)((int)puVar6 + 2);
    if ((undefined2 *)abStack_36 == puVar6) {
      return;
    }
  } while( true );
}


