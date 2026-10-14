000009a8 <terminal_parse>:
     9a8: e92d47f0     	push	{r4, r5, r6, r7, r8, r9, r10, lr}
     9ac: e2805020     	add	r5, r0, #32
     9b0: e1a06001     	mov	r6, r1
     9b4: e1a07002     	mov	r7, r2
     9b8: e3a01000     	mov	r1, #0
     9bc: e3a02b01     	mov	r2, #1024
     9c0: e1a08000     	mov	r8, r0
     9c4: e1a00005     	mov	r0, r5
     9c8: e1a04003     	mov	r4, r3
     9cc: ebffff21     	bl	0x658 <.plt+0x98>       @ imm = #-0x37c  // CALL memset
     9d0: e5961000     	ldr	r1, [r6]
     9d4: e1a00005     	mov	r0, r5
     9d8: ebffff0f     	bl	0x61c <.plt+0x5c>       @ imm = #-0x3c4  // CALL strcpy
     9dc: e3570000     	cmp	r7, #0
     9e0: da000023     	ble	0xa74 <terminal_parse+0xcc> @ imm = #0x8c
     9e4: e2844004     	add	r4, r4, #4
     9e8: e59fa3a8     	ldr	r10, [pc, #0x3a8]       @ 0xd98 <terminal_parse+0x3f0>  // u32=0x460; f32?=1.56945428e-42
     9ec: e0847187     	add	r7, r4, r7, lsl #3
     9f0: e59f93a4     	ldr	r9, [pc, #0x3a4]        @ 0xd9c <terminal_parse+0x3f4>  // u32=0x458; f32?=1.55824389e-42
     9f4: e0473004     	sub	r3, r7, r4
     9f8: e08fa00a     	add	r10, pc, r10
     9fc: e2430008     	sub	r0, r3, #8
     a00: e08f9009     	add	r9, pc, r9
     a04: e1a011a0     	lsr	r1, r0, #3
     a08: e2812001     	add	r2, r1, #1
     a0c: e212c003     	ands	r12, r2, #3
     a10: 0a00002f     	beq	0xad4 <terminal_parse+0x12c> @ imm = #0xbc
     a14: e35c0001     	cmp	r12, #1
     a18: 0a00000d     	beq	0xa54 <terminal_parse+0xac> @ imm = #0x34
     a1c: e35c0002     	cmp	r12, #2
     a20: 0a000005     	beq	0xa3c <terminal_parse+0x94> @ imm = #0x14
     a24: e514e004     	ldr	lr, [r4, #-0x4]
     a28: e35e0002     	cmp	lr, #2
     a2c: 0a0000c2     	beq	0xd3c <terminal_parse+0x394> @ imm = #0x308
     a30: e35e0001     	cmp	lr, #1
     a34: 0a0000cb     	beq	0xd68 <terminal_parse+0x3c0> @ imm = #0x32c
     a38: e2844008     	add	r4, r4, #8
     a3c: e514c004     	ldr	r12, [r4, #-0x4]
     a40: e35c0002     	cmp	r12, #2
     a44: 0a0000a5     	beq	0xce0 <terminal_parse+0x338> @ imm = #0x294
     a48: e35c0001     	cmp	r12, #1
     a4c: 0a0000ae     	beq	0xd0c <terminal_parse+0x364> @ imm = #0x2b8
     a50: e2844008     	add	r4, r4, #8
     a54: e514e004     	ldr	lr, [r4, #-0x4]
     a58: e35e0002     	cmp	lr, #2
     a5c: 0a000091     	beq	0xca8 <terminal_parse+0x300> @ imm = #0x244
     a60: e35e0001     	cmp	lr, #1
     a64: 0a000080     	beq	0xc6c <terminal_parse+0x2c4> @ imm = #0x200
     a68: e2844008     	add	r4, r4, #8
     a6c: e1540007     	cmp	r4, r7
     a70: 1a000017     	bne	0xad4 <terminal_parse+0x12c> @ imm = #0x5c
     a74: e1a00008     	mov	r0, r8
     a78: e8bd47f0     	pop	{r4, r5, r6, r7, r8, r9, r10, lr}
     a7c: eaffff04     	b	0x694 <.plt+0xd4>       @ imm = #-0x3f0  // CALL terminal_bang
     a80: e3500001     	cmp	r0, #1
     a84: 0a000054     	beq	0xbdc <terminal_parse+0x234> @ imm = #0x150
     a88: e2846008     	add	r6, r4, #8
     a8c: e516e004     	ldr	lr, [r6, #-0x4]
     a90: e35e0002     	cmp	lr, #2
     a94: 0a00001f     	beq	0xb18 <terminal_parse+0x170> @ imm = #0x7c
     a98: e35e0001     	cmp	lr, #1
     a9c: 0a00005a     	beq	0xc0c <terminal_parse+0x264> @ imm = #0x168
     aa0: e5962004     	ldr	r2, [r6, #0x4]
     aa4: e3520002     	cmp	r2, #2
     aa8: 0a000027     	beq	0xb4c <terminal_parse+0x1a4> @ imm = #0x9c
     aac: e3520001     	cmp	r2, #1
     ab0: 0a00003d     	beq	0xbac <terminal_parse+0x204> @ imm = #0xf4
     ab4: e596e00c     	ldr	lr, [r6, #0xc]
     ab8: e35e0002     	cmp	lr, #2
     abc: 0a00002f     	beq	0xb80 <terminal_parse+0x1d8> @ imm = #0xbc
     ac0: e35e0001     	cmp	lr, #1
     ac4: 0a00005c     	beq	0xc3c <terminal_parse+0x294> @ imm = #0x170
     ac8: e2864018     	add	r4, r6, #24
     acc: e1540007     	cmp	r4, r7
     ad0: 0affffe7     	beq	0xa74 <terminal_parse+0xcc> @ imm = #-0x64
     ad4: e5140004     	ldr	r0, [r4, #-0x4]
     ad8: e3500002     	cmp	r0, #2
     adc: 1affffe7     	bne	0xa80 <terminal_parse+0xd8> @ imm = #-0x64
     ae0: e1a00005     	mov	r0, r5
     ae4: ebfffed2     	bl	0x634 <.plt+0x74>       @ imm = #-0x4b8  // CALL strlen
     ae8: e1d910b0     	ldrh	r1, [r9]
     aec: e1a06000     	mov	r6, r0
     af0: e2803001     	add	r3, r0, #1
     af4: e18510b6     	strh	r1, [r5, r6]
     af8: e2846008     	add	r6, r4, #8
     afc: e5942000     	ldr	r2, [r4]
     b00: e0850003     	add	r0, r5, r3
     b04: e5921000     	ldr	r1, [r2]
     b08: ebfffec3     	bl	0x61c <.plt+0x5c>       @ imm = #-0x4f4  // CALL strcpy
     b0c: e516e004     	ldr	lr, [r6, #-0x4]
     b10: e35e0002     	cmp	lr, #2
     b14: 1affffdf     	bne	0xa98 <terminal_parse+0xf0> @ imm = #-0x84
     b18: e1a00005     	mov	r0, r5
     b1c: ebfffec4     	bl	0x634 <.plt+0x74>       @ imm = #-0x4f0  // CALL strlen
     b20: e1d910b0     	ldrh	r1, [r9]
     b24: e1a0c000     	mov	r12, r0
     b28: e2800001     	add	r0, r0, #1
     b2c: e18510bc     	strh	r1, [r5, r12]
     b30: e0850000     	add	r0, r5, r0
     b34: e5943008     	ldr	r3, [r4, #0x8]
     b38: e5931000     	ldr	r1, [r3]
     b3c: ebfffeb6     	bl	0x61c <.plt+0x5c>       @ imm = #-0x528  // CALL strcpy
     b40: e5962004     	ldr	r2, [r6, #0x4]
     b44: e3520002     	cmp	r2, #2
     b48: 1affffd7     	bne	0xaac <terminal_parse+0x104> @ imm = #-0xa4
     b4c: e1a00005     	mov	r0, r5
     b50: ebfffeb7     	bl	0x634 <.plt+0x74>       @ imm = #-0x524  // CALL strlen
     b54: e1d910b0     	ldrh	r1, [r9]
     b58: e1a0c000     	mov	r12, r0
     b5c: e2800001     	add	r0, r0, #1
     b60: e18510bc     	strh	r1, [r5, r12]
     b64: e0850000     	add	r0, r5, r0
     b68: e5963008     	ldr	r3, [r6, #0x8]
     b6c: e5931000     	ldr	r1, [r3]
     b70: ebfffea9     	bl	0x61c <.plt+0x5c>       @ imm = #-0x55c  // CALL strcpy
     b74: e596e00c     	ldr	lr, [r6, #0xc]
     b78: e35e0002     	cmp	lr, #2
     b7c: 1affffcf     	bne	0xac0 <terminal_parse+0x118> @ imm = #-0xc4
     b80: e1a00005     	mov	r0, r5
     b84: ebfffeaa     	bl	0x634 <.plt+0x74>       @ imm = #-0x558  // CALL strlen
     b88: e1d9c0b0     	ldrh	r12, [r9]
     b8c: e1a04000     	mov	r4, r0
     b90: e2801001     	add	r1, r0, #1
     b94: e185c0b4     	strh	r12, [r5, r4]
     b98: e0850001     	add	r0, r5, r1
     b9c: e5963010     	ldr	r3, [r6, #0x10]
     ba0: e5931000     	ldr	r1, [r3]
     ba4: ebfffe9c     	bl	0x61c <.plt+0x5c>       @ imm = #-0x590  // CALL strcpy
     ba8: eaffffc6     	b	0xac8 <terminal_parse+0x120> @ imm = #-0xe8
     bac: e1a00005     	mov	r0, r5
     bb0: ebfffe9f     	bl	0x634 <.plt+0x74>       @ imm = #-0x584  // CALL strlen
     bb4: e1da40b0     	ldrh	r4, [r10]
     bb8: e18540b0     	strh	r4, [r5, r0]
     bbc: edd64a02     	vldr	s9, [r6, #8]
     bc0: eebd5ae4     	vcvt.s32.f32	s10, s9
     bc4: ee150a10     	vmov	r0, s10
     bc8: ebfffe9f     	bl	0x64c <.plt+0x8c>       @ imm = #-0x584  // CALL terminal_itoa
     bcc: e1a01000     	mov	r1, r0
     bd0: e1a00005     	mov	r0, r5
     bd4: ebfffe8d     	bl	0x610 <.plt+0x50>       @ imm = #-0x5cc  // CALL strcat
     bd8: eaffffb5     	b	0xab4 <terminal_parse+0x10c> @ imm = #-0x12c
     bdc: e1a00005     	mov	r0, r5
     be0: ebfffe93     	bl	0x634 <.plt+0x74>       @ imm = #-0x5b4  // CALL strlen
     be4: e1dac0b0     	ldrh	r12, [r10]
     be8: e185c0b0     	strh	r12, [r5, r0]
     bec: edd42a00     	vldr	s5, [r4]
     bf0: eebd3ae2     	vcvt.s32.f32	s6, s5
     bf4: ee130a10     	vmov	r0, s6
     bf8: ebfffe93     	bl	0x64c <.plt+0x8c>       @ imm = #-0x5b4  // CALL terminal_itoa
     bfc: e1a01000     	mov	r1, r0
     c00: e1a00005     	mov	r0, r5
     c04: ebfffe81     	bl	0x610 <.plt+0x50>       @ imm = #-0x5fc  // CALL strcat
     c08: eaffff9e     	b	0xa88 <terminal_parse+0xe0> @ imm = #-0x188
     c0c: e1a00005     	mov	r0, r5
     c10: ebfffe87     	bl	0x634 <.plt+0x74>       @ imm = #-0x5e4  // CALL strlen
     c14: e1da40b0     	ldrh	r4, [r10]
     c18: e18540b0     	strh	r4, [r5, r0]
     c1c: edd63a00     	vldr	s7, [r6]
     c20: eebd4ae3     	vcvt.s32.f32	s8, s7
     c24: ee140a10     	vmov	r0, s8
     c28: ebfffe87     	bl	0x64c <.plt+0x8c>       @ imm = #-0x5e4  // CALL terminal_itoa
     c2c: e1a01000     	mov	r1, r0
     c30: e1a00005     	mov	r0, r5
     c34: ebfffe75     	bl	0x610 <.plt+0x50>       @ imm = #-0x62c  // CALL strcat
     c38: eaffff98     	b	0xaa0 <terminal_parse+0xf8> @ imm = #-0x1a0
     c3c: e1a00005     	mov	r0, r5
     c40: ebfffe7b     	bl	0x634 <.plt+0x74>       @ imm = #-0x614  // CALL strlen
     c44: e1da20b0     	ldrh	r2, [r10]
     c48: e18520b0     	strh	r2, [r5, r0]
     c4c: edd65a04     	vldr	s11, [r6, #16]
     c50: eebd6ae5     	vcvt.s32.f32	s12, s11
     c54: ee160a10     	vmov	r0, s12
     c58: ebfffe7b     	bl	0x64c <.plt+0x8c>       @ imm = #-0x614  // CALL terminal_itoa
     c5c: e1a01000     	mov	r1, r0
     c60: e1a00005     	mov	r0, r5
     c64: ebfffe69     	bl	0x610 <.plt+0x50>       @ imm = #-0x65c  // CALL strcat
     c68: eaffff96     	b	0xac8 <terminal_parse+0x120> @ imm = #-0x1a8
     c6c: e1a00005     	mov	r0, r5
     c70: e2844008     	add	r4, r4, #8
     c74: ebfffe6e     	bl	0x634 <.plt+0x74>       @ imm = #-0x648  // CALL strlen
     c78: e1dac0b0     	ldrh	r12, [r10]
     c7c: e185c0b0     	strh	r12, [r5, r0]
     c80: ed541a02     	vldr	s3, [r4, #-8]
     c84: eebd2ae1     	vcvt.s32.f32	s4, s3
     c88: ee120a10     	vmov	r0, s4
     c8c: ebfffe6e     	bl	0x64c <.plt+0x8c>       @ imm = #-0x648  // CALL terminal_itoa
     c90: e1a01000     	mov	r1, r0
     c94: e1a00005     	mov	r0, r5
     c98: ebfffe5c     	bl	0x610 <.plt+0x50>       @ imm = #-0x690  // CALL strcat
     c9c: e1540007     	cmp	r4, r7
     ca0: 1affff8b     	bne	0xad4 <terminal_parse+0x12c> @ imm = #-0x1d4
     ca4: eaffff72     	b	0xa74 <terminal_parse+0xcc> @ imm = #-0x238
     ca8: e1a00005     	mov	r0, r5
     cac: e2844008     	add	r4, r4, #8
     cb0: ebfffe5f     	bl	0x634 <.plt+0x74>       @ imm = #-0x684  // CALL strlen
     cb4: e1d910b0     	ldrh	r1, [r9]
     cb8: e1a06000     	mov	r6, r0
     cbc: e2803001     	add	r3, r0, #1
     cc0: e18510b6     	strh	r1, [r5, r6]
     cc4: e0850003     	add	r0, r5, r3
     cc8: e5142008     	ldr	r2, [r4, #-0x8]
     ccc: e5921000     	ldr	r1, [r2]
     cd0: ebfffe51     	bl	0x61c <.plt+0x5c>       @ imm = #-0x6bc  // CALL strcpy
     cd4: e1540007     	cmp	r4, r7
     cd8: 1affff7d     	bne	0xad4 <terminal_parse+0x12c> @ imm = #-0x20c
     cdc: eaffff64     	b	0xa74 <terminal_parse+0xcc> @ imm = #-0x270
     ce0: e1a00005     	mov	r0, r5
     ce4: ebfffe52     	bl	0x634 <.plt+0x74>       @ imm = #-0x6b8  // CALL strlen
     ce8: e1d930b0     	ldrh	r3, [r9]
     cec: e1a01000     	mov	r1, r0
     cf0: e2800001     	add	r0, r0, #1
     cf4: e18530b1     	strh	r3, [r5, r1]
     cf8: e0850000     	add	r0, r5, r0
     cfc: e5942000     	ldr	r2, [r4]
     d00: e5921000     	ldr	r1, [r2]
     d04: ebfffe44     	bl	0x61c <.plt+0x5c>       @ imm = #-0x6f0  // CALL strcpy
     d08: eaffff50     	b	0xa50 <terminal_parse+0xa8> @ imm = #-0x2c0
     d0c: e1a00005     	mov	r0, r5
     d10: ebfffe47     	bl	0x634 <.plt+0x74>       @ imm = #-0x6e4  // CALL strlen
     d14: e1da60b0     	ldrh	r6, [r10]
     d18: e18560b0     	strh	r6, [r5, r0]
     d1c: edd40a00     	vldr	s1, [r4]
     d20: eebd1ae0     	vcvt.s32.f32	s2, s1
     d24: ee110a10     	vmov	r0, s2
     d28: ebfffe47     	bl	0x64c <.plt+0x8c>       @ imm = #-0x6e4  // CALL terminal_itoa
     d2c: e1a01000     	mov	r1, r0
     d30: e1a00005     	mov	r0, r5
     d34: ebfffe35     	bl	0x610 <.plt+0x50>       @ imm = #-0x72c  // CALL strcat
     d38: eaffff44     	b	0xa50 <terminal_parse+0xa8> @ imm = #-0x2f0
     d3c: e1a00005     	mov	r0, r5
     d40: ebfffe3b     	bl	0x634 <.plt+0x74>       @ imm = #-0x714  // CALL strlen
     d44: e1d930b0     	ldrh	r3, [r9]
     d48: e1a01000     	mov	r1, r0
     d4c: e2800001     	add	r0, r0, #1
     d50: e18530b1     	strh	r3, [r5, r1]
     d54: e0850000     	add	r0, r5, r0
     d58: e5942000     	ldr	r2, [r4]
     d5c: e5921000     	ldr	r1, [r2]
     d60: ebfffe2d     	bl	0x61c <.plt+0x5c>       @ imm = #-0x74c  // CALL strcpy
     d64: eaffff33     	b	0xa38 <terminal_parse+0x90> @ imm = #-0x334
     d68: e1a00005     	mov	r0, r5
     d6c: ebfffe30     	bl	0x634 <.plt+0x74>       @ imm = #-0x740  // CALL strlen
     d70: e1da60b0     	ldrh	r6, [r10]
     d74: e18560b0     	strh	r6, [r5, r0]
     d78: edd47a00     	vldr	s15, [r4]
     d7c: eebd0ae7     	vcvt.s32.f32	s0, s15
     d80: ee100a10     	vmov	r0, s0
     d84: ebfffe30     	bl	0x64c <.plt+0x8c>       @ imm = #-0x740  // CALL terminal_itoa
     d88: e1a01000     	mov	r1, r0
     d8c: e1a00005     	mov	r0, r5
     d90: ebfffe1e     	bl	0x610 <.plt+0x50>       @ imm = #-0x788  // CALL strcat
     d94: eaffff27     	b	0xa38 <terminal_parse+0x90> @ imm = #-0x364
     d98: 60 04 00 00  	.word	0x00000460
     d9c: 58 04 00 00  	.word	0x00000458

