00002490 <main_out_stereo_tilde_free>:
    2490: e92d4010     	push	{r4, lr}
    2494: e1a04000     	mov	r4, r0
    2498: e5900028     	ldr	r0, [r0, #0x28]
    249c: ebffff0f     	bl	0x20e0 <.plt+0x188>     @ imm = #-0x3c4
    24a0: e594002c     	ldr	r0, [r4, #0x2c]
    24a4: ebfffecb     	bl	0x1fd8 <.plt+0x80>      @ imm = #-0x4d4
    24a8: e5940030     	ldr	r0, [r4, #0x30]
    24ac: e8bd4010     	pop	{r4, lr}
    24b0: eafffec8     	b	0x1fd8 <.plt+0x80>      @ imm = #-0x4e0

