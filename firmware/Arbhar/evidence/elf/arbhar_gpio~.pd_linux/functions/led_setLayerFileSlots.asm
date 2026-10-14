0000e2d8 <led_setLayerFileSlots>:
    e2d8: e92d4030     	push	{r4, r5, lr}
    e2dc: e1a04000     	mov	r4, r0
    e2e0: ed2d8b02     	vpush	{d8}
    e2e4: eeb08a40     	vmov.f32	s16, s0
    e2e8: e24dd024     	sub	sp, sp, #36
    e2ec: ed9f0a3a     	vldr	s0, [pc, #232]          @ 0xe3dc <led_setLayerFileSlots+0x104>
    e2f0: ebffd53c     	bl	0x37e8 <.plt+0xec>      @ imm = #-0xab10
    e2f4: e59f20e4     	ldr	r2, [pc, #0xe4]         @ 0xe3e0 <led_setLayerFileSlots+0x108>
    e2f8: eef77a00     	vmov.f32	s15, #1.000000e+00
    e2fc: e08f5002     	add	r5, pc, r2
    e300: e59f20dc     	ldr	r2, [pc, #0xdc]         @ 0xe3e4 <led_setLayerFileSlots+0x10c>
    e304: e5951074     	ldr	r1, [r5, #0x74]
    e308: eebd7ac8     	vcvt.s32.f32	s14, s16
    e30c: ee173a10     	vmov	r3, s14
    e310: ee300a67     	vsub.f32	s0, s0, s15
    e314: eefd0ac0     	vcvt.s32.f32	s1, s0
    e318: ee100a90     	vmov	r0, s1
    e31c: e040c003     	sub	r12, r0, r3
    e320: e1a00004     	mov	r0, r4
    e324: e151000c     	cmp	r1, r12
    e328: 1585c074     	strne	r12, [r5, #0x74]
    e32c: e08f5002     	add	r5, pc, r2
    e330: e085118c     	add	r1, r5, r12, lsl #3
    e334: e3a05000     	mov	r5, #0
    e338: e3445313     	movt	r5, #0x4313
    e33c: e51137b4     	ldr	r3, [r1, #-0x7b4]
    e340: e511c7b8     	ldr	r12, [r1, #-0x7b8]
    e344: ee013a10     	vmov	s2, r3
    e348: ee01ca90     	vmov	s3, r12
    e34c: eef80ac1     	vcvt.f32.s32	s1, s2
    e350: eeb80ae1     	vcvt.f32.s32	s0, s3
    e354: ebffd676     	bl	0x3d34 <.plt+0x638>     @ imm = #-0xa628
    e358: e2842a01     	add	r2, r4, #4096
    e35c: e59fc084     	ldr	r12, [pc, #0x84]        @ 0xe3e8 <led_setLayerFileSlots+0x110>
    e360: e3a01001     	mov	r1, #1
    e364: e5d20dd9     	ldrb	r0, [r2, #0xdd9]
    e368: e5d23dd8     	ldrb	r3, [r2, #0xdd8]
    e36c: e3a02000     	mov	r2, #0
    e370: e58d1000     	str	r1, [sp]
    e374: e3442312     	movt	r2, #0x4312
    e378: ee020a10     	vmov	s4, r0
    e37c: e08f000c     	add	r0, pc, r12
    e380: ee023a90     	vmov	s5, r3
    e384: e58d500c     	str	r5, [sp, #0xc]
    e388: eeb83a42     	vcvt.f32.u32	s6, s4
    e38c: e58d1008     	str	r1, [sp, #0x8]
    e390: e58d1010     	str	r1, [sp, #0x10]
    e394: e58d1018     	str	r1, [sp, #0x18]
    e398: e58d201c     	str	r2, [sp, #0x1c]
    e39c: e5945070     	ldr	r5, [r4, #0x70]
    e3a0: eef83a62     	vcvt.f32.u32	s7, s5
    e3a4: ed8d3a01     	vstr	s6, [sp, #4]
    e3a8: edcd3a05     	vstr	s7, [sp, #20]
    e3ac: ebffd4dd     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xac8c
    e3b0: e1a0300d     	mov	r3, sp
    e3b4: e3a02004     	mov	r2, #4
    e3b8: e1a01000     	mov	r1, r0
    e3bc: e1a00005     	mov	r0, r5
    e3c0: ebffd62b     	bl	0x3c74 <.plt+0x578>     @ imm = #-0xa754
    e3c4: e59400d8     	ldr	r0, [r4, #0xd8]
    e3c8: eeb30b09     	vmov.f64	d0, #2.500000e+01
    e3cc: ebffd556     	bl	0x392c <.plt+0x230>     @ imm = #-0xaaa8
    e3d0: e28dd024     	add	sp, sp, #36
    e3d4: ecbd8b02     	vpop	{d8}
    e3d8: e8bd8030     	pop	{r4, r5, pc}
    e3dc: 00 00 2c 42  	.word	0x422c0000
    e3e0: e8 8f 01 00  	.word	0x00018fe8
    e3e4: e0 6c 00 00  	.word	0x00006ce0
    e3e8: 38 67 00 00  	.word	0x00006738

