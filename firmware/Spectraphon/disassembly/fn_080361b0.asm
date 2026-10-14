; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
080361b0  10b5      push	{r4, lr}
080361b2  0024      movs	r4, #0
080361b4  98b0      sub	sp, #96
080361b6  2c22      movs	r2, #44
080361b8  2146      mov	r1, r4
080361ba  0ca8      add	r0, sp, #48
080361bc  0094      str	r4, [sp]
080361be  0494      str	r4, [sp, #16]
080361c0  cde90144  strd	r4, r4, [sp, #4]
080361c4  cde90544  strd	r4, r4, [sp, #20]
080361c8  cde90744  strd	r4, r4, [sp, #28]
080361cc  cde90944  strd	r4, r4, [sp, #36]
080361d0  00f057f9  bl	#686 ; -> 0x08036482 ; branch_target=0x08036482
080361d4  3048      ldr	r0, [pc, #192] ; [0x08036298] = 0x20014dc0
080361d6  8023      movs	r3, #128
080361d8  304a      ldr	r2, [pc, #192] ; [0x0803629c] = 0x40010000 / f32_bits_interpretation=2.015625
080361da  8460      str	r4, [r0, #8]
080361dc  c360      str	r3, [r0, #12]
080361de  8361      str	r3, [r0, #24]
080361e0  0123      movs	r3, #1
080361e2  c0e90444  strd	r4, r4, [r0, #16]
080361e6  c0e90023  strd	r2, r3, [r0]
080361ea  f1f751fc  bl	#-59230 ; -> 0x08027a90 ; branch_target=0x08027a90
080361ee  0028      cmp	r0, #0
080361f0  3bd1      bne	#118 ; -> 0x0803626a ; branch_target=0x0803626a
080361f2  0023      movs	r3, #0
080361f4  0022      movs	r2, #0
080361f6  6946      mov	r1, sp
080361f8  2748      ldr	r0, [pc, #156] ; [0x08036298] = 0x20014dc0
080361fa  cde90023  strd	r2, r3, [sp]
080361fe  0023      movs	r3, #0
08036200  0293      str	r3, [sp, #8]
08036202  f1f7e3ff  bl	#-57402 ; -> 0x080281cc ; branch_target=0x080281cc
08036206  0028      cmp	r0, #0
08036208  3fd1      bne	#126 ; -> 0x0803628a ; branch_target=0x0803628a
0803620a  0020      movs	r0, #0
0803620c  0021      movs	r1, #0
0803620e  0022      movs	r2, #0
08036210  6023      movs	r3, #96
08036212  cde90801  strd	r0, r1, [sp, #32]
08036216  04a9      add	r1, sp, #16
08036218  1f48      ldr	r0, [pc, #124] ; [0x08036298] = 0x20014dc0
0803621a  0a92      str	r2, [sp, #40]
0803621c  cde90432  strd	r3, r2, [sp, #16]
08036220  cde90622  strd	r2, r2, [sp, #24]
08036224  f1f79cfe  bl	#-58056 ; -> 0x08027f60 ; branch_target=0x08027f60
08036228  60bb      cbnz	r0, #88 ; -> 0x08036284 ; branch_target=0x08036284
0803622a  0822      movs	r2, #8
0803622c  04a9      add	r1, sp, #16
0803622e  1a48      ldr	r0, [pc, #104] ; [0x08036298] = 0x20014dc0
08036230  f1f796fe  bl	#-58068 ; -> 0x08027f60 ; branch_target=0x08027f60
08036234  18bb      cbnz	r0, #70 ; -> 0x0803627e ; branch_target=0x0803627e
08036236  0023      movs	r3, #0
08036238  4ff40052  mov.w	r2, #8192
0803623c  0ca9      add	r1, sp, #48
0803623e  1648      ldr	r0, [pc, #88] ; [0x08036298] = 0x20014dc0
08036240  1693      str	r3, [sp, #88]
08036242  9fed137b  vldr	d7, [pc, #76] ; [0x08036290] = 0x02000000 / f64_bits_interpretation=1.657809211691619e-316
08036246  cde90c33  strd	r3, r3, [sp, #48]
0803624a  cde90e33  strd	r3, r3, [sp, #56]
0803624e  cde91032  strd	r3, r2, [sp, #64]
08036252  cde91233  strd	r3, r3, [sp, #72]
08036256  8ded147b  vstr	d7, [sp, #80]
0803625a  f2f711f8  bl	#-57310 ; -> 0x08028280 ; branch_target=0x08028280
0803625e  38b9      cbnz	r0, #14 ; -> 0x08036270 ; branch_target=0x08036270
08036260  0d48      ldr	r0, [pc, #52] ; [0x08036298] = 0x20014dc0
08036262  fff701ff  bl	#-510 ; -> 0x08036068 ; branch_target=0x08036068
08036266  18b0      add	sp, #96
08036268  10bd      pop	{r4, pc}
0803626a  fef749ff  bl	#-4462 ; -> 0x08035100 ; branch_target=0x08035100
0803626e  c0e7      b	#-128 ; -> 0x080361f2 ; branch_target=0x080361f2
08036270  fef746ff  bl	#-4468 ; -> 0x08035100 ; branch_target=0x08035100
08036274  0848      ldr	r0, [pc, #32] ; [0x08036298] = 0x20014dc0
08036276  fff7f7fe  bl	#-530 ; -> 0x08036068 ; branch_target=0x08036068
0803627a  18b0      add	sp, #96
0803627c  10bd      pop	{r4, pc}
0803627e  fef73fff  bl	#-4482 ; -> 0x08035100 ; branch_target=0x08035100
08036282  d8e7      b	#-80 ; -> 0x08036236 ; branch_target=0x08036236
08036284  fef73cff  bl	#-4488 ; -> 0x08035100 ; branch_target=0x08035100
08036288  cfe7      b	#-98 ; -> 0x0803622a ; branch_target=0x0803622a
0803628a  fef739ff  bl	#-4494 ; -> 0x08035100 ; branch_target=0x08035100
0803628e  bce7      b	#-136 ; -> 0x0803620a ; branch_target=0x0803620a
