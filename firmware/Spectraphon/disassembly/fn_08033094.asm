; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: prologue_heuristic
08033094  2de9f04f  push.w	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
08033098  d1b0      sub	sp, #324
0803309a  0e46      mov	r6, r1
0803309c  9346      mov	r11, r2
0803309e  1f46      mov	r7, r3
080330a0  07a9      add	r1, sp, #28
080330a2  0c22      movs	r2, #12
080330a4  04ab      add	r3, sp, #16
080330a6  0546      mov	r5, r0
080330a8  f8f7dcf8  bl	#-32328 ; -> 0x0802b264 ; branch_target=0x0802b264
080330ac  0146      mov	r1, r0
080330ae  18b9      cbnz	r0, #6 ; -> 0x080330b8 ; branch_target=0x080330b8
080330b0  7e4b      ldr	r3, [pc, #504] ; [0x080332ac] = 0x46464952 / f32_bits_interpretation=12690.33008
080330b2  079a      ldr	r2, [sp, #28]
080330b4  9a42      cmp	r2, r3
080330b6  03d0      beq	#6 ; -> 0x080330c0 ; branch_target=0x080330c0
080330b8  0846      mov	r0, r1
080330ba  51b0      add	sp, #324
080330bc  bde8f08f  pop.w	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
080330c0  a3f57003  sub.w	r3, r3, #15728640
080330c4  099a      ldr	r2, [sp, #36]
080330c6  a3f2fb73  subw	r3, r3, #2043
080330ca  9a42      cmp	r2, r3
080330cc  f4d1      bne	#-24 ; -> 0x080330b8 ; branch_target=0x080330b8
080330ce  0096      str	r6, [sp]
080330d0  8246      mov	r10, r0
080330d2  cdf804b0  str.w	r11, [sp, #4]
080330d6  0c24      movs	r4, #12
080330d8  dff80082  ldr.w	r8, [pc, #512] ; [0x080332dc] = 0x20746d66
080330dc  0646      mov	r6, r0
080330de  dff80092  ldr.w	r9, [pc, #512] ; [0x080332e0] = 0x61746164
080330e2  8346      mov	r11, r0
080330e4  2146      mov	r1, r4
080330e6  2846      mov	r0, r5
080330e8  f8f72cfc  bl	#-30632 ; -> 0x0802b944 ; branch_target=0x0802b944
080330ec  0028      cmp	r0, #0
080330ee  40f09280  bne.w	#292 ; -> 0x08033216 ; branch_target=0x08033216
080330f2  04ab      add	r3, sp, #16
080330f4  0822      movs	r2, #8
080330f6  2846      mov	r0, r5
080330f8  05a9      add	r1, sp, #20
080330fa  f8f7b3f8  bl	#-32410 ; -> 0x0802b264 ; branch_target=0x0802b264
080330fe  0028      cmp	r0, #0
08033100  40f08980  bne.w	#274 ; -> 0x08033216 ; branch_target=0x08033216
08033104  059b      ldr	r3, [sp, #20]
08033106  4345      cmp	r3, r8
08033108  28d0      beq	#80 ; -> 0x0803315c ; branch_target=0x0803315c
0803310a  4b45      cmp	r3, r9
0803310c  069a      ldr	r2, [sp, #24]
0803310e  15d0      beq	#42 ; -> 0x0803313c ; branch_target=0x0803313c
08033110  6749      ldr	r1, [pc, #412] ; [0x080332b0] = 0x206d6c63
08033112  8b42      cmp	r3, r1
08033114  72d0      beq	#228 ; -> 0x080331fc ; branch_target=0x080331fc
08033116  0832      adds	r2, #8
08033118  1444      add	r4, r2
0803311a  e307      lsls	r3, r4, #31
0803311c  00d5      bpl	#0 ; -> 0x08033120 ; branch_target=0x08033120
0803311e  0134      adds	r4, #1
08033120  eb68      ldr	r3, [r5, #12]
08033122  a342      cmp	r3, r4
08033124  05d9      bls	#10 ; -> 0x08033132 ; branch_target=0x08033132
08033126  86f00103  eor	r3, r6, #1
0803312a  8af00102  eor	r2, r10, #1
0803312e  1343      orrs	r3, r2
08033130  d8d1      bne	#-80 ; -> 0x080330e4 ; branch_target=0x080330e4
08033132  5946      mov	r1, r11
08033134  0846      mov	r0, r1
08033136  51b0      add	sp, #324
08033138  bde8f08f  pop.w	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
0803313c  5b9b      ldr	r3, [sp, #364]
0803313e  1a60      str	r2, [r3]
08033140  0832      adds	r2, #8
08033142  04f10803  add.w	r3, r4, #8
08033146  1444      add	r4, r2
08033148  5c9a      ldr	r2, [sp, #368]
0803314a  e107      lsls	r1, r4, #31
0803314c  1360      str	r3, [r2]
0803314e  5d9b      ldr	r3, [sp, #372]
08033150  1c60      str	r4, [r3]
08033152  50d5      bpl	#160 ; -> 0x080331f6 ; branch_target=0x080331f6
08033154  0134      adds	r4, #1
08033156  4ff0010a  mov.w	r10, #1
0803315a  e1e7      b	#-62 ; -> 0x08033120 ; branch_target=0x08033120
0803315c  2146      mov	r1, r4
0803315e  2846      mov	r0, r5
08033160  f8f7f0fb  bl	#-30752 ; -> 0x0802b944 ; branch_target=0x0802b944
08033164  0028      cmp	r0, #0
08033166  40f09280  bne.w	#292 ; -> 0x0803328e ; branch_target=0x0803328e
0803316a  04ab      add	r3, sp, #16
0803316c  1622      movs	r2, #22
0803316e  2846      mov	r0, r5
08033170  0aa9      add	r1, sp, #40
08033172  f8f777f8  bl	#-32530 ; -> 0x0802b264 ; branch_target=0x0802b264
08033176  0028      cmp	r0, #0
08033178  40f08980  bne.w	#274 ; -> 0x0803328e ; branch_target=0x0803328e
0803317c  0d9b      ldr	r3, [sp, #52]
0803317e  009a      ldr	r2, [sp]
08033180  1360      str	r3, [r2]
08033182  bdf93230  ldrsh.w	r3, [sp, #50]
08033186  3b60      str	r3, [r7]
08033188  bdf93030  ldrsh.w	r3, [sp, #48]
0803318c  112b      cmp	r3, #17
0803318e  15dc      bgt	#42 ; -> 0x080331bc ; branch_target=0x080331bc
08033190  002b      cmp	r3, #0
08033192  0ddd      ble	#26 ; -> 0x080331b0 ; branch_target=0x080331b0
08033194  013b      subs	r3, #1
08033196  102b      cmp	r3, #16
08033198  0ad8      bhi	#20 ; -> 0x080331b0 ; branch_target=0x080331b0
0803319a  dfe803f0  tbb	[pc, r3]
080331b0  4ff0ff31  mov.w	r1, #4294967295
080331b4  0846      mov	r0, r1
080331b6  51b0      add	sp, #324
080331b8  bde8f08f  pop.w	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
080331bc  b3f5817f  cmp.w	r3, #258
080331c0  5ed0      beq	#188 ; -> 0x08033280 ; branch_target=0x08033280
080331c2  40f20312  movw	r2, #259
080331c6  9342      cmp	r3, r2
080331c8  0ad1      bne	#20 ; -> 0x080331e0 ; branch_target=0x080331e0
080331ca  0423      movs	r3, #4
080331cc  019a      ldr	r2, [sp, #4]
080331ce  1360      str	r3, [r2]
080331d0  384b      ldr	r3, [pc, #224] ; [0x080332b4] = 0x69626164
080331d2  5a9a      ldr	r2, [sp, #360]
080331d4  1360      str	r3, [r2]
080331d6  0b9b      ldr	r3, [sp, #44]
080331d8  0126      movs	r6, #1
080331da  0833      adds	r3, #8
080331dc  1c44      add	r4, r3
080331de  9fe7      b	#-194 ; -> 0x08033120 ; branch_target=0x08033120
080331e0  40f20112  movw	r2, #257
080331e4  9342      cmp	r3, r2
080331e6  e3d1      bne	#-58 ; -> 0x080331b0 ; branch_target=0x080331b0
080331e8  0823      movs	r3, #8
080331ea  019a      ldr	r2, [sp, #4]
080331ec  1360      str	r3, [r2]
080331ee  324b      ldr	r3, [pc, #200] ; [0x080332b8] = 0x6d753038
080331f0  5a9a      ldr	r2, [sp, #360]
080331f2  1360      str	r3, [r2]
080331f4  efe7      b	#-34 ; -> 0x080331d6 ; branch_target=0x080331d6
080331f6  4ff0010a  mov.w	r10, #1
080331fa  91e7      b	#-222 ; -> 0x08033120 ; branch_target=0x08033120
080331fc  04ab      add	r3, sp, #16
080331fe  10a9      add	r1, sp, #64
08033200  2846      mov	r0, r5
08033202  0834      adds	r4, #8
08033204  f8f72ef8  bl	#-32676 ; -> 0x0802b264 ; branch_target=0x0802b264
08033208  0028      cmp	r0, #0
0803320a  40d1      bne	#128 ; -> 0x0803328e ; branch_target=0x0803328e
0803320c  069b      ldr	r3, [sp, #24]
0803320e  1c44      add	r4, r3
08033210  e207      lsls	r2, r4, #31
08033212  85d5      bpl	#-246 ; -> 0x08033120 ; branch_target=0x08033120
08033214  83e7      b	#-250 ; -> 0x0803311e ; branch_target=0x0803311e
08033216  0146      mov	r1, r0
08033218  4ee7      b	#-356 ; -> 0x080330b8 ; branch_target=0x080330b8
0803321a  0423      movs	r3, #4
0803321c  019a      ldr	r2, [sp, #4]
0803321e  1360      str	r3, [r2]
08033220  264b      ldr	r3, [pc, #152] ; [0x080332bc] = 0x696d6164
08033222  5a9a      ldr	r2, [sp, #360]
08033224  1360      str	r3, [r2]
08033226  d6e7      b	#-84 ; -> 0x080331d6 ; branch_target=0x080331d6
08033228  2023      movs	r3, #32
0803322a  019a      ldr	r2, [sp, #4]
0803322c  1360      str	r3, [r2]
0803322e  244b      ldr	r3, [pc, #144] ; [0x080332c0] = 0x666c3332
08033230  5a9a      ldr	r2, [sp, #360]
08033232  1360      str	r3, [r2]
08033234  cfe7      b	#-98 ; -> 0x080331d6 ; branch_target=0x080331d6
08033236  0423      movs	r3, #4
08033238  019a      ldr	r2, [sp, #4]
0803323a  1360      str	r3, [r2]
0803323c  214b      ldr	r3, [pc, #132] ; [0x080332c4] = 0x6d736164
0803323e  5a9a      ldr	r2, [sp, #360]
08033240  1360      str	r3, [r2]
08033242  c8e7      b	#-112 ; -> 0x080331d6 ; branch_target=0x080331d6
08033244  04f11601  add.w	r1, r4, #22
08033248  2846      mov	r0, r5
0803324a  f8f77bfb  bl	#-30986 ; -> 0x0802b944 ; branch_target=0x0802b944
0803324e  f0b9      cbnz	r0, #60 ; -> 0x0803328e ; branch_target=0x0803328e
08033250  04ab      add	r3, sp, #16
08033252  0222      movs	r2, #2
08033254  2846      mov	r0, r5
08033256  0df10e01  add.w	r1, sp, #14
0803325a  f8f703f8  bl	#-32762 ; -> 0x0802b264 ; branch_target=0x0802b264
0803325e  b0b9      cbnz	r0, #44 ; -> 0x0803328e ; branch_target=0x0803328e
08033260  bdf90e30  ldrsh.w	r3, [sp, #14]
08033264  019a      ldr	r2, [sp, #4]
08033266  082b      cmp	r3, #8
08033268  1360      str	r3, [r2]
0803326a  12d0      beq	#36 ; -> 0x08033292 ; branch_target=0x08033292
0803326c  102b      cmp	r3, #16
0803326e  14d0      beq	#40 ; -> 0x0803329a ; branch_target=0x0803329a
08033270  182b      cmp	r3, #24
08033272  16d0      beq	#44 ; -> 0x080332a2 ; branch_target=0x080332a2
08033274  202b      cmp	r3, #32
08033276  aed1      bne	#-164 ; -> 0x080331d6 ; branch_target=0x080331d6
08033278  134b      ldr	r3, [pc, #76] ; [0x080332c8] = 0x73693332
0803327a  5a9a      ldr	r2, [sp, #360]
0803327c  1360      str	r3, [r2]
0803327e  aae7      b	#-172 ; -> 0x080331d6 ; branch_target=0x080331d6
08033280  0823      movs	r3, #8
08033282  019a      ldr	r2, [sp, #4]
08033284  1360      str	r3, [r2]
08033286  114b      ldr	r3, [pc, #68] ; [0x080332cc] = 0x616c3038
08033288  5a9a      ldr	r2, [sp, #360]
0803328a  1360      str	r3, [r2]
0803328c  a3e7      b	#-186 ; -> 0x080331d6 ; branch_target=0x080331d6
0803328e  0146      mov	r1, r0
08033290  12e7      b	#-476 ; -> 0x080330b8 ; branch_target=0x080330b8
08033292  0f4b      ldr	r3, [pc, #60] ; [0x080332d0] = 0x75733038
08033294  5a9a      ldr	r2, [sp, #360]
08033296  1360      str	r3, [r2]
08033298  9de7      b	#-198 ; -> 0x080331d6 ; branch_target=0x080331d6
0803329a  0e4b      ldr	r3, [pc, #56] ; [0x080332d4] = 0x73693136
0803329c  5a9a      ldr	r2, [sp, #360]
0803329e  1360      str	r3, [r2]
080332a0  99e7      b	#-206 ; -> 0x080331d6 ; branch_target=0x080331d6
080332a2  0d4b      ldr	r3, [pc, #52] ; [0x080332d8] = 0x73693234
080332a4  5a9a      ldr	r2, [sp, #360]
080332a6  1360      str	r3, [r2]
080332a8  95e7      b	#-214 ; -> 0x080331d6 ; branch_target=0x080331d6
