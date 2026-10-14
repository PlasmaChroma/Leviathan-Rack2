; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
080235f8  334b      ldr	r3, [pc, #204] ; [0x080236c8] = 0x58024400
080235fa  30b4      push	{r4, r5}
080235fc  9a6a      ldr	r2, [r3, #40]
080235fe  9c6a      ldr	r4, [r3, #40]
08023600  dd6a      ldr	r5, [r3, #44]
08023602  14f47c7f  tst.w	r4, #1008
08023606  596b      ldr	r1, [r3, #52]
08023608  c4f30510  ubfx	r0, r4, #4, #6
0802360c  36d0      beq	#108 ; -> 0x0802367c ; branch_target=0x0802367c
0802360e  c1f3cc01  ubfx	r1, r1, #3, #13
08023612  05f00105  and	r5, r5, #1
08023616  02f00302  and	r2, r2, #3
0802361a  07ee900a  vmov	s15, r0
0802361e  05fb01f1  mul	r1, r5, r1
08023622  012a      cmp	r2, #1
08023624  b8eee77a  vcvt.f32.s32	s14, s15
08023628  06ee901a  vmov	s13, r1
0802362c  faeee96a  vcvt.f32.s32	s13, s13, #13
08023630  02d0      beq	#4 ; -> 0x08023638 ; branch_target=0x08023638
08023632  022a      cmp	r2, #2
08023634  42d0      beq	#132 ; -> 0x080236bc ; branch_target=0x080236bc
08023636  1ab3      cbz	r2, #70 ; -> 0x08023680 ; branch_target=0x08023680
08023638  dfed247a  vldr	s15, [pc, #144] ; [0x080236cc] = 0x4a742400 / f32_bits_interpretation=4000000
0802363c  87ee876a  vdiv.f32	s12, s15, s14
08023640  1b6b      ldr	r3, [r3, #48]
08023642  c3f30803  ubfx	r3, r3, #0, #9
08023646  07ee903a  vmov	s15, r3
0802364a  f7ee005a  vmov.f32	s11, #1.000000e+00
0802364e  f8eee77a  vcvt.f32.s32	s15, s15
08023652  77eea57a  vadd.f32	s15, s15, s11
08023656  77eea67a  vadd.f32	s15, s15, s13
0802365a  67ee867a  vmul.f32	s15, s15, s12
0802365e  1a4b      ldr	r3, [pc, #104] ; [0x080236c8] = 0x58024400
08023660  1b6b      ldr	r3, [r3, #48]
08023662  c3f34623  ubfx	r3, r3, #9, #7
08023666  0133      adds	r3, #1
08023668  07ee103a  vmov	s14, r3
0802366c  f8eec76a  vcvt.f32.s32	s13, s14
08023670  87eea67a  vdiv.f32	s14, s15, s13
08023674  fceec77a  vcvt.u32.f32	s15, s14
08023678  17ee900a  vmov	r0, s15
0802367c  30bc      pop	{r4, r5}
0802367e  7047      bx	lr
08023680  1a68      ldr	r2, [r3]
08023682  9206      lsls	r2, r2, #26
08023684  1dd5      bpl	#58 ; -> 0x080236c2 ; branch_target=0x080236c2
08023686  1968      ldr	r1, [r3]
08023688  f7ee007a  vmov.f32	s15, #1.000000e+00
0802368c  104a      ldr	r2, [pc, #64] ; [0x080236d0] = 0x03d09000
0802368e  1b6b      ldr	r3, [r3, #48]
08023690  c1f3c101  ubfx	r1, r1, #3, #2
08023694  c3f30803  ubfx	r3, r3, #0, #9
08023698  ca40      lsrs	r2, r1
0802369a  06ee103a  vmov	s12, r3
0802369e  05ee902a  vmov	s11, r2
080236a2  b8eec66a  vcvt.f32.s32	s12, s12
080236a6  f8eee55a  vcvt.f32.s32	s11, s11
080236aa  36ee276a  vadd.f32	s12, s12, s15
080236ae  c5ee877a  vdiv.f32	s15, s11, s14
080236b2  36ee267a  vadd.f32	s14, s12, s13
080236b6  67ee877a  vmul.f32	s15, s15, s14
080236ba  d0e7      b	#-96 ; -> 0x0802365e ; branch_target=0x0802365e
080236bc  dfed057a  vldr	s15, [pc, #20] ; [0x080236d4] = 0x4bbebc20 / f32_bits_interpretation=25000000
080236c0  bce7      b	#-136 ; -> 0x0802363c ; branch_target=0x0802363c
080236c2  dfed057a  vldr	s15, [pc, #20] ; [0x080236d8] = 0x4c742400 / f32_bits_interpretation=64000000
080236c6  b9e7      b	#-142 ; -> 0x0802363c ; branch_target=0x0802363c
