/* 080002e0 FUN_080002e0; analyst naming is provisional. */

byte * FUN_080002e0(uint *param_1,uint param_2,uint param_3)

{
  byte bVar1;
  char cVar2;
  char cVar3;
  char cVar4;
  char cVar5;
  byte *pbVar6;
  uint *puVar7;
  byte *pbVar8;
  uint uVar9;
  uint uVar10;
  uint uVar11;
  uint uVar12;
  bool bVar13;
  bool bVar14;
  bool bVar15;
  bool bVar16;
  
  param_2 = param_2 & 0xff;
  if ((int)param_3 < 0x10) {
joined_r0x08000340:
    do {
      if (param_3 == 0) {
        return (byte *)0x0;
      }
      puVar7 = (uint *)((int)param_1 + 1);
      bVar1 = *(byte *)param_1;
      param_3 = param_3 - 1;
      param_1 = puVar7;
    } while (bVar1 != param_2);
  }
  else {
    uVar9 = (uint)param_1 & 7;
    while( true ) {
      if (uVar9 == 0) {
        uVar9 = param_2 | param_2 << 8;
        uVar9 = uVar9 | uVar9 << 0x10;
        uVar10 = param_3 & 0xfffffff8;
        do {
          puVar7 = param_1 + 2;
          uVar10 = uVar10 - 8;
          uVar11 = *param_1 ^ uVar9;
          uVar12 = param_1[1] ^ uVar9;
          cVar2 = -((char)uVar11 == '\0');
          cVar3 = -((char)(uVar11 >> 8) == '\0');
          cVar4 = -((char)(uVar11 >> 0x10) == '\0');
          cVar5 = -((char)(uVar11 >> 0x18) == '\0');
          uVar11 = CONCAT13(cVar5,CONCAT12(cVar4,CONCAT11(cVar3,cVar2)));
          bVar13 = (char)uVar12 != '\0';
          bVar14 = (char)(uVar12 >> 8) != '\0';
          bVar15 = (char)(uVar12 >> 0x10) != '\0';
          bVar16 = (char)(uVar12 >> 0x18) != '\0';
          uVar12 = CONCAT13(bVar16 * cVar5 - !bVar16,
                            CONCAT12(bVar15 * cVar4 - !bVar15,
                                     CONCAT11(bVar14 * cVar3 - !bVar14,bVar13 * cVar2 - !bVar13)));
          if (uVar12 != 0) {
            if (uVar11 == 0) {
              pbVar8 = (byte *)((int)param_1 + 5);
              uVar11 = uVar12;
            }
            else {
              pbVar8 = (byte *)((int)param_1 + 1);
            }
            if ((uVar11 & 1) == 0) {
              bVar13 = (uVar11 & 0x100) == 0;
              pbVar6 = pbVar8 + 1;
              if (bVar13) {
                bVar13 = (uVar11 & 0x18000) == 0;
                pbVar6 = pbVar8 + 2;
              }
              pbVar8 = pbVar6;
              if (bVar13) {
                pbVar8 = pbVar8 + 1;
              }
            }
            return pbVar8 + -1;
          }
          param_1 = puVar7;
        } while (uVar10 != 0);
        param_3 = param_3 & 7;
        goto joined_r0x08000340;
      }
      puVar7 = (uint *)((int)param_1 + 1);
      param_3 = param_3 - 1;
      if (*(byte *)param_1 == param_2) break;
      uVar9 = (uint)puVar7 & 7;
      param_1 = puVar7;
      if (param_3 == 0) {
        return (byte *)0x0;
      }
    }
  }
  return (byte *)((int)puVar7 + -1);
}


