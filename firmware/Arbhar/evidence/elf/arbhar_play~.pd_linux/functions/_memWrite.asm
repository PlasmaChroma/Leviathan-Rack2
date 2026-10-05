00002ef8 <_memWrite>:
    2ef8: eebd0ac0     	vcvt.s32.f32	s0, s0
    2efc: ee103a10     	vmov	r3, s0
    2f00: e2831ea2     	add	r1, r3, #2592
    2f04: e2812004     	add	r2, r1, #4
    2f08: e0800102     	add	r0, r0, r2, lsl #2
    2f0c: e590c004     	ldr	r12, [r0, #0x4]
    2f10: e35c0000     	cmp	r12, #0
    2f14: 0a000005     	beq	0x2f30 <_memWrite+0x38> @ imm = #0x14
    2f18: eefd0ae0     	vcvt.s32.f32	s1, s1
    2f1c: ee103a90     	vmov	r3, s1
    2f20: e1c31fc3     	bic	r1, r3, r3, asr #31
    2f24: e08c2101     	add	r2, r12, r1, lsl #2
    2f28: ed821a00     	vstr	s2, [r2]
    2f2c: e12fff1e     	bx	lr
    2f30: e59f0004     	ldr	r0, [pc, #0x4]          @ 0x2f3c <_memWrite+0x44>
    2f34: e08f0000     	add	r0, pc, r0
    2f38: eafffd81     	b	0x2544 <.plt+0x164>     @ imm = #-0x9fc
    2f3c: f0 9b 00 00  	.word	0x00009bf0

