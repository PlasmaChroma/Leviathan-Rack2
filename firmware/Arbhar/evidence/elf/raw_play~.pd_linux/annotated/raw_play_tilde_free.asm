00002628 <raw_play_tilde_free>:
    2628: e92d4010     	push	{r4, lr}
    262c: e2804923     	add	r4, r0, #573440
    2630: e5940a30     	ldr	r0, [r4, #0xa30]
    2634: ebfffe75     	bl	0x2010 <.plt+0x80>      @ imm = #-0x62c  // CALL outlet_free
    2638: e5940a34     	ldr	r0, [r4, #0xa34]
    263c: e8bd4010     	pop	{r4, lr}
    2640: eafffe72     	b	0x2010 <.plt+0x80>      @ imm = #-0x638  // CALL outlet_free

