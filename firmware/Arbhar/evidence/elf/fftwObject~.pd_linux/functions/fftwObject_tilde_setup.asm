00003d40 <fftwObject_tilde_setup>:
    3d40: e59f0234     	ldr	r0, [pc, #0x234]        @ 0x3f7c <fftwObject_tilde_setup+0x23c>
    3d44: e92d4070     	push	{r4, r5, r6, lr}
    3d48: e08f0000     	add	r0, pc, r0
    3d4c: e24dd008     	sub	sp, sp, #8
    3d50: e3a05000     	mov	r5, #0
    3d54: ebfff234     	bl	0x62c <.plt+0x14>       @ imm = #-0x3730
    3d58: e59f2220     	ldr	r2, [pc, #0x220]        @ 0x3f80 <fftwObject_tilde_setup+0x240>
    3d5c: e59f1220     	ldr	r1, [pc, #0x220]        @ 0x3f84 <fftwObject_tilde_setup+0x244>
    3d60: e3013058     	movw	r3, #0x1058
    3d64: e08f2002     	add	r2, pc, r2
    3d68: e58d5004     	str	r5, [sp, #0x4]
    3d6c: e58d5000     	str	r5, [sp]
    3d70: e3403066     	movt	r3, #0x66
    3d74: e08f1001     	add	r1, pc, r1
    3d78: e59f4208     	ldr	r4, [pc, #0x208]        @ 0x3f88 <fftwObject_tilde_setup+0x248>
    3d7c: ebfff25d     	bl	0x6f8 <.plt+0xe0>       @ imm = #-0x368c
    3d80: e3a0101c     	mov	r1, #28
    3d84: e08f4004     	add	r4, pc, r4
    3d88: e5840000     	str	r0, [r4]
    3d8c: ebfff25f     	bl	0x710 <.plt+0xf8>       @ imm = #-0x3684
    3d90: e59f31f4     	ldr	r3, [pc, #0x1f4]        @ 0x3f8c <fftwObject_tilde_setup+0x24c>
    3d94: e5946000     	ldr	r6, [r4]
    3d98: e08f0003     	add	r0, pc, r3
    3d9c: ebfff222     	bl	0x62c <.plt+0x14>       @ imm = #-0x3778
    3da0: e59fc1e8     	ldr	r12, [pc, #0x1e8]       @ 0x3f90 <fftwObject_tilde_setup+0x250>
    3da4: e58d5000     	str	r5, [sp]
    3da8: e3a0300b     	mov	r3, #11
    3dac: e08f100c     	add	r1, pc, r12
    3db0: e1a02000     	mov	r2, r0
    3db4: e1a00006     	mov	r0, r6
    3db8: ebfff257     	bl	0x71c <.plt+0x104>      @ imm = #-0x36a4
    3dbc: e59f01d0     	ldr	r0, [pc, #0x1d0]        @ 0x3f94 <fftwObject_tilde_setup+0x254>
    3dc0: e5946000     	ldr	r6, [r4]
    3dc4: e08f0000     	add	r0, pc, r0
    3dc8: ebfff217     	bl	0x62c <.plt+0x14>       @ imm = #-0x37a4
    3dcc: e59f21c4     	ldr	r2, [pc, #0x1c4]        @ 0x3f98 <fftwObject_tilde_setup+0x258>
    3dd0: e58d5000     	str	r5, [sp]
    3dd4: e3a03001     	mov	r3, #1
    3dd8: e08f1002     	add	r1, pc, r2
    3ddc: e1a02000     	mov	r2, r0
    3de0: e1a00006     	mov	r0, r6
    3de4: ebfff24c     	bl	0x71c <.plt+0x104>      @ imm = #-0x36d0
    3de8: e59f11ac     	ldr	r1, [pc, #0x1ac]        @ 0x3f9c <fftwObject_tilde_setup+0x25c>
    3dec: e5946000     	ldr	r6, [r4]
    3df0: e08f0001     	add	r0, pc, r1
    3df4: ebfff20c     	bl	0x62c <.plt+0x14>       @ imm = #-0x37d0
    3df8: e59fc1a0     	ldr	r12, [pc, #0x1a0]       @ 0x3fa0 <fftwObject_tilde_setup+0x260>
    3dfc: e58d5000     	str	r5, [sp]
    3e00: e3a03001     	mov	r3, #1
    3e04: e08f100c     	add	r1, pc, r12
    3e08: e1a02000     	mov	r2, r0
    3e0c: e1a00006     	mov	r0, r6
    3e10: ebfff241     	bl	0x71c <.plt+0x104>      @ imm = #-0x36fc
    3e14: e59f3188     	ldr	r3, [pc, #0x188]        @ 0x3fa4 <fftwObject_tilde_setup+0x264>
    3e18: e5946000     	ldr	r6, [r4]
    3e1c: e08f0003     	add	r0, pc, r3
    3e20: ebfff201     	bl	0x62c <.plt+0x14>       @ imm = #-0x37fc
    3e24: e59f217c     	ldr	r2, [pc, #0x17c]        @ 0x3fa8 <fftwObject_tilde_setup+0x268>
    3e28: e58d5000     	str	r5, [sp]
    3e2c: e3a03001     	mov	r3, #1
    3e30: e08f1002     	add	r1, pc, r2
    3e34: e1a02000     	mov	r2, r0
    3e38: e1a00006     	mov	r0, r6
    3e3c: ebfff236     	bl	0x71c <.plt+0x104>      @ imm = #-0x3728
    3e40: e59f0164     	ldr	r0, [pc, #0x164]        @ 0x3fac <fftwObject_tilde_setup+0x26c>
    3e44: e5946000     	ldr	r6, [r4]
    3e48: e08f0000     	add	r0, pc, r0
    3e4c: ebfff1f6     	bl	0x62c <.plt+0x14>       @ imm = #-0x3828
    3e50: e59f1158     	ldr	r1, [pc, #0x158]        @ 0x3fb0 <fftwObject_tilde_setup+0x270>
    3e54: e58d5000     	str	r5, [sp]
    3e58: e3a03001     	mov	r3, #1
    3e5c: e08f1001     	add	r1, pc, r1
    3e60: e1a02000     	mov	r2, r0
    3e64: e1a00006     	mov	r0, r6
    3e68: ebfff22b     	bl	0x71c <.plt+0x104>      @ imm = #-0x3754
    3e6c: e59fc140     	ldr	r12, [pc, #0x140]       @ 0x3fb4 <fftwObject_tilde_setup+0x274>
    3e70: e5946000     	ldr	r6, [r4]
    3e74: e08f000c     	add	r0, pc, r12
    3e78: ebfff1eb     	bl	0x62c <.plt+0x14>       @ imm = #-0x3854
    3e7c: e59f2134     	ldr	r2, [pc, #0x134]        @ 0x3fb8 <fftwObject_tilde_setup+0x278>
    3e80: e58d5000     	str	r5, [sp]
    3e84: e3a03001     	mov	r3, #1
    3e88: e08f1002     	add	r1, pc, r2
    3e8c: e1a02000     	mov	r2, r0
    3e90: e1a00006     	mov	r0, r6
    3e94: ebfff220     	bl	0x71c <.plt+0x104>      @ imm = #-0x3780
    3e98: e59f311c     	ldr	r3, [pc, #0x11c]        @ 0x3fbc <fftwObject_tilde_setup+0x27c>
    3e9c: e5946000     	ldr	r6, [r4]
    3ea0: e08f0003     	add	r0, pc, r3
    3ea4: ebfff1e0     	bl	0x62c <.plt+0x14>       @ imm = #-0x3880
    3ea8: e59f1110     	ldr	r1, [pc, #0x110]        @ 0x3fc0 <fftwObject_tilde_setup+0x280>
    3eac: e58d5000     	str	r5, [sp]
    3eb0: e3a03001     	mov	r3, #1
    3eb4: e08f1001     	add	r1, pc, r1
    3eb8: e1a02000     	mov	r2, r0
    3ebc: e1a00006     	mov	r0, r6
    3ec0: ebfff215     	bl	0x71c <.plt+0x104>      @ imm = #-0x37ac
    3ec4: e59f00f8     	ldr	r0, [pc, #0xf8]         @ 0x3fc4 <fftwObject_tilde_setup+0x284>
    3ec8: e5946000     	ldr	r6, [r4]
    3ecc: e08f0000     	add	r0, pc, r0
    3ed0: ebfff1d5     	bl	0x62c <.plt+0x14>       @ imm = #-0x38ac
    3ed4: e59fc0ec     	ldr	r12, [pc, #0xec]        @ 0x3fc8 <fftwObject_tilde_setup+0x288>
    3ed8: e58d5000     	str	r5, [sp]
    3edc: e3a03001     	mov	r3, #1
    3ee0: e08f100c     	add	r1, pc, r12
    3ee4: e1a02000     	mov	r2, r0
    3ee8: e1a00006     	mov	r0, r6
    3eec: ebfff20a     	bl	0x71c <.plt+0x104>      @ imm = #-0x37d8
    3ef0: e59f20d4     	ldr	r2, [pc, #0xd4]         @ 0x3fcc <fftwObject_tilde_setup+0x28c>
    3ef4: e5946000     	ldr	r6, [r4]
    3ef8: e08f0002     	add	r0, pc, r2
    3efc: ebfff1ca     	bl	0x62c <.plt+0x14>       @ imm = #-0x38d8
    3f00: e59f10c8     	ldr	r1, [pc, #0xc8]         @ 0x3fd0 <fftwObject_tilde_setup+0x290>
    3f04: e58d5000     	str	r5, [sp]
    3f08: e3a03001     	mov	r3, #1
    3f0c: e08f1001     	add	r1, pc, r1
    3f10: e1a02000     	mov	r2, r0
    3f14: e1a00006     	mov	r0, r6
    3f18: ebfff1ff     	bl	0x71c <.plt+0x104>      @ imm = #-0x3804
    3f1c: e59f30b0     	ldr	r3, [pc, #0xb0]         @ 0x3fd4 <fftwObject_tilde_setup+0x294>
    3f20: e5946000     	ldr	r6, [r4]
    3f24: e08f0003     	add	r0, pc, r3
    3f28: ebfff1bf     	bl	0x62c <.plt+0x14>       @ imm = #-0x3904
    3f2c: e59fc0a4     	ldr	r12, [pc, #0xa4]        @ 0x3fd8 <fftwObject_tilde_setup+0x298>
    3f30: e58d5000     	str	r5, [sp]
    3f34: e3a03001     	mov	r3, #1
    3f38: e08f100c     	add	r1, pc, r12
    3f3c: e1a02000     	mov	r2, r0
    3f40: e1a00006     	mov	r0, r6
    3f44: ebfff1f4     	bl	0x71c <.plt+0x104>      @ imm = #-0x3830
    3f48: e59f008c     	ldr	r0, [pc, #0x8c]         @ 0x3fdc <fftwObject_tilde_setup+0x29c>
    3f4c: e5944000     	ldr	r4, [r4]
    3f50: e08f0000     	add	r0, pc, r0
    3f54: ebfff1b4     	bl	0x62c <.plt+0x14>       @ imm = #-0x3930
    3f58: e58d5000     	str	r5, [sp]
    3f5c: e59f507c     	ldr	r5, [pc, #0x7c]         @ 0x3fe0 <fftwObject_tilde_setup+0x2a0>
    3f60: e3a03001     	mov	r3, #1
    3f64: e08f1005     	add	r1, pc, r5
    3f68: e1a02000     	mov	r2, r0
    3f6c: e1a00004     	mov	r0, r4
    3f70: ebfff1e9     	bl	0x71c <.plt+0x104>      @ imm = #-0x385c
    3f74: e28dd008     	add	sp, sp, #8
    3f78: e8bd8070     	pop	{r4, r5, r6, pc}
    3f7c: 14 05 00 00  	.word	0x00000514
    3f80: dc cb ff ff  	.word	0xffffcbdc
    3f84: ec cb ff ff  	.word	0xffffcbec
    3f88: f4 12 01 00  	.word	0x000112f4
    3f8c: d0 04 00 00  	.word	0x000004d0
    3f90: 44 cb ff ff  	.word	0xffffcb44
    3f94: a8 04 00 00  	.word	0x000004a8
    3f98: 80 ca ff ff  	.word	0xffffca80
    3f9c: 84 04 00 00  	.word	0x00000484
    3fa0: 70 ca ff ff  	.word	0xffffca70
    3fa4: 64 04 00 00  	.word	0x00000464
    3fa8: 64 ca ff ff  	.word	0xffffca64
    3fac: 40 04 00 00  	.word	0x00000440
    3fb0: 68 de ff ff  	.word	0xffffde68
    3fb4: 1c 04 00 00  	.word	0x0000041c
    3fb8: ec d9 ff ff  	.word	0xffffd9ec
    3fbc: f8 03 00 00  	.word	0x000003f8
    3fc0: 24 fa ff ff  	.word	0xfffffa24
    3fc4: d8 03 00 00  	.word	0x000003d8
    3fc8: a4 f5 ff ff  	.word	0xfffff5a4
    3fcc: bc 03 00 00  	.word	0x000003bc
    3fd0: 28 cf ff ff  	.word	0xffffcf28
    3fd4: 9c 03 00 00  	.word	0x0000039c
    3fd8: 1c d4 ff ff  	.word	0xffffd41c
    3fdc: 78 03 00 00  	.word	0x00000378
    3fe0: 54 c9 ff ff  	.word	0xffffc954

