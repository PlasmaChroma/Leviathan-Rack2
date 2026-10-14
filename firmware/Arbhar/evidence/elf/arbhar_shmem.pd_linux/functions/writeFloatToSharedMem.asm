00000ce4 <writeFloatToSharedMem>:
     ce4: eebd0ac0     	vcvt.s32.f32	s0, s0
     ce8: ee103a10     	vmov	r3, s0
     cec: e2831016     	add	r1, r3, #22
     cf0: e7902101     	ldr	r2, [r0, r1, lsl #2]
     cf4: e3520000     	cmp	r2, #0
     cf8: 0a000005     	beq	0xd14 <writeFloatToSharedMem+0x30> @ imm = #0x14
     cfc: eefd0ae0     	vcvt.s32.f32	s1, s1
     d00: ee100a90     	vmov	r0, s1
     d04: e1c0cfc0     	bic	r12, r0, r0, asr #31
     d08: e082310c     	add	r3, r2, r12, lsl #2
     d0c: ed831a00     	vstr	s2, [r3]
     d10: e12fff1e     	bx	lr
     d14: e59f1004     	ldr	r1, [pc, #0x4]          @ 0xd20 <writeFloatToSharedMem+0x3c>
     d18: e08f0001     	add	r0, pc, r1
     d1c: eaffff5d     	b	0xa98 <.plt+0xa4>       @ imm = #-0x28c
     d20: 68 1d 00 00  	.word	0x00001d68

