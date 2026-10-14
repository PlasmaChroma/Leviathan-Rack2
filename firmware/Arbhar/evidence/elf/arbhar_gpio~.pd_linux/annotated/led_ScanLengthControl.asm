00006648 <led_ScanLengthControl>:
    6648: e30034ae     	movw	r3, #0x4ae
    664c: e3002d82     	movw	r2, #0xd82
    6650: e19010b3     	ldrh	r1, [r0, r3]
    6654: e190c0b2     	ldrh	r12, [r0, r2]
    6658: e92d4010     	push	{r4, lr}
    665c: e241eb02     	sub	lr, r1, #2048
    6660: ed2d8b06     	vpush	{d8, d9, d10}
    6664: e1a04000     	mov	r4, r0
    6668: ee07ca90     	vmov	s15, r12
    666c: ee07ea10     	vmov	s14, lr
    6670: e24dd020     	sub	sp, sp, #32
    6674: ed9f0a87     	vldr	s0, [pc, #540]          @ 0x6898 <led_ScanLengthControl+0x250>  // f32=208
    6678: eef89ac7     	vcvt.f32.s32	s19, s14
    667c: ed9faa86     	vldr	s20, [pc, #536]         @ 0x689c <led_ScanLengthControl+0x254>  // f32=0
    6680: eddf8a86     	vldr	s17, [pc, #536]         @ 0x68a0 <led_ScanLengthControl+0x258>  // f32=4095
    6684: eeb89a67     	vcvt.f32.u32	s18, s15
    6688: ebfff456     	bl	0x37e8 <.plt+0xec>      @ imm = #-0x2ea8  // CALL readFromSharedMem
    668c: eddf0a84     	vldr	s1, [pc, #528]          @ 0x68a4 <led_ScanLengthControl+0x25c>  // f32=-32
    6690: e30009fa     	movw	r0, #0x9fa
    6694: e19430b0     	ldrh	r3, [r4, r0]
    6698: e1a00004     	mov	r0, r4
    669c: e2631eff     	rsb	r1, r3, #4080
    66a0: e281200f     	add	r2, r1, #15
    66a4: ee012a10     	vmov	s2, r2
    66a8: eeb88ac1     	vcvt.f32.s32	s16, s2
    66ac: ee009a20     	vmla.f32	s18, s0, s1
    66b0: ed9f0a7c     	vldr	s0, [pc, #496]          @ 0x68a8 <led_ScanLengthControl+0x260>  // f32=209
    66b4: eeb49aca     	vcmpe.f32	s18, s20
    66b8: eef01a49     	vmov.f32	s3, s18
    66bc: eef1fa10     	vmrs	APSR_nzcv, fpscr
    66c0: bef01a4a     	vmovlt.f32	s3, s20
    66c4: eef41ae8     	vcmpe.f32	s3, s17
    66c8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    66cc: 8ef01a68     	vmovhi.f32	s3, s17
    66d0: ee71aaa9     	vadd.f32	s21, s3, s19
    66d4: ebfff443     	bl	0x37e8 <.plt+0xec>      @ imm = #-0x2ef4  // CALL readFromSharedMem
    66d8: ed9f2a73     	vldr	s4, [pc, #460]          @ 0x68ac <led_ScanLengthControl+0x264>  // f32=32
    66dc: e5d4c039     	ldrb	r12, [r4, #0x39]
    66e0: eddf2a72     	vldr	s5, [pc, #456]          @ 0x68b0 <led_ScanLengthControl+0x268>  // f32=0.000244200259
    66e4: eddf1b63     	vldr	d17, [pc, #396]         @ 0x6878 <led_ScanLengthControl+0x230>  // f64=71
    66e8: ee008a02     	vmla.f32	s16, s0, s4
    66ec: eeb48aca     	vcmpe.f32	s16, s20
    66f0: eeb00a48     	vmov.f32	s0, s16
    66f4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    66f8: beb00a4a     	vmovlt.f32	s0, s20
    66fc: eeb40ae8     	vcmpe.f32	s0, s17
    6700: eef1fa10     	vmrs	APSR_nzcv, fpscr
    6704: 8eb00a68     	vmovhi.f32	s0, s17
    6708: ee203a22     	vmul.f32	s6, s0, s5
    670c: eef70ac3     	vcvt.f64.f32	d16, s6
    6710: ee204ba0     	vmul.f64	d4, d16, d16
    6714: eef4aaca     	vcmpe.f32	s21, s20
    6718: ee245b21     	vmul.f64	d5, d4, d17
    671c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    6720: bef0aa4a     	vmovlt.f32	s21, s20
    6724: eef4aae8     	vcmpe.f32	s21, s17
    6728: eef1fa10     	vmrs	APSR_nzcv, fpscr
    672c: 8ef03a68     	vmovhi.f32	s7, s17
    6730: 9ef03a6a     	vmovls.f32	s7, s21
    6734: e35c0000     	cmp	r12, #0
    6738: ee387ae3     	vsub.f32	s14, s17, s7
    673c: eef79bc5     	vcvt.f32.f64	s19, d5
    6740: 0a00002c     	beq	0x67f8 <led_ScanLengthControl+0x1b0> @ imm = #0xb0
    6744: ed9f1b4d     	vldr	d1, [pc, #308]          @ 0x6880 <led_ScanLengthControl+0x238>  // f64=4095
    6748: eeb1aa08     	vmov.f32	s20, #6.000000e+00
    674c: ee678a0a     	vmul.f32	s17, s14, s20
    6750: eeb70ae8     	vcvt.f64.f32	d0, s17
    6754: ebfff4bf     	bl	0x3a58 <.plt+0x35c>     @ imm = #-0x2d04  // CALL __fmod_finite
    6758: eeb77bc0     	vcvt.f32.f64	s14, d0
    675c: eddf2b49     	vldr	d18, [pc, #292]         @ 0x6888 <led_ScanLengthControl+0x240>  // f64=0.0172
    6760: e3a00001     	mov	r0, #1
    6764: e5d43030     	ldrb	r3, [r4, #0x30]
    6768: e3a01443     	mov	r1, #1124073472
    676c: e3a02000     	mov	r2, #0
    6770: e58d0000     	str	r0, [sp]
    6774: ed9f9a4e     	vldr	s18, [pc, #312]         @ 0x68b4 <led_ScanLengthControl+0x26c>  // f32=36
    6778: e3442303     	movt	r2, #0x4303
    677c: e58d0008     	str	r0, [sp, #0x8]
    6780: e58d0010     	str	r0, [sp, #0x10]
    6784: eef73ac7     	vcvt.f64.f32	d19, s14
    6788: e58d100c     	str	r1, [sp, #0xc]
    678c: eddf4b3f     	vldr	d20, [pc, #252]         @ 0x6890 <led_ScanLengthControl+0x248>  // f64=0.33339999999999997
    6790: e58d0018     	str	r0, [sp, #0x18]
    6794: e58d201c     	str	r2, [sp, #0x1c]
    6798: ee230ba2     	vmul.f64	d0, d19, d18
    679c: eebd1ae9     	vcvt.s32.f32	s2, s19
    67a0: eef70bc0     	vcvt.f32.f64	s1, d0
    67a4: eef40ac9     	vcmpe.f32	s1, s18
    67a8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    67ac: 5e309ac9     	vsubpl.f32	s18, s1, s18
    67b0: 5ddf2a40     	vldrpl	s5, [pc, #256]          @ 0x68b8 <led_ScanLengthControl+0x270>
    67b4: 5ddf0a40     	vldrpl	s1, [pc, #256]          @ 0x68bc <led_ScanLengthControl+0x274>
    67b8: 4d9f9a40     	vldrmi	s18, [pc, #256]         @ 0x68c0 <led_ScanLengthControl+0x278>
    67bc: eef85bc1     	vcvt.f64.s32	d21, s2
    67c0: 5e490a22     	vmlapl.f32	s1, s18, s5
    67c4: ee656ba4     	vmul.f64	d22, d21, d20
    67c8: 4e600a89     	vmulmi.f32	s1, s1, s18
    67cc: e3530062     	cmp	r3, #98
    67d0: eebd8ae0     	vcvt.s32.f32	s16, s1
    67d4: eefd1be6     	vcvt.s32.f64	s3, d22
    67d8: eef8aac8     	vcvt.f32.s32	s21, s16
    67dc: eeb82ae1     	vcvt.f32.s32	s4, s3
    67e0: edcdaa01     	vstr	s21, [sp, #4]
    67e4: ed8d2a05     	vstr	s4, [sp, #20]
    67e8: 9a000012     	bls	0x6838 <led_ScanLengthControl+0x1f0> @ imm = #0x48
    67ec: e28dd020     	add	sp, sp, #32
    67f0: ecbd8b06     	vpop	{d8, d9, d10}
    67f4: e8bd8010     	pop	{r4, pc}
    67f8: e5d4e054     	ldrb	lr, [r4, #0x54]
    67fc: e35e0000     	cmp	lr, #0
    6800: 0affffd5     	beq	0x675c <led_ScanLengthControl+0x114> @ imm = #-0xac
    6804: edd44a16     	vldr	s9, [r4, #88]
    6808: edd45a18     	vldr	s11, [r4, #96]
    680c: ed9f6a2c     	vldr	s12, [pc, #176]         @ 0x68c4 <led_ScanLengthControl+0x27c>  // f32=480000
    6810: ee746aa5     	vadd.f32	s13, s9, s11
    6814: eef46ac6     	vcmpe.f32	s13, s12
    6818: eef1fa10     	vmrs	APSR_nzcv, fpscr
    681c: ca000012     	bgt	0x686c <led_ScanLengthControl+0x224> @ imm = #0x48
    6820: eef56ac0     	vcmpe.f32	s13, #0
    6824: eef1fa10     	vmrs	APSR_nzcv, fpscr
    6828: 4e766a86     	vaddmi.f32	s13, s13, s12
    682c: eddf7a25     	vldr	s15, [pc, #148]         @ 0x68c8 <led_ScanLengthControl+0x280>  // f32=0.00853125006
    6830: ee267aa7     	vmul.f32	s14, s13, s15
    6834: eaffffc8     	b	0x675c <led_ScanLengthControl+0x114> @ imm = #-0xe0
    6838: e59fc08c     	ldr	r12, [pc, #0x8c]        @ 0x68cc <led_ScanLengthControl+0x284>  // u32=0xe274; f32?=8.12360746e-41
    683c: e2844a01     	add	r4, r4, #4096
    6840: e08f000c     	add	r0, pc, r12
    6844: e5944dac     	ldr	r4, [r4, #0xdac]
    6848: ebfff3b6     	bl	0x3728 <.plt+0x2c>      @ imm = #-0x3128  // CALL gensym
    684c: e1a0300d     	mov	r3, sp
    6850: e3a02004     	mov	r2, #4
    6854: e1a01000     	mov	r1, r0
    6858: e1a00004     	mov	r0, r4
    685c: ebfff504     	bl	0x3c74 <.plt+0x578>     @ imm = #-0x2bf0  // CALL outlet_list
    6860: e28dd020     	add	sp, sp, #32
    6864: ecbd8b06     	vpop	{d8, d9, d10}
    6868: e8bd8010     	pop	{r4, pc}
    686c: ee766ac6     	vsub.f32	s13, s13, s12
    6870: eaffffed     	b	0x682c <led_ScanLengthControl+0x1e4> @ imm = #-0x4c
    6874: e320f000     	nop
    6878: 00 00 00 00  	.word	0x00000000
    687c: 00 c0 51 40  	.word	0x4051c000
    6880: 00 00 00 00  	.word	0x00000000
    6884: 00 fe af 40  	.word	0x40affe00
    6888: 22 fd f6 75  	.word	0x75f6fd22
    688c: e0 9c 91 3f  	.word	0x3f919ce0
    6890: 2d 21 1f f4  	.word	0xf41f212d
    6894: 6c 56 d5 3f  	.word	0x3fd5566c
    6898: 00 00 50 43  	.word	0x43500000
    689c: 00 00 00 00  	.word	0x00000000
    68a0: 00 f0 7f 45  	.word	0x457ff000
    68a4: 00 00 00 c2  	.word	0xc2000000
    68a8: 00 00 51 43  	.word	0x43510000
    68ac: 00 00 00 42  	.word	0x42000000
    68b0: 01 08 80 39  	.word	0x39800801
    68b4: 00 00 10 42  	.word	0x42100000
    68b8: 8e e3 38 3f  	.word	0x3f38e38e
    68bc: 00 00 38 42  	.word	0x42380000
    68c0: 39 8e a3 3f  	.word	0x3fa38e39
    68c4: 00 60 ea 48  	.word	0x48ea6000
    68c8: a8 c6 0b 3c  	.word	0x3c0bc6a8
    68cc: 74 e2 00 00  	.word	0x0000e274

