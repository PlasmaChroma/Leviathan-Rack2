; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
080250bc  474a      ldr	r2, [pc, #284] ; [0x080251dc] = 0x58024400
080250be  70b4      push	{r4, r5, r6}
080250c0  916a      ldr	r1, [r2, #40]
080250c2  956a      ldr	r5, [r2, #40]
080250c4  d66a      ldr	r6, [r2, #44]
080250c6  15f07c7f  tst.w	r5, #66060288
080250ca  c5f30553  ubfx	r3, r5, #20, #6
080250ce  546c      ldr	r4, [r2, #68]
080250d0  5bd0      beq	#182 ; -> 0x0802518a ; branch_target=0x0802518a
080250d2  c4f3cc04  ubfx	r4, r4, #3, #13
080250d6  c6f30026  ubfx	r6, r6, #8, #1
080250da  01f00301  and	r1, r1, #3
080250de  07ee903a  vmov	s15, r3
080250e2  06fb04f4  mul	r4, r6, r4
080250e6  0129      cmp	r1, #1
080250e8  b8eee77a  vcvt.f32.s32	s14, s15
080250ec  06ee904a  vmov	s13, r4
080250f0  faeee96a  vcvt.f32.s32	s13, s13, #13
080250f4  03d0      beq	#6 ; -> 0x080250fe ; branch_target=0x080250fe
080250f6  0229      cmp	r1, #2
080250f8  6ad0      beq	#212 ; -> 0x080251d0 ; branch_target=0x080251d0
080250fa  0029      cmp	r1, #0
080250fc  4ad0      beq	#148 ; -> 0x08025194 ; branch_target=0x08025194
080250fe  dfed387a  vldr	s15, [pc, #224] ; [0x080251e0] = 0x4a742400 / f32_bits_interpretation=4000000
08025102  87ee876a  vdiv.f32	s12, s15, s14
08025106  136c      ldr	r3, [r2, #64]
08025108  c3f30803  ubfx	r3, r3, #0, #9
0802510c  07ee903a  vmov	s15, r3
08025110  f7ee005a  vmov.f32	s11, #1.000000e+00
08025114  f8eee77a  vcvt.f32.s32	s15, s15
08025118  77eea57a  vadd.f32	s15, s15, s11
0802511c  77eea67a  vadd.f32	s15, s15, s13
08025120  67ee867a  vmul.f32	s15, s15, s12
08025124  2d4a      ldr	r2, [pc, #180] ; [0x080251dc] = 0x58024400
08025126  b7ee006a  vmov.f32	s12, #1.000000e+00
0802512a  136c      ldr	r3, [r2, #64]
0802512c  c3f34623  ubfx	r3, r3, #9, #7
08025130  07ee103a  vmov	s14, r3
08025134  b8eec77a  vcvt.f32.s32	s14, s14
08025138  70bc      pop	{r4, r5, r6}
0802513a  37ee067a  vadd.f32	s14, s14, s12
0802513e  c7ee876a  vdiv.f32	s13, s15, s14
08025142  fceee66a  vcvt.u32.f32	s13, s13
08025146  c0ed006a  vstr	s13, [r0]
0802514a  136c      ldr	r3, [r2, #64]
0802514c  c3f30643  ubfx	r3, r3, #16, #7
08025150  07ee103a  vmov	s14, r3
08025154  b8eec77a  vcvt.f32.s32	s14, s14
08025158  37ee067a  vadd.f32	s14, s14, s12
0802515c  c7ee876a  vdiv.f32	s13, s15, s14
08025160  fceee66a  vcvt.u32.f32	s13, s13
08025164  c0ed016a  vstr	s13, [r0, #4]
08025168  136c      ldr	r3, [r2, #64]
0802516a  c3f30663  ubfx	r3, r3, #24, #7
0802516e  06ee903a  vmov	s13, r3
08025172  f8eee66a  vcvt.f32.s32	s13, s13
08025176  76ee866a  vadd.f32	s13, s13, s12
0802517a  87eea67a  vdiv.f32	s14, s15, s13
0802517e  fceec77a  vcvt.u32.f32	s15, s14
08025182  17ee903a  vmov	r3, s15
08025186  8360      str	r3, [r0, #8]
08025188  7047      bx	lr
0802518a  70bc      pop	{r4, r5, r6}
0802518c  c0e90033  strd	r3, r3, [r0]
08025190  8360      str	r3, [r0, #8]
08025192  7047      bx	lr
08025194  1368      ldr	r3, [r2]
08025196  9b06      lsls	r3, r3, #26
08025198  1dd5      bpl	#58 ; -> 0x080251d6 ; branch_target=0x080251d6
0802519a  1468      ldr	r4, [r2]
0802519c  f7ee007a  vmov.f32	s15, #1.000000e+00
080251a0  136c      ldr	r3, [r2, #64]
080251a2  1049      ldr	r1, [pc, #64] ; [0x080251e4] = 0x03d09000
080251a4  c4f3c102  ubfx	r2, r4, #3, #2
080251a8  c3f30803  ubfx	r3, r3, #0, #9
080251ac  d140      lsrs	r1, r2
080251ae  06ee103a  vmov	s12, r3
080251b2  05ee901a  vmov	s11, r1
080251b6  b8eec66a  vcvt.f32.s32	s12, s12
080251ba  f8eee55a  vcvt.f32.s32	s11, s11
080251be  36ee276a  vadd.f32	s12, s12, s15
080251c2  c5ee877a  vdiv.f32	s15, s11, s14
080251c6  36ee267a  vadd.f32	s14, s12, s13
080251ca  67ee877a  vmul.f32	s15, s15, s14
080251ce  a9e7      b	#-174 ; -> 0x08025124 ; branch_target=0x08025124
080251d0  dfed057a  vldr	s15, [pc, #20] ; [0x080251e8] = 0x4bbebc20 / f32_bits_interpretation=25000000
080251d4  95e7      b	#-214 ; -> 0x08025102 ; branch_target=0x08025102
080251d6  dfed057a  vldr	s15, [pc, #20] ; [0x080251ec] = 0x4c742400 / f32_bits_interpretation=64000000
080251da  92e7      b	#-220 ; -> 0x08025102 ; branch_target=0x08025102
