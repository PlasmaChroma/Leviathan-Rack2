; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08020c1c  2de9f041  push.w	{r4, r5, r6, r7, r8, lr}
08020c20  3d4b      ldr	r3, [pc, #244] ; [0x08020d18] = 0x40022000 / f32_bits_interpretation=2.033203125
08020c22  0446      mov	r4, r0
08020c24  0068      ldr	r0, [r0]
08020c26  0e46      mov	r6, r1
08020c28  1746      mov	r7, r2
08020c2a  9842      cmp	r0, r3
08020c2c  61d0      beq	#194 ; -> 0x08020cf2 ; branch_target=0x08020cf2
08020c2e  03f58073  add.w	r3, r3, #256
08020c32  9842      cmp	r0, r3
08020c34  5dd0      beq	#186 ; -> 0x08020cf2 ; branch_target=0x08020cf2
08020c36  394b      ldr	r3, [pc, #228] ; [0x08020d1c] = 0x58026300
08020c38  9b68      ldr	r3, [r3, #8]
08020c3a  8568      ldr	r5, [r0, #8]
08020c3c  15f00405  ands	r5, r5, #4
08020c40  5dd1      bne	#186 ; -> 0x08020cfe ; branch_target=0x08020cfe
08020c42  94f85020  ldrb.w	r2, [r4, #80]
08020c46  012a      cmp	r2, #1
08020c48  59d0      beq	#178 ; -> 0x08020cfe ; branch_target=0x08020cfe
08020c4a  03f01f08  and	r8, r3, #31
08020c4e  40f22123  movw	r3, #545
08020c52  0120      movs	r0, #1
08020c54  23fa08f3  lsr.w	r3, r3, r8
08020c58  84f85000  strb.w	r0, [r4, #80]
08020c5c  0340      ands	r3, r0
08020c5e  55d0      beq	#170 ; -> 0x08020d0c ; branch_target=0x08020d0c
08020c60  2046      mov	r0, r4
08020c62  fff783ff  bl	#-250 ; -> 0x08020b6c ; branch_target=0x08020b6c
08020c66  0028      cmp	r0, #0
08020c68  4cd1      bne	#152 ; -> 0x08020d04 ; branch_target=0x08020d04
08020c6a  626d      ldr	r2, [r4, #84]
08020c6c  2c4b      ldr	r3, [pc, #176] ; [0x08020d20] = 0xfffff0fe
08020c6e  1340      ands	r3, r2
08020c70  2268      ldr	r2, [r4]
08020c72  43f48073  orr	r3, r3, #256
08020c76  6365      str	r3, [r4, #84]
08020c78  2a4b      ldr	r3, [pc, #168] ; [0x08020d24] = 0x40022100 / f32_bits_interpretation=2.03326416
08020c7a  9a42      cmp	r2, r3
08020c7c  02d1      bne	#4 ; -> 0x08020c84 ; branch_target=0x08020c84
08020c7e  b8f1000f  cmp.w	r8, #0
08020c82  03d1      bne	#6 ; -> 0x08020c8c ; branch_target=0x08020c8c
08020c84  636d      ldr	r3, [r4, #84]
08020c86  23f48013  bic	r3, r3, #1048576
08020c8a  6365      str	r3, [r4, #84]
08020c8c  636d      ldr	r3, [r4, #84]
08020c8e  13f48053  ands	r3, r3, #4096
08020c92  3fd0      beq	#126 ; -> 0x08020d14 ; branch_target=0x08020d14
08020c94  a36d      ldr	r3, [r4, #88]
08020c96  23f00603  bic	r3, r3, #6
08020c9a  a365      str	r3, [r4, #88]
08020c9c  e16c      ldr	r1, [r4, #76]
08020c9e  3b46      mov	r3, r7
08020ca0  2148      ldr	r0, [pc, #132] ; [0x08020d28] = 0x0802067d
08020ca2  3246      mov	r2, r6
08020ca4  c863      str	r0, [r1, #60]
08020ca6  2148      ldr	r0, [pc, #132] ; [0x08020d2c] = 0x080203ed
08020ca8  e16c      ldr	r1, [r4, #76]
08020caa  0864      str	r0, [r1, #64]
08020cac  2048      ldr	r0, [pc, #128] ; [0x08020d30] = 0x080206e9
08020cae  e16c      ldr	r1, [r4, #76]
08020cb0  c864      str	r0, [r1, #76]
08020cb2  1c20      movs	r0, #28
08020cb4  2168      ldr	r1, [r4]
08020cb6  0860      str	r0, [r1]
08020cb8  0021      movs	r1, #0
08020cba  2068      ldr	r0, [r4]
08020cbc  84f85010  strb.w	r1, [r4, #80]
08020cc0  4168      ldr	r1, [r0, #4]
08020cc2  41f01001  orr	r1, r1, #16
08020cc6  4160      str	r1, [r0, #4]
08020cc8  2068      ldr	r0, [r4]
08020cca  e56a      ldr	r5, [r4, #44]
08020ccc  c168      ldr	r1, [r0, #12]
08020cce  21f00301  bic	r1, r1, #3
08020cd2  2943      orrs	r1, r5
08020cd4  c160      str	r1, [r0, #12]
08020cd6  2168      ldr	r1, [r4]
08020cd8  e06c      ldr	r0, [r4, #76]
08020cda  4031      adds	r1, #64
08020cdc  00f0e8ff  bl	#4048 ; -> 0x08021cb0 ; branch_target=0x08021cb0
08020ce0  2268      ldr	r2, [r4]
08020ce2  144b      ldr	r3, [pc, #80] ; [0x08020d34] = 0x7fffffc0
08020ce4  9168      ldr	r1, [r2, #8]
08020ce6  0b40      ands	r3, r1
08020ce8  43f00403  orr	r3, r3, #4
08020cec  9360      str	r3, [r2, #8]
08020cee  bde8f081  pop.w	{r4, r5, r6, r7, r8, pc}
08020cf2  114b      ldr	r3, [pc, #68] ; [0x08020d38] = 0x40022300 / f32_bits_interpretation=2.03338623
08020cf4  9b68      ldr	r3, [r3, #8]
08020cf6  8568      ldr	r5, [r0, #8]
08020cf8  15f00405  ands	r5, r5, #4
08020cfc  a1d0      beq	#-190 ; -> 0x08020c42 ; branch_target=0x08020c42
08020cfe  0220      movs	r0, #2
08020d00  bde8f081  pop.w	{r4, r5, r6, r7, r8, pc}
08020d04  84f85050  strb.w	r5, [r4, #80]
08020d08  bde8f081  pop.w	{r4, r5, r6, r7, r8, pc}
08020d0c  84f85030  strb.w	r3, [r4, #80]
08020d10  bde8f081  pop.w	{r4, r5, r6, r7, r8, pc}
08020d14  a365      str	r3, [r4, #88]
08020d16  c1e7      b	#-126 ; -> 0x08020c9c ; branch_target=0x08020c9c
