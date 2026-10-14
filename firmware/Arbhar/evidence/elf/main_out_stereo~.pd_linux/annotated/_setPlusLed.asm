00002714 <_setPlusLed>:
    2714: eefc7ac0     	vcvt.u32.f32	s15, s0
    2718: e3a00028     	mov	r0, #40
    271c: e92d4010     	push	{r4, lr}
    2720: e24dd008     	sub	sp, sp, #8
    2724: edcd7a01     	vstr	s15, [sp, #4]
    2728: e5dd4004     	ldrb	r4, [sp, #0x4]
    272c: e2041001     	and	r1, r4, #1
    2730: ebfffe61     	bl	0x20bc <.plt+0x164>     @ imm = #-0x67c  // CALL bcm2835_gpio_write
    2734: e1a010a4     	lsr	r1, r4, #1
    2738: e3a00029     	mov	r0, #41
    273c: e28dd008     	add	sp, sp, #8
    2740: e8bd4010     	pop	{r4, lr}
    2744: eafffe5c     	b	0x20bc <.plt+0x164>     @ imm = #-0x690  // CALL bcm2835_gpio_write

