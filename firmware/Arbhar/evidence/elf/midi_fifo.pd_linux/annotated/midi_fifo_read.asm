00000a84 <midi_fifo_read>:
     a84: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
     a88: e1a07000     	mov	r7, r0
     a8c: e5900024     	ldr	r0, [r0, #0x24]
     a90: e24dde43     	sub	sp, sp, #1072
     a94: e24dd004     	sub	sp, sp, #4
     a98: e3500000     	cmp	r0, #0
     a9c: ba000036     	blt	0xb7c <midi_fifo_read+0xf8> @ imm = #0xd8
     aa0: e28d4030     	add	r4, sp, #48
     aa4: e30023ff     	movw	r2, #0x3ff
     aa8: e1a01004     	mov	r1, r4
     aac: ebffff0f     	bl	0x6f0 <.plt+0x2c>       @ imm = #-0x3c4  // CALL read
     ab0: e2508000     	subs	r8, r0, #0
     ab4: da000033     	ble	0xb88 <midi_fifo_read+0x104> @ imm = #0xcc
     ab8: e0846008     	add	r6, r4, r8
     abc: e59f935c     	ldr	r9, [pc, #0x35c]        @ 0xe20 <midi_fifo_read+0x39c>  // u32=0x5b0; f32?=2.04029056e-42
     ac0: e59fa35c     	ldr	r10, [pc, #0x35c]       @ 0xe24 <midi_fifo_read+0x3a0>  // u32=0x58c; f32?=1.98984382e-42
     ac4: e3160001     	tst	r6, #1
     ac8: e08f9009     	add	r9, pc, r9
     acc: e3a0b000     	mov	r11, #0
     ad0: e08fa00a     	add	r10, pc, r10
     ad4: 0a000004     	beq	0xaec <midi_fifo_read+0x68> @ imm = #0x10
     ad8: e4d40001     	ldrb	r0, [r4], #1
     adc: e3100080     	tst	r0, #128
     ae0: 1a000045     	bne	0xbfc <midi_fifo_read+0x178> @ imm = #0x114
     ae4: e1540006     	cmp	r4, r6
     ae8: 0a000026     	beq	0xb88 <midi_fifo_read+0x104> @ imm = #0x98
     aec: e1a05004     	mov	r5, r4
     af0: e4d50001     	ldrb	r0, [r5], #1
     af4: e3100080     	tst	r0, #128
     af8: 1a000025     	bne	0xb94 <midi_fifo_read+0x110> @ imm = #0x94
     afc: e1a04005     	mov	r4, r5
     b00: e4d40001     	ldrb	r0, [r4], #1
     b04: e3100080     	tst	r0, #128
     b08: 0afffff5     	beq	0xae4 <midi_fifo_read+0x60> @ imm = #-0x2c
     b0c: ebffff2a     	bl	0x7bc <.plt+0xf8>       @ imm = #-0x358  // CALL midi_bytes_per_message
     b10: e3500000     	cmp	r0, #0
     b14: dafffff2     	ble	0xae4 <midi_fifo_read+0x60> @ imm = #-0x38
     b18: e5542001     	ldrb	r2, [r4, #-0x1]
     b1c: e1a00002     	mov	r0, r2
     b20: e58d200c     	str	r2, [sp, #0xc]
     b24: ebffff24     	bl	0x7bc <.plt+0xf8>       @ imm = #-0x370  // CALL midi_bytes_per_message
     b28: e1580000     	cmp	r8, r0
     b2c: baffffec     	blt	0xae4 <midi_fifo_read+0x60> @ imm = #-0x50
     b30: e59dc00c     	ldr	r12, [sp, #0xc]
     b34: e20ce0f0     	and	lr, r12, #240
     b38: e35e00a0     	cmp	lr, #160
     b3c: 0a000081     	beq	0xd48 <midi_fifo_read+0x2c4> @ imm = #0x204
     b40: da00007c     	ble	0xd38 <midi_fifo_read+0x2b4> @ imm = #0x1f0
     b44: e35e00d0     	cmp	lr, #208
     b48: 0a00007e     	beq	0xd48 <midi_fifo_read+0x2c4> @ imm = #0x1f8
     b4c: e35e00e0     	cmp	lr, #224
     b50: 0a00009c     	beq	0xdc8 <midi_fifo_read+0x344> @ imm = #0x270
     b54: e35e00b0     	cmp	lr, #176
     b58: 0a00007a     	beq	0xd48 <midi_fifo_read+0x2c4> @ imm = #0x1e8
     b5c: e5ddc033     	ldrb	r12, [sp, #0x33]
     b60: e1a0000a     	mov	r0, r10
     b64: e5dd3032     	ldrb	r3, [sp, #0x32]
     b68: e5dd2031     	ldrb	r2, [sp, #0x31]
     b6c: e58dc000     	str	r12, [sp]
     b70: e5dd1030     	ldrb	r1, [sp, #0x30]
     b74: ebfffefb     	bl	0x768 <.plt+0xa4>       @ imm = #-0x414  // CALL post
     b78: eaffffd9     	b	0xae4 <midi_fifo_read+0x60> @ imm = #-0x9c
     b7c: e59fe2a4     	ldr	lr, [pc, #0x2a4]        @ 0xe28 <midi_fifo_read+0x3a4>  // u32=0x4c8; f32?=1.71518932e-42
     b80: e08f000e     	add	r0, pc, lr
     b84: ebfffef7     	bl	0x768 <.plt+0xa4>       @ imm = #-0x424  // CALL post
     b88: e28dde43     	add	sp, sp, #1072
     b8c: e28dd004     	add	sp, sp, #4
     b90: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
     b94: ebffff08     	bl	0x7bc <.plt+0xf8>       @ imm = #-0x3e0  // CALL midi_bytes_per_message
     b98: e3500000     	cmp	r0, #0
     b9c: daffffd6     	ble	0xafc <midi_fifo_read+0x78> @ imm = #-0xa8
     ba0: e5554001     	ldrb	r4, [r5, #-0x1]
     ba4: e1a00004     	mov	r0, r4
     ba8: ebffff03     	bl	0x7bc <.plt+0xf8>       @ imm = #-0x3f4  // CALL midi_bytes_per_message
     bac: e1580000     	cmp	r8, r0
     bb0: baffffd1     	blt	0xafc <midi_fifo_read+0x78> @ imm = #-0xbc
     bb4: e20420f0     	and	r2, r4, #240
     bb8: e35200a0     	cmp	r2, #160
     bbc: 0a00003d     	beq	0xcb8 <midi_fifo_read+0x234> @ imm = #0xf4
     bc0: da000038     	ble	0xca8 <midi_fifo_read+0x224> @ imm = #0xe0
     bc4: e35200d0     	cmp	r2, #208
     bc8: 0a00003a     	beq	0xcb8 <midi_fifo_read+0x234> @ imm = #0xe8
     bcc: e35200e0     	cmp	r2, #224
     bd0: 0a000082     	beq	0xde0 <midi_fifo_read+0x35c> @ imm = #0x208
     bd4: e35200b0     	cmp	r2, #176
     bd8: 0a000036     	beq	0xcb8 <midi_fifo_read+0x234> @ imm = #0xd8
     bdc: e5dd1033     	ldrb	r1, [sp, #0x33]
     be0: e1a0000a     	mov	r0, r10
     be4: e5dd3032     	ldrb	r3, [sp, #0x32]
     be8: e5dd2031     	ldrb	r2, [sp, #0x31]
     bec: e58d1000     	str	r1, [sp]
     bf0: e5dd1030     	ldrb	r1, [sp, #0x30]
     bf4: ebfffedb     	bl	0x768 <.plt+0xa4>       @ imm = #-0x494  // CALL post
     bf8: eaffffbf     	b	0xafc <midi_fifo_read+0x78> @ imm = #-0x104
     bfc: ebfffeee     	bl	0x7bc <.plt+0xf8>       @ imm = #-0x448  // CALL midi_bytes_per_message
     c00: e3500000     	cmp	r0, #0
     c04: daffffb6     	ble	0xae4 <midi_fifo_read+0x60> @ imm = #-0x128
     c08: e5545001     	ldrb	r5, [r4, #-0x1]
     c0c: e1a00005     	mov	r0, r5
     c10: ebfffee9     	bl	0x7bc <.plt+0xf8>       @ imm = #-0x45c  // CALL midi_bytes_per_message
     c14: e1580000     	cmp	r8, r0
     c18: baffffb1     	blt	0xae4 <midi_fifo_read+0x60> @ imm = #-0x13c
     c1c: e20530f0     	and	r3, r5, #240
     c20: e35300a0     	cmp	r3, #160
     c24: 0a000006     	beq	0xc44 <midi_fifo_read+0x1c0> @ imm = #0x18
     c28: da000072     	ble	0xdf8 <midi_fifo_read+0x374> @ imm = #0x1c8
     c2c: e35300d0     	cmp	r3, #208
     c30: 0a000003     	beq	0xc44 <midi_fifo_read+0x1c0> @ imm = #0xc
     c34: e35300e0     	cmp	r3, #224
     c38: 0a000073     	beq	0xe0c <midi_fifo_read+0x388> @ imm = #0x1cc
     c3c: e35300b0     	cmp	r3, #176
     c40: 1affffc5     	bne	0xb5c <midi_fifo_read+0xd8> @ imm = #-0xec
     c44: e5d40001     	ldrb	r0, [r4, #0x1]
     c48: e5d4c000     	ldrb	r12, [r4]
     c4c: ee070a90     	vmov	s15, r0
     c50: eeb80ae7     	vcvt.f32.s32	s0, s15
     c54: e205e00f     	and	lr, r5, #15
     c58: ee073a10     	vmov	s14, r3
     c5c: e28e5001     	add	r5, lr, #1
     c60: ee02ca10     	vmov	s4, r12
     c64: e3a01001     	mov	r1, #1
     c68: e28d3010     	add	r3, sp, #16
     c6c: ee015a10     	vmov	s2, r5
     c70: e1a00009     	mov	r0, r9
     c74: ed8d0a0b     	vstr	s0, [sp, #44]
     c78: e58d1010     	str	r1, [sp, #0x10]
     c7c: e58d1018     	str	r1, [sp, #0x18]
     c80: eef80ac7     	vcvt.f32.s32	s1, s14
     c84: e58d1020     	str	r1, [sp, #0x20]
     c88: e58d1028     	str	r1, [sp, #0x28]
     c8c: e597501c     	ldr	r5, [r7, #0x1c]
     c90: eef81ac1     	vcvt.f32.s32	s3, s2
     c94: edcd0a05     	vstr	s1, [sp, #20]
     c98: eef82ac2     	vcvt.f32.s32	s5, s4
     c9c: edcd1a07     	vstr	s3, [sp, #28]
     ca0: edcd2a09     	vstr	s5, [sp, #36]
     ca4: ea00003f     	b	0xda8 <midi_fifo_read+0x324> @ imm = #0xfc
     ca8: e3520080     	cmp	r2, #128
     cac: 0a000001     	beq	0xcb8 <midi_fifo_read+0x234> @ imm = #0x4
     cb0: e3520090     	cmp	r2, #144
     cb4: 1affffc8     	bne	0xbdc <midi_fifo_read+0x158> @ imm = #-0xe0
     cb8: e5d51001     	ldrb	r1, [r5, #0x1]
     cbc: e5d5c000     	ldrb	r12, [r5]
     cc0: ee031a10     	vmov	s6, r1
     cc4: ee06ca90     	vmov	s13, r12
     cc8: eef83ac3     	vcvt.f32.s32	s7, s6
     ccc: e204400f     	and	r4, r4, #15
     cd0: ee052a10     	vmov	s10, r2
     cd4: e2840001     	add	r0, r4, #1
     cd8: e3a0e001     	mov	lr, #1
     cdc: e28d3010     	add	r3, sp, #16
     ce0: e597401c     	ldr	r4, [r7, #0x1c]
     ce4: ee040a90     	vmov	s9, r0
     ce8: e1a00009     	mov	r0, r9
     cec: e58d300c     	str	r3, [sp, #0xc]
     cf0: edcd3a0b     	vstr	s7, [sp, #44]
     cf4: e58de010     	str	lr, [sp, #0x10]
     cf8: e58de018     	str	lr, [sp, #0x18]
     cfc: eeb84ae6     	vcvt.f32.s32	s8, s13
     d00: e58de020     	str	lr, [sp, #0x20]
     d04: e58de028     	str	lr, [sp, #0x28]
     d08: eef85ac5     	vcvt.f32.s32	s11, s10
     d0c: ed8d4a09     	vstr	s8, [sp, #36]
     d10: eeb86ae4     	vcvt.f32.s32	s12, s9
     d14: edcd5a05     	vstr	s11, [sp, #20]
     d18: ed8d6a07     	vstr	s12, [sp, #28]
     d1c: ebfffe6d     	bl	0x6d8 <.plt+0x14>       @ imm = #-0x64c  // CALL gensym
     d20: e59d300c     	ldr	r3, [sp, #0xc]
     d24: e3a02004     	mov	r2, #4
     d28: e1a01000     	mov	r1, r0
     d2c: e1a00004     	mov	r0, r4
     d30: ebfffe95     	bl	0x78c <.plt+0xc8>       @ imm = #-0x5ac  // CALL outlet_list
     d34: eaffff70     	b	0xafc <midi_fifo_read+0x78> @ imm = #-0x240
     d38: e35e0080     	cmp	lr, #128
     d3c: 0a000001     	beq	0xd48 <midi_fifo_read+0x2c4> @ imm = #0x4
     d40: e35e0090     	cmp	lr, #144
     d44: 1affff84     	bne	0xb5c <midi_fifo_read+0xd8> @ imm = #-0x1f0
     d48: e5d41001     	ldrb	r1, [r4, #0x1]
     d4c: e5d52001     	ldrb	r2, [r5, #0x1]
     d50: ee071a90     	vmov	s15, r1
     d54: ee002a10     	vmov	s0, r2
     d58: eef80ae7     	vcvt.f32.s32	s1, s15
     d5c: e20cc00f     	and	r12, r12, #15
     d60: ee01ea90     	vmov	s3, lr
     d64: e28c0001     	add	r0, r12, #1
     d68: edcd0a0b     	vstr	s1, [sp, #44]
     d6c: e3a05001     	mov	r5, #1
     d70: e58d5010     	str	r5, [sp, #0x10]
     d74: e58d5018     	str	r5, [sp, #0x18]
     d78: e28d3010     	add	r3, sp, #16
     d7c: ee010a10     	vmov	s2, r0
     d80: e58d5020     	str	r5, [sp, #0x20]
     d84: e58d5028     	str	r5, [sp, #0x28]
     d88: e1a00009     	mov	r0, r9
     d8c: eeb87ac0     	vcvt.f32.s32	s14, s0
     d90: e597501c     	ldr	r5, [r7, #0x1c]
     d94: eeb82ae1     	vcvt.f32.s32	s4, s3
     d98: ed8d7a09     	vstr	s14, [sp, #36]
     d9c: eef82ac1     	vcvt.f32.s32	s5, s2
     da0: ed8d2a05     	vstr	s4, [sp, #20]
     da4: edcd2a07     	vstr	s5, [sp, #28]
     da8: e58d300c     	str	r3, [sp, #0xc]
     dac: ebfffe49     	bl	0x6d8 <.plt+0x14>       @ imm = #-0x6dc  // CALL gensym
     db0: e59d300c     	ldr	r3, [sp, #0xc]
     db4: e3a02004     	mov	r2, #4
     db8: e1a01000     	mov	r1, r0
     dbc: e1a00005     	mov	r0, r5
     dc0: ebfffe71     	bl	0x78c <.plt+0xc8>       @ imm = #-0x63c  // CALL outlet_list
     dc4: eaffff46     	b	0xae4 <midi_fifo_read+0x60> @ imm = #-0x2e8
     dc8: e5d40001     	ldrb	r0, [r4, #0x1]
     dcc: ee00ba90     	vmov	s1, r11
     dd0: e5d55001     	ldrb	r5, [r5, #0x1]
     dd4: e0853380     	add	r3, r5, r0, lsl #7
     dd8: ee003a10     	vmov	s0, r3
     ddc: eaffffde     	b	0xd5c <midi_fifo_read+0x2d8> @ imm = #-0x88
     de0: e5d50001     	ldrb	r0, [r5, #0x1]
     de4: ee03ba90     	vmov	s7, r11
     de8: e5d53000     	ldrb	r3, [r5]
     dec: e083e380     	add	lr, r3, r0, lsl #7
     df0: ee06ea90     	vmov	s13, lr
     df4: eaffffb4     	b	0xccc <midi_fifo_read+0x248> @ imm = #-0x130
     df8: e3530080     	cmp	r3, #128
     dfc: 0affff90     	beq	0xc44 <midi_fifo_read+0x1c0> @ imm = #-0x1c0
     e00: e3530090     	cmp	r3, #144
     e04: 0affff8e     	beq	0xc44 <midi_fifo_read+0x1c0> @ imm = #-0x1c8
     e08: eaffff53     	b	0xb5c <midi_fifo_read+0xd8> @ imm = #-0x2b4
     e0c: e5d41001     	ldrb	r1, [r4, #0x1]
     e10: ee00ba10     	vmov	s0, r11
     e14: e5d42000     	ldrb	r2, [r4]
     e18: e082c381     	add	r12, r2, r1, lsl #7
     e1c: eaffff8c     	b	0xc54 <midi_fifo_read+0x1d0> @ imm = #-0x1d0
     e20: b0 05 00 00  	.word	0x000005b0
     e24: 8c 05 00 00  	.word	0x0000058c
     e28: c8 04 00 00  	.word	0x000004c8

