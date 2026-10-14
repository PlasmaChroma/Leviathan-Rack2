; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
0802ce5c  494b      ldr	r3, [pc, #292] ; [0x0802cf84] = 0x20000b20
0802ce5e  f0ee406a  vmov.f32	s13, s0
0802ce62  b6ee087a  vmov.f32	s14, #7.500000e-01
0802ce66  dfed484a  vldr	s9, [pc, #288] ; [0x0802cf88] = 0x3c800000 / f32_bits_interpretation=0.015625
0802ce6a  b7ee044a  vmov.f32	s8, #1.250000e+00
0802ce6e  9fed475a  vldr	s10, [pc, #284] ; [0x0802cf8c] = 0x4e7de250 / f32_bits_interpretation=1064866816
0802ce72  feeecb6a  vcvt.s32.f32	s13, s13, #10
0802ce76  20ee070a  vmul.f32	s0, s0, s14
0802ce7a  70b5      push	{r4, r5, r6, lr}
0802ce7c  444c      ldr	r4, [pc, #272] ; [0x0802cf90] = 0x20000920
0802ce7e  34ee404a  vsub.f32	s8, s8, s0
0802ce82  1968      ldr	r1, [r3]
0802ce84  04eb0110  add.w	r0, r4, r1, lsl #4
0802ce88  0901      lsls	r1, r1, #4
0802ce8a  d0e90132  ldrd	r3, r2, [r0, #4]
0802ce8e  03fb02f2  mul	r2, r3, r2
0802ce92  531e      subs	r3, r2, #1
0802ce94  4fea821e  lsl.w	lr, r2, #6
0802ce98  07ee903a  vmov	s15, r3
0802ce9c  3d4b      ldr	r3, [pc, #244] ; [0x0802cf94] = 0x20002434
0802ce9e  93ed006a  vldr	s12, [r3]
0802cea2  f8eee77a  vcvt.f32.s32	s15, s15
0802cea6  b8eec66a  vcvt.f32.s32	s12, s12
0802ceaa  a7eea06a  vfma.f32	s12, s15, s1
0802ceae  fdeec67a  vcvt.s32.f32	s15, s12
0802ceb2  17ee900a  vmov	r0, s15
0802ceb6  f6eec67a  vrintz.f32	s15, s12
0802ceba  36ee676a  vsub.f32	s12, s12, s15
0802cebe  8301      lsls	r3, r0, #6
0802cec0  8242      cmp	r2, r0
0802cec2  00f10105  add.w	r5, r0, #1
0802cec6  f5ee007a  vmov.f32	s15, #2.500000e-01
0802ceca  03f1400c  add.w	r12, r3, #64
0802cece  6058      ldr	r0, [r4, r1]
0802ced0  d8bf      it	le
0802ced2  a3eb0e03  suble.w	r3, r3, lr
0802ced6  40f2ff31  movw	r1, #1023
0802ceda  aa42      cmp	r2, r5
0802cedc  16ee905a  vmov	r5, s13
0802cee0  b4ee670a  vcmp.f32	s0, s15
0802cee4  2c4c      ldr	r4, [pc, #176] ; [0x0802cf98] = 0x08042a74
0802cee6  d8bf      it	le
0802cee8  aceb0e0c  suble.w	r12, r12, lr
0802ceec  8d42      cmp	r5, r1
0802ceee  00eb0302  add.w	r2, r0, r3
0802cef2  2a4b      ldr	r3, [pc, #168] ; [0x0802cf9c] = 0x20002c40
0802cef4  a8bf      it	ge
0802cef6  0d46      movge	r5, r1
0802cef8  2949      ldr	r1, [pc, #164] ; [0x0802cfa0] = 0x20002f70
0802cefa  f1ee10fa  vmrs	APSR_nzcv, fpscr
0802cefe  6044      add	r0, r12
0802cf00  04eb8504  add.w	r4, r4, r5, lsl #2
0802cf04  0968      ldr	r1, [r1]
0802cf06  03f5807c  add.w	r12, r3, #256
0802cf0a  d4ed005a  vldr	s11, [r4]
0802cf0e  01eb8202  add.w	r2, r1, r2, lsl #2
0802cf12  18bf      it	ne
0802cf14  0125      movne	r5, #1
0802cf16  01eb8001  add.w	r1, r1, r0, lsl #2
0802cf1a  08bf      it	eq
0802cf1c  0025      moveq	r5, #0
0802cf1e  2148      ldr	r0, [pc, #132] ; [0x0802cfa4] = 0x20000f40
0802cf20  214c      ldr	r4, [pc, #132] ; [0x0802cfa8] = 0xc0876c0b / f32_bits_interpretation=-4.231938839
0802cf22  f2ec017a  vldmia	r2!, {s15}
0802cf26  f0ee456a  vmov.f32	s13, s10
0802cf2a  b1ec017a  vldmia	r1!, {s14}
0802cf2e  37ee677a  vsub.f32	s14, s14, s15
0802cf32  e6ee077a  vfma.f32	s15, s12, s14
0802cf36  17ee906a  vmov	r6, s15
0802cf3a  f5ee407a  vcmp.f32	s15, #0
0802cf3e  2644      add	r6, r4
0802cf40  f1ee10fa  vmrs	APSR_nzcv, fpscr
0802cf44  07ee106a  vmov	s14, r6
0802cf48  b8eec77a  vcvt.f32.s32	s14, s14
0802cf4c  e5ee876a  vfma.f32	s13, s11, s14
0802cf50  0ddd      ble	#26 ; -> 0x0802cf6e ; branch_target=0x0802cf6e
0802cf52  65b1      cbz	r5, #24 ; -> 0x0802cf6e ; branch_target=0x0802cf6e
0802cf54  fdeee66a  vcvt.s32.f32	s13, s13
0802cf58  f3ec017a  vldmia	r3!, {s15}
0802cf5c  9c45      cmp	r12, r3
0802cf5e  d4ee267a  vfnms.f32	s15, s8, s13
0802cf62  67eea47a  vmul.f32	s15, s15, s9
0802cf66  e0ec017a  vstmia	r0!, {s15}
0802cf6a  dad1      bne	#-76 ; -> 0x0802cf22 ; branch_target=0x0802cf22
0802cf6c  70bd      pop	{r4, r5, r6, pc}
0802cf6e  b3ec017a  vldmia	r3!, {s14}
0802cf72  77eec77a  vsub.f32	s15, s15, s14
0802cf76  6345      cmp	r3, r12
0802cf78  67eea47a  vmul.f32	s15, s15, s9
0802cf7c  e0ec017a  vstmia	r0!, {s15}
0802cf80  cfd1      bne	#-98 ; -> 0x0802cf22 ; branch_target=0x0802cf22
0802cf82  70bd      pop	{r4, r5, r6, pc}
