000027ec <arbhar_shmem_setup>:
    27ec: e59f0224     	ldr	r0, [pc, #0x224]        @ 0x2a18 <arbhar_shmem_setup+0x22c>  // u32=0x660; f32?=2.28691909e-42
    27f0: e92d41f0     	push	{r4, r5, r6, r7, r8, lr}
    27f4: e08f0000     	add	r0, pc, r0
    27f8: e24dd010     	sub	sp, sp, #16
    27fc: e59f4218     	ldr	r4, [pc, #0x218]        @ 0x2a1c <arbhar_shmem_setup+0x230>  // u32=0x107ec; f32?=9.46773294e-41
    2800: ebfff880     	bl	0xa08 <.plt+0x14>       @ imm = #-0x1e00  // CALL gensym
    2804: e59f2214     	ldr	r2, [pc, #0x214]        @ 0x2a20 <arbhar_shmem_setup+0x234>  // u32=0xc0; f32?=2.69049305e-43
    2808: e59f1214     	ldr	r1, [pc, #0x214]        @ 0x2a24 <arbhar_shmem_setup+0x238>  // u32=0xd8; f32?=3.02680468e-43
    280c: e08f4004     	add	r4, pc, r4
    2810: e3a05000     	mov	r5, #0
    2814: e3a030c0     	mov	r3, #192
    2818: e7942002     	ldr	r2, [r4, r2]
    281c: e3a07001     	mov	r7, #1
    2820: e7941001     	ldr	r1, [r4, r1]
    2824: e58d5004     	str	r5, [sp, #0x4]
    2828: e58d5000     	str	r5, [sp]
    282c: ebfff8c6     	bl	0xb4c <.plt+0x158>      @ imm = #-0x1ce8  // CALL class_new
    2830: e59f31f0     	ldr	r3, [pc, #0x1f0]        @ 0x2a28 <arbhar_shmem_setup+0x23c>  // u32=0xc8; f32?=2.80259693e-43
    2834: e59f61f0     	ldr	r6, [pc, #0x1f0]        @ 0x2a2c <arbhar_shmem_setup+0x240>  // u32=0xffffe4e0; f32?=nan
    2838: e59f81f0     	ldr	r8, [pc, #0x1f0]        @ 0x2a30 <arbhar_shmem_setup+0x244>  // u32=0x618; f32?=2.1860256e-42
    283c: e08f1006     	add	r1, pc, r6
    2840: e7946003     	ldr	r6, [r4, r3]
    2844: e5860000     	str	r0, [r6]
    2848: ebfff883     	bl	0xa5c <.plt+0x68>       @ imm = #-0x1df4  // CALL class_addbang
    284c: e08f0008     	add	r0, pc, r8
    2850: e5968000     	ldr	r8, [r6]
    2854: ebfff86b     	bl	0xa08 <.plt+0x14>       @ imm = #-0x1e54  // CALL gensym
    2858: e59fc1d4     	ldr	r12, [pc, #0x1d4]       @ 0x2a34 <arbhar_shmem_setup+0x248>  // u32=0xac; f32?=2.41023336e-43
    285c: e3a0300a     	mov	r3, #10
    2860: e794100c     	ldr	r1, [r4, r12]
    2864: e58d5000     	str	r5, [sp]
    2868: e1a02000     	mov	r2, r0
    286c: e1a00008     	mov	r0, r8
    2870: ebfff8c1     	bl	0xb7c <.plt+0x188>      @ imm = #-0x1cfc  // CALL class_addmethod
    2874: e59f01bc     	ldr	r0, [pc, #0x1bc]        @ 0x2a38 <arbhar_shmem_setup+0x24c>  // u32=0x5f0; f32?=2.12997367e-42
    2878: e5968000     	ldr	r8, [r6]
    287c: e08f0000     	add	r0, pc, r0
    2880: ebfff860     	bl	0xa08 <.plt+0x14>       @ imm = #-0x1e80  // CALL gensym
    2884: e59f21b0     	ldr	r2, [pc, #0x1b0]        @ 0x2a3c <arbhar_shmem_setup+0x250>  // u32=0xb8; f32?=2.57838917e-43
    2888: e3a0300a     	mov	r3, #10
    288c: e7941002     	ldr	r1, [r4, r2]
    2890: e58d5000     	str	r5, [sp]
    2894: e1a02000     	mov	r2, r0
    2898: e1a00008     	mov	r0, r8
    289c: ebfff8b6     	bl	0xb7c <.plt+0x188>      @ imm = #-0x1d28  // CALL class_addmethod
    28a0: e59f1198     	ldr	r1, [pc, #0x198]        @ 0x2a40 <arbhar_shmem_setup+0x254>  // u32=0x5cc; f32?=2.07952692e-42
    28a4: e5968000     	ldr	r8, [r6]
    28a8: e08f0001     	add	r0, pc, r1
    28ac: ebfff855     	bl	0xa08 <.plt+0x14>       @ imm = #-0x1eac  // CALL gensym
    28b0: e59fc18c     	ldr	r12, [pc, #0x18c]       @ 0x2a44 <arbhar_shmem_setup+0x258>  // u32=0x9c; f32?=2.1860256e-43
    28b4: e3a0300a     	mov	r3, #10
    28b8: e794100c     	ldr	r1, [r4, r12]
    28bc: e58d5000     	str	r5, [sp]
    28c0: e1a02000     	mov	r2, r0
    28c4: e1a00008     	mov	r0, r8
    28c8: ebfff8ab     	bl	0xb7c <.plt+0x188>      @ imm = #-0x1d54  // CALL class_addmethod
    28cc: e59f3174     	ldr	r3, [pc, #0x174]        @ 0x2a48 <arbhar_shmem_setup+0x25c>  // u32=0x5b0; f32?=2.04029056e-42
    28d0: e5968000     	ldr	r8, [r6]
    28d4: e08f0003     	add	r0, pc, r3
    28d8: ebfff84a     	bl	0xa08 <.plt+0x14>       @ imm = #-0x1ed8  // CALL gensym
    28dc: e59f2168     	ldr	r2, [pc, #0x168]        @ 0x2a4c <arbhar_shmem_setup+0x260>  // u32=0xb4; f32?=2.52233724e-43
    28e0: e1a03007     	mov	r3, r7
    28e4: e7941002     	ldr	r1, [r4, r2]
    28e8: e58d5008     	str	r5, [sp, #0x8]
    28ec: e58d7004     	str	r7, [sp, #0x4]
    28f0: e58d7000     	str	r7, [sp]
    28f4: e1a02000     	mov	r2, r0
    28f8: e1a00008     	mov	r0, r8
    28fc: ebfff89e     	bl	0xb7c <.plt+0x188>      @ imm = #-0x1d88  // CALL class_addmethod
    2900: e59f0148     	ldr	r0, [pc, #0x148]        @ 0x2a50 <arbhar_shmem_setup+0x264>  // u32=0x588; f32?=1.98423863e-42
    2904: e5968000     	ldr	r8, [r6]
    2908: e08f0000     	add	r0, pc, r0
    290c: ebfff83d     	bl	0xa08 <.plt+0x14>       @ imm = #-0x1f0c  // CALL gensym
    2910: e59f113c     	ldr	r1, [pc, #0x13c]        @ 0x2a54 <arbhar_shmem_setup+0x268>  // u32=0xcc; f32?=2.85864887e-43
    2914: e3a0300a     	mov	r3, #10
    2918: e7941001     	ldr	r1, [r4, r1]
    291c: e58d5000     	str	r5, [sp]
    2920: e1a02000     	mov	r2, r0
    2924: e1a00008     	mov	r0, r8
    2928: ebfff893     	bl	0xb7c <.plt+0x188>      @ imm = #-0x1db4  // CALL class_addmethod
    292c: e59fc124     	ldr	r12, [pc, #0x124]       @ 0x2a58 <arbhar_shmem_setup+0x26c>  // u32=0x564; f32?=1.93379188e-42
    2930: e5968000     	ldr	r8, [r6]
    2934: e08f000c     	add	r0, pc, r12
    2938: ebfff832     	bl	0xa08 <.plt+0x14>       @ imm = #-0x1f38  // CALL gensym
    293c: e59f2118     	ldr	r2, [pc, #0x118]        @ 0x2a5c <arbhar_shmem_setup+0x270>  // u32=0xa8; f32?=2.35418142e-43
    2940: e1a03007     	mov	r3, r7
    2944: e7941002     	ldr	r1, [r4, r2]
    2948: e58d5000     	str	r5, [sp]
    294c: e1a02000     	mov	r2, r0
    2950: e1a00008     	mov	r0, r8
    2954: ebfff888     	bl	0xb7c <.plt+0x188>      @ imm = #-0x1de0  // CALL class_addmethod
    2958: e59f3100     	ldr	r3, [pc, #0x100]        @ 0x2a60 <arbhar_shmem_setup+0x274>  // u32=0x544; f32?=1.88895033e-42
    295c: e5968000     	ldr	r8, [r6]
    2960: e08f0003     	add	r0, pc, r3
    2964: ebfff827     	bl	0xa08 <.plt+0x14>       @ imm = #-0x1f64  // CALL gensym
    2968: e59f10f4     	ldr	r1, [pc, #0xf4]         @ 0x2a64 <arbhar_shmem_setup+0x278>  // u32=0xdc; f32?=3.08285662e-43
    296c: e1a03007     	mov	r3, r7
    2970: e7941001     	ldr	r1, [r4, r1]
    2974: e58d5004     	str	r5, [sp, #0x4]
    2978: e58d7000     	str	r7, [sp]
    297c: e1a02000     	mov	r2, r0
    2980: e1a00008     	mov	r0, r8
    2984: ebfff87c     	bl	0xb7c <.plt+0x188>      @ imm = #-0x1e10  // CALL class_addmethod
    2988: e59f00d8     	ldr	r0, [pc, #0xd8]         @ 0x2a68 <arbhar_shmem_setup+0x27c>  // u32=0x51c; f32?=1.83289839e-42
    298c: e5968000     	ldr	r8, [r6]
    2990: e08f0000     	add	r0, pc, r0
    2994: ebfff81b     	bl	0xa08 <.plt+0x14>       @ imm = #-0x1f94  // CALL gensym
    2998: e59fc0cc     	ldr	r12, [pc, #0xcc]        @ 0x2a6c <arbhar_shmem_setup+0x280>  // u32=0xc4; f32?=2.74654499e-43
    299c: e1a03007     	mov	r3, r7
    29a0: e794100c     	ldr	r1, [r4, r12]
    29a4: e59f40c4     	ldr	r4, [pc, #0xc4]         @ 0x2a70 <arbhar_shmem_setup+0x284>  // u32=0x4fc; f32?=1.78805684e-42
    29a8: e58d5004     	str	r5, [sp, #0x4]
    29ac: e58d7000     	str	r7, [sp]
    29b0: e1a02000     	mov	r2, r0
    29b4: e1a00008     	mov	r0, r8
    29b8: ebfff86f     	bl	0xb7c <.plt+0x188>      @ imm = #-0x1e44  // CALL class_addmethod
    29bc: e08f0004     	add	r0, pc, r4
    29c0: e5967000     	ldr	r7, [r6]
    29c4: ebfff80f     	bl	0xa08 <.plt+0x14>       @ imm = #-0x1fc4  // CALL gensym
    29c8: e59f20a4     	ldr	r2, [pc, #0xa4]         @ 0x2a74 <arbhar_shmem_setup+0x288>  // u32=0xfffff23c; f32?=nan
    29cc: e3a0300a     	mov	r3, #10
    29d0: e58d5000     	str	r5, [sp]
    29d4: e08f1002     	add	r1, pc, r2
    29d8: e1a02000     	mov	r2, r0
    29dc: e1a00007     	mov	r0, r7
    29e0: ebfff865     	bl	0xb7c <.plt+0x188>      @ imm = #-0x1e6c  // CALL class_addmethod
    29e4: e59f308c     	ldr	r3, [pc, #0x8c]         @ 0x2a78 <arbhar_shmem_setup+0x28c>  // u32=0x4dc; f32?=1.74321529e-42
    29e8: e5966000     	ldr	r6, [r6]
    29ec: e08f0003     	add	r0, pc, r3
    29f0: ebfff804     	bl	0xa08 <.plt+0x14>       @ imm = #-0x1ff0  // CALL gensym
    29f4: e58d5000     	str	r5, [sp]
    29f8: e59f507c     	ldr	r5, [pc, #0x7c]         @ 0x2a7c <arbhar_shmem_setup+0x290>  // u32=0xfffff0bc; f32?=nan
    29fc: e3a0300a     	mov	r3, #10
    2a00: e08f1005     	add	r1, pc, r5
    2a04: e1a02000     	mov	r2, r0
    2a08: e1a00006     	mov	r0, r6
    2a0c: ebfff85a     	bl	0xb7c <.plt+0x188>      @ imm = #-0x1e98  // CALL class_addmethod
    2a10: e28dd010     	add	sp, sp, #16
    2a14: e8bd81f0     	pop	{r4, r5, r6, r7, r8, pc}
    2a18: 60 06 00 00  	.word	0x00000660
    2a1c: ec 07 01 00  	.word	0x000107ec
    2a20: c0 00 00 00  	.word	0x000000c0
    2a24: d8 00 00 00  	.word	0x000000d8
    2a28: c8 00 00 00  	.word	0x000000c8
    2a2c: e0 e4 ff ff  	.word	0xffffe4e0
    2a30: 18 06 00 00  	.word	0x00000618
    2a34: ac 00 00 00  	.word	0x000000ac
    2a38: f0 05 00 00  	.word	0x000005f0
    2a3c: b8 00 00 00  	.word	0x000000b8
    2a40: cc 05 00 00  	.word	0x000005cc
    2a44: 9c 00 00 00  	.word	0x0000009c
    2a48: b0 05 00 00  	.word	0x000005b0
    2a4c: b4 00 00 00  	.word	0x000000b4
    2a50: 88 05 00 00  	.word	0x00000588
    2a54: cc 00 00 00  	.word	0x000000cc
    2a58: 64 05 00 00  	.word	0x00000564
    2a5c: a8 00 00 00  	.word	0x000000a8
    2a60: 44 05 00 00  	.word	0x00000544
    2a64: dc 00 00 00  	.word	0x000000dc
    2a68: 1c 05 00 00  	.word	0x0000051c
    2a6c: c4 00 00 00  	.word	0x000000c4
    2a70: fc 04 00 00  	.word	0x000004fc
    2a74: 3c f2 ff ff  	.word	0xfffff23c
    2a78: dc 04 00 00  	.word	0x000004dc
    2a7c: bc f0 ff ff  	.word	0xfffff0bc

Disassembly of section .fini:

