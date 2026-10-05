0000411c <_setTriggerOut>:
    411c: eefc7ac0     	vcvt.u32.f32	s15, s0
    4120: e3a00022     	mov	r0, #34
    4124: ee171a90     	vmov	r1, s15
    4128: e2011001     	and	r1, r1, #1
    412c: eafffe9a     	b	0x3b9c <.plt+0x4a0>     @ imm = #-0x598  // CALL bcm2835_gpio_write

