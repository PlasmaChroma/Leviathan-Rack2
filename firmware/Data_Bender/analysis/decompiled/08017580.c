/* 08017580 DaisySP_Decimator_Process; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DaisySP_Decimator_Process(float param_1,int param_2)

{
  uint uVar1;
  undefined4 uVar2;
  float fVar3;
  uint uVar4;
  
  uVar1 = *(int *)(param_2 + 0x18) + 1;
  fVar3 = *(float *)(param_2 + 4) * *(float *)(param_2 + 4) * DAT_0801760c;
  *(uint *)(param_2 + 0x18) = uVar1;
  uVar4 = (uint)(0.0 < fVar3) * (int)fVar3;
  *(uint *)(param_2 + 0x1c) = uVar4;
  if (uVar4 < uVar1) {
    *(float *)(param_2 + 0x10) = param_1;
    *(undefined4 *)(param_2 + 0x18) = 0;
  }
  else {
    param_1 = *(float *)(param_2 + 0x10);
  }
  uVar1 = *(uint *)(param_2 + 0xc);
  if (*(char *)(param_2 + 0x20) != '\0') {
    *(float *)(param_2 + 0x14) =
         (float)(longlong)
                (((int)(*(float *)(param_2 + 0x24) * param_1 * DAT_08017610) >> (uVar1 + 1 & 0xff))
                << (uVar1 + 1 & 0xff)) / (*(float *)(param_2 + 0x24) * DAT_08017610);
    return;
  }
  uVar2 = FixedToFP(((int)(param_1 * DAT_08017610) >> (uVar1 & 0xff)) << (uVar1 & 0xff),0x20,0x20,
                    0x10,0,0);
  *(undefined4 *)(param_2 + 0x14) = uVar2;
  return;
}


