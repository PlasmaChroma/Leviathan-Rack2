; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
080251f0  474a      ldr	r2, [pc, #284] ; [0x08025310] = 0x58024400
080251f2  70b4      push	{r4, r5, r6}
080251f4  916a      ldr	r1, [r2, #40]
080251f6  956a      ldr	r5, [r2, #40]
080251f8  d66a      ldr	r6, [r2, #44]
080251fa  15f47c7f  tst.w	r5, #1008
080251fe  c5f30513  ubfx	r3, r5, #4, #6
08025202  546b      ldr	r4, [r2, #52]
08025204  5bd0      beq	#182 ; -> 0x080252be ; branch_target=0x080252be
08025206  c4f3cc04  ubfx	r4, r4, #3, #13
0802520a  06f00106  and	r6, r6, #1
0802520e  01f00301  and	r1, r1, #3
08025212  07ee903a  vmov	s15, r3
08025216  06fb04f4  mul	r4, r6, r4
0802521a  0129      cmp	r1, #1
0802521c  b8eee77a  vcvt.f32.s32	s14, s15
08025220  06ee904a  vmov	s13, r4
08025224  faeee96a  vcvt.f32.s32	s13, s13, #13
08025228  6fd0      beq	#222 ; -> 0x0802530a ; branch_target=0x0802530a
0802522a  0229      cmp	r1, #2
0802522c  6ad0      beq	#212 ; -> 0x08025304 ; branch_target=0x08025304
0802522e  0029      cmp	r1, #0
08025230  4ad0      beq	#148 ; -> 0x080252c8 ; branch_target=0x080252c8
08025232  dfed387a  vldr	s15, [pc, #224] ; [0x08025314] = 0x4c742400 / f32_bits_interpretation=64000000
08025236  87ee876a  vdiv.f32	s12, s15, s14
0802523a  136b      ldr	r3, [r2, #48]
0802523c  c3f30803  ubfx	r3, r3, #0, #9
08025240  07ee903a  vmov	s15, r3
08025244  f7ee005a  vmov.f32	s11, #1.000000e+00
08025248  f8eee77a  vcvt.f32.s32	s15, s15
0802524c  77eea57a  vadd.f32	s15, s15, s11
08025250  77eea67a  vadd.f32	s15, s15, s13
08025254  67ee867a  vmul.f32	s15, s15, s12
08025258  2d4a      ldr	r2, [pc, #180] ; [0x08025310] = 0x58024400
0802525a  b7ee006a  vmov.f32	s12, #1.000000e+00
0802525e  136b      ldr	r3, [r2, #48]
08025260  c3f34623  ubfx	r3, r3, #9, #7
08025264  07ee103a  vmov	s14, r3
08025268  b8eec77a  vcvt.f32.s32	s14, s14
0802526c  70bc      pop	{r4, r5, r6}
0802526e  37ee067a  vadd.f32	s14, s14, s12
08025272  c7ee876a  vdiv.f32	s13, s15, s14
08025276  fceee66a  vcvt.u32.f32	s13, s13
0802527a  c0ed006a  vstr	s13, [r0]
0802527e  136b      ldr	r3, [r2, #48]
08025280  c3f30643  ubfx	r3, r3, #16, #7
08025284  07ee103a  vmov	s14, r3
08025288  b8eec77a  vcvt.f32.s32	s14, s14
0802528c  37ee067a  vadd.f32	s14, s14, s12
08025290  c7ee876a  vdiv.f32	s13, s15, s14
08025294  fceee66a  vcvt.u32.f32	s13, s13
08025298  c0ed016a  vstr	s13, [r0, #4]
0802529c  136b      ldr	r3, [r2, #48]
0802529e  c3f30663  ubfx	r3, r3, #24, #7
080252a2  06ee903a  vmov	s13, r3
080252a6  f8eee66a  vcvt.f32.s32	s13, s13
080252aa  76ee866a  vadd.f32	s13, s13, s12
080252ae  87eea67a  vdiv.f32	s14, s15, s13
080252b2  fceec77a  vcvt.u32.f32	s15, s14
080252b6  17ee903a  vmov	r3, s15
080252ba  8360      str	r3, [r0, #8]
080252bc  7047      bx	lr
080252be  70bc      pop	{r4, r5, r6}
080252c0  c0e90033  strd	r3, r3, [r0]
080252c4  8360      str	r3, [r0, #8]
080252c6  7047      bx	lr
080252c8  1368      ldr	r3, [r2]
080252ca  9b06      lsls	r3, r3, #26
080252cc  b1d5      bpl	#-158 ; -> 0x08025232 ; branch_target=0x08025232
080252ce  1468      ldr	r4, [r2]
080252d0  f7ee007a  vmov.f32	s15, #1.000000e+00
080252d4  136b      ldr	r3, [r2, #48]
080252d6  1049      ldr	r1, [pc, #64] ; [0x08025318] = 0x03d09000
080252d8  c4f3c102  ubfx	r2, r4, #3, #2
080252dc  c3f30803  ubfx	r3, r3, #0, #9
080252e0  d140      lsrs	r1, r2
080252e2  06ee103a  vmov	s12, r3
080252e6  05ee901a  vmov	s11, r1
080252ea  b8eec66a  vcvt.f32.s32	s12, s12
080252ee  f8eee55a  vcvt.f32.s32	s11, s11
080252f2  36ee276a  vadd.f32	s12, s12, s15
080252f6  c5ee877a  vdiv.f32	s15, s11, s14
080252fa  36ee267a  vadd.f32	s14, s12, s13
080252fe  67ee877a  vmul.f32	s15, s15, s14
08025302  a9e7      b	#-174 ; -> 0x08025258 ; branch_target=0x08025258
08025304  dfed057a  vldr	s15, [pc, #20] ; [0x0802531c] = 0x4bbebc20 / f32_bits_interpretation=25000000
08025308  95e7      b	#-214 ; -> 0x08025236 ; branch_target=0x08025236
0802530a  dfed057a  vldr	s15, [pc, #20] ; [0x08025320] = 0x4a742400 / f32_bits_interpretation=4000000
0802530e  92e7      b	#-220 ; -> 0x08025236 ; branch_target=0x08025236
