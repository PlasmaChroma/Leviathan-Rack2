; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
0802cfac  4a4b      ldr	r3, [pc, #296] ; [0x0802d0d8] = 0x20002430
0802cfae  f0b5      push	{r4, r5, r6, r7, lr}
0802cfb0  4a4d      ldr	r5, [pc, #296] ; [0x0802d0dc] = 0x20000a20
0802cfb2  1c68      ldr	r4, [r3]
0802cfb4  05eb0413  add.w	r3, r5, r4, lsl #4
0802cfb8  2401      lsls	r4, r4, #4
0802cfba  d3e90113  ldrd	r1, r3, [r3, #4]
0802cfbe  03fb01f1  mul	r1, r3, r1
0802cfc2  0029      cmp	r1, #0
0802cfc4  40f38280  ble.w	#260 ; -> 0x0802d0cc ; branch_target=0x0802d0cc
0802cfc8  0023      movs	r3, #0
0802cfca  9c46      mov	r12, r3
0802cfcc  0133      adds	r3, #1
0802cfce  03fb03f2  mul	r2, r3, r3
0802cfd2  8a42      cmp	r2, r1
0802cfd4  f9db      blt	#-14 ; -> 0x0802cfca ; branch_target=0x0802cfca
0802cfd6  07ee90ca  vmov	s15, r12
0802cfda  1f46      mov	r7, r3
0802cfdc  9e01      lsls	r6, r3, #6
0802cfde  f8eee77a  vcvt.f32.s32	s15, s15
0802cfe2  60eea70a  vmul.f32	s1, s1, s15
0802cfe6  20ee270a  vmul.f32	s0, s0, s15
0802cfea  fdeee07a  vcvt.s32.f32	s15, s1
0802cfee  8801      lsls	r0, r1, #6
0802cff0  3b4a      ldr	r2, [pc, #236] ; [0x0802d0e0] = 0x2000227c
0802cff2  f6eee04a  vrintz.f32	s9, s1
0802cff6  2d59      ldr	r5, [r5, r4]
0802cff8  70eee44a  vsub.f32	s9, s1, s9
0802cffc  17ee901a  vmov	r1, s15
0802d000  5763      str	r7, [r2, #52]
0802d002  384c      ldr	r4, [pc, #224] ; [0x0802d0e4] = 0x20002f74
0802d004  9942      cmp	r1, r3
0802d006  384b      ldr	r3, [pc, #224] ; [0x0802d0e8] = 0x20002438
0802d008  01f1010e  add.w	lr, r1, #1
0802d00c  2468      ldr	r4, [r4]
0802d00e  d3ed007a  vldr	s15, [r3]
0802d012  acbf      ite	ge
0802d014  a1eb0c02  subge.w	r2, r1, r12
0802d018  0a46      movlt	r2, r1
0802d01a  6145      cmp	r1, r12
0802d01c  f8eee77a  vcvt.f32.s32	s15, s15
0802d020  9fed324a  vldr	s8, [pc, #200] ; [0x0802d0ec] = 0x3c800000 / f32_bits_interpretation=0.015625
0802d024  a8bf      it	ge
0802d026  aeeb0c0e  subge.w	lr, lr, r12
0802d02a  06fb02f2  mul	r2, r6, r2
0802d02e  77ee807a  vadd.f32	s15, s15, s0
0802d032  06fb0efc  mul	r12, r6, lr
0802d036  b6eee75a  vrintz.f32	s10, s15
0802d03a  37eec55a  vsub.f32	s10, s15, s10
0802d03e  fdeee77a  vcvt.s32.f32	s15, s15
0802d042  17ee903a  vmov	r3, s15
0802d046  9b01      lsls	r3, r3, #6
0802d048  d118      adds	r1, r2, r3
0802d04a  03f1400e  add.w	lr, r3, #64
0802d04e  6344      add	r3, r12
0802d050  7244      add	r2, lr
0802d052  8842      cmp	r0, r1
0802d054  f444      add	r12, lr
0802d056  d8bf      it	le
0802d058  091a      suble	r1, r1, r0
0802d05a  9042      cmp	r0, r2
0802d05c  d8bf      it	le
0802d05e  121a      suble	r2, r2, r0
0802d060  9842      cmp	r0, r3
0802d062  d8bf      it	le
0802d064  1b1a      suble	r3, r3, r0
0802d066  6045      cmp	r0, r12
0802d068  d8bf      it	le
0802d06a  aceb000c  suble.w	r12, r12, r0
0802d06e  6818      adds	r0, r5, r1
0802d070  a918      adds	r1, r5, r2
0802d072  ea18      adds	r2, r5, r3
0802d074  6544      add	r5, r12
0802d076  1e4b      ldr	r3, [pc, #120] ; [0x0802d0f0] = 0x20002a40
0802d078  04eb8000  add.w	r0, r4, r0, lsl #2
0802d07c  04eb8101  add.w	r1, r4, r1, lsl #2
0802d080  03f5807e  add.w	lr, r3, #256
0802d084  04eb8202  add.w	r2, r4, r2, lsl #2
0802d088  04eb850c  add.w	r12, r4, r5, lsl #2
0802d08c  194c      ldr	r4, [pc, #100] ; [0x0802d0f4] = 0x20000d40
0802d08e  f0ec016a  vldmia	r0!, {s13}
0802d092  b2ec017a  vldmia	r2!, {s14}
0802d096  f1ec015a  vldmia	r1!, {s11}
0802d09a  bcec016a  vldmia	r12!, {s12}
0802d09e  75eee65a  vsub.f32	s11, s11, s13
0802d0a2  f3ec017a  vldmia	r3!, {s15}
0802d0a6  36ee476a  vsub.f32	s12, s12, s14
0802d0aa  9e45      cmp	lr, r3
0802d0ac  e5ee856a  vfma.f32	s13, s11, s10
0802d0b0  a6ee057a  vfma.f32	s14, s12, s10
0802d0b4  76eee77a  vsub.f32	s15, s13, s15
0802d0b8  37ee667a  vsub.f32	s14, s14, s13
0802d0bc  e7ee247a  vfma.f32	s15, s14, s9
0802d0c0  67ee847a  vmul.f32	s15, s15, s8
0802d0c4  e4ec017a  vstmia	r4!, {s15}
0802d0c8  e1d1      bne	#-62 ; -> 0x0802d08e ; branch_target=0x0802d08e
0802d0ca  f0bd      pop	{r4, r5, r6, r7, pc}
0802d0cc  0227      movs	r7, #2
0802d0ce  8026      movs	r6, #128
0802d0d0  4ff0010c  mov.w	r12, #1
0802d0d4  3b46      mov	r3, r7
0802d0d6  88e7      b	#-240 ; -> 0x0802cfea ; branch_target=0x0802cfea
