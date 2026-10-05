0000a878 <_adaptInternalBpm>:
    a878: e92d41f0     	push	{r4, r5, r6, r7, r8, lr}
    a87c: e2806c1e     	add	r6, r0, #7680
    a880: e2804a01     	add	r4, r0, #4096
    a884: e24dd010     	sub	sp, sp, #16
    a888: edd60a00     	vldr	s1, [r6]
    a88c: e2842edf     	add	r2, r4, #3568
    a890: e2825008     	add	r5, r2, #8
    a894: edd57a00     	vldr	s15, [r5]
    a898: eef50ac0     	vcmpe.f32	s1, #0
    a89c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    a8a0: da000028     	ble	0xa948 <_adaptInternalBpm+0xd0> @ imm = #0xa0
    a8a4: eeb73ae7     	vcvt.f64.f32	d3, s15
    a8a8: e2841edf     	add	r1, r4, #3568
    a8ac: ed9f4b5b     	vldr	d4, [pc, #364]          @ 0xaa20 <_adaptInternalBpm+0x1a8>  // f64=0.10000000000000001
    a8b0: e3a08000     	mov	r8, #0
    a8b4: e34b8f80     	movt	r8, #0xbf80
    a8b8: e581800c     	str	r8, [r1, #0xc]
    a8bc: ee771ae0     	vsub.f32	s3, s15, s1
    a8c0: ee235b04     	vmul.f64	d5, d3, d4
    a8c4: eef02ae1     	vabs.f32	s5, s3
    a8c8: eeb76ae2     	vcvt.f64.f32	d6, s5
    a8cc: eeb46bc5     	vcmpe.f64	d6, d5
    a8d0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    a8d4: da00000c     	ble	0xa90c <_adaptInternalBpm+0x94> @ imm = #0x30
    a8d8: eddf3a52     	vldr	s7, [pc, #328]          @ 0xaa28 <_adaptInternalBpm+0x1b0>  // f32=0.899999976
    a8dc: e3a00000     	mov	r0, #0
    a8e0: eddf5a51     	vldr	s11, [pc, #324]         @ 0xaa2c <_adaptInternalBpm+0x1b4>  // f32=0.100000001
    a8e4: ee604aa3     	vmul.f32	s9, s1, s7
    a8e8: ee078a10     	vmov	s14, r8
    a8ec: ee474aa5     	vmla.f32	s9, s15, s11
    a8f0: eef07a64     	vmov.f32	s15, s9
    a8f4: edc54a00     	vstr	s9, [r5]
    a8f8: eef47a60     	vcmp.f32	s15, s1
    a8fc: eef1fa10     	vmrs	APSR_nzcv, fpscr
    a900: 1a000008     	bne	0xa928 <_adaptInternalBpm+0xb0> @ imm = #0x20
    a904: eef07a60     	vmov.f32	s15, s1
    a908: ea000003     	b	0xa91c <_adaptInternalBpm+0xa4> @ imm = #0xc
    a90c: edc50a00     	vstr	s1, [r5]
    a910: e3a00000     	mov	r0, #0
    a914: eef07a60     	vmov.f32	s15, s1
    a918: ee078a10     	vmov	s14, r8
    a91c: e3a08000     	mov	r8, #0
    a920: e34b8f80     	movt	r8, #0xbf80
    a924: e5868000     	str	r8, [r6]
    a928: eef47a47     	vcmp.f32	s15, s14
    a92c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    a930: 02844edf     	addeq	r4, r4, #3568
    a934: 03a08000     	moveq	r8, #0
    a938: 034b8f80     	movteq	r8, #0xbf80
    a93c: 0584800c     	streq	r8, [r4, #0xc]
    a940: e28dd010     	add	sp, sp, #16
    a944: e8bd81f0     	pop	{r4, r5, r6, r7, r8, pc}
    a948: e2843edf     	add	r3, r4, #3568
    a94c: e283800c     	add	r8, r3, #12
    a950: ed937a03     	vldr	s14, [r3, #12]
    a954: eeb57ac0     	vcmpe.f32	s14, #0
    a958: eef1fa10     	vmrs	APSR_nzcv, fpscr
    a95c: d3a00001     	movle	r0, #1
    a960: daffffe4     	ble	0xa8f8 <_adaptInternalBpm+0x80> @ imm = #-0x70
    a964: eef71ae7     	vcvt.f64.f32	d17, s15
    a968: e1a05000     	mov	r5, r0
    a96c: eddf0b2b     	vldr	d16, [pc, #172]         @ 0xaa20 <_adaptInternalBpm+0x1a8>  // f64=0.10000000000000001
    a970: e2841edf     	add	r1, r4, #3568
    a974: e59f00bc     	ldr	r0, [pc, #0xbc]         @ 0xaa38 <_adaptInternalBpm+0x1c0>  // u32=0xabf4; f32?=6.16851584e-41
    a978: e3a08002     	mov	r8, #2
    a97c: e58d8000     	str	r8, [sp]
    a980: e2847edf     	add	r7, r4, #3568
    a984: e08f0000     	add	r0, pc, r0
    a988: e287700c     	add	r7, r7, #12
    a98c: ee776ac7     	vsub.f32	s13, s15, s14
    a990: ee210ba0     	vmul.f64	d0, d17, d16
    a994: eeb01ae6     	vabs.f32	s2, s13
    a998: eeb72ac1     	vcvt.f64.f32	d2, s2
    a99c: eeb42bc0     	vcmpe.f64	d2, d0
    a9a0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    a9a4: cd9f6a21     	vldrgt	s12, [pc, #132]         @ 0xaa30 <_adaptInternalBpm+0x1b8>
    a9a8: cd9f1a21     	vldrgt	s2, [pc, #132]          @ 0xaa34 <_adaptInternalBpm+0x1bc>
    a9ac: ce277a06     	vmulgt.f32	s14, s14, s12
    a9b0: ce077a81     	vmlagt.f32	s14, s15, s2
    a9b4: ed817a02     	vstr	s14, [r1, #8]
    a9b8: ebffe35a     	bl	0x3728 <.plt+0x2c>      @ imm = #-0x7298  // CALL gensym
    a9bc: e5d5c030     	ldrb	r12, [r5, #0x30]
    a9c0: e3a02001     	mov	r2, #1
    a9c4: e58d2008     	str	r2, [sp, #0x8]
    a9c8: ed977a00     	vldr	s14, [r7]
    a9cc: e35c0062     	cmp	r12, #98
    a9d0: ed8d7a03     	vstr	s14, [sp, #12]
    a9d4: e58d0004     	str	r0, [sp, #0x4]
    a9d8: 9a000005     	bls	0xa9f4 <_adaptInternalBpm+0x17c> @ imm = #0x14
    a9dc: e2843edf     	add	r3, r4, #3568
    a9e0: e3a00000     	mov	r0, #0
    a9e4: edd60a00     	vldr	s1, [r6]
    a9e8: e2838008     	add	r8, r3, #8
    a9ec: edd37a02     	vldr	s15, [r3, #8]
    a9f0: eaffffc0     	b	0xa8f8 <_adaptInternalBpm+0x80> @ imm = #-0x100
    a9f4: e59fe040     	ldr	lr, [pc, #0x40]         @ 0xaa3c <_adaptInternalBpm+0x1c4>  // u32=0xa0b8; f32?=5.7655024e-41
    a9f8: e5945db4     	ldr	r5, [r4, #0xdb4]
    a9fc: e08f000e     	add	r0, pc, lr
    aa00: ebffe348     	bl	0x3728 <.plt+0x2c>      @ imm = #-0x72e0  // CALL gensym
    aa04: e1a02008     	mov	r2, r8
    aa08: e1a0300d     	mov	r3, sp
    aa0c: e1a01000     	mov	r1, r0
    aa10: e1a00005     	mov	r0, r5
    aa14: ebffe496     	bl	0x3c74 <.plt+0x578>     @ imm = #-0x6da8  // CALL outlet_list
    aa18: ed977a00     	vldr	s14, [r7]
    aa1c: eaffffee     	b	0xa9dc <_adaptInternalBpm+0x164> @ imm = #-0x48
    aa20: 9a 99 99 99  	.word	0x9999999a
    aa24: 99 99 b9 3f  	.word	0x3fb99999
    aa28: 66 66 66 3f  	.word	0x3f666666
    aa2c: cd cc cc 3d  	.word	0x3dcccccd
    aa30: 9a 99 99 3e  	.word	0x3e99999a
    aa34: 33 33 33 3f  	.word	0x3f333333
    aa38: f4 ab 00 00  	.word	0x0000abf4
    aa3c: b8 a0 00 00  	.word	0x0000a0b8

