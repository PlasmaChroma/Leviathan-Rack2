0000089c <fftwObject_tilde_play>:
     89c: e2800866     	add	r0, r0, #6684672
     8a0: eeb50a40     	vcmp.f32	s0, #0
     8a4: e5902d00     	ldr	r2, [r0, #0xd00]
     8a8: eef1fa10     	vmrs	APSR_nzcv, fpscr
     8ac: 13a03001     	movne	r3, #1
     8b0: 03a03000     	moveq	r3, #0
     8b4: e5802cf8     	str	r2, [r0, #0xcf8]
     8b8: e5c03d0e     	strb	r3, [r0, #0xd0e]
     8bc: e12fff1e     	bx	lr

