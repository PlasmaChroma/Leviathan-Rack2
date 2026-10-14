0000a850 <_getCaptureCvToShMem>:
    a850: e92d4010     	push	{r4, lr}
    a854: e1a04000     	mov	r4, r0
    a858: e3a00020     	mov	r0, #32
    a85c: ebffe3c9     	bl	0x3788 <.plt+0x8c>      @ imm = #-0x70dc
    a860: e3a0101b     	mov	r1, #27
    a864: e16f2f10     	clz	r2, r0
    a868: e1a00004     	mov	r0, r4
    a86c: e8bd4010     	pop	{r4, lr}
    a870: e1a022a2     	lsr	r2, r2, #5
    a874: eaffe49b     	b	0x3ae8 <.plt+0x3ec>     @ imm = #-0x6d94

