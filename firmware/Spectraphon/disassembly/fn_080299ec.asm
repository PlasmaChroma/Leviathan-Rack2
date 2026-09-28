; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
080299ec  2de9f843  push.w	{r3, r4, r5, r6, r7, r8, r9, lr}
080299f0  0a44      add	r2, r1
080299f2  0c46      mov	r4, r1
080299f4  0746      mov	r7, r0
080299f6  364d      ldr	r5, [pc, #216] ; [0x08029ad0] = 0x080365f4
080299f8  9442      cmp	r4, r2
080299fa  0ed0      beq	#28 ; -> 0x08029a1a ; branch_target=0x08029a1a
080299fc  14f8010b  ldrb	r0, [r4], #1
08029a00  a0f16101  sub.w	r1, r0, #97
08029a04  1929      cmp	r1, #25
08029a06  f7d9      bls	#-18 ; -> 0x080299f8 ; branch_target=0x080299f8
08029a08  2918      adds	r1, r5, r0
08029a0a  7f28      cmp	r0, #127
08029a0c  88bf      it	hi
08029a0e  11f8800c  ldrbhi	r0, [r1, #-128]
08029a12  0028      cmp	r0, #0
08029a14  f0d1      bne	#-32 ; -> 0x080299f8 ; branch_target=0x080299f8
08029a16  bde8f883  pop.w	{r3, r4, r5, r6, r7, r8, r9, pc}
08029a1a  1e1e      subs	r6, r3, #0
08029a1c  97f80080  ldrb.w	r8, [r7]
08029a20  18bf      it	ne
08029a22  0126      movne	r6, #1
08029a24  b8f1000f  cmp.w	r8, #0
08029a28  00d1      bne	#0 ; -> 0x08029a2c ; branch_target=0x08029a2c
08029a2a  76bb      cbnz	r6, #92 ; -> 0x08029a8a ; branch_target=0x08029a8a
08029a2c  284d      ldr	r5, [pc, #160] ; [0x08029ad0] = 0x080365f4
08029a2e  3946      mov	r1, r7
08029a30  4346      mov	r3, r8
08029a32  a146      mov	r9, r4
08029a34  0de0      b	#26 ; -> 0x08029a52 ; branch_target=0x08029a52
08029a36  203b      subs	r3, #32
08029a38  9bb2      uxth	r3, r3
08029a3a  19f8012b  ldrb	r2, [r9], #1
08029a3e  a2f16100  sub.w	r0, r2, #97
08029a42  1928      cmp	r0, #25
08029a44  18d8      bhi	#48 ; -> 0x08029a78 ; branch_target=0x08029a78
08029a46  203a      subs	r2, #32
08029a48  92b2      uxth	r2, r2
08029a4a  9a42      cmp	r2, r3
08029a4c  39d1      bne	#114 ; -> 0x08029ac2 ; branch_target=0x08029ac2
08029a4e  11f8013f  ldrb	r3, [r1, #1]!
08029a52  3f2b      cmp	r3, #63
08029a54  1cd0      beq	#56 ; -> 0x08029a90 ; branch_target=0x08029a90
08029a56  2a2b      cmp	r3, #42
08029a58  1ad0      beq	#52 ; -> 0x08029a90 ; branch_target=0x08029a90
08029a5a  a3f16102  sub.w	r2, r3, #97
08029a5e  192a      cmp	r2, #25
08029a60  e9d9      bls	#-46 ; -> 0x08029a36 ; branch_target=0x08029a36
08029a62  7f2b      cmp	r3, #127
08029a64  e9d9      bls	#-46 ; -> 0x08029a3a ; branch_target=0x08029a3a
08029a66  19f8012b  ldrb	r2, [r9], #1
08029a6a  2b44      add	r3, r5
08029a6c  a2f16100  sub.w	r0, r2, #97
08029a70  13f8803c  ldrb	r3, [r3, #-128]
08029a74  1928      cmp	r0, #25
08029a76  e6d9      bls	#-52 ; -> 0x08029a46 ; branch_target=0x08029a46
08029a78  7f2a      cmp	r2, #127
08029a7a  02d9      bls	#4 ; -> 0x08029a82 ; branch_target=0x08029a82
08029a7c  2a44      add	r2, r5
08029a7e  12f8802c  ldrb	r2, [r2, #-128]
08029a82  9a42      cmp	r2, r3
08029a84  1dd1      bne	#58 ; -> 0x08029ac2 ; branch_target=0x08029ac2
08029a86  002a      cmp	r2, #0
08029a88  e1d1      bne	#-62 ; -> 0x08029a4e ; branch_target=0x08029a4e
08029a8a  0120      movs	r0, #1
08029a8c  bde8f883  pop.w	{r3, r4, r5, r6, r7, r8, r9, pc}
08029a90  0023      movs	r3, #0
08029a92  11f801cb  ldrb	r12, [r1], #1
08029a96  1a46      mov	r2, r3
08029a98  bcf13f0f  cmp.w	r12, #63
08029a9c  0846      mov	r0, r1
08029a9e  11f801cb  ldrb	r12, [r1], #1
08029aa2  0cbf      ite	eq
08029aa4  0132      addeq	r2, #1
08029aa6  0123      movne	r3, #1
08029aa8  bcf13f0f  cmp.w	r12, #63
08029aac  f4d0      beq	#-24 ; -> 0x08029a98 ; branch_target=0x08029a98
08029aae  bcf12a0f  cmp.w	r12, #42
08029ab2  f1d0      beq	#-30 ; -> 0x08029a98 ; branch_target=0x08029a98
08029ab4  4946      mov	r1, r9
08029ab6  fff799ff  bl	#-206 ; -> 0x080299ec ; branch_target=0x080299ec
08029aba  0028      cmp	r0, #0
08029abc  e5d1      bne	#-54 ; -> 0x08029a8a ; branch_target=0x08029a8a
08029abe  99f80020  ldrb.w	r2, [r9]
08029ac2  0134      adds	r4, #1
08029ac4  0ab1      cbz	r2, #2 ; -> 0x08029aca ; branch_target=0x08029aca
08029ac6  002e      cmp	r6, #0
08029ac8  b1d1      bne	#-158 ; -> 0x08029a2e ; branch_target=0x08029a2e
08029aca  0020      movs	r0, #0
08029acc  a3e7      b	#-186 ; -> 0x08029a16 ; branch_target=0x08029a16
