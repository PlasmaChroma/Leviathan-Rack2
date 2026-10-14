00005e70 <_setStrikeLed>:
    5e70: e2802a01     	add	r2, r0, #4096
    5e74: e59f3098     	ldr	r3, [pc, #0x98]         @ 0x5f14 <_setStrikeLed+0xa4>
    5e78: e92d4010     	push	{r4, lr}
    5e7c: e08f0003     	add	r0, pc, r3
    5e80: e5924ddc     	ldr	r4, [r2, #0xddc]
    5e84: eeb17a00     	vmov.f32	s14, #4.000000e+00
    5e88: ed2d8b02     	vpush	{d8}
    5e8c: ee064a90     	vmov	s13, r4
    5e90: edd07a04     	vldr	s15, [r0, #16]
    5e94: e24dd018     	sub	sp, sp, #24
    5e98: e58d1004     	str	r1, [sp, #0x4]
    5e9c: eeb88a66     	vcvt.f32.u32	s16, s13
    5ea0: ee380a67     	vsub.f32	s0, s16, s15
    5ea4: eeb40ac7     	vcmpe.f32	s0, s14
    5ea8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    5eac: ca000005     	bgt	0x5ec8 <_setStrikeLed+0x58> @ imm = #0x14
    5eb0: e59f2060     	ldr	r2, [pc, #0x60]         @ 0x5f18 <_setStrikeLed+0xa8>
    5eb4: e08f3002     	add	r3, pc, r2
    5eb8: ed838a04     	vstr	s16, [r3, #16]
    5ebc: e28dd018     	add	sp, sp, #24
    5ec0: ecbd8b02     	vpop	{d8}
    5ec4: e8bd8010     	pop	{r4, pc}
    5ec8: ee001a90     	vmov	s1, r1
    5ecc: e59f1048     	ldr	r1, [pc, #0x48]         @ 0x5f1c <_setStrikeLed+0xac>
    5ed0: e3a0e001     	mov	lr, #1
    5ed4: e5924dac     	ldr	r4, [r2, #0xdac]
    5ed8: eeb81a60     	vcvt.f32.u32	s2, s1
    5edc: e08f0001     	add	r0, pc, r1
    5ee0: e58de008     	str	lr, [sp, #0x8]
    5ee4: e3a0c000     	mov	r12, #0
    5ee8: e58de010     	str	lr, [sp, #0x10]
    5eec: e344c311     	movt	r12, #0x4311
    5ef0: e58dc014     	str	r12, [sp, #0x14]
    5ef4: ed8d1a03     	vstr	s2, [sp, #12]
    5ef8: ebfff60a     	bl	0x3728 <.plt+0x2c>      @ imm = #-0x27d8
    5efc: e28d3008     	add	r3, sp, #8
    5f00: e3a02002     	mov	r2, #2
    5f04: e1a01000     	mov	r1, r0
    5f08: e1a00004     	mov	r0, r4
    5f0c: ebfff758     	bl	0x3c74 <.plt+0x578>     @ imm = #-0x22a0
    5f10: eaffffe6     	b	0x5eb0 <_setStrikeLed+0x40> @ imm = #-0x68
    5f14: 34 15 02 00  	.word	0x00021534
    5f18: fc 14 02 00  	.word	0x000214fc
    5f1c: d8 eb 00 00  	.word	0x0000ebd8

