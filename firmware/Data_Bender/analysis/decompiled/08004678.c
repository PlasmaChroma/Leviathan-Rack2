/* 08004678 DB_MacroBend_ChooseRateAndSlew; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DB_MacroBend_ChooseRateAndSlew(int param_1,int param_2)

{
  bool bVar1;
  bool bVar2;
  bool bVar3;
  int iVar4;
  int iVar5;
  float fVar6;
  undefined4 uVar8;
  double dVar7;
  float fVar9;
  uint uVar10;
  uint uVar11;
  
  iVar4 = newlib_rand_LCG64();
  uVar8 = 0x3fc00000;
  fVar6 = *(float *)(param_1 + 0x104);
  fVar9 = ((float)(longlong)
                  (iVar4 + (((int)((ulonglong)((longlong)DAT_08004790 * (longlong)iVar4) >> 0x20) +
                             iVar4 >> 7) - (iVar4 >> 0x1f)) * -0xff) / DAT_08004794) * fVar6 * 1.5 *
          9.0;
  uVar10 = (uint)(0.0 < fVar9) * (int)fVar9;
  if (uVar10 < 10) {
    uVar8 = *(undefined4 *)(DAT_08004798 + uVar10 * 4);
  }
  iVar4 = param_1 + param_2 * 4;
  bVar1 = fVar6 < DAT_0800479c;
  bVar2 = fVar6 != DAT_0800479c;
  bVar3 = NAN(DAT_0800479c);
  *(undefined4 *)(iVar4 + 0xa8) = uVar8;
  if (bVar2 && bVar1 == (NAN(fVar6) || bVar3)) {
    iVar5 = newlib_rand_LCG64();
    fVar6 = (float)(longlong)
                   (iVar5 + (((int)((ulonglong)((longlong)DAT_08004790 * (longlong)iVar5) >> 0x20) +
                              iVar5 >> 7) - (iVar5 >> 0x1f)) * -0xff) / DAT_08004794;
    if (fVar6 == 0.25 || fVar6 < 0.25 != NAN(fVar6)) {
      return;
    }
    uVar10 = newlib_rand_LCG64();
    dVar7 = DAT_08004788 + ((double)*(float *)(param_1 + 0x104) - DAT_08004778) * 3.0 * DAT_08004780
    ;
    uVar11 = (uint)(0.0 < dVar7) * (int)(longlong)dVar7;
    uVar10 = uVar10 - (uVar10 / uVar11) * uVar11;
    if (0x7f < uVar10) {
      uVar8 = DAT_080047a0;
      if (0xff < uVar10) {
        uVar8 = DAT_080047a4;
      }
      *(undefined4 *)(iVar4 + 0xe0) = uVar8;
      return;
    }
  }
  *(undefined4 *)(iVar4 + 0xe0) = 0x3f800000;
  return;
}


