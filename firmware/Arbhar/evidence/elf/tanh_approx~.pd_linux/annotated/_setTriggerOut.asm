000026f0 <_setTriggerOut>:
    26f0: eefc7ac0     	vcvt.u32.f32	s15, s0
    26f4: e3a00022     	mov	r0, #34
    26f8: ee171a90     	vmov	r1, s15
    26fc: e2011001     	and	r1, r1, #1
    2700: eafffe66     	b	0x20a0 <.plt+0x158>     @ imm = #-0x668  // CALL bcm2835_gpio_write

