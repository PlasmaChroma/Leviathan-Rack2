/* 08004bf8 DB_MacroBreak_ChooseRepeatSilencePosition; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DB_MacroBreak_ChooseRepeatSilencePosition(int param_1,int param_2)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  
  if ((*(int *)(param_1 + 0x68) == 1) && (param_2 == 1)) {
    *(undefined4 *)(param_1 + 0x74) = *(undefined4 *)(param_1 + 0x70);
    *(undefined4 *)(param_1 + 0x90) = *(undefined4 *)(param_1 + 0x8c);
    *(undefined4 *)(param_1 + 200) = *(undefined4 *)(param_1 + 0xc4);
    return;
  }
  fVar6 = *(float *)(param_1 + 0x108);
  fVar5 = DAT_08004d44;
  if (fVar6 != DAT_08004d40 && fVar6 < DAT_08004d40 == (NAN(fVar6) || NAN(DAT_08004d40))) {
    iVar3 = newlib_rand_LCG64();
    fVar6 = *(float *)(param_1 + 0x108);
    fVar5 = ((float)(longlong)
                    (iVar3 + (((int)((ulonglong)((longlong)DAT_08004d4c * (longlong)iVar3) >> 0x20)
                               + iVar3 >> 7) - (iVar3 >> 0x1f)) * -0xff) / DAT_08004d50) *
            fVar6 * 8.0;
    fVar5 = (float)(ulonglong)((uint)(0.0 < fVar5) * (int)fVar5);
  }
  iVar3 = param_1 + param_2 * 4;
  *(float *)(iVar3 + 0x70) = fVar5;
  if (fVar6 == 0.5 || fVar6 < 0.5 != NAN(fVar6)) {
    uVar2 = 0;
  }
  else {
    iVar1 = newlib_rand_LCG64();
    fVar6 = *(float *)(param_1 + 0x108);
    uVar2 = FPMaxNum(fVar6 * 2.0 + -1.0,DAT_08004d44);
    fVar5 = (float)FPMinNum(uVar2,0x3f800000);
    fVar5 = (float)FPRoundInt(fVar5 * 4.0,0x20,4,0);
    fVar5 = ((float)(longlong)
                    (iVar1 + (((int)((ulonglong)((longlong)DAT_08004d4c * (longlong)iVar1) >> 0x20)
                               + iVar1 >> 7) - (iVar1 >> 0x1f)) * -0xff) / DAT_08004d50) * fVar5;
    uVar2 = *(undefined4 *)(DAT_08004d54 + (uint)(0.0 < fVar5) * (int)fVar5 * 4);
  }
  fVar5 = DAT_08004d48;
  *(undefined4 *)(iVar3 + 0x8c) = uVar2;
  fVar4 = DAT_08004d44;
  if (fVar6 != fVar5 && fVar6 < fVar5 == (NAN(fVar6) || NAN(fVar5))) {
    iVar1 = newlib_rand_LCG64();
    fVar4 = (float)(longlong)
                   (iVar1 + (((int)((ulonglong)((longlong)DAT_08004d4c * (longlong)iVar1) >> 0x20) +
                              iVar1 >> 7) - (iVar1 >> 0x1f)) * -0xff) / DAT_08004d50;
  }
  *(float *)(iVar3 + 0xc4) = fVar4;
  return;
}


