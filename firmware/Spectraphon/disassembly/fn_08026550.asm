; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08026550  f0b5      push	{r4, r5, r6, r7, lr}
08026552  0022      movs	r2, #0
08026554  95b0      sub	sp, #84
08026556  0023      movs	r3, #0
08026558  0446      mov	r4, r0
0802655a  4ff48030  mov.w	r0, #65536
0802655e  cde90423  strd	r2, r3, [sp, #16]
08026562  cde90623  strd	r2, r3, [sp, #24]
08026566  fef7ddfe  bl	#-4678 ; -> 0x08025324 ; branch_target=0x08025324
0802656a  48b9      cbnz	r0, #18 ; -> 0x08026580 ; branch_target=0x08026580
0802656c  0122      movs	r2, #1
0802656e  4ff00063  mov.w	r3, #134217728
08026572  84f83020  strb.w	r2, [r4, #48]
08026576  6363      str	r3, [r4, #52]
08026578  0125      movs	r5, #1
0802657a  2846      mov	r0, r5
0802657c  15b0      add	sp, #84
0802657e  f0bd      pop	{r4, r5, r6, r7, pc}
08026580  060a      lsrs	r6, r0, #8
08026582  7f4b      ldr	r3, [pc, #508] ; [0x08026780] = 0x014f8b59
08026584  07aa      add	r2, sp, #28
08026586  0546      mov	r5, r0
08026588  a3fb0636  umull	r3, r6, r3, r6
0802658c  04ab      add	r3, sp, #16
0802658e  3609      lsrs	r6, r6, #4
08026590  0896      str	r6, [sp, #32]
08026592  7600      lsls	r6, r6, #1
08026594  92e80300  ldm.w	r2, {r0, r1}
08026598  8de80300  stm.w	sp, {r0, r1}
0802659c  0ecb      ldm	r3, {r1, r2, r3}
0802659e  2068      ldr	r0, [r4]
080265a0  01f060ff  bl	#7872 ; -> 0x08028464 ; branch_target=0x08028464
080265a4  2068      ldr	r0, [r4]
080265a6  01f083ff  bl	#7942 ; -> 0x080284b0 ; branch_target=0x080284b0
080265aa  7648      ldr	r0, [pc, #472] ; [0x08026784] = 0x00012110
080265ac  b5fbf6f5  udiv	r5, r5, r6
080265b0  b0fbf5f0  udiv	r0, r0, r5
080265b4  0130      adds	r0, #1
080265b6  f9f7f7fe  bl	#-25106 ; -> 0x080203a8 ; branch_target=0x080203a8
080265ba  0023      movs	r3, #0
080265bc  2068      ldr	r0, [r4]
080265be  0393      str	r3, [sp, #12]
080265c0  02f0a8fc  bl	#10576 ; -> 0x08028f14 ; branch_target=0x08028f14
080265c4  0546      mov	r5, r0
080265c6  60b9      cbnz	r0, #24 ; -> 0x080265e2 ; branch_target=0x080265e2
080265c8  2068      ldr	r0, [r4]
080265ca  02f0cffc  bl	#10654 ; -> 0x08028f6c ; branch_target=0x08028f6c
080265ce  78b9      cbnz	r0, #30 ; -> 0x080265f0 ; branch_target=0x080265f0
080265d0  0123      movs	r3, #1
080265d2  e363      str	r3, [r4, #60]
080265d4  0021      movs	r1, #0
080265d6  2068      ldr	r0, [r4]
080265d8  02f008fd  bl	#10768 ; -> 0x08028fec ; branch_target=0x08028fec
080265dc  90b1      cbz	r0, #36 ; -> 0x08026604 ; branch_target=0x08026604
080265de  4ff08055  mov.w	r5, #268435456
080265e2  0123      movs	r3, #1
080265e4  84f83030  strb.w	r3, [r4, #48]
080265e8  636b      ldr	r3, [r4, #52]
080265ea  2b43      orrs	r3, r5
080265ec  6363      str	r3, [r4, #52]
080265ee  c3e7      b	#-122 ; -> 0x08026578 ; branch_target=0x08026578
080265f0  e563      str	r5, [r4, #60]
080265f2  2068      ldr	r0, [r4]
080265f4  02f08efc  bl	#10524 ; -> 0x08028f14 ; branch_target=0x08028f14
080265f8  0546      mov	r5, r0
080265fa  0028      cmp	r0, #0
080265fc  f1d1      bne	#-30 ; -> 0x080265e2 ; branch_target=0x080265e2
080265fe  e36b      ldr	r3, [r4, #60]
08026600  012b      cmp	r3, #1
08026602  e7d0      beq	#-50 ; -> 0x080265d4 ; branch_target=0x080265d4
08026604  039b      ldr	r3, [sp, #12]
08026606  4ff6fe76  movw	r6, #65534
0802660a  b342      cmp	r3, r6
0802660c  00f2af80  bhi.w	#350 ; -> 0x0802676e ; branch_target=0x0802676e
08026610  5d4f      ldr	r7, [pc, #372] ; [0x08026788] = 0xc1100000 / f32_bits_interpretation=-9
08026612  11e0      b	#34 ; -> 0x08026638 ; branch_target=0x08026638
08026614  2068      ldr	r0, [r4]
08026616  02f095fd  bl	#11050 ; -> 0x08029144 ; branch_target=0x08029144
0802661a  0146      mov	r1, r0
0802661c  0028      cmp	r0, #0
0802661e  ded1      bne	#-68 ; -> 0x080265de ; branch_target=0x080265de
08026620  2068      ldr	r0, [r4]
08026622  01f051ff  bl	#7842 ; -> 0x080284c8 ; branch_target=0x080284c8
08026626  039b      ldr	r3, [sp, #12]
08026628  0028      cmp	r0, #0
0802662a  03f10103  add.w	r3, r3, #1
0802662e  0393      str	r3, [sp, #12]
08026630  039b      ldr	r3, [sp, #12]
08026632  0adb      blt	#20 ; -> 0x0802664a ; branch_target=0x0802664a
08026634  b342      cmp	r3, r6
08026636  08d8      bhi	#16 ; -> 0x0802664a ; branch_target=0x0802664a
08026638  0021      movs	r1, #0
0802663a  2068      ldr	r0, [r4]
0802663c  02f0d6fc  bl	#10668 ; -> 0x08028fec ; branch_target=0x08028fec
08026640  3946      mov	r1, r7
08026642  0546      mov	r5, r0
08026644  0028      cmp	r0, #0
08026646  e5d0      beq	#-54 ; -> 0x08026614 ; branch_target=0x08026614
08026648  cbe7      b	#-106 ; -> 0x080265e2 ; branch_target=0x080265e2
0802664a  039a      ldr	r2, [sp, #12]
0802664c  4ff6fe73  movw	r3, #65534
08026650  9a42      cmp	r2, r3
08026652  00f29080  bhi.w	#288 ; -> 0x08026776 ; branch_target=0x08026776
08026656  4300      lsls	r3, r0, #1
08026658  01d5      bpl	#2 ; -> 0x0802665e ; branch_target=0x0802665e
0802665a  0123      movs	r3, #1
0802665c  a363      str	r3, [r4, #56]
0802665e  0123      movs	r3, #1
08026660  2068      ldr	r0, [r4]
08026662  adf80a30  strh.w	r3, [sp, #10]
08026666  01f02bff  bl	#7766 ; -> 0x080284c0 ; branch_target=0x080284c0
0802666a  0028      cmp	r0, #0
0802666c  79d0      beq	#242 ; -> 0x08026762 ; branch_target=0x08026762
0802666e  a36b      ldr	r3, [r4, #56]
08026670  2068      ldr	r0, [r4]
08026672  032b      cmp	r3, #3
08026674  24d1      bne	#72 ; -> 0x080266c0 ; branch_target=0x080266c0
08026676  0421      movs	r1, #4
08026678  01f026ff  bl	#7756 ; -> 0x080284c8 ; branch_target=0x080284c8
0802667c  0346      mov	r3, r0
0802667e  09a9      add	r1, sp, #36
08026680  2046      mov	r0, r4
08026682  1b0d      lsrs	r3, r3, #20
08026684  2364      str	r3, [r4, #64]
08026686  fff77ffe  bl	#-770 ; -> 0x08026388 ; branch_target=0x08026388
0802668a  0546      mov	r5, r0
0802668c  0028      cmp	r0, #0
0802668e  6bd1      bne	#214 ; -> 0x08026768 ; branch_target=0x08026768
08026690  616c      ldr	r1, [r4, #68]
08026692  2068      ldr	r0, [r4]
08026694  0904      lsls	r1, r1, #16
08026696  02f091fb  bl	#10018 ; -> 0x08028dbc ; branch_target=0x08028dbc
0802669a  a0b9      cbnz	r0, #40 ; -> 0x080266c6 ; branch_target=0x080266c6
0802669c  4ff40071  mov.w	r1, #512
080266a0  2068      ldr	r0, [r4]
080266a2  01f02bff  bl	#7766 ; -> 0x080284fc ; branch_target=0x080284fc
080266a6  0028      cmp	r0, #0
080266a8  3ff467af  beq.w	#-306 ; -> 0x0802657a ; branch_target=0x0802657a
080266ac  2368      ldr	r3, [r4]
080266ae  0122      movs	r2, #1
080266b0  3649      ldr	r1, [pc, #216] ; [0x0802678c] = 0x1fe00fff
080266b2  9963      str	r1, [r3, #56]
080266b4  636b      ldr	r3, [r4, #52]
080266b6  0343      orrs	r3, r0
080266b8  6363      str	r3, [r4, #52]
080266ba  84f83020  strb.w	r2, [r4, #48]
080266be  5be7      b	#-330 ; -> 0x08026578 ; branch_target=0x08026578
080266c0  02f0cefe  bl	#11676 ; -> 0x08029460 ; branch_target=0x08029460
080266c4  30b1      cbz	r0, #12 ; -> 0x080266d4 ; branch_target=0x080266d4
080266c6  0123      movs	r3, #1
080266c8  84f83030  strb.w	r3, [r4, #48]
080266cc  636b      ldr	r3, [r4, #52]
080266ce  0343      orrs	r3, r0
080266d0  6363      str	r3, [r4, #52]
080266d2  51e7      b	#-350 ; -> 0x08026578 ; branch_target=0x08026578
080266d4  0146      mov	r1, r0
080266d6  2068      ldr	r0, [r4]
080266d8  01f0f6fe  bl	#7660 ; -> 0x080284c8 ; branch_target=0x080284c8
080266dc  0346      mov	r3, r0
080266de  0421      movs	r1, #4
080266e0  2068      ldr	r0, [r4]
080266e2  e366      str	r3, [r4, #108]
080266e4  01f0f0fe  bl	#7648 ; -> 0x080284c8 ; branch_target=0x080284c8
080266e8  0346      mov	r3, r0
080266ea  0821      movs	r1, #8
080266ec  2068      ldr	r0, [r4]
080266ee  2367      str	r3, [r4, #112]
080266f0  01f0eafe  bl	#7636 ; -> 0x080284c8 ; branch_target=0x080284c8
080266f4  0346      mov	r3, r0
080266f6  0c21      movs	r1, #12
080266f8  2068      ldr	r0, [r4]
080266fa  6367      str	r3, [r4, #116]
080266fc  01f0e4fe  bl	#7624 ; -> 0x080284c8 ; branch_target=0x080284c8
08026700  a36b      ldr	r3, [r4, #56]
08026702  a067      str	r0, [r4, #120]
08026704  032b      cmp	r3, #3
08026706  39d0      beq	#114 ; -> 0x0802677c ; branch_target=0x0802677c
08026708  0df10a01  add.w	r1, sp, #10
0802670c  2068      ldr	r0, [r4]
0802670e  02f025ff  bl	#11850 ; -> 0x0802955c ; branch_target=0x0802955c
08026712  0028      cmp	r0, #0
08026714  d7d1      bne	#-82 ; -> 0x080266c6 ; branch_target=0x080266c6
08026716  a36b      ldr	r3, [r4, #56]
08026718  2068      ldr	r0, [r4]
0802671a  032b      cmp	r3, #3
0802671c  abd0      beq	#-170 ; -> 0x08026676 ; branch_target=0x08026676
0802671e  bdf80a30  ldrh.w	r3, [sp, #10]
08026722  1904      lsls	r1, r3, #16
08026724  6364      str	r3, [r4, #68]
08026726  02f0d9fe  bl	#11698 ; -> 0x080294dc ; branch_target=0x080294dc
0802672a  0028      cmp	r0, #0
0802672c  cbd1      bne	#-106 ; -> 0x080266c6 ; branch_target=0x080266c6
0802672e  0146      mov	r1, r0
08026730  2068      ldr	r0, [r4]
08026732  01f0c9fe  bl	#7570 ; -> 0x080284c8 ; branch_target=0x080284c8
08026736  0346      mov	r3, r0
08026738  0421      movs	r1, #4
0802673a  2068      ldr	r0, [r4]
0802673c  e365      str	r3, [r4, #92]
0802673e  01f0c3fe  bl	#7558 ; -> 0x080284c8 ; branch_target=0x080284c8
08026742  0346      mov	r3, r0
08026744  0821      movs	r1, #8
08026746  2068      ldr	r0, [r4]
08026748  2366      str	r3, [r4, #96]
0802674a  01f0bdfe  bl	#7546 ; -> 0x080284c8 ; branch_target=0x080284c8
0802674e  0346      mov	r3, r0
08026750  0c21      movs	r1, #12
08026752  2068      ldr	r0, [r4]
08026754  6366      str	r3, [r4, #100]
08026756  01f0b7fe  bl	#7534 ; -> 0x080284c8 ; branch_target=0x080284c8
0802675a  0346      mov	r3, r0
0802675c  2068      ldr	r0, [r4]
0802675e  a366      str	r3, [r4, #104]
08026760  89e7      b	#-238 ; -> 0x08026676 ; branch_target=0x08026676
08026762  4ff08060  mov.w	r0, #67108864
08026766  aee7      b	#-164 ; -> 0x080266c6 ; branch_target=0x080266c6
08026768  4ff08050  mov.w	r0, #268435456
0802676c  abe7      b	#-170 ; -> 0x080266c6 ; branch_target=0x080266c6
0802676e  039b      ldr	r3, [sp, #12]
08026770  b342      cmp	r3, r6
08026772  7ff674af  bls.w	#-280 ; -> 0x0802665e ; branch_target=0x0802665e
08026776  4ff08075  mov.w	r5, #16777216
0802677a  32e7      b	#-412 ; -> 0x080265e2 ; branch_target=0x080265e2
0802677c  2068      ldr	r0, [r4]
0802677e  7ae7      b	#-268 ; -> 0x08026676 ; branch_target=0x08026676
