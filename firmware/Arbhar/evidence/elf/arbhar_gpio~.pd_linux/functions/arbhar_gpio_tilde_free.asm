000049a4 <arbhar_gpio_tilde_free>:
    49a4: e92d40f0     	push	{r4, r5, r6, r7, lr}
    49a8: e2805a01     	add	r5, r0, #4096
    49ac: e24dd05c     	sub	sp, sp, #92
    49b0: e1a04000     	mov	r4, r0
    49b4: ebfffc09     	bl	0x39e0 <.plt+0x2e4>     @ imm = #-0xfdc
    49b8: ebfffbea     	bl	0x3968 <.plt+0x26c>     @ imm = #-0x1058
    49bc: e5950dc0     	ldr	r0, [r5, #0xdc0]
    49c0: e3500000     	cmp	r0, #0
    49c4: 0a000002     	beq	0x49d4 <arbhar_gpio_tilde_free+0x30> @ imm = #0x8
    49c8: ebfffc79     	bl	0x3bb4 <.plt+0x4b8>     @ imm = #-0xe1c
    49cc: e3700001     	cmn	r0, #1
    49d0: 0a000060     	beq	0x4b58 <arbhar_gpio_tilde_free+0x1b4> @ imm = #0x180
    49d4: e5956dbc     	ldr	r6, [r5, #0xdbc]
    49d8: e3a03000     	mov	r3, #0
    49dc: e5853dc0     	str	r3, [r5, #0xdc0]
    49e0: e1560003     	cmp	r6, r3
    49e4: ca000047     	bgt	0x4b08 <arbhar_gpio_tilde_free+0x164> @ imm = #0x11c
    49e8: e5940118     	ldr	r0, [r4, #0x118]
    49ec: ebfffb89     	bl	0x3818 <.plt+0x11c>     @ imm = #-0x11dc
    49f0: e5950dac     	ldr	r0, [r5, #0xdac]
    49f4: ebfffb87     	bl	0x3818 <.plt+0x11c>     @ imm = #-0x11e4
    49f8: e594006c     	ldr	r0, [r4, #0x6c]
    49fc: ebfffb85     	bl	0x3818 <.plt+0x11c>     @ imm = #-0x11ec
    4a00: e5940070     	ldr	r0, [r4, #0x70]
    4a04: ebfffb83     	bl	0x3818 <.plt+0x11c>     @ imm = #-0x11f4
    4a08: e5950db0     	ldr	r0, [r5, #0xdb0]
    4a0c: ebfffb81     	bl	0x3818 <.plt+0x11c>     @ imm = #-0x11fc
    4a10: e5950db4     	ldr	r0, [r5, #0xdb4]
    4a14: ebfffb7f     	bl	0x3818 <.plt+0x11c>     @ imm = #-0x1204
    4a18: e5950db8     	ldr	r0, [r5, #0xdb8]
    4a1c: ebfffb7d     	bl	0x3818 <.plt+0x11c>     @ imm = #-0x120c
    4a20: e59400c8     	ldr	r0, [r4, #0xc8]
    4a24: e3500000     	cmp	r0, #0
    4a28: 0a000000     	beq	0x4a30 <arbhar_gpio_tilde_free+0x8c> @ imm = #0x0
    4a2c: ebfffcd5     	bl	0x3d88 <.plt+0x68c>     @ imm = #-0xcac
    4a30: e59400cc     	ldr	r0, [r4, #0xcc]
    4a34: e3500000     	cmp	r0, #0
    4a38: 0a000000     	beq	0x4a40 <arbhar_gpio_tilde_free+0x9c> @ imm = #0x0
    4a3c: ebfffcd1     	bl	0x3d88 <.plt+0x68c>     @ imm = #-0xcbc
    4a40: e59400d0     	ldr	r0, [r4, #0xd0]
    4a44: e3500000     	cmp	r0, #0
    4a48: 0a000000     	beq	0x4a50 <arbhar_gpio_tilde_free+0xac> @ imm = #0x0
    4a4c: ebfffccd     	bl	0x3d88 <.plt+0x68c>     @ imm = #-0xccc
    4a50: e59400d4     	ldr	r0, [r4, #0xd4]
    4a54: e3500000     	cmp	r0, #0
    4a58: 0a000000     	beq	0x4a60 <arbhar_gpio_tilde_free+0xbc> @ imm = #0x0
    4a5c: ebfffcc9     	bl	0x3d88 <.plt+0x68c>     @ imm = #-0xcdc
    4a60: e59400d8     	ldr	r0, [r4, #0xd8]
    4a64: e3500000     	cmp	r0, #0
    4a68: 0a000000     	beq	0x4a70 <arbhar_gpio_tilde_free+0xcc> @ imm = #0x0
    4a6c: ebfffcc5     	bl	0x3d88 <.plt+0x68c>     @ imm = #-0xcec
    4a70: e59400dc     	ldr	r0, [r4, #0xdc]
    4a74: e3500000     	cmp	r0, #0
    4a78: 0a000000     	beq	0x4a80 <arbhar_gpio_tilde_free+0xdc> @ imm = #0x0
    4a7c: ebfffcc1     	bl	0x3d88 <.plt+0x68c>     @ imm = #-0xcfc
    4a80: e59400e0     	ldr	r0, [r4, #0xe0]
    4a84: e3500000     	cmp	r0, #0
    4a88: 0a000000     	beq	0x4a90 <arbhar_gpio_tilde_free+0xec> @ imm = #0x0
    4a8c: ebfffcbd     	bl	0x3d88 <.plt+0x68c>     @ imm = #-0xd0c
    4a90: e59400e4     	ldr	r0, [r4, #0xe4]
    4a94: e3500000     	cmp	r0, #0
    4a98: 0a000000     	beq	0x4aa0 <arbhar_gpio_tilde_free+0xfc> @ imm = #0x0
    4a9c: ebfffcb9     	bl	0x3d88 <.plt+0x68c>     @ imm = #-0xd1c
    4aa0: e59400e8     	ldr	r0, [r4, #0xe8]
    4aa4: e3500000     	cmp	r0, #0
    4aa8: 0a000000     	beq	0x4ab0 <arbhar_gpio_tilde_free+0x10c> @ imm = #0x0
    4aac: ebfffcb5     	bl	0x3d88 <.plt+0x68c>     @ imm = #-0xd2c
    4ab0: e59400f0     	ldr	r0, [r4, #0xf0]
    4ab4: e3500000     	cmp	r0, #0
    4ab8: 0a000000     	beq	0x4ac0 <arbhar_gpio_tilde_free+0x11c> @ imm = #0x0
    4abc: ebfffcb1     	bl	0x3d88 <.plt+0x68c>     @ imm = #-0xd3c
    4ac0: e59400f4     	ldr	r0, [r4, #0xf4]
    4ac4: e3500000     	cmp	r0, #0
    4ac8: 0a000000     	beq	0x4ad0 <arbhar_gpio_tilde_free+0x12c> @ imm = #0x0
    4acc: ebfffcad     	bl	0x3d88 <.plt+0x68c>     @ imm = #-0xd4c
    4ad0: e59400f8     	ldr	r0, [r4, #0xf8]
    4ad4: e3500000     	cmp	r0, #0
    4ad8: 0a000000     	beq	0x4ae0 <arbhar_gpio_tilde_free+0x13c> @ imm = #0x0
    4adc: ebfffca9     	bl	0x3d88 <.plt+0x68c>     @ imm = #-0xd5c
    4ae0: e59400fc     	ldr	r0, [r4, #0xfc]
    4ae4: e3500000     	cmp	r0, #0
    4ae8: 0a000000     	beq	0x4af0 <arbhar_gpio_tilde_free+0x14c> @ imm = #0x0
    4aec: ebfffca5     	bl	0x3d88 <.plt+0x68c>     @ imm = #-0xd6c
    4af0: e5940100     	ldr	r0, [r4, #0x100]
    4af4: e3500000     	cmp	r0, #0
    4af8: 0a000000     	beq	0x4b00 <arbhar_gpio_tilde_free+0x15c> @ imm = #0x0
    4afc: ebfffca1     	bl	0x3d88 <.plt+0x68c>     @ imm = #-0xd7c
    4b00: e28dd05c     	add	sp, sp, #92
    4b04: e8bd80f0     	pop	{r4, r5, r6, r7, pc}
    4b08: e28d7004     	add	r7, sp, #4
    4b0c: e3a01002     	mov	r1, #2
    4b10: e1a00006     	mov	r0, r6
    4b14: e1a02007     	mov	r2, r7
    4b18: ebfffb9e     	bl	0x3998 <.plt+0x29c>     @ imm = #-0x1188
    4b1c: e3700001     	cmn	r0, #1
    4b20: 0affffb0     	beq	0x49e8 <arbhar_gpio_tilde_free+0x44> @ imm = #-0x140
    4b24: e59d104c     	ldr	r1, [sp, #0x4c]
    4b28: e3510000     	cmp	r1, #0
    4b2c: 1affffad     	bne	0x49e8 <arbhar_gpio_tilde_free+0x44> @ imm = #-0x14c
    4b30: e1a02007     	mov	r2, r7
    4b34: e1a00006     	mov	r0, r6
    4b38: ebfffb96     	bl	0x3998 <.plt+0x29c>     @ imm = #-0x11a8
    4b3c: e3700001     	cmn	r0, #1
    4b40: 1affffa8     	bne	0x49e8 <arbhar_gpio_tilde_free+0x44> @ imm = #-0x160
    4b44: e59f2020     	ldr	r2, [pc, #0x20]         @ 0x4b6c <arbhar_gpio_tilde_free+0x1c8>
    4b48: e1a01006     	mov	r1, r6
    4b4c: e08f0002     	add	r0, pc, r2
    4b50: ebfffb99     	bl	0x39bc <.plt+0x2c0>     @ imm = #-0x119c
    4b54: eaffffa3     	b	0x49e8 <arbhar_gpio_tilde_free+0x44> @ imm = #-0x174
    4b58: e59f0010     	ldr	r0, [pc, #0x10]         @ 0x4b70 <arbhar_gpio_tilde_free+0x1cc>
    4b5c: e5951dc0     	ldr	r1, [r5, #0xdc0]
    4b60: e08f0000     	add	r0, pc, r0
    4b64: ebfffb94     	bl	0x39bc <.plt+0x2c0>     @ imm = #-0x11b0
    4b68: eaffff99     	b	0x49d4 <arbhar_gpio_tilde_free+0x30> @ imm = #-0x19c
    4b6c: 2c 00 01 00  	.word	0x0001002c
    4b70: 04 00 01 00  	.word	0x00010004

