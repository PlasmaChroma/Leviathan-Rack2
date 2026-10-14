00007c50 <_setPlayParameters>:
    7c50: e92d4030     	push	{r4, r5, lr}
    7c54: e1a04000     	mov	r4, r0
    7c58: ed2d8b02     	vpush	{d8}
    7c5c: e5d0303c     	ldrb	r3, [r0, #0x3c]
    7c60: e24dd00c     	sub	sp, sp, #12
    7c64: e3530001     	cmp	r3, #1
    7c68: 9a0000b0     	bls	0x7f30 <_setPlayParameters+0x2e0> @ imm = #0x2c0
    7c6c: e59f0374     	ldr	r0, [pc, #0x374]        @ 0x7fe8 <_setPlayParameters+0x398>  // u32=0x1f740; f32?=1.80532084e-40
    7c70: e08f1000     	add	r1, pc, r0
    7c74: ed910a10     	vldr	s0, [r1, #64]
    7c78: e30014ae     	movw	r1, #0x4ae
    7c7c: e5d42054     	ldrb	r2, [r4, #0x54]
    7c80: e19450b1     	ldrh	r5, [r4, r1]
    7c84: ed9f1ac7     	vldr	s2, [pc, #796]          @ 0x7fa8 <_setPlayParameters+0x358>  // f32=0
    7c88: e265ce7f     	rsb	r12, r5, #2032
    7c8c: e5d40039     	ldrb	r0, [r4, #0x39]
    7c90: e28ce00f     	add	lr, r12, #15
    7c94: eddf1ac4     	vldr	s3, [pc, #784]          @ 0x7fac <_setPlayParameters+0x35c>  // f32=4095
    7c98: ee02ea10     	vmov	s4, lr
    7c9c: eef85ac2     	vcvt.f32.s32	s11, s4
    7ca0: ee752a80     	vadd.f32	s5, s11, s0
    7ca4: eef42ac1     	vcmpe.f32	s5, s2
    7ca8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    7cac: bef02a41     	vmovlt.f32	s5, s2
    7cb0: eef42ae1     	vcmpe.f32	s5, s3
    7cb4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    7cb8: 8ef02a61     	vmovhi.f32	s5, s3
    7cbc: e3520000     	cmp	r2, #0
    7cc0: 0a000017     	beq	0x7d24 <_setPlayParameters+0xd4> @ imm = #0x5c
    7cc4: e3500000     	cmp	r0, #0
    7cc8: 1a00007c     	bne	0x7ec0 <_setPlayParameters+0x270> @ imm = #0x1f0
    7ccc: e5943064     	ldr	r3, [r4, #0x64]
    7cd0: e5d45065     	ldrb	r5, [r4, #0x65]
    7cd4: e3c314ff     	bic	r1, r3, #-16777216
    7cd8: edd48a16     	vldr	s17, [r4, #88]
    7cdc: e3c12cff     	bic	r2, r1, #65280
    7ce0: e3520001     	cmp	r2, #1
    7ce4: 05c40064     	strbeq	r0, [r4, #0x64]
    7ce8: 15d40064     	ldrbne	r0, [r4, #0x64]
    7cec: e3550000     	cmp	r5, #0
    7cf0: 0a000082     	beq	0x7f00 <_setPlayParameters+0x2b0> @ imm = #0x208
    7cf4: ebfff085     	bl	0x3f10 <calculateFollowSpeed> @ imm = #-0x3dec
    7cf8: eddf6aac     	vldr	s13, [pc, #688]         @ 0x7fb0 <_setPlayParameters+0x360>  // f32=234.375
    7cfc: ed9f7aac     	vldr	s14, [pc, #688]         @ 0x7fb4 <_setPlayParameters+0x364>  // f32=480000
    7d00: ee650aa6     	vmul.f32	s1, s11, s13
    7d04: ed840a17     	vstr	s0, [r4, #92]
    7d08: ee788aa0     	vadd.f32	s17, s17, s1
    7d0c: edc40a18     	vstr	s1, [r4, #96]
    7d10: eef48ac7     	vcmpe.f32	s17, s14
    7d14: eef1fa10     	vmrs	APSR_nzcv, fpscr
    7d18: da00009b     	ble	0x7f8c <_setPlayParameters+0x33c> @ imm = #0x26c
    7d1c: ee788ac7     	vsub.f32	s17, s17, s14
    7d20: ea000079     	b	0x7f0c <_setPlayParameters+0x2bc> @ imm = #0x1e4
    7d24: ed9f0aa3     	vldr	s0, [pc, #652]          @ 0x7fb8 <_setPlayParameters+0x368>  // f32=117.216125
    7d28: e3500000     	cmp	r0, #0
    7d2c: ee628a80     	vmul.f32	s17, s5, s0
    7d30: edc48a14     	vstr	s17, [r4, #80]
    7d34: 0a000075     	beq	0x7f10 <_setPlayParameters+0x2c0> @ imm = #0x1d4
    7d38: ed9f4a9f     	vldr	s8, [pc, #636]          @ 0x7fbc <_setPlayParameters+0x36c>  // f32=146.520157
    7d3c: eddf4a9f     	vldr	s9, [pc, #636]          @ 0x7fc0 <_setPlayParameters+0x370>  // f32=9.99999975e-06
    7d40: ee628a84     	vmul.f32	s17, s5, s8
    7d44: ee285aa4     	vmul.f32	s10, s17, s9
    7d48: eebc6ac5     	vcvt.u32.f32	s12, s10
    7d4c: ed8d6a01     	vstr	s12, [sp, #4]
    7d50: e5dd1004     	ldrb	r1, [sp, #0x4]
    7d54: e3510005     	cmp	r1, #5
    7d58: 9a00005c     	bls	0x7ed0 <_setPlayParameters+0x280> @ imm = #0x170
    7d5c: e30009fa     	movw	r0, #0x9fa
    7d60: ed9f0a97     	vldr	s0, [pc, #604]          @ 0x7fc4 <_setPlayParameters+0x374>  // f32=209
    7d64: e194c0b0     	ldrh	r12, [r4, r0]
    7d68: e1a00004     	mov	r0, r4
    7d6c: e2845a01     	add	r5, r4, #4096
    7d70: e26ceeff     	rsb	lr, r12, #4080
    7d74: e28e300f     	add	r3, lr, #15
    7d78: ee053a90     	vmov	s11, r3
    7d7c: eeb88ae5     	vcvt.f32.s32	s16, s11
    7d80: ebffee98     	bl	0x37e8 <.plt+0xec>      @ imm = #-0x45a0  // CALL readFromSharedMem
    7d84: eddf2a8f     	vldr	s5, [pc, #572]          @ 0x7fc8 <_setPlayParameters+0x378>  // f32=32
    7d88: e5d51df5     	ldrb	r1, [r5, #0xdf5]
    7d8c: ed9f3a85     	vldr	s6, [pc, #532]          @ 0x7fa8 <_setPlayParameters+0x358>  // f32=0
    7d90: eddf3a85     	vldr	s7, [pc, #532]          @ 0x7fac <_setPlayParameters+0x35c>  // f32=4095
    7d94: ed9f4a8c     	vldr	s8, [pc, #560]          @ 0x7fcc <_setPlayParameters+0x37c>  // f32=0.000244200259
    7d98: ee008a22     	vmla.f32	s16, s0, s5
    7d9c: eeb48ac3     	vcmpe.f32	s16, s6
    7da0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    7da4: beb08a43     	vmovlt.f32	s16, s6
    7da8: eeb48ae3     	vcmpe.f32	s16, s7
    7dac: eef1fa10     	vmrs	APSR_nzcv, fpscr
    7db0: 8eb08a63     	vmovhi.f32	s16, s7
    7db4: e3510000     	cmp	r1, #0
    7db8: ee280a04     	vmul.f32	s0, s16, s8
    7dbc: 0a000011     	beq	0x7e08 <_setPlayParameters+0x1b8> @ imm = #0x44
    7dc0: e5952f9c     	ldr	r2, [r5, #0xf9c]
    7dc4: e3520000     	cmp	r2, #0
    7dc8: 0a00000e     	beq	0x7e08 <_setPlayParameters+0x1b8> @ imm = #0x38
    7dcc: e2850c0e     	add	r0, r5, #3584
    7dd0: e285cedf     	add	r12, r5, #3568
    7dd4: edd08a01     	vldr	s17, [r0, #4]
    7dd8: ed9c7a02     	vldr	s14, [r12, #8]
    7ddc: eef58a40     	vcmp.f32	s17, #0
    7de0: eef66a00     	vmov.f32	s13, #5.000000e-01
    7de4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    7de8: 0ddf8a78     	vldreq	s17, [pc, #480]         @ 0x7fd0 <_setPlayParameters+0x380>
    7dec: 1e277a28     	vmulne.f32	s14, s14, s17
    7df0: ee707a26     	vadd.f32	s15, s0, s13
    7df4: 1ddf6a76     	vldrne	s13, [pc, #472]         @ 0x7fd4 <_setPlayParameters+0x384>
    7df8: 1ec68a87     	vdivne.f32	s17, s13, s14
    7dfc: ee670aa7     	vmul.f32	s1, s15, s15
    7e00: ee206aa8     	vmul.f32	s12, s1, s17
    7e04: ea000002     	b	0x7e14 <_setPlayParameters+0x1c4> @ imm = #0x8
    7e08: ee604a00     	vmul.f32	s9, s0, s0
    7e0c: ed9f5a71     	vldr	s10, [pc, #452]         @ 0x7fd8 <_setPlayParameters+0x388>  // f32=100000
    7e10: ee246a85     	vmul.f32	s12, s9, s10
    7e14: eddf8a6f     	vldr	s17, [pc, #444]         @ 0x7fd8 <_setPlayParameters+0x388>  // f32=100000
    7e18: e3a01065     	mov	r1, #101
    7e1c: e1a00004     	mov	r0, r4
    7e20: eeb46ae8     	vcmpe.f32	s12, s17
    7e24: eef1fa10     	vmrs	APSR_nzcv, fpscr
    7e28: 8eb06a68     	vmovhi.f32	s12, s17
    7e2c: eebc1ac6     	vcvt.u32.f32	s2, s12
    7e30: ee112a10     	vmov	r2, s2
    7e34: ebffef2b     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x4354  // CALL writeToSharedMem
    7e38: e3003656     	movw	r3, #0x656
    7e3c: e19550b3     	ldrh	r5, [r5, r3]
    7e40: e1a00004     	mov	r0, r4
    7e44: ed9f0a64     	vldr	s0, [pc, #400]          @ 0x7fdc <_setPlayParameters+0x38c>  // f32=212
    7e48: e2651eff     	rsb	r1, r5, #4080
    7e4c: e281200f     	add	r2, r1, #15
    7e50: ee012a90     	vmov	s3, r2
    7e54: eeb88ae1     	vcvt.f32.s32	s16, s3
    7e58: ebffee62     	bl	0x37e8 <.plt+0xec>      @ imm = #-0x4678  // CALL readFromSharedMem
    7e5c: ed9f2a59     	vldr	s4, [pc, #356]          @ 0x7fc8 <_setPlayParameters+0x378>  // f32=32
    7e60: e1a00004     	mov	r0, r4
    7e64: e3a01067     	mov	r1, #103
    7e68: eddf5a4e     	vldr	s11, [pc, #312]         @ 0x7fa8 <_setPlayParameters+0x358>  // f32=0
    7e6c: eddf2a4e     	vldr	s5, [pc, #312]          @ 0x7fac <_setPlayParameters+0x35c>  // f32=4095
    7e70: ed9f3a55     	vldr	s6, [pc, #340]          @ 0x7fcc <_setPlayParameters+0x37c>  // f32=0.000244200259
    7e74: ee008a02     	vmla.f32	s16, s0, s4
    7e78: eeb48ae5     	vcmpe.f32	s16, s11
    7e7c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    7e80: beb08a65     	vmovlt.f32	s16, s11
    7e84: eeb48ae2     	vcmpe.f32	s16, s5
    7e88: eef1fa10     	vmrs	APSR_nzcv, fpscr
    7e8c: 8eb08a62     	vmovhi.f32	s16, s5
    7e90: ee683a03     	vmul.f32	s7, s16, s6
    7e94: ee234aa3     	vmul.f32	s8, s7, s7
    7e98: ee240a28     	vmul.f32	s0, s8, s17
    7e9c: eeb40ae8     	vcmpe.f32	s0, s17
    7ea0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    7ea4: 8eb00a68     	vmovhi.f32	s0, s17
    7ea8: eefc4ac0     	vcvt.u32.f32	s9, s0
    7eac: ee142a90     	vmov	r2, s9
    7eb0: e28dd00c     	add	sp, sp, #12
    7eb4: ecbd8b02     	vpop	{d8}
    7eb8: e8bd4030     	pop	{r4, r5, lr}
    7ebc: eaffef09     	b	0x3ae8 <.plt+0x3ec>     @ imm = #-0x43dc  // CALL writeToSharedMem
    7ec0: ed9f3a3c     	vldr	s6, [pc, #240]          @ 0x7fb8 <_setPlayParameters+0x368>  // f32=117.216125
    7ec4: ee623a83     	vmul.f32	s7, s5, s6
    7ec8: edc43a14     	vstr	s7, [r4, #80]
    7ecc: eaffff99     	b	0x7d38 <_setPlayParameters+0xe8> @ imm = #-0x19c
    7ed0: e1a00004     	mov	r0, r4
    7ed4: ebffef6c     	bl	0x3c8c <.plt+0x590>     @ imm = #-0x4250  // CALL _setPlayAndRecLayerInOmega
    7ed8: ed9f1b30     	vldr	d1, [pc, #192]          @ 0x7fa0 <_setPlayParameters+0x350>  // f64=99999
    7edc: eeb70ae8     	vcvt.f64.f32	d0, s17
    7ee0: ebffeedc     	bl	0x3a58 <.plt+0x35c>     @ imm = #-0x4490  // CALL __fmod_finite
    7ee4: e3a01064     	mov	r1, #100
    7ee8: e1a00004     	mov	r0, r4
    7eec: eef77bc0     	vcvt.f32.f64	s15, d0
    7ef0: eebc8ae7     	vcvt.u32.f32	s16, s15
    7ef4: ee182a10     	vmov	r2, s16
    7ef8: ebffeefa     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x4418  // CALL writeToSharedMem
    7efc: eaffff96     	b	0x7d5c <_setPlayParameters+0x10c> @ imm = #-0x1a8
    7f00: eeb00a62     	vmov.f32	s0, s5
    7f04: ebfff001     	bl	0x3f10 <calculateFollowSpeed> @ imm = #-0x3ffc
    7f08: ed840a17     	vstr	s0, [r4, #92]
    7f0c: edc48a14     	vstr	s17, [r4, #80]
    7f10: e3a01064     	mov	r1, #100
    7f14: e1a00004     	mov	r0, r4
    7f18: ed9f1a30     	vldr	s2, [pc, #192]          @ 0x7fe0 <_setPlayParameters+0x390>  // f32=0.208333328
    7f1c: ee681a81     	vmul.f32	s3, s17, s2
    7f20: eebc2ae1     	vcvt.u32.f32	s4, s3
    7f24: ee122a10     	vmov	r2, s4
    7f28: ebffeeee     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x4448  // CALL writeToSharedMem
    7f2c: eaffff8a     	b	0x7d5c <_setPlayParameters+0x10c> @ imm = #-0x1d8
    7f30: e3002d82     	movw	r2, #0xd82
    7f34: ed9f0a2a     	vldr	s0, [pc, #168]          @ 0x7fe4 <_setPlayParameters+0x394>  // f32=208
    7f38: e19050b2     	ldrh	r5, [r0, r2]
    7f3c: e265ceff     	rsb	r12, r5, #4080
    7f40: e28ce00f     	add	lr, r12, #15
    7f44: ee07ea90     	vmov	s15, lr
    7f48: eeb88ae7     	vcvt.f32.s32	s16, s15
    7f4c: ebffee25     	bl	0x37e8 <.plt+0xec>      @ imm = #-0x476c  // CALL readFromSharedMem
    7f50: eddf6a1c     	vldr	s13, [pc, #112]         @ 0x7fc8 <_setPlayParameters+0x378>  // f32=32
    7f54: e59f3090     	ldr	r3, [pc, #0x90]         @ 0x7fec <_setPlayParameters+0x39c>  // u32=0x1f454; f32?=1.79483913e-40
    7f58: ed9f7a12     	vldr	s14, [pc, #72]          @ 0x7fa8 <_setPlayParameters+0x358>  // f32=0
    7f5c: e08f0003     	add	r0, pc, r3
    7f60: eddf0a11     	vldr	s1, [pc, #68]           @ 0x7fac <_setPlayParameters+0x35c>  // f32=4095
    7f64: ee008a26     	vmla.f32	s16, s0, s13
    7f68: eeb48ac7     	vcmpe.f32	s16, s14
    7f6c: eeb00a48     	vmov.f32	s0, s16
    7f70: eef1fa10     	vmrs	APSR_nzcv, fpscr
    7f74: beb00a47     	vmovlt.f32	s0, s14
    7f78: eeb40ae0     	vcmpe.f32	s0, s1
    7f7c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    7f80: 8eb00a60     	vmovhi.f32	s0, s1
    7f84: ed800a10     	vstr	s0, [r0, #64]
    7f88: eaffff3a     	b	0x7c78 <_setPlayParameters+0x28> @ imm = #-0x318
    7f8c: eef58ac0     	vcmpe.f32	s17, #0
    7f90: eef1fa10     	vmrs	APSR_nzcv, fpscr
    7f94: 4e788a87     	vaddmi.f32	s17, s17, s14
    7f98: eaffffdb     	b	0x7f0c <_setPlayParameters+0x2bc> @ imm = #-0x94
    7f9c: e320f000     	nop
    7fa0: 00 00 00 00  	.word	0x00000000
    7fa4: f0 69 f8 40  	.word	0x40f869f0
    7fa8: 00 00 00 00  	.word	0x00000000
    7fac: 00 f0 7f 45  	.word	0x457ff000
    7fb0: 00 60 6a 43  	.word	0x436a6000
    7fb4: 00 60 ea 48  	.word	0x48ea6000
    7fb8: a8 6e ea 42  	.word	0x42ea6ea8
    7fbc: 29 85 12 43  	.word	0x43128529
    7fc0: ac c5 27 37  	.word	0x3727c5ac
    7fc4: 00 00 51 43  	.word	0x43510000
    7fc8: 00 00 00 42  	.word	0x42000000
    7fcc: 01 08 80 39  	.word	0x39800801
    7fd0: 00 00 7a 44  	.word	0x447a0000
    7fd4: 00 24 f4 49  	.word	0x49f42400
    7fd8: 00 50 c3 47  	.word	0x47c35000
    7fdc: 00 00 54 43  	.word	0x43540000
    7fe0: 55 55 55 3e  	.word	0x3e555555
    7fe4: 00 00 50 43  	.word	0x43500000
    7fe8: 40 f7 01 00  	.word	0x0001f740
    7fec: 54 f4 01 00  	.word	0x0001f454

