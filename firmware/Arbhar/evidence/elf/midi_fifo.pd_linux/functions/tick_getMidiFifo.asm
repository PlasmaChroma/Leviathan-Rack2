00000e2c <tick_getMidiFifo>:
     e2c: e92d4010     	push	{r4, lr}
     e30: e1a04000     	mov	r4, r0
     e34: ebfffe57     	bl	0x798 <.plt+0xd4>       @ imm = #-0x6a4
     e38: e5940020     	ldr	r0, [r4, #0x20]
     e3c: eeb70b00     	vmov.f64	d0, #1.000000e+00
     e40: e8bd4010     	pop	{r4, lr}
     e44: eafffe32     	b	0x714 <.plt+0x50>       @ imm = #-0x738

