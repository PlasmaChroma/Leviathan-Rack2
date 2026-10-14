; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
080362a0  00b5      push	{lr}
080362a2  0023      movs	r3, #0
080362a4  91b0      sub	sp, #68
080362a6  2f48      ldr	r0, [pc, #188] ; [0x08036364] = 0x20014d28
080362a8  8022      movs	r2, #128
080362aa  2f49      ldr	r1, [pc, #188] ; [0x08036368] = 0x40000400 / f32_bits_interpretation=2.000244141
080362ac  0493      str	r3, [sp, #16]
080362ae  0193      str	r3, [sp, #4]
080362b0  0893      str	r3, [sp, #32]
080362b2  0361      str	r3, [r0, #16]
080362b4  8261      str	r2, [r0, #24]
080362b6  0793      str	r3, [sp, #28]
080362b8  c0e90013  strd	r1, r3, [r0]
080362bc  c0e90232  strd	r3, r2, [r0, #8]
080362c0  cde90533  strd	r3, r3, [sp, #20]
080362c4  cde90233  strd	r3, r3, [sp, #8]
080362c8  cde90933  strd	r3, r3, [sp, #36]
080362cc  cde90b33  strd	r3, r3, [sp, #44]
080362d0  cde90d33  strd	r3, r3, [sp, #52]
080362d4  f1f7f6fa  bl	#-59924 ; -> 0x080278c4 ; branch_target=0x080278c4
080362d8  68bb      cbnz	r0, #90 ; -> 0x08036336 ; branch_target=0x08036336
080362da  4ff48053  mov.w	r3, #4096
080362de  04a9      add	r1, sp, #16
080362e0  2048      ldr	r0, [pc, #128] ; [0x08036364] = 0x20014d28
080362e2  0493      str	r3, [sp, #16]
080362e4  f1f730fd  bl	#-58784 ; -> 0x08027d48 ; branch_target=0x08027d48
080362e8  0028      cmp	r0, #0
080362ea  38d1      bne	#112 ; -> 0x0803635e ; branch_target=0x0803635e
080362ec  1d48      ldr	r0, [pc, #116] ; [0x08036364] = 0x20014d28
080362ee  f1f7cffb  bl	#-59490 ; -> 0x08027a90 ; branch_target=0x08027a90
080362f2  88bb      cbnz	r0, #98 ; -> 0x08036358 ; branch_target=0x08036358
080362f4  0023      movs	r3, #0
080362f6  01a9      add	r1, sp, #4
080362f8  1a48      ldr	r0, [pc, #104] ; [0x08036364] = 0x20014d28
080362fa  0193      str	r3, [sp, #4]
080362fc  0393      str	r3, [sp, #12]
080362fe  f1f765ff  bl	#-57654 ; -> 0x080281cc ; branch_target=0x080281cc
08036302  30bb      cbnz	r0, #76 ; -> 0x08036352 ; branch_target=0x08036352
08036304  6020      movs	r0, #96
08036306  0021      movs	r1, #0
08036308  0023      movs	r3, #0
0803630a  0822      movs	r2, #8
0803630c  cde90801  strd	r0, r1, [sp, #32]
08036310  08a9      add	r1, sp, #32
08036312  1448      ldr	r0, [pc, #80] ; [0x08036364] = 0x20014d28
08036314  0a93      str	r3, [sp, #40]
08036316  0c93      str	r3, [sp, #48]
08036318  f1f722fe  bl	#-58300 ; -> 0x08027f60 ; branch_target=0x08027f60
0803631c  b0b9      cbnz	r0, #44 ; -> 0x0803634c ; branch_target=0x0803634c
0803631e  0c22      movs	r2, #12
08036320  08a9      add	r1, sp, #32
08036322  1048      ldr	r0, [pc, #64] ; [0x08036364] = 0x20014d28
08036324  f1f71cfe  bl	#-58312 ; -> 0x08027f60 ; branch_target=0x08027f60
08036328  40b9      cbnz	r0, #16 ; -> 0x0803633c ; branch_target=0x0803633c
0803632a  0e48      ldr	r0, [pc, #56] ; [0x08036364] = 0x20014d28
0803632c  fff79cfe  bl	#-712 ; -> 0x08036068 ; branch_target=0x08036068
08036330  11b0      add	sp, #68
08036332  5df804fb  ldr	pc, [sp], #4
08036336  fef7e3fe  bl	#-4666 ; -> 0x08035100 ; branch_target=0x08035100
0803633a  cee7      b	#-100 ; -> 0x080362da ; branch_target=0x080362da
0803633c  fef7e0fe  bl	#-4672 ; -> 0x08035100 ; branch_target=0x08035100
08036340  0848      ldr	r0, [pc, #32] ; [0x08036364] = 0x20014d28
08036342  fff791fe  bl	#-734 ; -> 0x08036068 ; branch_target=0x08036068
08036346  11b0      add	sp, #68
08036348  5df804fb  ldr	pc, [sp], #4
0803634c  fef7d8fe  bl	#-4688 ; -> 0x08035100 ; branch_target=0x08035100
08036350  e5e7      b	#-54 ; -> 0x0803631e ; branch_target=0x0803631e
08036352  fef7d5fe  bl	#-4694 ; -> 0x08035100 ; branch_target=0x08035100
08036356  d5e7      b	#-86 ; -> 0x08036304 ; branch_target=0x08036304
08036358  fef7d2fe  bl	#-4700 ; -> 0x08035100 ; branch_target=0x08035100
0803635c  cae7      b	#-108 ; -> 0x080362f4 ; branch_target=0x080362f4
0803635e  fef7cffe  bl	#-4706 ; -> 0x08035100 ; branch_target=0x08035100
08036362  c3e7      b	#-122 ; -> 0x080362ec ; branch_target=0x080362ec
