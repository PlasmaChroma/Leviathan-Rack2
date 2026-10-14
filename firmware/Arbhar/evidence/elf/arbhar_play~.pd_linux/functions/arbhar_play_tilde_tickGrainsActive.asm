00003768 <arbhar_play_tilde_tickGrainsActive>:
    3768: e2803a02     	add	r3, r0, #8192
    376c: e59000f0     	ldr	r0, [r0, #0xf0]
    3770: e593162c     	ldr	r1, [r3, #0x62c]
    3774: ee001a10     	vmov	s0, r1
    3778: eeb80ac0     	vcvt.f32.s32	s0, s0
    377c: eafffbca     	b	0x26ac <.plt+0x2cc>     @ imm = #-0x10d8

