; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
080336f0  304b      ldr	r3, [pc, #192] ; [0x080337b4] = 0x73693234
080336f2  9942      cmp	r1, r3
080336f4  30b5      push	{r4, r5, lr}
080336f6  0446      mov	r4, r0
080336f8  8db0      sub	sp, #52
080336fa  2fd0      beq	#94 ; -> 0x0803375c ; branch_target=0x0803375c
080336fc  20d8      bhi	#64 ; -> 0x08033740 ; branch_target=0x08033740
080336fe  03f17343  add.w	r3, r3, #4076863488
08033702  03f54033  add.w	r3, r3, #196608
08033706  fe33      adds	r3, #254
08033708  9942      cmp	r1, r3
0803370a  27d0      beq	#78 ; -> 0x0803375c ; branch_target=0x0803375c
0803370c  03f14f63  add.w	r3, r3, #217055232
08033710  03f54f23  add.w	r3, r3, #847872
08033714  03f60463  addw	r3, r3, #3588
08033718  9942      cmp	r1, r3
0803371a  0cbf      ite	eq
0803371c  1021      moveq	r1, #16
0803371e  0e21      movne	r1, #14
08033720  2548      ldr	r0, [pc, #148] ; [0x080337b8] = 0x45564157 / f32_bits_interpretation=3428.08374
08033722  2223      movs	r3, #34
08033724  254d      ldr	r5, [pc, #148] ; [0x080337bc] = 0x46464952 / f32_bits_interpretation=12690.33008
08033726  264a      ldr	r2, [pc, #152] ; [0x080337c0] = 0x20746d66
08033728  0791      str	r1, [sp, #28]
0803372a  0021      movs	r1, #0
0803372c  0590      str	r0, [sp, #20]
0803372e  2046      mov	r0, r4
08033730  0395      str	r5, [sp, #12]
08033732  0692      str	r2, [sp, #24]
08033734  0493      str	r3, [sp, #16]
08033736  f8f705f9  bl	#-32246 ; -> 0x0802b944 ; branch_target=0x0802b944
0803373a  88b1      cbz	r0, #34 ; -> 0x08033760 ; branch_target=0x08033760
0803373c  0db0      add	sp, #52
0803373e  30bd      pop	{r4, r5, pc}
08033740  204b      ldr	r3, [pc, #128] ; [0x080337c4] = 0x73693332
08033742  9942      cmp	r1, r3
08033744  0ad0      beq	#20 ; -> 0x0803375c ; branch_target=0x0803375c
08033746  03f10273  add.w	r3, r3, #34078720
0803374a  03f5fe33  add.w	r3, r3, #130048
0803374e  03f58373  add.w	r3, r3, #262
08033752  9942      cmp	r1, r3
08033754  0cbf      ite	eq
08033756  1021      moveq	r1, #16
08033758  0e21      movne	r1, #14
0803375a  e1e7      b	#-62 ; -> 0x08033720 ; branch_target=0x08033720
0803375c  1021      movs	r1, #16
0803375e  dfe7      b	#-66 ; -> 0x08033720 ; branch_target=0x08033720
08033760  049a      ldr	r2, [sp, #16]
08033762  03a9      add	r1, sp, #12
08033764  2046      mov	r0, r4
08033766  6b46      mov	r3, sp
08033768  0832      adds	r2, #8
0803376a  f7f7affe  bl	#-33442 ; -> 0x0802b4cc ; branch_target=0x0802b4cc
0803376e  0028      cmp	r0, #0
08033770  e4d1      bne	#-56 ; -> 0x0803373c ; branch_target=0x0803373c
08033772  0c21      movs	r1, #12
08033774  2046      mov	r0, r4
08033776  f8f7e5f8  bl	#-32310 ; -> 0x0802b944 ; branch_target=0x0802b944
0803377a  0028      cmp	r0, #0
0803377c  ded1      bne	#-68 ; -> 0x0803373c ; branch_target=0x0803373c
0803377e  079a      ldr	r2, [sp, #28]
08033780  6b46      mov	r3, sp
08033782  06a9      add	r1, sp, #24
08033784  2046      mov	r0, r4
08033786  0832      adds	r2, #8
08033788  f7f7a0fe  bl	#-33472 ; -> 0x0802b4cc ; branch_target=0x0802b4cc
0803378c  0028      cmp	r0, #0
0803378e  d5d1      bne	#-86 ; -> 0x0803373c ; branch_target=0x0803373c
08033790  0799      ldr	r1, [sp, #28]
08033792  0d4b      ldr	r3, [pc, #52] ; [0x080337c8] = 0x61746164
08033794  0290      str	r0, [sp, #8]
08033796  1431      adds	r1, #20
08033798  2046      mov	r0, r4
0803379a  0193      str	r3, [sp, #4]
0803379c  f8f7d2f8  bl	#-32348 ; -> 0x0802b944 ; branch_target=0x0802b944
080337a0  0028      cmp	r0, #0
080337a2  cbd1      bne	#-106 ; -> 0x0803373c ; branch_target=0x0803373c
080337a4  6b46      mov	r3, sp
080337a6  0822      movs	r2, #8
080337a8  01a9      add	r1, sp, #4
080337aa  2046      mov	r0, r4
080337ac  f7f78efe  bl	#-33508 ; -> 0x0802b4cc ; branch_target=0x0802b4cc
080337b0  c4e7      b	#-120 ; -> 0x0803373c ; branch_target=0x0803373c
