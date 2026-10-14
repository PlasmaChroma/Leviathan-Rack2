; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: prologue_heuristic
08025ce8  38b5      push	{r3, r4, r5, lr}
08025cea  b2fa82f5  clz	r5, r2
08025cee  6d09      lsrs	r5, r5, #5
08025cf0  002a      cmp	r2, #0
08025cf2  53d0      beq	#166 ; -> 0x08025d9c ; branch_target=0x08025d9c
08025cf4  90f89130  ldrb.w	r3, [r0, #145]
08025cf8  0446      mov	r4, r0
08025cfa  012b      cmp	r3, #1
08025cfc  d8b2      uxtb	r0, r3
08025cfe  49d1      bne	#146 ; -> 0x08025d94 ; branch_target=0x08025d94
08025d00  94f89030  ldrb.w	r3, [r4, #144]
08025d04  012b      cmp	r3, #1
08025d06  45d0      beq	#138 ; -> 0x08025d94 ; branch_target=0x08025d94
08025d08  2b46      mov	r3, r5
08025d0a  a167      str	r1, [r4, #120]
08025d0c  2221      movs	r1, #34
08025d0e  c4f89450  str.w	r5, [r4, #148]
08025d12  62f30f03  bfi	r3, r2, #0, #16
08025d16  84f89000  strb.w	r0, [r4, #144]
08025d1a  84f89110  strb.w	r1, [r4, #145]
08025d1e  62f31f43  bfi	r3, r2, #16, #16
08025d22  d4f88420  ldr.w	r2, [r4, #132]
08025d26  2449      ldr	r1, [pc, #144] ; [0x08025db8] = 0x08025e2d
08025d28  e367      str	r3, [r4, #124]
08025d2a  244b      ldr	r3, [pc, #144] ; [0x08025dbc] = 0x08025e89
08025d2c  1364      str	r3, [r2, #64]
08025d2e  d4f88430  ldr.w	r3, [r4, #132]
08025d32  234a      ldr	r2, [pc, #140] ; [0x08025dc0] = 0x08025e95
08025d34  d963      str	r1, [r3, #60]
08025d36  d4f88430  ldr.w	r3, [r4, #132]
08025d3a  da64      str	r2, [r3, #76]
08025d3c  d4f88430  ldr.w	r3, [r4, #132]
08025d40  1d65      str	r5, [r3, #80]
08025d42  2168      ldr	r1, [r4]
08025d44  b4f87c30  ldrh.w	r3, [r4, #124]
08025d48  a26f      ldr	r2, [r4, #120]
08025d4a  1c31      adds	r1, #28
08025d4c  d4f88400  ldr.w	r0, [r4, #132]
08025d50  fbf7aeff  bl	#-16548 ; -> 0x08021cb0 ; branch_target=0x08021cb0
08025d54  00bb      cbnz	r0, #64 ; -> 0x08025d98 ; branch_target=0x08025d98
08025d56  626c      ldr	r2, [r4, #68]
08025d58  6368      ldr	r3, [r4, #4]
08025d5a  082a      cmp	r2, #8
08025d5c  20d0      beq	#64 ; -> 0x08025da0 ; branch_target=0x08025da0
08025d5e  0521      movs	r1, #5
08025d60  6122      movs	r2, #97
08025d62  023b      subs	r3, #2
08025d64  012b      cmp	r3, #1
08025d66  88bf      it	hi
08025d68  0a46      movhi	r2, r1
08025d6a  2168      ldr	r1, [r4]
08025d6c  0b69      ldr	r3, [r1, #16]
08025d6e  1343      orrs	r3, r2
08025d70  0b61      str	r3, [r1, #16]
08025d72  2268      ldr	r2, [r4]
08025d74  1368      ldr	r3, [r2]
08025d76  43f40033  orr	r3, r3, #131072
08025d7a  1360      str	r3, [r2]
08025d7c  2368      ldr	r3, [r4]
08025d7e  1a68      ldr	r2, [r3]
08025d80  d203      lsls	r2, r2, #15
08025d82  03d4      bmi	#6 ; -> 0x08025d8c ; branch_target=0x08025d8c
08025d84  1a68      ldr	r2, [r3]
08025d86  42f48032  orr	r2, r2, #65536
08025d8a  1a60      str	r2, [r3]
08025d8c  0023      movs	r3, #0
08025d8e  84f89030  strb.w	r3, [r4, #144]
08025d92  38bd      pop	{r3, r4, r5, pc}
08025d94  0220      movs	r0, #2
08025d96  38bd      pop	{r3, r4, r5, pc}
08025d98  84f89050  strb.w	r5, [r4, #144]
08025d9c  0120      movs	r0, #1
08025d9e  38bd      pop	{r3, r4, r5, pc}
08025da0  23f00202  bic	r2, r3, #2
08025da4  012a      cmp	r2, #1
08025da6  15bf      itete	ne
08025da8  0521      movne	r1, #5
08025daa  1521      moveq	r1, #21
08025dac  6122      movne	r2, #97
08025dae  7122      moveq	r2, #113
08025db0  d7e7      b	#-82 ; -> 0x08025d62 ; branch_target=0x08025d62
