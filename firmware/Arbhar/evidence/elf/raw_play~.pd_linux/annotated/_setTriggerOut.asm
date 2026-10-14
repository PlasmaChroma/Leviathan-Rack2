00002990 <_setTriggerOut>:
    2990: eefc7ac0     	vcvt.u32.f32	s15, s0
    2994: e3a00022     	mov	r0, #34
    2998: ee171a90     	vmov	r1, s15
    299c: e2011001     	and	r1, r1, #1
    29a0: eafffdd9     	b	0x210c <.plt+0x17c>     @ imm = #-0x89c  // CALL bcm2835_gpio_write

