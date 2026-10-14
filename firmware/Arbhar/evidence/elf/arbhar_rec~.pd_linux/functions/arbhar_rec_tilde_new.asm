00003a94 <arbhar_rec_tilde_new>:
    3a94: e59f3460     	ldr	r3, [pc, #0x460]        @ 0x3efc <arbhar_rec_tilde_new+0x468>
    3a98: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
    3a9c: e08f0003     	add	r0, pc, r3
    3aa0: e28db020     	add	r11, sp, #32
    3aa4: e24dd00c     	sub	sp, sp, #12
    3aa8: e590000c     	ldr	r0, [r0, #0xc]
    3aac: e1a06001     	mov	r6, r1
    3ab0: e1a09002     	mov	r9, r2
    3ab4: ebfffaba     	bl	0x25a4 <.plt+0xa4>      @ imm = #-0x1518
    3ab8: e59f1440     	ldr	r1, [pc, #0x440]        @ 0x3f00 <arbhar_rec_tilde_new+0x46c>
    3abc: e3a03101     	mov	r3, #1073741824
    3ac0: e3a02000     	mov	r2, #0
    3ac4: e2465001     	sub	r5, r6, #1
    3ac8: e59f8434     	ldr	r8, [pc, #0x434]        @ 0x3f04 <arbhar_rec_tilde_new+0x470>
    3acc: e08f8008     	add	r8, pc, r8
    3ad0: e1a04000     	mov	r4, r0
    3ad4: e08f0001     	add	r0, pc, r1
    3ad8: ebfffb1d     	bl	0x2754 <.plt+0x254>     @ imm = #-0x138c
    3adc: e3a02000     	mov	r2, #0
    3ae0: e1a01002     	mov	r1, r2
    3ae4: e58421e8     	str	r2, [r4, #0x1e8]
    3ae8: e1a00004     	mov	r0, r4
    3aec: eddf0af3     	vldr	s1, [pc, #972]          @ 0x3ec0 <arbhar_rec_tilde_new+0x42c>
    3af0: ed9f0af3     	vldr	s0, [pc, #972]          @ 0x3ec4 <arbhar_rec_tilde_new+0x430>
    3af4: ebfffaad     	bl	0x25b0 <.plt+0xb0>      @ imm = #-0x154c
    3af8: e1a00004     	mov	r0, r4
    3afc: e3a01001     	mov	r1, #1
    3b00: eddf0af0     	vldr	s1, [pc, #960]          @ 0x3ec8 <arbhar_rec_tilde_new+0x434>
    3b04: ed9f0af0     	vldr	s0, [pc, #960]          @ 0x3ecc <arbhar_rec_tilde_new+0x438>
    3b08: ebfffaa8     	bl	0x25b0 <.plt+0xb0>      @ imm = #-0x1560
    3b0c: e1a00004     	mov	r0, r4
    3b10: e3a01002     	mov	r1, #2
    3b14: eddf0aeb     	vldr	s1, [pc, #940]          @ 0x3ec8 <arbhar_rec_tilde_new+0x434>
    3b18: ed9f0aec     	vldr	s0, [pc, #944]          @ 0x3ed0 <arbhar_rec_tilde_new+0x43c>
    3b1c: ebfffaa3     	bl	0x25b0 <.plt+0xb0>      @ imm = #-0x1574
    3b20: e1a00004     	mov	r0, r4
    3b24: e3a01003     	mov	r1, #3
    3b28: eddf0ae6     	vldr	s1, [pc, #920]          @ 0x3ec8 <arbhar_rec_tilde_new+0x434>
    3b2c: ed9f0ae8     	vldr	s0, [pc, #928]          @ 0x3ed4 <arbhar_rec_tilde_new+0x440>
    3b30: ebfffa9e     	bl	0x25b0 <.plt+0xb0>      @ imm = #-0x1588
    3b34: e1a00004     	mov	r0, r4
    3b38: e3a01004     	mov	r1, #4
    3b3c: eddf0ae1     	vldr	s1, [pc, #900]          @ 0x3ec8 <arbhar_rec_tilde_new+0x434>
    3b40: ed9f0ae4     	vldr	s0, [pc, #912]          @ 0x3ed8 <arbhar_rec_tilde_new+0x444>
    3b44: ebfffa99     	bl	0x25b0 <.plt+0xb0>      @ imm = #-0x159c
    3b48: e1a00004     	mov	r0, r4
    3b4c: e3a01005     	mov	r1, #5
    3b50: eddf0adc     	vldr	s1, [pc, #880]          @ 0x3ec8 <arbhar_rec_tilde_new+0x434>
    3b54: ed9f0ae0     	vldr	s0, [pc, #896]          @ 0x3edc <arbhar_rec_tilde_new+0x448>
    3b58: ebfffa94     	bl	0x25b0 <.plt+0xb0>      @ imm = #-0x15b0
    3b5c: e1a00004     	mov	r0, r4
    3b60: e3a01006     	mov	r1, #6
    3b64: eddf0ad7     	vldr	s1, [pc, #860]          @ 0x3ec8 <arbhar_rec_tilde_new+0x434>
    3b68: ed9f0adc     	vldr	s0, [pc, #880]          @ 0x3ee0 <arbhar_rec_tilde_new+0x44c>
    3b6c: ebfffa8f     	bl	0x25b0 <.plt+0xb0>      @ imm = #-0x15c4
    3b70: e1a00004     	mov	r0, r4
    3b74: e3a01007     	mov	r1, #7
    3b78: eddf0ad2     	vldr	s1, [pc, #840]          @ 0x3ec8 <arbhar_rec_tilde_new+0x434>
    3b7c: ed9f0ad8     	vldr	s0, [pc, #864]          @ 0x3ee4 <arbhar_rec_tilde_new+0x450>
    3b80: ebfffa8a     	bl	0x25b0 <.plt+0xb0>      @ imm = #-0x15d8
    3b84: e1a00004     	mov	r0, r4
    3b88: e3a01008     	mov	r1, #8
    3b8c: eddf0acd     	vldr	s1, [pc, #820]          @ 0x3ec8 <arbhar_rec_tilde_new+0x434>
    3b90: ed9f0ad4     	vldr	s0, [pc, #848]          @ 0x3ee8 <arbhar_rec_tilde_new+0x454>
    3b94: ebfffa85     	bl	0x25b0 <.plt+0xb0>      @ imm = #-0x15ec
    3b98: e1a00004     	mov	r0, r4
    3b9c: e3a01009     	mov	r1, #9
    3ba0: eddf0ac8     	vldr	s1, [pc, #800]          @ 0x3ec8 <arbhar_rec_tilde_new+0x434>
    3ba4: ed9f0ad0     	vldr	s0, [pc, #832]          @ 0x3eec <arbhar_rec_tilde_new+0x458>
    3ba8: ebfffa80     	bl	0x25b0 <.plt+0xb0>      @ imm = #-0x1600
    3bac: e1a00004     	mov	r0, r4
    3bb0: e3a0100a     	mov	r1, #10
    3bb4: eddf0ac3     	vldr	s1, [pc, #780]          @ 0x3ec8 <arbhar_rec_tilde_new+0x434>
    3bb8: ed9f0acc     	vldr	s0, [pc, #816]          @ 0x3ef0 <arbhar_rec_tilde_new+0x45c>
    3bbc: ebfffa7b     	bl	0x25b0 <.plt+0xb0>      @ imm = #-0x1614
    3bc0: e1a00004     	mov	r0, r4
    3bc4: e3a0100b     	mov	r1, #11
    3bc8: eddf0abe     	vldr	s1, [pc, #760]          @ 0x3ec8 <arbhar_rec_tilde_new+0x434>
    3bcc: ed9f0ac8     	vldr	s0, [pc, #800]          @ 0x3ef4 <arbhar_rec_tilde_new+0x460>
    3bd0: ebfffa76     	bl	0x25b0 <.plt+0xb0>      @ imm = #-0x1628
    3bd4: e1a00004     	mov	r0, r4
    3bd8: e3a0100c     	mov	r1, #12
    3bdc: eddf0ab9     	vldr	s1, [pc, #740]          @ 0x3ec8 <arbhar_rec_tilde_new+0x434>
    3be0: ed9f0ac4     	vldr	s0, [pc, #784]          @ 0x3ef8 <arbhar_rec_tilde_new+0x464>
    3be4: ebfffa71     	bl	0x25b0 <.plt+0xb0>      @ imm = #-0x163c
    3be8: e1a00009     	mov	r0, r9
    3bec: ebfffb08     	bl	0x2814 <.plt+0x314>     @ imm = #-0x13e0
    3bf0: e3550000     	cmp	r5, #0
    3bf4: c1a07105     	lslgt	r7, r5, #2
    3bf8: d3a0a030     	movle	r10, #48
    3bfc: d3a0500c     	movle	r5, #12
    3c00: c1a0a007     	movgt	r10, r7
    3c04: e28ac007     	add	r12, r10, #7
    3c08: e3cc3007     	bic	r3, r12, #7
    3c0c: d1a0700a     	movle	r7, r10
    3c10: e04dd003     	sub	sp, sp, r3
    3c14: e3560001     	cmp	r6, #1
    3c18: e5845250     	str	r5, [r4, #0x250]
    3c1c: e1a0600d     	mov	r6, sp
    3c20: e50b0028     	str	r0, [r11, #-0x28]
    3c24: da0000c6     	ble	0x3f44 <arbhar_rec_tilde_new+0x4b0> @ imm = #0x318
    3c28: e247e004     	sub	lr, r7, #4
    3c2c: e2899008     	add	r9, r9, #8
    3c30: e1a05006     	mov	r5, r6
    3c34: e086a007     	add	r10, r6, r7
    3c38: e1a0012e     	lsr	r0, lr, #2
    3c3c: e2801001     	add	r1, r0, #1
    3c40: e2112007     	ands	r2, r1, #7
    3c44: 0a000029     	beq	0x3cf0 <arbhar_rec_tilde_new+0x25c> @ imm = #0xa4
    3c48: e3520001     	cmp	r2, #1
    3c4c: 0a000021     	beq	0x3cd8 <arbhar_rec_tilde_new+0x244> @ imm = #0x84
    3c50: e3520002     	cmp	r2, #2
    3c54: 0a00001b     	beq	0x3cc8 <arbhar_rec_tilde_new+0x234> @ imm = #0x6c
    3c58: e3520003     	cmp	r2, #3
    3c5c: 0a000015     	beq	0x3cb8 <arbhar_rec_tilde_new+0x224> @ imm = #0x54
    3c60: e3520004     	cmp	r2, #4
    3c64: 0a00000f     	beq	0x3ca8 <arbhar_rec_tilde_new+0x214> @ imm = #0x3c
    3c68: e3520005     	cmp	r2, #5
    3c6c: 0a000009     	beq	0x3c98 <arbhar_rec_tilde_new+0x204> @ imm = #0x24
    3c70: e3520006     	cmp	r2, #6
    3c74: 0a000003     	beq	0x3c88 <arbhar_rec_tilde_new+0x1f4> @ imm = #0xc
    3c78: e1a00009     	mov	r0, r9
    3c7c: e2899008     	add	r9, r9, #8
    3c80: ebfffae3     	bl	0x2814 <.plt+0x314>     @ imm = #-0x1474
    3c84: e4850004     	str	r0, [r5], #4
    3c88: e1a00009     	mov	r0, r9
    3c8c: e2899008     	add	r9, r9, #8
    3c90: ebfffadf     	bl	0x2814 <.plt+0x314>     @ imm = #-0x1484
    3c94: e4850004     	str	r0, [r5], #4
    3c98: e1a00009     	mov	r0, r9
    3c9c: e2899008     	add	r9, r9, #8
    3ca0: ebfffadb     	bl	0x2814 <.plt+0x314>     @ imm = #-0x1494
    3ca4: e4850004     	str	r0, [r5], #4
    3ca8: e1a00009     	mov	r0, r9
    3cac: e2899008     	add	r9, r9, #8
    3cb0: ebfffad7     	bl	0x2814 <.plt+0x314>     @ imm = #-0x14a4
    3cb4: e4850004     	str	r0, [r5], #4
    3cb8: e1a00009     	mov	r0, r9
    3cbc: e2899008     	add	r9, r9, #8
    3cc0: ebfffad3     	bl	0x2814 <.plt+0x314>     @ imm = #-0x14b4
    3cc4: e4850004     	str	r0, [r5], #4
    3cc8: e1a00009     	mov	r0, r9
    3ccc: e2899008     	add	r9, r9, #8
    3cd0: ebfffacf     	bl	0x2814 <.plt+0x314>     @ imm = #-0x14c4
    3cd4: e4850004     	str	r0, [r5], #4
    3cd8: e1a00009     	mov	r0, r9
    3cdc: e2899008     	add	r9, r9, #8
    3ce0: ebfffacb     	bl	0x2814 <.plt+0x314>     @ imm = #-0x14d4
    3ce4: e4850004     	str	r0, [r5], #4
    3ce8: e15a0005     	cmp	r10, r5
    3cec: 0a00001e     	beq	0x3d6c <arbhar_rec_tilde_new+0x2d8> @ imm = #0x78
    3cf0: e1a00009     	mov	r0, r9
    3cf4: ebfffac6     	bl	0x2814 <.plt+0x314>     @ imm = #-0x14e8
    3cf8: e1a0c005     	mov	r12, r5
    3cfc: e2855020     	add	r5, r5, #32
    3d00: e48c0004     	str	r0, [r12], #4
    3d04: e2890008     	add	r0, r9, #8
    3d08: e50bc02c     	str	r12, [r11, #-0x2c]
    3d0c: ebfffac0     	bl	0x2814 <.plt+0x314>     @ imm = #-0x1500
    3d10: e505001c     	str	r0, [r5, #-0x1c]
    3d14: e2890010     	add	r0, r9, #16
    3d18: ebfffabd     	bl	0x2814 <.plt+0x314>     @ imm = #-0x150c
    3d1c: e51b302c     	ldr	r3, [r11, #-0x2c]
    3d20: e5830004     	str	r0, [r3, #0x4]
    3d24: e2890018     	add	r0, r9, #24
    3d28: ebfffab9     	bl	0x2814 <.plt+0x314>     @ imm = #-0x151c
    3d2c: e5050014     	str	r0, [r5, #-0x14]
    3d30: e2890020     	add	r0, r9, #32
    3d34: ebfffab6     	bl	0x2814 <.plt+0x314>     @ imm = #-0x1528
    3d38: e5050010     	str	r0, [r5, #-0x10]
    3d3c: e2890028     	add	r0, r9, #40
    3d40: ebfffab3     	bl	0x2814 <.plt+0x314>     @ imm = #-0x1534
    3d44: e505000c     	str	r0, [r5, #-0xc]
    3d48: e2890030     	add	r0, r9, #48
    3d4c: ebfffab0     	bl	0x2814 <.plt+0x314>     @ imm = #-0x1540
    3d50: e5050008     	str	r0, [r5, #-0x8]
    3d54: e2890038     	add	r0, r9, #56
    3d58: ebfffaad     	bl	0x2814 <.plt+0x314>     @ imm = #-0x154c
    3d5c: e2899040     	add	r9, r9, #64
    3d60: e5050004     	str	r0, [r5, #-0x4]
    3d64: e15a0005     	cmp	r10, r5
    3d68: 1affffe0     	bne	0x3cf0 <arbhar_rec_tilde_new+0x25c> @ imm = #-0x80
    3d6c: e3a05000     	mov	r5, #0
    3d70: e1a01006     	mov	r1, r6
    3d74: e1a02007     	mov	r2, r7
    3d78: e2840f55     	add	r0, r4, #340
    3d7c: ebfffa14     	bl	0x25d4 <.plt+0xd4>      @ imm = #-0x17b0
    3d80: e1a02007     	mov	r2, r7
    3d84: e1a01005     	mov	r1, r5
    3d88: e28400f4     	add	r0, r4, #244
    3d8c: ebfffa6a     	bl	0x273c <.plt+0x23c>     @ imm = #-0x1658
    3d90: e59f2170     	ldr	r2, [pc, #0x170]        @ 0x3f08 <arbhar_rec_tilde_new+0x474>
    3d94: e2849e29     	add	r9, r4, #656
    3d98: e3a0cc01     	mov	r12, #256
    3d9c: e3e0e102     	mvn	lr, #-2147483648
    3da0: e3a075fe     	mov	r7, #1065353216
    3da4: e584e080     	str	lr, [r4, #0x80]
    3da8: e3a06000     	mov	r6, #0
    3dac: e5846254     	str	r6, [r4, #0x254]
    3db0: e1a01004     	mov	r1, r4
    3db4: e1c950b0     	strh	r5, [r9]
    3db8: e1a00004     	mov	r0, r4
    3dbc: e584e01c     	str	lr, [r4, #0x1c]
    3dc0: e584e04c     	str	lr, [r4, #0x4c]
    3dc4: e584c084     	str	r12, [r4, #0x84]
    3dc8: e584c08c     	str	r12, [r4, #0x8c]
    3dcc: e584c088     	str	r12, [r4, #0x88]
    3dd0: e584c090     	str	r12, [r4, #0x90]
    3dd4: e584c024     	str	r12, [r4, #0x24]
    3dd8: e584c02c     	str	r12, [r4, #0x2c]
    3ddc: e584c028     	str	r12, [r4, #0x28]
    3de0: e584c030     	str	r12, [r4, #0x30]
    3de4: e584c054     	str	r12, [r4, #0x54]
    3de8: e584c05c     	str	r12, [r4, #0x5c]
    3dec: e584c058     	str	r12, [r4, #0x58]
    3df0: e584c060     	str	r12, [r4, #0x60]
    3df4: e5847260     	str	r7, [r4, #0x260]
    3df8: e584703c     	str	r7, [r4, #0x3c]
    3dfc: e584706c     	str	r7, [r4, #0x6c]
    3e00: e584527c     	str	r5, [r4, #0x27c]
    3e04: e584526c     	str	r5, [r4, #0x26c]
    3e08: e5845270     	str	r5, [r4, #0x270]
    3e0c: e5845278     	str	r5, [r4, #0x278]
    3e10: e5845048     	str	r5, [r4, #0x48]
    3e14: e5845078     	str	r5, [r4, #0x78]
    3e18: e798a002     	ldr	r10, [r8, r2]
    3e1c: e1a0200a     	mov	r2, r10
    3e20: e1a0300a     	mov	r3, r10
    3e24: ebfff9f6     	bl	0x2604 <.plt+0x104>     @ imm = #-0x1828
    3e28: e1a0100a     	mov	r1, r10
    3e2c: e5840298     	str	r0, [r4, #0x298]
    3e30: e1a00004     	mov	r0, r4
    3e34: ebfffa55     	bl	0x2790 <.plt+0x290>     @ imm = #-0x16ac
    3e38: e59f30cc     	ldr	r3, [pc, #0xcc]         @ 0x3f0c <arbhar_rec_tilde_new+0x478>
    3e3c: e584029c     	str	r0, [r4, #0x29c]
    3e40: e1a00004     	mov	r0, r4
    3e44: e7981003     	ldr	r1, [r8, r3]
    3e48: ebfffa50     	bl	0x2790 <.plt+0x290>     @ imm = #-0x16c0
    3e4c: e51b7028     	ldr	r7, [r11, #-0x28]
    3e50: e59f10b8     	ldr	r1, [pc, #0xb8]         @ 0x3f10 <arbhar_rec_tilde_new+0x47c>
    3e54: e5845288     	str	r5, [r4, #0x288]
    3e58: e584528c     	str	r5, [r4, #0x28c]
    3e5c: e58472a4     	str	r7, [r4, #0x2a4]
    3e60: e58402a0     	str	r0, [r4, #0x2a0]
    3e64: e1a00004     	mov	r0, r4
    3e68: e7981001     	ldr	r1, [r8, r1]
    3e6c: ebfffa0b     	bl	0x26a0 <.plt+0x1a0>     @ imm = #-0x17d4
    3e70: e5948250     	ldr	r8, [r4, #0x250]
    3e74: e1580005     	cmp	r8, r5
    3e78: e584007c     	str	r0, [r4, #0x7c]
    3e7c: da00000c     	ble	0x3eb4 <arbhar_rec_tilde_new+0x420> @ imm = #0x30
    3e80: e1a06108     	lsl	r6, r8, #2
    3e84: e1a01005     	mov	r1, r5
    3e88: e2840faa     	add	r0, r4, #680
    3e8c: e1a02006     	mov	r2, r6
    3e90: ebfffa29     	bl	0x273c <.plt+0x23c>     @ imm = #-0x175c
    3e94: e1a02006     	mov	r2, r6
    3e98: e1a01005     	mov	r1, r5
    3e9c: e2840fc2     	add	r0, r4, #776
    3ea0: ebfffa25     	bl	0x273c <.plt+0x23c>     @ imm = #-0x176c
    3ea4: e1a02006     	mov	r2, r6
    3ea8: e1a01005     	mov	r1, r5
    3eac: e2840fda     	add	r0, r4, #872
    3eb0: ebfffa21     	bl	0x273c <.plt+0x23c>     @ imm = #-0x177c
    3eb4: e1a00004     	mov	r0, r4
    3eb8: e24bd020     	sub	sp, r11, #32
    3ebc: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
    3ec0: 00 00 7a 43  	.word	0x437a0000
    3ec4: 00 5b 6e 47  	.word	0x476e5b00
    3ec8: 00 58 18 49  	.word	0x49185800
    3ecc: 80 cc b0 47  	.word	0x47b0cc80
    3ed0: 00 cd b0 47  	.word	0x47b0cd00
    3ed4: 80 cd b0 47  	.word	0x47b0cd80
    3ed8: 00 ce b0 47  	.word	0x47b0ce00
    3edc: 80 ce b0 47  	.word	0x47b0ce80
    3ee0: 00 cf b0 47  	.word	0x47b0cf00
    3ee4: 80 cf b0 47  	.word	0x47b0cf80
    3ee8: 00 d0 b0 47  	.word	0x47b0d000
    3eec: 80 d0 b0 47  	.word	0x47b0d080
    3ef0: 00 d1 b0 47  	.word	0x47b0d100
    3ef4: 80 d1 b0 47  	.word	0x47b0d180
    3ef8: 00 d2 b0 47  	.word	0x47b0d200
    3efc: 0c 67 01 00  	.word	0x0001670c
    3f00: 40 5b 00 00  	.word	0x00005b40
    3f04: 2c 65 01 00  	.word	0x0001652c
    3f08: 64 01 00 00  	.word	0x00000164
    3f0c: 5c 01 00 00  	.word	0x0000015c
    3f10: 34 01 00 00  	.word	0x00000134
    3f14: e0 56 00 00  	.word	0x000056e0
    3f18: d0 56 00 00  	.word	0x000056d0
    3f1c: a0 56 00 00  	.word	0x000056a0
    3f20: 98 56 00 00  	.word	0x00005698
    3f24: 78 56 00 00  	.word	0x00005678
    3f28: c0 56 00 00  	.word	0x000056c0
    3f2c: b4 56 00 00  	.word	0x000056b4
    3f30: a8 56 00 00  	.word	0x000056a8
    3f34: 8c 56 00 00  	.word	0x0000568c
    3f38: 80 56 00 00  	.word	0x00005680
    3f3c: 6c 56 00 00  	.word	0x0000566c
    3f40: 60 56 00 00  	.word	0x00005660
    3f44: e51fe038     	ldr	lr, [pc, #-0x38]        @ 0x3f14 <arbhar_rec_tilde_new+0x480>
    3f48: e51fa038     	ldr	r10, [pc, #-0x38]       @ 0x3f18 <arbhar_rec_tilde_new+0x484>
    3f4c: e08f000e     	add	r0, pc, lr
    3f50: e51f503c     	ldr	r5, [pc, #-0x3c]        @ 0x3f1c <arbhar_rec_tilde_new+0x488>
    3f54: ebfff96e     	bl	0x2514 <.plt+0x14>      @ imm = #-0x1a48
    3f58: e51f9040     	ldr	r9, [pc, #-0x40]        @ 0x3f20 <arbhar_rec_tilde_new+0x48c>
    3f5c: e5860000     	str	r0, [r6]
    3f60: e08f000a     	add	r0, pc, r10
    3f64: ebfff96a     	bl	0x2514 <.plt+0x14>      @ imm = #-0x1a58
    3f68: e51fa04c     	ldr	r10, [pc, #-0x4c]       @ 0x3f24 <arbhar_rec_tilde_new+0x490>
    3f6c: e5860004     	str	r0, [r6, #0x4]
    3f70: e51f0050     	ldr	r0, [pc, #-0x50]        @ 0x3f28 <arbhar_rec_tilde_new+0x494>
    3f74: e08f0000     	add	r0, pc, r0
    3f78: ebfff965     	bl	0x2514 <.plt+0x14>      @ imm = #-0x1a6c
    3f7c: e51f1058     	ldr	r1, [pc, #-0x58]        @ 0x3f2c <arbhar_rec_tilde_new+0x498>
    3f80: e5860008     	str	r0, [r6, #0x8]
    3f84: e08f0001     	add	r0, pc, r1
    3f88: ebfff961     	bl	0x2514 <.plt+0x14>      @ imm = #-0x1a7c
    3f8c: e51f2064     	ldr	r2, [pc, #-0x64]        @ 0x3f30 <arbhar_rec_tilde_new+0x49c>
    3f90: e586000c     	str	r0, [r6, #0xc]
    3f94: e08f0002     	add	r0, pc, r2
    3f98: ebfff95d     	bl	0x2514 <.plt+0x14>      @ imm = #-0x1a8c
    3f9c: e5860010     	str	r0, [r6, #0x10]
    3fa0: e08f0005     	add	r0, pc, r5
    3fa4: ebfff95a     	bl	0x2514 <.plt+0x14>      @ imm = #-0x1a98
    3fa8: e5860014     	str	r0, [r6, #0x14]
    3fac: e08f0009     	add	r0, pc, r9
    3fb0: ebfff957     	bl	0x2514 <.plt+0x14>      @ imm = #-0x1aa4
    3fb4: e51fc088     	ldr	r12, [pc, #-0x88]       @ 0x3f34 <arbhar_rec_tilde_new+0x4a0>
    3fb8: e5860018     	str	r0, [r6, #0x18]
    3fbc: e08f000c     	add	r0, pc, r12
    3fc0: ebfff953     	bl	0x2514 <.plt+0x14>      @ imm = #-0x1ab4
    3fc4: e51f3094     	ldr	r3, [pc, #-0x94]        @ 0x3f38 <arbhar_rec_tilde_new+0x4a4>
    3fc8: e586001c     	str	r0, [r6, #0x1c]
    3fcc: e08f0003     	add	r0, pc, r3
    3fd0: ebfff94f     	bl	0x2514 <.plt+0x14>      @ imm = #-0x1ac4
    3fd4: e5860020     	str	r0, [r6, #0x20]
    3fd8: e08f000a     	add	r0, pc, r10
    3fdc: ebfff94c     	bl	0x2514 <.plt+0x14>      @ imm = #-0x1ad0
    3fe0: e5860024     	str	r0, [r6, #0x24]
    3fe4: e51f00b0     	ldr	r0, [pc, #-0xb0]        @ 0x3f3c <arbhar_rec_tilde_new+0x4a8>
    3fe8: e08f0000     	add	r0, pc, r0
    3fec: ebfff948     	bl	0x2514 <.plt+0x14>      @ imm = #-0x1ae0
    3ff0: e51f10b8     	ldr	r1, [pc, #-0xb8]        @ 0x3f40 <arbhar_rec_tilde_new+0x4ac>
    3ff4: e5860028     	str	r0, [r6, #0x28]
    3ff8: e08f0001     	add	r0, pc, r1
    3ffc: ebfff944     	bl	0x2514 <.plt+0x14>      @ imm = #-0x1af0
    4000: e586002c     	str	r0, [r6, #0x2c]
    4004: eaffff58     	b	0x3d6c <arbhar_rec_tilde_new+0x2d8> @ imm = #-0x2a0

