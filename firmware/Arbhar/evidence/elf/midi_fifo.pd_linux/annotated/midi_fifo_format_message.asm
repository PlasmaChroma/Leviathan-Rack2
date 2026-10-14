00000e48 <midi_fifo_format_message>:
     e48: e92d47f0     	push	{r4, r5, r6, r7, r8, r9, r10, lr}
     e4c: e2526000     	subs	r6, r2, #0
     e50: e59fa094     	ldr	r10, [pc, #0x94]        @ 0xeec <midi_fifo_format_message+0xa4>  // u32=0x111a0; f32?=9.81581548e-41
     e54: e24dd018     	sub	sp, sp, #24
     e58: e08fa00a     	add	r10, pc, r10
     e5c: da000020     	ble	0xee4 <midi_fifo_format_message+0x9c> @ imm = #0x80
     e60: e1a09000     	mov	r9, r0
     e64: e1a0800d     	mov	r8, sp
     e68: e1a05001     	mov	r5, r1
     e6c: e3a04000     	mov	r4, #0
     e70: e3a07001     	mov	r7, #1
     e74: e284c002     	add	r12, r4, #2
     e78: e3a02003     	mov	r2, #3
     e7c: e156000c     	cmp	r6, r12
     e80: e59f1068     	ldr	r1, [pc, #0x68]         @ 0xef0 <midi_fifo_format_message+0xa8>  // u32=0x60; f32?=1.34524653e-43
     e84: e1a03008     	mov	r3, r8
     e88: e0844002     	add	r4, r4, r2
     e8c: da000011     	ble	0xed8 <midi_fifo_format_message+0x90> @ imm = #0x44
     e90: e5d50000     	ldrb	r0, [r5]
     e94: e5d5e001     	ldrb	lr, [r5, #0x1]
     e98: e5d5c002     	ldrb	r12, [r5, #0x2]
     e9c: ee060a90     	vmov	s13, r0
     ea0: e58d7000     	str	r7, [sp]
     ea4: ee07ea10     	vmov	s14, lr
     ea8: e58d7008     	str	r7, [sp, #0x8]
     eac: ee07ca90     	vmov	s15, r12
     eb0: e58d7010     	str	r7, [sp, #0x10]
     eb4: eeb80ae6     	vcvt.f32.s32	s0, s13
     eb8: e599001c     	ldr	r0, [r9, #0x1c]
     ebc: eef80ac7     	vcvt.f32.s32	s1, s14
     ec0: ed8d0a01     	vstr	s0, [sp, #4]
     ec4: eeb81ae7     	vcvt.f32.s32	s2, s15
     ec8: edcd0a03     	vstr	s1, [sp, #12]
     ecc: ed8d1a05     	vstr	s2, [sp, #20]
     ed0: e79a1001     	ldr	r1, [r10, r1]
     ed4: ebfffe2c     	bl	0x78c <.plt+0xc8>       @ imm = #-0x750  // CALL outlet_list
     ed8: e1560004     	cmp	r6, r4
     edc: e2855003     	add	r5, r5, #3
     ee0: caffffe3     	bgt	0xe74 <midi_fifo_format_message+0x2c> @ imm = #-0x74
     ee4: e28dd018     	add	sp, sp, #24
     ee8: e8bd87f0     	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
     eec: a0 11 01 00  	.word	0x000111a0
     ef0: 60 00 00 00  	.word	0x00000060

