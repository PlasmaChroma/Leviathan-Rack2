; lubadh::Fade::move(float)
; VA 0x4bafc size 140

   4bafc: e280300c     	add	r3, r0, #12
   4bb00: e1a02000     	mov	r2, r0
   4bb04: e1c000d4     	ldrd	r0, r1, [r0, #4]
   4bb08: eeb77a00     	vmov.f32	s14, #1.000000e+00
   4bb0c: e8830003     	stm	r3, {r0, r1}
   4bb10: edd26a05     	vldr	s13, [r2, #20]
   4bb14: edd27a02     	vldr	s15, [r2, #8]
   4bb18: eee67a80     	vfma.f32	s15, s13, s0
   4bb1c: eef47ac7     	vcmpe.f32	s15, s14
   4bb20: edc27a02     	vstr	s15, [r2, #8]
   4bb24: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4bb28: a5923004     	ldrge	r3, [r2, #0x4]
   4bb2c: ba000006     	blt	0x4bb4c
   4bb30: ee777ac7     	vsub.f32	s15, s15, s14
   4bb34: e2833001     	add	r3, r3, #1
   4bb38: eef47ac7     	vcmpe.f32	s15, s14
   4bb3c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4bb40: aafffffa     	bge	0x4bb30
   4bb44: e5823004     	str	r3, [r2, #0x4]
   4bb48: edc27a02     	vstr	s15, [r2, #8]
   4bb4c: eef57ac0     	vcmpe.f32	s15, #0
   4bb50: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4bb54: 512fff1e     	bxpl	lr
   4bb58: e5923004     	ldr	r3, [r2, #0x4]
   4bb5c: eeb77a00     	vmov.f32	s14, #1.000000e+00
   4bb60: e2433001     	sub	r3, r3, #1
   4bb64: ee777a87     	vadd.f32	s15, s15, s14
   4bb68: e1a01003     	mov	r1, r3
   4bb6c: e2433001     	sub	r3, r3, #1
   4bb70: eef57ac0     	vcmpe.f32	s15, #0
   4bb74: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4bb78: 4afffff9     	bmi	0x4bb64
   4bb7c: e5821004     	str	r1, [r2, #0x4]
   4bb80: edc27a02     	vstr	s15, [r2, #8]
   4bb84: e12fff1e     	bx	lr
