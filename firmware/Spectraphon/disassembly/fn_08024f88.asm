; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08024f88  474a      ldr	r2, [pc, #284] ; [0x080250a8] = 0x58024400
08024f8a  70b4      push	{r4, r5, r6}
08024f8c  916a      ldr	r1, [r2, #40]
08024f8e  956a      ldr	r5, [r2, #40]
08024f90  d66a      ldr	r6, [r2, #44]
08024f92  15f47c3f  tst.w	r5, #258048
08024f96  c5f30533  ubfx	r3, r5, #12, #6
08024f9a  d46b      ldr	r4, [r2, #60]
08024f9c  5bd0      beq	#182 ; -> 0x08025056 ; branch_target=0x08025056
08024f9e  c4f3cc04  ubfx	r4, r4, #3, #13
08024fa2  c6f30016  ubfx	r6, r6, #4, #1
08024fa6  01f00301  and	r1, r1, #3
08024faa  07ee903a  vmov	s15, r3
08024fae  06fb04f4  mul	r4, r6, r4
08024fb2  0129      cmp	r1, #1
08024fb4  b8eee77a  vcvt.f32.s32	s14, s15
08024fb8  06ee904a  vmov	s13, r4
08024fbc  faeee96a  vcvt.f32.s32	s13, s13, #13
08024fc0  03d0      beq	#6 ; -> 0x08024fca ; branch_target=0x08024fca
08024fc2  0229      cmp	r1, #2
08024fc4  6ad0      beq	#212 ; -> 0x0802509c ; branch_target=0x0802509c
08024fc6  0029      cmp	r1, #0
08024fc8  4ad0      beq	#148 ; -> 0x08025060 ; branch_target=0x08025060
08024fca  dfed387a  vldr	s15, [pc, #224] ; [0x080250ac] = 0x4a742400 / f32_bits_interpretation=4000000
08024fce  87ee876a  vdiv.f32	s12, s15, s14
08024fd2  936b      ldr	r3, [r2, #56]
08024fd4  c3f30803  ubfx	r3, r3, #0, #9
08024fd8  07ee903a  vmov	s15, r3
08024fdc  f7ee005a  vmov.f32	s11, #1.000000e+00
08024fe0  f8eee77a  vcvt.f32.s32	s15, s15
08024fe4  77eea57a  vadd.f32	s15, s15, s11
08024fe8  77eea67a  vadd.f32	s15, s15, s13
08024fec  67ee867a  vmul.f32	s15, s15, s12
08024ff0  2d4a      ldr	r2, [pc, #180] ; [0x080250a8] = 0x58024400
08024ff2  b7ee006a  vmov.f32	s12, #1.000000e+00
08024ff6  936b      ldr	r3, [r2, #56]
08024ff8  c3f34623  ubfx	r3, r3, #9, #7
08024ffc  07ee103a  vmov	s14, r3
08025000  b8eec77a  vcvt.f32.s32	s14, s14
08025004  70bc      pop	{r4, r5, r6}
08025006  37ee067a  vadd.f32	s14, s14, s12
0802500a  c7ee876a  vdiv.f32	s13, s15, s14
0802500e  fceee66a  vcvt.u32.f32	s13, s13
08025012  c0ed006a  vstr	s13, [r0]
08025016  936b      ldr	r3, [r2, #56]
08025018  c3f30643  ubfx	r3, r3, #16, #7
0802501c  07ee103a  vmov	s14, r3
08025020  b8eec77a  vcvt.f32.s32	s14, s14
08025024  37ee067a  vadd.f32	s14, s14, s12
08025028  c7ee876a  vdiv.f32	s13, s15, s14
0802502c  fceee66a  vcvt.u32.f32	s13, s13
08025030  c0ed016a  vstr	s13, [r0, #4]
08025034  936b      ldr	r3, [r2, #56]
08025036  c3f30663  ubfx	r3, r3, #24, #7
0802503a  06ee903a  vmov	s13, r3
0802503e  f8eee66a  vcvt.f32.s32	s13, s13
08025042  76ee866a  vadd.f32	s13, s13, s12
08025046  87eea67a  vdiv.f32	s14, s15, s13
0802504a  fceec77a  vcvt.u32.f32	s15, s14
0802504e  17ee903a  vmov	r3, s15
08025052  8360      str	r3, [r0, #8]
08025054  7047      bx	lr
08025056  70bc      pop	{r4, r5, r6}
08025058  c0e90033  strd	r3, r3, [r0]
0802505c  8360      str	r3, [r0, #8]
0802505e  7047      bx	lr
08025060  1368      ldr	r3, [r2]
08025062  9b06      lsls	r3, r3, #26
08025064  1dd5      bpl	#58 ; -> 0x080250a2 ; branch_target=0x080250a2
08025066  1468      ldr	r4, [r2]
08025068  f7ee007a  vmov.f32	s15, #1.000000e+00
0802506c  936b      ldr	r3, [r2, #56]
0802506e  1049      ldr	r1, [pc, #64] ; [0x080250b0] = 0x03d09000
08025070  c4f3c102  ubfx	r2, r4, #3, #2
08025074  c3f30803  ubfx	r3, r3, #0, #9
08025078  d140      lsrs	r1, r2
0802507a  06ee103a  vmov	s12, r3
0802507e  05ee901a  vmov	s11, r1
08025082  b8eec66a  vcvt.f32.s32	s12, s12
08025086  f8eee55a  vcvt.f32.s32	s11, s11
0802508a  36ee276a  vadd.f32	s12, s12, s15
0802508e  c5ee877a  vdiv.f32	s15, s11, s14
08025092  36ee267a  vadd.f32	s14, s12, s13
08025096  67ee877a  vmul.f32	s15, s15, s14
0802509a  a9e7      b	#-174 ; -> 0x08024ff0 ; branch_target=0x08024ff0
0802509c  dfed057a  vldr	s15, [pc, #20] ; [0x080250b4] = 0x4bbebc20 / f32_bits_interpretation=25000000
080250a0  95e7      b	#-214 ; -> 0x08024fce ; branch_target=0x08024fce
080250a2  dfed057a  vldr	s15, [pc, #20] ; [0x080250b8] = 0x4c742400 / f32_bits_interpretation=64000000
080250a6  92e7      b	#-220 ; -> 0x08024fce ; branch_target=0x08024fce
