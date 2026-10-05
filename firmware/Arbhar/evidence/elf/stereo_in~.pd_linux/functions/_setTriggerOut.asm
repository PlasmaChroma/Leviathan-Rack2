00002670 <_setTriggerOut>:
    2670: eefc7ac0     	vcvt.u32.f32	s15, s0
    2674: e3a00022     	mov	r0, #34
    2678: ee171a90     	vmov	r1, s15
    267c: e2011001     	and	r1, r1, #1
    2680: eafffed3     	b	0x21d4 <.plt+0x170>     @ imm = #-0x4b4

