; lubadh::AudioEngine::recordInput(lubadh::Channel&, lubadh::Tap const&, lubadh::Channel::InputBuffer&, float, float)
; VA 0x47818 size 1008

   47818: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   4781c: e1a06002     	mov	r6, r2
   47820: e5d22014     	ldrb	r2, [r2, #0x14]
   47824: e1a04003     	mov	r4, r3
   47828: e59130e8     	ldr	r3, [r1, #0xe8]
   4782c: e3520000     	cmp	r2, #0
   47830: ed2d8b02     	vpush	{d8}
   47834: ed9f7aef     	vldr	s14, [pc, #956]         @ 0x47bf8 ; float 9.99999997475e-07
   47838: e24dd01c     	sub	sp, sp, #28
   4783c: e5965004     	ldr	r5, [r6, #0x4]
   47840: e1a07001     	mov	r7, r1
   47844: ed938a00     	vldr	s16, [r3]
   47848: 1d968a06     	vldrne	s16, [r6, #24]
   4784c: e596300c     	ldr	r3, [r6, #0xc]
   47850: edcd0a05     	vstr	s1, [sp, #20]
   47854: ee208a08     	vmul.f32	s16, s0, s16
   47858: eef07ac8     	vabs.f32	s15, s16
   4785c: eef47ac7     	vcmpe.f32	s15, s14
   47860: eef1fa10     	vmrs	APSR_nzcv, fpscr
   47864: 5eb77a00     	vmovpl.f32	s14, #1.000000e+00
   47868: 4ddf8ae3     	vldrmi	s17, [pc, #908]         @ 0x47bfc ; float 0
   4786c: 5ec78a27     	vdivpl.f32	s17, s14, s15
   47870: e1550003     	cmp	r5, r3
   47874: 1a000008     	bne	0x4789c
   47878: edd47a04     	vldr	s15, [r4, #16]
   4787c: ee788aa7     	vadd.f32	s17, s17, s15
   47880: eefd7ae8     	vcvt.s32.f32	s15, s17
   47884: eef87ae7     	vcvt.f32.s32	s15, s15
   47888: ee788ae7     	vsub.f32	s17, s17, s15
   4788c: edc48a04     	vstr	s17, [r4, #16]
   47890: e28dd01c     	add	sp, sp, #28
   47894: ecbd8b02     	vpop	{d8}
   47898: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
   4789c: e281aa2a     	add	r10, r1, #172032
   478a0: e0455003     	sub	r5, r5, r3
   478a4: e3550000     	cmp	r5, #0
   478a8: e28a003c     	add	r0, r10, #60
   478ac: b2655000     	rsblt	r5, r5, #0
   478b0: e28d2014     	add	r2, sp, #20
   478b4: e59a303c     	ldr	r3, [r10, #0x3c]
   478b8: e28a8024     	add	r8, r10, #36
   478bc: e59a1040     	ldr	r1, [r10, #0x40]
   478c0: e28ac030     	add	r12, r10, #48
   478c4: e58d000c     	str	r0, [sp, #0xc]
   478c8: e0411003     	sub	r1, r1, r3
   478cc: e58dc010     	str	r12, [sp, #0x10]
   478d0: e1a01141     	asr	r1, r1, #2
   478d4: eb000a9f     	bl	0x4a358
   478d8: e1a02005     	mov	r2, r5
   478dc: e1a01008     	mov	r1, r8
   478e0: e1a00006     	mov	r0, r6
   478e4: eb0011a4     	bl	0x4bf7c
   478e8: e2873a01     	add	r3, r7, #4096
   478ec: e5d32968     	ldrb	r2, [r3, #0x968]
   478f0: e3520000     	cmp	r2, #0
   478f4: 0a00002d     	beq	0x479b0
   478f8: e593c974     	ldr	r12, [r3, #0x974]
   478fc: e2831e97     	add	r1, r3, #2416
   47900: ee07ca90     	vmov	s15, r12
   47904: e593296c     	ldr	r2, [r3, #0x96c]
   47908: ed9f5abc     	vldr	s10, [pc, #752]         @ 0x47c00 ; float 0.10000000149
   4790c: eeb87ae7     	vcvt.f32.s32	s14, s15
   47910: ee072a90     	vmov	s15, r2
   47914: ed916a00     	vldr	s12, [r1]
   47918: eef86ae7     	vcvt.f32.s32	s13, s15
   4791c: edd17a02     	vldr	s15, [r1, #8]
   47920: ee377a27     	vadd.f32	s14, s14, s15
   47924: ee766a86     	vadd.f32	s13, s13, s12
   47928: ee367ac7     	vsub.f32	s14, s13, s14
   4792c: eef05ac7     	vabs.f32	s11, s14
   47930: eef45ac5     	vcmpe.f32	s11, s10
   47934: eef1fa10     	vmrs	APSR_nzcv, fpscr
   47938: 5a000045     	bpl	0x47a54
   4793c: e26200ff     	rsb	r0, r2, #255
   47940: e26210fe     	rsb	r1, r2, #254
   47944: e3033f98     	movw	r3, #0x3f98
   47948: e3403009     	movt	r3, #0x9
   4794c: e0831101     	add	r1, r3, r1, lsl #2
   47950: e0830100     	add	r0, r3, r0, lsl #2
   47954: eef76a00     	vmov.f32	s13, #1.000000e+00
   47958: ed9d7a05     	vldr	s14, [sp, #20]
   4795c: edd07a00     	vldr	s15, [r0]
   47960: ee364ac6     	vsub.f32	s8, s13, s12
   47964: edd14a00     	vldr	s9, [r1]
   47968: e0831102     	add	r1, r3, r2, lsl #2
   4796c: ee765ac7     	vsub.f32	s11, s13, s14
   47970: e59a3024     	ldr	r3, [r10, #0x24]
   47974: e59a203c     	ldr	r2, [r10, #0x3c]
   47978: ee744ae7     	vsub.f32	s9, s9, s15
   4797c: edd16a00     	vldr	s13, [r1]
   47980: ed915a01     	vldr	s10, [r1, #4]
   47984: e0831105     	add	r1, r3, r5, lsl #2
   47988: eee47a24     	vfma.f32	s15, s8, s9
   4798c: ee355a66     	vsub.f32	s10, s10, s13
   47990: eee66a05     	vfma.f32	s13, s12, s10
   47994: eea57aa7     	vfma.f32	s14, s11, s15
   47998: edd37a00     	vldr	s15, [r3]
   4799c: ee677aa6     	vmul.f32	s15, s15, s13
   479a0: ece37a01     	vstmia	r3!, {s15}
   479a4: e1510003     	cmp	r1, r3
   479a8: eca27a01     	vstmia	r2!, {s14}
   479ac: 1afffff9     	bne	0x47998
   479b0: eeb58ac0     	vcmpe.f32	s16, #0
   479b4: e596900c     	ldr	r9, [r6, #0xc]
   479b8: ed940a04     	vldr	s0, [r4, #16]
   479bc: e3a0b000     	mov	r11, #0
   479c0: eef1fa10     	vmrs	APSR_nzcv, fpscr
   479c4: c3a06001     	movgt	r6, #1
   479c8: d3a06000     	movle	r6, #0
   479cc: 43a03001     	movmi	r3, #1
   479d0: 53a03000     	movpl	r3, #0
   479d4: e0466003     	sub	r6, r6, r3
   479d8: e1a00004     	mov	r0, r4
   479dc: ebffc434     	bl	0x38ab4
   479e0: e59a3024     	ldr	r3, [r10, #0x24]
   479e4: e59a2030     	ldr	r2, [r10, #0x30]
   479e8: e083310b     	add	r3, r3, r11, lsl #2
   479ec: edd37a00     	vldr	s15, [r3]
   479f0: ee677a80     	vmul.f32	s15, s15, s0
   479f4: edc37a00     	vstr	s15, [r3]
   479f8: ed940a04     	vldr	s0, [r4, #16]
   479fc: e782910b     	str	r9, [r2, r11, lsl #2]
   47a00: e28bb001     	add	r11, r11, #1
   47a04: e155000b     	cmp	r5, r11
   47a08: e0899006     	add	r9, r9, r6
   47a0c: ee380a80     	vadd.f32	s0, s17, s0
   47a10: ed840a04     	vstr	s0, [r4, #16]
   47a14: caffffef     	bgt	0x479d8
   47a18: eefd7ac0     	vcvt.s32.f32	s15, s0
   47a1c: e59d300c     	ldr	r3, [sp, #0xc]
   47a20: e2870ba7     	add	r0, r7, #171008
   47a24: e58d3000     	str	r3, [sp]
   47a28: e2800fdf     	add	r0, r0, #892
   47a2c: e59d3010     	ldr	r3, [sp, #0x10]
   47a30: e1a02008     	mov	r2, r8
   47a34: e1a01005     	mov	r1, r5
   47a38: eef87ae7     	vcvt.f32.s32	s15, s15
   47a3c: ee300a67     	vsub.f32	s0, s0, s15
   47a40: ed840a04     	vstr	s0, [r4, #16]
   47a44: ebffc36c     	bl	0x387fc
   47a48: e28dd01c     	add	sp, sp, #28
   47a4c: ecbd8b02     	vpop	{d8}
   47a50: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
   47a54: ee065a10     	vmov	s12, r5
   47a58: e3520000     	cmp	r2, #0
   47a5c: eeb86ac6     	vcvt.f32.s32	s12, s12
   47a60: eec73a06     	vdiv.f32	s7, s14, s12
   47a64: ee855a86     	vdiv.f32	s10, s11, s12
   47a68: ba000044     	blt	0x47b80
   47a6c: e35200fe     	cmp	r2, #254
   47a70: da00005e     	ble	0x47bf0
   47a74: ed9f7a62     	vldr	s14, [pc, #392]         @ 0x47c04 ; float 254
   47a78: ee766ac7     	vsub.f32	s13, s13, s14
   47a7c: ee867a85     	vdiv.f32	s14, s13, s10
   47a80: eebd7ac7     	vcvt.s32.f32	s14, s14
   47a84: ee173a10     	vmov	r3, s14
   47a88: e2833001     	add	r3, r3, #1
   47a8c: e1530005     	cmp	r3, r5
   47a90: a1a03005     	movge	r3, r5
   47a94: e3530000     	cmp	r3, #0
   47a98: a0452003     	subge	r2, r5, r3
   47a9c: b1a02005     	movlt	r2, r5
   47aa0: e3520000     	cmp	r2, #0
   47aa4: daffffc1     	ble	0x479b0
   47aa8: e59ae024     	ldr	lr, [r10, #0x24]
   47aac: e3031f98     	movw	r1, #0x3f98
   47ab0: e3401009     	movt	r1, #0x9
   47ab4: e59ab03c     	ldr	r11, [r10, #0x3c]
   47ab8: eeb77a00     	vmov.f32	s14, #1.000000e+00
   47abc: e08e9102     	add	r9, lr, r2, lsl #2
   47ac0: e28c3001     	add	r3, r12, #1
   47ac4: e081010c     	add	r0, r1, r12, lsl #2
   47ac8: ee374a67     	vsub.f32	s8, s14, s15
   47acc: ed9e5a00     	vldr	s10, [lr]
   47ad0: e0812103     	add	r2, r1, r3, lsl #2
   47ad4: ed906a00     	vldr	s12, [r0]
   47ad8: e26c00ff     	rsb	r0, r12, #255
   47adc: edd25a00     	vldr	s11, [r2]
   47ae0: e26c20fe     	rsb	r2, r12, #254
   47ae4: e0810100     	add	r0, r1, r0, lsl #2
   47ae8: e0812102     	add	r2, r1, r2, lsl #2
   47aec: ee755ac6     	vsub.f32	s11, s11, s12
   47af0: edd06a00     	vldr	s13, [r0]
   47af4: edd24a00     	vldr	s9, [r2]
   47af8: eea56aa7     	vfma.f32	s12, s11, s15
   47afc: ee777aa3     	vadd.f32	s15, s15, s7
   47b00: eef47ac7     	vcmpe.f32	s15, s14
   47b04: eef05a46     	vmov.f32	s11, s12
   47b08: ee346ae6     	vsub.f32	s12, s9, s13
   47b0c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   47b10: eee46a06     	vfma.f32	s13, s8, s12
   47b14: ee655a25     	vmul.f32	s11, s10, s11
   47b18: ecee5a01     	vstmia	lr!, {s11}
   47b1c: eeb06a66     	vmov.f32	s12, s13
   47b20: eddd6a05     	vldr	s13, [sp, #20]
   47b24: ee775a66     	vsub.f32	s11, s14, s13
   47b28: eee56a86     	vfma.f32	s13, s11, s12
   47b2c: eceb6a01     	vstmia	r11!, {s13}
   47b30: ba000005     	blt	0x47b4c
   47b34: ee777ac7     	vsub.f32	s15, s15, s14
   47b38: e1a0c003     	mov	r12, r3
   47b3c: e2833001     	add	r3, r3, #1
   47b40: eef47ac7     	vcmpe.f32	s15, s14
   47b44: eef1fa10     	vmrs	APSR_nzcv, fpscr
   47b48: aafffff9     	bge	0x47b34
   47b4c: eef57ac0     	vcmpe.f32	s15, #0
   47b50: eef1fa10     	vmrs	APSR_nzcv, fpscr
   47b54: 424c3001     	submi	r3, r12, #1
   47b58: 5a000005     	bpl	0x47b74
   47b5c: ee777a87     	vadd.f32	s15, s15, s14
   47b60: e1a0c003     	mov	r12, r3
   47b64: e2433001     	sub	r3, r3, #1
   47b68: eef57ac0     	vcmpe.f32	s15, #0
   47b6c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   47b70: 4afffff9     	bmi	0x47b5c
   47b74: e159000e     	cmp	r9, lr
   47b78: 1affffd0     	bne	0x47ac0
   47b7c: eaffff8b     	b	0x479b0
   47b80: ed9f7a1d     	vldr	s14, [pc, #116]         @ 0x47bfc ; float 0
   47b84: ee776a66     	vsub.f32	s13, s14, s13
   47b88: ee866a85     	vdiv.f32	s12, s13, s10
   47b8c: eefd6ac6     	vcvt.s32.f32	s13, s12
   47b90: ee163a90     	vmov	r3, s13
   47b94: e2833001     	add	r3, r3, #1
   47b98: e1530005     	cmp	r3, r5
   47b9c: a1a03005     	movge	r3, r5
   47ba0: e3530000     	cmp	r3, #0
   47ba4: a0452003     	subge	r2, r5, r3
   47ba8: b1a02005     	movlt	r2, r5
   47bac: e1550002     	cmp	r5, r2
   47bb0: daffffbc     	ble	0x47aa8
   47bb4: e59a1024     	ldr	r1, [r10, #0x24]
   47bb8: e1a0e102     	lsl	lr, r2, #2
   47bbc: e59ab03c     	ldr	r11, [r10, #0x3c]
   47bc0: eef06a47     	vmov.f32	s13, s14
   47bc4: e081300e     	add	r3, r1, lr
   47bc8: e3a005fe     	mov	r0, #1065353216
   47bcc: e0811105     	add	r1, r1, r5, lsl #2
   47bd0: e08be00e     	add	lr, r11, lr
   47bd4: ed937a00     	vldr	s14, [r3]
   47bd8: ee277a26     	vmul.f32	s14, s14, s13
   47bdc: eca37a01     	vstmia	r3!, {s14}
   47be0: e1510003     	cmp	r1, r3
   47be4: e48e0004     	str	r0, [lr], #4
   47be8: 1afffff9     	bne	0x47bd4
   47bec: eaffffab     	b	0x47aa0
   47bf0: e1a02005     	mov	r2, r5
   47bf4: eaffffab     	b	0x47aa8
   47bf8: bd 37 86 35  	.word	0x358637bd
   47bfc: 00 00 00 00  	.word	0x00000000
   47c00: cd cc cc 3d  	.word	0x3dcccccd
   47c04: 00 00 7e 43  	.word	0x437e0000
