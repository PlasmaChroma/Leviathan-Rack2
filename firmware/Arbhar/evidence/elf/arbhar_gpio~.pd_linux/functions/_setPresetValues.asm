00008794 <_setPresetValues>:
    8794: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
    8798: e1a06002     	mov	r6, r2
    879c: ed2d8b08     	vpush	{d8, d9, d10, d11}
    87a0: e1a09000     	mov	r9, r0
    87a4: e3a02032     	mov	r2, #50
    87a8: e1a00003     	mov	r0, r3
    87ac: e1a05003     	mov	r5, r3
    87b0: e28db040     	add	r11, sp, #64
    87b4: e24dd064     	sub	sp, sp, #100
    87b8: e24b4078     	sub	r4, r11, #120
    87bc: e1a01004     	mov	r1, r4
    87c0: ebffed85     	bl	0x3ddc <.plt+0x6e0>     @ imm = #-0x49ec
    87c4: e3560000     	cmp	r6, #0
    87c8: 0a0001cb     	beq	0x8efc <_setPresetValues+0x768> @ imm = #0x72c
    87cc: e2167001     	ands	r7, r6, #1
    87d0: 1a0001b3     	bne	0x8ea4 <_setPresetValues+0x710> @ imm = #0x6cc
    87d4: e086efa6     	add	lr, r6, r6, lsr #31
    87d8: e3560001     	cmp	r6, #1
    87dc: e3a08001     	mov	r8, #1
    87e0: e5c980b8     	strb	r8, [r9, #0xb8]
    87e4: e1a0a85e     	asr	r10, lr, r8
    87e8: e50ba080     	str	r10, [r11, #-0x80]
    87ec: da0001c2     	ble	0x8efc <_setPresetValues+0x768> @ imm = #0x708
    87f0: ed9fabfc     	vldr	d10, [pc, #1008]        @ 0x8be8 <_setPresetValues+0x454>
    87f4: e289ca01     	add	r12, r9, #4096
    87f8: e59f23f8     	ldr	r2, [pc, #0x3f8]        @ 0x8bf8 <_setPresetValues+0x464>
    87fc: e28c3edf     	add	r3, r12, #3568
    8800: e2858008     	add	r8, r5, #8
    8804: e2830008     	add	r0, r3, #8
    8808: ed9f9bf8     	vldr	d9, [pc, #992]          @ 0x8bf0 <_setPresetValues+0x45c>
    880c: e08f1002     	add	r1, pc, r2
    8810: e0855186     	add	r5, r5, r6, lsl #3
    8814: e50bc088     	str	r12, [r11, #-0x88]
    8818: e2466001     	sub	r6, r6, #1
    881c: e50b8090     	str	r8, [r11, #-0x90]
    8820: e50b1084     	str	r1, [r11, #-0x84]
    8824: e50b009c     	str	r0, [r11, #-0x9c]
    8828: e50b608c     	str	r6, [r11, #-0x8c]
    882c: e1a00008     	mov	r0, r8
    8830: ebffec52     	bl	0x3980 <.plt+0x284>     @ imm = #-0x4eb8
    8834: e51b1084     	ldr	r1, [r11, #-0x84]
    8838: e1a00004     	mov	r0, r4
    883c: eef08a40     	vmov.f32	s17, s0
    8840: ebffebc7     	bl	0x3764 <.plt+0x68>      @ imm = #-0x50e4
    8844: e3500000     	cmp	r0, #0
    8848: 0a0001ae     	beq	0x8f08 <_setPresetValues+0x774> @ imm = #0x6b8
    884c: e59fe3a8     	ldr	lr, [pc, #0x3a8]        @ 0x8bfc <_setPresetValues+0x468>
    8850: e1a00004     	mov	r0, r4
    8854: e08f100e     	add	r1, pc, lr
    8858: ebffebc1     	bl	0x3764 <.plt+0x68>      @ imm = #-0x50fc
    885c: e3500000     	cmp	r0, #0
    8860: 0a000222     	beq	0x90f0 <_setPresetValues+0x95c> @ imm = #0x888
    8864: e59fa394     	ldr	r10, [pc, #0x394]       @ 0x8c00 <_setPresetValues+0x46c>
    8868: e1a00004     	mov	r0, r4
    886c: e08f100a     	add	r1, pc, r10
    8870: ebffebbb     	bl	0x3764 <.plt+0x68>      @ imm = #-0x5114
    8874: e3500000     	cmp	r0, #0
    8878: 0a00027f     	beq	0x927c <_setPresetValues+0xae8> @ imm = #0x9fc
    887c: e59fc380     	ldr	r12, [pc, #0x380]       @ 0x8c04 <_setPresetValues+0x470>
    8880: e1a00004     	mov	r0, r4
    8884: e08f100c     	add	r1, pc, r12
    8888: ebffebb5     	bl	0x3764 <.plt+0x68>      @ imm = #-0x512c
    888c: e3500000     	cmp	r0, #0
    8890: 0a000288     	beq	0x92b8 <_setPresetValues+0xb24> @ imm = #0xa20
    8894: e59f336c     	ldr	r3, [pc, #0x36c]        @ 0x8c08 <_setPresetValues+0x474>
    8898: e1a00004     	mov	r0, r4
    889c: e08f1003     	add	r1, pc, r3
    88a0: ebffebaf     	bl	0x3764 <.plt+0x68>      @ imm = #-0x5144
    88a4: e3500000     	cmp	r0, #0
    88a8: 0a000370     	beq	0x9670 <_setPresetValues+0xedc> @ imm = #0xdc0
    88ac: e59f2358     	ldr	r2, [pc, #0x358]        @ 0x8c0c <_setPresetValues+0x478>
    88b0: e1a00004     	mov	r0, r4
    88b4: e08f1002     	add	r1, pc, r2
    88b8: ebffeba9     	bl	0x3764 <.plt+0x68>      @ imm = #-0x515c
    88bc: e3500000     	cmp	r0, #0
    88c0: 0a0002f6     	beq	0x94a0 <_setPresetValues+0xd0c> @ imm = #0xbd8
    88c4: e59f1344     	ldr	r1, [pc, #0x344]        @ 0x8c10 <_setPresetValues+0x47c>
    88c8: e1a00004     	mov	r0, r4
    88cc: e08f1001     	add	r1, pc, r1
    88d0: ebffeba3     	bl	0x3764 <.plt+0x68>      @ imm = #-0x5174
    88d4: e3500000     	cmp	r0, #0
    88d8: 0a00036d     	beq	0x9694 <_setPresetValues+0xf00> @ imm = #0xdb4
    88dc: e59f0330     	ldr	r0, [pc, #0x330]        @ 0x8c14 <_setPresetValues+0x480>
    88e0: e08f1000     	add	r1, pc, r0
    88e4: e1a00004     	mov	r0, r4
    88e8: ebffeb9d     	bl	0x3764 <.plt+0x68>      @ imm = #-0x518c
    88ec: e3500000     	cmp	r0, #0
    88f0: 0a000391     	beq	0x973c <_setPresetValues+0xfa8> @ imm = #0xe44
    88f4: e59f631c     	ldr	r6, [pc, #0x31c]        @ 0x8c18 <_setPresetValues+0x484>
    88f8: e1a00004     	mov	r0, r4
    88fc: e08f1006     	add	r1, pc, r6
    8900: ebffeb97     	bl	0x3764 <.plt+0x68>      @ imm = #-0x51a4
    8904: e3500000     	cmp	r0, #0
    8908: 0a000393     	beq	0x975c <_setPresetValues+0xfc8> @ imm = #0xe4c
    890c: e59fe308     	ldr	lr, [pc, #0x308]        @ 0x8c1c <_setPresetValues+0x488>
    8910: e1a00004     	mov	r0, r4
    8914: e08f100e     	add	r1, pc, lr
    8918: ebffeb91     	bl	0x3764 <.plt+0x68>      @ imm = #-0x51bc
    891c: e3500000     	cmp	r0, #0
    8920: 0a000379     	beq	0x970c <_setPresetValues+0xf78> @ imm = #0xde4
    8924: e59fa2f4     	ldr	r10, [pc, #0x2f4]       @ 0x8c20 <_setPresetValues+0x48c>
    8928: e1a00004     	mov	r0, r4
    892c: e08f100a     	add	r1, pc, r10
    8930: ebffeb8b     	bl	0x3764 <.plt+0x68>      @ imm = #-0x51d4
    8934: e3500000     	cmp	r0, #0
    8938: 0a000393     	beq	0x978c <_setPresetValues+0xff8> @ imm = #0xe4c
    893c: e59fc2e0     	ldr	r12, [pc, #0x2e0]       @ 0x8c24 <_setPresetValues+0x490>
    8940: e1a00004     	mov	r0, r4
    8944: e08f100c     	add	r1, pc, r12
    8948: ebffeb85     	bl	0x3764 <.plt+0x68>      @ imm = #-0x51ec
    894c: e3500000     	cmp	r0, #0
    8950: 0a000396     	beq	0x97b0 <_setPresetValues+0x101c> @ imm = #0xe58
    8954: e59f32cc     	ldr	r3, [pc, #0x2cc]        @ 0x8c28 <_setPresetValues+0x494>
    8958: e1a00004     	mov	r0, r4
    895c: e08f1003     	add	r1, pc, r3
    8960: ebffeb7f     	bl	0x3764 <.plt+0x68>      @ imm = #-0x5204
    8964: e3500000     	cmp	r0, #0
    8968: 0a000398     	beq	0x97d0 <_setPresetValues+0x103c> @ imm = #0xe60
    896c: e59f22b8     	ldr	r2, [pc, #0x2b8]        @ 0x8c2c <_setPresetValues+0x498>
    8970: e1a00004     	mov	r0, r4
    8974: e08f1002     	add	r1, pc, r2
    8978: ebffeb79     	bl	0x3764 <.plt+0x68>      @ imm = #-0x521c
    897c: e3500000     	cmp	r0, #0
    8980: 0a0003a1     	beq	0x980c <_setPresetValues+0x1078> @ imm = #0xe84
    8984: e59f12a4     	ldr	r1, [pc, #0x2a4]        @ 0x8c30 <_setPresetValues+0x49c>
    8988: e1a00004     	mov	r0, r4
    898c: e08f1001     	add	r1, pc, r1
    8990: ebffeb73     	bl	0x3764 <.plt+0x68>      @ imm = #-0x5234
    8994: e3500000     	cmp	r0, #0
    8998: 0a0003a7     	beq	0x983c <_setPresetValues+0x10a8> @ imm = #0xe9c
    899c: e59f0290     	ldr	r0, [pc, #0x290]        @ 0x8c34 <_setPresetValues+0x4a0>
    89a0: e08f1000     	add	r1, pc, r0
    89a4: e1a00004     	mov	r0, r4
    89a8: ebffeb6d     	bl	0x3764 <.plt+0x68>      @ imm = #-0x524c
    89ac: e3500000     	cmp	r0, #0
    89b0: 0a0003a7     	beq	0x9854 <_setPresetValues+0x10c0> @ imm = #0xe9c
    89b4: e59f627c     	ldr	r6, [pc, #0x27c]        @ 0x8c38 <_setPresetValues+0x4a4>
    89b8: e1a00004     	mov	r0, r4
    89bc: e08f1006     	add	r1, pc, r6
    89c0: ebffeb67     	bl	0x3764 <.plt+0x68>      @ imm = #-0x5264
    89c4: e3500000     	cmp	r0, #0
    89c8: 0a0003ab     	beq	0x987c <_setPresetValues+0x10e8> @ imm = #0xeac
    89cc: e59fe268     	ldr	lr, [pc, #0x268]        @ 0x8c3c <_setPresetValues+0x4a8>
    89d0: e1a00004     	mov	r0, r4
    89d4: e08f100e     	add	r1, pc, lr
    89d8: ebffeb61     	bl	0x3764 <.plt+0x68>      @ imm = #-0x527c
    89dc: e3500000     	cmp	r0, #0
    89e0: 0a0003af     	beq	0x98a4 <_setPresetValues+0x1110> @ imm = #0xebc
    89e4: e59fa254     	ldr	r10, [pc, #0x254]       @ 0x8c40 <_setPresetValues+0x4ac>
    89e8: e1a00004     	mov	r0, r4
    89ec: e08f100a     	add	r1, pc, r10
    89f0: ebffeb5b     	bl	0x3764 <.plt+0x68>      @ imm = #-0x5294
    89f4: e3500000     	cmp	r0, #0
    89f8: 0a0003c9     	beq	0x9924 <_setPresetValues+0x1190> @ imm = #0xf24
    89fc: e59fc240     	ldr	r12, [pc, #0x240]       @ 0x8c44 <_setPresetValues+0x4b0>
    8a00: e1a00004     	mov	r0, r4
    8a04: e08f100c     	add	r1, pc, r12
    8a08: ebffeb55     	bl	0x3764 <.plt+0x68>      @ imm = #-0x52ac
    8a0c: e3500000     	cmp	r0, #0
    8a10: 0a0003b8     	beq	0x98f8 <_setPresetValues+0x1164> @ imm = #0xee0
    8a14: e59f322c     	ldr	r3, [pc, #0x22c]        @ 0x8c48 <_setPresetValues+0x4b4>
    8a18: e1a00004     	mov	r0, r4
    8a1c: e08f1003     	add	r1, pc, r3
    8a20: ebffeb4f     	bl	0x3764 <.plt+0x68>      @ imm = #-0x52c4
    8a24: e3500000     	cmp	r0, #0
    8a28: 0a0003d3     	beq	0x997c <_setPresetValues+0x11e8> @ imm = #0xf4c
    8a2c: e59f2218     	ldr	r2, [pc, #0x218]        @ 0x8c4c <_setPresetValues+0x4b8>
    8a30: e1a00004     	mov	r0, r4
    8a34: e08f1002     	add	r1, pc, r2
    8a38: ebffeb49     	bl	0x3764 <.plt+0x68>      @ imm = #-0x52dc
    8a3c: e3500000     	cmp	r0, #0
    8a40: 0a0003c1     	beq	0x994c <_setPresetValues+0x11b8> @ imm = #0xf04
    8a44: e59f1204     	ldr	r1, [pc, #0x204]        @ 0x8c50 <_setPresetValues+0x4bc>
    8a48: e1a00004     	mov	r0, r4
    8a4c: e08f1001     	add	r1, pc, r1
    8a50: ebffeb43     	bl	0x3764 <.plt+0x68>      @ imm = #-0x52f4
    8a54: e3500000     	cmp	r0, #0
    8a58: 0a000434     	beq	0x9b30 <_setPresetValues+0x139c> @ imm = #0x10d0
    8a5c: e59f01f0     	ldr	r0, [pc, #0x1f0]        @ 0x8c54 <_setPresetValues+0x4c0>
    8a60: e08f1000     	add	r1, pc, r0
    8a64: e1a00004     	mov	r0, r4
    8a68: ebffeb3d     	bl	0x3764 <.plt+0x68>      @ imm = #-0x530c
    8a6c: e3500000     	cmp	r0, #0
    8a70: 0a000422     	beq	0x9b00 <_setPresetValues+0x136c> @ imm = #0x1088
    8a74: e59f61dc     	ldr	r6, [pc, #0x1dc]        @ 0x8c58 <_setPresetValues+0x4c4>
    8a78: e1a00004     	mov	r0, r4
    8a7c: e08f1006     	add	r1, pc, r6
    8a80: ebffeb37     	bl	0x3764 <.plt+0x68>      @ imm = #-0x5324
    8a84: e3500000     	cmp	r0, #0
    8a88: 0a00040c     	beq	0x9ac0 <_setPresetValues+0x132c> @ imm = #0x1030
    8a8c: e59fe1c8     	ldr	lr, [pc, #0x1c8]        @ 0x8c5c <_setPresetValues+0x4c8>
    8a90: e1a00004     	mov	r0, r4
    8a94: e08f100e     	add	r1, pc, lr
    8a98: ebffeb31     	bl	0x3764 <.plt+0x68>      @ imm = #-0x533c
    8a9c: e3500000     	cmp	r0, #0
    8aa0: 0a0003f1     	beq	0x9a6c <_setPresetValues+0x12d8> @ imm = #0xfc4
    8aa4: e59fa1b4     	ldr	r10, [pc, #0x1b4]       @ 0x8c60 <_setPresetValues+0x4cc>
    8aa8: e1a00004     	mov	r0, r4
    8aac: e08f100a     	add	r1, pc, r10
    8ab0: ebffeb2b     	bl	0x3764 <.plt+0x68>      @ imm = #-0x5354
    8ab4: e3500000     	cmp	r0, #0
    8ab8: 0a00053d     	beq	0x9fb4 <_setPresetValues+0x1820> @ imm = #0x14f4
    8abc: e59fc1a0     	ldr	r12, [pc, #0x1a0]       @ 0x8c64 <_setPresetValues+0x4d0>
    8ac0: e1a00004     	mov	r0, r4
    8ac4: e08f100c     	add	r1, pc, r12
    8ac8: ebffeb25     	bl	0x3764 <.plt+0x68>      @ imm = #-0x536c
    8acc: e3500000     	cmp	r0, #0
    8ad0: 0a000528     	beq	0x9f78 <_setPresetValues+0x17e4> @ imm = #0x14a0
    8ad4: e59f318c     	ldr	r3, [pc, #0x18c]        @ 0x8c68 <_setPresetValues+0x4d4>
    8ad8: e1a00004     	mov	r0, r4
    8adc: e08f1003     	add	r1, pc, r3
    8ae0: ebffeb1f     	bl	0x3764 <.plt+0x68>      @ imm = #-0x5384
    8ae4: e3500000     	cmp	r0, #0
    8ae8: 0a00051a     	beq	0x9f58 <_setPresetValues+0x17c4> @ imm = #0x1468
    8aec: e59f2178     	ldr	r2, [pc, #0x178]        @ 0x8c6c <_setPresetValues+0x4d8>
    8af0: e1a00004     	mov	r0, r4
    8af4: e08f1002     	add	r1, pc, r2
    8af8: ebffeb19     	bl	0x3764 <.plt+0x68>      @ imm = #-0x539c
    8afc: e3500000     	cmp	r0, #0
    8b00: 0a00050c     	beq	0x9f38 <_setPresetValues+0x17a4> @ imm = #0x1430
    8b04: e59f1164     	ldr	r1, [pc, #0x164]        @ 0x8c70 <_setPresetValues+0x4dc>
    8b08: e1a00004     	mov	r0, r4
    8b0c: e08f1001     	add	r1, pc, r1
    8b10: ebffeb13     	bl	0x3764 <.plt+0x68>      @ imm = #-0x53b4
    8b14: e3500000     	cmp	r0, #0
    8b18: 0a000500     	beq	0x9f20 <_setPresetValues+0x178c> @ imm = #0x1400
    8b1c: e59f0150     	ldr	r0, [pc, #0x150]        @ 0x8c74 <_setPresetValues+0x4e0>
    8b20: e08f1000     	add	r1, pc, r0
    8b24: e1a00004     	mov	r0, r4
    8b28: ebffeb0d     	bl	0x3764 <.plt+0x68>      @ imm = #-0x53cc
    8b2c: e3500000     	cmp	r0, #0
    8b30: 0a0004eb     	beq	0x9ee4 <_setPresetValues+0x1750> @ imm = #0x13ac
    8b34: e59f613c     	ldr	r6, [pc, #0x13c]        @ 0x8c78 <_setPresetValues+0x4e4>
    8b38: e1a00004     	mov	r0, r4
    8b3c: e08f1006     	add	r1, pc, r6
    8b40: ebffeb07     	bl	0x3764 <.plt+0x68>      @ imm = #-0x53e4
    8b44: e3500000     	cmp	r0, #0
    8b48: 0a0004d6     	beq	0x9ea8 <_setPresetValues+0x1714> @ imm = #0x1358
    8b4c: e59fe128     	ldr	lr, [pc, #0x128]        @ 0x8c7c <_setPresetValues+0x4e8>
    8b50: e1a00004     	mov	r0, r4
    8b54: e08f100e     	add	r1, pc, lr
    8b58: ebffeb01     	bl	0x3764 <.plt+0x68>      @ imm = #-0x53fc
    8b5c: e3500000     	cmp	r0, #0
    8b60: 0a0004c1     	beq	0x9e6c <_setPresetValues+0x16d8> @ imm = #0x1304
    8b64: e59fa114     	ldr	r10, [pc, #0x114]       @ 0x8c80 <_setPresetValues+0x4ec>
    8b68: e1a00004     	mov	r0, r4
    8b6c: e08f100a     	add	r1, pc, r10
    8b70: ebffeafb     	bl	0x3764 <.plt+0x68>      @ imm = #-0x5414
    8b74: e3500000     	cmp	r0, #0
    8b78: 0a0004b4     	beq	0x9e50 <_setPresetValues+0x16bc> @ imm = #0x12d0
    8b7c: e59fc100     	ldr	r12, [pc, #0x100]       @ 0x8c84 <_setPresetValues+0x4f0>
    8b80: e1a00004     	mov	r0, r4
    8b84: e08f100c     	add	r1, pc, r12
    8b88: ebffeaf5     	bl	0x3764 <.plt+0x68>      @ imm = #-0x542c
    8b8c: e3500000     	cmp	r0, #0
    8b90: 0a0004a7     	beq	0x9e34 <_setPresetValues+0x16a0> @ imm = #0x129c
    8b94: e59f30ec     	ldr	r3, [pc, #0xec]         @ 0x8c88 <_setPresetValues+0x4f4>
    8b98: e1a00004     	mov	r0, r4
    8b9c: e08f1003     	add	r1, pc, r3
    8ba0: ebffeaef     	bl	0x3764 <.plt+0x68>      @ imm = #-0x5444
    8ba4: e3500000     	cmp	r0, #0
    8ba8: 0a00049a     	beq	0x9e18 <_setPresetValues+0x1684> @ imm = #0x1268
    8bac: e59f20d8     	ldr	r2, [pc, #0xd8]         @ 0x8c8c <_setPresetValues+0x4f8>
    8bb0: e1a00004     	mov	r0, r4
    8bb4: e08f1002     	add	r1, pc, r2
    8bb8: ebffeae9     	bl	0x3764 <.plt+0x68>      @ imm = #-0x545c
    8bbc: e3500000     	cmp	r0, #0
    8bc0: 0a00048d     	beq	0x9dfc <_setPresetValues+0x1668> @ imm = #0x1234
    8bc4: e59f10c4     	ldr	r1, [pc, #0xc4]         @ 0x8c90 <_setPresetValues+0x4fc>
    8bc8: e1a00004     	mov	r0, r4
    8bcc: e08f1001     	add	r1, pc, r1
    8bd0: ebffeae3     	bl	0x3764 <.plt+0x68>      @ imm = #-0x5474
    8bd4: e3500000     	cmp	r0, #0
    8bd8: 0a000480     	beq	0x9de0 <_setPresetValues+0x164c> @ imm = #0x1200
    8bdc: e59f00b0     	ldr	r0, [pc, #0xb0]         @ 0x8c94 <_setPresetValues+0x500>
    8be0: e08f1000     	add	r1, pc, r0
    8be4: ea000045     	b	0x8d00 <_setPresetValues+0x56c> @ imm = #0x114
    8be8: 00 00 00 00  	.word	0x00000000
    8bec: 00 40 9f 40  	.word	0x409f4000
    8bf0: 00 00 00 00  	.word	0x00000000
    8bf4: 00 00 00 00  	.word	0x00000000
    8bf8: e4 c7 00 00  	.word	0x0000c7e4
    8bfc: bc c7 00 00  	.word	0x0000c7bc
    8c00: b8 c7 00 00  	.word	0x0000c7b8
    8c04: ac c7 00 00  	.word	0x0000c7ac
    8c08: ac c7 00 00  	.word	0x0000c7ac
    8c0c: b4 c7 00 00  	.word	0x0000c7b4
    8c10: b0 c7 00 00  	.word	0x0000c7b0
    8c14: b0 c7 00 00  	.word	0x0000c7b0
    8c18: b4 c7 00 00  	.word	0x0000c7b4
    8c1c: ac c7 00 00  	.word	0x0000c7ac
    8c20: a4 c7 00 00  	.word	0x0000c7a4
    8c24: a0 c7 00 00  	.word	0x0000c7a0
    8c28: a4 c7 00 00  	.word	0x0000c7a4
    8c2c: 98 c7 00 00  	.word	0x0000c798
    8c30: 94 c7 00 00  	.word	0x0000c794
    8c34: 90 c7 00 00  	.word	0x0000c790
    8c38: 84 c7 00 00  	.word	0x0000c784
    8c3c: 7c c7 00 00  	.word	0x0000c77c
    8c40: 70 c7 00 00  	.word	0x0000c770
    8c44: 70 c7 00 00  	.word	0x0000c770
    8c48: 74 c7 00 00  	.word	0x0000c774
    8c4c: 6c c7 00 00  	.word	0x0000c76c
    8c50: 64 c7 00 00  	.word	0x0000c764
    8c54: 70 c7 00 00  	.word	0x0000c770
    8c58: 78 c7 00 00  	.word	0x0000c778
    8c5c: 70 c7 00 00  	.word	0x0000c770
    8c60: 64 c7 00 00  	.word	0x0000c764
    8c64: 68 c7 00 00  	.word	0x0000c768
    8c68: 68 c7 00 00  	.word	0x0000c768
    8c6c: 68 c7 00 00  	.word	0x0000c768
    8c70: 64 c7 00 00  	.word	0x0000c764
    8c74: 70 c7 00 00  	.word	0x0000c770
    8c78: 70 c7 00 00  	.word	0x0000c770
    8c7c: 64 c7 00 00  	.word	0x0000c764
    8c80: 6c c7 00 00  	.word	0x0000c76c
    8c84: 68 c7 00 00  	.word	0x0000c768
    8c88: 64 c7 00 00  	.word	0x0000c764
    8c8c: 5c c7 00 00  	.word	0x0000c75c
    8c90: 50 c7 00 00  	.word	0x0000c750
    8c94: 50 c7 00 00  	.word	0x0000c750
    8c98: 38 c6 00 00  	.word	0x0000c638
    8c9c: 3c c6 00 00  	.word	0x0000c63c
    8ca0: 40 c6 00 00  	.word	0x0000c640
    8ca4: 48 c6 00 00  	.word	0x0000c648
    8ca8: 44 c6 00 00  	.word	0x0000c644
    8cac: 48 c6 00 00  	.word	0x0000c648
    8cb0: 4c c6 00 00  	.word	0x0000c64c
    8cb4: 94 c6 00 00  	.word	0x0000c694
    8cb8: 34 c6 00 00  	.word	0x0000c634
    8cbc: 44 c6 00 00  	.word	0x0000c644
    8cc0: 5c c6 00 00  	.word	0x0000c65c
    8cc4: 60 c6 00 00  	.word	0x0000c660
    8cc8: 58 c6 00 00  	.word	0x0000c658
    8ccc: 4c c6 00 00  	.word	0x0000c64c
    8cd0: 48 c6 00 00  	.word	0x0000c648
    8cd4: 80 c1 00 00  	.word	0x0000c180
    8cd8: 30 c6 00 00  	.word	0x0000c630
    8cdc: 48 c6 00 00  	.word	0x0000c648
    8ce0: f0 c0 00 00  	.word	0x0000c0f0
    8ce4: 8c c5 00 00  	.word	0x0000c58c
    8ce8: 84 c5 00 00  	.word	0x0000c584
    8cec: 04 bf 00 00  	.word	0x0000bf04
    8cf0: 0c bf 00 00  	.word	0x0000bf0c
    8cf4: 7c bd 00 00  	.word	0x0000bd7c
    8cf8: 7c bd 00 00  	.word	0x0000bd7c
    8cfc: 40 bd 00 00  	.word	0x0000bd40
    8d00: e1a00004     	mov	r0, r4
    8d04: ebffea96     	bl	0x3764 <.plt+0x68>      @ imm = #-0x55a8
    8d08: e3500000     	cmp	r0, #0
    8d0c: 0a000429     	beq	0x9db8 <_setPresetValues+0x1624> @ imm = #0x10a4
    8d10: e51f6080     	ldr	r6, [pc, #-0x80]        @ 0x8c98 <_setPresetValues+0x504>
    8d14: e1a00004     	mov	r0, r4
    8d18: e08f1006     	add	r1, pc, r6
    8d1c: ebffea90     	bl	0x3764 <.plt+0x68>      @ imm = #-0x55c0
    8d20: e3500000     	cmp	r0, #0
    8d24: 0a00040e     	beq	0x9d64 <_setPresetValues+0x15d0> @ imm = #0x1038
    8d28: e51fe094     	ldr	lr, [pc, #-0x94]        @ 0x8c9c <_setPresetValues+0x508>
    8d2c: e1a00004     	mov	r0, r4
    8d30: e08f100e     	add	r1, pc, lr
    8d34: ebffea8a     	bl	0x3764 <.plt+0x68>      @ imm = #-0x55d8
    8d38: e3500000     	cmp	r0, #0
    8d3c: 0a000401     	beq	0x9d48 <_setPresetValues+0x15b4> @ imm = #0x1004
    8d40: e51fa0a8     	ldr	r10, [pc, #-0xa8]       @ 0x8ca0 <_setPresetValues+0x50c>
    8d44: e1a00004     	mov	r0, r4
    8d48: e08f100a     	add	r1, pc, r10
    8d4c: ebffea84     	bl	0x3764 <.plt+0x68>      @ imm = #-0x55f0
    8d50: e3500000     	cmp	r0, #0
    8d54: 0a0003f5     	beq	0x9d30 <_setPresetValues+0x159c> @ imm = #0xfd4
    8d58: e51fc0bc     	ldr	r12, [pc, #-0xbc]       @ 0x8ca4 <_setPresetValues+0x510>
    8d5c: e1a00004     	mov	r0, r4
    8d60: e08f100c     	add	r1, pc, r12
    8d64: ebffea7e     	bl	0x3764 <.plt+0x68>      @ imm = #-0x5608
    8d68: e3500000     	cmp	r0, #0
    8d6c: 0a0003e3     	beq	0x9d00 <_setPresetValues+0x156c> @ imm = #0xf8c
    8d70: e51f30d0     	ldr	r3, [pc, #-0xd0]        @ 0x8ca8 <_setPresetValues+0x514>
    8d74: e1a00004     	mov	r0, r4
    8d78: e08f1003     	add	r1, pc, r3
    8d7c: ebffea78     	bl	0x3764 <.plt+0x68>      @ imm = #-0x5620
    8d80: e3500000     	cmp	r0, #0
    8d84: 0a0003d6     	beq	0x9ce4 <_setPresetValues+0x1550> @ imm = #0xf58
    8d88: e51f20e4     	ldr	r2, [pc, #-0xe4]        @ 0x8cac <_setPresetValues+0x518>
    8d8c: e1a00004     	mov	r0, r4
    8d90: e08f1002     	add	r1, pc, r2
    8d94: ebffea72     	bl	0x3764 <.plt+0x68>      @ imm = #-0x5638
    8d98: e3500000     	cmp	r0, #0
    8d9c: 0a0003c6     	beq	0x9cbc <_setPresetValues+0x1528> @ imm = #0xf18
    8da0: e51f10f8     	ldr	r1, [pc, #-0xf8]        @ 0x8cb0 <_setPresetValues+0x51c>
    8da4: e1a00004     	mov	r0, r4
    8da8: e08f1001     	add	r1, pc, r1
    8dac: ebffea6c     	bl	0x3764 <.plt+0x68>      @ imm = #-0x5650
    8db0: e3500000     	cmp	r0, #0
    8db4: 0a0003ba     	beq	0x9ca4 <_setPresetValues+0x1510> @ imm = #0xee8
    8db8: e51f010c     	ldr	r0, [pc, #-0x10c]       @ 0x8cb4 <_setPresetValues+0x520>
    8dbc: e08f1000     	add	r1, pc, r0
    8dc0: e1a00004     	mov	r0, r4
    8dc4: ebffea66     	bl	0x3764 <.plt+0x68>      @ imm = #-0x5668
    8dc8: e3500000     	cmp	r0, #0
    8dcc: 0a0003a5     	beq	0x9c68 <_setPresetValues+0x14d4> @ imm = #0xe94
    8dd0: e51f6120     	ldr	r6, [pc, #-0x120]       @ 0x8cb8 <_setPresetValues+0x524>
    8dd4: e1a00004     	mov	r0, r4
    8dd8: e08f1006     	add	r1, pc, r6
    8ddc: ebffea60     	bl	0x3764 <.plt+0x68>      @ imm = #-0x5680
    8de0: e3500000     	cmp	r0, #0
    8de4: 0a000395     	beq	0x9c40 <_setPresetValues+0x14ac> @ imm = #0xe54
    8de8: e51fe134     	ldr	lr, [pc, #-0x134]       @ 0x8cbc <_setPresetValues+0x528>
    8dec: e1a00004     	mov	r0, r4
    8df0: e08f100e     	add	r1, pc, lr
    8df4: ebffea5a     	bl	0x3764 <.plt+0x68>      @ imm = #-0x5698
    8df8: e3500000     	cmp	r0, #0
    8dfc: 0a000357     	beq	0x9b60 <_setPresetValues+0x13cc> @ imm = #0xd5c
    8e00: e51fa148     	ldr	r10, [pc, #-0x148]      @ 0x8cc0 <_setPresetValues+0x52c>
    8e04: e1a00004     	mov	r0, r4
    8e08: e08f100a     	add	r1, pc, r10
    8e0c: ebffea54     	bl	0x3764 <.plt+0x68>      @ imm = #-0x56b0
    8e10: e3500000     	cmp	r0, #0
    8e14: 0a000309     	beq	0x9a40 <_setPresetValues+0x12ac> @ imm = #0xc24
    8e18: e51fc15c     	ldr	r12, [pc, #-0x15c]      @ 0x8cc4 <_setPresetValues+0x530>
    8e1c: e1a00004     	mov	r0, r4
    8e20: e08f100c     	add	r1, pc, r12
    8e24: ebffea4e     	bl	0x3764 <.plt+0x68>      @ imm = #-0x56c8
    8e28: e3500000     	cmp	r0, #0
    8e2c: 0a0002ec     	beq	0x99e4 <_setPresetValues+0x1250> @ imm = #0xbb0
    8e30: e51f3170     	ldr	r3, [pc, #-0x170]       @ 0x8cc8 <_setPresetValues+0x534>
    8e34: e1a00004     	mov	r0, r4
    8e38: e08f1003     	add	r1, pc, r3
    8e3c: ebffea48     	bl	0x3764 <.plt+0x68>      @ imm = #-0x56e0
    8e40: e3500000     	cmp	r0, #0
    8e44: 0a0002dd     	beq	0x99c0 <_setPresetValues+0x122c> @ imm = #0xb74
    8e48: e51f2184     	ldr	r2, [pc, #-0x184]       @ 0x8ccc <_setPresetValues+0x538>
    8e4c: e1a00004     	mov	r0, r4
    8e50: e08f1002     	add	r1, pc, r2
    8e54: ebffea42     	bl	0x3764 <.plt+0x68>      @ imm = #-0x56f8
    8e58: e3500000     	cmp	r0, #0
    8e5c: 0a0002cc     	beq	0x9994 <_setPresetValues+0x1200> @ imm = #0xb30
    8e60: e51f1198     	ldr	r1, [pc, #-0x198]       @ 0x8cd0 <_setPresetValues+0x53c>
    8e64: e1a00004     	mov	r0, r4
    8e68: e08f1001     	add	r1, pc, r1
    8e6c: ebffea3c     	bl	0x3764 <.plt+0x68>      @ imm = #-0x5710
    8e70: e3500000     	cmp	r0, #0
    8e74: 1a00002e     	bne	0x8f34 <_setPresetValues+0x7a0> @ imm = #0xb8
    8e78: eef74ae8     	vcvt.f64.f32	d20, s17
    8e7c: e51f01b0     	ldr	r0, [pc, #-0x1b0]       @ 0x8cd4 <_setPresetValues+0x540>
    8e80: e1a01004     	mov	r1, r4
    8e84: e08f0000     	add	r0, pc, r0
    8e88: ec532b34     	vmov	r2, r3, d20
    8e8c: ebffeb39     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x531c
    8e90: e51b6088     	ldr	r6, [r11, #-0x88]
    8e94: eefd0ae8     	vcvt.s32.f32	s1, s17
    8e98: ee10aa90     	vmov	r10, s1
    8e9c: e586af9c     	str	r10, [r6, #0xf9c]
    8ea0: ea000023     	b	0x8f34 <_setPresetValues+0x7a0> @ imm = #0x8c
    8ea4: e3560001     	cmp	r6, #1
    8ea8: da000013     	ble	0x8efc <_setPresetValues+0x768> @ imm = #0x4c
    8eac: e1a03106     	lsl	r3, r6, #2
    8eb0: e51f11e0     	ldr	r1, [pc, #-0x1e0]       @ 0x8cd8 <_setPresetValues+0x544>
    8eb4: e2830007     	add	r0, r3, #7
    8eb8: e50bd080     	str	sp, [r11, #-0x80]
    8ebc: e3c02007     	bic	r2, r0, #7
    8ec0: e08f1001     	add	r1, pc, r1
    8ec4: e04dd002     	sub	sp, sp, r2
    8ec8: e1a00004     	mov	r0, r4
    8ecc: ebffea24     	bl	0x3764 <.plt+0x68>      @ imm = #-0x5770
    8ed0: e2467001     	sub	r7, r6, #1
    8ed4: e28da008     	add	r10, sp, #8
    8ed8: e3500000     	cmp	r0, #0
    8edc: 0a000178     	beq	0x94c4 <_setPresetValues+0xd30> @ imm = #0x5e0
    8ee0: e1a00004     	mov	r0, r4
    8ee4: e51f4210     	ldr	r4, [pc, #-0x210]       @ 0x8cdc <_setPresetValues+0x548>
    8ee8: e08f1004     	add	r1, pc, r4
    8eec: ebffea1c     	bl	0x3764 <.plt+0x68>      @ imm = #-0x5790
    8ef0: e3500000     	cmp	r0, #0
    8ef4: 0a0000fe     	beq	0x92f4 <_setPresetValues+0xb60> @ imm = #0x3f8
    8ef8: e51bd080     	ldr	sp, [r11, #-0x80]
    8efc: e24bd040     	sub	sp, r11, #64
    8f00: ecbd8b08     	vpop	{d8, d9, d10, d11}
    8f04: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
    8f08: eef76ae8     	vcvt.f64.f32	d22, s17
    8f0c: e51fa234     	ldr	r10, [pc, #-0x234]      @ 0x8ce0 <_setPresetValues+0x54c>
    8f10: e1a01004     	mov	r1, r4
    8f14: e08f000a     	add	r0, pc, r10
    8f18: ec532b36     	vmov	r2, r3, d22
    8f1c: ebffeb15     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x53ac
    8f20: e3a0101a     	mov	r1, #26
    8f24: eefd7ae8     	vcvt.s32.f32	s15, s17
    8f28: e1a00009     	mov	r0, r9
    8f2c: ee172a90     	vmov	r2, s15
    8f30: ebffeaec     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x5450
    8f34: e51f2258     	ldr	r2, [pc, #-0x258]       @ 0x8ce4 <_setPresetValues+0x550>
    8f38: e1a00004     	mov	r0, r4
    8f3c: e08f1002     	add	r1, pc, r2
    8f40: ebffea07     	bl	0x3764 <.plt+0x68>      @ imm = #-0x57e4
    8f44: e3500000     	cmp	r0, #0
    8f48: 0a00000d     	beq	0x8f84 <_setPresetValues+0x7f0> @ imm = #0x34
    8f4c: e51f126c     	ldr	r1, [pc, #-0x26c]       @ 0x8ce8 <_setPresetValues+0x554>
    8f50: e1a00004     	mov	r0, r4
    8f54: e08f1001     	add	r1, pc, r1
    8f58: ebffea01     	bl	0x3764 <.plt+0x68>      @ imm = #-0x57fc
    8f5c: e3500000     	cmp	r0, #0
    8f60: 0a000070     	beq	0x9128 <_setPresetValues+0x994> @ imm = #0x1c0
    8f64: e51b2080     	ldr	r2, [r11, #-0x80]
    8f68: e2877001     	add	r7, r7, #1
    8f6c: e2888010     	add	r8, r8, #16
    8f70: e1570002     	cmp	r7, r2
    8f74: bafffe2c     	blt	0x882c <_setPresetValues+0x98> @ imm = #-0x750
    8f78: e24bd040     	sub	sp, r11, #64
    8f7c: ecbd8b08     	vpop	{d8, d9, d10, d11}
    8f80: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
    8f84: e51b308c     	ldr	r3, [r11, #-0x8c]
    8f88: e51be088     	ldr	lr, [r11, #-0x88]
    8f8c: e3530000     	cmp	r3, #0
    8f90: e58e3fa0     	str	r3, [lr, #0xfa0]
    8f94: dafffff2     	ble	0x8f64 <_setPresetValues+0x7d0> @ imm = #-0x38
    8f98: e51ba090     	ldr	r10, [r11, #-0x90]
    8f9c: e2891d7b     	add	r1, r9, #7872
    8fa0: e2816014     	add	r6, r1, #20
    8fa4: e045200a     	sub	r2, r5, r10
    8fa8: e242c008     	sub	r12, r2, #8
    8fac: e1a001ac     	lsr	r0, r12, #3
    8fb0: e2803001     	add	r3, r0, #1
    8fb4: e213e007     	ands	lr, r3, #7
    8fb8: 0a000029     	beq	0x9064 <_setPresetValues+0x8d0> @ imm = #0xa4
    8fbc: e35e0001     	cmp	lr, #1
    8fc0: 0a000021     	beq	0x904c <_setPresetValues+0x8b8> @ imm = #0x84
    8fc4: e35e0002     	cmp	lr, #2
    8fc8: 0a00001b     	beq	0x903c <_setPresetValues+0x8a8> @ imm = #0x6c
    8fcc: e35e0003     	cmp	lr, #3
    8fd0: 0a000015     	beq	0x902c <_setPresetValues+0x898> @ imm = #0x54
    8fd4: e35e0004     	cmp	lr, #4
    8fd8: 0a00000f     	beq	0x901c <_setPresetValues+0x888> @ imm = #0x3c
    8fdc: e35e0005     	cmp	lr, #5
    8fe0: 0a000009     	beq	0x900c <_setPresetValues+0x878> @ imm = #0x24
    8fe4: e35e0006     	cmp	lr, #6
    8fe8: 0a000003     	beq	0x8ffc <_setPresetValues+0x868> @ imm = #0xc
    8fec: e1a0000a     	mov	r0, r10
    8ff0: e28aa008     	add	r10, r10, #8
    8ff4: ebffea61     	bl	0x3980 <.plt+0x284>     @ imm = #-0x567c
    8ff8: eca60a01     	vstmia	r6!, {s0}
    8ffc: e1a0000a     	mov	r0, r10
    9000: e28aa008     	add	r10, r10, #8
    9004: ebffea5d     	bl	0x3980 <.plt+0x284>     @ imm = #-0x568c
    9008: eca60a01     	vstmia	r6!, {s0}
    900c: e1a0000a     	mov	r0, r10
    9010: e28aa008     	add	r10, r10, #8
    9014: ebffea59     	bl	0x3980 <.plt+0x284>     @ imm = #-0x569c
    9018: eca60a01     	vstmia	r6!, {s0}
    901c: e1a0000a     	mov	r0, r10
    9020: e28aa008     	add	r10, r10, #8
    9024: ebffea55     	bl	0x3980 <.plt+0x284>     @ imm = #-0x56ac
    9028: eca60a01     	vstmia	r6!, {s0}
    902c: e1a0000a     	mov	r0, r10
    9030: e28aa008     	add	r10, r10, #8
    9034: ebffea51     	bl	0x3980 <.plt+0x284>     @ imm = #-0x56bc
    9038: eca60a01     	vstmia	r6!, {s0}
    903c: e1a0000a     	mov	r0, r10
    9040: e28aa008     	add	r10, r10, #8
    9044: ebffea4d     	bl	0x3980 <.plt+0x284>     @ imm = #-0x56cc
    9048: eca60a01     	vstmia	r6!, {s0}
    904c: e1a0000a     	mov	r0, r10
    9050: e28aa008     	add	r10, r10, #8
    9054: ebffea49     	bl	0x3980 <.plt+0x284>     @ imm = #-0x56dc
    9058: e15a0005     	cmp	r10, r5
    905c: eca60a01     	vstmia	r6!, {s0}
    9060: 0affffbf     	beq	0x8f64 <_setPresetValues+0x7d0> @ imm = #-0x104
    9064: e50b7094     	str	r7, [r11, #-0x94]
    9068: e50b4098     	str	r4, [r11, #-0x98]
    906c: e1a0000a     	mov	r0, r10
    9070: e28a7008     	add	r7, r10, #8
    9074: ebffea41     	bl	0x3980 <.plt+0x284>     @ imm = #-0x56fc
    9078: e1a04006     	mov	r4, r6
    907c: e1a00007     	mov	r0, r7
    9080: e2866020     	add	r6, r6, #32
    9084: eca40a01     	vstmia	r4!, {s0}
    9088: ebffea3c     	bl	0x3980 <.plt+0x284>     @ imm = #-0x5710
    908c: e28a0010     	add	r0, r10, #16
    9090: ed060a07     	vstr	s0, [r6, #-28]
    9094: ebffea39     	bl	0x3980 <.plt+0x284>     @ imm = #-0x571c
    9098: e28a0018     	add	r0, r10, #24
    909c: ed840a01     	vstr	s0, [r4, #4]
    90a0: ebffea36     	bl	0x3980 <.plt+0x284>     @ imm = #-0x5728
    90a4: e28a0020     	add	r0, r10, #32
    90a8: ed060a05     	vstr	s0, [r6, #-20]
    90ac: ebffea33     	bl	0x3980 <.plt+0x284>     @ imm = #-0x5734
    90b0: e28a0028     	add	r0, r10, #40
    90b4: ed060a04     	vstr	s0, [r6, #-16]
    90b8: ebffea30     	bl	0x3980 <.plt+0x284>     @ imm = #-0x5740
    90bc: e28a0030     	add	r0, r10, #48
    90c0: ed060a03     	vstr	s0, [r6, #-12]
    90c4: ebffea2d     	bl	0x3980 <.plt+0x284>     @ imm = #-0x574c
    90c8: e28a0038     	add	r0, r10, #56
    90cc: e28aa040     	add	r10, r10, #64
    90d0: ed060a02     	vstr	s0, [r6, #-8]
    90d4: ebffea29     	bl	0x3980 <.plt+0x284>     @ imm = #-0x575c
    90d8: e15a0005     	cmp	r10, r5
    90dc: ed060a01     	vstr	s0, [r6, #-4]
    90e0: 1affffe1     	bne	0x906c <_setPresetValues+0x8d8> @ imm = #-0x7c
    90e4: e51b7094     	ldr	r7, [r11, #-0x94]
    90e8: e51b4098     	ldr	r4, [r11, #-0x98]
    90ec: eaffff9c     	b	0x8f64 <_setPresetValues+0x7d0> @ imm = #-0x190
    90f0: eeb7bae8     	vcvt.f64.f32	d11, s17
    90f4: e51fe410     	ldr	lr, [pc, #-0x410]       @ 0x8cec <_setPresetValues+0x558>
    90f8: e51f6410     	ldr	r6, [pc, #-0x410]       @ 0x8cf0 <_setPresetValues+0x55c>
    90fc: e1a01004     	mov	r1, r4
    9100: e08f000e     	add	r0, pc, lr
    9104: ec532b1b     	vmov	r2, r3, d11
    9108: ebffea9a     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x5598
    910c: e08f0006     	add	r0, pc, r6
    9110: ec532b1b     	vmov	r2, r3, d11
    9114: ebffea97     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x55a4
    9118: e1a00009     	mov	r0, r9
    911c: eeb00a68     	vmov.f32	s0, s17
    9120: ebfffd42     	bl	0x8630 <_setModCVMode>  @ imm = #-0xaf8
    9124: eaffff82     	b	0x8f34 <_setPresetValues+0x7a0> @ imm = #-0x1f8
    9128: e51bc08c     	ldr	r12, [r11, #-0x8c]
    912c: e51b0088     	ldr	r0, [r11, #-0x88]
    9130: e35c0000     	cmp	r12, #0
    9134: e580ced0     	str	r12, [r0, #0xed0]
    9138: daffff89     	ble	0x8f64 <_setPresetValues+0x7d0> @ imm = #-0x1dc
    913c: e51ba090     	ldr	r10, [r11, #-0x90]
    9140: e289ec1e     	add	lr, r9, #7680
    9144: e28e6008     	add	r6, lr, #8
    9148: e045300a     	sub	r3, r5, r10
    914c: e2432008     	sub	r2, r3, #8
    9150: e1a011a2     	lsr	r1, r2, #3
    9154: e281c001     	add	r12, r1, #1
    9158: e21c0007     	ands	r0, r12, #7
    915c: 0a000025     	beq	0x91f8 <_setPresetValues+0xa64> @ imm = #0x94
    9160: e3500001     	cmp	r0, #1
    9164: 0a00001d     	beq	0x91e0 <_setPresetValues+0xa4c> @ imm = #0x74
    9168: e3500002     	cmp	r0, #2
    916c: 0a000017     	beq	0x91d0 <_setPresetValues+0xa3c> @ imm = #0x5c
    9170: e3500003     	cmp	r0, #3
    9174: 0a000011     	beq	0x91c0 <_setPresetValues+0xa2c> @ imm = #0x44
    9178: e3500004     	cmp	r0, #4
    917c: 0a00000b     	beq	0x91b0 <_setPresetValues+0xa1c> @ imm = #0x2c
    9180: e3500005     	cmp	r0, #5
    9184: 0a000005     	beq	0x91a0 <_setPresetValues+0xa0c> @ imm = #0x14
    9188: e3500006     	cmp	r0, #6
    918c: 1a000159     	bne	0x96f8 <_setPresetValues+0xf64> @ imm = #0x564
    9190: e1a0000a     	mov	r0, r10
    9194: e28aa008     	add	r10, r10, #8
    9198: ebffe9f8     	bl	0x3980 <.plt+0x284>     @ imm = #-0x5820
    919c: eca60a01     	vstmia	r6!, {s0}
    91a0: e1a0000a     	mov	r0, r10
    91a4: e28aa008     	add	r10, r10, #8
    91a8: ebffe9f4     	bl	0x3980 <.plt+0x284>     @ imm = #-0x5830
    91ac: eca60a01     	vstmia	r6!, {s0}
    91b0: e1a0000a     	mov	r0, r10
    91b4: e28aa008     	add	r10, r10, #8
    91b8: ebffe9f0     	bl	0x3980 <.plt+0x284>     @ imm = #-0x5840
    91bc: eca60a01     	vstmia	r6!, {s0}
    91c0: e1a0000a     	mov	r0, r10
    91c4: e28aa008     	add	r10, r10, #8
    91c8: ebffe9ec     	bl	0x3980 <.plt+0x284>     @ imm = #-0x5850
    91cc: eca60a01     	vstmia	r6!, {s0}
    91d0: e1a0000a     	mov	r0, r10
    91d4: e28aa008     	add	r10, r10, #8
    91d8: ebffe9e8     	bl	0x3980 <.plt+0x284>     @ imm = #-0x5860
    91dc: eca60a01     	vstmia	r6!, {s0}
    91e0: e1a0000a     	mov	r0, r10
    91e4: e28aa008     	add	r10, r10, #8
    91e8: ebffe9e4     	bl	0x3980 <.plt+0x284>     @ imm = #-0x5870
    91ec: e155000a     	cmp	r5, r10
    91f0: eca60a01     	vstmia	r6!, {s0}
    91f4: 0affff5a     	beq	0x8f64 <_setPresetValues+0x7d0> @ imm = #-0x298
    91f8: e50b7094     	str	r7, [r11, #-0x94]
    91fc: e50b4098     	str	r4, [r11, #-0x98]
    9200: e1a0000a     	mov	r0, r10
    9204: e28a4008     	add	r4, r10, #8
    9208: ebffe9dc     	bl	0x3980 <.plt+0x284>     @ imm = #-0x5890
    920c: e1a07006     	mov	r7, r6
    9210: e1a00004     	mov	r0, r4
    9214: e2866020     	add	r6, r6, #32
    9218: eca70a01     	vstmia	r7!, {s0}
    921c: ebffe9d7     	bl	0x3980 <.plt+0x284>     @ imm = #-0x58a4
    9220: e28a0010     	add	r0, r10, #16
    9224: ed060a07     	vstr	s0, [r6, #-28]
    9228: ebffe9d4     	bl	0x3980 <.plt+0x284>     @ imm = #-0x58b0
    922c: e28a0018     	add	r0, r10, #24
    9230: ed870a01     	vstr	s0, [r7, #4]
    9234: ebffe9d1     	bl	0x3980 <.plt+0x284>     @ imm = #-0x58bc
    9238: e28a0020     	add	r0, r10, #32
    923c: ed060a05     	vstr	s0, [r6, #-20]
    9240: ebffe9ce     	bl	0x3980 <.plt+0x284>     @ imm = #-0x58c8
    9244: e28a0028     	add	r0, r10, #40
    9248: ed060a04     	vstr	s0, [r6, #-16]
    924c: ebffe9cb     	bl	0x3980 <.plt+0x284>     @ imm = #-0x58d4
    9250: e28a0030     	add	r0, r10, #48
    9254: ed060a03     	vstr	s0, [r6, #-12]
    9258: ebffe9c8     	bl	0x3980 <.plt+0x284>     @ imm = #-0x58e0
    925c: e28a0038     	add	r0, r10, #56
    9260: e28aa040     	add	r10, r10, #64
    9264: ed060a02     	vstr	s0, [r6, #-8]
    9268: ebffe9c4     	bl	0x3980 <.plt+0x284>     @ imm = #-0x58f0
    926c: e155000a     	cmp	r5, r10
    9270: ed060a01     	vstr	s0, [r6, #-4]
    9274: 1affffe1     	bne	0x9200 <_setPresetValues+0xa6c> @ imm = #-0x7c
    9278: eaffff99     	b	0x90e4 <_setPresetValues+0x950> @ imm = #-0x19c
    927c: eeb7bae8     	vcvt.f64.f32	d11, s17
    9280: e51fa594     	ldr	r10, [pc, #-0x594]      @ 0x8cf4 <_setPresetValues+0x560>
    9284: e1a01004     	mov	r1, r4
    9288: e08f000a     	add	r0, pc, r10
    928c: ec532b1b     	vmov	r2, r3, d11
    9290: ebffea38     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x5720
    9294: e51f15a4     	ldr	r1, [pc, #-0x5a4]       @ 0x8cf8 <_setPresetValues+0x564>
    9298: ec532b1b     	vmov	r2, r3, d11
    929c: e08f0001     	add	r0, pc, r1
    92a0: ebffea34     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x5730
    92a4: e51bc088     	ldr	r12, [r11, #-0x88]
    92a8: e28c0d37     	add	r0, r12, #3520
    92ac: edc08a03     	vstr	s17, [r0, #12]
    92b0: edc08a02     	vstr	s17, [r0, #8]
    92b4: eaffff1e     	b	0x8f34 <_setPresetValues+0x7a0> @ imm = #-0x388
    92b8: eeb7bae8     	vcvt.f64.f32	d11, s17
    92bc: e51f65c8     	ldr	r6, [pc, #-0x5c8]       @ 0x8cfc <_setPresetValues+0x568>
    92c0: e1a01004     	mov	r1, r4
    92c4: e08f0006     	add	r0, pc, r6
    92c8: ec532b1b     	vmov	r2, r3, d11
    92cc: ebffea29     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x575c
    92d0: e3a01032     	mov	r1, #50
    92d4: eddf3bf7     	vldr	d19, [pc, #988]         @ 0x96b8 <_setPresetValues+0xf24>
    92d8: e1a00009     	mov	r0, r9
    92dc: ed9f7bf7     	vldr	d7, [pc, #988]          @ 0x96c0 <_setPresetValues+0xf2c>
    92e0: ee0b7b23     	vmla.f64	d7, d11, d19
    92e4: eefc1bc7     	vcvt.u32.f64	s3, d7
    92e8: ee112a90     	vmov	r2, s3
    92ec: ebffe9fd     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x580c
    92f0: eaffff0f     	b	0x8f34 <_setPresetValues+0x7a0> @ imm = #-0x3c4
    92f4: e1a02007     	mov	r2, r7
    92f8: e3a0109f     	mov	r1, #159
    92fc: e1a00009     	mov	r0, r9
    9300: ebffe9f8     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x5820
    9304: ed9f0af1     	vldr	s0, [pc, #964]          @ 0x96d0 <_setPresetValues+0xf3c>
    9308: e3a010a0     	mov	r1, #160
    930c: e1a00009     	mov	r0, r9
    9310: ebffe9a3     	bl	0x39a4 <.plt+0x2a8>     @ imm = #-0x5974
    9314: e3a010a1     	mov	r1, #161
    9318: ed9f0aec     	vldr	s0, [pc, #944]          @ 0x96d0 <_setPresetValues+0xf3c>
    931c: e1a00009     	mov	r0, r9
    9320: ebffe99f     	bl	0x39a4 <.plt+0x2a8>     @ imm = #-0x5984
    9324: e1b080c7     	asrs	r8, r7, #1
    9328: 0afffef2     	beq	0x8ef8 <_setPresetValues+0x764> @ imm = #-0x438
    932c: e3180001     	tst	r8, #1
    9330: e59fe39c     	ldr	lr, [pc, #0x39c]        @ 0x96d4 <_setPresetValues+0xf40>
    9334: e2856008     	add	r6, r5, #8
    9338: e28a7004     	add	r7, r10, #4
    933c: e086c208     	add	r12, r6, r8, lsl #4
    9340: e3a040a2     	mov	r4, #162
    9344: e50bc084     	str	r12, [r11, #-0x84]
    9348: e08f800e     	add	r8, pc, lr
    934c: 0a00001a     	beq	0x93bc <_setPresetValues+0xc28> @ imm = #0x68
    9350: e1a00006     	mov	r0, r6
    9354: ebffe989     	bl	0x3980 <.plt+0x284>     @ imm = #-0x59dc
    9358: e1a01004     	mov	r1, r4
    935c: e1a00009     	mov	r0, r9
    9360: e3a040a4     	mov	r4, #164
    9364: ed070a01     	vstr	s0, [r7, #-4]
    9368: eef08a40     	vmov.f32	s17, s0
    936c: ebffe98c     	bl	0x39a4 <.plt+0x2a8>     @ imm = #-0x59d0
    9370: e2860008     	add	r0, r6, #8
    9374: e2856018     	add	r6, r5, #24
    9378: ebffe980     	bl	0x3980 <.plt+0x284>     @ imm = #-0x5a00
    937c: e1a00008     	mov	r0, r8
    9380: eef70ae8     	vcvt.f64.f32	d16, s17
    9384: ec532b30     	vmov	r2, r3, d16
    9388: eeb08a40     	vmov.f32	s16, s0
    938c: eeb70ac0     	vcvt.f64.f32	d0, s0
    9390: ed878a00     	vstr	s16, [r7]
    9394: e28a700c     	add	r7, r10, #12
    9398: ed8d0b00     	vstr	d0, [sp]
    939c: ebffe9f5     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x582c
    93a0: e3a010a3     	mov	r1, #163
    93a4: e1a00009     	mov	r0, r9
    93a8: eeb00a48     	vmov.f32	s0, s16
    93ac: ebffe97c     	bl	0x39a4 <.plt+0x2a8>     @ imm = #-0x5a10
    93b0: e51b5084     	ldr	r5, [r11, #-0x84]
    93b4: e1560005     	cmp	r6, r5
    93b8: 0afffece     	beq	0x8ef8 <_setPresetValues+0x764> @ imm = #-0x4c8
    93bc: e1a00006     	mov	r0, r6
    93c0: e1a05007     	mov	r5, r7
    93c4: ebffe96d     	bl	0x3980 <.plt+0x284>     @ imm = #-0x5a4c
    93c8: e1a01004     	mov	r1, r4
    93cc: e1a00009     	mov	r0, r9
    93d0: e286a010     	add	r10, r6, #16
    93d4: ed070a01     	vstr	s0, [r7, #-4]
    93d8: eeb09a40     	vmov.f32	s18, s0
    93dc: ebffe970     	bl	0x39a4 <.plt+0x2a8>     @ imm = #-0x5a40
    93e0: e2860008     	add	r0, r6, #8
    93e4: ebffe965     	bl	0x3980 <.plt+0x284>     @ imm = #-0x5a6c
    93e8: e1a00008     	mov	r0, r8
    93ec: eeb71ac9     	vcvt.f64.f32	d1, s18
    93f0: ec532b11     	vmov	r2, r3, d1
    93f4: ee101a10     	vmov	r1, s0
    93f8: eeb72ac0     	vcvt.f64.f32	d2, s0
    93fc: eef09a40     	vmov.f32	s19, s0
    9400: e4851008     	str	r1, [r5], #8
    9404: ed8d2b00     	vstr	d2, [sp]
    9408: ebffe9da     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x5898
    940c: e2843001     	add	r3, r4, #1
    9410: e1a00009     	mov	r0, r9
    9414: e6ef1073     	uxtb	r1, r3
    9418: eeb00a69     	vmov.f32	s0, s19
    941c: ebffe960     	bl	0x39a4 <.plt+0x2a8>     @ imm = #-0x5a80
    9420: e2842002     	add	r2, r4, #2
    9424: e1a0000a     	mov	r0, r10
    9428: e6ef4072     	uxtb	r4, r2
    942c: ebffe953     	bl	0x3980 <.plt+0x284>     @ imm = #-0x5ab4
    9430: e1a00009     	mov	r0, r9
    9434: e1a01004     	mov	r1, r4
    9438: ed050a01     	vstr	s0, [r5, #-4]
    943c: eeb0aa40     	vmov.f32	s20, s0
    9440: ebffe957     	bl	0x39a4 <.plt+0x2a8>     @ imm = #-0x5aa4
    9444: e2860018     	add	r0, r6, #24
    9448: e2866020     	add	r6, r6, #32
    944c: ebffe94b     	bl	0x3980 <.plt+0x284>     @ imm = #-0x5ad4
    9450: e1a00008     	mov	r0, r8
    9454: eeb73aca     	vcvt.f64.f32	d3, s20
    9458: ec532b13     	vmov	r2, r3, d3
    945c: eeb74ac0     	vcvt.f64.f32	d4, s0
    9460: ed870a02     	vstr	s0, [r7, #8]
    9464: e2857008     	add	r7, r5, #8
    9468: eef0aa40     	vmov.f32	s21, s0
    946c: ed8d4b00     	vstr	d4, [sp]
    9470: ebffe9c0     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x5900
    9474: e284c001     	add	r12, r4, #1
    9478: e1a00009     	mov	r0, r9
    947c: eeb00a6a     	vmov.f32	s0, s21
    9480: e6ef107c     	uxtb	r1, r12
    9484: ebffe946     	bl	0x39a4 <.plt+0x2a8>     @ imm = #-0x5ae8
    9488: e51b5084     	ldr	r5, [r11, #-0x84]
    948c: e2840002     	add	r0, r4, #2
    9490: e1560005     	cmp	r6, r5
    9494: e6ef4070     	uxtb	r4, r0
    9498: 1affffc7     	bne	0x93bc <_setPresetValues+0xc28> @ imm = #-0xe4
    949c: eafffe95     	b	0x8ef8 <_setPresetValues+0x764> @ imm = #-0x5ac
    94a0: eeb7bae8     	vcvt.f64.f32	d11, s17
    94a4: e59f322c     	ldr	r3, [pc, #0x22c]        @ 0x96d8 <_setPresetValues+0xf44>
    94a8: e1a01004     	mov	r1, r4
    94ac: e08f0003     	add	r0, pc, r3
    94b0: ec532b1b     	vmov	r2, r3, d11
    94b4: ebffe9af     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x5944
    94b8: e3a01033     	mov	r1, #51
    94bc: eddf3b7d     	vldr	d19, [pc, #500]         @ 0x96b8 <_setPresetValues+0xf24>
    94c0: eaffff84     	b	0x92d8 <_setPresetValues+0xb44> @ imm = #-0x1f0
    94c4: e1a02007     	mov	r2, r7
    94c8: e3a0109f     	mov	r1, #159
    94cc: e1a00009     	mov	r0, r9
    94d0: ebffe984     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x59f0
    94d4: ed9f0a7d     	vldr	s0, [pc, #500]          @ 0x96d0 <_setPresetValues+0xf3c>
    94d8: e3a010a0     	mov	r1, #160
    94dc: e1a00009     	mov	r0, r9
    94e0: ebffe92f     	bl	0x39a4 <.plt+0x2a8>     @ imm = #-0x5b44
    94e4: e3a010a1     	mov	r1, #161
    94e8: ed9f0a78     	vldr	s0, [pc, #480]          @ 0x96d0 <_setPresetValues+0xf3c>
    94ec: e1a00009     	mov	r0, r9
    94f0: ebffe92b     	bl	0x39a4 <.plt+0x2a8>     @ imm = #-0x5b54
    94f4: e1b080c7     	asrs	r8, r7, #1
    94f8: 0afffe7e     	beq	0x8ef8 <_setPresetValues+0x764> @ imm = #-0x608
    94fc: e3180001     	tst	r8, #1
    9500: e59f11d4     	ldr	r1, [pc, #0x1d4]        @ 0x96dc <_setPresetValues+0xf48>
    9504: e2856008     	add	r6, r5, #8
    9508: e28a7004     	add	r7, r10, #4
    950c: e086e208     	add	lr, r6, r8, lsl #4
    9510: e3a040a2     	mov	r4, #162
    9514: e50be084     	str	lr, [r11, #-0x84]
    9518: e08f8001     	add	r8, pc, r1
    951c: 0a00001a     	beq	0x958c <_setPresetValues+0xdf8> @ imm = #0x68
    9520: e1a00006     	mov	r0, r6
    9524: ebffe915     	bl	0x3980 <.plt+0x284>     @ imm = #-0x5bac
    9528: e1a01004     	mov	r1, r4
    952c: e1a00009     	mov	r0, r9
    9530: e3a040a4     	mov	r4, #164
    9534: ed070a01     	vstr	s0, [r7, #-4]
    9538: eeb0ba40     	vmov.f32	s22, s0
    953c: ebffe918     	bl	0x39a4 <.plt+0x2a8>     @ imm = #-0x5ba0
    9540: e2860008     	add	r0, r6, #8
    9544: e2856018     	add	r6, r5, #24
    9548: ebffe90c     	bl	0x3980 <.plt+0x284>     @ imm = #-0x5bd0
    954c: e1a00008     	mov	r0, r8
    9550: eeb75acb     	vcvt.f64.f32	d5, s22
    9554: ec532b15     	vmov	r2, r3, d5
    9558: eeb76ac0     	vcvt.f64.f32	d6, s0
    955c: ed870a00     	vstr	s0, [r7]
    9560: e28a700c     	add	r7, r10, #12
    9564: eef0ba40     	vmov.f32	s23, s0
    9568: ed8d6b00     	vstr	d6, [sp]
    956c: ebffe981     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x59fc
    9570: e3a010a3     	mov	r1, #163
    9574: e1a00009     	mov	r0, r9
    9578: eeb00a6b     	vmov.f32	s0, s23
    957c: ebffe908     	bl	0x39a4 <.plt+0x2a8>     @ imm = #-0x5be0
    9580: e51ba084     	ldr	r10, [r11, #-0x84]
    9584: e15a0006     	cmp	r10, r6
    9588: 0afffe5a     	beq	0x8ef8 <_setPresetValues+0x764> @ imm = #-0x698
    958c: e1a00006     	mov	r0, r6
    9590: e1a05007     	mov	r5, r7
    9594: ebffe8f9     	bl	0x3980 <.plt+0x284>     @ imm = #-0x5c1c
    9598: e1a01004     	mov	r1, r4
    959c: e1a00009     	mov	r0, r9
    95a0: e286a010     	add	r10, r6, #16
    95a4: ed070a01     	vstr	s0, [r7, #-4]
    95a8: eef08a40     	vmov.f32	s17, s0
    95ac: ebffe8fc     	bl	0x39a4 <.plt+0x2a8>     @ imm = #-0x5c10
    95b0: e2860008     	add	r0, r6, #8
    95b4: ebffe8f1     	bl	0x3980 <.plt+0x284>     @ imm = #-0x5c3c
    95b8: e1a00008     	mov	r0, r8
    95bc: eeb77ae8     	vcvt.f64.f32	d7, s17
    95c0: ec532b17     	vmov	r2, r3, d7
    95c4: ee10ca10     	vmov	r12, s0
    95c8: eef71ac0     	vcvt.f64.f32	d17, s0
    95cc: eeb08a40     	vmov.f32	s16, s0
    95d0: e485c008     	str	r12, [r5], #8
    95d4: edcd1b00     	vstr	d17, [sp]
    95d8: ebffe966     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x5a68
    95dc: e2843001     	add	r3, r4, #1
    95e0: e1a00009     	mov	r0, r9
    95e4: e6ef1073     	uxtb	r1, r3
    95e8: eeb00a48     	vmov.f32	s0, s16
    95ec: ebffe8ec     	bl	0x39a4 <.plt+0x2a8>     @ imm = #-0x5c50
    95f0: e2842002     	add	r2, r4, #2
    95f4: e1a0000a     	mov	r0, r10
    95f8: e6ef4072     	uxtb	r4, r2
    95fc: ebffe8df     	bl	0x3980 <.plt+0x284>     @ imm = #-0x5c84
    9600: e1a00009     	mov	r0, r9
    9604: e1a01004     	mov	r1, r4
    9608: ed050a01     	vstr	s0, [r5, #-4]
    960c: eeb09a40     	vmov.f32	s18, s0
    9610: ebffe8e3     	bl	0x39a4 <.plt+0x2a8>     @ imm = #-0x5c74
    9614: e2860018     	add	r0, r6, #24
    9618: e2866020     	add	r6, r6, #32
    961c: ebffe8d7     	bl	0x3980 <.plt+0x284>     @ imm = #-0x5ca4
    9620: e1a00008     	mov	r0, r8
    9624: eef72ac9     	vcvt.f64.f32	d18, s18
    9628: ec532b32     	vmov	r2, r3, d18
    962c: eef73ac0     	vcvt.f64.f32	d19, s0
    9630: ed870a02     	vstr	s0, [r7, #8]
    9634: e2857008     	add	r7, r5, #8
    9638: eef09a40     	vmov.f32	s19, s0
    963c: edcd3b00     	vstr	d19, [sp]
    9640: ebffe94c     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x5ad0
    9644: e2841001     	add	r1, r4, #1
    9648: e1a00009     	mov	r0, r9
    964c: eeb00a69     	vmov.f32	s0, s19
    9650: e6ef1071     	uxtb	r1, r1
    9654: ebffe8d2     	bl	0x39a4 <.plt+0x2a8>     @ imm = #-0x5cb8
    9658: e51b5084     	ldr	r5, [r11, #-0x84]
    965c: e2840002     	add	r0, r4, #2
    9660: e1550006     	cmp	r5, r6
    9664: e6ef4070     	uxtb	r4, r0
    9668: 1affffc7     	bne	0x958c <_setPresetValues+0xdf8> @ imm = #-0xe4
    966c: eafffe21     	b	0x8ef8 <_setPresetValues+0x764> @ imm = #-0x77c
    9670: eeb7bae8     	vcvt.f64.f32	d11, s17
    9674: e59f2064     	ldr	r2, [pc, #0x64]         @ 0x96e0 <_setPresetValues+0xf4c>
    9678: e1a01004     	mov	r1, r4
    967c: e08f0002     	add	r0, pc, r2
    9680: ec532b1b     	vmov	r2, r3, d11
    9684: ebffe93b     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x5b14
    9688: e3a01032     	mov	r1, #50
    968c: eddf3b0d     	vldr	d19, [pc, #52]          @ 0x96c8 <_setPresetValues+0xf34>
    9690: eaffff10     	b	0x92d8 <_setPresetValues+0xb44> @ imm = #-0x3c0
    9694: eeb7bae8     	vcvt.f64.f32	d11, s17
    9698: e59fe044     	ldr	lr, [pc, #0x44]         @ 0x96e4 <_setPresetValues+0xf50>
    969c: e1a01004     	mov	r1, r4
    96a0: e08f000e     	add	r0, pc, lr
    96a4: ec532b1b     	vmov	r2, r3, d11
    96a8: ebffe932     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x5b38
    96ac: e3a01034     	mov	r1, #52
    96b0: eddf3b00     	vldr	d19, [pc]               @ 0x96b8 <_setPresetValues+0xf24>
    96b4: eaffff07     	b	0x92d8 <_setPresetValues+0xb44> @ imm = #-0x3e4
    96b8: 9a 99 99 99  	.word	0x9999999a
    96bc: 99 99 79 c0  	.word	0xc0799999
    96c0: 00 00 00 00  	.word	0x00000000
    96c4: 00 00 a0 40  	.word	0x40a00000
    96c8: 7b 14 ae 47  	.word	0x47ae147b
    96cc: e1 7a 34 c0  	.word	0xc0347ae1
    96d0: 00 00 00 00  	.word	0x00000000
    96d4: bc c1 00 00  	.word	0x0000c1bc
    96d8: 58 bb 00 00  	.word	0x0000bb58
    96dc: ec bf 00 00  	.word	0x0000bfec
    96e0: 88 b9 00 00  	.word	0x0000b988
    96e4: 64 b9 00 00  	.word	0x0000b964
    96e8: ec b8 00 00  	.word	0x0000b8ec
    96ec: bc b8 00 00  	.word	0x0000b8bc
    96f0: 9c b8 00 00  	.word	0x0000b89c
    96f4: 6c b8 00 00  	.word	0x0000b86c
    96f8: e1a0000a     	mov	r0, r10
    96fc: e28aa008     	add	r10, r10, #8
    9700: ebffe89e     	bl	0x3980 <.plt+0x284>     @ imm = #-0x5d88
    9704: eca60a01     	vstmia	r6!, {s0}
    9708: eafffea0     	b	0x9190 <_setPresetValues+0x9fc> @ imm = #-0x580
    970c: eef74ae8     	vcvt.f64.f32	d20, s17
    9710: e51fa030     	ldr	r10, [pc, #-0x30]       @ 0x96e8 <_setPresetValues+0xf54>
    9714: e1a01004     	mov	r1, r4
    9718: e08f000a     	add	r0, pc, r10
    971c: eefd8ae8     	vcvt.s32.f32	s17, s17
    9720: ec532b34     	vmov	r2, r3, d20
    9724: ebffe913     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x5bb4
    9728: e3a01040     	mov	r1, #64
    972c: e1a00009     	mov	r0, r9
    9730: ee182a90     	vmov	r2, s17
    9734: ebffe8eb     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x5c54
    9738: eafffdfd     	b	0x8f34 <_setPresetValues+0x7a0> @ imm = #-0x80c
    973c: eeb7bae8     	vcvt.f64.f32	d11, s17
    9740: e51f005c     	ldr	r0, [pc, #-0x5c]        @ 0x96ec <_setPresetValues+0xf58>
    9744: e1a01004     	mov	r1, r4
    9748: e08f0000     	add	r0, pc, r0
    974c: ec532b1b     	vmov	r2, r3, d11
    9750: ebffe908     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x5be0
    9754: e3a01034     	mov	r1, #52
    9758: eaffffcb     	b	0x968c <_setPresetValues+0xef8> @ imm = #-0xd4
    975c: eef75ae8     	vcvt.f64.f32	d21, s17
    9760: e51fc078     	ldr	r12, [pc, #-0x78]       @ 0x96f0 <_setPresetValues+0xf5c>
    9764: e1a01004     	mov	r1, r4
    9768: e08f000c     	add	r0, pc, r12
    976c: ec532b35     	vmov	r2, r3, d21
    9770: ebffe900     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x5c00
    9774: e3a0103f     	mov	r1, #63
    9778: eefd0ae8     	vcvt.s32.f32	s1, s17
    977c: e1a00009     	mov	r0, r9
    9780: ee102a90     	vmov	r2, s1
    9784: ebffe8d7     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x5ca4
    9788: eafffde9     	b	0x8f34 <_setPresetValues+0x7a0> @ imm = #-0x85c
    978c: eeb7bae8     	vcvt.f64.f32	d11, s17
    9790: e51f60a4     	ldr	r6, [pc, #-0xa4]        @ 0x96f4 <_setPresetValues+0xf60>
    9794: e1a01004     	mov	r1, r4
    9798: e08f0006     	add	r0, pc, r6
    979c: ec532b1b     	vmov	r2, r3, d11
    97a0: ebffe8f4     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x5c30
    97a4: e3a01035     	mov	r1, #53
    97a8: eddf3bf6     	vldr	d19, [pc, #984]         @ 0x9b88 <_setPresetValues+0x13f4>
    97ac: eafffec9     	b	0x92d8 <_setPresetValues+0xb44> @ imm = #-0x4dc
    97b0: eeb7bae8     	vcvt.f64.f32	d11, s17
    97b4: e59f23d4     	ldr	r2, [pc, #0x3d4]        @ 0x9b90 <_setPresetValues+0x13fc>
    97b8: e1a01004     	mov	r1, r4
    97bc: e08f0002     	add	r0, pc, r2
    97c0: ec532b1b     	vmov	r2, r3, d11
    97c4: ebffe8eb     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x5c54
    97c8: e3a01035     	mov	r1, #53
    97cc: eaffffae     	b	0x968c <_setPresetValues+0xef8> @ imm = #-0x148
    97d0: eef72ae8     	vcvt.f64.f32	d18, s17
    97d4: e59fe3b8     	ldr	lr, [pc, #0x3b8]        @ 0x9b94 <_setPresetValues+0x1400>
    97d8: e1a01004     	mov	r1, r4
    97dc: e08f000e     	add	r0, pc, lr
    97e0: eefcbae8     	vcvt.u32.f32	s23, s17
    97e4: ec532b32     	vmov	r2, r3, d18
    97e8: ebffe8e2     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x5c78
    97ec: e3a01028     	mov	r1, #40
    97f0: e1a00009     	mov	r0, r9
    97f4: ee1b3a90     	vmov	r3, s23
    97f8: ed4bba25     	vstr	s23, [r11, #-148]
    97fc: e6ef2073     	uxtb	r2, r3
    9800: e5c9204c     	strb	r2, [r9, #0x4c]
    9804: ebffe8b7     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x5d24
    9808: eafffdc9     	b	0x8f34 <_setPresetValues+0x7a0> @ imm = #-0x8dc
    980c: eef71ae8     	vcvt.f64.f32	d17, s17
    9810: e59f0380     	ldr	r0, [pc, #0x380]        @ 0x9b98 <_setPresetValues+0x1404>
    9814: e1a01004     	mov	r1, r4
    9818: e08f0000     	add	r0, pc, r0
    981c: ec532b31     	vmov	r2, r3, d17
    9820: ebffe8d4     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x5cb0
    9824: e3a01025     	mov	r1, #37
    9828: eebd8ae8     	vcvt.s32.f32	s16, s17
    982c: e1a00009     	mov	r0, r9
    9830: ee182a10     	vmov	r2, s16
    9834: ebffe8ab     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x5d54
    9838: eafffdbd     	b	0x8f34 <_setPresetValues+0x7a0> @ imm = #-0x90c
    983c: eeb77ae8     	vcvt.f64.f32	d7, s17
    9840: e59fc354     	ldr	r12, [pc, #0x354]       @ 0x9b9c <_setPresetValues+0x1408>
    9844: e1a01004     	mov	r1, r4
    9848: e08f000c     	add	r0, pc, r12
    984c: ec532b17     	vmov	r2, r3, d7
    9850: eafffff2     	b	0x9820 <_setPresetValues+0x108c> @ imm = #-0x38
    9854: eeb75ae8     	vcvt.f64.f32	d5, s17
    9858: e59f6340     	ldr	r6, [pc, #0x340]        @ 0x9ba0 <_setPresetValues+0x140c>
    985c: e1a01004     	mov	r1, r4
    9860: e08f0006     	add	r0, pc, r6
    9864: ec532b15     	vmov	r2, r3, d5
    9868: ebffe8c2     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x5cf8
    986c: eebc6ae8     	vcvt.u32.f32	s12, s17
    9870: ee16aa10     	vmov	r10, s12
    9874: e5c9a02a     	strb	r10, [r9, #0x2a]
    9878: eafffdad     	b	0x8f34 <_setPresetValues+0x7a0> @ imm = #-0x94c
    987c: eeb74ae8     	vcvt.f64.f32	d4, s17
    9880: e59fe31c     	ldr	lr, [pc, #0x31c]        @ 0x9ba4 <_setPresetValues+0x1410>
    9884: e1a01004     	mov	r1, r4
    9888: e08f000e     	add	r0, pc, lr
    988c: eebcbae8     	vcvt.u32.f32	s22, s17
    9890: ec532b14     	vmov	r2, r3, d4
    9894: ebffe8b7     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x5d24
    9898: ee1b1a10     	vmov	r1, s22
    989c: e5c9102a     	strb	r1, [r9, #0x2a]
    98a0: eafffda3     	b	0x8f34 <_setPresetValues+0x7a0> @ imm = #-0x974
    98a4: eeb71ae8     	vcvt.f64.f32	d1, s17
    98a8: e59f02f8     	ldr	r0, [pc, #0x2f8]        @ 0x9ba8 <_setPresetValues+0x1414>
    98ac: e1a01004     	mov	r1, r4
    98b0: e08f0000     	add	r0, pc, r0
    98b4: ec532b11     	vmov	r2, r3, d1
    98b8: ebffe8ae     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x5d48
    98bc: eebc2ae8     	vcvt.u32.f32	s4, s17
    98c0: ee122a10     	vmov	r2, s4
    98c4: ed0b2a25     	vstr	s4, [r11, #-148]
    98c8: e6ef1072     	uxtb	r1, r2
    98cc: e281600f     	add	r6, r1, #15
    98d0: e1a0a186     	lsl	r10, r6, #3
    98d4: e089c00a     	add	r12, r9, r10
    98d8: e28a3003     	add	r3, r10, #3
    98dc: e0890003     	add	r0, r9, r3
    98e0: e58900b4     	str	r0, [r9, #0xb4]
    98e4: e5dc200a     	ldrb	r2, [r12, #0xa]
    98e8: ee032a10     	vmov	s6, r2
    98ec: eeb80a43     	vcvt.f32.u32	s0, s6
    98f0: ebffe81f     	bl	0x3974 <.plt+0x278>     @ imm = #-0x5f84
    98f4: eafffd8e     	b	0x8f34 <_setPresetValues+0x7a0> @ imm = #-0x9c8
    98f8: eef70ae8     	vcvt.f64.f32	d16, s17
    98fc: e59fc2a8     	ldr	r12, [pc, #0x2a8]       @ 0x9bac <_setPresetValues+0x1418>
    9900: e1a01004     	mov	r1, r4
    9904: e08f000c     	add	r0, pc, r12
    9908: ec532b30     	vmov	r2, r3, d16
    990c: ebffe899     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x5d9c
    9910: e3a01046     	mov	r1, #70
    9914: eeb00a68     	vmov.f32	s0, s17
    9918: e1a00009     	mov	r0, r9
    991c: ebffe820     	bl	0x39a4 <.plt+0x2a8>     @ imm = #-0x5f80
    9920: eafffd83     	b	0x8f34 <_setPresetValues+0x7a0> @ imm = #-0x9f4
    9924: eeb70ae8     	vcvt.f64.f32	d0, s17
    9928: e59fe280     	ldr	lr, [pc, #0x280]        @ 0x9bb0 <_setPresetValues+0x141c>
    992c: e1a01004     	mov	r1, r4
    9930: e08f000e     	add	r0, pc, lr
    9934: ec532b10     	vmov	r2, r3, d0
    9938: ebffe88e     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x5dc8
    993c: eefc6ae8     	vcvt.u32.f32	s13, s17
    9940: ee163a90     	vmov	r3, s13
    9944: e5c930c4     	strb	r3, [r9, #0xc4]
    9948: eafffd79     	b	0x8f34 <_setPresetValues+0x7a0> @ imm = #-0xa1c
    994c: eef7eae8     	vcvt.f64.f32	d30, s17
    9950: e59f625c     	ldr	r6, [pc, #0x25c]        @ 0x9bb4 <_setPresetValues+0x1420>
    9954: e1a01004     	mov	r1, r4
    9958: e08f0006     	add	r0, pc, r6
    995c: ec532b3e     	vmov	r2, r3, d30
    9960: ebffe884     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x5df0
    9964: e3a01047     	mov	r1, #71
    9968: edc98a3b     	vstr	s17, [r9, #236]
    996c: e1a00009     	mov	r0, r9
    9970: eeb00a68     	vmov.f32	s0, s17
    9974: ebffe80a     	bl	0x39a4 <.plt+0x2a8>     @ imm = #-0x5fd8
    9978: eafffd6d     	b	0x8f34 <_setPresetValues+0x7a0> @ imm = #-0xa4c
    997c: eef7fae8     	vcvt.f64.f32	d31, s17
    9980: e59fa230     	ldr	r10, [pc, #0x230]       @ 0x9bb8 <_setPresetValues+0x1424>
    9984: e1a01004     	mov	r1, r4
    9988: e08f000a     	add	r0, pc, r10
    998c: ec532b3f     	vmov	r2, r3, d31
    9990: eafffff2     	b	0x9960 <_setPresetValues+0x11cc> @ imm = #-0x38
    9994: eef75ae8     	vcvt.f64.f32	d21, s17
    9998: e59fe21c     	ldr	lr, [pc, #0x21c]        @ 0x9bbc <_setPresetValues+0x1428>
    999c: e1a01004     	mov	r1, r4
    99a0: e08f000e     	add	r0, pc, lr
    99a4: ec532b35     	vmov	r2, r3, d21
    99a8: ebffe872     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x5e38
    99ac: e51bc088     	ldr	r12, [r11, #-0x88]
    99b0: eefd1ae8     	vcvt.s32.f32	s3, s17
    99b4: ee113a90     	vmov	r3, s3
    99b8: e58c3fa4     	str	r3, [r12, #0xfa4]
    99bc: eafffd5c     	b	0x8f34 <_setPresetValues+0x7a0> @ imm = #-0xa90
    99c0: eef76ae8     	vcvt.f64.f32	d22, s17
    99c4: e59f01f4     	ldr	r0, [pc, #0x1f4]        @ 0x9bc0 <_setPresetValues+0x142c>
    99c8: e1a01004     	mov	r1, r4
    99cc: e08f0000     	add	r0, pc, r0
    99d0: ec532b36     	vmov	r2, r3, d22
    99d4: ebffe867     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x5e64
    99d8: e51b209c     	ldr	r2, [r11, #-0x9c]
    99dc: edc28a00     	vstr	s17, [r2]
    99e0: eafffd53     	b	0x8f34 <_setPresetValues+0x7a0> @ imm = #-0xab4
    99e4: eef77ae8     	vcvt.f64.f32	d23, s17
    99e8: e59f61d4     	ldr	r6, [pc, #0x1d4]        @ 0x9bc4 <_setPresetValues+0x1430>
    99ec: e1a01004     	mov	r1, r4
    99f0: e08f0006     	add	r0, pc, r6
    99f4: ec532b37     	vmov	r2, r3, d23
    99f8: ebffe85e     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x5e88
    99fc: e51bc088     	ldr	r12, [r11, #-0x88]
    9a00: eefd7ae8     	vcvt.s32.f32	s15, s17
    9a04: ee171a90     	vmov	r1, s15
    9a08: ed4b7a25     	vstr	s15, [r11, #-148]
    9a0c: e6efa071     	uxtb	r10, r1
    9a10: e5ccadf5     	strb	r10, [r12, #0xdf5]
    9a14: e35a0000     	cmp	r10, #0
    9a18: 0a000002     	beq	0x9a28 <_setPresetValues+0x1294> @ imm = #0x8
    9a1c: eeb00b4a     	vmov.f64	d0, d10
    9a20: e5990100     	ldr	r0, [r9, #0x100]
    9a24: ebffe7c0     	bl	0x392c <.plt+0x230>     @ imm = #-0x6100
    9a28: e51be088     	ldr	lr, [r11, #-0x88]
    9a2c: e3a0108c     	mov	r1, #140
    9a30: e1a00009     	mov	r0, r9
    9a34: e5de2df5     	ldrb	r2, [lr, #0xdf5]
    9a38: ebffe82a     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x5f58
    9a3c: eafffd3c     	b	0x8f34 <_setPresetValues+0x7a0> @ imm = #-0xb10
    9a40: eef78ae8     	vcvt.f64.f32	d24, s17
    9a44: e59f017c     	ldr	r0, [pc, #0x17c]        @ 0x9bc8 <_setPresetValues+0x1434>
    9a48: e1a01004     	mov	r1, r4
    9a4c: e08f0000     	add	r0, pc, r0
    9a50: ec532b38     	vmov	r2, r3, d24
    9a54: ebffe847     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x5ee4
    9a58: e51b2088     	ldr	r2, [r11, #-0x88]
    9a5c: eefc2ae8     	vcvt.u32.f32	s5, s17
    9a60: ee123a90     	vmov	r3, s5
    9a64: e5c23df4     	strb	r3, [r2, #0xdf4]
    9a68: eafffd31     	b	0x8f34 <_setPresetValues+0x7a0> @ imm = #-0xb3c
    9a6c: eef7aae8     	vcvt.f64.f32	d26, s17
    9a70: e59fa154     	ldr	r10, [pc, #0x154]       @ 0x9bcc <_setPresetValues+0x1438>
    9a74: e1a01004     	mov	r1, r4
    9a78: e08f000a     	add	r0, pc, r10
    9a7c: ec532b3a     	vmov	r2, r3, d26
    9a80: ebffe83c     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x5f10
    9a84: e3a01024     	mov	r1, #36
    9a88: eefc7ae8     	vcvt.u32.f32	s15, s17
    9a8c: e1a00009     	mov	r0, r9
    9a90: ee172a90     	vmov	r2, s15
    9a94: ed4b7a25     	vstr	s15, [r11, #-148]
    9a98: e6ef2072     	uxtb	r2, r2
    9a9c: e5c92054     	strb	r2, [r9, #0x54]
    9aa0: ebffe810     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x5fc0
    9aa4: eef72a00     	vmov.f32	s5, #1.000000e+00
    9aa8: eef48ae2     	vcmpe.f32	s17, s5
    9aac: eef1fa10     	vmrs	APSR_nzcv, fpscr
    9ab0: 4a00000e     	bmi	0x9af0 <_setPresetValues+0x135c> @ imm = #0x38
    9ab4: e3a01000     	mov	r1, #0
    9ab8: e5891058     	str	r1, [r9, #0x58]
    9abc: eafffd1c     	b	0x8f34 <_setPresetValues+0x7a0> @ imm = #-0xb90
    9ac0: eef7bae8     	vcvt.f64.f32	d27, s17
    9ac4: e59fc104     	ldr	r12, [pc, #0x104]       @ 0x9bd0 <_setPresetValues+0x143c>
    9ac8: e1a01004     	mov	r1, r4
    9acc: e08f000c     	add	r0, pc, r12
    9ad0: ec532b3b     	vmov	r2, r3, d27
    9ad4: ebffe827     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x5f64
    9ad8: e3a0103e     	mov	r1, #62
    9adc: eefc3ae8     	vcvt.u32.f32	s7, s17
    9ae0: e1a00009     	mov	r0, r9
    9ae4: ee132a90     	vmov	r2, s7
    9ae8: ebffe7fe     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x6008
    9aec: eafffd10     	b	0x8f34 <_setPresetValues+0x7a0> @ imm = #-0xbc0
    9af0: eeb00b49     	vmov.f64	d0, d9
    9af4: e59900dc     	ldr	r0, [r9, #0xdc]
    9af8: ebffe78b     	bl	0x392c <.plt+0x230>     @ imm = #-0x61d4
    9afc: eaffffec     	b	0x9ab4 <_setPresetValues+0x1320> @ imm = #-0x50
    9b00: eef7cae8     	vcvt.f64.f32	d28, s17
    9b04: e59fe0c8     	ldr	lr, [pc, #0xc8]         @ 0x9bd4 <_setPresetValues+0x1440>
    9b08: e1a01004     	mov	r1, r4
    9b0c: e08f000e     	add	r0, pc, lr
    9b10: ec532b3c     	vmov	r2, r3, d28
    9b14: ebffe817     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x5fa4
    9b18: e3a0103d     	mov	r1, #61
    9b1c: eefc4ae8     	vcvt.u32.f32	s9, s17
    9b20: e1a00009     	mov	r0, r9
    9b24: ee142a90     	vmov	r2, s9
    9b28: ebffe7ee     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x6048
    9b2c: eafffd00     	b	0x8f34 <_setPresetValues+0x7a0> @ imm = #-0xc00
    9b30: eef7dae8     	vcvt.f64.f32	d29, s17
    9b34: e59f009c     	ldr	r0, [pc, #0x9c]         @ 0x9bd8 <_setPresetValues+0x1444>
    9b38: e1a01004     	mov	r1, r4
    9b3c: e08f0000     	add	r0, pc, r0
    9b40: ec532b3d     	vmov	r2, r3, d29
    9b44: ebffe80b     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x5fd4
    9b48: e3a0103c     	mov	r1, #60
    9b4c: eefc5ae8     	vcvt.u32.f32	s11, s17
    9b50: e1a00009     	mov	r0, r9
    9b54: ee152a90     	vmov	r2, s11
    9b58: ebffe7e2     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x6078
    9b5c: eafffcf4     	b	0x8f34 <_setPresetValues+0x7a0> @ imm = #-0xc30
    9b60: eef79ae8     	vcvt.f64.f32	d25, s17
    9b64: e59f6070     	ldr	r6, [pc, #0x70]         @ 0x9bdc <_setPresetValues+0x1448>
    9b68: e1a01004     	mov	r1, r4
    9b6c: e08f0006     	add	r0, pc, r6
    9b70: ec532b39     	vmov	r2, r3, d25
    9b74: ebffe7ff     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x6004
    9b78: eefc3ae8     	vcvt.u32.f32	s7, s17
    9b7c: ee131a90     	vmov	r1, s7
    9b80: e5c91029     	strb	r1, [r9, #0x29]
    9b84: eafffcea     	b	0x8f34 <_setPresetValues+0x7a0> @ imm = #-0xc58
    9b88: 9a 99 99 99  	.word	0x9999999a
    9b8c: 99 99 79 c0  	.word	0xc0799999
    9b90: 48 b8 00 00  	.word	0x0000b848
    9b94: 28 b8 00 00  	.word	0x0000b828
    9b98: ec b7 00 00  	.word	0x0000b7ec
    9b9c: bc b7 00 00  	.word	0x0000b7bc
    9ba0: a4 b7 00 00  	.word	0x0000b7a4
    9ba4: 7c b7 00 00  	.word	0x0000b77c
    9ba8: 54 b7 00 00  	.word	0x0000b754
    9bac: 00 b7 00 00  	.word	0x0000b700
    9bb0: d4 b6 00 00  	.word	0x0000b6d4
    9bb4: ac b6 00 00  	.word	0x0000b6ac
    9bb8: 7c b6 00 00  	.word	0x0000b67c
    9bbc: 64 b6 00 00  	.word	0x0000b664
    9bc0: 38 b6 00 00  	.word	0x0000b638
    9bc4: 14 b6 00 00  	.word	0x0000b614
    9bc8: b8 b5 00 00  	.word	0x0000b5b8
    9bcc: 8c b5 00 00  	.word	0x0000b58c
    9bd0: 38 b5 00 00  	.word	0x0000b538
    9bd4: f8 b4 00 00  	.word	0x0000b4f8
    9bd8: c8 b4 00 00  	.word	0x0000b4c8
    9bdc: 98 b4 00 00  	.word	0x0000b498
    9be0: b8 b3 00 00  	.word	0x0000b3b8
    9be4: 90 b3 00 00  	.word	0x0000b390
    9be8: 54 b3 00 00  	.word	0x0000b354
    9bec: 3c b3 00 00  	.word	0x0000b33c
    9bf0: 14 b3 00 00  	.word	0x0000b314
    9bf4: f8 b2 00 00  	.word	0x0000b2f8
    9bf8: c8 b2 00 00  	.word	0x0000b2c8
    9bfc: b0 b2 00 00  	.word	0x0000b2b0
    9c00: 94 b2 00 00  	.word	0x0000b294
    9c04: 40 b2 00 00  	.word	0x0000b240
    9c08: 18 b2 00 00  	.word	0x0000b218
    9c0c: fc b1 00 00  	.word	0x0000b1fc
    9c10: e0 b1 00 00  	.word	0x0000b1e0
    9c14: c4 b1 00 00  	.word	0x0000b1c4
    9c18: a8 b1 00 00  	.word	0x0000b1a8
    9c1c: 8c b1 00 00  	.word	0x0000b18c
    9c20: 50 b1 00 00  	.word	0x0000b150
    9c24: 14 b1 00 00  	.word	0x0000b114
    9c28: d8 b0 00 00  	.word	0x0000b0d8
    9c2c: c0 b0 00 00  	.word	0x0000b0c0
    9c30: a0 b0 00 00  	.word	0x0000b0a0
    9c34: 80 b0 00 00  	.word	0x0000b080
    9c38: 44 b0 00 00  	.word	0x0000b044
    9c3c: 00 00 00 00  	.word	0x00000000
    9c40: eef7aae8     	vcvt.f64.f32	d26, s17
    9c44: e51fa06c     	ldr	r10, [pc, #-0x6c]       @ 0x9be0 <_setPresetValues+0x144c>
    9c48: e1a01004     	mov	r1, r4
    9c4c: e08f000a     	add	r0, pc, r10
    9c50: ec532b3a     	vmov	r2, r3, d26
    9c54: ebffe7c7     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x60e4
    9c58: eefc4ae8     	vcvt.u32.f32	s9, s17
    9c5c: ee14ca90     	vmov	r12, s9
    9c60: e5c9c029     	strb	r12, [r9, #0x29]
    9c64: eafffcb2     	b	0x8f34 <_setPresetValues+0x7a0> @ imm = #-0xd38
    9c68: eef7bae8     	vcvt.f64.f32	d27, s17
    9c6c: e51fe090     	ldr	lr, [pc, #-0x90]        @ 0x9be4 <_setPresetValues+0x1450>
    9c70: e1a01004     	mov	r1, r4
    9c74: e08f000e     	add	r0, pc, lr
    9c78: ec532b3b     	vmov	r2, r3, d27
    9c7c: ebffe7bd     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x610c
    9c80: eefc5ae8     	vcvt.u32.f32	s11, s17
    9c84: ee153a90     	vmov	r3, s11
    9c88: ed4b5a25     	vstr	s11, [r11, #-148]
    9c8c: e6ef2073     	uxtb	r2, r3
    9c90: e5c92028     	strb	r2, [r9, #0x28]
    9c94: e3520000     	cmp	r2, #0
    9c98: 13a02000     	movne	r2, #0
    9c9c: 15c92025     	strbne	r2, [r9, #0x25]
    9ca0: eafffca3     	b	0x8f34 <_setPresetValues+0x7a0> @ imm = #-0xd74
    9ca4: eef7cae8     	vcvt.f64.f32	d28, s17
    9ca8: e51f00c8     	ldr	r0, [pc, #-0xc8]        @ 0x9be8 <_setPresetValues+0x1454>
    9cac: e1a01004     	mov	r1, r4
    9cb0: e08f0000     	add	r0, pc, r0
    9cb4: ec532b3c     	vmov	r2, r3, d28
    9cb8: eaffffef     	b	0x9c7c <_setPresetValues+0x14e8> @ imm = #-0x44
    9cbc: eef7dae8     	vcvt.f64.f32	d29, s17
    9cc0: e51f60dc     	ldr	r6, [pc, #-0xdc]        @ 0x9bec <_setPresetValues+0x1458>
    9cc4: e1a01004     	mov	r1, r4
    9cc8: e08f0006     	add	r0, pc, r6
    9ccc: ec532b3d     	vmov	r2, r3, d29
    9cd0: ebffe7a8     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x6160
    9cd4: eefc6ae8     	vcvt.u32.f32	s13, s17
    9cd8: ee161a90     	vmov	r1, s13
    9cdc: e5c91025     	strb	r1, [r9, #0x25]
    9ce0: eafffc93     	b	0x8f34 <_setPresetValues+0x7a0> @ imm = #-0xdb4
    9ce4: eef7eae8     	vcvt.f64.f32	d30, s17
    9ce8: e51fa100     	ldr	r10, [pc, #-0x100]      @ 0x9bf0 <_setPresetValues+0x145c>
    9cec: e1a01004     	mov	r1, r4
    9cf0: e08f000a     	add	r0, pc, r10
    9cf4: ec532b3e     	vmov	r2, r3, d30
    9cf8: ebffe79e     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x6188
    9cfc: eafffc8c     	b	0x8f34 <_setPresetValues+0x7a0> @ imm = #-0xdd0
    9d00: eef7fae8     	vcvt.f64.f32	d31, s17
    9d04: e51fc118     	ldr	r12, [pc, #-0x118]      @ 0x9bf4 <_setPresetValues+0x1460>
    9d08: e1a01004     	mov	r1, r4
    9d0c: e08f000c     	add	r0, pc, r12
    9d10: ec532b3f     	vmov	r2, r3, d31
    9d14: ebffe797     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x61a4
    9d18: e3a01036     	mov	r1, #54
    9d1c: eebd0ae8     	vcvt.s32.f32	s0, s17
    9d20: e1a00009     	mov	r0, r9
    9d24: ee102a10     	vmov	r2, s0
    9d28: ebffe76e     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x6248
    9d2c: eafffc80     	b	0x8f34 <_setPresetValues+0x7a0> @ imm = #-0xe00
    9d30: eef70ae8     	vcvt.f64.f32	d16, s17
    9d34: e51fe144     	ldr	lr, [pc, #-0x144]       @ 0x9bf8 <_setPresetValues+0x1464>
    9d38: e1a01004     	mov	r1, r4
    9d3c: e08f000e     	add	r0, pc, lr
    9d40: ec532b30     	vmov	r2, r3, d16
    9d44: eafffff2     	b	0x9d14 <_setPresetValues+0x1580> @ imm = #-0x38
    9d48: eeb71ae8     	vcvt.f64.f32	d1, s17
    9d4c: e51f0158     	ldr	r0, [pc, #-0x158]       @ 0x9bfc <_setPresetValues+0x1468>
    9d50: e1a01004     	mov	r1, r4
    9d54: e08f0000     	add	r0, pc, r0
    9d58: ec532b11     	vmov	r2, r3, d1
    9d5c: ebffe785     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x61ec
    9d60: eafffc73     	b	0x8f34 <_setPresetValues+0x7a0> @ imm = #-0xe34
    9d64: eeb72ae8     	vcvt.f64.f32	d2, s17
    9d68: e51f6170     	ldr	r6, [pc, #-0x170]       @ 0x9c00 <_setPresetValues+0x146c>
    9d6c: e1a01004     	mov	r1, r4
    9d70: e08f0006     	add	r0, pc, r6
    9d74: ec532b12     	vmov	r2, r3, d2
    9d78: ebffe77e     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x6208
    9d7c: e1a00009     	mov	r0, r9
    9d80: eebc3ae8     	vcvt.u32.f32	s6, s17
    9d84: ee133a10     	vmov	r3, s6
    9d88: ed0b3a25     	vstr	s6, [r11, #-148]
    9d8c: e6ef2073     	uxtb	r2, r3
    9d90: e5c92027     	strb	r2, [r9, #0x27]
    9d94: e3520000     	cmp	r2, #0
    9d98: 1eb10a0c     	vmovne.f32	s0, #7.000000e+00
    9d9c: 0d1f0a5a     	vldreq	s0, [pc, #-360]         @ 0x9c3c <_setPresetValues+0x14a8>
    9da0: ebffe7d1     	bl	0x3cec <.plt+0x5f0>     @ imm = #-0x60bc
    9da4: e5d92027     	ldrb	r2, [r9, #0x27]
    9da8: e3a01022     	mov	r1, #34
    9dac: e1a00009     	mov	r0, r9
    9db0: ebffe74c     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x62d0
    9db4: eafffc5e     	b	0x8f34 <_setPresetValues+0x7a0> @ imm = #-0xe88
    9db8: eeb74ae8     	vcvt.f64.f32	d4, s17
    9dbc: e51fa1c0     	ldr	r10, [pc, #-0x1c0]      @ 0x9c04 <_setPresetValues+0x1470>
    9dc0: e1a01004     	mov	r1, r4
    9dc4: e08f000a     	add	r0, pc, r10
    9dc8: eebcbae8     	vcvt.u32.f32	s22, s17
    9dcc: ec532b14     	vmov	r2, r3, d4
    9dd0: ebffe768     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x6260
    9dd4: ee1b1a10     	vmov	r1, s22
    9dd8: e5c91027     	strb	r1, [r9, #0x27]
    9ddc: eafffc54     	b	0x8f34 <_setPresetValues+0x7a0> @ imm = #-0xeb0
    9de0: eeb75ae8     	vcvt.f64.f32	d5, s17
    9de4: e51fc1e4     	ldr	r12, [pc, #-0x1e4]      @ 0x9c08 <_setPresetValues+0x1474>
    9de8: e1a01004     	mov	r1, r4
    9dec: e08f000c     	add	r0, pc, r12
    9df0: ec532b15     	vmov	r2, r3, d5
    9df4: ebffe75f     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x6284
    9df8: eafffc4d     	b	0x8f34 <_setPresetValues+0x7a0> @ imm = #-0xecc
    9dfc: eeb76ae8     	vcvt.f64.f32	d6, s17
    9e00: e51fe1fc     	ldr	lr, [pc, #-0x1fc]       @ 0x9c0c <_setPresetValues+0x1478>
    9e04: e1a01004     	mov	r1, r4
    9e08: e08f000e     	add	r0, pc, lr
    9e0c: ec532b16     	vmov	r2, r3, d6
    9e10: ebffe758     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x62a0
    9e14: eafffc46     	b	0x8f34 <_setPresetValues+0x7a0> @ imm = #-0xee8
    9e18: eeb78ae8     	vcvt.f64.f32	d8, s17
    9e1c: e51f0214     	ldr	r0, [pc, #-0x214]       @ 0x9c10 <_setPresetValues+0x147c>
    9e20: e1a01004     	mov	r1, r4
    9e24: e08f0000     	add	r0, pc, r0
    9e28: ec532b18     	vmov	r2, r3, d8
    9e2c: ebffe751     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x62bc
    9e30: eafffc3f     	b	0x8f34 <_setPresetValues+0x7a0> @ imm = #-0xf04
    9e34: eeb77ae8     	vcvt.f64.f32	d7, s17
    9e38: e51f622c     	ldr	r6, [pc, #-0x22c]       @ 0x9c14 <_setPresetValues+0x1480>
    9e3c: e1a01004     	mov	r1, r4
    9e40: e08f0006     	add	r0, pc, r6
    9e44: ec532b17     	vmov	r2, r3, d7
    9e48: ebffe74a     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x62d8
    9e4c: eafffc38     	b	0x8f34 <_setPresetValues+0x7a0> @ imm = #-0xf20
    9e50: eef71ae8     	vcvt.f64.f32	d17, s17
    9e54: e51fa244     	ldr	r10, [pc, #-0x244]      @ 0x9c18 <_setPresetValues+0x1484>
    9e58: e1a01004     	mov	r1, r4
    9e5c: e08f000a     	add	r0, pc, r10
    9e60: ec532b31     	vmov	r2, r3, d17
    9e64: ebffe743     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x62f4
    9e68: eafffc31     	b	0x8f34 <_setPresetValues+0x7a0> @ imm = #-0xf3c
    9e6c: eef72ae8     	vcvt.f64.f32	d18, s17
    9e70: e51fc25c     	ldr	r12, [pc, #-0x25c]      @ 0x9c1c <_setPresetValues+0x1488>
    9e74: e1a01004     	mov	r1, r4
    9e78: e08f000c     	add	r0, pc, r12
    9e7c: eefcbae8     	vcvt.u32.f32	s23, s17
    9e80: ec532b32     	vmov	r2, r3, d18
    9e84: ebffe73b     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x6314
    9e88: e3a0102f     	mov	r1, #47
    9e8c: e1a00009     	mov	r0, r9
    9e90: ee1b3a90     	vmov	r3, s23
    9e94: ed4bba25     	vstr	s23, [r11, #-148]
    9e98: e6ef2073     	uxtb	r2, r3
    9e9c: e5c92067     	strb	r2, [r9, #0x67]
    9ea0: ebffe710     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x63c0
    9ea4: eafffc22     	b	0x8f34 <_setPresetValues+0x7a0> @ imm = #-0xf78
    9ea8: eef73ae8     	vcvt.f64.f32	d19, s17
    9eac: e51fe294     	ldr	lr, [pc, #-0x294]       @ 0x9c20 <_setPresetValues+0x148c>
    9eb0: e1a01004     	mov	r1, r4
    9eb4: e08f000e     	add	r0, pc, lr
    9eb8: eefc8ae8     	vcvt.u32.f32	s17, s17
    9ebc: ec532b33     	vmov	r2, r3, d19
    9ec0: ebffe72c     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x6350
    9ec4: e3a0102e     	mov	r1, #46
    9ec8: e1a00009     	mov	r0, r9
    9ecc: ee182a90     	vmov	r2, s17
    9ed0: ed4b8a25     	vstr	s17, [r11, #-148]
    9ed4: e6ef2072     	uxtb	r2, r2
    9ed8: e5c92066     	strb	r2, [r9, #0x66]
    9edc: ebffe701     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x63fc
    9ee0: eafffc13     	b	0x8f34 <_setPresetValues+0x7a0> @ imm = #-0xfb4
    9ee4: eef74ae8     	vcvt.f64.f32	d20, s17
    9ee8: e51f02cc     	ldr	r0, [pc, #-0x2cc]       @ 0x9c24 <_setPresetValues+0x1490>
    9eec: e1a01004     	mov	r1, r4
    9ef0: e08f0000     	add	r0, pc, r0
    9ef4: ec532b34     	vmov	r2, r3, d20
    9ef8: ebffe71e     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x6388
    9efc: e3a01030     	mov	r1, #48
    9f00: eefc0ae8     	vcvt.u32.f32	s1, s17
    9f04: e1a00009     	mov	r0, r9
    9f08: ee10aa90     	vmov	r10, s1
    9f0c: ed4b0a25     	vstr	s1, [r11, #-148]
    9f10: e6ef207a     	uxtb	r2, r10
    9f14: e5c92065     	strb	r2, [r9, #0x65]
    9f18: ebffe6f2     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x6438
    9f1c: eafffc04     	b	0x8f34 <_setPresetValues+0x7a0> @ imm = #-0xff0
    9f20: eef75ae8     	vcvt.f64.f32	d21, s17
    9f24: e51f6304     	ldr	r6, [pc, #-0x304]       @ 0x9c28 <_setPresetValues+0x1494>
    9f28: e1a01004     	mov	r1, r4
    9f2c: e08f0006     	add	r0, pc, r6
    9f30: ec532b35     	vmov	r2, r3, d21
    9f34: eaffffef     	b	0x9ef8 <_setPresetValues+0x1764> @ imm = #-0x44
    9f38: eef76ae8     	vcvt.f64.f32	d22, s17
    9f3c: e51fc318     	ldr	r12, [pc, #-0x318]      @ 0x9c2c <_setPresetValues+0x1498>
    9f40: e1a01004     	mov	r1, r4
    9f44: e08f000c     	add	r0, pc, r12
    9f48: ec532b36     	vmov	r2, r3, d22
    9f4c: ebffe709     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x63dc
    9f50: edc98a18     	vstr	s17, [r9, #96]
    9f54: eafffbf6     	b	0x8f34 <_setPresetValues+0x7a0> @ imm = #-0x1028
    9f58: eef77ae8     	vcvt.f64.f32	d23, s17
    9f5c: e51fe334     	ldr	lr, [pc, #-0x334]       @ 0x9c30 <_setPresetValues+0x149c>
    9f60: e1a01004     	mov	r1, r4
    9f64: e08f000e     	add	r0, pc, lr
    9f68: ec532b37     	vmov	r2, r3, d23
    9f6c: ebffe701     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x63fc
    9f70: edc98a18     	vstr	s17, [r9, #96]
    9f74: eafffbee     	b	0x8f34 <_setPresetValues+0x7a0> @ imm = #-0x1048
    9f78: eef78ae8     	vcvt.f64.f32	d24, s17
    9f7c: e51f0350     	ldr	r0, [pc, #-0x350]       @ 0x9c34 <_setPresetValues+0x14a0>
    9f80: e1a01004     	mov	r1, r4
    9f84: e08f0000     	add	r0, pc, r0
    9f88: ec532b38     	vmov	r2, r3, d24
    9f8c: ebffe6f9     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x641c
    9f90: e3a0102d     	mov	r1, #45
    9f94: eefc1ae8     	vcvt.u32.f32	s3, s17
    9f98: e1a00009     	mov	r0, r9
    9f9c: ee113a90     	vmov	r3, s3
    9fa0: ed4b1a25     	vstr	s3, [r11, #-148]
    9fa4: e6ef2073     	uxtb	r2, r3
    9fa8: e5c92064     	strb	r2, [r9, #0x64]
    9fac: ebffe6cd     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x64cc
    9fb0: eafffbdf     	b	0x8f34 <_setPresetValues+0x7a0> @ imm = #-0x1084
    9fb4: eef79ae8     	vcvt.f64.f32	d25, s17
    9fb8: e51f6388     	ldr	r6, [pc, #-0x388]       @ 0x9c38 <_setPresetValues+0x14a4>
    9fbc: e1a01004     	mov	r1, r4
    9fc0: e08f0006     	add	r0, pc, r6
    9fc4: ec532b39     	vmov	r2, r3, d25
    9fc8: eaffffef     	b	0x9f8c <_setPresetValues+0x17f8> @ imm = #-0x44

