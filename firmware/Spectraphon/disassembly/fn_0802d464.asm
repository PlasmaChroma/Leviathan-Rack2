; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
0802d464  ae4b      ldr	r3, [pc, #696] ; [0x0802d720] = 0x200144d4
0802d466  10b5      push	{r4, lr}
0802d468  1a68      ldr	r2, [r3]
0802d46a  002a      cmp	r2, #0
0802d46c  40f33881  ble.w	#624 ; -> 0x0802d6e0 ; branch_target=0x0802d6e0
0802d470  ac4b      ldr	r3, [pc, #688] ; [0x0802d724] = 0x20002eec
0802d472  0120      movs	r0, #1
0802d474  ac49      ldr	r1, [pc, #688] ; [0x0802d728] = 0x20002ef0
0802d476  1b68      ldr	r3, [r3]
0802d478  0860      str	r0, [r1]
0802d47a  13f4004f  tst.w	r3, #32768
0802d47e  4ff07f03  mov.w	r3, #127
0802d482  47d0      beq	#142 ; -> 0x0802d514 ; branch_target=0x0802d514
0802d484  4ff08042  mov.w	r2, #1073741824
0802d488  a848      ldr	r0, [pc, #672] ; [0x0802d72c] = 0x40001800 / f32_bits_interpretation=2.001464844
0802d48a  a949      ldr	r1, [pc, #676] ; [0x0802d730] = 0x40000800 / f32_bits_interpretation=2.000488281
0802d48c  4363      str	r3, [r0, #52]
0802d48e  1364      str	r3, [r2, #64]
0802d490  d363      str	r3, [r2, #60]
0802d492  cb63      str	r3, [r1, #60]
0802d494  a74c      ldr	r4, [pc, #668] ; [0x0802d734] = 0x20002f50
0802d496  a378      ldrb	r3, [r4, #2]
0802d498  002b      cmp	r3, #0
0802d49a  40f08180  bne.w	#258 ; -> 0x0802d5a0 ; branch_target=0x0802d5a0
0802d49e  a64b      ldr	r3, [pc, #664] ; [0x0802d738] = 0x20002e84
0802d4a0  1b68      ldr	r3, [r3]
0802d4a2  1bb1      cbz	r3, #6 ; -> 0x0802d4ac ; branch_target=0x0802d4ac
0802d4a4  9f4b      ldr	r3, [pc, #636] ; [0x0802d724] = 0x20002eec
0802d4a6  1b68      ldr	r3, [r3]
0802d4a8  d904      lsls	r1, r3, #19
0802d4aa  79d4      bmi	#242 ; -> 0x0802d5a0 ; branch_target=0x0802d5a0
0802d4ac  0122      movs	r2, #1
0802d4ae  4ff48051  mov.w	r1, #4096
0802d4b2  a248      ldr	r0, [pc, #648] ; [0x0802d73c] = 0x58020c00
0802d4b4  f5f7aaff  bl	#-41132 ; -> 0x0802340c ; branch_target=0x0802340c
0802d4b8  e378      ldrb	r3, [r4, #3]
0802d4ba  002b      cmp	r3, #0
0802d4bc  79d1      bne	#242 ; -> 0x0802d5b2 ; branch_target=0x0802d5b2
0802d4be  a04b      ldr	r3, [pc, #640] ; [0x0802d740] = 0x20002e80
0802d4c0  1b68      ldr	r3, [r3]
0802d4c2  1bb1      cbz	r3, #6 ; -> 0x0802d4cc ; branch_target=0x0802d4cc
0802d4c4  974b      ldr	r3, [pc, #604] ; [0x0802d724] = 0x20002eec
0802d4c6  1b68      ldr	r3, [r3]
0802d4c8  da04      lsls	r2, r3, #19
0802d4ca  72d4      bmi	#228 ; -> 0x0802d5b2 ; branch_target=0x0802d5b2
0802d4cc  0122      movs	r2, #1
0802d4ce  4ff40051  mov.w	r1, #8192
0802d4d2  9a48      ldr	r0, [pc, #616] ; [0x0802d73c] = 0x58020c00
0802d4d4  f5f79aff  bl	#-41164 ; -> 0x0802340c ; branch_target=0x0802340c
0802d4d8  2379      ldrb	r3, [r4, #4]
0802d4da  012b      cmp	r3, #1
0802d4dc  72d0      beq	#228 ; -> 0x0802d5c4 ; branch_target=0x0802d5c4
0802d4de  002b      cmp	r3, #0
0802d4e0  00f01781  beq.w	#558 ; -> 0x0802d712 ; branch_target=0x0802d712
0802d4e4  022b      cmp	r3, #2
0802d4e6  00f00f81  beq.w	#542 ; -> 0x0802d708 ; branch_target=0x0802d708
0802d4ea  964a      ldr	r2, [pc, #600] ; [0x0802d744] = 0x200023e8
0802d4ec  964b      ldr	r3, [pc, #600] ; [0x0802d748] = 0x200023e0
0802d4ee  d2ed006a  vldr	s13, [r2]
0802d4f2  93ed007a  vldr	s14, [r3]
0802d4f6  c6ee877a  vdiv.f32	s15, s13, s14
0802d4fa  f5ee407a  vcmp.f32	s15, #0
0802d4fe  f1ee10fa  vmrs	APSR_nzcv, fpscr
0802d502  66d1      bne	#204 ; -> 0x0802d5d2 ; branch_target=0x0802d5d2
0802d504  0022      movs	r2, #0
0802d506  4ff48071  mov.w	r1, #256
0802d50a  9048      ldr	r0, [pc, #576] ; [0x0802d74c] = 0x58020000
0802d50c  bde81040  pop.w	{r4, lr}
0802d510  f5f77cbf  b.w	#-41224 ; -> 0x0802340c ; branch_target=0x0802340c
0802d514  4ff08041  mov.w	r1, #1073741824
0802d518  844c      ldr	r4, [pc, #528] ; [0x0802d72c] = 0x40001800 / f32_bits_interpretation=2.001464844
0802d51a  8548      ldr	r0, [pc, #532] ; [0x0802d730] = 0x40000800 / f32_bits_interpretation=2.000488281
0802d51c  6363      str	r3, [r4, #52]
0802d51e  0b64      str	r3, [r1, #64]
0802d520  cb63      str	r3, [r1, #60]
0802d522  1146      mov	r1, r2
0802d524  c363      str	r3, [r0, #60]
0802d526  0c07      lsls	r4, r1, #28
0802d528  01f00103  and	r3, r1, #1
0802d52c  02d5      bpl	#4 ; -> 0x0802d534 ; branch_target=0x0802d534
0802d52e  7f48      ldr	r0, [pc, #508] ; [0x0802d72c] = 0x40001800 / f32_bits_interpretation=2.001464844
0802d530  0024      movs	r4, #0
0802d532  4463      str	r4, [r0, #52]
0802d534  4807      lsls	r0, r1, #29
0802d536  00f1b481  bmi.w	#872 ; -> 0x0802d8a2 ; branch_target=0x0802d8a2
0802d53a  8907      lsls	r1, r1, #30
0802d53c  03d5      bpl	#6 ; -> 0x0802d546 ; branch_target=0x0802d546
0802d53e  4ff08041  mov.w	r1, #1073741824
0802d542  0020      movs	r0, #0
0802d544  0864      str	r0, [r1, #64]
0802d546  002b      cmp	r3, #0
0802d548  40f07281  bne.w	#740 ; -> 0x0802d830 ; branch_target=0x0802d830
0802d54c  002a      cmp	r2, #0
0802d54e  a1d1      bne	#-190 ; -> 0x0802d494 ; branch_target=0x0802d494
0802d550  7f4c      ldr	r4, [pc, #508] ; [0x0802d750] = 0x2001348c
0802d552  804b      ldr	r3, [pc, #512] ; [0x0802d754] = 0x20002ec4
0802d554  b4f90420  ldrsh.w	r2, [r4, #4]
0802d558  1b68      ldr	r3, [r3]
0802d55a  9a42      cmp	r2, r3
0802d55c  00f06d81  beq.w	#730 ; -> 0x0802d83a ; branch_target=0x0802d83a
0802d560  704b      ldr	r3, [pc, #448] ; [0x0802d724] = 0x20002eec
0802d562  1b68      ldr	r3, [r3]
0802d564  9b04      lsls	r3, r3, #18
0802d566  40f17581  bpl.w	#746 ; -> 0x0802d854 ; branch_target=0x0802d854
0802d56a  0122      movs	r2, #1
0802d56c  4021      movs	r1, #64
0802d56e  7a48      ldr	r0, [pc, #488] ; [0x0802d758] = 0x58020800
0802d570  f5f74cff  bl	#-41320 ; -> 0x0802340c ; branch_target=0x0802340c
0802d574  794b      ldr	r3, [pc, #484] ; [0x0802d75c] = 0x20002ec0
0802d576  b4f90620  ldrsh.w	r2, [r4, #6]
0802d57a  1b68      ldr	r3, [r3]
0802d57c  9a42      cmp	r2, r3
0802d57e  00f07581  beq.w	#746 ; -> 0x0802d86c ; branch_target=0x0802d86c
0802d582  684b      ldr	r3, [pc, #416] ; [0x0802d724] = 0x20002eec
0802d584  1b68      ldr	r3, [r3]
0802d586  9804      lsls	r0, r3, #18
0802d588  40f17d81  bpl.w	#762 ; -> 0x0802d886 ; branch_target=0x0802d886
0802d58c  694c      ldr	r4, [pc, #420] ; [0x0802d734] = 0x20002f50
0802d58e  0122      movs	r2, #1
0802d590  8021      movs	r1, #128
0802d592  7148      ldr	r0, [pc, #452] ; [0x0802d758] = 0x58020800
0802d594  f5f73aff  bl	#-41356 ; -> 0x0802340c ; branch_target=0x0802340c
0802d598  a378      ldrb	r3, [r4, #2]
0802d59a  002b      cmp	r3, #0
0802d59c  3ff47faf  beq.w	#-258 ; -> 0x0802d49e ; branch_target=0x0802d49e
0802d5a0  0022      movs	r2, #0
0802d5a2  4ff48051  mov.w	r1, #4096
0802d5a6  6548      ldr	r0, [pc, #404] ; [0x0802d73c] = 0x58020c00
0802d5a8  f5f730ff  bl	#-41376 ; -> 0x0802340c ; branch_target=0x0802340c
0802d5ac  e378      ldrb	r3, [r4, #3]
0802d5ae  002b      cmp	r3, #0
0802d5b0  85d0      beq	#-246 ; -> 0x0802d4be ; branch_target=0x0802d4be
0802d5b2  0022      movs	r2, #0
0802d5b4  4ff40051  mov.w	r1, #8192
0802d5b8  6048      ldr	r0, [pc, #384] ; [0x0802d73c] = 0x58020c00
0802d5ba  f5f727ff  bl	#-41394 ; -> 0x0802340c ; branch_target=0x0802340c
0802d5be  2379      ldrb	r3, [r4, #4]
0802d5c0  012b      cmp	r3, #1
0802d5c2  8cd1      bne	#-232 ; -> 0x0802d4de ; branch_target=0x0802d4de
0802d5c4  0022      movs	r2, #0
0802d5c6  4ff40041  mov.w	r1, #32768
0802d5ca  6548      ldr	r0, [pc, #404] ; [0x0802d760] = 0x58020400
0802d5cc  f5f71eff  bl	#-41412 ; -> 0x0802340c ; branch_target=0x0802340c
0802d5d0  8be7      b	#-234 ; -> 0x0802d4ea ; branch_target=0x0802d4ea
0802d5d2  b7ee007a  vmov.f32	s14, #1.000000e+00
0802d5d6  f4eec77a  vcmpe.f32	s15, s14
0802d5da  f1ee10fa  vmrs	APSR_nzcv, fpscr
0802d5de  40f1e180  bpl.w	#450 ; -> 0x0802d7a4 ; branch_target=0x0802d7a4
0802d5e2  f6ee006a  vmov.f32	s13, #5.000000e-01
0802d5e6  b0ee677a  vmov.f32	s14, s15
0802d5ea  77eea77a  vadd.f32	s15, s15, s15
0802d5ee  b4eee67a  vcmpe.f32	s14, s13
0802d5f2  f1ee10fa  vmrs	APSR_nzcv, fpscr
0802d5f6  f6d4      bmi	#-20 ; -> 0x0802d5e6 ; branch_target=0x0802d5e6
0802d5f8  9fed5a6a  vldr	s12, [pc, #360] ; [0x0802d764] = 0x3efd70a4 / f32_bits_interpretation=0.4950000048
0802d5fc  dfed5a6a  vldr	s13, [pc, #360] ; [0x0802d768] = 0x3f0147ae / f32_bits_interpretation=0.5049999952
0802d600  b4eec67a  vcmpe.f32	s14, s12
0802d604  f1ee10fa  vmrs	APSR_nzcv, fpscr
0802d608  b4eee67a  vcmpe.f32	s14, s13
0802d60c  ccbf      ite	gt
0802d60e  0123      movgt	r3, #1
0802d610  0023      movle	r3, #0
0802d612  f1ee10fa  vmrs	APSR_nzcv, fpscr
0802d616  03f00103  and	r3, r3, #1
0802d61a  58bf      it	pl
0802d61c  0023      movpl	r3, #0
0802d61e  002b      cmp	r3, #0
0802d620  7ff470af  bne.w	#-288 ; -> 0x0802d504 ; branch_target=0x0802d504
0802d624  9fed517a  vldr	s14, [pc, #324] ; [0x0802d76c] = 0x3f9eb852 / f32_bits_interpretation=1.24000001
0802d628  f4ee477a  vcmp.f32	s15, s14
0802d62c  f1ee10fa  vmrs	APSR_nzcv, fpscr
0802d630  0edd      ble	#28 ; -> 0x0802d650 ; branch_target=0x0802d650
0802d632  9fed4f7a  vldr	s14, [pc, #316] ; [0x0802d770] = 0x3fa147ae / f32_bits_interpretation=1.25999999
0802d636  f4ee477a  vcmp.f32	s15, s14
0802d63a  f1ee10fa  vmrs	APSR_nzcv, fpscr
0802d63e  07d5      bpl	#14 ; -> 0x0802d650 ; branch_target=0x0802d650
0802d640  0022      movs	r2, #0
0802d642  4ff48061  mov.w	r1, #1024
0802d646  4148      ldr	r0, [pc, #260] ; [0x0802d74c] = 0x58020000
0802d648  bde81040  pop.w	{r4, lr}
0802d64c  f5f7debe  b.w	#-41540 ; -> 0x0802340c ; branch_target=0x0802340c
0802d650  9fed487a  vldr	s14, [pc, #288] ; [0x0802d774] = 0x3fa95810 / f32_bits_interpretation=1.322999954
0802d654  f4ee477a  vcmp.f32	s15, s14
0802d658  f1ee10fa  vmrs	APSR_nzcv, fpscr
0802d65c  07dd      ble	#14 ; -> 0x0802d66e ; branch_target=0x0802d66e
0802d65e  9fed467a  vldr	s14, [pc, #280] ; [0x0802d778] = 0x3fabe76d / f32_bits_interpretation=1.343000054
0802d662  f4eec77a  vcmpe.f32	s15, s14
0802d666  f1ee10fa  vmrs	APSR_nzcv, fpscr
0802d66a  3ff54baf  bmi.w	#-362 ; -> 0x0802d504 ; branch_target=0x0802d504
0802d66e  9fed437a  vldr	s14, [pc, #268] ; [0x0802d77c] = 0x3fbeb852 / f32_bits_interpretation=1.49000001
0802d672  f4ee477a  vcmp.f32	s15, s14
0802d676  f1ee10fa  vmrs	APSR_nzcv, fpscr
0802d67a  07dd      ble	#14 ; -> 0x0802d68c ; branch_target=0x0802d68c
0802d67c  9fed407a  vldr	s14, [pc, #256] ; [0x0802d780] = 0x3fc147ae / f32_bits_interpretation=1.50999999
0802d680  f4eec77a  vcmpe.f32	s15, s14
0802d684  f1ee10fa  vmrs	APSR_nzcv, fpscr
0802d688  3ff53caf  bmi.w	#-392 ; -> 0x0802d504 ; branch_target=0x0802d504
0802d68c  9fed3d7a  vldr	s14, [pc, #244] ; [0x0802d784] = 0x3fcb851f / f32_bits_interpretation=1.590000033
0802d690  f4ee477a  vcmp.f32	s15, s14
0802d694  f1ee10fa  vmrs	APSR_nzcv, fpscr
0802d698  06dd      ble	#12 ; -> 0x0802d6a8 ; branch_target=0x0802d6a8
0802d69a  9fed3b7a  vldr	s14, [pc, #236] ; [0x0802d788] = 0x3fce147b / f32_bits_interpretation=1.610000014
0802d69e  f4eec77a  vcmpe.f32	s15, s14
0802d6a2  f1ee10fa  vmrs	APSR_nzcv, fpscr
0802d6a6  cbd4      bmi	#-106 ; -> 0x0802d640 ; branch_target=0x0802d640
0802d6a8  9fed387a  vldr	s14, [pc, #224] ; [0x0802d78c] = 0x3fd3f7cf / f32_bits_interpretation=1.656000018
0802d6ac  f4ee477a  vcmp.f32	s15, s14
0802d6b0  f1ee10fa  vmrs	APSR_nzcv, fpscr
0802d6b4  06dd      ble	#12 ; -> 0x0802d6c4 ; branch_target=0x0802d6c4
0802d6b6  9fed367a  vldr	s14, [pc, #216] ; [0x0802d790] = 0x3fd6872b / f32_bits_interpretation=1.675999999
0802d6ba  f4eec77a  vcmpe.f32	s15, s14
0802d6be  f1ee10fa  vmrs	APSR_nzcv, fpscr
0802d6c2  bdd4      bmi	#-134 ; -> 0x0802d640 ; branch_target=0x0802d640
0802d6c4  0122      movs	r2, #1
0802d6c6  4ff48061  mov.w	r1, #1024
0802d6ca  2048      ldr	r0, [pc, #128] ; [0x0802d74c] = 0x58020000
0802d6cc  f5f79efe  bl	#-41668 ; -> 0x0802340c ; branch_target=0x0802340c
0802d6d0  0122      movs	r2, #1
0802d6d2  4ff48071  mov.w	r1, #256
0802d6d6  1d48      ldr	r0, [pc, #116] ; [0x0802d74c] = 0x58020000
0802d6d8  bde81040  pop.w	{r4, lr}
0802d6dc  f5f796be  b.w	#-41684 ; -> 0x0802340c ; branch_target=0x0802340c
0802d6e0  2c4b      ldr	r3, [pc, #176] ; [0x0802d794] = 0x20002f08
0802d6e2  1b68      ldr	r3, [r3]
0802d6e4  002b      cmp	r3, #0
0802d6e6  76dd      ble	#236 ; -> 0x0802d7d6 ; branch_target=0x0802d7d6
0802d6e8  0f49      ldr	r1, [pc, #60] ; [0x0802d728] = 0x20002ef0
0802d6ea  0124      movs	r4, #1
0802d6ec  7f23      movs	r3, #127
0802d6ee  4ff08040  mov.w	r0, #1073741824
0802d6f2  0c60      str	r4, [r1]
0802d6f4  2849      ldr	r1, [pc, #160] ; [0x0802d798] = 0x20002430
0802d6f6  0d4c      ldr	r4, [pc, #52] ; [0x0802d72c] = 0x40001800 / f32_bits_interpretation=2.001464844
0802d6f8  0968      ldr	r1, [r1]
0802d6fa  6363      str	r3, [r4, #52]
0802d6fc  a4f58054  sub.w	r4, r4, #4096
0802d700  0364      str	r3, [r0, #64]
0802d702  c363      str	r3, [r0, #60]
0802d704  e363      str	r3, [r4, #60]
0802d706  0ee7      b	#-484 ; -> 0x0802d526 ; branch_target=0x0802d526
0802d708  064b      ldr	r3, [pc, #24] ; [0x0802d724] = 0x20002eec
0802d70a  1b68      ldr	r3, [r3]
0802d70c  9b04      lsls	r3, r3, #18
0802d70e  3ff559af  bmi.w	#-334 ; -> 0x0802d5c4 ; branch_target=0x0802d5c4
0802d712  0122      movs	r2, #1
0802d714  4ff40041  mov.w	r1, #32768
0802d718  1148      ldr	r0, [pc, #68] ; [0x0802d760] = 0x58020400
0802d71a  f5f777fe  bl	#-41746 ; -> 0x0802340c ; branch_target=0x0802340c
0802d71e  e4e6      b	#-568 ; -> 0x0802d4ea ; branch_target=0x0802d4ea
0802d7a4  b0ee007a  vmov.f32	s14, #2.000000e+00
0802d7a8  f4eec77a  vcmpe.f32	s15, s14
0802d7ac  f1ee10fa  vmrs	APSR_nzcv, fpscr
0802d7b0  7bdb      blt	#246 ; -> 0x0802d8aa ; branch_target=0x0802d8aa
0802d7b2  b6ee006a  vmov.f32	s12, #5.000000e-01
0802d7b6  f1ee006a  vmov.f32	s13, #4.000000e+00
0802d7ba  b0ee677a  vmov.f32	s14, s15
0802d7be  67ee867a  vmul.f32	s15, s15, s12
0802d7c2  b4eee67a  vcmpe.f32	s14, s13
0802d7c6  f1ee10fa  vmrs	APSR_nzcv, fpscr
0802d7ca  f6da      bge	#-20 ; -> 0x0802d7ba ; branch_target=0x0802d7ba
0802d7cc  1fed0d6a  vldr	s12, [pc, #-52] ; [0x0802d79c] = 0x3ffd70a4 / f32_bits_interpretation=1.980000019
0802d7d0  5fed0d6a  vldr	s13, [pc, #-52] ; [0x0802d7a0] = 0x400147ae / f32_bits_interpretation=2.019999981
0802d7d4  14e7      b	#-472 ; -> 0x0802d600 ; branch_target=0x0802d600
0802d7d6  3a4b      ldr	r3, [pc, #232] ; [0x0802d8c0] = 0x20002f04
0802d7d8  1b68      ldr	r3, [r3]
0802d7da  002b      cmp	r3, #0
0802d7dc  4ff07f03  mov.w	r3, #127
0802d7e0  06dd      ble	#12 ; -> 0x0802d7f0 ; branch_target=0x0802d7f0
0802d7e2  3849      ldr	r1, [pc, #224] ; [0x0802d8c4] = 0x20002ef0
0802d7e4  0124      movs	r4, #1
0802d7e6  4ff08040  mov.w	r0, #1073741824
0802d7ea  0c60      str	r4, [r1]
0802d7ec  3649      ldr	r1, [pc, #216] ; [0x0802d8c8] = 0x20000b20
0802d7ee  82e7      b	#-252 ; -> 0x0802d6f6 ; branch_target=0x0802d6f6
0802d7f0  3448      ldr	r0, [pc, #208] ; [0x0802d8c4] = 0x20002ef0
0802d7f2  0024      movs	r4, #0
0802d7f4  4ff08041  mov.w	r1, #1073741824
0802d7f8  0460      str	r4, [r0]
0802d7fa  3448      ldr	r0, [pc, #208] ; [0x0802d8cc] = 0x40001800 / f32_bits_interpretation=2.001464844
0802d7fc  4363      str	r3, [r0, #52]
0802d7fe  0b64      str	r3, [r1, #64]
0802d800  cb63      str	r3, [r1, #60]
0802d802  c1f83c38  str.w	r3, [r1, #2108]
0802d806  324b      ldr	r3, [pc, #200] ; [0x0802d8d0] = 0x20002e8c
0802d808  1b68      ldr	r3, [r3]
0802d80a  9907      lsls	r1, r3, #30
0802d80c  00d5      bpl	#0 ; -> 0x0802d810 ; branch_target=0x0802d810
0802d80e  4463      str	r4, [r0, #52]
0802d810  dc07      lsls	r4, r3, #31
0802d812  02d5      bpl	#4 ; -> 0x0802d81a ; branch_target=0x0802d81a
0802d814  2f4b      ldr	r3, [pc, #188] ; [0x0802d8d4] = 0x40000800 / f32_bits_interpretation=2.000488281
0802d816  0021      movs	r1, #0
0802d818  d963      str	r1, [r3, #60]
0802d81a  2f4b      ldr	r3, [pc, #188] ; [0x0802d8d8] = 0x20002e88
0802d81c  1b68      ldr	r3, [r3]
0802d81e  9807      lsls	r0, r3, #30
0802d820  03d5      bpl	#6 ; -> 0x0802d82a ; branch_target=0x0802d82a
0802d822  4ff08041  mov.w	r1, #1073741824
0802d826  0020      movs	r0, #0
0802d828  0864      str	r0, [r1, #64]
0802d82a  d907      lsls	r1, r3, #31
0802d82c  7ff58eae  bpl.w	#-740 ; -> 0x0802d54c ; branch_target=0x0802d54c
0802d830  4ff08043  mov.w	r3, #1073741824
0802d834  0021      movs	r1, #0
0802d836  d963      str	r1, [r3, #60]
0802d838  88e6      b	#-752 ; -> 0x0802d54c ; branch_target=0x0802d54c
0802d83a  284b      ldr	r3, [pc, #160] ; [0x0802d8dc] = 0x20002f60
0802d83c  284a      ldr	r2, [pc, #160] ; [0x0802d8e0] = 0x2000242c
0802d83e  1b78      ldrb	r3, [r3]
0802d840  1268      ldr	r2, [r2]
0802d842  012b      cmp	r3, #1
0802d844  29d0      beq	#82 ; -> 0x0802d89a ; branch_target=0x0802d89a
0802d846  274b      ldr	r3, [pc, #156] ; [0x0802d8e4] = 0x200023d0
0802d848  1b68      ldr	r3, [r3]
0802d84a  012b      cmp	r3, #1
0802d84c  25d0      beq	#74 ; -> 0x0802d89a ; branch_target=0x0802d89a
0802d84e  002a      cmp	r2, #0
0802d850  7ff78bae  ble.w	#-746 ; -> 0x0802d56a ; branch_target=0x0802d56a
0802d854  0022      movs	r2, #0
0802d856  4021      movs	r1, #64
0802d858  2348      ldr	r0, [pc, #140] ; [0x0802d8e8] = 0x58020800
0802d85a  f5f7d7fd  bl	#-42066 ; -> 0x0802340c ; branch_target=0x0802340c
0802d85e  234b      ldr	r3, [pc, #140] ; [0x0802d8ec] = 0x20002ec0
0802d860  b4f90620  ldrsh.w	r2, [r4, #6]
0802d864  1b68      ldr	r3, [r3]
0802d866  9a42      cmp	r2, r3
0802d868  7ff48bae  bne.w	#-746 ; -> 0x0802d582 ; branch_target=0x0802d582
0802d86c  1b4b      ldr	r3, [pc, #108] ; [0x0802d8dc] = 0x20002f60
0802d86e  204a      ldr	r2, [pc, #128] ; [0x0802d8f0] = 0x20002428
0802d870  5b78      ldrb	r3, [r3, #1]
0802d872  1268      ldr	r2, [r2]
0802d874  012b      cmp	r3, #1
0802d876  0cd0      beq	#24 ; -> 0x0802d892 ; branch_target=0x0802d892
0802d878  1e4b      ldr	r3, [pc, #120] ; [0x0802d8f4] = 0x20000820
0802d87a  1b68      ldr	r3, [r3]
0802d87c  012b      cmp	r3, #1
0802d87e  08d0      beq	#16 ; -> 0x0802d892 ; branch_target=0x0802d892
0802d880  002a      cmp	r2, #0
0802d882  7ff783ae  ble.w	#-762 ; -> 0x0802d58c ; branch_target=0x0802d58c
0802d886  0022      movs	r2, #0
0802d888  8021      movs	r1, #128
0802d88a  1748      ldr	r0, [pc, #92] ; [0x0802d8e8] = 0x58020800
0802d88c  f5f7befd  bl	#-42116 ; -> 0x0802340c ; branch_target=0x0802340c
0802d890  00e6      b	#-1024 ; -> 0x0802d494 ; branch_target=0x0802d494
0802d892  002a      cmp	r2, #0
0802d894  3ff77aae  bgt.w	#-780 ; -> 0x0802d58c ; branch_target=0x0802d58c
0802d898  f5e7      b	#-22 ; -> 0x0802d886 ; branch_target=0x0802d886
0802d89a  002a      cmp	r2, #0
0802d89c  3ff765ae  bgt.w	#-822 ; -> 0x0802d56a ; branch_target=0x0802d56a
0802d8a0  d8e7      b	#-80 ; -> 0x0802d854 ; branch_target=0x0802d854
0802d8a2  0c48      ldr	r0, [pc, #48] ; [0x0802d8d4] = 0x40000800 / f32_bits_interpretation=2.000488281
0802d8a4  0024      movs	r4, #0
0802d8a6  c463      str	r4, [r0, #60]
0802d8a8  47e6      b	#-882 ; -> 0x0802d53a ; branch_target=0x0802d53a
0802d8aa  dfed136a  vldr	s13, [pc, #76] ; [0x0802d8f8] = 0x3f7d70a4 / f32_bits_interpretation=0.9900000095
0802d8ae  9fed137a  vldr	s14, [pc, #76] ; [0x0802d8fc] = 0x3f8147ae / f32_bits_interpretation=1.00999999
0802d8b2  f4eee67a  vcmpe.f32	s15, s13
0802d8b6  f1ee10fa  vmrs	APSR_nzcv, fpscr
0802d8ba  f4eec77a  vcmpe.f32	s15, s14
0802d8be  a5e6      b	#-694 ; -> 0x0802d60c ; branch_target=0x0802d60c
