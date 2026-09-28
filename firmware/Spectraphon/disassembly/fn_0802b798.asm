; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: prologue_heuristic
0802b798  70b5      push	{r4, r5, r6, lr}
0802b79a  0368      ldr	r3, [r0]
0802b79c  0446      mov	r4, r0
0802b79e  2bb1      cbz	r3, #10 ; -> 0x0802b7ac ; branch_target=0x0802b7ac
0802b7a0  1a78      ldrb	r2, [r3]
0802b7a2  1ab1      cbz	r2, #6 ; -> 0x0802b7ac ; branch_target=0x0802b7ac
0802b7a4  8188      ldrh	r1, [r0, #4]
0802b7a6  da88      ldrh	r2, [r3, #6]
0802b7a8  9142      cmp	r1, r2
0802b7aa  01d0      beq	#2 ; -> 0x0802b7b0 ; branch_target=0x0802b7b0
0802b7ac  0920      movs	r0, #9
0802b7ae  70bd      pop	{r4, r5, r6, pc}
0802b7b0  5878      ldrb	r0, [r3, #1]
0802b7b2  fef785f8  bl	#-7926 ; -> 0x080298c0 ; branch_target=0x080298c0
0802b7b6  c207      lsls	r2, r0, #31
0802b7b8  f8d4      bmi	#-16 ; -> 0x0802b7ac ; branch_target=0x0802b7ac
0802b7ba  237d      ldrb	r3, [r4, #20]
0802b7bc  13f04000  ands	r0, r3, #64
0802b7c0  f5d0      beq	#-22 ; -> 0x0802b7ae ; branch_target=0x0802b7ae
0802b7c2  1b06      lsls	r3, r3, #24
0802b7c4  2568      ldr	r5, [r4]
0802b7c6  2cd4      bmi	#88 ; -> 0x0802b822 ; branch_target=0x0802b822
0802b7c8  07f0e8f9  bl	#29648 ; -> 0x08032b9c ; branch_target=0x08032b9c
0802b7cc  616a      ldr	r1, [r4, #36]
0802b7ce  0646      mov	r6, r0
0802b7d0  2846      mov	r0, r5
0802b7d2  fef7f3fa  bl	#-6682 ; -> 0x08029dbc ; branch_target=0x08029dbc
0802b7d6  0028      cmp	r0, #0
0802b7d8  e9d1      bne	#-46 ; -> 0x0802b7ae ; branch_target=0x0802b7ae
0802b7da  a36a      ldr	r3, [r4, #40]
0802b7dc  da7a      ldrb	r2, [r3, #11]
0802b7de  42f02002  orr	r2, r2, #32
0802b7e2  da72      strb	r2, [r3, #11]
0802b7e4  a268      ldr	r2, [r4, #8]
0802b7e6  2168      ldr	r1, [r4]
0802b7e8  c2f30720  ubfx	r0, r2, #8, #8
0802b7ec  9a76      strb	r2, [r3, #26]
0802b7ee  d876      strb	r0, [r3, #27]
0802b7f0  0978      ldrb	r1, [r1]
0802b7f2  0329      cmp	r1, #3
0802b7f4  03d1      bne	#6 ; -> 0x0802b7fe ; branch_target=0x0802b7fe
0802b7f6  120c      lsrs	r2, r2, #16
0802b7f8  1a75      strb	r2, [r3, #20]
0802b7fa  120a      lsrs	r2, r2, #8
0802b7fc  5a75      strb	r2, [r3, #21]
0802b7fe  e168      ldr	r1, [r4, #12]
0802b800  0022      movs	r2, #0
0802b802  c3f81660  str.w	r6, [r3, #22]
0802b806  2846      mov	r0, r5
0802b808  d961      str	r1, [r3, #28]
0802b80a  5a82      strh	r2, [r3, #18]
0802b80c  0123      movs	r3, #1
0802b80e  eb70      strb	r3, [r5, #3]
0802b810  fef776fa  bl	#-6932 ; -> 0x08029d00 ; branch_target=0x08029d00
0802b814  237d      ldrb	r3, [r4, #20]
0802b816  23f04003  bic	r3, r3, #64
0802b81a  2375      strb	r3, [r4, #20]
0802b81c  70bd      pop	{r4, r5, r6, pc}
0802b822  0123      movs	r3, #1
0802b824  226a      ldr	r2, [r4, #32]
0802b826  04f13001  add.w	r1, r4, #48
0802b82a  6878      ldrb	r0, [r5, #1]
0802b82c  fef772f8  bl	#-7964 ; -> 0x08029914 ; branch_target=0x08029914
0802b830  20b9      cbnz	r0, #8 ; -> 0x0802b83c ; branch_target=0x0802b83c
0802b832  237d      ldrb	r3, [r4, #20]
0802b834  03f07f03  and	r3, r3, #127
0802b838  2375      strb	r3, [r4, #20]
0802b83a  c5e7      b	#-118 ; -> 0x0802b7c8 ; branch_target=0x0802b7c8
0802b83c  0120      movs	r0, #1
0802b83e  70bd      pop	{r4, r5, r6, pc}
