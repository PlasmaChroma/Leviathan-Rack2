/* 08014660 FUN_08014660; analyst naming is provisional. */

void FUN_08014660(code **param_1)

{
  code *pcVar1;
  code *pcVar2;
  code *pcVar3;
  code *pcVar4;
  
  pcVar1 = param_1[3];
  pcVar2 = param_1[4];
  pcVar3 = pcVar1 + -(*(uint *)(DAT_080146e0 + 0x8c) & 0xffff);
  if ((*param_1 != (code *)0x0) && (pcVar2 != pcVar3)) {
    pcVar4 = param_1[2];
    if (pcVar2 < pcVar3) {
      FUN_08015470();
      (**param_1)(pcVar4 + (int)pcVar2,(int)pcVar3 - (int)pcVar2,param_1[1],0);
      pcVar1 = param_1[3];
      param_1[4] = pcVar3;
    }
    else {
      FUN_08015470();
      (**param_1)(pcVar4 + (int)pcVar2,(int)pcVar1 - (int)pcVar2,param_1[1],0);
      FUN_08015470(pcVar4,pcVar3);
      (**param_1)(pcVar4,pcVar3,param_1[1],0);
      pcVar1 = param_1[3];
      param_1[4] = pcVar3;
    }
    if (pcVar1 == pcVar3) {
      param_1[4] = (code *)0x0;
      return;
    }
  }
  return;
}


