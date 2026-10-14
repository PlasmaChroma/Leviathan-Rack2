; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08023518  10b4      push	{r4}
0802351a  0223      movs	r3, #2
0802351c  0468      ldr	r4, [r0]
0802351e  8446      mov	r12, r0
08023520  80f83630  strb.w	r3, [r0, #54]
08023524  e068      ldr	r0, [r4, #12]
08023526  204b      ldr	r3, [pc, #128] ; [0x080235a8] = 0x20000014
08023528  20f48010  bic	r0, r0, #1048576
0802352c  1b68      ldr	r3, [r3]
0802352e  e060      str	r0, [r4, #12]
08023530  1e48      ldr	r0, [pc, #120] ; [0x080235ac] = 0xd1b71759
08023532  dcf80040  ldr.w	r4, [r12]
08023536  a0fb0303  umull	r0, r3, r0, r3
0802353a  2069      ldr	r0, [r4, #16]
0802353c  9b0b      lsrs	r3, r3, #14
0802353e  40f00100  orr	r0, r0, #1
08023542  2061      str	r0, [r4, #16]
08023544  1024      movs	r4, #16
08023546  dcf80000  ldr.w	r0, [r12]
0802354a  4460      str	r4, [r0, #4]
0802354c  dcf80000  ldr.w	r0, [r12]
08023550  8161      str	r1, [r0, #24]
08023552  4ff47a71  mov.w	r1, #1000
08023556  dcf80000  ldr.w	r0, [r12]
0802355a  01fb03f3  mul	r3, r1, r3
0802355e  1946      mov	r1, r3
08023560  0139      subs	r1, #1
08023562  19d0      beq	#50 ; -> 0x08023598 ; branch_target=0x08023598
08023564  0468      ldr	r4, [r0]
08023566  e406      lsls	r4, r4, #27
08023568  fad5      bpl	#-12 ; -> 0x08023560 ; branch_target=0x08023560
0802356a  0821      movs	r1, #8
0802356c  4160      str	r1, [r0, #4]
0802356e  dcf80010  ldr.w	r1, [r12]
08023572  4a61      str	r2, [r1, #20]
08023574  dcf80020  ldr.w	r2, [r12]
08023578  013b      subs	r3, #1
0802357a  12d0      beq	#36 ; -> 0x080235a2 ; branch_target=0x080235a2
0802357c  1168      ldr	r1, [r2]
0802357e  0907      lsls	r1, r1, #28
08023580  fad5      bpl	#-12 ; -> 0x08023578 ; branch_target=0x08023578
08023582  1369      ldr	r3, [r2, #16]
08023584  0121      movs	r1, #1
08023586  0020      movs	r0, #0
08023588  43f00403  orr	r3, r3, #4
0802358c  1361      str	r3, [r2, #16]
0802358e  8cf83610  strb.w	r1, [r12, #54]
08023592  5df8044b  ldr	r4, [sp], #4
08023596  7047      bx	lr
08023598  0368      ldr	r3, [r0]
0802359a  0320      movs	r0, #3
0802359c  5df8044b  ldr	r4, [sp], #4
080235a0  7047      bx	lr
080235a2  1368      ldr	r3, [r2]
080235a4  f9e7      b	#-14 ; -> 0x0802359a ; branch_target=0x0802359a
