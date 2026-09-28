; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08029c7c  70b5      push	{r4, r5, r6, lr}
08029c7e  4ff0ff36  mov.w	r6, #4294967295
08029c82  0023      movs	r3, #0
08029c84  0446      mov	r4, r0
08029c86  b142      cmp	r1, r6
08029c88  c370      strb	r3, [r0, #3]
08029c8a  c662      str	r6, [r0, #44]
08029c8c  22d1      bne	#68 ; -> 0x08029cd4 ; branch_target=0x08029cd4
08029c8e  4af65523  movw	r3, #43605
08029c92  b4f82e22  ldrh.w	r2, [r4, #558]
08029c96  9a42      cmp	r2, r3
08029c98  1ad1      bne	#52 ; -> 0x08029cd0 ; branch_target=0x08029cd0
08029c9a  94f83030  ldrb.w	r3, [r4, #48]
08029c9e  e92b      cmp	r3, #233
08029ca0  07d0      beq	#14 ; -> 0x08029cb2 ; branch_target=0x08029cb2
08029ca2  236b      ldr	r3, [r4, #48]
08029ca4  134a      ldr	r2, [pc, #76] ; [0x08029cf4] = 0x009000eb
08029ca6  03f0ff13  and	r3, r3, #16711935
08029caa  9342      cmp	r3, r2
08029cac  01d0      beq	#2 ; -> 0x08029cb2 ; branch_target=0x08029cb2
08029cae  0220      movs	r0, #2
08029cb0  70bd      pop	{r4, r5, r6, pc}
08029cb2  d4f86630  ldr.w	r3, [r4, #102]
08029cb6  104a      ldr	r2, [pc, #64] ; [0x08029cf8] = 0x00544146
08029cb8  23f07f43  bic	r3, r3, #4278190080
08029cbc  9342      cmp	r3, r2
08029cbe  17d0      beq	#46 ; -> 0x08029cf0 ; branch_target=0x08029cf0
08029cc0  d4f88200  ldr.w	r0, [r4, #130]
08029cc4  0d4b      ldr	r3, [pc, #52] ; [0x08029cfc] = 0x33544146 / f32_bits_interpretation=4.94194623e-08
08029cc6  c01a      subs	r0, r0, r3
08029cc8  18bf      it	ne
08029cca  0120      movne	r0, #1
08029ccc  4000      lsls	r0, r0, #1
08029cce  70bd      pop	{r4, r5, r6, pc}
08029cd0  0320      movs	r0, #3
08029cd2  70bd      pop	{r4, r5, r6, pc}
08029cd4  0d46      mov	r5, r1
08029cd6  0a46      mov	r2, r1
08029cd8  0123      movs	r3, #1
08029cda  00f13001  add.w	r1, r0, #48
08029cde  4078      ldrb	r0, [r0, #1]
08029ce0  fff70afe  bl	#-1004 ; -> 0x080298f8 ; branch_target=0x080298f8
08029ce4  10b1      cbz	r0, #4 ; -> 0x08029cec ; branch_target=0x08029cec
08029ce6  0420      movs	r0, #4
08029ce8  e662      str	r6, [r4, #44]
08029cea  70bd      pop	{r4, r5, r6, pc}
08029cec  e562      str	r5, [r4, #44]
08029cee  cee7      b	#-100 ; -> 0x08029c8e ; branch_target=0x08029c8e
08029cf0  0020      movs	r0, #0
08029cf2  70bd      pop	{r4, r5, r6, pc}
