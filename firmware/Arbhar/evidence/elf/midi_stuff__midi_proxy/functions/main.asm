00010a78 <main>:
   10a78: e92d4800     	push	{r11, lr}
   10a7c: e28db004     	add	r11, sp, #4
   10a80: e24ddc85     	sub	sp, sp, #34048
   10a84: e24dd0f8     	sub	sp, sp, #248
   10a88: e24b306c     	sub	r3, r11, #108
   10a8c: e3a02028     	mov	r2, #40
   10a90: e3a01000     	mov	r1, #0
   10a94: e1a00003     	mov	r0, r3
   10a98: ebffff08     	bl	0x106c0 <.plt+0xbc>     @ imm = #-0x3e0
   10a9c: e3a03000     	mov	r3, #0
   10aa0: e50b3070     	str	r3, [r11, #-0x70]
   10aa4: e3a03000     	mov	r3, #0
   10aa8: e50b3008     	str	r3, [r11, #-0x8]
   10aac: e3a01000     	mov	r1, #0
   10ab0: e59f0708     	ldr	r0, [pc, #0x708]        @ 0x111c0 <main+0x748>
   10ab4: ebffff07     	bl	0x106d8 <.plt+0xd4>     @ imm = #-0x3e4
   10ab8: e1a03000     	mov	r3, r0
   10abc: e3730001     	cmn	r3, #1
   10ac0: 1a00000c     	bne	0x10af8 <main+0x80>     @ imm = #0x30
   10ac4: e59f16f4     	ldr	r1, [pc, #0x6f4]        @ 0x111c0 <main+0x748>
   10ac8: e59f06f4     	ldr	r0, [pc, #0x6f4]        @ 0x111c4 <main+0x74c>
   10acc: ebfffed1     	bl	0x10618 <.plt+0x14>     @ imm = #-0x4bc
   10ad0: e59f16f0     	ldr	r1, [pc, #0x6f0]        @ 0x111c8 <main+0x750>
   10ad4: e59f06e4     	ldr	r0, [pc, #0x6e4]        @ 0x111c0 <main+0x748>
   10ad8: ebffff0d     	bl	0x10714 <.plt+0x110>    @ imm = #-0x3cc
   10adc: e1a03000     	mov	r3, r0
   10ae0: e3530000     	cmp	r3, #0
   10ae4: aa000003     	bge	0x10af8 <main+0x80>     @ imm = #0xc
   10ae8: e59f06dc     	ldr	r0, [pc, #0x6dc]        @ 0x111cc <main+0x754>
   10aec: ebfffed2     	bl	0x1063c <.plt+0x38>     @ imm = #-0x4b8
   10af0: e3a03001     	mov	r3, #1
   10af4: ea0001ae     	b	0x111b4 <main+0x73c>    @ imm = #0x6b8
   10af8: e3a01001     	mov	r1, #1
   10afc: e59f06bc     	ldr	r0, [pc, #0x6bc]        @ 0x111c0 <main+0x748>
   10b00: ebfffedc     	bl	0x10678 <.plt+0x74>     @ imm = #-0x490
   10b04: e50b0024     	str	r0, [r11, #-0x24]
   10b08: e51b3024     	ldr	r3, [r11, #-0x24]
   10b0c: e3530000     	cmp	r3, #0
   10b10: aa000003     	bge	0x10b24 <main+0xac>     @ imm = #0xc
   10b14: e59f06b4     	ldr	r0, [pc, #0x6b4]        @ 0x111d0 <main+0x758>
   10b18: ebfffec7     	bl	0x1063c <.plt+0x38>     @ imm = #-0x4e4
   10b1c: e3a03001     	mov	r3, #1
   10b20: ea0001a3     	b	0x111b4 <main+0x73c>    @ imm = #0x68c
   10b24: e24b306c     	sub	r3, r11, #108
   10b28: e3a0100a     	mov	r1, #10
   10b2c: e1a00003     	mov	r0, r3
   10b30: ebffff4b     	bl	0x10864 <scan_and_open_midi_devices> @ imm = #-0x2d4
   10b34: e1a03000     	mov	r3, r0
   10b38: e50b3070     	str	r3, [r11, #-0x70]
   10b3c: e51b3070     	ldr	r3, [r11, #-0x70]
   10b40: e3530000     	cmp	r3, #0
   10b44: 1a000001     	bne	0x10b50 <main+0xd8>     @ imm = #0x4
   10b48: e59f0684     	ldr	r0, [pc, #0x684]        @ 0x111d4 <main+0x75c>
   10b4c: ebfffec0     	bl	0x10654 <.plt+0x50>     @ imm = #-0x500
   10b50: ebfffecb     	bl	0x10684 <.plt+0x80>     @ imm = #-0x4d4
   10b54: e50b0028     	str	r0, [r11, #-0x28]
   10b58: e51b3028     	ldr	r3, [r11, #-0x28]
   10b5c: e3530000     	cmp	r3, #0
   10b60: aa000003     	bge	0x10b74 <main+0xfc>     @ imm = #0xc
   10b64: e59f066c     	ldr	r0, [pc, #0x66c]        @ 0x111d8 <main+0x760>
   10b68: ebfffeb3     	bl	0x1063c <.plt+0x38>     @ imm = #-0x534
   10b6c: e3a03001     	mov	r3, #1
   10b70: ea00018f     	b	0x111b4 <main+0x73c>    @ imm = #0x63c
   10b74: e3a02c03     	mov	r2, #768
   10b78: e59f165c     	ldr	r1, [pc, #0x65c]        @ 0x111dc <main+0x764>
   10b7c: e51b0028     	ldr	r0, [r11, #-0x28]
   10b80: ebfffec2     	bl	0x10690 <.plt+0x8c>     @ imm = #-0x4f8
   10b84: e50b002c     	str	r0, [r11, #-0x2c]
   10b88: e51b302c     	ldr	r3, [r11, #-0x2c]
   10b8c: e3530000     	cmp	r3, #0
   10b90: aa000003     	bge	0x10ba4 <main+0x12c>    @ imm = #0xc
   10b94: e59f0644     	ldr	r0, [pc, #0x644]        @ 0x111e0 <main+0x768>
   10b98: ebfffea7     	bl	0x1063c <.plt+0x38>     @ imm = #-0x564
   10b9c: e3a03001     	mov	r3, #1
   10ba0: ea000183     	b	0x111b4 <main+0x73c>    @ imm = #0x60c
   10ba4: e24b3e4e     	sub	r3, r11, #1248
   10ba8: e2433004     	sub	r3, r3, #4
   10bac: e243300c     	sub	r3, r3, #12
   10bb0: e50b3030     	str	r3, [r11, #-0x30]
   10bb4: e3a03000     	mov	r3, #0
   10bb8: e50b300c     	str	r3, [r11, #-0xc]
   10bbc: ea000006     	b	0x10bdc <main+0x164>    @ imm = #0x18
   10bc0: e51b3030     	ldr	r3, [r11, #-0x30]
   10bc4: e51b200c     	ldr	r2, [r11, #-0xc]
   10bc8: e3a01000     	mov	r1, #0
   10bcc: e7831102     	str	r1, [r3, r2, lsl #2]
   10bd0: e51b300c     	ldr	r3, [r11, #-0xc]
   10bd4: e2833001     	add	r3, r3, #1
   10bd8: e50b300c     	str	r3, [r11, #-0xc]
   10bdc: e51b300c     	ldr	r3, [r11, #-0xc]
   10be0: e353001f     	cmp	r3, #31
   10be4: 9afffff5     	bls	0x10bc0 <main+0x148>    @ imm = #-0x2c
   10be8: e51b2028     	ldr	r2, [r11, #-0x28]
   10bec: e51b3024     	ldr	r3, [r11, #-0x24]
   10bf0: e1520003     	cmp	r2, r3
   10bf4: a1a03002     	movge	r3, r2
   10bf8: b1a03003     	movlt	r3, r3
   10bfc: e50b3008     	str	r3, [r11, #-0x8]
   10c00: e3a03000     	mov	r3, #0
   10c04: e50b3010     	str	r3, [r11, #-0x10]
   10c08: ea00002f     	b	0x10ccc <main+0x254>    @ imm = #0xbc
   10c0c: e51b3010     	ldr	r3, [r11, #-0x10]
   10c10: e1a03103     	lsl	r3, r3, #2
   10c14: e24b2004     	sub	r2, r11, #4
   10c18: e0823003     	add	r3, r2, r3
   10c1c: e5133068     	ldr	r3, [r3, #-0x68]
   10c20: e283201f     	add	r2, r3, #31
   10c24: e3530000     	cmp	r3, #0
   10c28: b1a03002     	movlt	r3, r2
   10c2c: a1a03003     	movge	r3, r3
   10c30: e1a032c3     	asr	r3, r3, #5
   10c34: e1a02003     	mov	r2, r3
   10c38: e1a03102     	lsl	r3, r2, #2
   10c3c: e24b1004     	sub	r1, r11, #4
   10c40: e0813003     	add	r3, r1, r3
   10c44: e51314ec     	ldr	r1, [r3, #-0x4ec]
   10c48: e51b3010     	ldr	r3, [r11, #-0x10]
   10c4c: e1a03103     	lsl	r3, r3, #2
   10c50: e24b0004     	sub	r0, r11, #4
   10c54: e0803003     	add	r3, r0, r3
   10c58: e5133068     	ldr	r3, [r3, #-0x68]
   10c5c: e2730000     	rsbs	r0, r3, #0
   10c60: e203301f     	and	r3, r3, #31
   10c64: e200001f     	and	r0, r0, #31
   10c68: 52603000     	rsbpl	r3, r0, #0
   10c6c: e3a00001     	mov	r0, #1
   10c70: e1a03310     	lsl	r3, r0, r3
   10c74: e1811003     	orr	r1, r1, r3
   10c78: e1a03102     	lsl	r3, r2, #2
   10c7c: e24b2004     	sub	r2, r11, #4
   10c80: e0823003     	add	r3, r2, r3
   10c84: e50314ec     	str	r1, [r3, #-0x4ec]
   10c88: e51b3010     	ldr	r3, [r11, #-0x10]
   10c8c: e1a03103     	lsl	r3, r3, #2
   10c90: e24b2004     	sub	r2, r11, #4
   10c94: e0823003     	add	r3, r2, r3
   10c98: e5133068     	ldr	r3, [r3, #-0x68]
   10c9c: e51b2008     	ldr	r2, [r11, #-0x8]
   10ca0: e1520003     	cmp	r2, r3
   10ca4: aa000005     	bge	0x10cc0 <main+0x248>    @ imm = #0x14
   10ca8: e51b3010     	ldr	r3, [r11, #-0x10]
   10cac: e1a03103     	lsl	r3, r3, #2
   10cb0: e24b2004     	sub	r2, r11, #4
   10cb4: e0823003     	add	r3, r2, r3
   10cb8: e5133068     	ldr	r3, [r3, #-0x68]
   10cbc: e50b3008     	str	r3, [r11, #-0x8]
   10cc0: e51b3010     	ldr	r3, [r11, #-0x10]
   10cc4: e2833001     	add	r3, r3, #1
   10cc8: e50b3010     	str	r3, [r11, #-0x10]
   10ccc: e51b3070     	ldr	r3, [r11, #-0x70]
   10cd0: e51b2010     	ldr	r2, [r11, #-0x10]
   10cd4: e1520003     	cmp	r2, r3
   10cd8: baffffcb     	blt	0x10c0c <main+0x194>    @ imm = #-0xd4
   10cdc: e51b3028     	ldr	r3, [r11, #-0x28]
   10ce0: e283201f     	add	r2, r3, #31
   10ce4: e3530000     	cmp	r3, #0
   10ce8: b1a03002     	movlt	r3, r2
   10cec: a1a03003     	movge	r3, r3
   10cf0: e1a032c3     	asr	r3, r3, #5
   10cf4: e1a02003     	mov	r2, r3
   10cf8: e1a03102     	lsl	r3, r2, #2
   10cfc: e24b1004     	sub	r1, r11, #4
   10d00: e0813003     	add	r3, r1, r3
   10d04: e51314ec     	ldr	r1, [r3, #-0x4ec]
   10d08: e51b3028     	ldr	r3, [r11, #-0x28]
   10d0c: e2730000     	rsbs	r0, r3, #0
   10d10: e203301f     	and	r3, r3, #31
   10d14: e200001f     	and	r0, r0, #31
   10d18: 52603000     	rsbpl	r3, r0, #0
   10d1c: e3a00001     	mov	r0, #1
   10d20: e1a03310     	lsl	r3, r0, r3
   10d24: e1811003     	orr	r1, r1, r3
   10d28: e1a03102     	lsl	r3, r2, #2
   10d2c: e24b2004     	sub	r2, r11, #4
   10d30: e0823003     	add	r3, r2, r3
   10d34: e50314ec     	str	r1, [r3, #-0x4ec]
   10d38: e51b3008     	ldr	r3, [r11, #-0x8]
   10d3c: e2830001     	add	r0, r3, #1
   10d40: e24b1e4e     	sub	r1, r11, #1248
   10d44: e2411004     	sub	r1, r1, #4
   10d48: e241100c     	sub	r1, r1, #12
   10d4c: e3a03000     	mov	r3, #0
   10d50: e58d3000     	str	r3, [sp]
   10d54: e3a03000     	mov	r3, #0
   10d58: e3a02000     	mov	r2, #0
   10d5c: ebfffe33     	bl	0x10630 <.plt+0x2c>     @ imm = #-0x734
   10d60: e50b0034     	str	r0, [r11, #-0x34]
   10d64: e51b3034     	ldr	r3, [r11, #-0x34]
   10d68: e3530000     	cmp	r3, #0
   10d6c: aa000009     	bge	0x10d98 <main+0x320>    @ imm = #0x24
   10d70: e59f046c     	ldr	r0, [pc, #0x46c]        @ 0x111e4 <main+0x76c>
   10d74: ebfffe30     	bl	0x1063c <.plt+0x38>     @ imm = #-0x740
   10d78: e1a00000     	mov	r0, r0
   10d7c: e51b0024     	ldr	r0, [r11, #-0x24]
   10d80: ebfffe60     	bl	0x10708 <.plt+0x104>    @ imm = #-0x680
   10d84: e51b0028     	ldr	r0, [r11, #-0x28]
   10d88: ebfffe5e     	bl	0x10708 <.plt+0x104>    @ imm = #-0x688
   10d8c: e3a03000     	mov	r3, #0
   10d90: e50b3020     	str	r3, [r11, #-0x20]
   10d94: ea000101     	b	0x111a0 <main+0x728>    @ imm = #0x404
   10d98: e51b3028     	ldr	r3, [r11, #-0x28]
   10d9c: e283201f     	add	r2, r3, #31
   10da0: e3530000     	cmp	r3, #0
   10da4: b1a03002     	movlt	r3, r2
   10da8: a1a03003     	movge	r3, r3
   10dac: e1a032c3     	asr	r3, r3, #5
   10db0: e1a03103     	lsl	r3, r3, #2
   10db4: e24b2004     	sub	r2, r11, #4
   10db8: e0823003     	add	r3, r2, r3
   10dbc: e51324ec     	ldr	r2, [r3, #-0x4ec]
   10dc0: e51b3028     	ldr	r3, [r11, #-0x28]
   10dc4: e2731000     	rsbs	r1, r3, #0
   10dc8: e203301f     	and	r3, r3, #31
   10dcc: e201101f     	and	r1, r1, #31
   10dd0: 52613000     	rsbpl	r3, r1, #0
   10dd4: e3a01001     	mov	r1, #1
   10dd8: e1a03311     	lsl	r3, r1, r3
   10ddc: e0033002     	and	r3, r3, r2
   10de0: e3530000     	cmp	r3, #0
   10de4: 0a000088     	beq	0x1100c <main+0x594>    @ imm = #0x220
   10de8: e24b3b21     	sub	r3, r11, #33792
   10dec: e2433004     	sub	r3, r3, #4
   10df0: e24330ec     	sub	r3, r3, #236
   10df4: e3a02902     	mov	r2, #32768
   10df8: e1a01003     	mov	r1, r3
   10dfc: e51b0028     	ldr	r0, [r11, #-0x28]
   10e00: ebfffe07     	bl	0x10624 <.plt+0x20>     @ imm = #-0x7e4
   10e04: e50b0038     	str	r0, [r11, #-0x38]
   10e08: e51b3038     	ldr	r3, [r11, #-0x38]
   10e0c: e3530000     	cmp	r3, #0
   10e10: aa000002     	bge	0x10e20 <main+0x3a8>    @ imm = #0x8
   10e14: e59f03cc     	ldr	r0, [pc, #0x3cc]        @ 0x111e8 <main+0x770>
   10e18: ebfffe07     	bl	0x1063c <.plt+0x38>     @ imm = #-0x7e4
   10e1c: ea0000d4     	b	0x11174 <main+0x6fc>    @ imm = #0x350
   10e20: e3a03000     	mov	r3, #0
   10e24: e50b3014     	str	r3, [r11, #-0x14]
   10e28: ea000073     	b	0x10ffc <main+0x584>    @ imm = #0x1cc
   10e2c: e24b3b21     	sub	r3, r11, #33792
   10e30: e2433004     	sub	r3, r3, #4
   10e34: e24330ec     	sub	r3, r3, #236
   10e38: e51b2014     	ldr	r2, [r11, #-0x14]
   10e3c: e0833002     	add	r3, r3, r2
   10e40: e50b303c     	str	r3, [r11, #-0x3c]
   10e44: e51b303c     	ldr	r3, [r11, #-0x3c]
   10e48: e5933004     	ldr	r3, [r3, #0x4]
   10e4c: e2033c01     	and	r3, r3, #256
   10e50: e3530000     	cmp	r3, #0
   10e54: 0a00002b     	beq	0x10f08 <main+0x490>    @ imm = #0xac
   10e58: e51b303c     	ldr	r3, [r11, #-0x3c]
   10e5c: e2833010     	add	r3, r3, #16
   10e60: e3a02005     	mov	r2, #5
   10e64: e59f1380     	ldr	r1, [pc, #0x380]        @ 0x111ec <main+0x774>
   10e68: e1a00003     	mov	r0, r3
   10e6c: ebfffe1f     	bl	0x106f0 <.plt+0xec>     @ imm = #-0x784
   10e70: e1a03000     	mov	r3, r0
   10e74: e3530000     	cmp	r3, #0
   10e78: 1a000022     	bne	0x10f08 <main+0x490>    @ imm = #0x88
   10e7c: e51b303c     	ldr	r3, [r11, #-0x3c]
   10e80: e2833010     	add	r3, r3, #16
   10e84: e1a01003     	mov	r1, r3
   10e88: e59f0360     	ldr	r0, [pc, #0x360]        @ 0x111f0 <main+0x778>
   10e8c: ebfffde1     	bl	0x10618 <.plt+0x14>     @ imm = #-0x87c
   10e90: e51b303c     	ldr	r3, [r11, #-0x3c]
   10e94: e2833010     	add	r3, r3, #16
   10e98: e24b0c85     	sub	r0, r11, #34048
   10e9c: e2400004     	sub	r0, r0, #4
   10ea0: e24000ec     	sub	r0, r0, #236
   10ea4: e58d3000     	str	r3, [sp]
   10ea8: e59f332c     	ldr	r3, [pc, #0x32c]        @ 0x111dc <main+0x764>
   10eac: e59f2340     	ldr	r2, [pc, #0x340]        @ 0x111f4 <main+0x77c>
   10eb0: e3a01c01     	mov	r1, #256
   10eb4: ebfffdfb     	bl	0x106a8 <.plt+0xa4>     @ imm = #-0x814
   10eb8: e51b3070     	ldr	r3, [r11, #-0x70]
   10ebc: e3530009     	cmp	r3, #9
   10ec0: ca000010     	bgt	0x10f08 <main+0x490>    @ imm = #0x40
   10ec4: e24b3c85     	sub	r3, r11, #34048
   10ec8: e2433004     	sub	r3, r3, #4
   10ecc: e24330ec     	sub	r3, r3, #236
   10ed0: e1a00003     	mov	r0, r3
   10ed4: ebfffe50     	bl	0x1081c <open_midi_device> @ imm = #-0x6c0
   10ed8: e50b0040     	str	r0, [r11, #-0x40]
   10edc: e51b3040     	ldr	r3, [r11, #-0x40]
   10ee0: e3530000     	cmp	r3, #0
   10ee4: ba000007     	blt	0x10f08 <main+0x490>    @ imm = #0x1c
   10ee8: e51b3070     	ldr	r3, [r11, #-0x70]
   10eec: e2832001     	add	r2, r3, #1
   10ef0: e50b2070     	str	r2, [r11, #-0x70]
   10ef4: e1a03103     	lsl	r3, r3, #2
   10ef8: e24b2004     	sub	r2, r11, #4
   10efc: e0823003     	add	r3, r2, r3
   10f00: e51b2040     	ldr	r2, [r11, #-0x40]
   10f04: e5032068     	str	r2, [r3, #-0x68]
   10f08: e51b303c     	ldr	r3, [r11, #-0x3c]
   10f0c: e5933004     	ldr	r3, [r3, #0x4]
   10f10: e2033c02     	and	r3, r3, #512
   10f14: e3530000     	cmp	r3, #0
   10f18: 0a000031     	beq	0x10fe4 <main+0x56c>    @ imm = #0xc4
   10f1c: e51b303c     	ldr	r3, [r11, #-0x3c]
   10f20: e2833010     	add	r3, r3, #16
   10f24: e3a02005     	mov	r2, #5
   10f28: e59f12bc     	ldr	r1, [pc, #0x2bc]        @ 0x111ec <main+0x774>
   10f2c: e1a00003     	mov	r0, r3
   10f30: ebfffdee     	bl	0x106f0 <.plt+0xec>     @ imm = #-0x848
   10f34: e1a03000     	mov	r3, r0
   10f38: e3530000     	cmp	r3, #0
   10f3c: 1a000028     	bne	0x10fe4 <main+0x56c>    @ imm = #0xa0
   10f40: e51b303c     	ldr	r3, [r11, #-0x3c]
   10f44: e2833010     	add	r3, r3, #16
   10f48: e1a01003     	mov	r1, r3
   10f4c: e59f02a4     	ldr	r0, [pc, #0x2a4]        @ 0x111f8 <main+0x780>
   10f50: ebfffdb0     	bl	0x10618 <.plt+0x14>     @ imm = #-0x940
   10f54: e3a03000     	mov	r3, #0
   10f58: e50b3018     	str	r3, [r11, #-0x18]
   10f5c: ea00001c     	b	0x10fd4 <main+0x55c>    @ imm = #0x70
   10f60: e51b303c     	ldr	r3, [r11, #-0x3c]
   10f64: e2833010     	add	r3, r3, #16
   10f68: e24b0c85     	sub	r0, r11, #34048
   10f6c: e2400004     	sub	r0, r0, #4
   10f70: e24000ec     	sub	r0, r0, #236
   10f74: e58d3000     	str	r3, [sp]
   10f78: e59f325c     	ldr	r3, [pc, #0x25c]        @ 0x111dc <main+0x764>
   10f7c: e59f2270     	ldr	r2, [pc, #0x270]        @ 0x111f4 <main+0x77c>
   10f80: e3a01c01     	mov	r1, #256
   10f84: ebfffdc7     	bl	0x106a8 <.plt+0xa4>     @ imm = #-0x8e4
   10f88: e51b3018     	ldr	r3, [r11, #-0x18]
   10f8c: e1a03103     	lsl	r3, r3, #2
   10f90: e24b2004     	sub	r2, r11, #4
   10f94: e0823003     	add	r3, r2, r3
   10f98: e5133068     	ldr	r3, [r3, #-0x68]
   10f9c: e3a01001     	mov	r1, #1
   10fa0: e1a00003     	mov	r0, r3
   10fa4: ebfffdc2     	bl	0x106b4 <.plt+0xb0>     @ imm = #-0x8f8
   10fa8: e1a03000     	mov	r3, r0
   10fac: e3730001     	cmn	r3, #1
   10fb0: 1a000004     	bne	0x10fc8 <main+0x550>    @ imm = #0x10
   10fb4: e24b2070     	sub	r2, r11, #112
   10fb8: e24b306c     	sub	r3, r11, #108
   10fbc: e51b1018     	ldr	r1, [r11, #-0x18]
   10fc0: e1a00003     	mov	r0, r3
   10fc4: ebfffe76     	bl	0x109a4 <close_midi_device> @ imm = #-0x628
   10fc8: e51b3018     	ldr	r3, [r11, #-0x18]
   10fcc: e2833001     	add	r3, r3, #1
   10fd0: e50b3018     	str	r3, [r11, #-0x18]
   10fd4: e51b3070     	ldr	r3, [r11, #-0x70]
   10fd8: e51b2018     	ldr	r2, [r11, #-0x18]
   10fdc: e1520003     	cmp	r2, r3
   10fe0: baffffde     	blt	0x10f60 <main+0x4e8>    @ imm = #-0x88
   10fe4: e51b303c     	ldr	r3, [r11, #-0x3c]
   10fe8: e593200c     	ldr	r2, [r3, #0xc]
   10fec: e51b3014     	ldr	r3, [r11, #-0x14]
   10ff0: e0823003     	add	r3, r2, r3
   10ff4: e2833010     	add	r3, r3, #16
   10ff8: e50b3014     	str	r3, [r11, #-0x14]
   10ffc: e51b2014     	ldr	r2, [r11, #-0x14]
   11000: e51b3038     	ldr	r3, [r11, #-0x38]
   11004: e1520003     	cmp	r2, r3
   11008: baffff87     	blt	0x10e2c <main+0x3b4>    @ imm = #-0x1e4
   1100c: e3a03000     	mov	r3, #0
   11010: e50b301c     	str	r3, [r11, #-0x1c]
   11014: ea000052     	b	0x11164 <main+0x6ec>    @ imm = #0x148
   11018: e51b301c     	ldr	r3, [r11, #-0x1c]
   1101c: e1a03103     	lsl	r3, r3, #2
   11020: e24b2004     	sub	r2, r11, #4
   11024: e0823003     	add	r3, r2, r3
   11028: e5133068     	ldr	r3, [r3, #-0x68]
   1102c: e283201f     	add	r2, r3, #31
   11030: e3530000     	cmp	r3, #0
   11034: b1a03002     	movlt	r3, r2
   11038: a1a03003     	movge	r3, r3
   1103c: e1a032c3     	asr	r3, r3, #5
   11040: e1a03103     	lsl	r3, r3, #2
   11044: e24b2004     	sub	r2, r11, #4
   11048: e0823003     	add	r3, r2, r3
   1104c: e51324ec     	ldr	r2, [r3, #-0x4ec]
   11050: e51b301c     	ldr	r3, [r11, #-0x1c]
   11054: e1a03103     	lsl	r3, r3, #2
   11058: e24b1004     	sub	r1, r11, #4
   1105c: e0813003     	add	r3, r1, r3
   11060: e5133068     	ldr	r3, [r3, #-0x68]
   11064: e2731000     	rsbs	r1, r3, #0
   11068: e203301f     	and	r3, r3, #31
   1106c: e201101f     	and	r1, r1, #31
   11070: 52613000     	rsbpl	r3, r1, #0
   11074: e3a01001     	mov	r1, #1
   11078: e1a03311     	lsl	r3, r1, r3
   1107c: e0033002     	and	r3, r3, r2
   11080: e3530000     	cmp	r3, #0
   11084: 0a000033     	beq	0x11158 <main+0x6e0>    @ imm = #0xcc
   11088: e51b301c     	ldr	r3, [r11, #-0x1c]
   1108c: e1a03103     	lsl	r3, r3, #2
   11090: e24b2004     	sub	r2, r11, #4
   11094: e0823003     	add	r3, r2, r3
   11098: e5130068     	ldr	r0, [r3, #-0x68]
   1109c: e24b3e46     	sub	r3, r11, #1120
   110a0: e2433004     	sub	r3, r3, #4
   110a4: e243300c     	sub	r3, r3, #12
   110a8: e3a02b01     	mov	r2, #1024
   110ac: e1a01003     	mov	r1, r3
   110b0: ebfffd5b     	bl	0x10624 <.plt+0x20>     @ imm = #-0xa94
   110b4: e50b0044     	str	r0, [r11, #-0x44]
   110b8: e51b3044     	ldr	r3, [r11, #-0x44]
   110bc: e3530000     	cmp	r3, #0
   110c0: da000007     	ble	0x110e4 <main+0x66c>    @ imm = #0x1c
   110c4: e51b2044     	ldr	r2, [r11, #-0x44]
   110c8: e24b3e46     	sub	r3, r11, #1120
   110cc: e2433004     	sub	r3, r3, #4
   110d0: e243300c     	sub	r3, r3, #12
   110d4: e1a01003     	mov	r1, r3
   110d8: e51b0024     	ldr	r0, [r11, #-0x24]
   110dc: ebfffd7a     	bl	0x106cc <.plt+0xc8>     @ imm = #-0xa18
   110e0: ea00001c     	b	0x11158 <main+0x6e0>    @ imm = #0x70
   110e4: e51b3044     	ldr	r3, [r11, #-0x44]
   110e8: e3530000     	cmp	r3, #0
   110ec: aa000019     	bge	0x11158 <main+0x6e0>    @ imm = #0x64
   110f0: ebfffd69     	bl	0x1069c <.plt+0x98>     @ imm = #-0xa5c
   110f4: e1a03000     	mov	r3, r0
   110f8: e5933000     	ldr	r3, [r3]
   110fc: e3530013     	cmp	r3, #19
   11100: 0a000004     	beq	0x11118 <main+0x6a0>    @ imm = #0x10
   11104: ebfffd64     	bl	0x1069c <.plt+0x98>     @ imm = #-0xa70
   11108: e1a03000     	mov	r3, r0
   1110c: e5933000     	ldr	r3, [r3]
   11110: e3530005     	cmp	r3, #5
   11114: 1a00000d     	bne	0x11150 <main+0x6d8>    @ imm = #0x34
   11118: e51b301c     	ldr	r3, [r11, #-0x1c]
   1111c: e1a03103     	lsl	r3, r3, #2
   11120: e24b2004     	sub	r2, r11, #4
   11124: e0823003     	add	r3, r2, r3
   11128: e5133068     	ldr	r3, [r3, #-0x68]
   1112c: e1a01003     	mov	r1, r3
   11130: e59f00c4     	ldr	r0, [pc, #0xc4]         @ 0x111fc <main+0x784>
   11134: ebfffd37     	bl	0x10618 <.plt+0x14>     @ imm = #-0xb24
   11138: e24b2070     	sub	r2, r11, #112
   1113c: e24b306c     	sub	r3, r11, #108
   11140: e51b101c     	ldr	r1, [r11, #-0x1c]
   11144: e1a00003     	mov	r0, r3
   11148: ebfffe15     	bl	0x109a4 <close_midi_device> @ imm = #-0x7ac
   1114c: ea000001     	b	0x11158 <main+0x6e0>    @ imm = #0x4
   11150: e59f00a8     	ldr	r0, [pc, #0xa8]         @ 0x11200 <main+0x788>
   11154: ebfffd38     	bl	0x1063c <.plt+0x38>     @ imm = #-0xb20
   11158: e51b301c     	ldr	r3, [r11, #-0x1c]
   1115c: e2833001     	add	r3, r3, #1
   11160: e50b301c     	str	r3, [r11, #-0x1c]
   11164: e51b3070     	ldr	r3, [r11, #-0x70]
   11168: e51b201c     	ldr	r2, [r11, #-0x1c]
   1116c: e1520003     	cmp	r2, r3
   11170: baffffa8     	blt	0x11018 <main+0x5a0>    @ imm = #-0x160
   11174: eafffe8a     	b	0x10ba4 <main+0x12c>    @ imm = #-0x5d8
   11178: e51b3020     	ldr	r3, [r11, #-0x20]
   1117c: e1a03103     	lsl	r3, r3, #2
   11180: e24b2004     	sub	r2, r11, #4
   11184: e0823003     	add	r3, r2, r3
   11188: e5133068     	ldr	r3, [r3, #-0x68]
   1118c: e1a00003     	mov	r0, r3
   11190: ebfffd5c     	bl	0x10708 <.plt+0x104>    @ imm = #-0xa90
   11194: e51b3020     	ldr	r3, [r11, #-0x20]
   11198: e2833001     	add	r3, r3, #1
   1119c: e50b3020     	str	r3, [r11, #-0x20]
   111a0: e51b3070     	ldr	r3, [r11, #-0x70]
   111a4: e51b2020     	ldr	r2, [r11, #-0x20]
   111a8: e1520003     	cmp	r2, r3
   111ac: bafffff1     	blt	0x11178 <main+0x700>    @ imm = #-0x3c
   111b0: e3a03000     	mov	r3, #0
   111b4: e1a00003     	mov	r0, r3
   111b8: e24bd004     	sub	sp, r11, #4
   111bc: e8bd8800     	pop	{r11, pc}
   111c0: 24 13 01 00  	.word	0x00011324
   111c4: 3c 13 01 00  	.word	0x0001133c
   111c8: b6 01 00 00  	.word	0x000001b6
   111cc: 64 13 01 00  	.word	0x00011364
   111d0: 7c 13 01 00  	.word	0x0001137c
   111d4: 90 13 01 00  	.word	0x00011390
   111d8: b4 13 01 00  	.word	0x000113b4
   111dc: 90 12 01 00  	.word	0x00011290
   111e0: c8 13 01 00  	.word	0x000113c8
   111e4: f4 13 01 00  	.word	0x000113f4
   111e8: 04 14 01 00  	.word	0x00011404
   111ec: c0 12 01 00  	.word	0x000112c0
   111f0: 18 14 01 00  	.word	0x00011418
   111f4: c8 12 01 00  	.word	0x000112c8
   111f8: 38 14 01 00  	.word	0x00011438
   111fc: 54 14 01 00  	.word	0x00011454
   11200: 78 14 01 00  	.word	0x00011478

