00005ccc <_getAllAdcsDuringCalibration>:
    5ccc: e92d41f0     	push	{r4, r5, r6, r7, r8, lr}
    5cd0: e2807d75     	add	r7, r0, #7488
    5cd4: e24dd008     	sub	sp, sp, #8
    5cd8: e287701c     	add	r7, r7, #28
    5cdc: e2804f47     	add	r4, r0, #284
    5ce0: e28d6004     	add	r6, sp, #4
    5ce4: e3a05000     	mov	r5, #0
    5ce8: e5d40000     	ldrb	r0, [r4]
    5cec: e2848f71     	add	r8, r4, #452
    5cf0: e5cd5006     	strb	r5, [sp, #0x6]
    5cf4: e2003007     	and	r3, r0, #7
    5cf8: e1a001a0     	lsr	r0, r0, #3
    5cfc: e1a02143     	asr	r2, r3, #2
    5d00: e1a01303     	lsl	r1, r3, #6
    5d04: e382c006     	orr	r12, r2, #6
    5d08: e5cd1005     	strb	r1, [sp, #0x5]
    5d0c: e5cdc004     	strb	r12, [sp, #0x4]
    5d10: ebfff79b     	bl	0x3b84 <.plt+0x488>     @ imm = #-0x2194  // CALL bcm2835_spi_chipSelect
    5d14: e3a01003     	mov	r1, #3
    5d18: e1a00006     	mov	r0, r6
    5d1c: ebfff6d5     	bl	0x3878 <.plt+0x17c>     @ imm = #-0x24ac  // CALL bcm2835_spi_transfern
    5d20: e5dd2005     	ldrb	r2, [sp, #0x5]
    5d24: e5dd3006     	ldrb	r3, [sp, #0x6]
    5d28: e1a00004     	mov	r0, r4
    5d2c: e2844fe2     	add	r4, r4, #904
    5d30: e1a01402     	lsl	r1, r2, #8
    5d34: e201cc0f     	and	r12, r1, #3840
    5d38: e183200c     	orr	r2, r3, r12
    5d3c: ee072a90     	vmov	s15, r2
    5d40: eeb80a67     	vcvt.f32.u32	s0, s15
    5d44: ed040ae1     	vstr	s0, [r4, #-900]
    5d48: ebfff7f0     	bl	0x3d10 <.plt+0x614>     @ imm = #-0x2040  // CALL _stabliseADC_DuringCalibration
    5d4c: e55401c4     	ldrb	r0, [r4, #-0x1c4]
    5d50: e5cd5006     	strb	r5, [sp, #0x6]
    5d54: e2003007     	and	r3, r0, #7
    5d58: e1a001a0     	lsr	r0, r0, #3
    5d5c: e1a01143     	asr	r1, r3, #2
    5d60: e1a0c303     	lsl	r12, r3, #6
    5d64: e3812006     	orr	r2, r1, #6
    5d68: e5cdc005     	strb	r12, [sp, #0x5]
    5d6c: e5cd2004     	strb	r2, [sp, #0x4]
    5d70: ebfff783     	bl	0x3b84 <.plt+0x488>     @ imm = #-0x21f4  // CALL bcm2835_spi_chipSelect
    5d74: e3a01003     	mov	r1, #3
    5d78: e1a00006     	mov	r0, r6
    5d7c: ebfff6bd     	bl	0x3878 <.plt+0x17c>     @ imm = #-0x250c  // CALL bcm2835_spi_transfern
    5d80: e5dd1005     	ldrb	r1, [sp, #0x5]
    5d84: e5dd3006     	ldrb	r3, [sp, #0x6]
    5d88: e1a00008     	mov	r0, r8
    5d8c: e1a0c401     	lsl	r12, r1, #8
    5d90: e20c2c0f     	and	r2, r12, #3840
    5d94: e1831002     	orr	r1, r3, r2
    5d98: ee001a90     	vmov	s1, r1
    5d9c: eeb81a60     	vcvt.f32.u32	s2, s1
    5da0: ed881a01     	vstr	s2, [r8, #4]
    5da4: ebfff7d9     	bl	0x3d10 <.plt+0x614>     @ imm = #-0x209c  // CALL _stabliseADC_DuringCalibration
    5da8: e1570004     	cmp	r7, r4
    5dac: 1affffcd     	bne	0x5ce8 <_getAllAdcsDuringCalibration+0x1c> @ imm = #-0xcc
    5db0: e28dd008     	add	sp, sp, #8
    5db4: e8bd81f0     	pop	{r4, r5, r6, r7, r8, pc}

