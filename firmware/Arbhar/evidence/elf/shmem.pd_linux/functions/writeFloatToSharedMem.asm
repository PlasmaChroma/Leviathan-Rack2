000009d8 <writeFloatToSharedMem>:
     9d8: e5902024     	ldr	r2, [r0, #0x24]
     9dc: e3520000     	cmp	r2, #0
     9e0: 0a000005     	beq	0x9fc <writeFloatToSharedMem+0x24> @ imm = #0x14
     9e4: eebd0ac0     	vcvt.s32.f32	s0, s0
     9e8: ee103a10     	vmov	r3, s0
     9ec: e1c30fc3     	bic	r0, r3, r3, asr #31
     9f0: e0821100     	add	r1, r2, r0, lsl #2
     9f4: edc10a00     	vstr	s1, [r1]
     9f8: e12fff1e     	bx	lr
     9fc: e59fc004     	ldr	r12, [pc, #0x4]         @ 0xa08 <writeFloatToSharedMem+0x30>
     a00: e08f000c     	add	r0, pc, r12
     a04: eaffff73     	b	0x7d8 <.plt+0x68>       @ imm = #-0x234
     a08: ec 0d 00 00  	.word	0x00000dec

