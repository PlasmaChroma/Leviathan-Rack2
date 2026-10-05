00002d48 <_setTriggerOut>:
    2d48: eefc7ac0     	vcvt.u32.f32	s15, s0
    2d4c: e3a00022     	mov	r0, #34
    2d50: ee171a90     	vmov	r1, s15
    2d54: e2011001     	and	r1, r1, #1
    2d58: eafffded     	b	0x2514 <.plt+0x230>     @ imm = #-0x84c

