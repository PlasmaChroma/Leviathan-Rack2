; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
0802b840  58b1      cbz	r0, #22 ; -> 0x0802b85a ; branch_target=0x0802b85a
0802b842  70b5      push	{r4, r5, r6, lr}
0802b844  0368      ldr	r3, [r0]
0802b846  0446      mov	r4, r0
0802b848  2bb1      cbz	r3, #10 ; -> 0x0802b856 ; branch_target=0x0802b856
0802b84a  1a78      ldrb	r2, [r3]
0802b84c  1ab1      cbz	r2, #6 ; -> 0x0802b856 ; branch_target=0x0802b856
0802b84e  8188      ldrh	r1, [r0, #4]
0802b850  da88      ldrh	r2, [r3, #6]
0802b852  9142      cmp	r1, r2
0802b854  03d0      beq	#6 ; -> 0x0802b85e ; branch_target=0x0802b85e
0802b856  0920      movs	r0, #9
0802b858  70bd      pop	{r4, r5, r6, pc}
0802b85a  0920      movs	r0, #9
0802b85c  7047      bx	lr
0802b85e  5878      ldrb	r0, [r3, #1]
0802b860  fef72ef8  bl	#-8100 ; -> 0x080298c0 ; branch_target=0x080298c0
0802b864  c107      lsls	r1, r0, #31
0802b866  f6d4      bmi	#-20 ; -> 0x0802b856 ; branch_target=0x0802b856
0802b868  237d      ldrb	r3, [r4, #20]
0802b86a  2568      ldr	r5, [r4]
0802b86c  5a06      lsls	r2, r3, #25
0802b86e  2ed5      bpl	#92 ; -> 0x0802b8ce ; branch_target=0x0802b8ce
0802b870  1b06      lsls	r3, r3, #24
0802b872  51d4      bmi	#162 ; -> 0x0802b918 ; branch_target=0x0802b918
0802b874  07f092f9  bl	#29476 ; -> 0x08032b9c ; branch_target=0x08032b9c
0802b878  616a      ldr	r1, [r4, #36]
0802b87a  0646      mov	r6, r0
0802b87c  2846      mov	r0, r5
0802b87e  fef79dfa  bl	#-6854 ; -> 0x08029dbc ; branch_target=0x08029dbc
0802b882  0028      cmp	r0, #0
0802b884  e8d1      bne	#-48 ; -> 0x0802b858 ; branch_target=0x0802b858
0802b886  a36a      ldr	r3, [r4, #40]
0802b888  da7a      ldrb	r2, [r3, #11]
0802b88a  42f02002  orr	r2, r2, #32
0802b88e  da72      strb	r2, [r3, #11]
0802b890  a268      ldr	r2, [r4, #8]
0802b892  2168      ldr	r1, [r4]
0802b894  c2f30720  ubfx	r0, r2, #8, #8
0802b898  9a76      strb	r2, [r3, #26]
0802b89a  d876      strb	r0, [r3, #27]
0802b89c  0978      ldrb	r1, [r1]
0802b89e  0329      cmp	r1, #3
0802b8a0  03d1      bne	#6 ; -> 0x0802b8aa ; branch_target=0x0802b8aa
0802b8a2  120c      lsrs	r2, r2, #16
0802b8a4  1a75      strb	r2, [r3, #20]
0802b8a6  120a      lsrs	r2, r2, #8
0802b8a8  5a75      strb	r2, [r3, #21]
0802b8aa  e168      ldr	r1, [r4, #12]
0802b8ac  0022      movs	r2, #0
0802b8ae  c3f81660  str.w	r6, [r3, #22]
0802b8b2  2846      mov	r0, r5
0802b8b4  d961      str	r1, [r3, #28]
0802b8b6  5a82      strh	r2, [r3, #18]
0802b8b8  0123      movs	r3, #1
0802b8ba  eb70      strb	r3, [r5, #3]
0802b8bc  fef720fa  bl	#-7104 ; -> 0x08029d00 ; branch_target=0x08029d00
0802b8c0  237d      ldrb	r3, [r4, #20]
0802b8c2  23f04003  bic	r3, r3, #64
0802b8c6  2375      strb	r3, [r4, #20]
0802b8c8  0028      cmp	r0, #0
0802b8ca  c5d1      bne	#-118 ; -> 0x0802b858 ; branch_target=0x0802b858
0802b8cc  2568      ldr	r5, [r4]
0802b8ce  002d      cmp	r5, #0
0802b8d0  c1d0      beq	#-126 ; -> 0x0802b856 ; branch_target=0x0802b856
0802b8d2  2b78      ldrb	r3, [r5]
0802b8d4  002b      cmp	r3, #0
0802b8d6  bed0      beq	#-132 ; -> 0x0802b856 ; branch_target=0x0802b856
0802b8d8  a288      ldrh	r2, [r4, #4]
0802b8da  eb88      ldrh	r3, [r5, #6]
0802b8dc  9a42      cmp	r2, r3
0802b8de  bad1      bne	#-140 ; -> 0x0802b856 ; branch_target=0x0802b856
0802b8e0  6878      ldrb	r0, [r5, #1]
0802b8e2  fdf7edff  bl	#-8230 ; -> 0x080298c0 ; branch_target=0x080298c0
0802b8e6  10f00100  ands	r0, r0, #1
0802b8ea  b4d1      bne	#-152 ; -> 0x0802b856 ; branch_target=0x0802b856
0802b8ec  2369      ldr	r3, [r4, #16]
0802b8ee  013b      subs	r3, #1
0802b8f0  012b      cmp	r3, #1
0802b8f2  1ed8      bhi	#60 ; -> 0x0802b932 ; branch_target=0x0802b932
0802b8f4  1249      ldr	r1, [pc, #72] ; [0x0802b940] = 0x2000206c
0802b8f6  1d01      lsls	r5, r3, #4
0802b8f8  01eb0313  add.w	r3, r1, r3, lsl #4
0802b8fc  9a89      ldrh	r2, [r3, #12]
0802b8fe  b2f5807f  cmp.w	r2, #256
0802b902  1ad0      beq	#52 ; -> 0x0802b93a ; branch_target=0x0802b93a
0802b904  1ab1      cbz	r2, #6 ; -> 0x0802b90e ; branch_target=0x0802b90e
0802b906  013a      subs	r2, #1
0802b908  92b2      uxth	r2, r2
0802b90a  9a81      strh	r2, [r3, #12]
0802b90c  0ab9      cbnz	r2, #2 ; -> 0x0802b912 ; branch_target=0x0802b912
0802b90e  0023      movs	r3, #0
0802b910  4b51      str	r3, [r1, r5]
0802b912  0023      movs	r3, #0
0802b914  2360      str	r3, [r4]
0802b916  70bd      pop	{r4, r5, r6, pc}
0802b918  0123      movs	r3, #1
0802b91a  226a      ldr	r2, [r4, #32]
0802b91c  04f13001  add.w	r1, r4, #48
0802b920  6878      ldrb	r0, [r5, #1]
0802b922  fdf7f7ff  bl	#-8210 ; -> 0x08029914 ; branch_target=0x08029914
0802b926  30b9      cbnz	r0, #12 ; -> 0x0802b936 ; branch_target=0x0802b936
0802b928  237d      ldrb	r3, [r4, #20]
0802b92a  03f07f03  and	r3, r3, #127
0802b92e  2375      strb	r3, [r4, #20]
0802b930  a0e7      b	#-192 ; -> 0x0802b874 ; branch_target=0x0802b874
0802b932  0220      movs	r0, #2
0802b934  70bd      pop	{r4, r5, r6, pc}
0802b936  0120      movs	r0, #1
0802b938  70bd      pop	{r4, r5, r6, pc}
0802b93a  9881      strh	r0, [r3, #12]
0802b93c  e7e7      b	#-50 ; -> 0x0802b90e ; branch_target=0x0802b90e
