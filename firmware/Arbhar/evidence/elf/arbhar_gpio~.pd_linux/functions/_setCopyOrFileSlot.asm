0000df08 <_setCopyOrFileSlot>:
    df08: e3510006     	cmp	r1, #6
    df0c: 82413007     	subhi	r3, r1, #7
    df10: 93a030ff     	movls	r3, #255
    df14: 83a010ff     	movhi	r1, #255
    df18: 86ef3073     	uxtbhi	r3, r3
    df1c: e5c01033     	strb	r1, [r0, #0x33]
    df20: e5c03034     	strb	r3, [r0, #0x34]
    df24: e12fff1e     	bx	lr

