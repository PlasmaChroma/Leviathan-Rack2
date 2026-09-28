; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08025bbc  2de9f041  push.w	{r4, r5, r6, r7, r8, lr}
08025bc0  0f46      mov	r7, r1
08025bc2  0446      mov	r4, r0
08025bc4  1546      mov	r5, r2
08025bc6  faf7e9fb  bl	#-22574 ; -> 0x0802039c ; branch_target=0x0802039c
08025bca  002f      cmp	r7, #0
08025bcc  74d0      beq	#232 ; -> 0x08025cb8 ; branch_target=0x08025cb8
08025bce  b5fa85f8  clz	r8, r5
08025bd2  4fea5818  lsr.w	r8, r8, #5
08025bd6  002d      cmp	r5, #0
08025bd8  6ed0      beq	#220 ; -> 0x08025cb8 ; branch_target=0x08025cb8
08025bda  94f89130  ldrb.w	r3, [r4, #145]
08025bde  012b      cmp	r3, #1
08025be0  dab2      uxtb	r2, r3
08025be2  5ad1      bne	#180 ; -> 0x08025c9a ; branch_target=0x08025c9a
08025be4  94f89030  ldrb.w	r3, [r4, #144]
08025be8  012b      cmp	r3, #1
08025bea  56d0      beq	#172 ; -> 0x08025c9a ; branch_target=0x08025c9a
08025bec  4346      mov	r3, r8
08025bee  84f89020  strb.w	r2, [r4, #144]
08025bf2  1221      movs	r1, #18
08025bf4  d4f88020  ldr.w	r2, [r4, #128]
08025bf8  65f30f03  bfi	r3, r5, #0, #16
08025bfc  a767      str	r7, [r4, #120]
08025bfe  c4f89480  str.w	r8, [r4, #148]
08025c02  0646      mov	r6, r0
08025c04  65f31f43  bfi	r3, r5, #16, #16
08025c08  84f89110  strb.w	r1, [r4, #145]
08025c0c  e367      str	r3, [r4, #124]
08025c0e  324b      ldr	r3, [pc, #200] ; [0x08025cd8] = 0x08025e21
08025c10  1364      str	r3, [r2, #64]
08025c12  324a      ldr	r2, [pc, #200] ; [0x08025cdc] = 0x08025dc5
08025c14  d4f88030  ldr.w	r3, [r4, #128]
08025c18  da63      str	r2, [r3, #60]
08025c1a  314a      ldr	r2, [pc, #196] ; [0x08025ce0] = 0x08025e95
08025c1c  d4f88030  ldr.w	r3, [r4, #128]
08025c20  da64      str	r2, [r3, #76]
08025c22  d4f88030  ldr.w	r3, [r4, #128]
08025c26  c3f85080  str.w	r8, [r3, #80]
08025c2a  2268      ldr	r2, [r4]
08025c2c  b4f87c30  ldrh.w	r3, [r4, #124]
08025c30  1c32      adds	r2, #28
08025c32  a16f      ldr	r1, [r4, #120]
08025c34  d4f88000  ldr.w	r0, [r4, #128]
08025c38  fcf73af8  bl	#-16268 ; -> 0x08021cb0 ; branch_target=0x08021cb0
08025c3c  0746      mov	r7, r0
08025c3e  0028      cmp	r0, #0
08025c40  38d1      bne	#112 ; -> 0x08025cb4 ; branch_target=0x08025cb4
08025c42  626c      ldr	r2, [r4, #68]
08025c44  6368      ldr	r3, [r4, #4]
08025c46  082a      cmp	r2, #8
08025c48  2bd0      beq	#86 ; -> 0x08025ca2 ; branch_target=0x08025ca2
08025c4a  0521      movs	r1, #5
08025c4c  6122      movs	r2, #97
08025c4e  023b      subs	r3, #2
08025c50  012b      cmp	r3, #1
08025c52  88bf      it	hi
08025c54  0a46      movhi	r2, r1
08025c56  2168      ldr	r1, [r4]
08025c58  0b69      ldr	r3, [r1, #16]
08025c5a  1343      orrs	r3, r2
08025c5c  0b61      str	r3, [r1, #16]
08025c5e  2268      ldr	r2, [r4]
08025c60  1368      ldr	r3, [r2]
08025c62  43f40033  orr	r3, r3, #131072
08025c66  1360      str	r3, [r2]
08025c68  05e0      b	#10 ; -> 0x08025c76 ; branch_target=0x08025c76
08025c6a  faf797fb  bl	#-22738 ; -> 0x0802039c ; branch_target=0x0802039c
08025c6e  831b      subs	r3, r0, r6
08025c70  b3f57a7f  cmp.w	r3, #1000
08025c74  24d8      bhi	#72 ; -> 0x08025cc0 ; branch_target=0x08025cc0
08025c76  2268      ldr	r2, [r4]
08025c78  5569      ldr	r5, [r2, #20]
08025c7a  15f4e025  ands	r5, r5, #458752
08025c7e  f4d0      beq	#-24 ; -> 0x08025c6a ; branch_target=0x08025c6a
08025c80  1368      ldr	r3, [r2]
08025c82  db03      lsls	r3, r3, #15
08025c84  03d4      bmi	#6 ; -> 0x08025c8e ; branch_target=0x08025c8e
08025c86  1368      ldr	r3, [r2]
08025c88  43f48033  orr	r3, r3, #65536
08025c8c  1360      str	r3, [r2]
08025c8e  0023      movs	r3, #0
08025c90  3846      mov	r0, r7
08025c92  84f89030  strb.w	r3, [r4, #144]
08025c96  bde8f081  pop.w	{r4, r5, r6, r7, r8, pc}
08025c9a  0227      movs	r7, #2
08025c9c  3846      mov	r0, r7
08025c9e  bde8f081  pop.w	{r4, r5, r6, r7, r8, pc}
08025ca2  23f00202  bic	r2, r3, #2
08025ca6  012a      cmp	r2, #1
08025ca8  15bf      itete	ne
08025caa  0521      movne	r1, #5
08025cac  1521      moveq	r1, #21
08025cae  6122      movne	r2, #97
08025cb0  7122      moveq	r2, #113
08025cb2  cce7      b	#-104 ; -> 0x08025c4e ; branch_target=0x08025c4e
08025cb4  84f89080  strb.w	r8, [r4, #144]
08025cb8  0127      movs	r7, #1
08025cba  3846      mov	r0, r7
08025cbc  bde8f081  pop.w	{r4, r5, r6, r7, r8, pc}
08025cc0  d4f89430  ldr.w	r3, [r4, #148]
08025cc4  0327      movs	r7, #3
08025cc6  84f89050  strb.w	r5, [r4, #144]
08025cca  43f04003  orr	r3, r3, #64
08025cce  3846      mov	r0, r7
08025cd0  c4f89430  str.w	r3, [r4, #148]
08025cd4  bde8f081  pop.w	{r4, r5, r6, r7, r8, pc}
