00002888 <_setQuantPitch>:
    2888: eefd7ac0     	vcvt.s32.f32	s15, s0
    288c: e2800a02     	add	r0, r0, #8192
    2890: e92d4030     	push	{r4, r5, lr}
    2894: e280ee6a     	add	lr, r0, #1696
    2898: e2804e63     	add	r4, r0, #1584
    289c: e5901894     	ldr	r1, [r0, #0x894]
    28a0: e1a0c00e     	mov	r12, lr
    28a4: e1a0000e     	mov	r0, lr
    28a8: ed840a01     	vstr	s0, [r4, #4]
    28ac: e3a055fe     	mov	r5, #1065353216
    28b0: e58e5004     	str	r5, [lr, #0x4]
    28b4: e2844004     	add	r4, r4, #4
    28b8: e59f207c     	ldr	r2, [pc, #0x7c]         @ 0x293c <_setQuantPitch+0xb4>  // u32=0x84e8; f32?=4.7677779e-41
    28bc: ee173a90     	vmov	r3, s15
    28c0: eddf6a1c     	vldr	s13, [pc, #112]         @ 0x2938 <_setQuantPitch+0xb0>  // f32=69
    28c4: e08f2002     	add	r2, pc, r2
    28c8: e283e050     	add	lr, r3, #80
    28cc: e1a0308e     	lsl	r3, lr, #1
    28d0: e283e001     	add	lr, r3, #1
    28d4: ee003a10     	vmov	s0, r3
    28d8: ee07ea10     	vmov	s14, lr
    28dc: eef80ac0     	vcvt.f32.s32	s1, s0
    28e0: eebd1ae0     	vcvt.s32.f32	s2, s1
    28e4: ee113a10     	vmov	r3, s2
    28e8: eef81ac7     	vcvt.f32.s32	s3, s14
    28ec: eefd3ae1     	vcvt.s32.f32	s7, s3
    28f0: e0813103     	add	r3, r1, r3, lsl #2
    28f4: ed932a00     	vldr	s4, [r3]
    28f8: ee722a26     	vadd.f32	s5, s4, s13
    28fc: eebd3ae2     	vcvt.s32.f32	s6, s5
    2900: ee133a10     	vmov	r3, s6
    2904: e0823103     	add	r3, r2, r3, lsl #2
    2908: e5933000     	ldr	r3, [r3]
    290c: e58c3008     	str	r3, [r12, #0x8]
    2910: ee13ca90     	vmov	r12, s7
    2914: e081110c     	add	r1, r1, r12, lsl #2
    2918: ed914a00     	vldr	s8, [r1]
    291c: ee744a26     	vadd.f32	s9, s8, s13
    2920: eebd5ae4     	vcvt.s32.f32	s10, s9
    2924: ee153a10     	vmov	r3, s10
    2928: e0822103     	add	r2, r2, r3, lsl #2
    292c: e592c000     	ldr	r12, [r2]
    2930: e580c00c     	str	r12, [r0, #0xc]
    2934: e8bd8030     	pop	{r4, r5, pc}
    2938: 00 00 8a 42  	.word	0x428a0000
    293c: e8 84 00 00  	.word	0x000084e8

