; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08033434  f0b5      push	{r4, r5, r6, r7, lr}
08033436  83b0      sub	sp, #12
08033438  0e46      mov	r6, r1
0803343a  1746      mov	r7, r2
0803343c  a349      ldr	r1, [pc, #652] ; [0x080336cc] = 0x20003468
0803343e  099c      ldr	r4, [sp, #36]
08033440  04fb03f3  mul	r3, r4, r3
08033444  002b      cmp	r3, #0
08033446  b8bf      it	lt
08033448  0733      addlt	r3, #7
0803344a  dc10      asrs	r4, r3, #3
0803344c  01ab      add	r3, sp, #4
0803344e  0434      adds	r4, #4
08033450  2246      mov	r2, r4
08033452  f7f707ff  bl	#-33266 ; -> 0x0802b264 ; branch_target=0x0802b264
08033456  0028      cmp	r0, #0
08033458  70d1      bne	#224 ; -> 0x0803353c ; branch_target=0x0803353c
0803345a  019a      ldr	r2, [sp, #4]
0803345c  032a      cmp	r2, #3
0803345e  02d9      bls	#4 ; -> 0x08033466 ; branch_target=0x08033466
08033460  a242      cmp	r2, r4
08033462  08bf      it	eq
08033464  043a      subeq	r2, #4
08033466  16f00306  ands	r6, r6, #3
0803346a  29d1      bne	#82 ; -> 0x080334c0 ; branch_target=0x080334c0
0803346c  984b      ldr	r3, [pc, #608] ; [0x080336d0] = 0x73693234
0803346e  9f42      cmp	r7, r3
08033470  00f0b880  beq.w	#368 ; -> 0x080335e4 ; branch_target=0x080335e4
08033474  964b      ldr	r3, [pc, #600] ; [0x080336d0] = 0x73693234
08033476  9f42      cmp	r7, r3
08033478  54dc      bgt	#168 ; -> 0x08033524 ; branch_target=0x08033524
0803347a  03f17343  add.w	r3, r3, #4076863488
0803347e  03f54033  add.w	r3, r3, #196608
08033482  fe33      adds	r3, #254
08033484  9f42      cmp	r7, r3
08033486  00f09f80  beq.w	#318 ; -> 0x080335c8 ; branch_target=0x080335c8
0803348a  924b      ldr	r3, [pc, #584] ; [0x080336d4] = 0x73693136
0803348c  9f42      cmp	r7, r3
0803348e  56d1      bne	#172 ; -> 0x0803353e ; branch_target=0x0803353e
08033490  5308      lsrs	r3, r2, #1
08033492  53d0      beq	#166 ; -> 0x0803353c ; branch_target=0x0803353c
08033494  089a      ldr	r2, [sp, #32]
08033496  8d49      ldr	r1, [pc, #564] ; [0x080336cc] = 0x20003468
08033498  9fed8f7a  vldr	s14, [pc, #572] ; [0x080336d8] = 0x38000000 / f32_bits_interpretation=3.051757812e-05
0803349c  02eb8300  add.w	r0, r2, r3, lsl #2
080334a0  31f9024b  ldrsh	r4, [r1], #2
080334a4  07ee904a  vmov	s15, r4
080334a8  f8eee77a  vcvt.f32.s32	s15, s15
080334ac  67ee877a  vmul.f32	s15, s15, s14
080334b0  e2ec017a  vstmia	r2!, {s15}
080334b4  8242      cmp	r2, r0
080334b6  f3d1      bne	#-26 ; -> 0x080334a0 ; branch_target=0x080334a0
080334b8  1d46      mov	r5, r3
080334ba  2846      mov	r0, r5
080334bc  03b0      add	sp, #12
080334be  f0bd      pop	{r4, r5, r6, r7, pc}
080334c0  002a      cmp	r2, #0
080334c2  00f0df80  beq.w	#446 ; -> 0x08033684 ; branch_target=0x08033684
080334c6  531e      subs	r3, r2, #1
080334c8  052b      cmp	r3, #5
080334ca  3bd9      bls	#118 ; -> 0x08033544 ; branch_target=0x08033544
080334cc  7f4d      ldr	r5, [pc, #508] ; [0x080336cc] = 0x20003468
080334ce  22f00304  bic	r4, r2, #3
080334d2  4fea920c  lsr.w	r12, r2, #2
080334d6  7319      adds	r3, r6, r5
080334d8  2946      mov	r1, r5
080334da  1c44      add	r4, r3
080334dc  53f8040b  ldr	r0, [r3], #4
080334e0  a342      cmp	r3, r4
080334e2  41f8040b  str	r0, [r1], #4
080334e6  f9d1      bne	#-14 ; -> 0x080334dc ; branch_target=0x080334dc
080334e8  9107      lsls	r1, r2, #30
080334ea  22f00303  bic	r3, r2, #3
080334ee  00f0d980  beq.w	#434 ; -> 0x080336a4 ; branch_target=0x080336a4
080334f2  46ea0301  orr.w	r1, r6, r3
080334f6  695c      ldrb	r1, [r5, r1]
080334f8  e954      strb	r1, [r5, r3]
080334fa  591c      adds	r1, r3, #1
080334fc  8a42      cmp	r2, r1
080334fe  08d9      bls	#16 ; -> 0x08033512 ; branch_target=0x08033512
08033500  0233      adds	r3, #2
08033502  7018      adds	r0, r6, r1
08033504  9342      cmp	r3, r2
08033506  285c      ldrb	r0, [r5, r0]
08033508  6854      strb	r0, [r5, r1]
0803350a  02d2      bhs	#4 ; -> 0x08033512 ; branch_target=0x08033512
0803350c  1e44      add	r6, r3
0803350e  a95d      ldrb	r1, [r5, r6]
08033510  e954      strb	r1, [r5, r3]
08033512  6f4b      ldr	r3, [pc, #444] ; [0x080336d0] = 0x73693234
08033514  1546      mov	r5, r2
08033516  9f42      cmp	r7, r3
08033518  acd1      bne	#-168 ; -> 0x08033474 ; branch_target=0x08033474
0803351a  7048      ldr	r0, [pc, #448] ; [0x080336dc] = 0xaaaaaaab
0803351c  a0fb0230  umull	r3, r0, r0, r2
08033520  4008      lsrs	r0, r0, #1
08033522  66e0      b	#204 ; -> 0x080335f2 ; branch_target=0x080335f2
08033524  6e4b      ldr	r3, [pc, #440] ; [0x080336e0] = 0x73693332
08033526  9f42      cmp	r7, r3
08033528  3ad0      beq	#116 ; -> 0x080335a0 ; branch_target=0x080335a0
0803352a  03f10273  add.w	r3, r3, #34078720
0803352e  03f5fe33  add.w	r3, r3, #130048
08033532  03f58373  add.w	r3, r3, #262
08033536  9f42      cmp	r7, r3
08033538  01d1      bne	#2 ; -> 0x0803353e ; branch_target=0x0803353e
0803353a  e2b9      cbnz	r2, #56 ; -> 0x08033576 ; branch_target=0x08033576
0803353c  0025      movs	r5, #0
0803353e  2846      mov	r0, r5
08033540  03b0      add	sp, #12
08033542  f0bd      pop	{r4, r5, r6, r7, pc}
08033544  6149      ldr	r1, [pc, #388] ; [0x080336cc] = 0x20003468
08033546  013e      subs	r6, #1
08033548  0846      mov	r0, r1
0803354a  3144      add	r1, r6
0803354c  8418      adds	r4, r0, r2
0803354e  11f8013f  ldrb	r3, [r1, #1]!
08033552  00f8013b  strb	r3, [r0], #1
08033556  a042      cmp	r0, r4
08033558  f9d1      bne	#-14 ; -> 0x0803354e ; branch_target=0x0803354e
0803355a  1546      mov	r5, r2
0803355c  86e7      b	#-244 ; -> 0x0803346c ; branch_target=0x0803346c
0803355e  604b      ldr	r3, [pc, #384] ; [0x080336e0] = 0x73693332
08033560  9f42      cmp	r7, r3
08033562  00f0b080  beq.w	#352 ; -> 0x080336c6 ; branch_target=0x080336c6
08033566  03f10273  add.w	r3, r3, #34078720
0803356a  03f5fe33  add.w	r3, r3, #130048
0803356e  03f58373  add.w	r3, r3, #262
08033572  9f42      cmp	r7, r3
08033574  e3d1      bne	#-58 ; -> 0x0803353e ; branch_target=0x0803353e
08033576  0899      ldr	r1, [sp, #32]
08033578  5448      ldr	r0, [pc, #336] ; [0x080336cc] = 0x20003468
0803357a  9fed5a7a  vldr	s14, [pc, #360] ; [0x080336e4] = 0x3c000000 / f32_bits_interpretation=0.0078125
0803357e  01eb8204  add.w	r4, r1, r2, lsl #2
08033582  10f8013b  ldrb	r3, [r0], #1
08033586  803b      subs	r3, #128
08033588  07ee903a  vmov	s15, r3
0803358c  f8eee77a  vcvt.f32.s32	s15, s15
08033590  67ee877a  vmul.f32	s15, s15, s14
08033594  e1ec017a  vstmia	r1!, {s15}
08033598  a142      cmp	r1, r4
0803359a  f2d1      bne	#-28 ; -> 0x08033582 ; branch_target=0x08033582
0803359c  1546      mov	r5, r2
0803359e  cee7      b	#-100 ; -> 0x0803353e ; branch_target=0x0803353e
080335a0  9308      lsrs	r3, r2, #2
080335a2  cbd0      beq	#-106 ; -> 0x0803353c ; branch_target=0x0803353c
080335a4  089a      ldr	r2, [sp, #32]
080335a6  4949      ldr	r1, [pc, #292] ; [0x080336cc] = 0x20003468
080335a8  9fed4f7a  vldr	s14, [pc, #316] ; [0x080336e8] = 0x2ffffff6 / f32_bits_interpretation=4.656610098e-10
080335ac  02eb8300  add.w	r0, r2, r3, lsl #2
080335b0  f1ec017a  vldmia	r1!, {s15}
080335b4  f8eee77a  vcvt.f32.s32	s15, s15
080335b8  67ee877a  vmul.f32	s15, s15, s14
080335bc  e2ec017a  vstmia	r2!, {s15}
080335c0  8242      cmp	r2, r0
080335c2  f5d1      bne	#-22 ; -> 0x080335b0 ; branch_target=0x080335b0
080335c4  1d46      mov	r5, r3
080335c6  78e7      b	#-272 ; -> 0x080334ba ; branch_target=0x080334ba
080335c8  9308      lsrs	r3, r2, #2
080335ca  b7d0      beq	#-146 ; -> 0x0803353c ; branch_target=0x0803353c
080335cc  089a      ldr	r2, [sp, #32]
080335ce  3f49      ldr	r1, [pc, #252] ; [0x080336cc] = 0x20003468
080335d0  02eb8304  add.w	r4, r2, r3, lsl #2
080335d4  51f8040b  ldr	r0, [r1], #4
080335d8  42f8040b  str	r0, [r2], #4
080335dc  a242      cmp	r2, r4
080335de  f9d1      bne	#-14 ; -> 0x080335d4 ; branch_target=0x080335d4
080335e0  1d46      mov	r5, r3
080335e2  6ae7      b	#-300 ; -> 0x080334ba ; branch_target=0x080334ba
080335e4  3d48      ldr	r0, [pc, #244] ; [0x080336dc] = 0xaaaaaaab
080335e6  022a      cmp	r2, #2
080335e8  a0fb0230  umull	r3, r0, r0, r2
080335ec  4fea5000  lsr.w	r0, r0, #1
080335f0  a4d9      bls	#-184 ; -> 0x0803353c ; branch_target=0x0803353c
080335f2  3649      ldr	r1, [pc, #216] ; [0x080336cc] = 0x20003468
080335f4  0025      movs	r5, #0
080335f6  089b      ldr	r3, [sp, #32]
080335f8  9fed3b7a  vldr	s14, [pc, #236] ; [0x080336e8] = 0x2ffffff6 / f32_bits_interpretation=4.656610098e-10
080335fc  3b4c      ldr	r4, [pc, #236] ; [0x080336ec] = 0x00ffff00
080335fe  32e0      b	#100 ; -> 0x08033666 ; branch_target=0x08033666
08033600  4a88      ldrh	r2, [r1, #2]
08033602  b842      cmp	r0, r7
08033604  4e68      ldr	r6, [r1, #4]
08033606  02f47f42  and	r2, r2, #65280
0803360a  02eb0642  add.w	r2, r2, r6, lsl #16
0803360e  07ee902a  vmov	s15, r2
08033612  f8eee77a  vcvt.f32.s32	s15, s15
08033616  67ee877a  vmul.f32	s15, s15, s14
0803361a  c3ed017a  vstr	s15, [r3, #4]
0803361e  8ed0      beq	#-228 ; -> 0x0803353e ; branch_target=0x0803353e
08033620  4a68      ldr	r2, [r1, #4]
08033622  8e68      ldr	r6, [r1, #8]
08033624  04ea1222  and.w	r2, r4, r2, lsr #8
08033628  02eb0662  add.w	r2, r2, r6, lsl #24
0803362c  07ee902a  vmov	s15, r2
08033630  ea1c      adds	r2, r5, #3
08033632  f8eee77a  vcvt.f32.s32	s15, s15
08033636  9042      cmp	r0, r2
08033638  67ee877a  vmul.f32	s15, s15, s14
0803363c  c3ed027a  vstr	s15, [r3, #8]
08033640  3ff47daf  beq.w	#-262 ; -> 0x0803353e ; branch_target=0x0803353e
08033644  8a68      ldr	r2, [r1, #8]
08033646  0435      adds	r5, #4
08033648  1033      adds	r3, #16
0803364a  0c31      adds	r1, #12
0803364c  22f0ff02  bic	r2, r2, #255
08033650  a842      cmp	r0, r5
08033652  07ee902a  vmov	s15, r2
08033656  f8eee77a  vcvt.f32.s32	s15, s15
0803365a  67ee877a  vmul.f32	s15, s15, s14
0803365e  43ed017a  vstr	s15, [r3, #-4]
08033662  7ff66caf  bls.w	#-296 ; -> 0x0803353e ; branch_target=0x0803353e
08033666  0a68      ldr	r2, [r1]
08033668  6e1c      adds	r6, r5, #1
0803366a  af1c      adds	r7, r5, #2
0803366c  1202      lsls	r2, r2, #8
0803366e  b042      cmp	r0, r6
08033670  07ee902a  vmov	s15, r2
08033674  f8eee77a  vcvt.f32.s32	s15, s15
08033678  67ee877a  vmul.f32	s15, s15, s14
0803367c  c3ed007a  vstr	s15, [r3]
08033680  bed1      bne	#-132 ; -> 0x08033600 ; branch_target=0x08033600
08033682  5ce7      b	#-328 ; -> 0x0803353e ; branch_target=0x0803353e
08033684  124b      ldr	r3, [pc, #72] ; [0x080336d0] = 0x73693234
08033686  9f42      cmp	r7, r3
08033688  3ff458af  beq.w	#-336 ; -> 0x0803353c ; branch_target=0x0803353c
0803368c  3ff756af  bgt.w	#-340 ; -> 0x0803353c ; branch_target=0x0803353c
08033690  03f17343  add.w	r3, r3, #4076863488
08033694  03f54033  add.w	r3, r3, #196608
08033698  fe33      adds	r3, #254
0803369a  9f42      cmp	r7, r3
0803369c  3ff44eaf  beq.w	#-356 ; -> 0x0803353c ; branch_target=0x0803353c
080336a0  1546      mov	r5, r2
080336a2  f2e6      b	#-540 ; -> 0x0803348a ; branch_target=0x0803348a
080336a4  0a4b      ldr	r3, [pc, #40] ; [0x080336d0] = 0x73693234
080336a6  1546      mov	r5, r2
080336a8  9f42      cmp	r7, r3
080336aa  3ff436af  beq.w	#-404 ; -> 0x0803351a ; branch_target=0x0803351a
080336ae  3ff756af  bgt.w	#-340 ; -> 0x0803355e ; branch_target=0x0803355e
080336b2  03f17343  add.w	r3, r3, #4076863488
080336b6  03f54033  add.w	r3, r3, #196608
080336ba  fe33      adds	r3, #254
080336bc  9f42      cmp	r7, r3
080336be  7ff4e4ae  bne.w	#-568 ; -> 0x0803348a ; branch_target=0x0803348a
080336c2  6346      mov	r3, r12
080336c4  82e7      b	#-252 ; -> 0x080335cc ; branch_target=0x080335cc
080336c6  6346      mov	r3, r12
080336c8  6ce7      b	#-296 ; -> 0x080335a4 ; branch_target=0x080335a4
