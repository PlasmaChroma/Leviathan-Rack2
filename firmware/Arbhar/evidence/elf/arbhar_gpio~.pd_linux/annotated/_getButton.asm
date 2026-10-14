0000af98 <_getButton>:
    af98: e35200ff     	cmp	r2, #255
    af9c: 0a000020     	beq	0xb024 <_getButton+0x8c> @ imm = #0x80
    afa0: e92d4070     	push	{r4, r5, r6, lr}
    afa4: e1a05000     	mov	r5, r0
    afa8: e5d14005     	ldrb	r4, [r1, #0x5]
    afac: e5d1c001     	ldrb	r12, [r1, #0x1]
    afb0: e5d03075     	ldrb	r3, [r0, #0x75]
    afb4: e3540000     	cmp	r4, #0
    afb8: 05c12001     	strbeq	r2, [r1, #0x1]
    afbc: e3520000     	cmp	r2, #0
    afc0: e5c12000     	strb	r2, [r1]
    afc4: e1a04001     	mov	r4, r1
    afc8: e3a02001     	mov	r2, #1
    afcc: e5c1c006     	strb	r12, [r1, #0x6]
    afd0: e5c12007     	strb	r2, [r1, #0x7]
    afd4: 0a000009     	beq	0xb000 <_getButton+0x68> @ imm = #0x24
    afd8: e3530000     	cmp	r3, #0
    afdc: 0a000017     	beq	0xb040 <_getButton+0xa8> @ imm = #0x5c
    afe0: e2850a01     	add	r0, r5, #4096
    afe4: e3a02000     	mov	r2, #0
    afe8: e3a03001     	mov	r3, #1
    afec: e5c42004     	strb	r2, [r4, #0x4]
    aff0: e5c43003     	strb	r3, [r4, #0x3]
    aff4: e5901ddc     	ldr	r1, [r0, #0xddc]
    aff8: e5841008     	str	r1, [r4, #0x8]
    affc: e8bd8070     	pop	{r4, r5, r6, pc}
    b000: e3530000     	cmp	r3, #0
    b004: 0a000009     	beq	0xb030 <_getButton+0x98> @ imm = #0x24
    b008: e5940008     	ldr	r0, [r4, #0x8]
    b00c: e3a01001     	mov	r1, #1
    b010: e3a0c000     	mov	r12, #0
    b014: e5c41004     	strb	r1, [r4, #0x4]
    b018: e5c4c003     	strb	r12, [r4, #0x3]
    b01c: e584000c     	str	r0, [r4, #0xc]
    b020: e8bd8070     	pop	{r4, r5, r6, pc}
    b024: e3a03000     	mov	r3, #0
    b028: e5c13007     	strb	r3, [r1, #0x7]
    b02c: e12fff1e     	bx	lr
    b030: e59000f4     	ldr	r0, [r0, #0xf4]
    b034: ed9f0b05     	vldr	d0, [pc, #20]           @ 0xb050 <_getButton+0xb8>  // f64=0
    b038: ebffe23b     	bl	0x392c <.plt+0x230>     @ imm = #-0x7714  // CALL clock_delay
    b03c: eafffff1     	b	0xb008 <_getButton+0x70> @ imm = #-0x3c
    b040: ed9f0b02     	vldr	d0, [pc, #8]            @ 0xb050 <_getButton+0xb8>  // f64=0
    b044: e59000f0     	ldr	r0, [r0, #0xf0]
    b048: ebffe237     	bl	0x392c <.plt+0x230>     @ imm = #-0x7724  // CALL clock_delay
    b04c: eaffffe3     	b	0xafe0 <_getButton+0x48> @ imm = #-0x74
    b050: 00 00 00 00  	.word	0x00000000
    b054: 00 00 00 00  	.word	0x00000000

