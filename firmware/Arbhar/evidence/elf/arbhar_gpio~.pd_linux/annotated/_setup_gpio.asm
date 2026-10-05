00004b74 <_setup_gpio>:
    4b74: e92d4070     	push	{r4, r5, r6, lr}
    4b78: e3a03000     	mov	r3, #0
    4b7c: e24dd010     	sub	sp, sp, #16
    4b80: e5c030bb     	strb	r3, [r0, #0xbb]
    4b84: e1a05000     	mov	r5, r0
    4b88: ebfffc42     	bl	0x3c98 <.plt+0x59c>     @ imm = #-0xef8  // CALL bcm2835_init
    4b8c: e3500000     	cmp	r0, #0
    4b90: 0a00007e     	beq	0x4d90 <_setup_gpio+0x21c> @ imm = #0x1f8
    4b94: ebfffb10     	bl	0x37dc <.plt+0xe0>      @ imm = #-0x13c0  // CALL bcm2835_version
    4b98: e59f2200     	ldr	r2, [pc, #0x200]        @ 0x4da0 <_setup_gpio+0x22c>  // u32=0xfff4; f32?=9.18186806e-41
    4b9c: e1a01000     	mov	r1, r0
    4ba0: e08f0002     	add	r0, pc, r2
    4ba4: ebfffbf3     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x1034  // CALL post
    4ba8: e3a00021     	mov	r0, #33
    4bac: ebfffaf5     	bl	0x3788 <.plt+0x8c>      @ imm = #-0x142c  // CALL bcm2835_gpio_lev
    4bb0: e1a06000     	mov	r6, r0
    4bb4: e3a0002a     	mov	r0, #42
    4bb8: ebfffaf2     	bl	0x3788 <.plt+0x8c>      @ imm = #-0x1438  // CALL bcm2835_gpio_lev
    4bbc: e1a04000     	mov	r4, r0
    4bc0: e3a00026     	mov	r0, #38
    4bc4: ebfffaef     	bl	0x3788 <.plt+0x8c>      @ imm = #-0x1444  // CALL bcm2835_gpio_lev
    4bc8: e3560000     	cmp	r6, #0
    4bcc: e16f1f14     	clz	r1, r4
    4bd0: e1a0c2a1     	lsr	r12, r1, #5
    4bd4: 13a0c000     	movne	r12, #0
    4bd8: e3500000     	cmp	r0, #0
    4bdc: 01a0000c     	moveq	r0, r12
    4be0: 13a00000     	movne	r0, #0
    4be4: e3500000     	cmp	r0, #0
    4be8: 1a000054     	bne	0x4d40 <_setup_gpio+0x1cc> @ imm = #0x150
    4bec: e5c50030     	strb	r0, [r5, #0x30]
    4bf0: e2853a01     	add	r3, r5, #4096
    4bf4: e59f01a8     	ldr	r0, [pc, #0x1a8]        @ 0x4da4 <_setup_gpio+0x230>  // u32=0xfeb0; f32?=9.13646599e-41
    4bf8: e3a04001     	mov	r4, #1
    4bfc: e5936db0     	ldr	r6, [r3, #0xdb0]
    4c00: e3a0e000     	mov	lr, #0
    4c04: e08f0000     	add	r0, pc, r0
    4c08: e344e37e     	movt	lr, #0x437e
    4c0c: e58d4000     	str	r4, [sp]
    4c10: e58de004     	str	lr, [sp, #0x4]
    4c14: ebfffac3     	bl	0x3728 <.plt+0x2c>      @ imm = #-0x14f4  // CALL gensym
    4c18: e1a02004     	mov	r2, r4
    4c1c: e1a0300d     	mov	r3, sp
    4c20: e1a01000     	mov	r1, r0
    4c24: e1a00006     	mov	r0, r6
    4c28: ebfffc11     	bl	0x3c74 <.plt+0x578>     @ imm = #-0xfbc  // CALL outlet_list
    4c2c: e5d51030     	ldrb	r1, [r5, #0x30]
    4c30: e59f5170     	ldr	r5, [pc, #0x170]        @ 0x4da8 <_setup_gpio+0x234>  // u32=0xff70; f32?=9.16337092e-41
    4c34: e08f0005     	add	r0, pc, r5
    4c38: ebfffbce     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x10c8  // CALL post
    4c3c: e3a01000     	mov	r1, #0
    4c40: e3a0002a     	mov	r0, #42
    4c44: ebfffc04     	bl	0x3c5c <.plt+0x560>     @ imm = #-0xff0  // CALL bcm2835_gpio_fsel
    4c48: e3a01000     	mov	r1, #0
    4c4c: e3a00020     	mov	r0, #32
    4c50: ebfffc01     	bl	0x3c5c <.plt+0x560>     @ imm = #-0xffc  // CALL bcm2835_gpio_fsel
    4c54: e3a01000     	mov	r1, #0
    4c58: e3a00026     	mov	r0, #38
    4c5c: ebfffbfe     	bl	0x3c5c <.plt+0x560>     @ imm = #-0x1008  // CALL bcm2835_gpio_fsel
    4c60: e3a01000     	mov	r1, #0
    4c64: e3a00024     	mov	r0, #36
    4c68: ebfffbfb     	bl	0x3c5c <.plt+0x560>     @ imm = #-0x1014  // CALL bcm2835_gpio_fsel
    4c6c: e3a01000     	mov	r1, #0
    4c70: e3a00021     	mov	r0, #33
    4c74: ebfffbf8     	bl	0x3c5c <.plt+0x560>     @ imm = #-0x1020  // CALL bcm2835_gpio_fsel
    4c78: e3a01000     	mov	r1, #0
    4c7c: e3a0001a     	mov	r0, #26
    4c80: ebfffbf5     	bl	0x3c5c <.plt+0x560>     @ imm = #-0x102c  // CALL bcm2835_gpio_fsel
    4c84: e3a01000     	mov	r1, #0
    4c88: e3a00017     	mov	r0, #23
    4c8c: ebfffbf2     	bl	0x3c5c <.plt+0x560>     @ imm = #-0x1038  // CALL bcm2835_gpio_fsel
    4c90: e3a01002     	mov	r1, #2
    4c94: e3a0002a     	mov	r0, #42
    4c98: ebfffc4c     	bl	0x3dd0 <.plt+0x6d4>     @ imm = #-0xed0  // CALL bcm2835_gpio_set_pud
    4c9c: e3a01002     	mov	r1, #2
    4ca0: e3a00020     	mov	r0, #32
    4ca4: ebfffc49     	bl	0x3dd0 <.plt+0x6d4>     @ imm = #-0xedc  // CALL bcm2835_gpio_set_pud
    4ca8: e3a01002     	mov	r1, #2
    4cac: e3a00026     	mov	r0, #38
    4cb0: ebfffc46     	bl	0x3dd0 <.plt+0x6d4>     @ imm = #-0xee8  // CALL bcm2835_gpio_set_pud
    4cb4: e3a01002     	mov	r1, #2
    4cb8: e3a00024     	mov	r0, #36
    4cbc: ebfffc43     	bl	0x3dd0 <.plt+0x6d4>     @ imm = #-0xef4  // CALL bcm2835_gpio_set_pud
    4cc0: e3a01002     	mov	r1, #2
    4cc4: e3a00021     	mov	r0, #33
    4cc8: ebfffc40     	bl	0x3dd0 <.plt+0x6d4>     @ imm = #-0xf00  // CALL bcm2835_gpio_set_pud
    4ccc: e3a01002     	mov	r1, #2
    4cd0: e3a0001a     	mov	r0, #26
    4cd4: ebfffc3d     	bl	0x3dd0 <.plt+0x6d4>     @ imm = #-0xf0c  // CALL bcm2835_gpio_set_pud
    4cd8: e3a01002     	mov	r1, #2
    4cdc: e3a00017     	mov	r0, #23
    4ce0: ebfffc3a     	bl	0x3dd0 <.plt+0x6d4>     @ imm = #-0xf18  // CALL bcm2835_gpio_set_pud
    4ce4: e3a01001     	mov	r1, #1
    4ce8: e3a00025     	mov	r0, #37
    4cec: ebfffbda     	bl	0x3c5c <.plt+0x560>     @ imm = #-0x1098  // CALL bcm2835_gpio_fsel
    4cf0: e3a01001     	mov	r1, #1
    4cf4: e3a00016     	mov	r0, #22
    4cf8: ebfffbd7     	bl	0x3c5c <.plt+0x560>     @ imm = #-0x10a4  // CALL bcm2835_gpio_fsel
    4cfc: e3a01001     	mov	r1, #1
    4d00: e3a00023     	mov	r0, #35
    4d04: ebfffbd4     	bl	0x3c5c <.plt+0x560>     @ imm = #-0x10b0  // CALL bcm2835_gpio_fsel
    4d08: e3a01001     	mov	r1, #1
    4d0c: e3a0000d     	mov	r0, #13
    4d10: ebfffbd1     	bl	0x3c5c <.plt+0x560>     @ imm = #-0x10bc  // CALL bcm2835_gpio_fsel
    4d14: e3a01001     	mov	r1, #1
    4d18: e3a00028     	mov	r0, #40
    4d1c: ebfffbce     	bl	0x3c5c <.plt+0x560>     @ imm = #-0x10c8  // CALL bcm2835_gpio_fsel
    4d20: e3a01001     	mov	r1, #1
    4d24: e3a00029     	mov	r0, #41
    4d28: ebfffbcb     	bl	0x3c5c <.plt+0x560>     @ imm = #-0x10d4  // CALL bcm2835_gpio_fsel
    4d2c: e3a01001     	mov	r1, #1
    4d30: e3a00022     	mov	r0, #34
    4d34: ebfffbc8     	bl	0x3c5c <.plt+0x560>     @ imm = #-0x10e0  // CALL bcm2835_gpio_fsel
    4d38: e28dd010     	add	sp, sp, #16
    4d3c: e8bd8070     	pop	{r4, r5, r6, pc}
    4d40: e59fe064     	ldr	lr, [pc, #0x64]         @ 0x4dac <_setup_gpio+0x238>  // u32=0xfd64; f32?=9.08994288e-41
    4d44: e285ca01     	add	r12, r5, #4096
    4d48: e3a02063     	mov	r2, #99
    4d4c: e5c52030     	strb	r2, [r5, #0x30]
    4d50: e08f000e     	add	r0, pc, lr
    4d54: e3a03001     	mov	r3, #1
    4d58: e59c4db0     	ldr	r4, [r12, #0xdb0]
    4d5c: e3a01000     	mov	r1, #0
    4d60: e58d3000     	str	r3, [sp]
    4d64: e344137f     	movt	r1, #0x437f
    4d68: e58d3008     	str	r3, [sp, #0x8]
    4d6c: e58d1004     	str	r1, [sp, #0x4]
    4d70: e58d100c     	str	r1, [sp, #0xc]
    4d74: ebfffa6b     	bl	0x3728 <.plt+0x2c>      @ imm = #-0x1654  // CALL gensym
    4d78: e1a0300d     	mov	r3, sp
    4d7c: e3a02002     	mov	r2, #2
    4d80: e1a01000     	mov	r1, r0
    4d84: e1a00004     	mov	r0, r4
    4d88: ebfffbb9     	bl	0x3c74 <.plt+0x578>     @ imm = #-0x111c  // CALL outlet_list
    4d8c: eaffffa6     	b	0x4c2c <_setup_gpio+0xb8> @ imm = #-0x168
    4d90: e59f0018     	ldr	r0, [pc, #0x18]         @ 0x4db0 <_setup_gpio+0x23c>  // u32=0xfe30; f32?=9.11852937e-41
    4d94: e08f0000     	add	r0, pc, r0
    4d98: ebfffb76     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x1228  // CALL post
    4d9c: eaffffa6     	b	0x4c3c <_setup_gpio+0xc8> @ imm = #-0x168
    4da0: f4 ff 00 00  	.word	0x0000fff4
    4da4: b0 fe 00 00  	.word	0x0000feb0
    4da8: 70 ff 00 00  	.word	0x0000ff70
    4dac: 64 fd 00 00  	.word	0x0000fd64
    4db0: 30 fe 00 00  	.word	0x0000fe30

