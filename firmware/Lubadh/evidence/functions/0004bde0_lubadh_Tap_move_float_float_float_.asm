; lubadh::Tap::move(float, float, float)
; VA 0x4bde0 size 316

   4bde0: e5d0c014     	ldrb	r12, [r0, #0x14]
   4bde4: e280200c     	add	r2, r0, #12
   4bde8: e1a03000     	mov	r3, r0
   4bdec: eeb77a00     	vmov.f32	s14, #1.000000e+00
   4bdf0: e1c000d4     	ldrd	r0, r1, [r0, #4]
   4bdf4: e35c0000     	cmp	r12, #0
   4bdf8: e8820003     	stm	r2, {r0, r1}
   4bdfc: 1d930a06     	vldrne	s0, [r3, #24]
   4be00: edd37a02     	vldr	s15, [r3, #8]
   4be04: ee600a80     	vmul.f32	s1, s1, s0
   4be08: ee201a81     	vmul.f32	s2, s1, s2
   4be0c: ee717a27     	vadd.f32	s15, s2, s15
   4be10: eef47ac7     	vcmpe.f32	s15, s14
   4be14: edc37a02     	vstr	s15, [r3, #8]
   4be18: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4be1c: a5932004     	ldrge	r2, [r3, #0x4]
   4be20: a2822001     	addge	r2, r2, #1
   4be24: ba000007     	blt	0x4be48
   4be28: ee777ac7     	vsub.f32	s15, s15, s14
   4be2c: e1a01002     	mov	r1, r2
   4be30: e2822001     	add	r2, r2, #1
   4be34: eef47ac7     	vcmpe.f32	s15, s14
   4be38: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4be3c: aafffff9     	bge	0x4be28
   4be40: e5831004     	str	r1, [r3, #0x4]
   4be44: edc37a02     	vstr	s15, [r3, #8]
   4be48: eef57ac0     	vcmpe.f32	s15, #0
   4be4c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4be50: 45932004     	ldrmi	r2, [r3, #0x4]
   4be54: 4eb77a00     	vmovmi.f32	s14, #1.000000e+00
   4be58: 5a000006     	bpl	0x4be78
   4be5c: ee777a87     	vadd.f32	s15, s15, s14
   4be60: e2422001     	sub	r2, r2, #1
   4be64: eef57ac0     	vcmpe.f32	s15, #0
   4be68: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4be6c: 4afffffa     	bmi	0x4be5c
   4be70: e5832004     	str	r2, [r3, #0x4]
   4be74: edc37a02     	vstr	s15, [r3, #8]
   4be78: e283c060     	add	r12, r3, #96
   4be7c: eeb77a00     	vmov.f32	s14, #1.000000e+00
   4be80: ea000002     	b	0x4be90
   4be84: e2833018     	add	r3, r3, #24
   4be88: e15c0003     	cmp	r12, r3
   4be8c: 012fff1e     	bxeq	lr
   4be90: e5d3201c     	ldrb	r2, [r3, #0x1c]
   4be94: e3520000     	cmp	r2, #0
   4be98: 0afffff9     	beq	0x4be84
   4be9c: e2832028     	add	r2, r3, #40
   4bea0: e1c302d0     	ldrd	r0, r1, [r3, #32]
   4bea4: e8820003     	stm	r2, {r0, r1}
   4bea8: edd36a0c     	vldr	s13, [r3, #48]
   4beac: edd37a09     	vldr	s15, [r3, #36]
   4beb0: eee17a26     	vfma.f32	s15, s2, s13
   4beb4: eef47ac7     	vcmpe.f32	s15, s14
   4beb8: edc37a09     	vstr	s15, [r3, #36]
   4bebc: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4bec0: a5932020     	ldrge	r2, [r3, #0x20]
   4bec4: ba000006     	blt	0x4bee4
   4bec8: ee777ac7     	vsub.f32	s15, s15, s14
   4becc: e2822001     	add	r2, r2, #1
   4bed0: eef47ac7     	vcmpe.f32	s15, s14
   4bed4: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4bed8: aafffffa     	bge	0x4bec8
   4bedc: e5832020     	str	r2, [r3, #0x20]
   4bee0: edc37a09     	vstr	s15, [r3, #36]
   4bee4: eef57ac0     	vcmpe.f32	s15, #0
   4bee8: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4beec: 45932020     	ldrmi	r2, [r3, #0x20]
   4bef0: 42422001     	submi	r2, r2, #1
   4bef4: 5affffe2     	bpl	0x4be84
   4bef8: ee777a87     	vadd.f32	s15, s15, s14
   4befc: e1a01002     	mov	r1, r2
   4bf00: e2422001     	sub	r2, r2, #1
   4bf04: eef57ac0     	vcmpe.f32	s15, #0
   4bf08: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4bf0c: 4afffff9     	bmi	0x4bef8
   4bf10: e5831020     	str	r1, [r3, #0x20]
   4bf14: edc37a09     	vstr	s15, [r3, #36]
   4bf18: eaffffd9     	b	0x4be84
