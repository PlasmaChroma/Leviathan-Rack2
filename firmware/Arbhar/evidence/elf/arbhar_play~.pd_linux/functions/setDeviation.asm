0000404c <setDeviation>:
    404c: eeb50ac0     	vcmpe.f32	s0, #0
    4050: e2800a02     	add	r0, r0, #8192
    4054: e92d4030     	push	{r4, r5, lr}
    4058: e3a0c000     	mov	r12, #0
    405c: e5901894     	ldr	r1, [r0, #0x894]
    4060: eef1fa10     	vmrs	APSR_nzcv, fpscr
    4064: aa00001c     	bge	0x40dc <setDeviation+0x90> @ imm = #0x70
    4068: e2803e6a     	add	r3, r0, #1696
    406c: e2802e63     	add	r2, r0, #1584
    4070: ed9f7a49     	vldr	s14, [pc, #292]         @ 0x419c <setDeviation+0x150>
    4074: e3a045fe     	mov	r4, #1065353216
    4078: e582c004     	str	r12, [r2, #0x4]
    407c: e1a0e003     	mov	lr, r3
    4080: e5834004     	str	r4, [r3, #0x4]
    4084: e2800e6b     	add	r0, r0, #1712
    4088: edd17aa0     	vldr	s15, [r1, #640]
    408c: e59fc10c     	ldr	r12, [pc, #0x10c]       @ 0x41a0 <setDeviation+0x154>
    4090: e08f200c     	add	r2, pc, r12
    4094: eeb10a40     	vneg.f32	s0, s0
    4098: e1a0c00e     	mov	r12, lr
    409c: e28ee008     	add	lr, lr, #8
    40a0: ee770a87     	vadd.f32	s1, s15, s14
    40a4: eebd1ae0     	vcvt.s32.f32	s2, s1
    40a8: ee113a10     	vmov	r3, s2
    40ac: e0823103     	add	r3, r2, r3, lsl #2
    40b0: e5933000     	ldr	r3, [r3]
    40b4: e58e3000     	str	r3, [lr]
    40b8: edd11aa1     	vldr	s3, [r1, #644]
    40bc: ee312a87     	vadd.f32	s4, s3, s14
    40c0: eefd2ac2     	vcvt.s32.f32	s5, s4
    40c4: ee121a90     	vmov	r1, s5
    40c8: e0822101     	add	r2, r2, r1, lsl #2
    40cc: e5923000     	ldr	r3, [r2]
    40d0: e58c300c     	str	r3, [r12, #0xc]
    40d4: ed800a00     	vstr	s0, [r0]
    40d8: e8bd8030     	pop	{r4, r5, pc}
    40dc: ed913a9f     	vldr	s6, [r1, #636]
    40e0: e2805e63     	add	r5, r0, #1584
    40e4: e2804e6a     	add	r4, r0, #1696
    40e8: e59fe0b4     	ldr	lr, [pc, #0xb4]         @ 0x41a4 <setDeviation+0x158>
    40ec: eef63a00     	vmov.f32	s7, #5.000000e-01
    40f0: e08fe00e     	add	lr, pc, lr
    40f4: eddf6a28     	vldr	s13, [pc, #160]         @ 0x419c <setDeviation+0x150>
    40f8: ee204a03     	vmul.f32	s8, s0, s6
    40fc: ee644a23     	vmul.f32	s9, s8, s7
    4100: eebd5ae4     	vcvt.s32.f32	s10, s9
    4104: edc54a01     	vstr	s9, [r5, #4]
    4108: e3a055fe     	mov	r5, #1065353216
    410c: e5845004     	str	r5, [r4, #0x4]
    4110: e2805e6a     	add	r5, r0, #1696
    4114: e2800e6b     	add	r0, r0, #1712
    4118: e1a04005     	mov	r4, r5
    411c: e2855008     	add	r5, r5, #8
    4120: e284400c     	add	r4, r4, #12
    4124: ee153a10     	vmov	r3, s10
    4128: e2832050     	add	r2, r3, #80
    412c: e1a03082     	lsl	r3, r2, #1
    4130: e2832001     	add	r2, r3, #1
    4134: ee053a90     	vmov	s11, r3
    4138: ee002a10     	vmov	s0, r2
    413c: eeb86ae5     	vcvt.f32.s32	s12, s11
    4140: eebd7ac6     	vcvt.s32.f32	s14, s12
    4144: ee173a10     	vmov	r3, s14
    4148: eef87ac0     	vcvt.f32.s32	s15, s0
    414c: eebd2ae7     	vcvt.s32.f32	s4, s15
    4150: e0813103     	add	r3, r1, r3, lsl #2
    4154: edd30a00     	vldr	s1, [r3]
    4158: ee301aa6     	vadd.f32	s2, s1, s13
    415c: eefd1ac1     	vcvt.s32.f32	s3, s2
    4160: ee113a90     	vmov	r3, s3
    4164: e08e3103     	add	r3, lr, r3, lsl #2
    4168: e5933000     	ldr	r3, [r3]
    416c: e5853000     	str	r3, [r5]
    4170: ee123a10     	vmov	r3, s4
    4174: e0811103     	add	r1, r1, r3, lsl #2
    4178: edd12a00     	vldr	s5, [r1]
    417c: ee323aa6     	vadd.f32	s6, s5, s13
    4180: eefd3ac3     	vcvt.s32.f32	s7, s6
    4184: ee133a90     	vmov	r3, s7
    4188: e08e2103     	add	r2, lr, r3, lsl #2
    418c: e5921000     	ldr	r1, [r2]
    4190: e5841000     	str	r1, [r4]
    4194: e580c000     	str	r12, [r0]
    4198: e8bd8030     	pop	{r4, r5, pc}
    419c: 00 00 8a 42  	.word	0x428a0000
    41a0: 1c 6d 00 00  	.word	0x00006d1c
    41a4: bc 6c 00 00  	.word	0x00006cbc

