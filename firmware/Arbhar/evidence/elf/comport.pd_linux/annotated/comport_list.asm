00001b18 <comport_list>:
    1b18: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
    1b1c: e3520901     	cmp	r2, #16384
    1b20: e24dd901     	sub	sp, sp, #16384
    1b24: e24dd004     	sub	sp, sp, #4
    1b28: e1a09002     	mov	r9, r2
    1b2c: e1a07000     	mov	r7, r0
    1b30: e1a04003     	mov	r4, r3
    1b34: ca0000bf     	bgt	0x1e38 <comport_list+0x320> @ imm = #0x2fc
    1b38: e3520000     	cmp	r2, #0
    1b3c: da0000b4     	ble	0x1e14 <comport_list+0x2fc> @ imm = #0x2d0
    1b40: e1a00004     	mov	r0, r4
    1b44: ebfffbf6     	bl	0xb24 <.plt+0x11c>      @ imm = #-0x1028  // CALL atom_getint
    1b48: e3a06001     	mov	r6, #1
    1b4c: e2498001     	sub	r8, r9, #1
    1b50: e1560009     	cmp	r6, r9
    1b54: e208b007     	and	r11, r8, #7
    1b58: e1a0a00d     	mov	r10, sp
    1b5c: e2844008     	add	r4, r4, #8
    1b60: e1a0500d     	mov	r5, sp
    1b64: e5cd0000     	strb	r0, [sp]
    1b68: aa000050     	bge	0x1cb0 <comport_list+0x198> @ imm = #0x140
    1b6c: e35b0000     	cmp	r11, #0
    1b70: 0a000031     	beq	0x1c3c <comport_list+0x124> @ imm = #0xc4
    1b74: e35b0001     	cmp	r11, #1
    1b78: 0a000028     	beq	0x1c20 <comport_list+0x108> @ imm = #0xa0
    1b7c: e35b0002     	cmp	r11, #2
    1b80: 0a000021     	beq	0x1c0c <comport_list+0xf4> @ imm = #0x84
    1b84: e35b0003     	cmp	r11, #3
    1b88: 0a00001a     	beq	0x1bf8 <comport_list+0xe0> @ imm = #0x68
    1b8c: e35b0004     	cmp	r11, #4
    1b90: 0a000013     	beq	0x1be4 <comport_list+0xcc> @ imm = #0x4c
    1b94: e35b0005     	cmp	r11, #5
    1b98: 0a00000c     	beq	0x1bd0 <comport_list+0xb8> @ imm = #0x30
    1b9c: e35b0006     	cmp	r11, #6
    1ba0: 0a000005     	beq	0x1bbc <comport_list+0xa4> @ imm = #0x14
    1ba4: e1a00004     	mov	r0, r4
    1ba8: ebfffbdd     	bl	0xb24 <.plt+0x11c>      @ imm = #-0x108c  // CALL atom_getint
    1bac: e1a0500d     	mov	r5, sp
    1bb0: e2844008     	add	r4, r4, #8
    1bb4: e3a06002     	mov	r6, #2
    1bb8: e5e50001     	strb	r0, [r5, #0x1]!
    1bbc: e1a00004     	mov	r0, r4
    1bc0: ebfffbd7     	bl	0xb24 <.plt+0x11c>      @ imm = #-0x10a4  // CALL atom_getint
    1bc4: e2866001     	add	r6, r6, #1
    1bc8: e2844008     	add	r4, r4, #8
    1bcc: e5e50001     	strb	r0, [r5, #0x1]!
    1bd0: e1a00004     	mov	r0, r4
    1bd4: ebfffbd2     	bl	0xb24 <.plt+0x11c>      @ imm = #-0x10b8  // CALL atom_getint
    1bd8: e2866001     	add	r6, r6, #1
    1bdc: e2844008     	add	r4, r4, #8
    1be0: e5e50001     	strb	r0, [r5, #0x1]!
    1be4: e1a00004     	mov	r0, r4
    1be8: ebfffbcd     	bl	0xb24 <.plt+0x11c>      @ imm = #-0x10cc  // CALL atom_getint
    1bec: e2866001     	add	r6, r6, #1
    1bf0: e2844008     	add	r4, r4, #8
    1bf4: e5e50001     	strb	r0, [r5, #0x1]!
    1bf8: e1a00004     	mov	r0, r4
    1bfc: ebfffbc8     	bl	0xb24 <.plt+0x11c>      @ imm = #-0x10e0  // CALL atom_getint
    1c00: e2866001     	add	r6, r6, #1
    1c04: e2844008     	add	r4, r4, #8
    1c08: e5e50001     	strb	r0, [r5, #0x1]!
    1c0c: e1a00004     	mov	r0, r4
    1c10: ebfffbc3     	bl	0xb24 <.plt+0x11c>      @ imm = #-0x10f4  // CALL atom_getint
    1c14: e2866001     	add	r6, r6, #1
    1c18: e2844008     	add	r4, r4, #8
    1c1c: e5e50001     	strb	r0, [r5, #0x1]!
    1c20: e1a00004     	mov	r0, r4
    1c24: ebfffbbe     	bl	0xb24 <.plt+0x11c>      @ imm = #-0x1108  // CALL atom_getint
    1c28: e2866001     	add	r6, r6, #1
    1c2c: e1560009     	cmp	r6, r9
    1c30: e2844008     	add	r4, r4, #8
    1c34: e5e50001     	strb	r0, [r5, #0x1]!
    1c38: aa00001c     	bge	0x1cb0 <comport_list+0x198> @ imm = #0x70
    1c3c: e1a00004     	mov	r0, r4
    1c40: ebfffbb7     	bl	0xb24 <.plt+0x11c>      @ imm = #-0x1124  // CALL atom_getint
    1c44: e2841008     	add	r1, r4, #8
    1c48: e2866008     	add	r6, r6, #8
    1c4c: e5c50001     	strb	r0, [r5, #0x1]
    1c50: e1a00001     	mov	r0, r1
    1c54: ebfffbb2     	bl	0xb24 <.plt+0x11c>      @ imm = #-0x1138  // CALL atom_getint
    1c58: e5c50002     	strb	r0, [r5, #0x2]
    1c5c: e2840010     	add	r0, r4, #16
    1c60: ebfffbaf     	bl	0xb24 <.plt+0x11c>      @ imm = #-0x1144  // CALL atom_getint
    1c64: e5c50003     	strb	r0, [r5, #0x3]
    1c68: e2840018     	add	r0, r4, #24
    1c6c: ebfffbac     	bl	0xb24 <.plt+0x11c>      @ imm = #-0x1150  // CALL atom_getint
    1c70: e5c50004     	strb	r0, [r5, #0x4]
    1c74: e2840020     	add	r0, r4, #32
    1c78: ebfffba9     	bl	0xb24 <.plt+0x11c>      @ imm = #-0x115c  // CALL atom_getint
    1c7c: e5c50005     	strb	r0, [r5, #0x5]
    1c80: e2840028     	add	r0, r4, #40
    1c84: ebfffba6     	bl	0xb24 <.plt+0x11c>      @ imm = #-0x1168  // CALL atom_getint
    1c88: e5c50006     	strb	r0, [r5, #0x6]
    1c8c: e2840030     	add	r0, r4, #48
    1c90: ebfffba3     	bl	0xb24 <.plt+0x11c>      @ imm = #-0x1174  // CALL atom_getint
    1c94: e5c50007     	strb	r0, [r5, #0x7]
    1c98: e2840038     	add	r0, r4, #56
    1c9c: ebfffba0     	bl	0xb24 <.plt+0x11c>      @ imm = #-0x1180  // CALL atom_getint
    1ca0: e1560009     	cmp	r6, r9
    1ca4: e2844040     	add	r4, r4, #64
    1ca8: e5e50008     	strb	r0, [r5, #0x8]!
    1cac: baffffe2     	blt	0x1c3c <comport_list+0x124> @ imm = #-0x78
    1cb0: e5972020     	ldr	r2, [r7, #0x20]
    1cb4: e3720001     	cmn	r2, #1
    1cb8: 0a000064     	beq	0x1e50 <comport_list+0x338> @ imm = #0x190
    1cbc: e2873a01     	add	r3, r7, #4096
    1cc0: e593b0fc     	ldr	r11, [r3, #0xfc]
    1cc4: e593c0f8     	ldr	r12, [r3, #0xf8]
    1cc8: e15b000c     	cmp	r11, r12
    1ccc: aa00004a     	bge	0x1dfc <comport_list+0x2e4> @ imm = #0x128
    1cd0: e249e001     	sub	lr, r9, #1
    1cd4: e21e0003     	ands	r0, lr, #3
    1cd8: e3a01000     	mov	r1, #0
    1cdc: 0a000023     	beq	0x1d70 <comport_list+0x258> @ imm = #0x8c
    1ce0: e4dae001     	ldrb	lr, [r10], #1
    1ce4: e59380f0     	ldr	r8, [r3, #0xf0]
    1ce8: e3a01001     	mov	r1, #1
    1cec: e1510009     	cmp	r1, r9
    1cf0: e7c8e00b     	strb	lr, [r8, r11]
    1cf4: e593b0fc     	ldr	r11, [r3, #0xfc]
    1cf8: e08bb001     	add	r11, r11, r1
    1cfc: e583b0fc     	str	r11, [r3, #0xfc]
    1d00: aa000047     	bge	0x1e24 <comport_list+0x30c> @ imm = #0x11c
    1d04: e59350f8     	ldr	r5, [r3, #0xf8]
    1d08: e15b0005     	cmp	r11, r5
    1d0c: aa00003a     	bge	0x1dfc <comport_list+0x2e4> @ imm = #0xe8
    1d10: e1500001     	cmp	r0, r1
    1d14: 0a000015     	beq	0x1d70 <comport_list+0x258> @ imm = #0x54
    1d18: e3500002     	cmp	r0, #2
    1d1c: 0a000009     	beq	0x1d48 <comport_list+0x230> @ imm = #0x24
    1d20: e4da6001     	ldrb	r6, [r10], #1
    1d24: e59340f0     	ldr	r4, [r3, #0xf0]
    1d28: e3a01002     	mov	r1, #2
    1d2c: e7c4600b     	strb	r6, [r4, r11]
    1d30: e59370fc     	ldr	r7, [r3, #0xfc]
    1d34: e593c0f8     	ldr	r12, [r3, #0xf8]
    1d38: e287b001     	add	r11, r7, #1
    1d3c: e15b000c     	cmp	r11, r12
    1d40: e583b0fc     	str	r11, [r3, #0xfc]
    1d44: aa00002c     	bge	0x1dfc <comport_list+0x2e4> @ imm = #0xb0
    1d48: e4da4001     	ldrb	r4, [r10], #1
    1d4c: e59370f0     	ldr	r7, [r3, #0xf0]
    1d50: e2811001     	add	r1, r1, #1
    1d54: e7c7400b     	strb	r4, [r7, r11]
    1d58: e59320fc     	ldr	r2, [r3, #0xfc]
    1d5c: e593e0f8     	ldr	lr, [r3, #0xf8]
    1d60: e282b001     	add	r11, r2, #1
    1d64: e15b000e     	cmp	r11, lr
    1d68: e583b0fc     	str	r11, [r3, #0xfc]
    1d6c: aa000022     	bge	0x1dfc <comport_list+0x2e4> @ imm = #0x88
    1d70: e1a0000a     	mov	r0, r10
    1d74: e59380f0     	ldr	r8, [r3, #0xf0]
    1d78: e4d0e001     	ldrb	lr, [r0], #1
    1d7c: e2811001     	add	r1, r1, #1
    1d80: e1510009     	cmp	r1, r9
    1d84: e7c8e00b     	strb	lr, [r8, r11]
    1d88: e59350fc     	ldr	r5, [r3, #0xfc]
    1d8c: e2856001     	add	r6, r5, #1
    1d90: e58360fc     	str	r6, [r3, #0xfc]
    1d94: aa000022     	bge	0x1e24 <comport_list+0x30c> @ imm = #0x88
    1d98: e59340f8     	ldr	r4, [r3, #0xf8]
    1d9c: e2811003     	add	r1, r1, #3
    1da0: e1560004     	cmp	r6, r4
    1da4: aa000014     	bge	0x1dfc <comport_list+0x2e4> @ imm = #0x50
    1da8: e5d07000     	ldrb	r7, [r0]
    1dac: e593c0f0     	ldr	r12, [r3, #0xf0]
    1db0: e7cc7006     	strb	r7, [r12, r6]
    1db4: e59320fc     	ldr	r2, [r3, #0xfc]
    1db8: e593e0f8     	ldr	lr, [r3, #0xf8]
    1dbc: e282b001     	add	r11, r2, #1
    1dc0: e15b000e     	cmp	r11, lr
    1dc4: e583b0fc     	str	r11, [r3, #0xfc]
    1dc8: aa00000b     	bge	0x1dfc <comport_list+0x2e4> @ imm = #0x2c
    1dcc: e5d08001     	ldrb	r8, [r0, #0x1]
    1dd0: e59300f0     	ldr	r0, [r3, #0xf0]
    1dd4: e7c0800b     	strb	r8, [r0, r11]
    1dd8: e59350fc     	ldr	r5, [r3, #0xfc]
    1ddc: e59360f8     	ldr	r6, [r3, #0xf8]
    1de0: e285b001     	add	r11, r5, #1
    1de4: e15b0006     	cmp	r11, r6
    1de8: e583b0fc     	str	r11, [r3, #0xfc]
    1dec: b5da4003     	ldrblt	r4, [r10, #0x3]
    1df0: b59370f0     	ldrlt	r7, [r3, #0xf0]
    1df4: b28aa004     	addlt	r10, r10, #4
    1df8: baffffd5     	blt	0x1d54 <comport_list+0x23c> @ imm = #-0xac
    1dfc: e59fa064     	ldr	r10, [pc, #0x64]        @ 0x1e68 <comport_list+0x350>  // u32=0x281c; f32?=1.43885326e-41
    1e00: e08f000a     	add	r0, pc, r10
    1e04: e28dd901     	add	sp, sp, #16384
    1e08: e28dd004     	add	sp, sp, #4
    1e0c: e8bd4ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
    1e10: eafffb49     	b	0xb3c <.plt+0x134>      @ imm = #-0x12dc  // CALL post
    1e14: e5903020     	ldr	r3, [r0, #0x20]
    1e18: e3730001     	cmn	r3, #1
    1e1c: 0a00000b     	beq	0x1e50 <comport_list+0x338> @ imm = #0x2c
    1e20: e3a01000     	mov	r1, #0
    1e24: e1510009     	cmp	r1, r9
    1e28: 1afffff3     	bne	0x1dfc <comport_list+0x2e4> @ imm = #-0x34
    1e2c: e28dd901     	add	sp, sp, #16384
    1e30: e28dd004     	add	sp, sp, #4
    1e34: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
    1e38: e59f002c     	ldr	r0, [pc, #0x2c]         @ 0x1e6c <comport_list+0x354>  // u32=0x2788; f32?=1.41811405e-41
    1e3c: e1a01002     	mov	r1, r2
    1e40: e08f0000     	add	r0, pc, r0
    1e44: ebfffb3c     	bl	0xb3c <.plt+0x134>      @ imm = #-0x1310  // CALL post
    1e48: e3a09901     	mov	r9, #16384
    1e4c: eaffff3b     	b	0x1b40 <comport_list+0x28> @ imm = #-0x314
    1e50: e59f9018     	ldr	r9, [pc, #0x18]         @ 0x1e70 <comport_list+0x358>  // u32=0x27a4; f32?=1.42203768e-41
    1e54: e08f0009     	add	r0, pc, r9
    1e58: e28dd901     	add	sp, sp, #16384
    1e5c: e28dd004     	add	sp, sp, #4
    1e60: e8bd4ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
    1e64: eafffb34     	b	0xb3c <.plt+0x134>      @ imm = #-0x1330  // CALL post
    1e68: 1c 28 00 00  	.word	0x0000281c
    1e6c: 88 27 00 00  	.word	0x00002788
    1e70: a4 27 00 00  	.word	0x000027a4

