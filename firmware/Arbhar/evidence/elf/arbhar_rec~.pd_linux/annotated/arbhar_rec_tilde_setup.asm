00004ce0 <arbhar_rec_tilde_setup>:
    4ce0: e59f02a4     	ldr	r0, [pc, #0x2a4]        @ 0x4f8c <arbhar_rec_tilde_setup+0x2ac>  // u32=0x4974; f32?=2.63500163e-41
    4ce4: e92d41f0     	push	{r4, r5, r6, r7, r8, lr}
    4ce8: e08f0000     	add	r0, pc, r0
    4cec: e24dd010     	sub	sp, sp, #16
    4cf0: e59f6298     	ldr	r6, [pc, #0x298]        @ 0x4f90 <arbhar_rec_tilde_setup+0x2b0>  // u32=0x152f8; f32?=1.21599076e-40
    4cf4: ebfff606     	bl	0x2514 <.plt+0x14>      @ imm = #-0x27e8  // CALL gensym
    4cf8: e59f2294     	ldr	r2, [pc, #0x294]        @ 0x4f94 <arbhar_rec_tilde_setup+0x2b4>  // u32=0x144; f32?=4.54020702e-43
    4cfc: e59f1294     	ldr	r1, [pc, #0x294]        @ 0x4f98 <arbhar_rec_tilde_setup+0x2b8>  // u32=0x140; f32?=4.48415509e-43
    4d00: e08f6006     	add	r6, pc, r6
    4d04: e3a05000     	mov	r5, #0
    4d08: e3a0700a     	mov	r7, #10
    4d0c: e7962002     	ldr	r2, [r6, r2]
    4d10: e3a03ff2     	mov	r3, #968
    4d14: e7961001     	ldr	r1, [r6, r1]
    4d18: e58d5008     	str	r5, [sp, #0x8]
    4d1c: e88d00a0     	stm	sp, {r5, r7}
    4d20: ebfff6ac     	bl	0x27d8 <.plt+0x2d8>     @ imm = #-0x2550  // CALL class_new
    4d24: e59f4270     	ldr	r4, [pc, #0x270]        @ 0x4f9c <arbhar_rec_tilde_setup+0x2bc>  // u32=0x1547c; f32?=1.22142779e-40
    4d28: e59f3270     	ldr	r3, [pc, #0x270]        @ 0x4fa0 <arbhar_rec_tilde_setup+0x2c0>  // u32=0x4934; f32?=2.62603332e-41
    4d2c: e08f4004     	add	r4, pc, r4
    4d30: e1a08000     	mov	r8, r0
    4d34: e08f0003     	add	r0, pc, r3
    4d38: e584800c     	str	r8, [r4, #0xc]
    4d3c: ebfff5f4     	bl	0x2514 <.plt+0x14>      @ imm = #-0x2830  // CALL gensym
    4d40: e59fc25c     	ldr	r12, [pc, #0x25c]       @ 0x4fa4 <arbhar_rec_tilde_setup+0x2c4>  // u32=0x14c; f32?=4.6523109e-43
    4d44: e3a0300b     	mov	r3, #11
    4d48: e796100c     	ldr	r1, [r6, r12]
    4d4c: e58d5000     	str	r5, [sp]
    4d50: e1a02000     	mov	r2, r0
    4d54: e1a00008     	mov	r0, r8
    4d58: ebfff6b0     	bl	0x2820 <.plt+0x320>     @ imm = #-0x2540  // CALL class_addmethod
    4d5c: e59f0244     	ldr	r0, [pc, #0x244]        @ 0x4fa8 <arbhar_rec_tilde_setup+0x2c8>  // u32=0x4908; f32?=2.61986761e-41
    4d60: e594800c     	ldr	r8, [r4, #0xc]
    4d64: e08f0000     	add	r0, pc, r0
    4d68: ebfff5e9     	bl	0x2514 <.plt+0x14>      @ imm = #-0x285c  // CALL gensym
    4d6c: e59f2238     	ldr	r2, [pc, #0x238]        @ 0x4fac <arbhar_rec_tilde_setup+0x2cc>  // u32=0xffffdc38; f32?=nan
    4d70: e58d5000     	str	r5, [sp]
    4d74: e3a03001     	mov	r3, #1
    4d78: e08f1002     	add	r1, pc, r2
    4d7c: e1a02000     	mov	r2, r0
    4d80: e1a00008     	mov	r0, r8
    4d84: ebfff6a5     	bl	0x2820 <.plt+0x320>     @ imm = #-0x256c  // CALL class_addmethod
    4d88: e59f1220     	ldr	r1, [pc, #0x220]        @ 0x4fb0 <arbhar_rec_tilde_setup+0x2d0>  // u32=0x48e4; f32?=2.61482293e-41
    4d8c: e594800c     	ldr	r8, [r4, #0xc]
    4d90: e08f0001     	add	r0, pc, r1
    4d94: ebfff5de     	bl	0x2514 <.plt+0x14>      @ imm = #-0x2888  // CALL gensym
    4d98: e59fc214     	ldr	r12, [pc, #0x214]       @ 0x4fb4 <arbhar_rec_tilde_setup+0x2d4>  // u32=0xffffe334; f32?=nan
    4d9c: e1a03005     	mov	r3, r5
    4da0: e08f100c     	add	r1, pc, r12
    4da4: e1a02000     	mov	r2, r0
    4da8: e1a00008     	mov	r0, r8
    4dac: ebfff69b     	bl	0x2820 <.plt+0x320>     @ imm = #-0x2594  // CALL class_addmethod
    4db0: e59f3200     	ldr	r3, [pc, #0x200]        @ 0x4fb8 <arbhar_rec_tilde_setup+0x2d8>  // u32=0x48c4; f32?=2.61033878e-41
    4db4: e594800c     	ldr	r8, [r4, #0xc]
    4db8: e08f0003     	add	r0, pc, r3
    4dbc: ebfff5d4     	bl	0x2514 <.plt+0x14>      @ imm = #-0x28b0  // CALL gensym
    4dc0: e59f21f4     	ldr	r2, [pc, #0x1f4]        @ 0x4fbc <arbhar_rec_tilde_setup+0x2dc>  // u32=0xffffdc14; f32?=nan
    4dc4: e58d5000     	str	r5, [sp]
    4dc8: e3a03001     	mov	r3, #1
    4dcc: e08f1002     	add	r1, pc, r2
    4dd0: e1a02000     	mov	r2, r0
    4dd4: e1a00008     	mov	r0, r8
    4dd8: ebfff690     	bl	0x2820 <.plt+0x320>     @ imm = #-0x25c0  // CALL class_addmethod
    4ddc: e59f01dc     	ldr	r0, [pc, #0x1dc]        @ 0x4fc0 <arbhar_rec_tilde_setup+0x2e0>  // u32=0x48a0; f32?=2.6052941e-41
    4de0: e594800c     	ldr	r8, [r4, #0xc]
    4de4: e08f0000     	add	r0, pc, r0
    4de8: ebfff5c9     	bl	0x2514 <.plt+0x14>      @ imm = #-0x28dc  // CALL gensym
    4dec: e59f11d0     	ldr	r1, [pc, #0x1d0]        @ 0x4fc4 <arbhar_rec_tilde_setup+0x2e4>  // u32=0xffffdc28; f32?=nan
    4df0: e58d5000     	str	r5, [sp]
    4df4: e3a03001     	mov	r3, #1
    4df8: e08f1001     	add	r1, pc, r1
    4dfc: e1a02000     	mov	r2, r0
    4e00: e1a00008     	mov	r0, r8
    4e04: ebfff685     	bl	0x2820 <.plt+0x320>     @ imm = #-0x25ec  // CALL class_addmethod
    4e08: e59fc1b8     	ldr	r12, [pc, #0x1b8]       @ 0x4fc8 <arbhar_rec_tilde_setup+0x2e8>  // u32=0x4878; f32?=2.59968891e-41
    4e0c: e594800c     	ldr	r8, [r4, #0xc]
    4e10: e08f000c     	add	r0, pc, r12
    4e14: ebfff5be     	bl	0x2514 <.plt+0x14>      @ imm = #-0x2908  // CALL gensym
    4e18: e59f21ac     	ldr	r2, [pc, #0x1ac]        @ 0x4fcc <arbhar_rec_tilde_setup+0x2ec>  // u32=0xffffdbd8; f32?=nan
    4e1c: e58d5000     	str	r5, [sp]
    4e20: e3a03001     	mov	r3, #1
    4e24: e08f1002     	add	r1, pc, r2
    4e28: e1a02000     	mov	r2, r0
    4e2c: e1a00008     	mov	r0, r8
    4e30: ebfff67a     	bl	0x2820 <.plt+0x320>     @ imm = #-0x2618  // CALL class_addmethod
    4e34: e59f3194     	ldr	r3, [pc, #0x194]        @ 0x4fd0 <arbhar_rec_tilde_setup+0x2f0>  // u32=0x4858; f32?=2.59520476e-41
    4e38: e594800c     	ldr	r8, [r4, #0xc]
    4e3c: e08f0003     	add	r0, pc, r3
    4e40: ebfff5b3     	bl	0x2514 <.plt+0x14>      @ imm = #-0x2934  // CALL gensym
    4e44: e59f1188     	ldr	r1, [pc, #0x188]        @ 0x4fd4 <arbhar_rec_tilde_setup+0x2f4>  // u32=0xffffdbb8; f32?=nan
    4e48: e58d5000     	str	r5, [sp]
    4e4c: e3a03001     	mov	r3, #1
    4e50: e08f1001     	add	r1, pc, r1
    4e54: e1a02000     	mov	r2, r0
    4e58: e1a00008     	mov	r0, r8
    4e5c: ebfff66f     	bl	0x2820 <.plt+0x320>     @ imm = #-0x2644  // CALL class_addmethod
    4e60: e59f0170     	ldr	r0, [pc, #0x170]        @ 0x4fd8 <arbhar_rec_tilde_setup+0x2f8>  // u32=0x4838; f32?=2.5907206e-41
    4e64: e594800c     	ldr	r8, [r4, #0xc]
    4e68: e08f0000     	add	r0, pc, r0
    4e6c: ebfff5a8     	bl	0x2514 <.plt+0x14>      @ imm = #-0x2960  // CALL gensym
    4e70: e59fc164     	ldr	r12, [pc, #0x164]       @ 0x4fdc <arbhar_rec_tilde_setup+0x2fc>  // u32=0xffffdb98; f32?=nan
    4e74: e58d5000     	str	r5, [sp]
    4e78: e3a03001     	mov	r3, #1
    4e7c: e08f100c     	add	r1, pc, r12
    4e80: e1a02000     	mov	r2, r0
    4e84: e1a00008     	mov	r0, r8
    4e88: ebfff664     	bl	0x2820 <.plt+0x320>     @ imm = #-0x2670  // CALL class_addmethod
    4e8c: e59f214c     	ldr	r2, [pc, #0x14c]        @ 0x4fe0 <arbhar_rec_tilde_setup+0x300>  // u32=0x481c; f32?=2.58679697e-41
    4e90: e594800c     	ldr	r8, [r4, #0xc]
    4e94: e08f0002     	add	r0, pc, r2
    4e98: ebfff59d     	bl	0x2514 <.plt+0x14>      @ imm = #-0x298c  // CALL gensym
    4e9c: e59f1140     	ldr	r1, [pc, #0x140]        @ 0x4fe4 <arbhar_rec_tilde_setup+0x304>  // u32=0xffffdb24; f32?=nan
    4ea0: e58d5000     	str	r5, [sp]
    4ea4: e3a03001     	mov	r3, #1
    4ea8: e08f1001     	add	r1, pc, r1
    4eac: e1a02000     	mov	r2, r0
    4eb0: e1a00008     	mov	r0, r8
    4eb4: ebfff659     	bl	0x2820 <.plt+0x320>     @ imm = #-0x269c  // CALL class_addmethod
    4eb8: e59f3128     	ldr	r3, [pc, #0x128]        @ 0x4fe8 <arbhar_rec_tilde_setup+0x308>  // u32=0x47f8; f32?=2.58175229e-41
    4ebc: e594800c     	ldr	r8, [r4, #0xc]
    4ec0: e08f0003     	add	r0, pc, r3
    4ec4: ebfff592     	bl	0x2514 <.plt+0x14>      @ imm = #-0x29b8  // CALL gensym
    4ec8: e59fc11c     	ldr	r12, [pc, #0x11c]       @ 0x4fec <arbhar_rec_tilde_setup+0x30c>  // u32=0xfffffb30; f32?=nan
    4ecc: e58d5000     	str	r5, [sp]
    4ed0: e3a03001     	mov	r3, #1
    4ed4: e08f100c     	add	r1, pc, r12
    4ed8: e1a02000     	mov	r2, r0
    4edc: e1a00008     	mov	r0, r8
    4ee0: ebfff64e     	bl	0x2820 <.plt+0x320>     @ imm = #-0x26c8  // CALL class_addmethod
    4ee4: e59f0104     	ldr	r0, [pc, #0x104]        @ 0x4ff0 <arbhar_rec_tilde_setup+0x310>  // u32=0x47d4; f32?=2.57670762e-41
    4ee8: e594800c     	ldr	r8, [r4, #0xc]
    4eec: e08f0000     	add	r0, pc, r0
    4ef0: ebfff587     	bl	0x2514 <.plt+0x14>      @ imm = #-0x29e4  // CALL gensym
    4ef4: e59f20f8     	ldr	r2, [pc, #0xf8]         @ 0x4ff4 <arbhar_rec_tilde_setup+0x314>  // u32=0xffffdebc; f32?=nan
    4ef8: e58d5000     	str	r5, [sp]
    4efc: e3a03001     	mov	r3, #1
    4f00: e08f1002     	add	r1, pc, r2
    4f04: e1a02000     	mov	r2, r0
    4f08: e1a00008     	mov	r0, r8
    4f0c: ebfff643     	bl	0x2820 <.plt+0x320>     @ imm = #-0x26f4  // CALL class_addmethod
    4f10: e59f10e0     	ldr	r1, [pc, #0xe0]         @ 0x4ff8 <arbhar_rec_tilde_setup+0x318>  // u32=0x47b8; f32?=2.57278398e-41
    4f14: e594800c     	ldr	r8, [r4, #0xc]
    4f18: e08f0001     	add	r0, pc, r1
    4f1c: ebfff57c     	bl	0x2514 <.plt+0x14>      @ imm = #-0x2a10  // CALL gensym
    4f20: e59fc0d4     	ldr	r12, [pc, #0xd4]        @ 0x4ffc <arbhar_rec_tilde_setup+0x31c>  // u32=0xffffdda8; f32?=nan
    4f24: e58d5000     	str	r5, [sp]
    4f28: e1a03007     	mov	r3, r7
    4f2c: e08f100c     	add	r1, pc, r12
    4f30: e1a02000     	mov	r2, r0
    4f34: e1a00008     	mov	r0, r8
    4f38: ebfff638     	bl	0x2820 <.plt+0x320>     @ imm = #-0x2720  // CALL class_addmethod
    4f3c: e59f30bc     	ldr	r3, [pc, #0xbc]         @ 0x5000 <arbhar_rec_tilde_setup+0x320>  // u32=0x479c; f32?=2.56886034e-41
    4f40: e594800c     	ldr	r8, [r4, #0xc]
    4f44: e08f0003     	add	r0, pc, r3
    4f48: ebfff571     	bl	0x2514 <.plt+0x14>      @ imm = #-0x2a3c  // CALL gensym
    4f4c: e59f20b0     	ldr	r2, [pc, #0xb0]         @ 0x5004 <arbhar_rec_tilde_setup+0x324>  // u32=0x148; f32?=4.59625896e-43
    4f50: e1a03007     	mov	r3, r7
    4f54: e7961002     	ldr	r1, [r6, r2]
    4f58: e58d5000     	str	r5, [sp]
    4f5c: e59f50a4     	ldr	r5, [pc, #0xa4]         @ 0x5008 <arbhar_rec_tilde_setup+0x328>  // u32=0xfffff8d0; f32?=nan
    4f60: e1a02000     	mov	r2, r0
    4f64: e1a00008     	mov	r0, r8
    4f68: ebfff62c     	bl	0x2820 <.plt+0x320>     @ imm = #-0x2750  // CALL class_addmethod
    4f6c: e594000c     	ldr	r0, [r4, #0xc]
    4f70: e08f1005     	add	r1, pc, r5
    4f74: ebfff59f     	bl	0x25f8 <.plt+0xf8>      @ imm = #-0x2984  // CALL class_addbang
    4f78: e594000c     	ldr	r0, [r4, #0xc]
    4f7c: e3a01f95     	mov	r1, #596
    4f80: e28dd010     	add	sp, sp, #16
    4f84: e8bd41f0     	pop	{r4, r5, r6, r7, r8, lr}
    4f88: eafff61b     	b	0x27fc <.plt+0x2fc>     @ imm = #-0x2794  // CALL class_domainsignalin
    4f8c: 74 49 00 00  	.word	0x00004974
    4f90: f8 52 01 00  	.word	0x000152f8
    4f94: 44 01 00 00  	.word	0x00000144
    4f98: 40 01 00 00  	.word	0x00000140
    4f9c: 7c 54 01 00  	.word	0x0001547c
    4fa0: 34 49 00 00  	.word	0x00004934
    4fa4: 4c 01 00 00  	.word	0x0000014c
    4fa8: 08 49 00 00  	.word	0x00004908
    4fac: 38 dc ff ff  	.word	0xffffdc38
    4fb0: e4 48 00 00  	.word	0x000048e4
    4fb4: 34 e3 ff ff  	.word	0xffffe334
    4fb8: c4 48 00 00  	.word	0x000048c4
    4fbc: 14 dc ff ff  	.word	0xffffdc14
    4fc0: a0 48 00 00  	.word	0x000048a0
    4fc4: 28 dc ff ff  	.word	0xffffdc28
    4fc8: 78 48 00 00  	.word	0x00004878
    4fcc: d8 db ff ff  	.word	0xffffdbd8
    4fd0: 58 48 00 00  	.word	0x00004858
    4fd4: b8 db ff ff  	.word	0xffffdbb8
    4fd8: 38 48 00 00  	.word	0x00004838
    4fdc: 98 db ff ff  	.word	0xffffdb98
    4fe0: 1c 48 00 00  	.word	0x0000481c
    4fe4: 24 db ff ff  	.word	0xffffdb24
    4fe8: f8 47 00 00  	.word	0x000047f8
    4fec: 30 fb ff ff  	.word	0xfffffb30
    4ff0: d4 47 00 00  	.word	0x000047d4
    4ff4: bc de ff ff  	.word	0xffffdebc
    4ff8: b8 47 00 00  	.word	0x000047b8
    4ffc: a8 dd ff ff  	.word	0xffffdda8
    5000: 9c 47 00 00  	.word	0x0000479c
    5004: 48 01 00 00  	.word	0x00000148
    5008: d0 f8 ff ff  	.word	0xfffff8d0

