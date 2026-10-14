; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
0802cd0c  494b      ldr	r3, [pc, #292] ; [0x0802ce34] = 0x20002430
0802cd0e  f0ee406a  vmov.f32	s13, s0
0802cd12  b6ee087a  vmov.f32	s14, #7.500000e-01
0802cd16  dfed484a  vldr	s9, [pc, #288] ; [0x0802ce38] = 0x3c800000 / f32_bits_interpretation=0.015625
0802cd1a  b7ee044a  vmov.f32	s8, #1.250000e+00
0802cd1e  9fed475a  vldr	s10, [pc, #284] ; [0x0802ce3c] = 0x4e7de250 / f32_bits_interpretation=1064866816
0802cd22  feeecb6a  vcvt.s32.f32	s13, s13, #10
0802cd26  20ee070a  vmul.f32	s0, s0, s14
0802cd2a  70b5      push	{r4, r5, r6, lr}
0802cd2c  444c      ldr	r4, [pc, #272] ; [0x0802ce40] = 0x20000a20
0802cd2e  34ee404a  vsub.f32	s8, s8, s0
0802cd32  1968      ldr	r1, [r3]
0802cd34  04eb0110  add.w	r0, r4, r1, lsl #4
0802cd38  0901      lsls	r1, r1, #4
0802cd3a  d0e90123  ldrd	r2, r3, [r0, #4]
0802cd3e  03fb02f2  mul	r2, r3, r2
0802cd42  531e      subs	r3, r2, #1
0802cd44  4fea821e  lsl.w	lr, r2, #6
0802cd48  07ee903a  vmov	s15, r3
0802cd4c  3d4b      ldr	r3, [pc, #244] ; [0x0802ce44] = 0x20002438
0802cd4e  93ed006a  vldr	s12, [r3]
0802cd52  f8eee77a  vcvt.f32.s32	s15, s15
0802cd56  b8eec66a  vcvt.f32.s32	s12, s12
0802cd5a  a7eea06a  vfma.f32	s12, s15, s1
0802cd5e  fdeec67a  vcvt.s32.f32	s15, s12
0802cd62  17ee900a  vmov	r0, s15
0802cd66  f6eec67a  vrintz.f32	s15, s12
0802cd6a  36ee676a  vsub.f32	s12, s12, s15
0802cd6e  8301      lsls	r3, r0, #6
0802cd70  8242      cmp	r2, r0
0802cd72  00f10105  add.w	r5, r0, #1
0802cd76  f5ee007a  vmov.f32	s15, #2.500000e-01
0802cd7a  03f1400c  add.w	r12, r3, #64
0802cd7e  6058      ldr	r0, [r4, r1]
0802cd80  d8bf      it	le
0802cd82  a3eb0e03  suble.w	r3, r3, lr
0802cd86  40f2ff31  movw	r1, #1023
0802cd8a  aa42      cmp	r2, r5
0802cd8c  16ee905a  vmov	r5, s13
0802cd90  b4ee670a  vcmp.f32	s0, s15
0802cd94  2c4c      ldr	r4, [pc, #176] ; [0x0802ce48] = 0x08042a74
0802cd96  d8bf      it	le
0802cd98  aceb0e0c  suble.w	r12, r12, lr
0802cd9c  8d42      cmp	r5, r1
0802cd9e  00eb0302  add.w	r2, r0, r3
0802cda2  2a4b      ldr	r3, [pc, #168] ; [0x0802ce4c] = 0x20002a40
0802cda4  a8bf      it	ge
0802cda6  0d46      movge	r5, r1
0802cda8  2949      ldr	r1, [pc, #164] ; [0x0802ce50] = 0x20002f74
0802cdaa  f1ee10fa  vmrs	APSR_nzcv, fpscr
0802cdae  6044      add	r0, r12
0802cdb0  04eb8504  add.w	r4, r4, r5, lsl #2
0802cdb4  0968      ldr	r1, [r1]
0802cdb6  03f5807c  add.w	r12, r3, #256
0802cdba  d4ed005a  vldr	s11, [r4]
0802cdbe  01eb8202  add.w	r2, r1, r2, lsl #2
0802cdc2  18bf      it	ne
0802cdc4  0125      movne	r5, #1
0802cdc6  01eb8001  add.w	r1, r1, r0, lsl #2
0802cdca  08bf      it	eq
0802cdcc  0025      moveq	r5, #0
0802cdce  2148      ldr	r0, [pc, #132] ; [0x0802ce54] = 0x20000d40
0802cdd0  214c      ldr	r4, [pc, #132] ; [0x0802ce58] = 0xc0876c0b / f32_bits_interpretation=-4.231938839
0802cdd2  f2ec017a  vldmia	r2!, {s15}
0802cdd6  f0ee456a  vmov.f32	s13, s10
0802cdda  b1ec017a  vldmia	r1!, {s14}
0802cdde  37ee677a  vsub.f32	s14, s14, s15
0802cde2  e6ee077a  vfma.f32	s15, s12, s14
0802cde6  17ee906a  vmov	r6, s15
0802cdea  f5ee407a  vcmp.f32	s15, #0
0802cdee  2644      add	r6, r4
0802cdf0  f1ee10fa  vmrs	APSR_nzcv, fpscr
0802cdf4  07ee106a  vmov	s14, r6
0802cdf8  b8eec77a  vcvt.f32.s32	s14, s14
0802cdfc  e5ee876a  vfma.f32	s13, s11, s14
0802ce00  0ddd      ble	#26 ; -> 0x0802ce1e ; branch_target=0x0802ce1e
0802ce02  65b1      cbz	r5, #24 ; -> 0x0802ce1e ; branch_target=0x0802ce1e
0802ce04  fdeee66a  vcvt.s32.f32	s13, s13
0802ce08  f3ec017a  vldmia	r3!, {s15}
0802ce0c  9c45      cmp	r12, r3
0802ce0e  d4ee267a  vfnms.f32	s15, s8, s13
0802ce12  67eea47a  vmul.f32	s15, s15, s9
0802ce16  e0ec017a  vstmia	r0!, {s15}
0802ce1a  dad1      bne	#-76 ; -> 0x0802cdd2 ; branch_target=0x0802cdd2
0802ce1c  70bd      pop	{r4, r5, r6, pc}
0802ce1e  b3ec017a  vldmia	r3!, {s14}
0802ce22  77eec77a  vsub.f32	s15, s15, s14
0802ce26  6345      cmp	r3, r12
0802ce28  67eea47a  vmul.f32	s15, s15, s9
0802ce2c  e0ec017a  vstmia	r0!, {s15}
0802ce30  cfd1      bne	#-98 ; -> 0x0802cdd2 ; branch_target=0x0802cdd2
0802ce32  70bd      pop	{r4, r5, r6, pc}
