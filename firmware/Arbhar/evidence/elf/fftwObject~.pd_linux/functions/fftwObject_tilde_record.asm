00000860 <fftwObject_tilde_record>:
     860: eeb50a40     	vcmp.f32	s0, #0
     864: e2800866     	add	r0, r0, #6684672
     868: eef1fa10     	vmrs	APSR_nzcv, fpscr
     86c: 13a03001     	movne	r3, #1
     870: 03a03000     	moveq	r3, #0
     874: e5c03d0c     	strb	r3, [r0, #0xd0c]
     878: e12fff1e     	bx	lr

