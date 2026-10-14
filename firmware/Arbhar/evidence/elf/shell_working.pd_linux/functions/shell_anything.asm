00001904 <shell_anything>:
    1904: e92d47f0     	push	{r4, r5, r6, r7, r8, r9, r10, lr}
    1908: e1a07001     	mov	r7, r1
    190c: e59f6374     	ldr	r6, [pc, #0x374]        @ 0x1c88 <shell_anything+0x384>
    1910: e24dd008     	sub	sp, sp, #8
    1914: e1a04000     	mov	r4, r0
    1918: e5910000     	ldr	r0, [r1]
    191c: e08fa006     	add	r10, pc, r6
    1920: e1a09002     	mov	r9, r2
    1924: e1a08003     	mov	r8, r3
    1928: e59f535c     	ldr	r5, [pc, #0x35c]        @ 0x1c8c <shell_anything+0x388>
    192c: e1a0100a     	mov	r1, r10
    1930: ebfffc7d     	bl	0xb2c <.plt+0x20>       @ imm = #-0xe0c
    1934: e08f5005     	add	r5, pc, r5
    1938: e3500000     	cmp	r0, #0
    193c: 0a00002b     	beq	0x19f0 <shell_anything+0xec> @ imm = #0xac
    1940: e5943030     	ldr	r3, [r4, #0x30]
    1944: e3730001     	cmn	r3, #1
    1948: 0a000007     	beq	0x196c <shell_anything+0x68> @ imm = #0x1c
    194c: e59f033c     	ldr	r0, [pc, #0x33c]        @ 0x1c90 <shell_anything+0x38c>
    1950: e08f0000     	add	r0, pc, r0
    1954: ebfffcc8     	bl	0xc7c <.plt+0x170>      @ imm = #-0xce0
    1958: e3a01009     	mov	r1, #9
    195c: e5940040     	ldr	r0, [r4, #0x40]
    1960: ebfffcb3     	bl	0xc34 <.plt+0x128>      @ imm = #-0xd34
    1964: e1a00004     	mov	r0, r4
    1968: ebfffc87     	bl	0xb8c <.plt+0x80>       @ imm = #-0xde4
    196c: e2840030     	add	r0, r4, #48
    1970: ebfffcca     	bl	0xca0 <.plt+0x194>      @ imm = #-0xcd8
    1974: e3500000     	cmp	r0, #0
    1978: ba000029     	blt	0x1a24 <shell_anything+0x120> @ imm = #0xa4
    197c: e2840038     	add	r0, r4, #56
    1980: ebfffcc6     	bl	0xca0 <.plt+0x194>      @ imm = #-0xce8
    1984: e3500000     	cmp	r0, #0
    1988: ba000020     	blt	0x1a10 <shell_anything+0x10c> @ imm = #0x80
    198c: e59f1300     	ldr	r1, [pc, #0x300]        @ 0x1c94 <shell_anything+0x390>
    1990: e1a02004     	mov	r2, r4
    1994: e5940030     	ldr	r0, [r4, #0x30]
    1998: e7951001     	ldr	r1, [r5, r1]
    199c: ebfffcb3     	bl	0xc70 <.plt+0x164>      @ imm = #-0xd34
    19a0: ebfffcc1     	bl	0xcac <.plt+0x1a0>      @ imm = #-0xcfc
    19a4: e3500000     	cmp	r0, #0
    19a8: e1a06000     	mov	r6, r0
    19ac: e5840040     	str	r0, [r4, #0x40]
    19b0: 0a000020     	beq	0x1a38 <shell_anything+0x134> @ imm = #0x80
    19b4: e3a02004     	mov	r2, #4
    19b8: eeb10b00     	vmov.f64	d0, #4.000000e+00
    19bc: e5842044     	str	r2, [r4, #0x44]
    19c0: e594004c     	ldr	r0, [r4, #0x4c]
    19c4: ebfffc7f     	bl	0xbc8 <.plt+0xbc>       @ imm = #-0xe04
    19c8: e594c01c     	ldr	r12, [r4, #0x1c]
    19cc: e35c0000     	cmp	r12, #0
    19d0: 0a000004     	beq	0x19e8 <shell_anything+0xe4> @ imm = #0x10
    19d4: e1a03008     	mov	r3, r8
    19d8: e1a02009     	mov	r2, r9
    19dc: e1a01007     	mov	r1, r7
    19e0: e594000c     	ldr	r0, [r4, #0xc]
    19e4: ebfffcc2     	bl	0xcf4 <.plt+0x1e8>      @ imm = #-0xcf8
    19e8: e28dd008     	add	sp, sp, #8
    19ec: e8bd87f0     	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
    19f0: e1a0000a     	mov	r0, r10
    19f4: ebfffca0     	bl	0xc7c <.plt+0x170>      @ imm = #-0xd80
    19f8: e1a02008     	mov	r2, r8
    19fc: e1a01009     	mov	r1, r9
    1a00: e2840038     	add	r0, r4, #56
    1a04: ebfffeef     	bl	0x15c8 <shell_send.isra.0> @ imm = #-0x444
    1a08: e28dd008     	add	sp, sp, #8
    1a0c: e8bd87f0     	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
    1a10: e59f5280     	ldr	r5, [pc, #0x280]        @ 0x1c98 <shell_anything+0x394>
    1a14: e08f0005     	add	r0, pc, r5
    1a18: ebfffc76     	bl	0xbf8 <.plt+0xec>       @ imm = #-0xe28
    1a1c: e28dd008     	add	sp, sp, #8
    1a20: e8bd87f0     	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
    1a24: e59fe270     	ldr	lr, [pc, #0x270]        @ 0x1c9c <shell_anything+0x398>
    1a28: e08f000e     	add	r0, pc, lr
    1a2c: ebfffc71     	bl	0xbf8 <.plt+0xec>       @ imm = #-0xe3c
    1a30: e28dd008     	add	sp, sp, #8
    1a34: e8bd87f0     	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
    1a38: e3a01001     	mov	r1, #1
    1a3c: e5940034     	ldr	r0, [r4, #0x34]
    1a40: ebfffc5a     	bl	0xbb0 <.plt+0xa4>       @ imm = #-0xe98
    1a44: e1a01006     	mov	r1, r6
    1a48: e594003c     	ldr	r0, [r4, #0x3c]
    1a4c: ebfffc57     	bl	0xbb0 <.plt+0xa4>       @ imm = #-0xea4
    1a50: e28d2008     	add	r2, sp, #8
    1a54: e1a01006     	mov	r1, r6
    1a58: e1a00006     	mov	r0, r6
    1a5c: e5226004     	str	r6, [r2, #-0x4]!
    1a60: ebfffcb2     	bl	0xd30 <.plt+0x224>      @ imm = #-0xd38
    1a64: ebfffc3c     	bl	0xb5c <.plt+0x50>       @ imm = #-0xf10
    1a68: ebfffc53     	bl	0xbbc <.plt+0xb0>       @ imm = #-0xeb4
    1a6c: e3a00b01     	mov	r0, #1024
    1a70: ebfffc42     	bl	0xb80 <.plt+0x74>       @ imm = #-0xef8
    1a74: e1a04000     	mov	r4, r0
    1a78: e3a00b01     	mov	r0, #1024
    1a7c: ebfffc3f     	bl	0xb80 <.plt+0x74>       @ imm = #-0xf04
    1a80: e5971000     	ldr	r1, [r7]
    1a84: e1a05000     	mov	r5, r0
    1a88: e1a00004     	mov	r0, r4
    1a8c: ebfffc53     	bl	0xbe0 <.plt+0xd4>       @ imm = #-0xeb4
    1a90: e3590000     	cmp	r9, #0
    1a94: da000072     	ble	0x1c64 <shell_anything+0x360> @ imm = #0x1c8
    1a98: e249e001     	sub	lr, r9, #1
    1a9c: e3a020ff     	mov	r2, #255
    1aa0: e1a01005     	mov	r1, r5
    1aa4: e1a00008     	mov	r0, r8
    1aa8: e20ea003     	and	r10, lr, #3
    1aac: ebfffca2     	bl	0xd3c <.plt+0x230>      @ imm = #-0xd78
    1ab0: e1a00004     	mov	r0, r4
    1ab4: e3a07020     	mov	r7, #32
    1ab8: ebfffc69     	bl	0xc64 <.plt+0x158>      @ imm = #-0xe5c
    1abc: e1a01005     	mov	r1, r5
    1ac0: e2888008     	add	r8, r8, #8
    1ac4: e3a06002     	mov	r6, #2
    1ac8: e1a03000     	mov	r3, r0
    1acc: e2800001     	add	r0, r0, #1
    1ad0: e0840000     	add	r0, r4, r0
    1ad4: e7c47003     	strb	r7, [r4, r3]
    1ad8: ebfffc40     	bl	0xbe0 <.plt+0xd4>       @ imm = #-0xf00
    1adc: e3590002     	cmp	r9, #2
    1ae0: ba00005f     	blt	0x1c64 <shell_anything+0x360> @ imm = #0x17c
    1ae4: e35a0000     	cmp	r10, #0
    1ae8: 0a00002c     	beq	0x1ba0 <shell_anything+0x29c> @ imm = #0xb0
    1aec: e35a0001     	cmp	r10, #1
    1af0: 0a00001b     	beq	0x1b64 <shell_anything+0x260> @ imm = #0x6c
    1af4: e35a0002     	cmp	r10, #2
    1af8: 0a00000c     	beq	0x1b30 <shell_anything+0x22c> @ imm = #0x30
    1afc: e3a020ff     	mov	r2, #255
    1b00: e1a01005     	mov	r1, r5
    1b04: e1a00008     	mov	r0, r8
    1b08: e2888008     	add	r8, r8, #8
    1b0c: ebfffc8a     	bl	0xd3c <.plt+0x230>      @ imm = #-0xdd8
    1b10: e1a00004     	mov	r0, r4
    1b14: ebfffc52     	bl	0xc64 <.plt+0x158>      @ imm = #-0xeb8
    1b18: e3a06003     	mov	r6, #3
    1b1c: e2801001     	add	r1, r0, #1
    1b20: e7c47000     	strb	r7, [r4, r0]
    1b24: e0840001     	add	r0, r4, r1
    1b28: e1a01005     	mov	r1, r5
    1b2c: ebfffc2b     	bl	0xbe0 <.plt+0xd4>       @ imm = #-0xf54
    1b30: e3a020ff     	mov	r2, #255
    1b34: e1a01005     	mov	r1, r5
    1b38: e1a00008     	mov	r0, r8
    1b3c: e2866001     	add	r6, r6, #1
    1b40: ebfffc7d     	bl	0xd3c <.plt+0x230>      @ imm = #-0xe0c
    1b44: e1a00004     	mov	r0, r4
    1b48: ebfffc45     	bl	0xc64 <.plt+0x158>      @ imm = #-0xeec
    1b4c: e1a01005     	mov	r1, r5
    1b50: e2888008     	add	r8, r8, #8
    1b54: e2802001     	add	r2, r0, #1
    1b58: e7c47000     	strb	r7, [r4, r0]
    1b5c: e0840002     	add	r0, r4, r2
    1b60: ebfffc1e     	bl	0xbe0 <.plt+0xd4>       @ imm = #-0xf88
    1b64: e3a020ff     	mov	r2, #255
    1b68: e1a01005     	mov	r1, r5
    1b6c: e1a00008     	mov	r0, r8
    1b70: e2866001     	add	r6, r6, #1
    1b74: ebfffc70     	bl	0xd3c <.plt+0x230>      @ imm = #-0xe40
    1b78: e1a00004     	mov	r0, r4
    1b7c: ebfffc38     	bl	0xc64 <.plt+0x158>      @ imm = #-0xf20
    1b80: e1a01005     	mov	r1, r5
    1b84: e2888008     	add	r8, r8, #8
    1b88: e280c001     	add	r12, r0, #1
    1b8c: e7c47000     	strb	r7, [r4, r0]
    1b90: e084000c     	add	r0, r4, r12
    1b94: ebfffc11     	bl	0xbe0 <.plt+0xd4>       @ imm = #-0xfbc
    1b98: e1590006     	cmp	r9, r6
    1b9c: ba000030     	blt	0x1c64 <shell_anything+0x360> @ imm = #0xc0
    1ba0: e3a020ff     	mov	r2, #255
    1ba4: e1a01005     	mov	r1, r5
    1ba8: e1a00008     	mov	r0, r8
    1bac: e288a008     	add	r10, r8, #8
    1bb0: ebfffc61     	bl	0xd3c <.plt+0x230>      @ imm = #-0xe7c
    1bb4: e1a00004     	mov	r0, r4
    1bb8: ebfffc29     	bl	0xc64 <.plt+0x158>      @ imm = #-0xf5c
    1bbc: e1a01005     	mov	r1, r5
    1bc0: e2866004     	add	r6, r6, #4
    1bc4: e2803001     	add	r3, r0, #1
    1bc8: e7c47000     	strb	r7, [r4, r0]
    1bcc: e0840003     	add	r0, r4, r3
    1bd0: ebfffc02     	bl	0xbe0 <.plt+0xd4>       @ imm = #-0xff8
    1bd4: e3a020ff     	mov	r2, #255
    1bd8: e1a01005     	mov	r1, r5
    1bdc: e1a0000a     	mov	r0, r10
    1be0: ebfffc55     	bl	0xd3c <.plt+0x230>      @ imm = #-0xeac
    1be4: e1a00004     	mov	r0, r4
    1be8: ebfffc1d     	bl	0xc64 <.plt+0x158>      @ imm = #-0xf8c
    1bec: e2801001     	add	r1, r0, #1
    1bf0: e7c47000     	strb	r7, [r4, r0]
    1bf4: e0840001     	add	r0, r4, r1
    1bf8: e1a01005     	mov	r1, r5
    1bfc: ebfffbf7     	bl	0xbe0 <.plt+0xd4>       @ imm = #-0x1024
    1c00: e2880010     	add	r0, r8, #16
    1c04: e3a020ff     	mov	r2, #255
    1c08: e1a01005     	mov	r1, r5
    1c0c: ebfffc4a     	bl	0xd3c <.plt+0x230>      @ imm = #-0xed8
    1c10: e1a00004     	mov	r0, r4
    1c14: ebfffc12     	bl	0xc64 <.plt+0x158>      @ imm = #-0xfb8
    1c18: e1a01005     	mov	r1, r5
    1c1c: e2802001     	add	r2, r0, #1
    1c20: e7c47000     	strb	r7, [r4, r0]
    1c24: e0840002     	add	r0, r4, r2
    1c28: ebfffbec     	bl	0xbe0 <.plt+0xd4>       @ imm = #-0x1050
    1c2c: e3a020ff     	mov	r2, #255
    1c30: e1a01005     	mov	r1, r5
    1c34: e2880018     	add	r0, r8, #24
    1c38: ebfffc3f     	bl	0xd3c <.plt+0x230>      @ imm = #-0xf04
    1c3c: e1a00004     	mov	r0, r4
    1c40: ebfffc07     	bl	0xc64 <.plt+0x158>      @ imm = #-0xfe4
    1c44: e1a01005     	mov	r1, r5
    1c48: e2888020     	add	r8, r8, #32
    1c4c: e280c001     	add	r12, r0, #1
    1c50: e7c47000     	strb	r7, [r4, r0]
    1c54: e084000c     	add	r0, r4, r12
    1c58: ebfffbe0     	bl	0xbe0 <.plt+0xd4>       @ imm = #-0x1080
    1c5c: e1590006     	cmp	r9, r6
    1c60: aaffffce     	bge	0x1ba0 <shell_anything+0x29c> @ imm = #-0xc8
    1c64: e59f9034     	ldr	r9, [pc, #0x34]         @ 0x1ca0 <shell_anything+0x39c>
    1c68: e1a02004     	mov	r2, r4
    1c6c: e3a00004     	mov	r0, #4
    1c70: e08f1009     	add	r1, pc, r9
    1c74: ebfffc1b     	bl	0xce8 <.plt+0x1dc>      @ imm = #-0xf94
    1c78: e1a00004     	mov	r0, r4
    1c7c: ebfffbe3     	bl	0xc10 <.plt+0x104>      @ imm = #-0x1074
    1c80: e3a00000     	mov	r0, #0
    1c84: ebfffbed     	bl	0xc40 <.plt+0x134>      @ imm = #-0x104c
    1c88: 84 04 00 00  	.word	0x00000484
    1c8c: c4 06 01 00  	.word	0x000106c4
    1c90: 58 04 00 00  	.word	0x00000458
    1c94: dc 00 00 00  	.word	0x000000dc
    1c98: d0 03 00 00  	.word	0x000003d0
    1c9c: a4 03 00 00  	.word	0x000003a4
    1ca0: 90 01 00 00  	.word	0x00000190

