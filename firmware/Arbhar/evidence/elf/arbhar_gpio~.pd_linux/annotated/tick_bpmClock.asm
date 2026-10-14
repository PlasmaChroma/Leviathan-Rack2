0000aa40 <tick_bpmClock>:
    aa40: e92d4070     	push	{r4, r5, r6, lr}
    aa44: e2804a01     	add	r4, r0, #4096
    aa48: ed2d8b02     	vpush	{d8}
    aa4c: e1a05000     	mov	r5, r0
    aa50: e5943ddc     	ldr	r3, [r4, #0xddc]
    aa54: e5843fac     	str	r3, [r4, #0xfac]
    aa58: ebffe3b6     	bl	0x3938 <.plt+0x23c>     @ imm = #-0x7128  // CALL _adaptInternalBpm
    aa5c: e59f22d8     	ldr	r2, [pc, #0x2d8]        @ 0xad3c <tick_bpmClock+0x2fc>  // u32=0x1c884; f32?=1.63766949e-40
    aa60: e08f1002     	add	r1, pc, r2
    aa64: edd17a05     	vldr	s15, [r1, #20]
    aa68: e1a06000     	mov	r6, r0
    aa6c: e2840edf     	add	r0, r4, #3568
    aa70: edd06a02     	vldr	s13, [r0, #8]
    aa74: eef46a67     	vcmp.f32	s13, s15
    aa78: eef1fa10     	vmrs	APSR_nzcv, fpscr
    aa7c: 1a000004     	bne	0xaa94 <tick_bpmClock+0x54> @ imm = #0x10
    aa80: e59fc2b8     	ldr	r12, [pc, #0x2b8]       @ 0xad40 <tick_bpmClock+0x300>  // u32=0x1c92c; f32?=1.64002367e-40
    aa84: e08fe00c     	add	lr, pc, r12
    aa88: e59e307c     	ldr	r3, [lr, #0x7c]
    aa8c: e1530006     	cmp	r3, r6
    aa90: 0a000007     	beq	0xaab4 <tick_bpmClock+0x74> @ imm = #0x1c
    aa94: eef70ae6     	vcvt.f64.f32	d16, s13
    aa98: e59f02a4     	ldr	r0, [pc, #0x2a4]        @ 0xad44 <tick_bpmClock+0x304>  // u32=0xaae4; f32?=6.13040052e-41
    aa9c: e1a01006     	mov	r1, r6
    aaa0: e08f0000     	add	r0, pc, r0
    aaa4: ec532b30     	vmov	r2, r3, d16
    aaa8: ebffe432     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x6f38  // CALL post
    aaac: e2842edf     	add	r2, r4, #3568
    aab0: edd26a02     	vldr	s13, [r2, #8]
    aab4: e5941fac     	ldr	r1, [r4, #0xfac]
    aab8: e594cfa8     	ldr	r12, [r4, #0xfa8]
    aabc: eddf5a9b     	vldr	s11, [pc, #620]         @ 0xad30 <tick_bpmClock+0x2f0>  // f32=60000
    aac0: e041e00c     	sub	lr, r1, r12
    aac4: e59f327c     	ldr	r3, [pc, #0x27c]        @ 0xad48 <tick_bpmClock+0x308>  // u32=0x1c8e4; f32?=1.63901474e-40
    aac8: ee00ea10     	vmov	s0, lr
    aacc: e08f0003     	add	r0, pc, r3
    aad0: ee856aa6     	vdiv.f32	s12, s11, s13
    aad4: ed9f7a96     	vldr	s14, [pc, #600]         @ 0xad34 <tick_bpmClock+0x2f4>  // f32=1.33333337
    aad8: e580607c     	str	r6, [r0, #0x7c]
    aadc: ed9f1b8d     	vldr	d1, [pc, #564]          @ 0xad18 <tick_bpmClock+0x2d8>  // f64=1.1000000000000001
    aae0: eef80ac0     	vcvt.f32.s32	s1, s0
    aae4: ee202a87     	vmul.f32	s4, s1, s14
    aae8: eef72ac2     	vcvt.f64.f32	d18, s4
    aaec: eef71ac6     	vcvt.f64.f32	d17, s12
    aaf0: ee213b81     	vmul.f64	d3, d17, d1
    aaf4: eef42bc3     	vcmpe.f64	d18, d3
    aaf8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    aafc: 8a000010     	bhi	0xab44 <tick_bpmClock+0x104> @ imm = #0x40
    ab00: eeb64b00     	vmov.f64	d4, #5.000000e-01
    ab04: eec21a06     	vdiv.f32	s3, s4, s12
    ab08: ee215b84     	vmul.f64	d5, d17, d4
    ab0c: eeb78ae6     	vcvt.f64.f32	d8, s13
    ab10: eef42bc5     	vcmpe.f64	d18, d5
    ab14: eef1fa10     	vmrs	APSR_nzcv, fpscr
    ab18: 8eb72a00     	vmovhi.f32	s4, #1.000000e+00
    ab1c: 9ddf2b7f     	vldrls	d18, [pc, #508]         @ 0xad20 <tick_bpmClock+0x2e0>
    ab20: 92840edf     	addls	r0, r4, #3568
    ab24: 82840edf     	addhi	r0, r4, #3568
    ab28: 8ddf2b7c     	vldrhi	d18, [pc, #496]         @ 0xad20 <tick_bpmClock+0x2e0>
    ab2c: 8e711ac2     	vsubhi.f32	s3, s3, s4
    ab30: eef73ae1     	vcvt.f64.f32	d19, s3
    ab34: ee634ba2     	vmul.f64	d20, d19, d18
    ab38: ee048b88     	vmla.f64	d8, d20, d8
    ab3c: eef76bc8     	vcvt.f32.f64	s13, d8
    ab40: edc06a02     	vstr	s13, [r0, #8]
    ab44: e30062ce     	movw	r6, #0x2ce
    ab48: e59f21fc     	ldr	r2, [pc, #0x1fc]        @ 0xad4c <tick_bpmClock+0x30c>  // u32=0x1c794; f32?=1.63430637e-40
    ab4c: e194c0b6     	ldrh	r12, [r4, r6]
    ab50: e08f1002     	add	r1, pc, r2
    ab54: e35c0efa     	cmp	r12, #4000
    ab58: edc16a05     	vstr	s13, [r1, #20]
    ab5c: 8a000053     	bhi	0xacb0 <tick_bpmClock+0x270> @ imm = #0x14c
    ab60: e26ccb02     	rsb	r12, r12, #2048
    ab64: e284ec0e     	add	lr, r4, #3584
    ab68: eef73a00     	vmov.f32	s7, #1.000000e+00
    ab6c: e28e0004     	add	r0, lr, #4
    ab70: ee02ca90     	vmov	s5, r12
    ab74: eefa2aea     	vcvt.f32.s32	s5, s5, #11
    ab78: edc03a00     	vstr	s7, [r0]
    ab7c: eef52ac0     	vcmpe.f32	s5, #0
    ab80: eef1fa10     	vmrs	APSR_nzcv, fpscr
    ab84: ba000034     	blt	0xac5c <tick_bpmClock+0x21c> @ imm = #0xd0
    ab88: e5942ed0     	ldr	r2, [r4, #0xed0]
    ab8c: eef68b00     	vmov.f64	d24, #5.000000e-01
    ab90: ee002a90     	vmov	s1, r2
    ab94: eeb87ae0     	vcvt.f32.s32	s14, s1
    ab98: ee271a22     	vmul.f32	s2, s14, s5
    ab9c: eef79ac1     	vcvt.f64.f32	d25, s2
    aba0: ee79aba8     	vadd.f64	d26, d25, d24
    aba4: eebd6bea     	vcvt.s32.f64	s12, d26
    aba8: ee161a10     	vmov	r1, s12
    abac: e281cd1e     	add	r12, r1, #1920
    abb0: e28ce002     	add	lr, r12, #2
    abb4: e085610e     	add	r6, r5, lr, lsl #2
    abb8: ed968a00     	vldr	s16, [r6]
    abbc: ed808a00     	vstr	s16, [r0]
    abc0: e59f0188     	ldr	r0, [pc, #0x188]        @ 0xad50 <tick_bpmClock+0x310>  // u32=0x1c7e8; f32?=1.63548346e-40
    abc4: eeb58ac0     	vcmpe.f32	s16, #0
    abc8: e08f2000     	add	r2, pc, r0
    abcc: e5923080     	ldr	r3, [r2, #0x80]
    abd0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    abd4: e2831001     	add	r1, r3, #1
    abd8: e5821080     	str	r1, [r2, #0x80]
    abdc: da00004a     	ble	0xad0c <tick_bpmClock+0x2cc> @ imm = #0x128
    abe0: e59fc16c     	ldr	r12, [pc, #0x16c]       @ 0xad54 <tick_bpmClock+0x314>  // u32=0x1c7cc; f32?=1.6350911e-40
    abe4: e08fe00c     	add	lr, pc, r12
    abe8: ed9e2a21     	vldr	s4, [lr, #132]
    abec: eeb42a48     	vcmp.f32	s4, s16
    abf0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    abf4: 0a000008     	beq	0xac1c <tick_bpmClock+0x1dc> @ imm = #0x20
    abf8: eeb73a00     	vmov.f32	s6, #1.000000e+00
    abfc: e59500fc     	ldr	r0, [r5, #0xfc]
    ac00: eeb48ac3     	vcmpe.f32	s16, s6
    ac04: eef1fa10     	vmrs	APSR_nzcv, fpscr
    ac08: ca000035     	bgt	0xace4 <tick_bpmClock+0x2a4> @ imm = #0xd4
    ac0c: e2846edf     	add	r6, r4, #3568
    ac10: ed9f0b44     	vldr	d0, [pc, #272]          @ 0xad28 <tick_bpmClock+0x2e8>  // f64=0
    ac14: ebffe344     	bl	0x392c <.plt+0x230>     @ imm = #-0x72f0  // CALL clock_delay
    ac18: edd66a02     	vldr	s13, [r6, #8]
    ac1c: e59f0134     	ldr	r0, [pc, #0x134]        @ 0xad58 <tick_bpmClock+0x318>  // u32=0x1c78c; f32?=1.63419427e-40
    ac20: e5d44df5     	ldrb	r4, [r4, #0xdf5]
    ac24: e08f2000     	add	r2, pc, r0
    ac28: e5950100     	ldr	r0, [r5, #0x100]
    ac2c: e3540000     	cmp	r4, #0
    ac30: ed828a21     	vstr	s16, [r2, #132]
    ac34: 0a000027     	beq	0xacd8 <tick_bpmClock+0x298> @ imm = #0x9c
    ac38: ed9f4a3c     	vldr	s8, [pc, #240]          @ 0xad30 <tick_bpmClock+0x2f0>  // f32=60000
    ac3c: ee845a26     	vdiv.f32	s10, s8, s13
    ac40: eeb70ac5     	vcvt.f64.f32	d0, s10
    ac44: ebffe338     	bl	0x392c <.plt+0x230>     @ imm = #-0x7320  // CALL clock_delay
    ac48: ecbd8b02     	vpop	{d8}
    ac4c: e59500e0     	ldr	r0, [r5, #0xe0]
    ac50: ed9f0b34     	vldr	d0, [pc, #208]          @ 0xad28 <tick_bpmClock+0x2e8>  // f64=0
    ac54: e8bd4070     	pop	{r4, r5, r6, lr}
    ac58: eaffe333     	b	0x392c <.plt+0x230>     @ imm = #-0x7334  // CALL clock_delay
    ac5c: e5946fa0     	ldr	r6, [r4, #0xfa0]
    ac60: eef65b00     	vmov.f64	d21, #5.000000e-01
    ac64: ee046a90     	vmov	s9, r6
    ac68: eef88ae4     	vcvt.f32.s32	s17, s9
    ac6c: ee687aa2     	vmul.f32	s15, s17, s5
    ac70: eef76ae7     	vcvt.f64.f32	d22, s15
    ac74: ee767ba5     	vadd.f64	d23, d22, d21
    ac78: eefd5be7     	vcvt.s32.f64	s11, d23
    ac7c: ee153a90     	vmov	r3, s11
    ac80: e3530000     	cmp	r3, #0
    ac84: b2633000     	rsblt	r3, r3, #0
    ac88: e2832e7b     	add	r2, r3, #1968
    ac8c: e2821004     	add	r1, r2, #4
    ac90: e085c101     	add	r12, r5, r1, lsl #2
    ac94: ed9c0a01     	vldr	s0, [r12, #4]
    ac98: eeb50ac0     	vcmpe.f32	s0, #0
    ac9c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    aca0: da000012     	ble	0xacf0 <tick_bpmClock+0x2b0> @ imm = #0x48
    aca4: ee838a80     	vdiv.f32	s16, s7, s0
    aca8: ed808a00     	vstr	s16, [r0]
    acac: eaffffc3     	b	0xabc0 <tick_bpmClock+0x180> @ imm = #-0xf4
    acb0: e59fe0a4     	ldr	lr, [pc, #0xa4]         @ 0xad5c <tick_bpmClock+0x31c>  // u32=0x1c6f4; f32?=1.6320643e-40
    acb4: e2846c0e     	add	r6, r4, #3584
    acb8: e3a03000     	mov	r3, #0
    acbc: e08f000e     	add	r0, pc, lr
    acc0: e5863004     	str	r3, [r6, #0x4]
    acc4: ee083a10     	vmov	s16, r3
    acc8: e5902080     	ldr	r2, [r0, #0x80]
    accc: e2821001     	add	r1, r2, #1
    acd0: e5801080     	str	r1, [r0, #0x80]
    acd4: eaffffd0     	b	0xac1c <tick_bpmClock+0x1dc> @ imm = #-0xc0
    acd8: ecbd8b02     	vpop	{d8}
    acdc: e8bd4070     	pop	{r4, r5, r6, lr}
    ace0: eaffe36e     	b	0x3aa0 <.plt+0x3a4>     @ imm = #-0x7248  // CALL clock_unset
    ace4: ebffe36d     	bl	0x3aa0 <.plt+0x3a4>     @ imm = #-0x724c  // CALL clock_unset
    ace8: e59500fc     	ldr	r0, [r5, #0xfc]
    acec: eaffffc6     	b	0xac0c <tick_bpmClock+0x1cc> @ imm = #-0xe8
    acf0: e59fe068     	ldr	lr, [pc, #0x68]         @ 0xad60 <tick_bpmClock+0x320>  // u32=0x1c6b8; f32?=1.63122352e-40
    acf4: eeb08a63     	vmov.f32	s16, s7
    acf8: e08f000e     	add	r0, pc, lr
    acfc: e5906080     	ldr	r6, [r0, #0x80]
    ad00: e2863001     	add	r3, r6, #1
    ad04: e5803080     	str	r3, [r0, #0x80]
    ad08: eaffffb4     	b	0xabe0 <tick_bpmClock+0x1a0> @ imm = #-0x130
    ad0c: ed9f8a09     	vldr	s16, [pc, #36]          @ 0xad38 <tick_bpmClock+0x2f8>  // f32=0
    ad10: eaffffc1     	b	0xac1c <tick_bpmClock+0x1dc> @ imm = #-0xfc
    ad14: e320f000     	nop
    ad18: 9a 99 99 99  	.word	0x9999999a
    ad1c: 99 99 f1 3f  	.word	0x3ff19999
    ad20: 9a 99 99 99  	.word	0x9999999a
    ad24: 99 99 b9 3f  	.word	0x3fb99999
    ad28: 00 00 00 00  	.word	0x00000000
    ad2c: 00 00 00 00  	.word	0x00000000
    ad30: 00 60 6a 47  	.word	0x476a6000
    ad34: ab aa aa 3f  	.word	0x3faaaaab
    ad38: 00 00 00 00  	.word	0x00000000
    ad3c: 84 c8 01 00  	.word	0x0001c884
    ad40: 2c c9 01 00  	.word	0x0001c92c
    ad44: e4 aa 00 00  	.word	0x0000aae4
    ad48: e4 c8 01 00  	.word	0x0001c8e4
    ad4c: 94 c7 01 00  	.word	0x0001c794
    ad50: e8 c7 01 00  	.word	0x0001c7e8
    ad54: cc c7 01 00  	.word	0x0001c7cc
    ad58: 8c c7 01 00  	.word	0x0001c78c
    ad5c: f4 c6 01 00  	.word	0x0001c6f4
    ad60: b8 c6 01 00  	.word	0x0001c6b8

