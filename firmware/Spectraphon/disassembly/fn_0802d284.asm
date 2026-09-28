; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: prologue_heuristic
0802d284  10b5      push	{r4, lr}
0802d286  b3f90010  ldrsh.w	r1, [r3]
0802d28a  b3f90400  ldrsh.w	r0, [r3, #4]
0802d28e  a1f10101  sub.w	r1, r1, #1
0802d292  0128      cmp	r0, #1
0802d294  b1fa81f1  clz	r1, r1
0802d298  4fea5111  lsr.w	r1, r1, #5
0802d29c  4fea0141  lsl.w	r1, r1, #16
0802d2a0  1160      str	r1, [r2]
0802d2a2  02d1      bne	#4 ; -> 0x0802d2aa ; branch_target=0x0802d2aa
0802d2a4  41f48011  orr	r1, r1, #1048576
0802d2a8  1160      str	r1, [r2]
0802d2aa  b3f90200  ldrsh.w	r0, [r3, #2]
0802d2ae  0128      cmp	r0, #1
0802d2b0  02d1      bne	#4 ; -> 0x0802d2b8 ; branch_target=0x0802d2b8
0802d2b2  41f00101  orr	r1, r1, #1
0802d2b6  1160      str	r1, [r2]
0802d2b8  b3f90600  ldrsh.w	r0, [r3, #6]
0802d2bc  0128      cmp	r0, #1
0802d2be  02d1      bne	#4 ; -> 0x0802d2c6 ; branch_target=0x0802d2c6
0802d2c0  41f01001  orr	r1, r1, #16
0802d2c4  1160      str	r1, [r2]
0802d2c6  1c69      ldr	r4, [r3, #16]
0802d2c8  9968      ldr	r1, [r3, #8]
0802d2ca  5869      ldr	r0, [r3, #20]
0802d2cc  41ea0441  orr.w	r1, r1, r4, lsl #16
0802d2d0  1c6a      ldr	r4, [r3, #32]
0802d2d2  5160      str	r1, [r2, #4]
0802d2d4  d968      ldr	r1, [r3, #12]
0802d2d6  41ea0041  orr.w	r1, r1, r0, lsl #16
0802d2da  586a      ldr	r0, [r3, #36]
0802d2dc  9160      str	r1, [r2, #8]
0802d2de  9969      ldr	r1, [r3, #24]
0802d2e0  41ea0441  orr.w	r1, r1, r4, lsl #16
0802d2e4  dc6a      ldr	r4, [r3, #44]
0802d2e6  d160      str	r1, [r2, #12]
0802d2e8  d969      ldr	r1, [r3, #28]
0802d2ea  9461      str	r4, [r2, #24]
0802d2ec  41ea0041  orr.w	r1, r1, r0, lsl #16
0802d2f0  0d4c      ldr	r4, [pc, #52] ; [0x0802d328] = 0x20002ebc
0802d2f2  1161      str	r1, [r2, #16]
0802d2f4  196b      ldr	r1, [r3, #48]
0802d2f6  9b6a      ldr	r3, [r3, #40]
0802d2f8  d161      str	r1, [r2, #28]
0802d2fa  5361      str	r3, [r2, #20]
0802d2fc  f5f786fd  bl	#-42228 ; -> 0x08022e0c ; branch_target=0x08022e0c
0802d300  2168      ldr	r1, [r4]
0802d302  0120      movs	r0, #1
0802d304  074a      ldr	r2, [pc, #28] ; [0x0802d324] = 0x2001346c
0802d306  f5f71dfd  bl	#-42438 ; -> 0x08022d44 ; branch_target=0x08022d44
0802d30a  2368      ldr	r3, [r4]
0802d30c  074a      ldr	r2, [pc, #28] ; [0x0802d32c] = 0x20002eb8
0802d30e  0021      movs	r1, #0
0802d310  2033      adds	r3, #32
0802d312  1160      str	r1, [r2]
0802d314  2360      str	r3, [r4]
0802d316  bde81040  pop.w	{r4, lr}
0802d31a  f5f79dbd  b.w	#-42182 ; -> 0x08022e58 ; branch_target=0x08022e58
