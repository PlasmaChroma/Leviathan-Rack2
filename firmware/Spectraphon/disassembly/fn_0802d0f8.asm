; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
0802d0f8  4a4b      ldr	r3, [pc, #296] ; [0x0802d224] = 0x20000b20
0802d0fa  f0b5      push	{r4, r5, r6, r7, lr}
0802d0fc  4a4d      ldr	r5, [pc, #296] ; [0x0802d228] = 0x20000920
0802d0fe  1c68      ldr	r4, [r3]
0802d100  05eb0413  add.w	r3, r5, r4, lsl #4
0802d104  2401      lsls	r4, r4, #4
0802d106  d3e90113  ldrd	r1, r3, [r3, #4]
0802d10a  03fb01f1  mul	r1, r3, r1
0802d10e  0029      cmp	r1, #0
0802d110  40f38280  ble.w	#260 ; -> 0x0802d218 ; branch_target=0x0802d218
0802d114  0023      movs	r3, #0
0802d116  9c46      mov	r12, r3
0802d118  0133      adds	r3, #1
0802d11a  03fb03f2  mul	r2, r3, r3
0802d11e  8a42      cmp	r2, r1
0802d120  f9db      blt	#-14 ; -> 0x0802d116 ; branch_target=0x0802d116
0802d122  07ee90ca  vmov	s15, r12
0802d126  1f46      mov	r7, r3
0802d128  9e01      lsls	r6, r3, #6
0802d12a  f8eee77a  vcvt.f32.s32	s15, s15
0802d12e  60eea70a  vmul.f32	s1, s1, s15
0802d132  20ee270a  vmul.f32	s0, s0, s15
0802d136  fdeee07a  vcvt.s32.f32	s15, s1
0802d13a  8801      lsls	r0, r1, #6
0802d13c  3b4a      ldr	r2, [pc, #236] ; [0x0802d22c] = 0x2000227c
0802d13e  f6eee04a  vrintz.f32	s9, s1
0802d142  2d59      ldr	r5, [r5, r4]
0802d144  70eee44a  vsub.f32	s9, s1, s9
0802d148  17ee901a  vmov	r1, s15
0802d14c  9763      str	r7, [r2, #56]
0802d14e  384c      ldr	r4, [pc, #224] ; [0x0802d230] = 0x20002f70
0802d150  9942      cmp	r1, r3
0802d152  384b      ldr	r3, [pc, #224] ; [0x0802d234] = 0x20002434
0802d154  01f1010e  add.w	lr, r1, #1
0802d158  2468      ldr	r4, [r4]
0802d15a  d3ed007a  vldr	s15, [r3]
0802d15e  acbf      ite	ge
0802d160  a1eb0c02  subge.w	r2, r1, r12
0802d164  0a46      movlt	r2, r1
0802d166  6145      cmp	r1, r12
0802d168  f8eee77a  vcvt.f32.s32	s15, s15
0802d16c  9fed324a  vldr	s8, [pc, #200] ; [0x0802d238] = 0x3c800000 / f32_bits_interpretation=0.015625
0802d170  a8bf      it	ge
0802d172  aeeb0c0e  subge.w	lr, lr, r12
0802d176  06fb02f2  mul	r2, r6, r2
0802d17a  77ee807a  vadd.f32	s15, s15, s0
0802d17e  06fb0efc  mul	r12, r6, lr
0802d182  b6eee75a  vrintz.f32	s10, s15
0802d186  37eec55a  vsub.f32	s10, s15, s10
0802d18a  fdeee77a  vcvt.s32.f32	s15, s15
0802d18e  17ee903a  vmov	r3, s15
0802d192  9b01      lsls	r3, r3, #6
0802d194  d118      adds	r1, r2, r3
0802d196  03f1400e  add.w	lr, r3, #64
0802d19a  6344      add	r3, r12
0802d19c  7244      add	r2, lr
0802d19e  8842      cmp	r0, r1
0802d1a0  f444      add	r12, lr
0802d1a2  d8bf      it	le
0802d1a4  091a      suble	r1, r1, r0
0802d1a6  9042      cmp	r0, r2
0802d1a8  d8bf      it	le
0802d1aa  121a      suble	r2, r2, r0
0802d1ac  9842      cmp	r0, r3
0802d1ae  d8bf      it	le
0802d1b0  1b1a      suble	r3, r3, r0
0802d1b2  6045      cmp	r0, r12
0802d1b4  d8bf      it	le
0802d1b6  aceb000c  suble.w	r12, r12, r0
0802d1ba  6818      adds	r0, r5, r1
0802d1bc  a918      adds	r1, r5, r2
0802d1be  ea18      adds	r2, r5, r3
0802d1c0  6544      add	r5, r12
0802d1c2  1e4b      ldr	r3, [pc, #120] ; [0x0802d23c] = 0x20002c40
0802d1c4  04eb8000  add.w	r0, r4, r0, lsl #2
0802d1c8  04eb8101  add.w	r1, r4, r1, lsl #2
0802d1cc  03f5807e  add.w	lr, r3, #256
0802d1d0  04eb8202  add.w	r2, r4, r2, lsl #2
0802d1d4  04eb850c  add.w	r12, r4, r5, lsl #2
0802d1d8  194c      ldr	r4, [pc, #100] ; [0x0802d240] = 0x20000f40
0802d1da  f0ec016a  vldmia	r0!, {s13}
0802d1de  b2ec017a  vldmia	r2!, {s14}
0802d1e2  f1ec015a  vldmia	r1!, {s11}
0802d1e6  bcec016a  vldmia	r12!, {s12}
0802d1ea  75eee65a  vsub.f32	s11, s11, s13
0802d1ee  f3ec017a  vldmia	r3!, {s15}
0802d1f2  36ee476a  vsub.f32	s12, s12, s14
0802d1f6  9e45      cmp	lr, r3
0802d1f8  e5ee856a  vfma.f32	s13, s11, s10
0802d1fc  a6ee057a  vfma.f32	s14, s12, s10
0802d200  76eee77a  vsub.f32	s15, s13, s15
0802d204  37ee667a  vsub.f32	s14, s14, s13
0802d208  e7ee247a  vfma.f32	s15, s14, s9
0802d20c  67ee847a  vmul.f32	s15, s15, s8
0802d210  e4ec017a  vstmia	r4!, {s15}
0802d214  e1d1      bne	#-62 ; -> 0x0802d1da ; branch_target=0x0802d1da
0802d216  f0bd      pop	{r4, r5, r6, r7, pc}
0802d218  0227      movs	r7, #2
0802d21a  8026      movs	r6, #128
0802d21c  4ff0010c  mov.w	r12, #1
0802d220  3b46      mov	r3, r7
0802d222  88e7      b	#-240 ; -> 0x0802d136 ; branch_target=0x0802d136
