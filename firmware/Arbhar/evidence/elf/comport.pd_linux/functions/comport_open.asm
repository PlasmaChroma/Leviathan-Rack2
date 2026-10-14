000028b0 <comport_open>:
    28b0: e5903020     	ldr	r3, [r0, #0x20]
    28b4: e92d4070     	push	{r4, r5, r6, lr}
    28b8: e3730001     	cmn	r3, #1
    28bc: ed2d8b02     	vpush	{d8}
    28c0: e1a04000     	mov	r4, r0
    28c4: eeb08a40     	vmov.f32	s16, s0
    28c8: e2805a01     	add	r5, r0, #4096
    28cc: 0a00001a     	beq	0x293c <comport_open+0x8c> @ imm = #0x68
    28d0: e59500c4     	ldr	r0, [r5, #0xc4]
    28d4: ebfff886     	bl	0xaf4 <.plt+0xec>       @ imm = #-0x1de8
    28d8: e5946020     	ldr	r6, [r4, #0x20]
    28dc: e3a00001     	mov	r0, #1
    28e0: e3760001     	cmn	r6, #1
    28e4: e58500c8     	str	r0, [r5, #0xc8]
    28e8: 0a00000b     	beq	0x291c <comport_open+0x6c> @ imm = #0x2c
    28ec: e2842060     	add	r2, r4, #96
    28f0: e3a01000     	mov	r1, #0
    28f4: e1a00006     	mov	r0, r6
    28f8: ebfff86b     	bl	0xaac <.plt+0xa4>       @ imm = #-0x1e54
    28fc: e1a00006     	mov	r0, r6
    2900: ebfff8b4     	bl	0xbd8 <.plt+0x1d0>      @ imm = #-0x1d30
    2904: e594209c     	ldr	r2, [r4, #0x9c]
    2908: e59fc05c     	ldr	r12, [pc, #0x5c]        @ 0x296c <comport_open+0xbc>
    290c: e1d51af0     	ldrsh	r1, [r5, #160]
    2910: e08f000c     	add	r0, pc, r12
    2914: e5922000     	ldr	r2, [r2]
    2918: ebfff887     	bl	0xb3c <.plt+0x134>      @ imm = #-0x1de4
    291c: e59500e8     	ldr	r0, [r5, #0xe8]
    2920: e3e01000     	mvn	r1, #0
    2924: e3500000     	cmp	r0, #0
    2928: e5841020     	str	r1, [r4, #0x20]
    292c: e1c51ab0     	strh	r1, [r5, #160]
    2930: 0a000001     	beq	0x293c <comport_open+0x8c> @ imm = #0x4
    2934: ed9f0a0b     	vldr	s0, [pc, #44]           @ 0x2968 <comport_open+0xb8>
    2938: ebfff88e     	bl	0xb78 <.plt+0x170>      @ imm = #-0x1dc8
    293c: eefc7ac8     	vcvt.u32.f32	s15, s16
    2940: e1a01004     	mov	r1, r4
    2944: ee170a90     	vmov	r0, s15
    2948: ebfffb06     	bl	0x1568 <open_serial>    @ imm = #-0x13e8
    294c: ecbd8b02     	vpop	{d8}
    2950: e2843d43     	add	r3, r4, #4288
    2954: ed930b06     	vldr	d0, [r3, #24]
    2958: e5840020     	str	r0, [r4, #0x20]
    295c: e59500c4     	ldr	r0, [r5, #0xc4]
    2960: e8bd4070     	pop	{r4, r5, r6, lr}
    2964: eafff847     	b	0xa88 <.plt+0x80>       @ imm = #-0x1ee4
    2968: 00 00 80 bf  	.word	0xbf800000
    296c: 44 1d 00 00  	.word	0x00001d44

