00003b7c <_memReadLayer>:
    3b7c: e0801101     	add	r1, r0, r1, lsl #2
    3b80: e59120c8     	ldr	r2, [r1, #0xc8]
    3b84: e3520000     	cmp	r2, #0
    3b88: 0a000008     	beq	0x3bb0 <_memReadLayer+0x34> @ imm = #0x20
    3b8c: eebd0ac0     	vcvt.s32.f32	s0, s0
    3b90: e59100fc     	ldr	r0, [r1, #0xfc]
    3b94: ee103a10     	vmov	r3, s0
    3b98: e1c3cfc3     	bic	r12, r3, r3, asr #31
    3b9c: e150000c     	cmp	r0, r12
    3ba0: d240c001     	suble	r12, r0, #1
    3ba4: e082110c     	add	r1, r2, r12, lsl #2
    3ba8: ed910a00     	vldr	s0, [r1]
    3bac: e12fff1e     	bx	lr
    3bb0: ed9f0a00     	vldr	s0, [pc]                @ 0x3bb8 <_memReadLayer+0x3c>
    3bb4: e12fff1e     	bx	lr
    3bb8: 00 00 00 00  	.word	0x00000000

