00002434 <stereo_in_tilde_free>:
    2434: e92d4010     	push	{r4, lr}
    2438: e1a04000     	mov	r4, r0
    243c: e5900024     	ldr	r0, [r0, #0x24]
    2440: ebffff6c     	bl	0x21f8 <.plt+0x194>     @ imm = #-0x250  // CALL inlet_free
    2444: e5940028     	ldr	r0, [r4, #0x28]
    2448: ebffff25     	bl	0x20e4 <.plt+0x80>      @ imm = #-0x36c  // CALL outlet_free
    244c: e594002c     	ldr	r0, [r4, #0x2c]
    2450: e8bd4010     	pop	{r4, lr}
    2454: eaffff22     	b	0x20e4 <.plt+0x80>      @ imm = #-0x378  // CALL outlet_free

