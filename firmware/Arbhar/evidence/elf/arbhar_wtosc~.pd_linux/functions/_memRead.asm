00003748 <_memRead>:
    3748: eebd0ac0     	vcvt.s32.f32	s0, s0
    374c: ee103a10     	vmov	r3, s0
    3750: e0800103     	add	r0, r0, r3, lsl #2
    3754: e59020c8     	ldr	r2, [r0, #0xc8]
    3758: e3520000     	cmp	r2, #0
    375c: 0a000008     	beq	0x3784 <_memRead+0x3c>  @ imm = #0x20
    3760: eefd0ae0     	vcvt.s32.f32	s1, s1
    3764: e59010fc     	ldr	r1, [r0, #0xfc]
    3768: ee10ca90     	vmov	r12, s1
    376c: e1cc3fcc     	bic	r3, r12, r12, asr #31
    3770: e1510003     	cmp	r1, r3
    3774: d2413001     	suble	r3, r1, #1
    3778: e0820103     	add	r0, r2, r3, lsl #2
    377c: ed900a00     	vldr	s0, [r0]
    3780: e12fff1e     	bx	lr
    3784: eebf0a00     	vmov.f32	s0, #-1.000000e+00
    3788: e12fff1e     	bx	lr

