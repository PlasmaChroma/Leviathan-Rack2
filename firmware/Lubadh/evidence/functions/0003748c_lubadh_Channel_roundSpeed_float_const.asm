; lubadh::Channel::roundSpeed(float) const
; VA 0x3748c size 132

   3748c: e59020e8     	ldr	r2, [r0, #0xe8]
   37490: eef07a40     	vmov.f32	s15, s0
   37494: e59230b4     	ldr	r3, [r2, #0xb4]
   37498: e59200b8     	ldr	r0, [r2, #0xb8]
   3749c: e0401003     	sub	r1, r0, r3
   374a0: edd36a00     	vldr	s13, [r3]
   374a4: e1a01141     	asr	r1, r1, #2
   374a8: e2511001     	subs	r1, r1, #1
   374ac: 0a000012     	beq	0x374fc
   374b0: e3a02000     	mov	r2, #0
   374b4: ecb30a01     	vldmia	r3!, {s0}
   374b8: e2822001     	add	r2, r2, #1
   374bc: eeb40ae7     	vcmpe.f32	s0, s15
   374c0: eef1fa10     	vmrs	APSR_nzcv, fpscr
   374c4: 8a00000a     	bhi	0x374f4
   374c8: ed937a00     	vldr	s14, [r3]
   374cc: eeb47ae7     	vcmpe.f32	s14, s15
   374d0: eef1fa10     	vmrs	APSR_nzcv, fpscr
   374d4: ba000006     	blt	0x374f4
   374d8: ee706a07     	vadd.f32	s13, s0, s14
   374dc: eeb66a00     	vmov.f32	s12, #5.000000e-01
   374e0: ee666a86     	vmul.f32	s13, s13, s12
   374e4: eef47ae6     	vcmpe.f32	s15, s13
   374e8: eef1fa10     	vmrs	APSR_nzcv, fpscr
   374ec: 5eb00a47     	vmovpl.f32	s0, s14
   374f0: e12fff1e     	bx	lr
   374f4: e1520001     	cmp	r2, r1
   374f8: 1affffed     	bne	0x374b4
   374fc: eef47ae6     	vcmpe.f32	s15, s13
   37500: eef1fa10     	vmrs	APSR_nzcv, fpscr
   37504: 5d100a01     	vldrpl	s0, [r0, #-4]
   37508: 4eb00a66     	vmovmi.f32	s0, s13
   3750c: e12fff1e     	bx	lr
