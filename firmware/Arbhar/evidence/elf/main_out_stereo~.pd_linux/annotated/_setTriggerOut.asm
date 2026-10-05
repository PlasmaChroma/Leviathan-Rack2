00002698 <_setTriggerOut>:
    2698: eefc7ac0     	vcvt.u32.f32	s15, s0
    269c: e3a00022     	mov	r0, #34
    26a0: ee171a90     	vmov	r1, s15
    26a4: e2011001     	and	r1, r1, #1
    26a8: eafffe83     	b	0x20bc <.plt+0x164>     @ imm = #-0x5f4  // CALL bcm2835_gpio_write

