00001ac4 <saveLayerToRaw>:
    1ac4: e92d40f0     	push	{r4, r5, r6, r7, lr}
    1ac8: e24dd084     	sub	sp, sp, #132
    1acc: e28d501c     	add	r5, sp, #28
    1ad0: e1a04003     	mov	r4, r3
    1ad4: e3a02064     	mov	r2, #100
    1ad8: e1a06000     	mov	r6, r0
    1adc: e1a01005     	mov	r1, r5
    1ae0: e2830008     	add	r0, r3, #8
    1ae4: ebfffc30     	bl	0xbac <.plt+0x1b8>      @ imm = #-0xf40  // CALL atom_string
    1ae8: e1a00004     	mov	r0, r4
    1aec: ebfffc01     	bl	0xaf8 <.plt+0x104>      @ imm = #-0xffc  // CALL atom_getint
    1af0: e2504000     	subs	r4, r0, #0
    1af4: ca000001     	bgt	0x1b00 <saveLayerToRaw+0x3c> @ imm = #0x4
    1af8: e28dd084     	add	sp, sp, #132
    1afc: e8bd80f0     	pop	{r4, r5, r6, r7, pc}
    1b00: e59f10f8     	ldr	r1, [pc, #0xf8]         @ 0x1c00 <saveLayerToRaw+0x13c>  // u32=0x1174; f32?=6.26100154e-42
    1b04: e1a00005     	mov	r0, r5
    1b08: e08f1001     	add	r1, pc, r1
    1b0c: ebfffbc3     	bl	0xa20 <.plt+0x2c>       @ imm = #-0x10f4  // CALL fopen
    1b10: e2507000     	subs	r7, r0, #0
    1b14: 0a00002d     	beq	0x1bd0 <saveLayerToRaw+0x10c> @ imm = #0xb4
    1b18: e0865104     	add	r5, r6, r4, lsl #2
    1b1c: e595308c     	ldr	r3, [r5, #0x8c]
    1b20: e3530000     	cmp	r3, #0
    1b24: ca00001a     	bgt	0x1b94 <saveLayerToRaw+0xd0> @ imm = #0x68
    1b28: e1a00007     	mov	r0, r7
    1b2c: e3a07001     	mov	r7, #1
    1b30: ebfffbfc     	bl	0xb28 <.plt+0x134>      @ imm = #-0x1010  // CALL fclose
    1b34: ee074a90     	vmov	s15, r4
    1b38: e59f40c4     	ldr	r4, [pc, #0xc4]         @ 0x1c04 <saveLayerToRaw+0x140>  // u32=0x1168; f32?=6.24418596e-42
    1b3c: e3a01002     	mov	r1, #2
    1b40: eeb80ae7     	vcvt.f32.s32	s0, s15
    1b44: e58d100c     	str	r1, [sp, #0xc]
    1b48: e08f0004     	add	r0, pc, r4
    1b4c: e58d7004     	str	r7, [sp, #0x4]
    1b50: ed8d0a02     	vstr	s0, [sp, #8]
    1b54: ebfffbab     	bl	0xa08 <.plt+0x14>       @ imm = #-0x1154  // CALL gensym
    1b58: e3a025fe     	mov	r2, #1065353216
    1b5c: e5966054     	ldr	r6, [r6, #0x54]
    1b60: e58d2018     	str	r2, [sp, #0x18]
    1b64: e58d7014     	str	r7, [sp, #0x14]
    1b68: e58d0010     	str	r0, [sp, #0x10]
    1b6c: e59f0094     	ldr	r0, [pc, #0x94]         @ 0x1c08 <saveLayerToRaw+0x144>  // u32=0x107c; f32?=5.91347952e-42
    1b70: e08f0000     	add	r0, pc, r0
    1b74: ebfffba3     	bl	0xa08 <.plt+0x14>       @ imm = #-0x1174  // CALL gensym
    1b78: e28d3004     	add	r3, sp, #4
    1b7c: e3a02003     	mov	r2, #3
    1b80: e1a01000     	mov	r1, r0
    1b84: e1a00006     	mov	r0, r6
    1b88: ebfffbf8     	bl	0xb70 <.plt+0x17c>      @ imm = #-0x1020  // CALL outlet_list
    1b8c: e28dd084     	add	sp, sp, #132
    1b90: e8bd80f0     	pop	{r4, r5, r6, r7, pc}
    1b94: e59fc070     	ldr	r12, [pc, #0x70]        @ 0x1c0c <saveLayerToRaw+0x148>  // u32=0x10fc; f32?=6.09284572e-42
    1b98: e08f000c     	add	r0, pc, r12
    1b9c: ebfffbdb     	bl	0xb10 <.plt+0x11c>      @ imm = #-0x1094  // CALL post
    1ba0: e3a02000     	mov	r2, #0
    1ba4: e1a01002     	mov	r1, r2
    1ba8: e1a00007     	mov	r0, r7
    1bac: ebfffbf5     	bl	0xb88 <.plt+0x194>      @ imm = #-0x102c  // CALL fseek
    1bb0: e3500000     	cmp	r0, #0
    1bb4: 1affffdb     	bne	0x1b28 <saveLayerToRaw+0x64> @ imm = #-0x94
    1bb8: e1a03007     	mov	r3, r7
    1bbc: e595208c     	ldr	r2, [r5, #0x8c]
    1bc0: e5950058     	ldr	r0, [r5, #0x58]
    1bc4: e3a01004     	mov	r1, #4
    1bc8: ebfffba9     	bl	0xa74 <.plt+0x80>       @ imm = #-0x115c  // CALL fwrite
    1bcc: eaffffd5     	b	0x1b28 <saveLayerToRaw+0x64> @ imm = #-0xac
    1bd0: e59f0038     	ldr	r0, [pc, #0x38]         @ 0x1c10 <saveLayerToRaw+0x14c>  // u32=0x10ac; f32?=5.98074185e-42
    1bd4: e08f1000     	add	r1, pc, r0
    1bd8: e1a00005     	mov	r0, r5
    1bdc: ebfffb8f     	bl	0xa20 <.plt+0x2c>       @ imm = #-0x11c4  // CALL fopen
    1be0: e59f202c     	ldr	r2, [pc, #0x2c]         @ 0x1c14 <saveLayerToRaw+0x150>  // u32=0x1098; f32?=5.95271588e-42
    1be4: e1a01005     	mov	r1, r5
    1be8: e1a07000     	mov	r7, r0
    1bec: e08f0002     	add	r0, pc, r2
    1bf0: ebfffbc6     	bl	0xb10 <.plt+0x11c>      @ imm = #-0x10e8  // CALL post
    1bf4: e3570000     	cmp	r7, #0
    1bf8: 0affffca     	beq	0x1b28 <saveLayerToRaw+0x64> @ imm = #-0xd8
    1bfc: eaffffc5     	b	0x1b18 <saveLayerToRaw+0x54> @ imm = #-0xec
    1c00: 74 11 00 00  	.word	0x00001174
    1c04: 68 11 00 00  	.word	0x00001168
    1c08: 7c 10 00 00  	.word	0x0000107c
    1c0c: fc 10 00 00  	.word	0x000010fc
    1c10: ac 10 00 00  	.word	0x000010ac
    1c14: 98 10 00 00  	.word	0x00001098

