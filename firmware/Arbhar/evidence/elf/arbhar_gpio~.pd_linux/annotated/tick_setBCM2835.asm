00008608 <tick_setBCM2835>:
    8608: e92d4010     	push	{r4, lr}
    860c: e1a04000     	mov	r4, r0
    8610: ed9f0a05     	vldr	s0, [pc, #20]           @ 0x862c <tick_setBCM2835+0x24>  // f32=0
    8614: ebffed2d     	bl	0x3ad0 <.plt+0x3d4>     @ imm = #-0x4b4c  // CALL _setTriggerOut
    8618: e1a00004     	mov	r0, r4
    861c: e3a02000     	mov	r2, #0
    8620: e3a01075     	mov	r1, #117
    8624: e8bd4010     	pop	{r4, lr}
    8628: eaffed2e     	b	0x3ae8 <.plt+0x3ec>     @ imm = #-0x4b48  // CALL writeToSharedMem
    862c: 00 00 00 00  	.word	0x00000000

