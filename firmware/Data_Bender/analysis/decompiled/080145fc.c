/* 080145fc FUN_080145fc; analyst naming is provisional. */

int FUN_080145fc(int **param_1,int *param_2)

{
  int iVar1;
  int *piVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  int *piVar6;
  int local_34;
  int local_30;
  int local_2c;
  int local_28 [4];
  undefined4 local_18;
  int local_14;
  int local_10;
  int iStack_c;
  
  piVar2 = (int *)(*param_2 * 100 + DAT_08014610);
  *param_1 = piVar2;
  iVar5 = *param_2;
  if (iVar5 < 4) {
    iVar4 = param_2[2];
    iVar1 = *param_2;
    piVar2[1] = param_2[1];
    *piVar2 = iVar1;
    piVar2[2] = iVar4;
    piVar6 = DAT_080143cc;
    *(undefined *)(piVar2 + 3) = *(undefined *)(param_2 + 3);
    if (piVar2[1] == 0) {
      iVar1 = 0;
    }
    else {
      iVar1 = 0x10;
    }
    piVar2[6] = iVar1;
    piVar2[5] = 0;
    local_28[0] = *piVar6;
    local_28[1] = piVar6[1];
    local_28[2] = piVar6[2];
    local_28[3] = piVar6[3];
    iVar5 = local_28[iVar5];
    piVar2[4] = iVar5;
    if ((iVar5 == 0x40000000) || (iVar5 == DAT_080143d0)) {
      uVar3 = piVar2[2];
    }
    else {
      uVar3 = (uint)*(ushort *)(piVar2 + 2);
    }
    piVar6 = piVar2 + 4;
    piVar2[10] = 0x80;
    piVar2[7] = uVar3;
    piVar2[8] = 0;
    local_34 = FUN_080164e8(piVar6);
    if (local_34 != 0) {
      software_bkpt(0xff);
      do {
                    /* WARNING: Do nothing block with infinite loop */
      } while( true );
    }
    local_18 = 0x1000;
    local_30 = local_34;
    local_2c = local_34;
    local_14 = local_34;
    local_10 = local_34;
    iStack_c = local_34;
    local_34 = FUN_080160d4(piVar6,&local_18);
    if (local_34 != 0) {
      software_bkpt(0xff);
      do {
                    /* WARNING: Do nothing block with infinite loop */
      } while( true );
    }
    local_2c = local_34;
    iVar5 = FUN_0801654c(piVar6,&local_34);
    if (iVar5 != 0) {
      software_bkpt(0xff);
      do {
                    /* WARNING: Do nothing block with infinite loop */
      } while( true );
    }
  }
  else {
    iVar5 = 1;
  }
  return iVar5;
}


