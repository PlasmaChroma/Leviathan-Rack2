00000948 <fftwObject_tilde_free>:
     948: e92d4010     	push	{r4, lr}
     94c: e2804866     	add	r4, r0, #6684672
     950: e2844a01     	add	r4, r4, #4096
     954: e5940050     	ldr	r0, [r4, #0x50]
     958: ebffff60     	bl	0x6e0 <.plt+0xc8>       @ imm = #-0x280  // CALL inlet_free
     95c: e5940054     	ldr	r0, [r4, #0x54]
     960: e8bd4010     	pop	{r4, lr}
     964: eaffff39     	b	0x650 <.plt+0x38>       @ imm = #-0x31c  // CALL outlet_free

