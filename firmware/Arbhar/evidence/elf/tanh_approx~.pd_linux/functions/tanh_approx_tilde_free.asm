00002510 <tanh_approx_tilde_free>:
    2510: e92d4010     	push	{r4, lr}
    2514: e1a04000     	mov	r4, r0
    2518: e5900028     	ldr	r0, [r0, #0x28]
    251c: ebfffee8     	bl	0x20c4 <.plt+0x17c>     @ imm = #-0x460
    2520: e594002c     	ldr	r0, [r4, #0x2c]
    2524: e8bd4010     	pop	{r4, lr}
    2528: eafffea6     	b	0x1fc8 <.plt+0x80>      @ imm = #-0x568

