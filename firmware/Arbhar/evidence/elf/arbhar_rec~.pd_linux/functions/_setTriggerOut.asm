000032d0 <_setTriggerOut>:
    32d0: eefc7ac0     	vcvt.u32.f32	s15, s0
    32d4: e3a00022     	mov	r0, #34
    32d8: ee171a90     	vmov	r1, s15
    32dc: e2011001     	and	r1, r1, #1
    32e0: eafffd1e     	b	0x2760 <.plt+0x260>     @ imm = #-0xb88

