00000dbc <comport_retries>:
     dbc: eeb50ac0     	vcmpe.f32	s0, #0
     dc0: e2800a01     	add	r0, r0, #4096
     dc4: eef1fa10     	vmrs	APSR_nzcv, fpscr
     dc8: 5efd7ac0     	vcvtpl.s32.f32	s15, s0
     dcc: 43a03000     	movmi	r3, #0
     dd0: 5e173a90     	vmovpl	r3, s15
     dd4: e58030cc     	str	r3, [r0, #0xcc]
     dd8: e12fff1e     	bx	lr

