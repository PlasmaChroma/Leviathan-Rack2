000046c0 <_takeCalibrationValues>:
    46c0: e59f3260     	ldr	r3, [pc, #0x260]        @ 0x4928 <_takeCalibrationValues+0x268>  // u32=0x22ce8; f32?=1.99780319e-40
    46c4: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
    46c8: e08f1003     	add	r1, pc, r3
    46cc: ed2d8b02     	vpush	{d8}
    46d0: e591b000     	ldr	r11, [r1]
    46d4: e35b0000     	cmp	r11, #0
    46d8: e24dd05c     	sub	sp, sp, #92
    46dc: 0a000006     	beq	0x46fc <_takeCalibrationValues+0x3c> @ imm = #0x18
    46e0: e59f0244     	ldr	r0, [pc, #0x244]        @ 0x492c <_takeCalibrationValues+0x26c>  // u32=0x22cc8; f32?=1.99735478e-40
    46e4: e28b6001     	add	r6, r11, #1
    46e8: e08f2000     	add	r2, pc, r0
    46ec: e5826000     	str	r6, [r2]
    46f0: e28dd05c     	add	sp, sp, #92
    46f4: ecbd8b02     	vpop	{d8}
    46f8: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
    46fc: e1a07000     	mov	r7, r0
    4700: ebfffd2e     	bl	0x3bc0 <.plt+0x4c4>     @ imm = #-0xb48  // CALL _learnMid
    4704: e59f0224     	ldr	r0, [pc, #0x224]        @ 0x4930 <_takeCalibrationValues+0x270>  // u32=0x103a8; f32?=9.31471115e-41
    4708: e2879a01     	add	r9, r7, #4096
    470c: e59f8220     	ldr	r8, [pc, #0x220]        @ 0x4934 <_takeCalibrationValues+0x274>  // u32=0x10394; f32?=9.31190856e-41
    4710: e28da040     	add	r10, sp, #64
    4714: e08f0000     	add	r0, pc, r0
    4718: e59f5218     	ldr	r5, [pc, #0x218]        @ 0x4938 <_takeCalibrationValues+0x278>  // u32=0xe8ec; f32?=8.35566248e-41
    471c: ebfffd15     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0xbac  // CALL post
    4720: e08f8008     	add	r8, pc, r8
    4724: e59f2210     	ldr	r2, [pc, #0x210]        @ 0x493c <_takeCalibrationValues+0x27c>  // u32=0x103a0; f32?=9.31359011e-41
    4728: e08f5005     	add	r5, pc, r5
    472c: e2856010     	add	r6, r5, #16
    4730: e58da014     	str	r10, [sp, #0x14]
    4734: e08f4002     	add	r4, pc, r2
    4738: e58d6018     	str	r6, [sp, #0x18]
    473c: e58d401c     	str	r4, [sp, #0x1c]
    4740: e4d51001     	ldrb	r1, [r5], #1
    4744: e3a0bf71     	mov	r11, #452
    4748: e59d001c     	ldr	r0, [sp, #0x1c]
    474c: e3a04001     	mov	r4, #1
    4750: e02c719b     	mla	r12, r11, r1, r7
    4754: eddc7a4a     	vldr	s15, [r12, #296]
    4758: e5dc211c     	ldrb	r2, [r12, #0x11c]
    475c: e282a0c0     	add	r10, r2, #192
    4760: eebd7ae7     	vcvt.s32.f32	s14, s15
    4764: edcc7ab0     	vstr	s15, [r12, #704]
    4768: e58da000     	str	r10, [sp]
    476c: ee173a10     	vmov	r3, s14
    4770: ed8d7a03     	vstr	s14, [sp, #12]
    4774: e203603f     	and	r6, r3, #63
    4778: e1a0b343     	asr	r11, r3, #6
    477c: e58d6008     	str	r6, [sp, #0x8]
    4780: e58db004     	str	r11, [sp, #0x4]
    4784: ebfffcfb     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0xc14  // CALL post
    4788: ee006a10     	vmov	s0, r6
    478c: ee01ba10     	vmov	s2, r11
    4790: e1a00008     	mov	r0, r8
    4794: ee01aa90     	vmov	s3, r10
    4798: e5996db0     	ldr	r6, [r9, #0xdb0]
    479c: eef80ac0     	vcvt.f32.s32	s1, s0
    47a0: e58d4040     	str	r4, [sp, #0x40]
    47a4: e58d4048     	str	r4, [sp, #0x48]
    47a8: e58d4050     	str	r4, [sp, #0x50]
    47ac: eef88ac1     	vcvt.f32.s32	s17, s2
    47b0: edcd0a13     	vstr	s1, [sp, #76]
    47b4: eeb88ae1     	vcvt.f32.s32	s16, s3
    47b8: edcd8a15     	vstr	s17, [sp, #84]
    47bc: ed8d8a11     	vstr	s16, [sp, #68]
    47c0: ebfffbd8     	bl	0x3728 <.plt+0x2c>      @ imm = #-0x10a0  // CALL gensym
    47c4: e59d3014     	ldr	r3, [sp, #0x14]
    47c8: e3a02003     	mov	r2, #3
    47cc: e1a01000     	mov	r1, r0
    47d0: e1a00006     	mov	r0, r6
    47d4: ebfffd26     	bl	0x3c74 <.plt+0x578>     @ imm = #-0xb68  // CALL outlet_list
    47d8: e59d3018     	ldr	r3, [sp, #0x18]
    47dc: e1530005     	cmp	r3, r5
    47e0: 1affffd6     	bne	0x4740 <_takeCalibrationValues+0x80> @ imm = #-0xa8
    47e4: e5975020     	ldr	r5, [r7, #0x20]
    47e8: e1a00008     	mov	r0, r8
    47ec: e599bdb0     	ldr	r11, [r9, #0xdb0]
    47f0: e3a0e000     	mov	lr, #0
    47f4: e205103f     	and	r1, r5, #63
    47f8: e344e350     	movt	lr, #0x4350
    47fc: e1a0a345     	asr	r10, r5, #6
    4800: e58de044     	str	lr, [sp, #0x44]
    4804: ee081a90     	vmov	s17, r1
    4808: e58d4040     	str	r4, [sp, #0x40]
    480c: ee02aa90     	vmov	s5, r10
    4810: e58d4048     	str	r4, [sp, #0x48]
    4814: eeb82ae8     	vcvt.f32.s32	s4, s17
    4818: e58d4050     	str	r4, [sp, #0x50]
    481c: eeb83ae2     	vcvt.f32.s32	s6, s5
    4820: ed8d2a13     	vstr	s4, [sp, #76]
    4824: ed8d3a15     	vstr	s6, [sp, #84]
    4828: ebfffbbe     	bl	0x3728 <.plt+0x2c>      @ imm = #-0x1108  // CALL gensym
    482c: e59d3014     	ldr	r3, [sp, #0x14]
    4830: e3a02003     	mov	r2, #3
    4834: e1a01000     	mov	r1, r0
    4838: e1a0000b     	mov	r0, r11
    483c: ebfffd0c     	bl	0x3c74 <.plt+0x578>     @ imm = #-0xbd0  // CALL outlet_list
    4840: e59f00f8     	ldr	r0, [pc, #0xf8]         @ 0x4940 <_takeCalibrationValues+0x280>  // u32=0x102c8; f32?=9.28332207e-41
    4844: e1a0300a     	mov	r3, r10
    4848: e1a01005     	mov	r1, r5
    484c: e58d5004     	str	r5, [sp, #0x4]
    4850: e3a020d0     	mov	r2, #208
    4854: e08f0000     	add	r0, pc, r0
    4858: edcd8a00     	vstr	s17, [sp]
    485c: ebfffcc5     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0xcec  // CALL post
    4860: e1a00008     	mov	r0, r8
    4864: e5996db0     	ldr	r6, [r9, #0xdb0]
    4868: e3a02000     	mov	r2, #0
    486c: e58d4030     	str	r4, [sp, #0x30]
    4870: e344237f     	movt	r2, #0x437f
    4874: e58d4038     	str	r4, [sp, #0x38]
    4878: e3a05064     	mov	r5, #100
    487c: e58d2034     	str	r2, [sp, #0x34]
    4880: e58d203c     	str	r2, [sp, #0x3c]
    4884: ebfffba7     	bl	0x3728 <.plt+0x2c>      @ imm = #-0x1164  // CALL gensym
    4888: e28d3030     	add	r3, sp, #48
    488c: e3a02002     	mov	r2, #2
    4890: e1a01000     	mov	r1, r0
    4894: e1a00006     	mov	r0, r6
    4898: ebfffcf5     	bl	0x3c74 <.plt+0x578>     @ imm = #-0xc2c  // CALL outlet_list
    489c: e1a00008     	mov	r0, r8
    48a0: e5999db0     	ldr	r9, [r9, #0xdb0]
    48a4: e3a0c000     	mov	r12, #0
    48a8: e58d4020     	str	r4, [sp, #0x20]
    48ac: e344c37e     	movt	r12, #0x437e
    48b0: e58dc024     	str	r12, [sp, #0x24]
    48b4: ebfffb9b     	bl	0x3728 <.plt+0x2c>      @ imm = #-0x1194  // CALL gensym
    48b8: e28d3020     	add	r3, sp, #32
    48bc: e1a02004     	mov	r2, r4
    48c0: e1a01000     	mov	r1, r0
    48c4: e1a00009     	mov	r0, r9
    48c8: ebfffce9     	bl	0x3c74 <.plt+0x578>     @ imm = #-0xc5c  // CALL outlet_list
    48cc: e59f3070     	ldr	r3, [pc, #0x70]         @ 0x4944 <_takeCalibrationValues+0x284>  // u32=0x10278; f32?=9.27211168e-41
    48d0: e5c75030     	strb	r5, [r7, #0x30]
    48d4: e3a01002     	mov	r1, #2
    48d8: e08f0003     	add	r0, pc, r3
    48dc: e58d1028     	str	r1, [sp, #0x28]
    48e0: ebfffb90     	bl	0x3728 <.plt+0x2c>      @ imm = #-0x11c0  // CALL gensym
    48e4: e5d7a030     	ldrb	r10, [r7, #0x30]
    48e8: e35a0062     	cmp	r10, #98
    48ec: e58d002c     	str	r0, [sp, #0x2c]
    48f0: 8a000003     	bhi	0x4904 <_takeCalibrationValues+0x244> @ imm = #0xc
    48f4: e59f804c     	ldr	r8, [pc, #0x4c]         @ 0x4948 <_takeCalibrationValues+0x288>  // u32=0x22ab8; f32?=1.98995592e-40
    48f8: e08f4008     	add	r4, pc, r8
    48fc: e594b000     	ldr	r11, [r4]
    4900: eaffff76     	b	0x46e0 <_takeCalibrationValues+0x20> @ imm = #-0x228
    4904: e1a00008     	mov	r0, r8
    4908: e597706c     	ldr	r7, [r7, #0x6c]
    490c: ebfffb85     	bl	0x3728 <.plt+0x2c>      @ imm = #-0x11ec  // CALL gensym
    4910: e1a02004     	mov	r2, r4
    4914: e28d3028     	add	r3, sp, #40
    4918: e1a01000     	mov	r1, r0
    491c: e1a00007     	mov	r0, r7
    4920: ebfffcd3     	bl	0x3c74 <.plt+0x578>     @ imm = #-0xcb4  // CALL outlet_list
    4924: eafffff2     	b	0x48f4 <_takeCalibrationValues+0x234> @ imm = #-0x38
    4928: e8 2c 02 00  	.word	0x00022ce8
    492c: c8 2c 02 00  	.word	0x00022cc8
    4930: a8 03 01 00  	.word	0x000103a8
    4934: 94 03 01 00  	.word	0x00010394
    4938: ec e8 00 00  	.word	0x0000e8ec
    493c: a0 03 01 00  	.word	0x000103a0
    4940: c8 02 01 00  	.word	0x000102c8
    4944: 78 02 01 00  	.word	0x00010278
    4948: b8 2a 02 00  	.word	0x00022ab8

