00000ddc <comport_ports>:
     ddc: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
     de0: e3a02000     	mov	r2, #0
     de4: e24dd074     	sub	sp, sp, #116
     de8: e28040a0     	add	r4, r0, #160
     dec: e1a09000     	mov	r9, r0
     df0: e1a01002     	mov	r1, r2
     df4: e1a00004     	mov	r0, r4
     df8: e28d3010     	add	r3, sp, #16
     dfc: ebffff57     	bl	0xb60 <.plt+0x158>      @ imm = #-0x2a4
     e00: e3500002     	cmp	r0, #2
     e04: 0a00003f     	beq	0xf08 <comport_ports+0x12c> @ imm = #0xfc
     e08: e3500003     	cmp	r0, #3
     e0c: 0a000037     	beq	0xef0 <comport_ports+0x114> @ imm = #0xdc
     e10: e3500001     	cmp	r0, #1
     e14: 0a00002f     	beq	0xed8 <comport_ports+0xfc> @ imm = #0xbc
     e18: e59d2010     	ldr	r2, [sp, #0x10]
     e1c: e3520000     	cmp	r2, #0
     e20: 0a00002a     	beq	0xed0 <comport_ports+0xf4> @ imm = #0xa8
     e24: e59fb0f4     	ldr	r11, [pc, #0xf4]        @ 0xf20 <comport_ports+0x144>
     e28: e59f80f4     	ldr	r8, [pc, #0xf4]         @ 0xf24 <comport_ports+0x148>
     e2c: e08fb00b     	add	r11, pc, r11
     e30: e2899a01     	add	r9, r9, #4096
     e34: e3a04000     	mov	r4, #0
     e38: e28d7034     	add	r7, sp, #52
     e3c: e3a0a001     	mov	r10, #1
     e40: e59d5014     	ldr	r5, [sp, #0x14]
     e44: e1a01008     	mov	r1, r8
     e48: e1a06104     	lsl	r6, r4, #2
     e4c: e7950104     	ldr	r0, [r5, r4, lsl #2]
     e50: ebffff24     	bl	0xae8 <.plt+0xe0>       @ imm = #-0x370
     e54: e1a01007     	mov	r1, r7
     e58: e3700001     	cmn	r0, #1
     e5c: e1a05000     	mov	r5, r0
     e60: 0a000016     	beq	0xec0 <comport_ports+0xe4> @ imm = #0x58
     e64: ebffff64     	bl	0xbfc <.plt+0x1f4>      @ imm = #-0x270
     e68: e3700001     	cmn	r0, #1
     e6c: 0a000011     	beq	0xeb8 <comport_ports+0xdc> @ imm = #0x44
     e70: ee074a90     	vmov	s15, r4
     e74: e59dc014     	ldr	r12, [sp, #0x14]
     e78: e3a0e002     	mov	lr, #2
     e7c: e58de008     	str	lr, [sp, #0x8]
     e80: eeb80a67     	vcvt.f32.u32	s0, s15
     e84: e58da000     	str	r10, [sp]
     e88: ed8d0a01     	vstr	s0, [sp, #4]
     e8c: e79c0006     	ldr	r0, [r12, r6]
     e90: ebfffee1     	bl	0xa1c <.plt+0x14>       @ imm = #-0x47c
     e94: e59960e8     	ldr	r6, [r9, #0xe8]
     e98: e58d000c     	str	r0, [sp, #0xc]
     e9c: e1a0000b     	mov	r0, r11
     ea0: ebfffedd     	bl	0xa1c <.plt+0x14>       @ imm = #-0x48c
     ea4: e1a0300d     	mov	r3, sp
     ea8: e3a02002     	mov	r2, #2
     eac: e1a01000     	mov	r1, r0
     eb0: e1a00006     	mov	r0, r6
     eb4: ebffff38     	bl	0xb9c <.plt+0x194>      @ imm = #-0x320
     eb8: e1a00005     	mov	r0, r5
     ebc: ebffff45     	bl	0xbd8 <.plt+0x1d0>      @ imm = #-0x2ec
     ec0: e59d1010     	ldr	r1, [sp, #0x10]
     ec4: e2844001     	add	r4, r4, #1
     ec8: e1510004     	cmp	r1, r4
     ecc: 8affffdb     	bhi	0xe40 <comport_ports+0x64> @ imm = #-0x94
     ed0: e28dd074     	add	sp, sp, #116
     ed4: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
     ed8: e59f1048     	ldr	r1, [pc, #0x48]         @ 0xf28 <comport_ports+0x14c>
     edc: e1a02004     	mov	r2, r4
     ee0: e08f1001     	add	r1, pc, r1
     ee4: e1a00009     	mov	r0, r9
     ee8: ebffff3d     	bl	0xbe4 <.plt+0x1dc>      @ imm = #-0x30c
     eec: eaffffc9     	b	0xe18 <comport_ports+0x3c> @ imm = #-0xdc
     ef0: e59f0034     	ldr	r0, [pc, #0x34]         @ 0xf2c <comport_ports+0x150>
     ef4: e1a02004     	mov	r2, r4
     ef8: e08f1000     	add	r1, pc, r0
     efc: e1a00009     	mov	r0, r9
     f00: ebffff37     	bl	0xbe4 <.plt+0x1dc>      @ imm = #-0x324
     f04: eaffffc3     	b	0xe18 <comport_ports+0x3c> @ imm = #-0xf4
     f08: e59f3020     	ldr	r3, [pc, #0x20]         @ 0xf30 <comport_ports+0x154>
     f0c: e1a02004     	mov	r2, r4
     f10: e08f1003     	add	r1, pc, r3
     f14: e1a00009     	mov	r0, r9
     f18: ebffff31     	bl	0xbe4 <.plt+0x1dc>      @ imm = #-0x33c
     f1c: eaffffbd     	b	0xe18 <comport_ports+0x3c> @ imm = #-0x10c
     f20: e4 2f 00 00  	.word	0x00002fe4
     f24: 02 09 00 00  	.word	0x00000902
     f28: c8 2e 00 00  	.word	0x00002ec8
     f2c: ec 2e 00 00  	.word	0x00002eec
     f30: bc 2e 00 00  	.word	0x00002ebc

