00006b64 <led_controlForRecording>:
    6b64: e92d4070     	push	{r4, r5, r6, lr}
    6b68: e24dd020     	sub	sp, sp, #32
    6b6c: ed9f0a49     	vldr	s0, [pc, #292]          @ 0x6c98 <led_controlForRecording+0x134>
    6b70: e1a05000     	mov	r5, r0
    6b74: ebfff31b     	bl	0x37e8 <.plt+0xec>      @ imm = #-0x3394
    6b78: e59f3124     	ldr	r3, [pc, #0x124]        @ 0x6ca4 <led_controlForRecording+0x140>
    6b7c: eddf7a46     	vldr	s15, [pc, #280]         @ 0x6c9c <led_controlForRecording+0x138>
    6b80: e08f0003     	add	r0, pc, r3
    6b84: e5901004     	ldr	r1, [r0, #0x4]
    6b88: ee200a27     	vmul.f32	s0, s0, s15
    6b8c: eefd0ac0     	vcvt.s32.f32	s1, s0
    6b90: ee104a90     	vmov	r4, s1
    6b94: e1510004     	cmp	r1, r4
    6b98: 0a00002b     	beq	0x6c4c <led_controlForRecording+0xe8> @ imm = #0xac
    6b9c: e5d52024     	ldrb	r2, [r5, #0x24]
    6ba0: ee012a10     	vmov	s2, r2
    6ba4: e3540048     	cmp	r4, #72
    6ba8: ee024a10     	vmov	s4, r4
    6bac: e3a0e001     	mov	lr, #1
    6bb0: eef81a41     	vcvt.f32.u32	s3, s2
    6bb4: e3a0c000     	mov	r12, #0
    6bb8: e3a03000     	mov	r3, #0
    6bbc: e344c30e     	movt	r12, #0x430e
    6bc0: e344330f     	movt	r3, #0x430f
    6bc4: e58de000     	str	lr, [sp]
    6bc8: e58de008     	str	lr, [sp, #0x8]
    6bcc: e58de010     	str	lr, [sp, #0x10]
    6bd0: e58dc00c     	str	r12, [sp, #0xc]
    6bd4: e58de018     	str	lr, [sp, #0x18]
    6bd8: eef82ac2     	vcvt.f32.s32	s5, s4
    6bdc: e58d301c     	str	r3, [sp, #0x1c]
    6be0: edcd1a01     	vstr	s3, [sp, #4]
    6be4: edcd2a05     	vstr	s5, [sp, #20]
    6be8: da00001e     	ble	0x6c68 <led_controlForRecording+0x104> @ imm = #0x78
    6bec: e59fc0b4     	ldr	r12, [pc, #0xb4]        @ 0x6ca8 <led_controlForRecording+0x144>
    6bf0: e59f30b4     	ldr	r3, [pc, #0xb4]         @ 0x6cac <led_controlForRecording+0x148>
    6bf4: e08fe00c     	add	lr, pc, r12
    6bf8: e08f1003     	add	r1, pc, r3
    6bfc: e59e002c     	ldr	r0, [lr, #0x2c]
    6c00: e5814004     	str	r4, [r1, #0x4]
    6c04: e1520000     	cmp	r2, r0
    6c08: 0a00000a     	beq	0x6c38 <led_controlForRecording+0xd4> @ imm = #0x28
    6c0c: eeb17a0c     	vmov.f32	s14, #7.000000e+00
    6c10: e5d54027     	ldrb	r4, [r5, #0x27]
    6c14: ed9f3a21     	vldr	s6, [pc, #132]          @ 0x6ca0 <led_controlForRecording+0x13c>
    6c18: e1a00005     	mov	r0, r5
    6c1c: e3540000     	cmp	r4, #0
    6c20: eeb10a00     	vmov.f32	s0, #4.000000e+00
    6c24: 1eb03a47     	vmovne.f32	s6, s14
    6c28: e3520000     	cmp	r2, #0
    6c2c: 0eb00a43     	vmoveq.f32	s0, s6
    6c30: ebfff42d     	bl	0x3cec <.plt+0x5f0>     @ imm = #-0x2f4c
    6c34: e5d50024     	ldrb	r0, [r5, #0x24]
    6c38: e59f5070     	ldr	r5, [pc, #0x70]         @ 0x6cb0 <led_controlForRecording+0x14c>
    6c3c: e08f6005     	add	r6, pc, r5
    6c40: e586002c     	str	r0, [r6, #0x2c]
    6c44: e28dd020     	add	sp, sp, #32
    6c48: e8bd8070     	pop	{r4, r5, r6, pc}
    6c4c: e59f2060     	ldr	r2, [pc, #0x60]         @ 0x6cb4 <led_controlForRecording+0x150>
    6c50: e08f6002     	add	r6, pc, r2
    6c54: e5d52024     	ldrb	r2, [r5, #0x24]
    6c58: e596002c     	ldr	r0, [r6, #0x2c]
    6c5c: e1520000     	cmp	r2, r0
    6c60: 1affffce     	bne	0x6ba0 <led_controlForRecording+0x3c> @ imm = #-0xc8
    6c64: eafffff3     	b	0x6c38 <led_controlForRecording+0xd4> @ imm = #-0x34
    6c68: e59f0048     	ldr	r0, [pc, #0x48]         @ 0x6cb8 <led_controlForRecording+0x154>
    6c6c: e2851a01     	add	r1, r5, #4096
    6c70: e08f0000     	add	r0, pc, r0
    6c74: e5916dac     	ldr	r6, [r1, #0xdac]
    6c78: ebfff2aa     	bl	0x3728 <.plt+0x2c>      @ imm = #-0x3558
    6c7c: e3a02004     	mov	r2, #4
    6c80: e1a0300d     	mov	r3, sp
    6c84: e1a01000     	mov	r1, r0
    6c88: e1a00006     	mov	r0, r6
    6c8c: ebfff3f8     	bl	0x3c74 <.plt+0x578>     @ imm = #-0x3020
    6c90: e5d52024     	ldrb	r2, [r5, #0x24]
    6c94: eaffffd4     	b	0x6bec <led_controlForRecording+0x88> @ imm = #-0xb0
    6c98: 00 00 79 43  	.word	0x43790000
    6c9c: 14 1a 1b 39  	.word	0x391b1a14
    6ca0: 00 00 00 00  	.word	0x00000000
    6ca4: 64 07 02 00  	.word	0x00020764
    6ca8: bc 07 02 00  	.word	0x000207bc
    6cac: ec 06 02 00  	.word	0x000206ec
    6cb0: 74 07 02 00  	.word	0x00020774
    6cb4: 60 07 02 00  	.word	0x00020760
    6cb8: 44 de 00 00  	.word	0x0000de44

