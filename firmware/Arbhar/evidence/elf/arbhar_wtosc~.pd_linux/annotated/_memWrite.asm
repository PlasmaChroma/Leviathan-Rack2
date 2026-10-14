00003b3c <_memWrite>:
    3b3c: eebd0ac0     	vcvt.s32.f32	s0, s0
    3b40: ee103a10     	vmov	r3, s0
    3b44: e2831032     	add	r1, r3, #50
    3b48: e7902101     	ldr	r2, [r0, r1, lsl #2]
    3b4c: e3520000     	cmp	r2, #0
    3b50: 0a000005     	beq	0x3b6c <_memWrite+0x30> @ imm = #0x14
    3b54: eefd0ae0     	vcvt.s32.f32	s1, s1
    3b58: ee100a90     	vmov	r0, s1
    3b5c: e1c0cfc0     	bic	r12, r0, r0, asr #31
    3b60: e082310c     	add	r3, r2, r12, lsl #2
    3b64: ed831a00     	vstr	s2, [r3]
    3b68: e12fff1e     	bx	lr
    3b6c: e59f1004     	ldr	r1, [pc, #0x4]          @ 0x3b78 <_memWrite+0x3c>  // u32=0x4abc; f32?=2.68096422e-41
    3b70: e08f0001     	add	r0, pc, r1
    3b74: eafffa30     	b	0x243c <.plt+0x158>     @ imm = #-0x1740  // CALL error
    3b78: bc 4a 00 00  	.word	0x00004abc

