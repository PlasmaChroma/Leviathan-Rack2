00003e98 <arbhar_wtosc_tilde_setup>:
    3e98: e59f01cc     	ldr	r0, [pc, #0x1cc]        @ 0x406c <arbhar_wtosc_tilde_setup+0x1d4>  // u32=0x47c0; f32?=2.57390502e-41
    3e9c: e92d40f0     	push	{r4, r5, r6, r7, lr}
    3ea0: e08f0000     	add	r0, pc, r0
    3ea4: e24dd014     	sub	sp, sp, #20
    3ea8: e59f61c0     	ldr	r6, [pc, #0x1c0]        @ 0x4070 <arbhar_wtosc_tilde_setup+0x1d8>  // u32=0x15140; f32?=1.20982504e-40
    3eac: ebfff911     	bl	0x22f8 <.plt+0x14>      @ imm = #-0x1bbc  // CALL gensym
    3eb0: e59f21bc     	ldr	r2, [pc, #0x1bc]        @ 0x4074 <arbhar_wtosc_tilde_setup+0x1dc>  // u32=0x120; f32?=4.03573958e-43
    3eb4: e59f11bc     	ldr	r1, [pc, #0x1bc]        @ 0x4078 <arbhar_wtosc_tilde_setup+0x1e0>  // u32=0x11c; f32?=3.97968764e-43
    3eb8: e08f6006     	add	r6, pc, r6
    3ebc: e3a05000     	mov	r5, #0
    3ec0: e3a0c00a     	mov	r12, #10
    3ec4: e7962002     	ldr	r2, [r6, r2]
    3ec8: e3a03e13     	mov	r3, #304
    3ecc: e7961001     	ldr	r1, [r6, r1]
    3ed0: e88d1020     	stm	sp, {r5, r12}
    3ed4: e58d5008     	str	r5, [sp, #0x8]
    3ed8: ebfff9a5     	bl	0x2574 <.plt+0x290>     @ imm = #-0x196c  // CALL class_new
    3edc: e59f4198     	ldr	r4, [pc, #0x198]        @ 0x407c <arbhar_wtosc_tilde_setup+0x1e4>  // u32=0x15280; f32?=1.2143092e-40
    3ee0: e59f3198     	ldr	r3, [pc, #0x198]        @ 0x4080 <arbhar_wtosc_tilde_setup+0x1e8>  // u32=0x4784; f32?=2.56549723e-41
    3ee4: e08f4004     	add	r4, pc, r4
    3ee8: e1a07000     	mov	r7, r0
    3eec: e08f0003     	add	r0, pc, r3
    3ef0: e584700c     	str	r7, [r4, #0xc]
    3ef4: ebfff8ff     	bl	0x22f8 <.plt+0x14>      @ imm = #-0x1c04  // CALL gensym
    3ef8: e59fc184     	ldr	r12, [pc, #0x184]       @ 0x4084 <arbhar_wtosc_tilde_setup+0x1ec>  // u32=0x10c; f32?=3.75547988e-43
    3efc: e3a0300b     	mov	r3, #11
    3f00: e796100c     	ldr	r1, [r6, r12]
    3f04: e58d5000     	str	r5, [sp]
    3f08: e1a02000     	mov	r2, r0
    3f0c: e1a00007     	mov	r0, r7
    3f10: ebfff9a3     	bl	0x25a4 <.plt+0x2c0>     @ imm = #-0x1974  // CALL class_addmethod
    3f14: e59f016c     	ldr	r0, [pc, #0x16c]        @ 0x4088 <arbhar_wtosc_tilde_setup+0x1f0>  // u32=0x4758; f32?=2.55933152e-41
    3f18: e594600c     	ldr	r6, [r4, #0xc]
    3f1c: e08f0000     	add	r0, pc, r0
    3f20: ebfff8f4     	bl	0x22f8 <.plt+0x14>      @ imm = #-0x1c30  // CALL gensym
    3f24: e59f2160     	ldr	r2, [pc, #0x160]        @ 0x408c <arbhar_wtosc_tilde_setup+0x1f4>  // u32=0xffffe85c; f32?=nan
    3f28: e58d5000     	str	r5, [sp]
    3f2c: e3a03002     	mov	r3, #2
    3f30: e08f1002     	add	r1, pc, r2
    3f34: e1a02000     	mov	r2, r0
    3f38: e1a00006     	mov	r0, r6
    3f3c: ebfff998     	bl	0x25a4 <.plt+0x2c0>     @ imm = #-0x19a0  // CALL class_addmethod
    3f40: e59f1148     	ldr	r1, [pc, #0x148]        @ 0x4090 <arbhar_wtosc_tilde_setup+0x1f8>  // u32=0x4730; f32?=2.55372632e-41
    3f44: e594700c     	ldr	r7, [r4, #0xc]
    3f48: e08f0001     	add	r0, pc, r1
    3f4c: ebfff8e9     	bl	0x22f8 <.plt+0x14>      @ imm = #-0x1c5c  // CALL gensym
    3f50: e59fc13c     	ldr	r12, [pc, #0x13c]       @ 0x4094 <arbhar_wtosc_tilde_setup+0x1fc>  // u32=0xffffe79c; f32?=nan
    3f54: e58d5000     	str	r5, [sp]
    3f58: e3a03001     	mov	r3, #1
    3f5c: e08f100c     	add	r1, pc, r12
    3f60: e1a02000     	mov	r2, r0
    3f64: e1a00007     	mov	r0, r7
    3f68: ebfff98d     	bl	0x25a4 <.plt+0x2c0>     @ imm = #-0x19cc  // CALL class_addmethod
    3f6c: e59f3124     	ldr	r3, [pc, #0x124]        @ 0x4098 <arbhar_wtosc_tilde_setup+0x200>  // u32=0x470c; f32?=2.54868165e-41
    3f70: e594600c     	ldr	r6, [r4, #0xc]
    3f74: e08f0003     	add	r0, pc, r3
    3f78: ebfff8de     	bl	0x22f8 <.plt+0x14>      @ imm = #-0x1c88  // CALL gensym
    3f7c: e59f2118     	ldr	r2, [pc, #0x118]        @ 0x409c <arbhar_wtosc_tilde_setup+0x204>  // u32=0xffffe7cc; f32?=nan
    3f80: e58d5000     	str	r5, [sp]
    3f84: e3a03001     	mov	r3, #1
    3f88: e08f1002     	add	r1, pc, r2
    3f8c: e1a02000     	mov	r2, r0
    3f90: e1a00006     	mov	r0, r6
    3f94: ebfff982     	bl	0x25a4 <.plt+0x2c0>     @ imm = #-0x19f8  // CALL class_addmethod
    3f98: e59f0100     	ldr	r0, [pc, #0x100]        @ 0x40a0 <arbhar_wtosc_tilde_setup+0x208>  // u32=0x46e8; f32?=2.54363697e-41
    3f9c: e594700c     	ldr	r7, [r4, #0xc]
    3fa0: e08f0000     	add	r0, pc, r0
    3fa4: ebfff8d3     	bl	0x22f8 <.plt+0x14>      @ imm = #-0x1cb4  // CALL gensym
    3fa8: e59f10f4     	ldr	r1, [pc, #0xf4]         @ 0x40a4 <arbhar_wtosc_tilde_setup+0x20c>  // u32=0xffffe764; f32?=nan
    3fac: e58d5000     	str	r5, [sp]
    3fb0: e3a03001     	mov	r3, #1
    3fb4: e08f1001     	add	r1, pc, r1
    3fb8: e1a02000     	mov	r2, r0
    3fbc: e1a00007     	mov	r0, r7
    3fc0: ebfff977     	bl	0x25a4 <.plt+0x2c0>     @ imm = #-0x1a24  // CALL class_addmethod
    3fc4: e59fc0dc     	ldr	r12, [pc, #0xdc]        @ 0x40a8 <arbhar_wtosc_tilde_setup+0x210>  // u32=0x46c4; f32?=2.5385923e-41
    3fc8: e594600c     	ldr	r6, [r4, #0xc]
    3fcc: e08f000c     	add	r0, pc, r12
    3fd0: ebfff8c8     	bl	0x22f8 <.plt+0x14>      @ imm = #-0x1ce0  // CALL gensym
    3fd4: e59f20d0     	ldr	r2, [pc, #0xd0]         @ 0x40ac <arbhar_wtosc_tilde_setup+0x214>  // u32=0xffffe750; f32?=nan
    3fd8: e58d5000     	str	r5, [sp]
    3fdc: e3a03001     	mov	r3, #1
    3fe0: e08f1002     	add	r1, pc, r2
    3fe4: e1a02000     	mov	r2, r0
    3fe8: e1a00006     	mov	r0, r6
    3fec: ebfff96c     	bl	0x25a4 <.plt+0x2c0>     @ imm = #-0x1a50  // CALL class_addmethod
    3ff0: e59f30b8     	ldr	r3, [pc, #0xb8]         @ 0x40b0 <arbhar_wtosc_tilde_setup+0x218>  // u32=0x46a8; f32?=2.53466866e-41
    3ff4: e594700c     	ldr	r7, [r4, #0xc]
    3ff8: e08f0003     	add	r0, pc, r3
    3ffc: ebfff8bd     	bl	0x22f8 <.plt+0x14>      @ imm = #-0x1d0c  // CALL gensym
    4000: e59f10ac     	ldr	r1, [pc, #0xac]         @ 0x40b4 <arbhar_wtosc_tilde_setup+0x21c>  // u32=0xffffe740; f32?=nan
    4004: e58d5000     	str	r5, [sp]
    4008: e3a03001     	mov	r3, #1
    400c: e08f1001     	add	r1, pc, r1
    4010: e1a02000     	mov	r2, r0
    4014: e1a00007     	mov	r0, r7
    4018: ebfff961     	bl	0x25a4 <.plt+0x2c0>     @ imm = #-0x1a7c  // CALL class_addmethod
    401c: e59f0094     	ldr	r0, [pc, #0x94]         @ 0x40b8 <arbhar_wtosc_tilde_setup+0x220>  // u32=0x4688; f32?=2.53018451e-41
    4020: e594600c     	ldr	r6, [r4, #0xc]
    4024: e08f0000     	add	r0, pc, r0
    4028: ebfff8b2     	bl	0x22f8 <.plt+0x14>      @ imm = #-0x1d38  // CALL gensym
    402c: e58d5000     	str	r5, [sp]
    4030: e59f5084     	ldr	r5, [pc, #0x84]         @ 0x40bc <arbhar_wtosc_tilde_setup+0x224>  // u32=0xffffe85c; f32?=nan
    4034: e3a03001     	mov	r3, #1
    4038: e08f1005     	add	r1, pc, r5
    403c: e1a02000     	mov	r2, r0
    4040: e1a00006     	mov	r0, r6
    4044: ebfff956     	bl	0x25a4 <.plt+0x2c0>     @ imm = #-0x1aa8  // CALL class_addmethod
    4048: e59fc070     	ldr	r12, [pc, #0x70]        @ 0x40c0 <arbhar_wtosc_tilde_setup+0x228>  // u32=0xffffe7f8; f32?=nan
    404c: e594000c     	ldr	r0, [r4, #0xc]
    4050: e08f100c     	add	r1, pc, r12
    4054: ebfff8d7     	bl	0x23b8 <.plt+0xd4>      @ imm = #-0x1ca4  // CALL class_addbang
    4058: e594000c     	ldr	r0, [r4, #0xc]
    405c: e3a01034     	mov	r1, #52
    4060: e28dd014     	add	sp, sp, #20
    4064: e8bd40f0     	pop	{r4, r5, r6, r7, lr}
    4068: eafff944     	b	0x2580 <.plt+0x29c>     @ imm = #-0x1af0  // CALL class_domainsignalin
    406c: c0 47 00 00  	.word	0x000047c0
    4070: 40 51 01 00  	.word	0x00015140
    4074: 20 01 00 00  	.word	0x00000120
    4078: 1c 01 00 00  	.word	0x0000011c
    407c: 80 52 01 00  	.word	0x00015280
    4080: 84 47 00 00  	.word	0x00004784
    4084: 0c 01 00 00  	.word	0x0000010c
    4088: 58 47 00 00  	.word	0x00004758
    408c: 5c e8 ff ff  	.word	0xffffe85c
    4090: 30 47 00 00  	.word	0x00004730
    4094: 9c e7 ff ff  	.word	0xffffe79c
    4098: 0c 47 00 00  	.word	0x0000470c
    409c: cc e7 ff ff  	.word	0xffffe7cc
    40a0: e8 46 00 00  	.word	0x000046e8
    40a4: 64 e7 ff ff  	.word	0xffffe764
    40a8: c4 46 00 00  	.word	0x000046c4
    40ac: 50 e7 ff ff  	.word	0xffffe750
    40b0: a8 46 00 00  	.word	0x000046a8
    40b4: 40 e7 ff ff  	.word	0xffffe740
    40b8: 88 46 00 00  	.word	0x00004688
    40bc: 5c e8 ff ff  	.word	0xffffe85c
    40c0: f8 e7 ff ff  	.word	0xffffe7f8

