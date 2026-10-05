00001c18 <loadRawToLayer>:
    1c18: e92d41f0     	push	{r4, r5, r6, r7, r8, lr}
    1c1c: e24dd078     	sub	sp, sp, #120
    1c20: e28d5014     	add	r5, sp, #20
    1c24: e3a02064     	mov	r2, #100
    1c28: e1a06000     	mov	r6, r0
    1c2c: e1a00003     	mov	r0, r3
    1c30: e1a01005     	mov	r1, r5
    1c34: e1a04003     	mov	r4, r3
    1c38: ebfffbdb     	bl	0xbac <.plt+0x1b8>      @ imm = #-0x1094
    1c3c: e2840008     	add	r0, r4, #8
    1c40: ebfffbac     	bl	0xaf8 <.plt+0x104>      @ imm = #-0x1150
    1c44: e2504000     	subs	r4, r0, #0
    1c48: ca000001     	bgt	0x1c54 <loadRawToLayer+0x3c> @ imm = #0x4
    1c4c: e28dd078     	add	sp, sp, #120
    1c50: e8bd81f0     	pop	{r4, r5, r6, r7, r8, pc}
    1c54: e59f00dc     	ldr	r0, [pc, #0xdc]         @ 0x1d38 <loadRawToLayer+0x120>
    1c58: e3a03002     	mov	r3, #2
    1c5c: e58d3004     	str	r3, [sp, #0x4]
    1c60: e08f0000     	add	r0, pc, r0
    1c64: ebfffb67     	bl	0xa08 <.plt+0x14>       @ imm = #-0x1264
    1c68: e59f10cc     	ldr	r1, [pc, #0xcc]         @ 0x1d3c <loadRawToLayer+0x124>
    1c6c: e08f1001     	add	r1, pc, r1
    1c70: e58d0008     	str	r0, [sp, #0x8]
    1c74: e1a00005     	mov	r0, r5
    1c78: ebfffb68     	bl	0xa20 <.plt+0x2c>       @ imm = #-0x1260
    1c7c: e2508000     	subs	r8, r0, #0
    1c80: 0a000003     	beq	0x1c94 <loadRawToLayer+0x7c> @ imm = #0xc
    1c84: e0867104     	add	r7, r6, r4, lsl #2
    1c88: e597208c     	ldr	r2, [r7, #0x8c]
    1c8c: e3520000     	cmp	r2, #0
    1c90: ca000012     	bgt	0x1ce0 <loadRawToLayer+0xc8> @ imm = #0x48
    1c94: e59fc0a4     	ldr	r12, [pc, #0xa4]        @ 0x1d40 <loadRawToLayer+0x128>
    1c98: e1a01005     	mov	r1, r5
    1c9c: e59f40a0     	ldr	r4, [pc, #0xa0]         @ 0x1d44 <loadRawToLayer+0x12c>
    1ca0: e3a05000     	mov	r5, #0
    1ca4: e08f000c     	add	r0, pc, r12
    1ca8: ebfffb98     	bl	0xb10 <.plt+0x11c>      @ imm = #-0x11a0
    1cac: e08f0004     	add	r0, pc, r4
    1cb0: e3a0e001     	mov	lr, #1
    1cb4: e5966054     	ldr	r6, [r6, #0x54]
    1cb8: e58de00c     	str	lr, [sp, #0xc]
    1cbc: e58d5010     	str	r5, [sp, #0x10]
    1cc0: ebfffb50     	bl	0xa08 <.plt+0x14>       @ imm = #-0x12c0
    1cc4: e28d3004     	add	r3, sp, #4
    1cc8: e3a02002     	mov	r2, #2
    1ccc: e1a01000     	mov	r1, r0
    1cd0: e1a00006     	mov	r0, r6
    1cd4: ebfffba5     	bl	0xb70 <.plt+0x17c>      @ imm = #-0x116c
    1cd8: e28dd078     	add	sp, sp, #120
    1cdc: e8bd81f0     	pop	{r4, r5, r6, r7, r8, pc}
    1ce0: ee074a90     	vmov	s15, r4
    1ce4: e1a00006     	mov	r0, r6
    1ce8: eeb80ae7     	vcvt.f32.s32	s0, s15
    1cec: ebfffb54     	bl	0xa44 <.plt+0x50>       @ imm = #-0x12b0
    1cf0: e3a02000     	mov	r2, #0
    1cf4: e1a01002     	mov	r1, r2
    1cf8: e1a00008     	mov	r0, r8
    1cfc: ebfffba1     	bl	0xb88 <.plt+0x194>      @ imm = #-0x117c
    1d00: e3500000     	cmp	r0, #0
    1d04: 0a000005     	beq	0x1d20 <loadRawToLayer+0x108> @ imm = #0x14
    1d08: e1a00008     	mov	r0, r8
    1d0c: e3a055fe     	mov	r5, #1065353216
    1d10: ebfffb84     	bl	0xb28 <.plt+0x134>      @ imm = #-0x11f0
    1d14: e59f002c     	ldr	r0, [pc, #0x2c]         @ 0x1d48 <loadRawToLayer+0x130>
    1d18: e08f0000     	add	r0, pc, r0
    1d1c: eaffffe3     	b	0x1cb0 <loadRawToLayer+0x98> @ imm = #-0x74
    1d20: e1a03008     	mov	r3, r8
    1d24: e597208c     	ldr	r2, [r7, #0x8c]
    1d28: e5970058     	ldr	r0, [r7, #0x58]
    1d2c: e3a01004     	mov	r1, #4
    1d30: ebfffb52     	bl	0xa80 <.plt+0x8c>       @ imm = #-0x12b8
    1d34: eafffff3     	b	0x1d08 <loadRawToLayer+0xf0> @ imm = #-0x34
    1d38: 60 10 00 00  	.word	0x00001060
    1d3c: 10 10 00 00  	.word	0x00001010
    1d40: 28 10 00 00  	.word	0x00001028
    1d44: 40 0f 00 00  	.word	0x00000f40
    1d48: d4 0e 00 00  	.word	0x00000ed4

