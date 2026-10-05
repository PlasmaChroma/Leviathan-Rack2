0000385c <comport_setup>:
    385c: e59f03e8     	ldr	r0, [pc, #0x3e8]        @ 0x3c4c <comport_setup+0x3f0>  // u32=0x11d8; f32?=6.40113139e-42
    3860: e92d41f0     	push	{r4, r5, r6, r7, r8, lr}
    3864: e08f0000     	add	r0, pc, r0
    3868: e24dd010     	sub	sp, sp, #16
    386c: e3a04000     	mov	r4, #0
    3870: ebfff469     	bl	0xa1c <.plt+0x14>       @ imm = #-0x2e5c  // CALL gensym
    3874: e3a0700a     	mov	r7, #10
    3878: e59f23d0     	ldr	r2, [pc, #0x3d0]        @ 0x3c50 <comport_setup+0x3f4>  // u32=0xffffe5ec; f32?=nan
    387c: e59f13d0     	ldr	r1, [pc, #0x3d0]        @ 0x3c54 <comport_setup+0x3f8>  // u32=0xffffeac0; f32?=nan
    3880: e08f2002     	add	r2, pc, r2
    3884: e88d0090     	stm	sp, {r4, r7}
    3888: e58d4008     	str	r4, [sp, #0x8]
    388c: e3a03c11     	mov	r3, #4352
    3890: e08f1001     	add	r1, pc, r1
    3894: ebfff4b4     	bl	0xb6c <.plt+0x164>      @ imm = #-0x2d30  // CALL class_new
    3898: e59f63b8     	ldr	r6, [pc, #0x3b8]        @ 0x3c58 <comport_setup+0x3fc>  // u32=0x11758; f32?=1.00209656e-40
    389c: e59f33b8     	ldr	r3, [pc, #0x3b8]        @ 0x3c5c <comport_setup+0x400>  // u32=0xbc; f32?=2.63444111e-43
    38a0: e08f6006     	add	r6, pc, r6
    38a4: e59f53b4     	ldr	r5, [pc, #0x3b4]        @ 0x3c60 <comport_setup+0x404>  // u32=0xffffef00; f32?=nan
    38a8: e59f83b4     	ldr	r8, [pc, #0x3b4]        @ 0x3c64 <comport_setup+0x408>  // u32=0xffffe254; f32?=nan
    38ac: e08f1005     	add	r1, pc, r5
    38b0: e7965003     	ldr	r5, [r6, r3]
    38b4: e5850000     	str	r0, [r5]
    38b8: ebfff4b1     	bl	0xb84 <.plt+0x17c>      @ imm = #-0x2d3c  // CALL class_doaddfloat
    38bc: e08f1008     	add	r1, pc, r8
    38c0: e5950000     	ldr	r0, [r5]
    38c4: ebfff47b     	bl	0xab8 <.plt+0xb0>       @ imm = #-0x2e14  // CALL class_addlist
    38c8: e59fc398     	ldr	r12, [pc, #0x398]       @ 0x3c68 <comport_setup+0x40c>  // u32=0x1120; f32?=6.14329247e-42
    38cc: e5958000     	ldr	r8, [r5]
    38d0: e08f000c     	add	r0, pc, r12
    38d4: ebfff450     	bl	0xa1c <.plt+0x14>       @ imm = #-0x2ec0  // CALL gensym
    38d8: e59f238c     	ldr	r2, [pc, #0x38c]        @ 0x3c6c <comport_setup+0x410>  // u32=0xffffe9b8; f32?=nan
    38dc: e3a03001     	mov	r3, #1
    38e0: e08f1002     	add	r1, pc, r2
    38e4: e58d4000     	str	r4, [sp]
    38e8: e1a02000     	mov	r2, r0
    38ec: e1a00008     	mov	r0, r8
    38f0: ebfff4ac     	bl	0xba8 <.plt+0x1a0>      @ imm = #-0x2d50  // CALL class_addmethod
    38f4: e59f0374     	ldr	r0, [pc, #0x374]        @ 0x3c70 <comport_setup+0x414>  // u32=0x1148; f32?=6.19934441e-42
    38f8: e5958000     	ldr	r8, [r5]
    38fc: e08f0000     	add	r0, pc, r0
    3900: ebfff445     	bl	0xa1c <.plt+0x14>       @ imm = #-0x2eec  // CALL gensym
    3904: e59f1368     	ldr	r1, [pc, #0x368]        @ 0x3c74 <comport_setup+0x418>  // u32=0xffffd8dc; f32?=nan
    3908: e58d4000     	str	r4, [sp]
    390c: e08f1001     	add	r1, pc, r1
    3910: e3a03001     	mov	r3, #1
    3914: e1a02000     	mov	r2, r0
    3918: e1a00008     	mov	r0, r8
    391c: ebfff4a1     	bl	0xba8 <.plt+0x1a0>      @ imm = #-0x2d7c  // CALL class_addmethod
    3920: e59f3350     	ldr	r3, [pc, #0x350]        @ 0x3c78 <comport_setup+0x41c>  // u32=0x1124; f32?=6.14889766e-42
    3924: e5958000     	ldr	r8, [r5]
    3928: e08f0003     	add	r0, pc, r3
    392c: ebfff43a     	bl	0xa1c <.plt+0x14>       @ imm = #-0x2f18  // CALL gensym
    3930: e59fc344     	ldr	r12, [pc, #0x344]       @ 0x3c7c <comport_setup+0x420>  // u32=0xffffe8a4; f32?=nan
    3934: e3a03001     	mov	r3, #1
    3938: e08f100c     	add	r1, pc, r12
    393c: e58d4000     	str	r4, [sp]
    3940: e1a02000     	mov	r2, r0
    3944: e1a00008     	mov	r0, r8
    3948: ebfff496     	bl	0xba8 <.plt+0x1a0>      @ imm = #-0x2da8  // CALL class_addmethod
    394c: e59f232c     	ldr	r2, [pc, #0x32c]        @ 0x3c80 <comport_setup+0x424>  // u32=0x10c4; f32?=6.01437301e-42
    3950: e5958000     	ldr	r8, [r5]
    3954: e08f0002     	add	r0, pc, r2
    3958: ebfff42f     	bl	0xa1c <.plt+0x14>       @ imm = #-0x2f44  // CALL gensym
    395c: e59f1320     	ldr	r1, [pc, #0x320]        @ 0x3c84 <comport_setup+0x428>  // u32=0xffffe7b8; f32?=nan
    3960: e3a03001     	mov	r3, #1
    3964: e08f1001     	add	r1, pc, r1
    3968: e58d4000     	str	r4, [sp]
    396c: e1a02000     	mov	r2, r0
    3970: e1a00008     	mov	r0, r8
    3974: ebfff48b     	bl	0xba8 <.plt+0x1a0>      @ imm = #-0x2dd4  // CALL class_addmethod
    3978: e59f0308     	ldr	r0, [pc, #0x308]        @ 0x3c88 <comport_setup+0x42c>  // u32=0x10d4; f32?=6.03679378e-42
    397c: e5958000     	ldr	r8, [r5]
    3980: e08f0000     	add	r0, pc, r0
    3984: ebfff424     	bl	0xa1c <.plt+0x14>       @ imm = #-0x2f70  // CALL gensym
    3988: e59fc2fc     	ldr	r12, [pc, #0x2fc]       @ 0x3c8c <comport_setup+0x430>  // u32=0xffffe61c; f32?=nan
    398c: e58d4000     	str	r4, [sp]
    3990: e08f100c     	add	r1, pc, r12
    3994: e3a03001     	mov	r3, #1
    3998: e1a02000     	mov	r2, r0
    399c: e1a00008     	mov	r0, r8
    39a0: ebfff480     	bl	0xba8 <.plt+0x1a0>      @ imm = #-0x2e00  // CALL class_addmethod
    39a4: e59f32e4     	ldr	r3, [pc, #0x2e4]        @ 0x3c90 <comport_setup+0x434>  // u32=0x10ac; f32?=5.98074185e-42
    39a8: e5958000     	ldr	r8, [r5]
    39ac: e08f0003     	add	r0, pc, r3
    39b0: ebfff419     	bl	0xa1c <.plt+0x14>       @ imm = #-0x2f9c  // CALL gensym
    39b4: e59f22d8     	ldr	r2, [pc, #0x2d8]        @ 0x3c94 <comport_setup+0x438>  // u32=0xffffe540; f32?=nan
    39b8: e3a03001     	mov	r3, #1
    39bc: e08f1002     	add	r1, pc, r2
    39c0: e58d4000     	str	r4, [sp]
    39c4: e1a02000     	mov	r2, r0
    39c8: e1a00008     	mov	r0, r8
    39cc: ebfff475     	bl	0xba8 <.plt+0x1a0>      @ imm = #-0x2e2c  // CALL class_addmethod
    39d0: e59f12c0     	ldr	r1, [pc, #0x2c0]        @ 0x3c98 <comport_setup+0x43c>  // u32=0x1028; f32?=5.79577045e-42
    39d4: e5958000     	ldr	r8, [r5]
    39d8: e08f0001     	add	r0, pc, r1
    39dc: ebfff40e     	bl	0xa1c <.plt+0x14>       @ imm = #-0x2fc8  // CALL gensym
    39e0: e59fc2b4     	ldr	r12, [pc, #0x2b4]       @ 0x3c9c <comport_setup+0x440>  // u32=0xffffd710; f32?=nan
    39e4: e3a03001     	mov	r3, #1
    39e8: e08f100c     	add	r1, pc, r12
    39ec: e58d4000     	str	r4, [sp]
    39f0: e1a02000     	mov	r2, r0
    39f4: e1a00008     	mov	r0, r8
    39f8: ebfff46a     	bl	0xba8 <.plt+0x1a0>      @ imm = #-0x2e58  // CALL class_addmethod
    39fc: e59f029c     	ldr	r0, [pc, #0x29c]        @ 0x3ca0 <comport_setup+0x444>  // u32=0x101c; f32?=5.77895487e-42
    3a00: e5958000     	ldr	r8, [r5]
    3a04: e08f0000     	add	r0, pc, r0
    3a08: ebfff403     	bl	0xa1c <.plt+0x14>       @ imm = #-0x2ff4  // CALL gensym
    3a0c: e59f2290     	ldr	r2, [pc, #0x290]        @ 0x3ca4 <comport_setup+0x448>  // u32=0xffffe648; f32?=nan
    3a10: e58d4000     	str	r4, [sp]
    3a14: e08f1002     	add	r1, pc, r2
    3a18: e3a03001     	mov	r3, #1
    3a1c: e1a02000     	mov	r2, r0
    3a20: e1a00008     	mov	r0, r8
    3a24: ebfff45f     	bl	0xba8 <.plt+0x1a0>      @ imm = #-0x2e84  // CALL class_addmethod
    3a28: e59f3278     	ldr	r3, [pc, #0x278]        @ 0x3ca8 <comport_setup+0x44c>  // u32=0xff8; f32?=5.72850812e-42
    3a2c: e5958000     	ldr	r8, [r5]
    3a30: e08f0003     	add	r0, pc, r3
    3a34: ebfff3f8     	bl	0xa1c <.plt+0x14>       @ imm = #-0x3020  // CALL gensym
    3a38: e59f126c     	ldr	r1, [pc, #0x26c]        @ 0x3cac <comport_setup+0x450>  // u32=0xffffe03c; f32?=nan
    3a3c: e3a03001     	mov	r3, #1
    3a40: e08f1001     	add	r1, pc, r1
    3a44: e58d4000     	str	r4, [sp]
    3a48: e1a02000     	mov	r2, r0
    3a4c: e1a00008     	mov	r0, r8
    3a50: ebfff454     	bl	0xba8 <.plt+0x1a0>      @ imm = #-0x2eb0  // CALL class_addmethod
    3a54: e59fc254     	ldr	r12, [pc, #0x254]       @ 0x3cb0 <comport_setup+0x454>  // u32=0x1000; f32?=5.73971851e-42
    3a58: e5958000     	ldr	r8, [r5]
    3a5c: e08f000c     	add	r0, pc, r12
    3a60: ebfff3ed     	bl	0xa1c <.plt+0x14>       @ imm = #-0x304c  // CALL gensym
    3a64: e1a03004     	mov	r3, r4
    3a68: e1a02000     	mov	r2, r0
    3a6c: e59f0240     	ldr	r0, [pc, #0x240]        @ 0x3cb4 <comport_setup+0x458>  // u32=0xffffedb4; f32?=nan
    3a70: e08f1000     	add	r1, pc, r0
    3a74: e1a00008     	mov	r0, r8
    3a78: ebfff44a     	bl	0xba8 <.plt+0x1a0>      @ imm = #-0x2ed8  // CALL class_addmethod
    3a7c: e59f2234     	ldr	r2, [pc, #0x234]        @ 0x3cb8 <comport_setup+0x45c>  // u32=0xf5c; f32?=5.50990556e-42
    3a80: e5958000     	ldr	r8, [r5]
    3a84: e08f0002     	add	r0, pc, r2
    3a88: ebfff3e3     	bl	0xa1c <.plt+0x14>       @ imm = #-0x3074  // CALL gensym
    3a8c: e59f1228     	ldr	r1, [pc, #0x228]        @ 0x3cbc <comport_setup+0x460>  // u32=0xffffee14; f32?=nan
    3a90: e58d4000     	str	r4, [sp]
    3a94: e08f1001     	add	r1, pc, r1
    3a98: e3a03001     	mov	r3, #1
    3a9c: e1a02000     	mov	r2, r0
    3aa0: e1a00008     	mov	r0, r8
    3aa4: ebfff43f     	bl	0xba8 <.plt+0x1a0>      @ imm = #-0x2f04  // CALL class_addmethod
    3aa8: e59f3210     	ldr	r3, [pc, #0x210]        @ 0x3cc0 <comport_setup+0x464>  // u32=0xfb4; f32?=5.63321983e-42
    3aac: e5958000     	ldr	r8, [r5]
    3ab0: e08f0003     	add	r0, pc, r3
    3ab4: ebfff3d8     	bl	0xa1c <.plt+0x14>       @ imm = #-0x30a0  // CALL gensym
    3ab8: e59fc204     	ldr	r12, [pc, #0x204]       @ 0x3cc4 <comport_setup+0x468>  // u32=0xfffff878; f32?=nan
    3abc: e3a03002     	mov	r3, #2
    3ac0: e08f100c     	add	r1, pc, r12
    3ac4: e58d4000     	str	r4, [sp]
    3ac8: e1a02000     	mov	r2, r0
    3acc: e1a00008     	mov	r0, r8
    3ad0: ebfff434     	bl	0xba8 <.plt+0x1a0>      @ imm = #-0x2f30  // CALL class_addmethod
    3ad4: e59f01ec     	ldr	r0, [pc, #0x1ec]        @ 0x3cc8 <comport_setup+0x46c>  // u32=0xf94; f32?=5.58837828e-42
    3ad8: e5958000     	ldr	r8, [r5]
    3adc: e08f0000     	add	r0, pc, r0
    3ae0: ebfff3cd     	bl	0xa1c <.plt+0x14>       @ imm = #-0x30cc  // CALL gensym
    3ae4: e1a03007     	mov	r3, r7
    3ae8: e59f71dc     	ldr	r7, [pc, #0x1dc]        @ 0x3ccc <comport_setup+0x470>  // u32=0xffffeb68; f32?=nan
    3aec: e58d4000     	str	r4, [sp]
    3af0: e08f1007     	add	r1, pc, r7
    3af4: e1a02000     	mov	r2, r0
    3af8: e1a00008     	mov	r0, r8
    3afc: ebfff429     	bl	0xba8 <.plt+0x1a0>      @ imm = #-0x2f5c  // CALL class_addmethod
    3b00: e59f21c8     	ldr	r2, [pc, #0x1c8]        @ 0x3cd0 <comport_setup+0x474>  // u32=0xf70; f32?=5.53793153e-42
    3b04: e5958000     	ldr	r8, [r5]
    3b08: e08f0002     	add	r0, pc, r2
    3b0c: ebfff3c2     	bl	0xa1c <.plt+0x14>       @ imm = #-0x30f8  // CALL gensym
    3b10: e59f11bc     	ldr	r1, [pc, #0x1bc]        @ 0x3cd4 <comport_setup+0x478>  // u32=0xffffd270; f32?=nan
    3b14: e58d4000     	str	r4, [sp]
    3b18: e08f1001     	add	r1, pc, r1
    3b1c: e3a03001     	mov	r3, #1
    3b20: e1a02000     	mov	r2, r0
    3b24: e1a00008     	mov	r0, r8
    3b28: ebfff41e     	bl	0xba8 <.plt+0x1a0>      @ imm = #-0x2f88  // CALL class_addmethod
    3b2c: e59f31a4     	ldr	r3, [pc, #0x1a4]        @ 0x3cd8 <comport_setup+0x47c>  // u32=0xf54; f32?=5.49869517e-42
    3b30: e5957000     	ldr	r7, [r5]
    3b34: e08f0003     	add	r0, pc, r3
    3b38: ebfff3b7     	bl	0xa1c <.plt+0x14>       @ imm = #-0x3124  // CALL gensym
    3b3c: e59fc198     	ldr	r12, [pc, #0x198]       @ 0x3cdc <comport_setup+0x480>  // u32=0xffffd270; f32?=nan
    3b40: e3a03001     	mov	r3, #1
    3b44: e08f100c     	add	r1, pc, r12
    3b48: e58d4000     	str	r4, [sp]
    3b4c: e1a02000     	mov	r2, r0
    3b50: e1a00007     	mov	r0, r7
    3b54: ebfff413     	bl	0xba8 <.plt+0x1a0>      @ imm = #-0x2fb4  // CALL class_addmethod
    3b58: e59f0180     	ldr	r0, [pc, #0x180]        @ 0x3ce0 <comport_setup+0x484>  // u32=0xf30; f32?=5.44824843e-42
    3b5c: e5958000     	ldr	r8, [r5]
    3b60: e08f0000     	add	r0, pc, r0
    3b64: ebfff3ac     	bl	0xa1c <.plt+0x14>       @ imm = #-0x3150  // CALL gensym
    3b68: e59f2174     	ldr	r2, [pc, #0x174]        @ 0x3ce4 <comport_setup+0x488>  // u32=0xffffd55c; f32?=nan
    3b6c: e3a03001     	mov	r3, #1
    3b70: e08f1002     	add	r1, pc, r2
    3b74: e58d4000     	str	r4, [sp]
    3b78: e1a02000     	mov	r2, r0
    3b7c: e1a00008     	mov	r0, r8
    3b80: ebfff408     	bl	0xba8 <.plt+0x1a0>      @ imm = #-0x2fe0  // CALL class_addmethod
    3b84: e59f115c     	ldr	r1, [pc, #0x15c]        @ 0x3ce8 <comport_setup+0x48c>  // u32=0xf0c; f32?=5.39780168e-42
    3b88: e5957000     	ldr	r7, [r5]
    3b8c: e08f0001     	add	r0, pc, r1
    3b90: ebfff3a1     	bl	0xa1c <.plt+0x14>       @ imm = #-0x317c  // CALL gensym
    3b94: e59fc150     	ldr	r12, [pc, #0x150]       @ 0x3cec <comport_setup+0x490>  // u32=0xffffd4bc; f32?=nan
    3b98: e1a03004     	mov	r3, r4
    3b9c: e08f100c     	add	r1, pc, r12
    3ba0: e1a02000     	mov	r2, r0
    3ba4: e1a00007     	mov	r0, r7
    3ba8: ebfff3fe     	bl	0xba8 <.plt+0x1a0>      @ imm = #-0x3008  // CALL class_addmethod
    3bac: e59f313c     	ldr	r3, [pc, #0x13c]        @ 0x3cf0 <comport_setup+0x494>  // u32=0xeec; f32?=5.35296013e-42
    3bb0: e5958000     	ldr	r8, [r5]
    3bb4: e08f0003     	add	r0, pc, r3
    3bb8: ebfff397     	bl	0xa1c <.plt+0x14>       @ imm = #-0x31a4  // CALL gensym
    3bbc: e1a03004     	mov	r3, r4
    3bc0: e1a02000     	mov	r2, r0
    3bc4: e59f0128     	ldr	r0, [pc, #0x128]        @ 0x3cf4 <comport_setup+0x498>  // u32=0xfffff2f8; f32?=nan
    3bc8: e08f1000     	add	r1, pc, r0
    3bcc: e1a00008     	mov	r0, r8
    3bd0: ebfff3f4     	bl	0xba8 <.plt+0x1a0>      @ imm = #-0x3030  // CALL class_addmethod
    3bd4: e59f211c     	ldr	r2, [pc, #0x11c]        @ 0x3cf8 <comport_setup+0x49c>  // u32=0xecc; f32?=5.30811858e-42
    3bd8: e5957000     	ldr	r7, [r5]
    3bdc: e08f0002     	add	r0, pc, r2
    3be0: ebfff38d     	bl	0xa1c <.plt+0x14>       @ imm = #-0x31cc  // CALL gensym
    3be4: e59f1110     	ldr	r1, [pc, #0x110]        @ 0x3cfc <comport_setup+0x4a0>  // u32=0xffffd340; f32?=nan
    3be8: e1a03004     	mov	r3, r4
    3bec: e08f1001     	add	r1, pc, r1
    3bf0: e59f8108     	ldr	r8, [pc, #0x108]        @ 0x3d00 <comport_setup+0x4a4>  // u32=0xffffd1c0; f32?=nan
    3bf4: e1a02000     	mov	r2, r0
    3bf8: e1a00007     	mov	r0, r7
    3bfc: ebfff3e9     	bl	0xba8 <.plt+0x1a0>      @ imm = #-0x305c  // CALL class_addmethod
    3c00: e59fc0fc     	ldr	r12, [pc, #0xfc]        @ 0x3d04 <comport_setup+0x4a8>  // u32=0x208; f32?=7.28675201e-43
    3c04: e5955000     	ldr	r5, [r5]
    3c08: e08f000c     	add	r0, pc, r12
    3c0c: ebfff382     	bl	0xa1c <.plt+0x14>       @ imm = #-0x31f8  // CALL gensym
    3c10: e1a03004     	mov	r3, r4
    3c14: e08f1008     	add	r1, pc, r8
    3c18: e1a02000     	mov	r2, r0
    3c1c: e1a00005     	mov	r0, r5
    3c20: ebfff3e0     	bl	0xba8 <.plt+0x1a0>      @ imm = #-0x3080  // CALL class_addmethod
    3c24: e59f30dc     	ldr	r3, [pc, #0xdc]         @ 0x3d08 <comport_setup+0x4ac>  // u32=0xc8; f32?=2.80259693e-43
    3c28: e59f00dc     	ldr	r0, [pc, #0xdc]         @ 0x3d0c <comport_setup+0x4b0>  // u32=0xe80; f32?=5.2016199e-42
    3c2c: e7966003     	ldr	r6, [r6, r3]
    3c30: e08f1000     	add	r1, pc, r0
    3c34: e3e00000     	mvn	r0, #0
    3c38: e5864000     	str	r4, [r6]
    3c3c: e5864004     	str	r4, [r6, #0x4]
    3c40: e28dd010     	add	sp, sp, #16
    3c44: e8bd41f0     	pop	{r4, r5, r6, r7, r8, lr}
    3c48: eafff3d0     	b	0xb90 <.plt+0x188>      @ imm = #-0x30c0  // CALL verbose
    3c4c: d8 11 00 00  	.word	0x000011d8
    3c50: ec e5 ff ff  	.word	0xffffe5ec
    3c54: c0 ea ff ff  	.word	0xffffeac0
    3c58: 58 17 01 00  	.word	0x00011758
    3c5c: bc 00 00 00  	.word	0x000000bc
    3c60: 00 ef ff ff  	.word	0xffffef00
    3c64: 54 e2 ff ff  	.word	0xffffe254
    3c68: 20 11 00 00  	.word	0x00001120
    3c6c: b8 e9 ff ff  	.word	0xffffe9b8
    3c70: 48 11 00 00  	.word	0x00001148
    3c74: dc d8 ff ff  	.word	0xffffd8dc
    3c78: 24 11 00 00  	.word	0x00001124
    3c7c: a4 e8 ff ff  	.word	0xffffe8a4
    3c80: c4 10 00 00  	.word	0x000010c4
    3c84: b8 e7 ff ff  	.word	0xffffe7b8
    3c88: d4 10 00 00  	.word	0x000010d4
    3c8c: 1c e6 ff ff  	.word	0xffffe61c
    3c90: ac 10 00 00  	.word	0x000010ac
    3c94: 40 e5 ff ff  	.word	0xffffe540
    3c98: 28 10 00 00  	.word	0x00001028
    3c9c: 10 d7 ff ff  	.word	0xffffd710
    3ca0: 1c 10 00 00  	.word	0x0000101c
    3ca4: 48 e6 ff ff  	.word	0xffffe648
    3ca8: f8 0f 00 00  	.word	0x00000ff8
    3cac: 3c e0 ff ff  	.word	0xffffe03c
    3cb0: 00 10 00 00  	.word	0x00001000
    3cb4: b4 ed ff ff  	.word	0xffffedb4
    3cb8: 5c 0f 00 00  	.word	0x00000f5c
    3cbc: 14 ee ff ff  	.word	0xffffee14
    3cc0: b4 0f 00 00  	.word	0x00000fb4
    3cc4: 78 f8 ff ff  	.word	0xfffff878
    3cc8: 94 0f 00 00  	.word	0x00000f94
    3ccc: 68 eb ff ff  	.word	0xffffeb68
    3cd0: 70 0f 00 00  	.word	0x00000f70
    3cd4: 70 d2 ff ff  	.word	0xffffd270
    3cd8: 54 0f 00 00  	.word	0x00000f54
    3cdc: 70 d2 ff ff  	.word	0xffffd270
    3ce0: 30 0f 00 00  	.word	0x00000f30
    3ce4: 5c d5 ff ff  	.word	0xffffd55c
    3ce8: 0c 0f 00 00  	.word	0x00000f0c
    3cec: bc d4 ff ff  	.word	0xffffd4bc
    3cf0: ec 0e 00 00  	.word	0x00000eec
    3cf4: f8 f2 ff ff  	.word	0xfffff2f8
    3cf8: cc 0e 00 00  	.word	0x00000ecc
    3cfc: 40 d3 ff ff  	.word	0xffffd340
    3d00: c0 d1 ff ff  	.word	0xffffd1c0
    3d04: 08 02 00 00  	.word	0x00000208
    3d08: c8 00 00 00  	.word	0x000000c8
    3d0c: 80 0e 00 00  	.word	0x00000e80

Disassembly of section .fini:

