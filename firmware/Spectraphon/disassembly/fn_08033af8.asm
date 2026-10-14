; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08033af8  2de9f04f  push.w	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
08033afc  2ded048b  vpush	{d8, d9}
08033b00  89b0      sub	sp, #36
08033b02  0028      cmp	r0, #0
08033b04  40f0b480  bne.w	#360 ; -> 0x08033c70 ; branch_target=0x08033c70
08033b08  b54d      ldr	r5, [pc, #724] ; [0x08033de0] = 0x20000a20
08033b0a  0346      mov	r3, r0
08033b0c  b54f      ldr	r7, [pc, #724] ; [0x08033de4] = 0x20002f8c
08033b0e  ee68      ldr	r6, [r5, #12]
08033b10  1a46      mov	r2, r3
08033b12  0133      adds	r3, #1
08033b14  9fedb09b  vldr	d9, [pc, #704] ; [0x08033dd8] = 0x00000000 / f64_bits_interpretation=32767
08033b18  4eb1      cbz	r6, #18 ; -> 0x08033b2e ; branch_target=0x08033b2e
08033b1a  102b      cmp	r3, #16
08033b1c  05f11005  add.w	r5, r5, #16
08033b20  00f09f80  beq.w	#318 ; -> 0x08033c62 ; branch_target=0x08033c62
08033b24  ee68      ldr	r6, [r5, #12]
08033b26  1a46      mov	r2, r3
08033b28  0133      adds	r3, #1
08033b2a  002e      cmp	r6, #0
08033b2c  f5d1      bne	#-22 ; -> 0x08033b1a ; branch_target=0x08033b1a
08033b2e  eb60      str	r3, [r5, #12]
08033b30  0093      str	r3, [sp]
08033b32  ad4b      ldr	r3, [pc, #692] ; [0x08033de8] = 0x63657073
08033b34  8df81c60  strb.w	r6, [sp, #28]
08033b38  0493      str	r3, [sp, #16]
08033b3a  03f19753  add.w	r3, r3, #316669952
08033b3e  03f5e013  add.w	r3, r3, #1835008
08033b42  03f2bb63  addw	r3, r3, #1723
08033b46  0693      str	r3, [sp, #24]
08033b48  a84b      ldr	r3, [pc, #672] ; [0x08033dec] = 0x66666667
08033b4a  83fb0210  smull	r1, r0, r3, r2
08033b4e  d117      asrs	r1, r2, #31
08033b50  c1eba001  rsb	r1, r1, r0, asr #2
08033b54  43f26100  movw	r0, #12385
08033b58  adf81400  strh.w	r0, [sp, #20]
08033b5c  01eb8100  add.w	r0, r1, r1, lsl #2
08033b60  3031      adds	r1, #48
08033b62  a2eb4002  sub.w	r2, r2, r0, lsl #1
08033b66  9f48      ldr	r0, [pc, #636] ; [0x08033de4] = 0x20002f8c
08033b68  8df81610  strb.w	r1, [sp, #22]
08033b6c  04a9      add	r1, sp, #16
08033b6e  3032      adds	r2, #48
08033b70  8df81720  strb.w	r2, [sp, #23]
08033b74  0b22      movs	r2, #11
08033b76  f7f7fdf9  bl	#-35846 ; -> 0x0802af74 ; branch_target=0x0802af74
08033b7a  9d49      ldr	r1, [pc, #628] ; [0x08033df0] = 0x73693136
08033b7c  9948      ldr	r0, [pc, #612] ; [0x08033de4] = 0x20002f8c
08033b7e  fff7b7fd  bl	#-1170 ; -> 0x080336f0 ; branch_target=0x080336f0
08033b82  9c4a      ldr	r2, [pc, #624] ; [0x08033df4] = 0x20002e9c
08033b84  d7f80c80  ldr.w	r8, [r7, #12]
08033b88  d2ed007a  vldr	s15, [r2]
08033b8c  009b      ldr	r3, [sp]
08033b8e  f5ee407a  vcmp.f32	s15, #0
08033b92  f1ee10fa  vmrs	APSR_nzcv, fpscr
08033b96  07dd      ble	#14 ; -> 0x08033ba8 ; branch_target=0x08033ba8
08033b98  b7ee007a  vmov.f32	s14, #1.000000e+00
08033b9c  f4ee477a  vcmp.f32	s15, s14
08033ba0  f1ee10fa  vmrs	APSR_nzcv, fpscr
08033ba4  00f10c81  bmi.w	#536 ; -> 0x08033dc0 ; branch_target=0x08033dc0
08033ba8  b7ee008a  vmov.f32	s16, #1.000000e+00
08033bac  d5e90121  ldrd	r2, r1, [r5, #4]
08033bb0  01fb02f2  mul	r2, r1, r2
08033bb4  002a      cmp	r2, #0
08033bb6  40f30981  ble.w	#530 ; -> 0x08033dcc ; branch_target=0x08033dcc
08033bba  03aa      add	r2, sp, #12
08033bbc  4ff00009  mov.w	r9, #0
08033bc0  dff848a2  ldr.w	r10, [pc, #584] ; [0x08033e0c] = 0x60c01000
08033bc4  8c4c      ldr	r4, [pc, #560] ; [0x08033df8] = 0x200135c8
08033bc6  0092      str	r2, [sp]
08033bc8  0193      str	r3, [sp, #4]
08033bca  2b68      ldr	r3, [r5]
08033bcc  dff840b2  ldr.w	r11, [pc, #576] ; [0x08033e10] = 0x200134c8
08033bd0  4b44      add	r3, r9
08033bd2  5946      mov	r1, r11
08033bd4  0aeb8303  add.w	r3, r10, r3, lsl #2
08033bd8  d3ed007a  vldr	s15, [r3]
08033bdc  67ee887a  vmul.f32	s15, s15, s16
08033be0  e1ec017a  vstmia	r1!, {s15}
08033be4  a142      cmp	r1, r4
08033be6  e3ec017a  vstmia	r3!, {s15}
08033bea  f5d1      bne	#-22 ; -> 0x08033bd8 ; branch_target=0x08033bd8
08033bec  3101      lsls	r1, r6, #4
08033bee  3846      mov	r0, r7
08033bf0  08ebe101  add.w	r1, r8, r1, asr #3
08033bf4  21f00301  bic	r1, r1, #3
08033bf8  f7f7a4fe  bl	#-33464 ; -> 0x0802b944 ; branch_target=0x0802b944
08033bfc  7f4b      ldr	r3, [pc, #508] ; [0x08033dfc] = 0x20003466
08033bfe  bbec017a  vldmia	r11!, {s14}
08033c02  b7eec77a  vcvt.f64.f32	d7, s14
08033c06  a345      cmp	r11, r4
08033c08  27ee097b  vmul.f64	d7, d7, d9
08033c0c  bdeec77b  vcvt.s32.f64	s14, d7
08033c10  17ee102a  vmov	r2, s14
08033c14  23f8022f  strh	r2, [r3, #2]!
08033c18  f1d1      bne	#-30 ; -> 0x08033bfe ; branch_target=0x08033bfe
08033c1a  009b      ldr	r3, [sp]
08033c1c  8022      movs	r2, #128
08033c1e  7849      ldr	r1, [pc, #480] ; [0x08033e00] = 0x20003468
08033c20  3846      mov	r0, r7
08033c22  f7f753fc  bl	#-34650 ; -> 0x0802b4cc ; branch_target=0x0802b4cc
08033c26  b0fa80f3  clz	r3, r0
08033c2a  09f14009  add.w	r9, r9, #64
08033c2e  5b09      lsrs	r3, r3, #5
08033c30  06eb8316  add.w	r6, r6, r3, lsl #6
08033c34  d5e90132  ldrd	r3, r2, [r5, #4]
08033c38  02fb03f3  mul	r3, r2, r3
08033c3c  b9eb831f  cmp.w	r9, r3, lsl #6
08033c40  c3db      blt	#-122 ; -> 0x08033bca ; branch_target=0x08033bca
08033c42  3201      lsls	r2, r6, #4
08033c44  019b      ldr	r3, [sp, #4]
08033c46  d210      asrs	r2, r2, #3
08033c48  6949      ldr	r1, [pc, #420] ; [0x08033df0] = 0x73693136
08033c4a  3846      mov	r0, r7
08033c4c  0093      str	r3, [sp]
08033c4e  fff7bdfd  bl	#-1158 ; -> 0x080337cc ; branch_target=0x080337cc
08033c52  3846      mov	r0, r7
08033c54  1035      adds	r5, #16
08033c56  f7f7f3fd  bl	#-33818 ; -> 0x0802b840 ; branch_target=0x0802b840
08033c5a  009b      ldr	r3, [sp]
08033c5c  102b      cmp	r3, #16
08033c5e  7ff461af  bne.w	#-318 ; -> 0x08033b24 ; branch_target=0x08033b24
08033c62  f9f70dfb  bl	#-27110 ; -> 0x0802d280 ; branch_target=0x0802d280
08033c66  09b0      add	sp, #36
08033c68  bdec048b  vpop	{d8, d9}
08033c6c  bde8f08f  pop.w	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
08033c70  0128      cmp	r0, #1
08033c72  f6d1      bne	#-20 ; -> 0x08033c62 ; branch_target=0x08033c62
08033c74  4ff0000a  mov.w	r10, #0
08033c78  624d      ldr	r5, [pc, #392] ; [0x08033e04] = 0x20000920
08033c7a  dff86881  ldr.w	r8, [pc, #360] ; [0x08033de4] = 0x20002f8c
08033c7e  d346      mov	r11, r10
08033c80  9fed559b  vldr	d9, [pc, #340] ; [0x08033dd8] = 0x00000000 / f64_bits_interpretation=32767
08033c84  04e0      b	#8 ; -> 0x08033c90 ; branch_target=0x08033c90
08033c86  bbf1100f  cmp.w	r11, #16
08033c8a  05f11005  add.w	r5, r5, #16
08033c8e  e8d0      beq	#-48 ; -> 0x08033c62 ; branch_target=0x08033c62
08033c90  ee68      ldr	r6, [r5, #12]
08033c92  5a46      mov	r2, r11
08033c94  0bf1010b  add.w	r11, r11, #1
08033c98  002e      cmp	r6, #0
08033c9a  f4d1      bne	#-24 ; -> 0x08033c86 ; branch_target=0x08033c86
08033c9c  524b      ldr	r3, [pc, #328] ; [0x08033de8] = 0x63657073
08033c9e  c5f80cb0  str.w	r11, [r5, #12]
08033ca2  0493      str	r3, [sp, #16]
08033ca4  03f19753  add.w	r3, r3, #316669952
08033ca8  8df81c60  strb.w	r6, [sp, #28]
08033cac  03f5e013  add.w	r3, r3, #1835008
08033cb0  03f2bb63  addw	r3, r3, #1723
08033cb4  0693      str	r3, [sp, #24]
08033cb6  4d4b      ldr	r3, [pc, #308] ; [0x08033dec] = 0x66666667
08033cb8  83fb0210  smull	r1, r0, r3, r2
08033cbc  d117      asrs	r1, r2, #31
08033cbe  c1eba001  rsb	r1, r1, r0, asr #2
08033cc2  43f26200  movw	r0, #12386
08033cc6  adf81400  strh.w	r0, [sp, #20]
08033cca  01eb8100  add.w	r0, r1, r1, lsl #2
08033cce  3031      adds	r1, #48
08033cd0  a2eb4002  sub.w	r2, r2, r0, lsl #1
08033cd4  4348      ldr	r0, [pc, #268] ; [0x08033de4] = 0x20002f8c
08033cd6  8df81610  strb.w	r1, [sp, #22]
08033cda  04a9      add	r1, sp, #16
08033cdc  3032      adds	r2, #48
08033cde  8df81720  strb.w	r2, [sp, #23]
08033ce2  0b22      movs	r2, #11
08033ce4  f7f746f9  bl	#-36212 ; -> 0x0802af74 ; branch_target=0x0802af74
08033ce8  4149      ldr	r1, [pc, #260] ; [0x08033df0] = 0x73693136
08033cea  3e48      ldr	r0, [pc, #248] ; [0x08033de4] = 0x20002f8c
08033cec  fff700fd  bl	#-1536 ; -> 0x080336f0 ; branch_target=0x080336f0
08033cf0  454a      ldr	r2, [pc, #276] ; [0x08033e08] = 0x200011e0
08033cf2  d8f80ca0  ldr.w	r10, [r8, #12]
08033cf6  d2ed007a  vldr	s15, [r2]
08033cfa  f5ee407a  vcmp.f32	s15, #0
08033cfe  f1ee10fa  vmrs	APSR_nzcv, fpscr
08033d02  06dd      ble	#12 ; -> 0x08033d12 ; branch_target=0x08033d12
08033d04  b7ee007a  vmov.f32	s14, #1.000000e+00
08033d08  f4ee477a  vcmp.f32	s15, s14
08033d0c  f1ee10fa  vmrs	APSR_nzcv, fpscr
08033d10  59d4      bmi	#178 ; -> 0x08033dc6 ; branch_target=0x08033dc6
08033d12  b7ee008a  vmov.f32	s16, #1.000000e+00
08033d16  d5e90121  ldrd	r2, r1, [r5, #4]
08033d1a  01fb02f2  mul	r2, r1, r2
08033d1e  002a      cmp	r2, #0
08033d20  56dd      ble	#172 ; -> 0x08033dd0 ; branch_target=0x08033dd0
08033d22  03ab      add	r3, sp, #12
08033d24  0027      movs	r7, #0
08033d26  344c      ldr	r4, [pc, #208] ; [0x08033df8] = 0x200135c8
08033d28  dff8e890  ldr.w	r9, [pc, #232] ; [0x08033e14] = 0x60001000
08033d2c  0093      str	r3, [sp]
08033d2e  cdf804b0  str.w	r11, [sp, #4]
08033d32  2b68      ldr	r3, [r5]
08033d34  dff8d8b0  ldr.w	r11, [pc, #216] ; [0x08033e10] = 0x200134c8
08033d38  3b44      add	r3, r7
08033d3a  5946      mov	r1, r11
08033d3c  09eb8303  add.w	r3, r9, r3, lsl #2
08033d40  d3ed007a  vldr	s15, [r3]
08033d44  67ee887a  vmul.f32	s15, s15, s16
08033d48  e1ec017a  vstmia	r1!, {s15}
08033d4c  8c42      cmp	r4, r1
08033d4e  e3ec017a  vstmia	r3!, {s15}
08033d52  f5d1      bne	#-22 ; -> 0x08033d40 ; branch_target=0x08033d40
08033d54  3101      lsls	r1, r6, #4
08033d56  4046      mov	r0, r8
08033d58  0aebe101  add.w	r1, r10, r1, asr #3
08033d5c  21f00301  bic	r1, r1, #3
08033d60  f7f7f0fd  bl	#-33824 ; -> 0x0802b944 ; branch_target=0x0802b944
08033d64  254b      ldr	r3, [pc, #148] ; [0x08033dfc] = 0x20003466
08033d66  bbec017a  vldmia	r11!, {s14}
08033d6a  b7eec77a  vcvt.f64.f32	d7, s14
08033d6e  5c45      cmp	r4, r11
08033d70  27ee097b  vmul.f64	d7, d7, d9
08033d74  bdeec77b  vcvt.s32.f64	s14, d7
08033d78  17ee102a  vmov	r2, s14
08033d7c  23f8022f  strh	r2, [r3, #2]!
08033d80  f1d1      bne	#-30 ; -> 0x08033d66 ; branch_target=0x08033d66
08033d82  009b      ldr	r3, [sp]
08033d84  8022      movs	r2, #128
08033d86  1e49      ldr	r1, [pc, #120] ; [0x08033e00] = 0x20003468
08033d88  4046      mov	r0, r8
08033d8a  f7f79ffb  bl	#-35010 ; -> 0x0802b4cc ; branch_target=0x0802b4cc
08033d8e  b0fa80f3  clz	r3, r0
08033d92  4037      adds	r7, #64
08033d94  5b09      lsrs	r3, r3, #5
08033d96  06eb8316  add.w	r6, r6, r3, lsl #6
08033d9a  d5e90132  ldrd	r3, r2, [r5, #4]
08033d9e  02fb03f3  mul	r3, r2, r3
08033da2  b7eb831f  cmp.w	r7, r3, lsl #6
08033da6  c4db      blt	#-120 ; -> 0x08033d32 ; branch_target=0x08033d32
08033da8  3201      lsls	r2, r6, #4
08033daa  ddf804b0  ldr.w	r11, [sp, #4]
08033dae  d210      asrs	r2, r2, #3
08033db0  0f49      ldr	r1, [pc, #60] ; [0x08033df0] = 0x73693136
08033db2  4046      mov	r0, r8
08033db4  fff70afd  bl	#-1516 ; -> 0x080337cc ; branch_target=0x080337cc
08033db8  4046      mov	r0, r8
08033dba  f7f741fd  bl	#-34174 ; -> 0x0802b840 ; branch_target=0x0802b840
08033dbe  62e7      b	#-316 ; -> 0x08033c86 ; branch_target=0x08033c86
08033dc0  87ee278a  vdiv.f32	s16, s14, s15
08033dc4  f2e6      b	#-540 ; -> 0x08033bac ; branch_target=0x08033bac
08033dc6  87ee278a  vdiv.f32	s16, s14, s15
08033dca  a4e7      b	#-184 ; -> 0x08033d16 ; branch_target=0x08033d16
08033dcc  0022      movs	r2, #0
08033dce  3be7      b	#-394 ; -> 0x08033c48 ; branch_target=0x08033c48
08033dd0  0022      movs	r2, #0
08033dd2  ede7      b	#-38 ; -> 0x08033db0 ; branch_target=0x08033db0
