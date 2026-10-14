; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08022bc4  70b5      push	{r4, r5, r6, lr}
08022bc6  0a7a      ldrb	r2, [r1, #8]
08022bc8  0346      mov	r3, r0
08022bca  012a      cmp	r2, #1
08022bcc  2ed0      beq	#92 ; -> 0x08022c2c ; branch_target=0x08022c2c
08022bce  0024      movs	r4, #0
08022bd0  2546      mov	r5, r4
08022bd2  93f83520  ldrb.w	r2, [r3, #53]
08022bd6  012a      cmp	r2, #1
08022bd8  d0b2      uxtb	r0, r2
08022bda  22d1      bne	#68 ; -> 0x08022c22 ; branch_target=0x08022c22
08022bdc  93f83420  ldrb.w	r2, [r3, #52]
08022be0  012a      cmp	r2, #1
08022be2  26d0      beq	#76 ; -> 0x08022c32 ; branch_target=0x08022c32
08022be4  83f83400  strb.w	r0, [r3, #52]
08022be8  4ff0000c  mov.w	r12, #0
08022bec  186e      ldr	r0, [r3, #96]
08022bee  0268      ldr	r2, [r0]
08022bf0  22f48132  bic	r2, r2, #66048
08022bf4  0260      str	r2, [r0]
08022bf6  ca68      ldr	r2, [r1, #12]
08022bf8  1e6e      ldr	r6, [r3, #96]
08022bfa  501e      subs	r0, r2, #1
08022bfc  0a7a      ldrb	r2, [r1, #8]
08022bfe  91f809e0  ldrb.w	lr, [r1, #9]
08022c02  1204      lsls	r2, r2, #16
08022c04  3168      ldr	r1, [r6]
08022c06  42eac042  orr.w	r2, r2, r0, lsl #19
08022c0a  6046      mov	r0, r12
08022c0c  c9b2      uxtb	r1, r1
08022c0e  0a43      orrs	r2, r1
08022c10  42ea4e22  orr.w	r2, r2, lr, lsl #9
08022c14  2243      orrs	r2, r4
08022c16  42ea0562  orr.w	r2, r2, r5, lsl #24
08022c1a  3260      str	r2, [r6]
08022c1c  83f834c0  strb.w	r12, [r3, #52]
08022c20  70bd      pop	{r4, r5, r6, pc}
08022c22  4ff40062  mov.w	r2, #2048
08022c26  0120      movs	r0, #1
08022c28  5a65      str	r2, [r3, #84]
08022c2a  70bd      pop	{r4, r5, r6, pc}
08022c2c  d1e90054  ldrd	r5, r4, [r1]
08022c30  cfe7      b	#-98 ; -> 0x08022bd2 ; branch_target=0x08022bd2
08022c32  0220      movs	r0, #2
08022c34  70bd      pop	{r4, r5, r6, pc}
