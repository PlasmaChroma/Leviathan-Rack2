00002ec8 <comport_info>:
    2ec8: e92d43f0     	push	{r4, r5, r6, r7, r8, r9, lr}
    2ecc: ed2d8b02     	vpush	{d8}
    2ed0: e5903020     	ldr	r3, [r0, #0x20]
    2ed4: e1a07000     	mov	r7, r0
    2ed8: e3730001     	cmn	r3, #1
    2edc: e24dd00c     	sub	sp, sp, #12
    2ee0: 0a0000fd     	beq	0x32dc <comport_info+0x414> @ imm = #0x3f4
    2ee4: e59f041c     	ldr	r0, [pc, #0x41c]        @ 0x3308 <comport_info+0x440>  // u32=0x1af4; f32?=9.6689594e-42
    2ee8: e3a085fe     	mov	r8, #1065353216
    2eec: e08f0000     	add	r0, pc, r0
    2ef0: ebfff6c9     	bl	0xa1c <.plt+0x14>       @ imm = #-0x24dc  // CALL gensym
    2ef4: e1a06000     	mov	r6, r0
    2ef8: e3a00008     	mov	r0, #8
    2efc: ebfff6d5     	bl	0xa58 <.plt+0x50>       @ imm = #-0x24ac  // CALL getbytes
    2f00: e1a05000     	mov	r5, r0
    2f04: e2874a01     	add	r4, r7, #4096
    2f08: e3a02001     	mov	r2, #1
    2f0c: e1a03005     	mov	r3, r5
    2f10: e5858004     	str	r8, [r5, #0x4]
    2f14: e5852000     	str	r2, [r5]
    2f18: e1a01006     	mov	r1, r6
    2f1c: e59400e8     	ldr	r0, [r4, #0xe8]
    2f20: ebfff71d     	bl	0xb9c <.plt+0x194>      @ imm = #-0x238c  // CALL outlet_anything
    2f24: e3a01008     	mov	r1, #8
    2f28: e1a00005     	mov	r0, r5
    2f2c: ebfff720     	bl	0xbb4 <.plt+0x1ac>      @ imm = #-0x2380  // CALL freebytes
    2f30: e59f23d4     	ldr	r2, [pc, #0x3d4]        @ 0x330c <comport_info+0x444>  // u32=0x1ab0; f32?=9.57367111e-42
    2f34: e3a06001     	mov	r6, #1
    2f38: e08f0002     	add	r0, pc, r2
    2f3c: ebfff6b6     	bl	0xa1c <.plt+0x14>       @ imm = #-0x2528  // CALL gensym
    2f40: e1d4caf0     	ldrsh	r12, [r4, #160]
    2f44: ee08ca10     	vmov	s16, r12
    2f48: eef88ac8     	vcvt.f32.s32	s17, s16
    2f4c: e1a09000     	mov	r9, r0
    2f50: e3a00008     	mov	r0, #8
    2f54: ebfff6bf     	bl	0xa58 <.plt+0x50>       @ imm = #-0x2504  // CALL getbytes
    2f58: e1a02006     	mov	r2, r6
    2f5c: e1a01009     	mov	r1, r9
    2f60: edc08a01     	vstr	s17, [r0, #4]
    2f64: e1a03000     	mov	r3, r0
    2f68: e1a05000     	mov	r5, r0
    2f6c: e5806000     	str	r6, [r0]
    2f70: e59400e8     	ldr	r0, [r4, #0xe8]
    2f74: ebfff708     	bl	0xb9c <.plt+0x194>      @ imm = #-0x23e0  // CALL outlet_anything
    2f78: e3a01008     	mov	r1, #8
    2f7c: e1a00005     	mov	r0, r5
    2f80: ebfff70b     	bl	0xbb4 <.plt+0x1ac>      @ imm = #-0x23d4  // CALL freebytes
    2f84: e59f3384     	ldr	r3, [pc, #0x384]        @ 0x3310 <comport_info+0x448>  // u32=0x1a68; f32?=9.47277762e-42
    2f88: e08f0003     	add	r0, pc, r3
    2f8c: ebfff6a2     	bl	0xa1c <.plt+0x14>       @ imm = #-0x2578  // CALL gensym
    2f90: e59490a4     	ldr	r9, [r4, #0xa4]
    2f94: e1a08000     	mov	r8, r0
    2f98: e3a00008     	mov	r0, #8
    2f9c: ebfff6ad     	bl	0xa58 <.plt+0x50>       @ imm = #-0x254c  // CALL getbytes
    2fa0: e1a02006     	mov	r2, r6
    2fa4: e1a01008     	mov	r1, r8
    2fa8: e1a03000     	mov	r3, r0
    2fac: e5806000     	str	r6, [r0]
    2fb0: e1a05000     	mov	r5, r0
    2fb4: e5809004     	str	r9, [r0, #0x4]
    2fb8: e59400e8     	ldr	r0, [r4, #0xe8]
    2fbc: ebfff6f6     	bl	0xb9c <.plt+0x194>      @ imm = #-0x2428  // CALL outlet_anything
    2fc0: e1a00005     	mov	r0, r5
    2fc4: e3a01008     	mov	r1, #8
    2fc8: ebfff6f9     	bl	0xbb4 <.plt+0x1ac>      @ imm = #-0x241c  // CALL freebytes
    2fcc: e59f0340     	ldr	r0, [pc, #0x340]        @ 0x3314 <comport_info+0x44c>  // u32=0x1a28; f32?=9.38309452e-42
    2fd0: e08f0000     	add	r0, pc, r0
    2fd4: ebfff690     	bl	0xa1c <.plt+0x14>       @ imm = #-0x25c0  // CALL gensym
    2fd8: e1a06000     	mov	r6, r0
    2fdc: e5970020     	ldr	r0, [r7, #0x20]
    2fe0: e3700001     	cmn	r0, #1
    2fe4: 0a0000b8     	beq	0x32cc <comport_info+0x404> @ imm = #0x2e0
    2fe8: e28d2004     	add	r2, sp, #4
    2fec: e59f1324     	ldr	r1, [pc, #0x324]        @ 0x3318 <comport_info+0x450>  // u32=0x5415; f32?=3.01629494e-41
    2ff0: ebfff6a7     	bl	0xa94 <.plt+0x8c>       @ imm = #-0x2564  // CALL ioctl
    2ff4: e59d1004     	ldr	r1, [sp, #0x4]
    2ff8: e2012001     	and	r2, r1, #1
    2ffc: ee072a90     	vmov	s15, r2
    3000: eeb88ae7     	vcvt.f32.s32	s16, s15
    3004: e3a00008     	mov	r0, #8
    3008: ebfff692     	bl	0xa58 <.plt+0x50>       @ imm = #-0x25b8  // CALL getbytes
    300c: e3a02001     	mov	r2, #1
    3010: e1a01006     	mov	r1, r6
    3014: ed808a01     	vstr	s16, [r0, #4]
    3018: e1a08000     	mov	r8, r0
    301c: e5802000     	str	r2, [r0]
    3020: e1a03008     	mov	r3, r8
    3024: e59400e8     	ldr	r0, [r4, #0xe8]
    3028: ebfff6db     	bl	0xb9c <.plt+0x194>      @ imm = #-0x2494  // CALL outlet_anything
    302c: e1a00008     	mov	r0, r8
    3030: e3a01008     	mov	r1, #8
    3034: ebfff6de     	bl	0xbb4 <.plt+0x1ac>      @ imm = #-0x2488  // CALL freebytes
    3038: e59fc2dc     	ldr	r12, [pc, #0x2dc]       @ 0x331c <comport_info+0x454>  // u32=0x19c0; f32?=9.23735948e-42
    303c: e08f000c     	add	r0, pc, r12
    3040: ebfff675     	bl	0xa1c <.plt+0x14>       @ imm = #-0x262c  // CALL gensym
    3044: e1a09000     	mov	r9, r0
    3048: e5970020     	ldr	r0, [r7, #0x20]
    304c: e3700001     	cmn	r0, #1
    3050: 0a00009f     	beq	0x32d4 <comport_info+0x40c> @ imm = #0x27c
    3054: e28d2004     	add	r2, sp, #4
    3058: e59f12b8     	ldr	r1, [pc, #0x2b8]        @ 0x3318 <comport_info+0x450>  // u32=0x5415; f32?=3.01629494e-41
    305c: ebfff68c     	bl	0xa94 <.plt+0x8c>       @ imm = #-0x25d0  // CALL ioctl
    3060: e59d7004     	ldr	r7, [sp, #0x4]
    3064: e1a032a7     	lsr	r3, r7, #5
    3068: e2035001     	and	r5, r3, #1
    306c: ee005a10     	vmov	s0, r5
    3070: eef88ac0     	vcvt.f32.s32	s17, s0
    3074: e3a00008     	mov	r0, #8
    3078: ebfff676     	bl	0xa58 <.plt+0x50>       @ imm = #-0x2628  // CALL getbytes
    307c: e3a05001     	mov	r5, #1
    3080: e1a02005     	mov	r2, r5
    3084: e1a01009     	mov	r1, r9
    3088: edc08a01     	vstr	s17, [r0, #4]
    308c: e1a03000     	mov	r3, r0
    3090: e1a06000     	mov	r6, r0
    3094: e5805000     	str	r5, [r0]
    3098: e59400e8     	ldr	r0, [r4, #0xe8]
    309c: ebfff6be     	bl	0xb9c <.plt+0x194>      @ imm = #-0x2508  // CALL outlet_anything
    30a0: e3a01008     	mov	r1, #8
    30a4: e1a00006     	mov	r0, r6
    30a8: ebfff6c1     	bl	0xbb4 <.plt+0x1ac>      @ imm = #-0x24fc  // CALL freebytes
    30ac: e59f026c     	ldr	r0, [pc, #0x26c]        @ 0x3320 <comport_info+0x458>  // u32=0x1950; f32?=9.08041405e-42
    30b0: e08f0000     	add	r0, pc, r0
    30b4: ebfff658     	bl	0xa1c <.plt+0x14>       @ imm = #-0x26a0  // CALL gensym
    30b8: e59490ac     	ldr	r9, [r4, #0xac]
    30bc: e1a08000     	mov	r8, r0
    30c0: e3a00008     	mov	r0, #8
    30c4: ebfff663     	bl	0xa58 <.plt+0x50>       @ imm = #-0x2674  // CALL getbytes
    30c8: e1a02005     	mov	r2, r5
    30cc: e1a01008     	mov	r1, r8
    30d0: e1a03000     	mov	r3, r0
    30d4: e1a07000     	mov	r7, r0
    30d8: e5809004     	str	r9, [r0, #0x4]
    30dc: e5805000     	str	r5, [r0]
    30e0: e59400e8     	ldr	r0, [r4, #0xe8]
    30e4: ebfff6ac     	bl	0xb9c <.plt+0x194>      @ imm = #-0x2550  // CALL outlet_anything
    30e8: e1a00007     	mov	r0, r7
    30ec: e3a01008     	mov	r1, #8
    30f0: ebfff6af     	bl	0xbb4 <.plt+0x1ac>      @ imm = #-0x2544  // CALL freebytes
    30f4: e59f1228     	ldr	r1, [pc, #0x228]        @ 0x3324 <comport_info+0x45c>  // u32=0x1910; f32?=8.99073095e-42
    30f8: e08f0001     	add	r0, pc, r1
    30fc: ebfff646     	bl	0xa1c <.plt+0x14>       @ imm = #-0x26e8  // CALL gensym
    3100: edd40a2c     	vldr	s1, [r4, #176]
    3104: ed9f1a7d     	vldr	s2, [pc, #500]          @ 0x3300 <comport_info+0x438>  // f32=1
    3108: ee308a81     	vadd.f32	s16, s1, s2
    310c: e1a08000     	mov	r8, r0
    3110: e3a00008     	mov	r0, #8
    3114: ebfff64f     	bl	0xa58 <.plt+0x50>       @ imm = #-0x26c4  // CALL getbytes
    3118: e1a02005     	mov	r2, r5
    311c: e1a01008     	mov	r1, r8
    3120: ed808a01     	vstr	s16, [r0, #4]
    3124: e1a03000     	mov	r3, r0
    3128: e1a06000     	mov	r6, r0
    312c: e5805000     	str	r5, [r0]
    3130: e59400e8     	ldr	r0, [r4, #0xe8]
    3134: ebfff698     	bl	0xb9c <.plt+0x194>      @ imm = #-0x25a0  // CALL outlet_anything
    3138: e3a01008     	mov	r1, #8
    313c: e1a00006     	mov	r0, r6
    3140: ebfff69b     	bl	0xbb4 <.plt+0x1ac>      @ imm = #-0x2594  // CALL freebytes
    3144: e59f21dc     	ldr	r2, [pc, #0x1dc]        @ 0x3328 <comport_info+0x460>  // u32=0x18c8; f32?=8.88983746e-42
    3148: e08f0002     	add	r0, pc, r2
    314c: ebfff632     	bl	0xa1c <.plt+0x14>       @ imm = #-0x2738  // CALL gensym
    3150: e59470a8     	ldr	r7, [r4, #0xa8]
    3154: e1a09000     	mov	r9, r0
    3158: e3a00008     	mov	r0, #8
    315c: ebfff63d     	bl	0xa58 <.plt+0x50>       @ imm = #-0x270c  // CALL getbytes
    3160: e1a02005     	mov	r2, r5
    3164: e1a01009     	mov	r1, r9
    3168: e1a03000     	mov	r3, r0
    316c: e1a08000     	mov	r8, r0
    3170: e5807004     	str	r7, [r0, #0x4]
    3174: e5805000     	str	r5, [r0]
    3178: e59400e8     	ldr	r0, [r4, #0xe8]
    317c: ebfff686     	bl	0xb9c <.plt+0x194>      @ imm = #-0x25e8  // CALL outlet_anything
    3180: e3a01008     	mov	r1, #8
    3184: e1a00008     	mov	r0, r8
    3188: ebfff689     	bl	0xbb4 <.plt+0x1ac>      @ imm = #-0x25dc  // CALL freebytes
    318c: e59fc198     	ldr	r12, [pc, #0x198]       @ 0x332c <comport_info+0x464>  // u32=0x1888; f32?=8.80015436e-42
    3190: e08f000c     	add	r0, pc, r12
    3194: ebfff620     	bl	0xa1c <.plt+0x14>       @ imm = #-0x2780  // CALL gensym
    3198: edd41a2e     	vldr	s3, [r4, #184]
    319c: eef88ae1     	vcvt.f32.s32	s17, s3
    31a0: e1a09000     	mov	r9, r0
    31a4: e3a00008     	mov	r0, #8
    31a8: ebfff62a     	bl	0xa58 <.plt+0x50>       @ imm = #-0x2758  // CALL getbytes
    31ac: e1a02005     	mov	r2, r5
    31b0: e1a01009     	mov	r1, r9
    31b4: edc08a01     	vstr	s17, [r0, #4]
    31b8: e1a03000     	mov	r3, r0
    31bc: e1a06000     	mov	r6, r0
    31c0: e5805000     	str	r5, [r0]
    31c4: e59400e8     	ldr	r0, [r4, #0xe8]
    31c8: ebfff673     	bl	0xb9c <.plt+0x194>      @ imm = #-0x2634  // CALL outlet_anything
    31cc: e3a01008     	mov	r1, #8
    31d0: e1a00006     	mov	r0, r6
    31d4: ebfff676     	bl	0xbb4 <.plt+0x1ac>      @ imm = #-0x2628  // CALL freebytes
    31d8: e59f3150     	ldr	r3, [pc, #0x150]        @ 0x3330 <comport_info+0x468>  // u32=0x1844; f32?=8.70486606e-42
    31dc: e08f0003     	add	r0, pc, r3
    31e0: ebfff60d     	bl	0xa1c <.plt+0x14>       @ imm = #-0x27cc  // CALL gensym
    31e4: ed942a2d     	vldr	s4, [r4, #180]
    31e8: eeb88ac2     	vcvt.f32.s32	s16, s4
    31ec: e1a07000     	mov	r7, r0
    31f0: e3a00008     	mov	r0, #8
    31f4: ebfff617     	bl	0xa58 <.plt+0x50>       @ imm = #-0x27a4  // CALL getbytes
    31f8: e1a02005     	mov	r2, r5
    31fc: e1a01007     	mov	r1, r7
    3200: ed808a01     	vstr	s16, [r0, #4]
    3204: e1a03000     	mov	r3, r0
    3208: e1a08000     	mov	r8, r0
    320c: e5805000     	str	r5, [r0]
    3210: e59400e8     	ldr	r0, [r4, #0xe8]
    3214: ebfff660     	bl	0xb9c <.plt+0x194>      @ imm = #-0x2680  // CALL outlet_anything
    3218: e3a01008     	mov	r1, #8
    321c: e1a00008     	mov	r0, r8
    3220: ebfff663     	bl	0xbb4 <.plt+0x1ac>      @ imm = #-0x2674  // CALL freebytes
    3224: e59f0108     	ldr	r0, [pc, #0x108]        @ 0x3334 <comport_info+0x46c>  // u32=0x1800; f32?=8.60957776e-42
    3228: e08f0000     	add	r0, pc, r0
    322c: ebfff5fa     	bl	0xa1c <.plt+0x14>       @ imm = #-0x2818  // CALL gensym
    3230: edd42a2f     	vldr	s5, [r4, #188]
    3234: eef88ae2     	vcvt.f32.s32	s17, s5
    3238: e1a09000     	mov	r9, r0
    323c: e3a00008     	mov	r0, #8
    3240: ebfff604     	bl	0xa58 <.plt+0x50>       @ imm = #-0x27f0  // CALL getbytes
    3244: e1a02005     	mov	r2, r5
    3248: e1a01009     	mov	r1, r9
    324c: edc08a01     	vstr	s17, [r0, #4]
    3250: e1a03000     	mov	r3, r0
    3254: e1a06000     	mov	r6, r0
    3258: e5805000     	str	r5, [r0]
    325c: e59400e8     	ldr	r0, [r4, #0xe8]
    3260: ebfff64d     	bl	0xb9c <.plt+0x194>      @ imm = #-0x26cc  // CALL outlet_anything
    3264: e1a00006     	mov	r0, r6
    3268: e3a01008     	mov	r1, #8
    326c: ebfff650     	bl	0xbb4 <.plt+0x1ac>      @ imm = #-0x26c0  // CALL freebytes
    3270: e59f10c0     	ldr	r1, [pc, #0xc0]         @ 0x3338 <comport_info+0x470>  // u32=0x17bc; f32?=8.51428947e-42
    3274: e08f0001     	add	r0, pc, r1
    3278: ebfff5e7     	bl	0xa1c <.plt+0x14>       @ imm = #-0x2864  // CALL gensym
    327c: e1d42cf0     	ldrsh	r2, [r4, #192]
    3280: ee032a10     	vmov	s6, r2
    3284: eeb88ac3     	vcvt.f32.s32	s16, s6
    3288: e1a07000     	mov	r7, r0
    328c: e3a00008     	mov	r0, #8
    3290: ebfff5f0     	bl	0xa58 <.plt+0x50>       @ imm = #-0x2840  // CALL getbytes
    3294: e1a02005     	mov	r2, r5
    3298: e1a01007     	mov	r1, r7
    329c: ed808a01     	vstr	s16, [r0, #4]
    32a0: e1a03000     	mov	r3, r0
    32a4: e1a08000     	mov	r8, r0
    32a8: e5805000     	str	r5, [r0]
    32ac: e59400e8     	ldr	r0, [r4, #0xe8]
    32b0: ebfff639     	bl	0xb9c <.plt+0x194>      @ imm = #-0x271c  // CALL outlet_anything
    32b4: e1a00008     	mov	r0, r8
    32b8: e3a01008     	mov	r1, #8
    32bc: ebfff63c     	bl	0xbb4 <.plt+0x1ac>      @ imm = #-0x2710  // CALL freebytes
    32c0: e28dd00c     	add	sp, sp, #12
    32c4: ecbd8b02     	vpop	{d8}
    32c8: e8bd83f0     	pop	{r4, r5, r6, r7, r8, r9, pc}
    32cc: ed9f8a0c     	vldr	s16, [pc, #48]          @ 0x3304 <comport_info+0x43c>  // f32=0
    32d0: eaffff4b     	b	0x3004 <comport_info+0x13c> @ imm = #-0x2d4
    32d4: eddf8a0a     	vldr	s17, [pc, #40]          @ 0x3304 <comport_info+0x43c>  // f32=0
    32d8: eaffff65     	b	0x3074 <comport_info+0x1ac> @ imm = #-0x26c
    32dc: e59f1058     	ldr	r1, [pc, #0x58]         @ 0x333c <comport_info+0x474>  // u32=0x16fc; f32?=8.24524016e-42
    32e0: e3a08000     	mov	r8, #0
    32e4: e08f0001     	add	r0, pc, r1
    32e8: ebfff5cb     	bl	0xa1c <.plt+0x14>       @ imm = #-0x28d4  // CALL gensym
    32ec: e1a06000     	mov	r6, r0
    32f0: e3a00008     	mov	r0, #8
    32f4: ebfff5d7     	bl	0xa58 <.plt+0x50>       @ imm = #-0x28a4  // CALL getbytes
    32f8: e1a05000     	mov	r5, r0
    32fc: eaffff00     	b	0x2f04 <comport_info+0x3c> @ imm = #-0x400
    3300: 00 00 80 3f  	.word	0x3f800000
    3304: 00 00 00 00  	.word	0x00000000
    3308: f4 1a 00 00  	.word	0x00001af4
    330c: b0 1a 00 00  	.word	0x00001ab0
    3310: 68 1a 00 00  	.word	0x00001a68
    3314: 28 1a 00 00  	.word	0x00001a28
    3318: 15 54 00 00  	.word	0x00005415
    331c: c0 19 00 00  	.word	0x000019c0
    3320: 50 19 00 00  	.word	0x00001950
    3324: 10 19 00 00  	.word	0x00001910
    3328: c8 18 00 00  	.word	0x000018c8
    332c: 88 18 00 00  	.word	0x00001888
    3330: 44 18 00 00  	.word	0x00001844
    3334: 00 18 00 00  	.word	0x00001800
    3338: bc 17 00 00  	.word	0x000017bc
    333c: fc 16 00 00  	.word	0x000016fc

