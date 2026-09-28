; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
0802163c  3649      ldr	r1, [pc, #216] ; [0x08021718] = 0x40020010 / f32_bits_interpretation=2.031253815
0802163e  0246      mov	r2, r0
08021640  0368      ldr	r3, [r0]
08021642  8b42      cmp	r3, r1
08021644  33d0      beq	#102 ; -> 0x080216ae ; branch_target=0x080216ae
08021646  1831      adds	r1, #24
08021648  591a      subs	r1, r3, r1
0802164a  b1fa81f1  clz	r1, r1
0802164e  4909      lsrs	r1, r1, #5
08021650  69bb      cbnz	r1, #90 ; -> 0x080216ae ; branch_target=0x080216ae
08021652  3248      ldr	r0, [pc, #200] ; [0x0802171c] = 0x40020040 / f32_bits_interpretation=2.031265259
08021654  8342      cmp	r3, r0
08021656  3ed0      beq	#124 ; -> 0x080216d6 ; branch_target=0x080216d6
08021658  1830      adds	r0, #24
0802165a  8342      cmp	r3, r0
0802165c  3ed0      beq	#124 ; -> 0x080216dc ; branch_target=0x080216dc
0802165e  1830      adds	r0, #24
08021660  8342      cmp	r3, r0
08021662  34d0      beq	#104 ; -> 0x080216ce ; branch_target=0x080216ce
08021664  1830      adds	r0, #24
08021666  8342      cmp	r3, r0
08021668  3bd0      beq	#118 ; -> 0x080216e2 ; branch_target=0x080216e2
0802166a  1830      adds	r0, #24
0802166c  8342      cmp	r3, r0
0802166e  3ed0      beq	#124 ; -> 0x080216ee ; branch_target=0x080216ee
08021670  1830      adds	r0, #24
08021672  8342      cmp	r3, r0
08021674  2ad0      beq	#84 ; -> 0x080216cc ; branch_target=0x080216cc
08021676  00f55670  add.w	r0, r0, #856
0802167a  8342      cmp	r3, r0
0802167c  35d0      beq	#106 ; -> 0x080216ea ; branch_target=0x080216ea
0802167e  2849      ldr	r1, [pc, #160] ; [0x08021720] = 0x40020428 / f32_bits_interpretation=2.031503677
08021680  8b42      cmp	r3, r1
08021682  31d0      beq	#98 ; -> 0x080216e8 ; branch_target=0x080216e8
08021684  1831      adds	r1, #24
08021686  8b42      cmp	r3, r1
08021688  34d0      beq	#104 ; -> 0x080216f4 ; branch_target=0x080216f4
0802168a  1831      adds	r1, #24
0802168c  8b42      cmp	r3, r1
0802168e  34d0      beq	#104 ; -> 0x080216fa ; branch_target=0x080216fa
08021690  1831      adds	r1, #24
08021692  8b42      cmp	r3, r1
08021694  34d0      beq	#104 ; -> 0x08021700 ; branch_target=0x08021700
08021696  1831      adds	r1, #24
08021698  8b42      cmp	r3, r1
0802169a  34d0      beq	#104 ; -> 0x08021706 ; branch_target=0x08021706
0802169c  1831      adds	r1, #24
0802169e  8b42      cmp	r3, r1
080216a0  34d0      beq	#104 ; -> 0x0802170c ; branch_target=0x0802170c
080216a2  1831      adds	r1, #24
080216a4  8b42      cmp	r3, r1
080216a6  34d0      beq	#104 ; -> 0x08021712 ; branch_target=0x08021712
080216a8  23f0ff00  bic	r0, r3, #255
080216ac  11e0      b	#34 ; -> 0x080216d2 ; branch_target=0x080216d2
080216ae  dbb2      uxtb	r3, r3
080216b0  1c49      ldr	r1, [pc, #112] ; [0x08021724] = 0xaaaaaaab
080216b2  1d48      ldr	r0, [pc, #116] ; [0x08021728] = 0x40020000 / f32_bits_interpretation=2.03125
080216b4  103b      subs	r3, #16
080216b6  a1fb0313  umull	r1, r3, r1, r3
080216ba  10b4      push	{r4}
080216bc  1b09      lsrs	r3, r3, #4
080216be  1b4c      ldr	r4, [pc, #108] ; [0x0802172c] = 0x080365dc
080216c0  e15c      ldrb	r1, [r4, r3]
080216c2  c2e91601  strd	r0, r1, [r2, #88]
080216c6  5df8044b  ldr	r4, [sp], #4
080216ca  7047      bx	lr
080216cc  1621      movs	r1, #22
080216ce  1848      ldr	r0, [pc, #96] ; [0x08021730] = 0x40020004 / f32_bits_interpretation=2.031250954
080216d0  d165      str	r1, [r2, #92]
080216d2  9065      str	r0, [r2, #88]
080216d4  7047      bx	lr
080216d6  1021      movs	r1, #16
080216d8  1348      ldr	r0, [pc, #76] ; [0x08021728] = 0x40020000 / f32_bits_interpretation=2.03125
080216da  f9e7      b	#-14 ; -> 0x080216d0 ; branch_target=0x080216d0
080216dc  1621      movs	r1, #22
080216de  1248      ldr	r0, [pc, #72] ; [0x08021728] = 0x40020000 / f32_bits_interpretation=2.03125
080216e0  f6e7      b	#-20 ; -> 0x080216d0 ; branch_target=0x080216d0
080216e2  0621      movs	r1, #6
080216e4  1248      ldr	r0, [pc, #72] ; [0x08021730] = 0x40020004 / f32_bits_interpretation=2.031250954
080216e6  f3e7      b	#-26 ; -> 0x080216d0 ; branch_target=0x080216d0
080216e8  0621      movs	r1, #6
080216ea  1248      ldr	r0, [pc, #72] ; [0x08021734] = 0x40020400 / f32_bits_interpretation=2.031494141
080216ec  f0e7      b	#-32 ; -> 0x080216d0 ; branch_target=0x080216d0
080216ee  1021      movs	r1, #16
080216f0  0f48      ldr	r0, [pc, #60] ; [0x08021730] = 0x40020004 / f32_bits_interpretation=2.031250954
080216f2  ede7      b	#-38 ; -> 0x080216d0 ; branch_target=0x080216d0
080216f4  1021      movs	r1, #16
080216f6  0f48      ldr	r0, [pc, #60] ; [0x08021734] = 0x40020400 / f32_bits_interpretation=2.031494141
080216f8  eae7      b	#-44 ; -> 0x080216d0 ; branch_target=0x080216d0
080216fa  1621      movs	r1, #22
080216fc  0d48      ldr	r0, [pc, #52] ; [0x08021734] = 0x40020400 / f32_bits_interpretation=2.031494141
080216fe  e7e7      b	#-50 ; -> 0x080216d0 ; branch_target=0x080216d0
08021700  0021      movs	r1, #0
08021702  0d48      ldr	r0, [pc, #52] ; [0x08021738] = 0x40020404 / f32_bits_interpretation=2.031495094
08021704  e4e7      b	#-56 ; -> 0x080216d0 ; branch_target=0x080216d0
08021706  0621      movs	r1, #6
08021708  0b48      ldr	r0, [pc, #44] ; [0x08021738] = 0x40020404 / f32_bits_interpretation=2.031495094
0802170a  e1e7      b	#-62 ; -> 0x080216d0 ; branch_target=0x080216d0
0802170c  1021      movs	r1, #16
0802170e  0a48      ldr	r0, [pc, #40] ; [0x08021738] = 0x40020404 / f32_bits_interpretation=2.031495094
08021710  dee7      b	#-68 ; -> 0x080216d0 ; branch_target=0x080216d0
08021712  1621      movs	r1, #22
08021714  0848      ldr	r0, [pc, #32] ; [0x08021738] = 0x40020404 / f32_bits_interpretation=2.031495094
08021716  dbe7      b	#-74 ; -> 0x080216d0 ; branch_target=0x080216d0
