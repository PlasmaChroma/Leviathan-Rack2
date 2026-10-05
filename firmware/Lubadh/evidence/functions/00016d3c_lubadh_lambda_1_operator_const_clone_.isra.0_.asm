; lubadh::{lambda()#1}::operator()() const [clone .isra.0]
; VA 0x16d3c size 2756

   16d3c: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   16d40: e3a01000     	mov	r1, #0
   16d44: e304229c     	movw	r2, #0x429c
   16d48: ed2d8b04     	vpush	{d8, d9}
   16d4c: e24dd034     	sub	sp, sp, #52
   16d50: e1a05000     	mov	r5, r0
   16d54: ebfffc06     	bl	0x15d74    @ imm = #-0xfe8 ; memset
   16d58: eddf0bf4     	vldr	d16, [pc, #976]         @ 0x17130 ; float 5.30498947741e-313
   16d5c: eddf1bf5     	vldr	d17, [pc, #980]         @ 0x17138 ; float 8.48798316386e-314
   16d60: e1a03005     	mov	r3, r5
   16d64: eddf2bf5     	vldr	d18, [pc, #980]         @ 0x17140 ; float 3.16202013338e-322
   16d68: eddf3bf6     	vldr	d19, [pc, #984]         @ 0x17148 ; float 4.24399158193e-314
   16d6c: e2850020     	add	r0, r5, #32
   16d70: f2c04050     	vmov.i32	q10, #0x0
   16d74: e2856044     	add	r6, r5, #68
   16d78: e3a01000     	mov	r1, #0
   16d7c: e3a02c05     	mov	r2, #1280
   16d80: f4402a8f     	vst1.32	{d18, d19}, [r0]
   16d84: e1a00006     	mov	r0, r6
   16d88: e1a04001     	mov	r4, r1
   16d8c: f4434a8d     	vst1.32	{d20, d21}, [r3]!
   16d90: e30ccccd     	movw	r12, #0xcccd
   16d94: e343cdcc     	movt	r12, #0x3dcc
   16d98: f4430a8f     	vst1.32	{d16, d17}, [r3]
   16d9c: e1a08006     	mov	r8, r6
   16da0: e1a09004     	mov	r9, r4
   16da4: e5852030     	str	r2, [r5, #0x30]
   16da8: e3a02e1e     	mov	r2, #480
   16dac: e585c034     	str	r12, [r5, #0x34]
   16db0: ebfffbef     	bl	0x15d74    @ imm = #-0x1044 ; memset
   16db4: eddf2be5     	vldr	d18, [pc, #916]         @ 0x17150 ; float -2.00000143424
   16db8: eddf3be6     	vldr	d19, [pc, #920]         @ 0x17158 ; float -3.0517599896e-05
   16dbc: e2853058     	add	r3, r5, #88
   16dc0: eddf0be6     	vldr	d16, [pc, #920]         @ 0x17160 ; float 0.00781250183354
   16dc4: eddf1be7     	vldr	d17, [pc, #924]         @ 0x17168 ; float 512.00012207
   16dc8: e1a07004     	mov	r7, r4
   16dcc: f4462a8f     	vst1.32	{d18, d19}, [r6]
   16dd0: f4430a8f     	vst1.32	{d16, d17}, [r3]
   16dd4: ea00000e     	b	0x16e14
   16dd8: e1a0b009     	mov	r11, r9
   16ddc: ed988a00     	vldr	s16, [r8]
   16de0: e1a0a007     	mov	r10, r7
   16de4: ecab8a01     	vstmia	r11!, {s16}
   16de8: eeb18a48     	vneg.f32	s16, s16
   16dec: e154000b     	cmp	r4, r11
   16df0: 0a000025     	beq	0x16e8c
   16df4: e1a0900b     	mov	r9, r11
   16df8: e1a0700a     	mov	r7, r10
   16dfc: eca98a01     	vstmia	r9!, {s16}
   16e00: e2853f89     	add	r3, r5, #548
   16e04: e2888004     	add	r8, r8, #4
   16e08: e1580003     	cmp	r8, r3
   16e0c: e58d300c     	str	r3, [sp, #0xc]
   16e10: 0a000058     	beq	0x16f78
   16e14: e1540009     	cmp	r4, r9
   16e18: 1affffee     	bne	0x16dd8
   16e1c: e0449007     	sub	r9, r4, r7
   16e20: e1a04149     	asr	r4, r9, #2
   16e24: e374021e     	cmn	r4, #-536870911
   16e28: 0a000251     	beq	0x17774
   16e2c: e3540000     	cmp	r4, #0
   16e30: 0a00003d     	beq	0x16f2c
   16e34: e1540084     	cmp	r4, r4, lsl #1
   16e38: e1a04084     	lsl	r4, r4, #1
   16e3c: 83e0410e     	mvnhi	r4, #-2147483645
   16e40: 9a000031     	bls	0x16f0c
   16e44: e1a00004     	mov	r0, r4
   16e48: ebfffaaf     	bl	0x1590c     @ imm = #-0x1544 ; _Znwj
   16e4c: e1a0a000     	mov	r10, r0
   16e50: e0804004     	add	r4, r0, r4
   16e54: ed988a00     	vldr	s16, [r8]
   16e58: e08a3009     	add	r3, r10, r9
   16e5c: e289b004     	add	r11, r9, #4
   16e60: e3590000     	cmp	r9, #0
   16e64: e08ab00b     	add	r11, r10, r11
   16e68: ed838a00     	vstr	s16, [r3]
   16e6c: ca00001f     	bgt	0x16ef0
   16e70: e3570000     	cmp	r7, #0
   16e74: 0affffdb     	beq	0x16de8
   16e78: e1a00007     	mov	r0, r7
   16e7c: ebfffbef     	bl	0x15e40    @ imm = #-0x1044 ; _ZdlPv
   16e80: eeb18a48     	vneg.f32	s16, s16
   16e84: e154000b     	cmp	r4, r11
   16e88: 1affffd9     	bne	0x16df4
   16e8c: e044b00a     	sub	r11, r4, r10
   16e90: e1a0414b     	asr	r4, r11, #2
   16e94: e374021e     	cmn	r4, #-536870911
   16e98: 0a000238     	beq	0x17780
   16e9c: e3540000     	cmp	r4, #0
   16ea0: 0a000032     	beq	0x16f70
   16ea4: e1540084     	cmp	r4, r4, lsl #1
   16ea8: e1a04084     	lsl	r4, r4, #1
   16eac: 83e0410e     	mvnhi	r4, #-2147483645
   16eb0: 9a000026     	bls	0x16f50
   16eb4: e1a00004     	mov	r0, r4
   16eb8: ebfffa93     	bl	0x1590c     @ imm = #-0x15b4 ; _Znwj
   16ebc: e1a07000     	mov	r7, r0
   16ec0: e0804004     	add	r4, r0, r4
   16ec4: e087300b     	add	r3, r7, r11
   16ec8: e28b9004     	add	r9, r11, #4
   16ecc: e35b0000     	cmp	r11, #0
   16ed0: e0879009     	add	r9, r7, r9
   16ed4: ed838a00     	vstr	s16, [r3]
   16ed8: ca000015     	bgt	0x16f34
   16edc: e35a0000     	cmp	r10, #0
   16ee0: 0affffc6     	beq	0x16e00
   16ee4: e1a0000a     	mov	r0, r10
   16ee8: ebfffbd4     	bl	0x15e40    @ imm = #-0x10b0 ; _ZdlPv
   16eec: eaffffc3     	b	0x16e00
   16ef0: e1a02009     	mov	r2, r9
   16ef4: e1a01007     	mov	r1, r7
   16ef8: e1a0000a     	mov	r0, r10
   16efc: ebfffab8     	bl	0x159e4    @ imm = #-0x1520 ; memmove
   16f00: e1a00007     	mov	r0, r7
   16f04: ebfffbcd     	bl	0x15e40    @ imm = #-0x10cc ; _ZdlPv
   16f08: eaffffdc     	b	0x16e80
   16f0c: e3540000     	cmp	r4, #0
   16f10: 01a0a004     	moveq	r10, r4
   16f14: 0affffce     	beq	0x16e54
   16f18: e3e0320e     	mvn	r3, #-536870912
   16f1c: e1540003     	cmp	r4, r3
   16f20: 21a04003     	movhs	r4, r3
   16f24: e1a04104     	lsl	r4, r4, #2
   16f28: eaffffc5     	b	0x16e44
   16f2c: e3a04004     	mov	r4, #4
   16f30: eaffffc3     	b	0x16e44
   16f34: e1a0200b     	mov	r2, r11
   16f38: e1a0100a     	mov	r1, r10
   16f3c: e1a00007     	mov	r0, r7
   16f40: ebfffaa7     	bl	0x159e4    @ imm = #-0x1564 ; memmove
   16f44: e1a0000a     	mov	r0, r10
   16f48: ebfffbbc     	bl	0x15e40    @ imm = #-0x1110 ; _ZdlPv
   16f4c: eaffffab     	b	0x16e00
   16f50: e3540000     	cmp	r4, #0
   16f54: 01a07004     	moveq	r7, r4
   16f58: 0affffd9     	beq	0x16ec4
   16f5c: e3e0320e     	mvn	r3, #-536870912
   16f60: e1540003     	cmp	r4, r3
   16f64: 21a04003     	movhs	r4, r3
   16f68: e1a04104     	lsl	r4, r4, #2
   16f6c: eaffffd0     	b	0x16eb4
   16f70: e3a04004     	mov	r4, #4
   16f74: eaffffce     	b	0x16eb4
   16f78: e1590007     	cmp	r9, r7
   16f7c: 0a000039     	beq	0x17068
   16f80: e0494007     	sub	r4, r9, r7
   16f84: e1a01009     	mov	r1, r9
   16f88: e1a00007     	mov	r0, r7
   16f8c: e3a03000     	mov	r3, #0
   16f90: e1a02144     	asr	r2, r4, #2
   16f94: e2878004     	add	r8, r7, #4
   16f98: e16f2f12     	clz	r2, r2
   16f9c: e262201f     	rsb	r2, r2, #31
   16fa0: e1a02082     	lsl	r2, r2, #1
   16fa4: eb005f6e     	bl	0x2ed64
   16fa8: e3540040     	cmp	r4, #64
   16fac: ca0000e9     	bgt	0x17358
   16fb0: e1580009     	cmp	r8, r9
   16fb4: 0a00012b     	beq	0x17468
   16fb8: e1a04008     	mov	r4, r8
   16fbc: ea000009     	b	0x16fe8
   16fc0: e1540007     	cmp	r4, r7
   16fc4: 0a000003     	beq	0x16fd8
   16fc8: e0442007     	sub	r2, r4, r7
   16fcc: e1a01007     	mov	r1, r7
   16fd0: e2870004     	add	r0, r7, #4
   16fd4: ebfffa82     	bl	0x159e4    @ imm = #-0x15f8 ; memmove
   16fd8: ed878a00     	vstr	s16, [r7]
   16fdc: e2844004     	add	r4, r4, #4
   16fe0: e1540009     	cmp	r4, r9
   16fe4: 0a00011f     	beq	0x17468
   16fe8: ed948a00     	vldr	s16, [r4]
   16fec: edd77a00     	vldr	s15, [r7]
   16ff0: eeb48ae7     	vcmpe.f32	s16, s15
   16ff4: eef1fa10     	vmrs	APSR_nzcv, fpscr
   16ff8: 4afffff0     	bmi	0x16fc0
   16ffc: ed547a01     	vldr	s15, [r4, #-4]
   17000: e2443004     	sub	r3, r4, #4
   17004: eeb48ae7     	vcmpe.f32	s16, s15
   17008: eef1fa10     	vmrs	APSR_nzcv, fpscr
   1700c: 5a0001d1     	bpl	0x17758
   17010: e1a02003     	mov	r2, r3
   17014: edc37a01     	vstr	s15, [r3, #4]
   17018: ed737a01     	vldmdb	r3!, {s15}
   1701c: eeb48ae7     	vcmpe.f32	s16, s15
   17020: eef1fa10     	vmrs	APSR_nzcv, fpscr
   17024: 4afffff9     	bmi	0x17010
   17028: ed828a00     	vstr	s16, [r2]
   1702c: eaffffea     	b	0x16fdc
   17030: e1530009     	cmp	r3, r9
   17034: 0a00000b     	beq	0x17068
   17038: e2832008     	add	r2, r3, #8
   1703c: e1590002     	cmp	r9, r2
   17040: 0a000007     	beq	0x17064
   17044: ecf27a01     	vldmia	r2!, {s15}
   17048: ed937a00     	vldr	s14, [r3]
   1704c: eeb47a67     	vcmp.f32	s14, s15
   17050: eef1fa10     	vmrs	APSR_nzcv, fpscr
   17054: 1dc37a01     	vstrne	s15, [r3, #4]
   17058: 12833004     	addne	r3, r3, #4
   1705c: e1590002     	cmp	r9, r2
   17060: 1afffff7     	bne	0x17044
   17064: e2839004     	add	r9, r3, #4
   17068: e3a02e1e     	mov	r2, #480
   1706c: e3a01000     	mov	r1, #0
   17070: e1a00006     	mov	r0, r6
   17074: ebfffb3e     	bl	0x15d74    @ imm = #-0x1308 ; memset
   17078: e1590007     	cmp	r9, r7
   1707c: 0a000003     	beq	0x17090
   17080: e0492007     	sub	r2, r9, r7
   17084: e1a01007     	mov	r1, r7
   17088: e1a00006     	mov	r0, r6
   1708c: ebfffbec     	bl	0x16044    @ imm = #-0x1050 ; memcpy
   17090: e3570000     	cmp	r7, #0
   17094: 0a000001     	beq	0x170a0
   17098: e1a00007     	mov	r0, r7
   1709c: ebfffb67     	bl	0x15e40    @ imm = #-0x1264 ; _ZdlPv
   170a0: f2c00010     	vmov.i32	d16, #0x0
   170a4: e3a00e7a     	mov	r0, #1952
   170a8: e3a03000     	mov	r3, #0
   170ac: e58d3018     	str	r3, [sp, #0x18]
   170b0: edcd0b04     	vstr	d16, [sp, #16]
   170b4: ebfffa14     	bl	0x1590c     @ imm = #-0x17b0 ; _Znwj
   170b8: e59d7010     	ldr	r7, [sp, #0x10]
   170bc: e1a04000     	mov	r4, r0
   170c0: e59d2014     	ldr	r2, [sp, #0x14]
   170c4: e0422007     	sub	r2, r2, r7
   170c8: e3520000     	cmp	r2, #0
   170cc: ca0001a3     	bgt	0x17760
   170d0: e3570000     	cmp	r7, #0
   170d4: 1a0001a3     	bne	0x17768
   170d8: f2c00010     	vmov.i32	d16, #0x0
   170dc: ee814b90     	vdup.32	d17, r4
   170e0: e28d2024     	add	r2, sp, #36
   170e4: e2840e7a     	add	r0, r4, #1952
   170e8: e28d1020     	add	r1, sp, #32
   170ec: e58d0018     	str	r0, [sp, #0x18]
   170f0: e28d0010     	add	r0, sp, #16
   170f4: e3a03000     	mov	r3, #0
   170f8: e34c3080     	movt	r3, #0xc080
   170fc: f442078f     	vst1.32	{d16}, [r2]
   17100: edcd1b04     	vstr	d17, [sp, #16]
   17104: e58d3020     	str	r3, [sp, #0x20]
   17108: e3a03000     	mov	r3, #0
   1710c: e58d302c     	str	r3, [sp, #0x2c]
   17110: ebfffe13     	bl	0x16964
   17114: e59d8014     	ldr	r8, [sp, #0x14]
   17118: eef18a00     	vmov.f32	s17, #4.000000e+00
   1711c: e59d4018     	ldr	r4, [sp, #0x18]
   17120: ed9f9a12     	vldr	s18, [pc, #72]          @ 0x17170 ; float 4095
   17124: eddf9a12     	vldr	s19, [pc, #72]          @ 0x17174 ; float 0
   17128: e58d6004     	str	r6, [sp, #0x4]
   1712c: ea00001d     	b	0x171a8
   17130: 00 00 00 00  	.word	0x00000000
   17134: 19 00 00 00  	.word	0x00000019
   17138: 00 00 00 00  	.word	0x00000000
   1713c: 04 00 00 00  	.word	0x00000004
   17140: 40 00 00 00  	.word	0x00000040
   17144: 00 00 00 00  	.word	0x00000000
   17148: 00 00 00 00  	.word	0x00000000
   1714c: 02 00 00 00  	.word	0x00000002
   17150: 00 00 80 c0  	.word	0xc0800000
   17154: 00 00 00 c0  	.word	0xc0000000
   17158: 00 00 80 bf  	.word	0xbf800000
   1715c: 00 00 00 bf  	.word	0xbf000000
   17160: 00 00 00 3f  	.word	0x3f000000
   17164: 00 00 80 3f  	.word	0x3f800000
   17168: 00 00 00 40  	.word	0x40000000
   1716c: 00 00 80 40  	.word	0x40800000
   17170: 00 f0 7f 45  	.word	0x457ff000
   17174: 00 00 00 00  	.word	0x00000000
   17178: f0 1f 07 00  	.word	0x00071ff0
   1717c: 00 20 07 00  	.word	0x00072000
   17180: e59d300c     	ldr	r3, [sp, #0xc]
   17184: e2888010     	add	r8, r8, #16
   17188: e59d2004     	ldr	r2, [sp, #0x4]
   1718c: ed088a04     	vstr	s16, [r8, #-16]
   17190: e508600c     	str	r6, [r8, #-0xc]
   17194: e1530002     	cmp	r3, r2
   17198: ed487a02     	vstr	s15, [r8, #-8]
   1719c: e5087004     	str	r7, [r8, #-0x4]
   171a0: e58d8014     	str	r8, [sp, #0x14]
   171a4: 0a000037     	beq	0x17288
   171a8: e59d3004     	ldr	r3, [sp, #0x4]
   171ac: eeb47a00     	vmov.f32	s14, #1.250000e-01
   171b0: e3002fff     	movw	r2, #0xfff
   171b4: ecb38a01     	vldmia	r3!, {s16}
   171b8: ee787a28     	vadd.f32	s15, s16, s17
   171bc: e58d3004     	str	r3, [sp, #0x4]
   171c0: ee677a87     	vmul.f32	s15, s15, s14
   171c4: eeb07a69     	vmov.f32	s14, s19
   171c8: eea77a89     	vfma.f32	s14, s15, s18
   171cc: eefd7ac7     	vcvt.s32.f32	s15, s14
   171d0: ee173a90     	vmov	r3, s15
   171d4: edcd7a02     	vstr	s15, [sp, #8]
   171d8: e2436032     	sub	r6, r3, #50
   171dc: e2837032     	add	r7, r3, #50
   171e0: e1560002     	cmp	r6, r2
   171e4: a1a06002     	movge	r6, r2
   171e8: e1570002     	cmp	r7, r2
   171ec: a1a07002     	movge	r7, r2
   171f0: e1580004     	cmp	r8, r4
   171f4: e1c66fc6     	bic	r6, r6, r6, asr #31
   171f8: e1c77fc7     	bic	r7, r7, r7, asr #31
   171fc: 1affffdf     	bne	0x17180
   17200: e59da010     	ldr	r10, [sp, #0x10]
   17204: e048900a     	sub	r9, r8, r10
   17208: e1a04249     	asr	r4, r9, #4
   1720c: e374037e     	cmn	r4, #-134217727
   17210: 0a00015d     	beq	0x1778c
   17214: e3540000     	cmp	r4, #0
   17218: 0a00007c     	beq	0x17410
   1721c: e1540084     	cmp	r4, r4, lsl #1
   17220: e1a04084     	lsl	r4, r4, #1
   17224: 83e0413e     	mvnhi	r4, #-2147483633
   17228: 9a000070     	bls	0x173f0
   1722c: e1a00004     	mov	r0, r4
   17230: ebfff9b5     	bl	0x1590c     @ imm = #-0x192c ; _Znwj
   17234: e1a0b000     	mov	r11, r0
   17238: e0804004     	add	r4, r0, r4
   1723c: e08b2009     	add	r2, r11, r9
   17240: e2893010     	add	r3, r9, #16
   17244: e08b8003     	add	r8, r11, r3
   17248: e59d3008     	ldr	r3, [sp, #0x8]
   1724c: e3590000     	cmp	r9, #0
   17250: e5826004     	str	r6, [r2, #0x4]
   17254: e5823008     	str	r3, [r2, #0x8]
   17258: e582700c     	str	r7, [r2, #0xc]
   1725c: ed828a00     	vstr	s16, [r2]
   17260: ca00005b     	bgt	0x173d4
   17264: e35a0000     	cmp	r10, #0
   17268: 1a00005d     	bne	0x173e4
   1726c: e59d300c     	ldr	r3, [sp, #0xc]
   17270: e59d2004     	ldr	r2, [sp, #0x4]
   17274: e58db010     	str	r11, [sp, #0x10]
   17278: e1530002     	cmp	r3, r2
   1727c: e58d8014     	str	r8, [sp, #0x14]
   17280: e58d4018     	str	r4, [sp, #0x18]
   17284: 1affffc7     	bne	0x171a8
   17288: e51f3118     	ldr	r3, [pc, #-0x118]       @ 0x17178
   1728c: e28dc020     	add	r12, sp, #32
   17290: e893000f     	ldm	r3, {r0, r1, r2, r3}
   17294: e88c000f     	stm	r12, {r0, r1, r2, r3}
   17298: e28d0010     	add	r0, sp, #16
   1729c: e1a0100c     	mov	r1, r12
   172a0: ebfffdaf     	bl	0x16964
   172a4: e59d4010     	ldr	r4, [sp, #0x10]
   172a8: e59d6014     	ldr	r6, [sp, #0x14]
   172ac: e1540006     	cmp	r4, r6
   172b0: 0a000079     	beq	0x1749c
   172b4: e0467004     	sub	r7, r6, r4
   172b8: e1a01006     	mov	r1, r6
   172bc: e1a00004     	mov	r0, r4
   172c0: e3a03000     	mov	r3, #0
   172c4: e1a02247     	asr	r2, r7, #4
   172c8: e16f2f12     	clz	r2, r2
   172cc: e262201f     	rsb	r2, r2, #31
   172d0: e1a02082     	lsl	r2, r2, #1
   172d4: ebfffe12     	bl	0x16b24
   172d8: e3570c01     	cmp	r7, #256
   172dc: da0000e1     	ble	0x17668
   172e0: e2847c01     	add	r7, r4, #256
   172e4: e1a00004     	mov	r0, r4
   172e8: e1a01007     	mov	r1, r7
   172ec: ebfffdd8     	bl	0x16a54
   172f0: e1a0e007     	mov	lr, r7
   172f4: e156000e     	cmp	r6, lr
   172f8: e1a0700e     	mov	r7, lr
   172fc: 0a0000dc     	beq	0x17674
   17300: e28dc020     	add	r12, sp, #32
   17304: e89e000f     	ldm	lr, {r0, r1, r2, r3}
   17308: e88c000f     	stm	r12, {r0, r1, r2, r3}
   1730c: e59e8008     	ldr	r8, [lr, #0x8]
   17310: e51e3008     	ldr	r3, [lr, #-0x8]
   17314: e1580003     	cmp	r8, r3
   17318: aa000008     	bge	0x17340
   1731c: e24ec010     	sub	r12, lr, #16
   17320: e28c4010     	add	r4, r12, #16
   17324: e1a0700c     	mov	r7, r12
   17328: e89c000f     	ldm	r12, {r0, r1, r2, r3}
   1732c: e24cc010     	sub	r12, r12, #16
   17330: e884000f     	stm	r4, {r0, r1, r2, r3}
   17334: e59c3008     	ldr	r3, [r12, #0x8]
   17338: e1530008     	cmp	r3, r8
   1733c: cafffff7     	bgt	0x17320
   17340: e28d3020     	add	r3, sp, #32
   17344: e58d8028     	str	r8, [sp, #0x28]
   17348: e28ee010     	add	lr, lr, #16
   1734c: e893000f     	ldm	r3, {r0, r1, r2, r3}
   17350: e887000f     	stm	r7, {r0, r1, r2, r3}
   17354: eaffffe6     	b	0x172f4
   17358: e287a040     	add	r10, r7, #64
   1735c: e1a04008     	mov	r4, r8
   17360: ea000009     	b	0x1738c
   17364: e1540007     	cmp	r4, r7
   17368: 0a000003     	beq	0x1737c
   1736c: e0442007     	sub	r2, r4, r7
   17370: e1a01007     	mov	r1, r7
   17374: e2870004     	add	r0, r7, #4
   17378: ebfff999     	bl	0x159e4    @ imm = #-0x199c ; memmove
   1737c: ed878a00     	vstr	s16, [r7]
   17380: e2844004     	add	r4, r4, #4
   17384: e15a0004     	cmp	r10, r4
   17388: 0a000022     	beq	0x17418
   1738c: ed948a00     	vldr	s16, [r4]
   17390: edd77a00     	vldr	s15, [r7]
   17394: eeb48ae7     	vcmpe.f32	s16, s15
   17398: eef1fa10     	vmrs	APSR_nzcv, fpscr
   1739c: 4afffff0     	bmi	0x17364
   173a0: ed547a01     	vldr	s15, [r4, #-4]
   173a4: e2443004     	sub	r3, r4, #4
   173a8: eeb48ae7     	vcmpe.f32	s16, s15
   173ac: eef1fa10     	vmrs	APSR_nzcv, fpscr
   173b0: 5a0000e6     	bpl	0x17750
   173b4: e1a02003     	mov	r2, r3
   173b8: edc37a01     	vstr	s15, [r3, #4]
   173bc: ed737a01     	vldmdb	r3!, {s15}
   173c0: eeb48ae7     	vcmpe.f32	s16, s15
   173c4: eef1fa10     	vmrs	APSR_nzcv, fpscr
   173c8: 4afffff9     	bmi	0x173b4
   173cc: ed828a00     	vstr	s16, [r2]
   173d0: eaffffea     	b	0x17380
   173d4: e1a02009     	mov	r2, r9
   173d8: e1a0100a     	mov	r1, r10
   173dc: e1a0000b     	mov	r0, r11
   173e0: ebfff97f     	bl	0x159e4    @ imm = #-0x1a04 ; memmove
   173e4: e1a0000a     	mov	r0, r10
   173e8: ebfffa94     	bl	0x15e40    @ imm = #-0x15b0 ; _ZdlPv
   173ec: eaffff9e     	b	0x1726c
   173f0: e3540000     	cmp	r4, #0
   173f4: 01a0b004     	moveq	r11, r4
   173f8: 0affff8f     	beq	0x1723c
   173fc: e3e0333e     	mvn	r3, #-134217728
   17400: e1540003     	cmp	r4, r3
   17404: 21a04003     	movhs	r4, r3
   17408: e1a04204     	lsl	r4, r4, #4
   1740c: eaffff86     	b	0x1722c
   17410: e3a04010     	mov	r4, #16
   17414: eaffff84     	b	0x1722c
   17418: e15a0009     	cmp	r10, r9
   1741c: 0a000011     	beq	0x17468
   17420: e1a0300a     	mov	r3, r10
   17424: e287103c     	add	r1, r7, #60
   17428: e1a00003     	mov	r0, r3
   1742c: e1a02001     	mov	r2, r1
   17430: ecb37a01     	vldmia	r3!, {s14}
   17434: ecf17a01     	vldmia	r1!, {s15}
   17438: eeb47ae7     	vcmpe.f32	s14, s15
   1743c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   17440: 5a000005     	bpl	0x1745c
   17444: e1a00002     	mov	r0, r2
   17448: edc27a01     	vstr	s15, [r2, #4]
   1744c: ed727a01     	vldmdb	r2!, {s15}
   17450: eeb47ae7     	vcmpe.f32	s14, s15
   17454: eef1fa10     	vmrs	APSR_nzcv, fpscr
   17458: 4afffff9     	bmi	0x17444
   1745c: e1590003     	cmp	r9, r3
   17460: ed807a00     	vstr	s14, [r0]
   17464: 1affffef     	bne	0x17428
   17468: e1a02008     	mov	r2, r8
   1746c: e1a01007     	mov	r1, r7
   17470: ea000005     	b	0x1748c
   17474: edd27a00     	vldr	s15, [r2]
   17478: e2822004     	add	r2, r2, #4
   1747c: ecb17a01     	vldmia	r1!, {s14}
   17480: eeb47a67     	vcmp.f32	s14, s15
   17484: eef1fa10     	vmrs	APSR_nzcv, fpscr
   17488: 0afffee8     	beq	0x17030
   1748c: e1a03001     	mov	r3, r1
   17490: e1520009     	cmp	r2, r9
   17494: 1afffff6     	bne	0x17474
   17498: eafffef2     	b	0x17068
   1749c: e0466004     	sub	r6, r6, r4
   174a0: e1a03246     	asr	r3, r6, #4
   174a4: e3530001     	cmp	r3, #1
   174a8: 0a00004c     	beq	0x175e0
   174ac: e2446010     	sub	r6, r4, #16
   174b0: e1a01004     	mov	r1, r4
   174b4: e0866203     	add	r6, r6, r3, lsl #4
   174b8: e1a03004     	mov	r3, r4
   174bc: e593000c     	ldr	r0, [r3, #0xc]
   174c0: e5932014     	ldr	r2, [r3, #0x14]
   174c4: e1500002     	cmp	r0, r2
   174c8: da000008     	ble	0x174f0
   174cc: e593c008     	ldr	r12, [r3, #0x8]
   174d0: e5930018     	ldr	r0, [r3, #0x18]
   174d4: e040200c     	sub	r2, r0, r12
   174d8: e0822fa2     	add	r2, r2, r2, lsr #31
   174dc: e1a020c2     	asr	r2, r2, #1
   174e0: e082c00c     	add	r12, r2, r12
   174e4: e0402002     	sub	r2, r0, r2
   174e8: e583c00c     	str	r12, [r3, #0xc]
   174ec: e5832014     	str	r2, [r3, #0x14]
   174f0: e2833010     	add	r3, r3, #16
   174f4: e1530006     	cmp	r3, r6
   174f8: 1affffef     	bne	0x174bc
   174fc: e5913004     	ldr	r3, [r1, #0x4]
   17500: e591200c     	ldr	r2, [r1, #0xc]
   17504: e1530002     	cmp	r3, r2
   17508: aa00001c     	bge	0x17580
   1750c: e042e003     	sub	lr, r2, r3
   17510: e591c000     	ldr	r12, [r1]
   17514: e24e0001     	sub	r0, lr, #1
   17518: e3500002     	cmp	r0, #2
   1751c: 9a00000c     	bls	0x17554
   17520: e2830089     	add	r0, r3, #137
   17524: eea0cb90     	vdup.32	q8, r12
   17528: e1a0812e     	lsr	r8, lr, #2
   1752c: e3a07000     	mov	r7, #0
   17530: e0850100     	add	r0, r5, r0, lsl #2
   17534: e2877001     	add	r7, r7, #1
   17538: f4400a8d     	vst1.32	{d16, d17}, [r0]!
   1753c: e1570008     	cmp	r7, r8
   17540: 1afffffb     	bne	0x17534
   17544: e3ce0003     	bic	r0, lr, #3
   17548: e0833000     	add	r3, r3, r0
   1754c: e15e0000     	cmp	lr, r0
   17550: 0a00000a     	beq	0x17580
   17554: e085e103     	add	lr, r5, r3, lsl #2
   17558: e2830001     	add	r0, r3, #1
   1755c: e1520000     	cmp	r2, r0
   17560: e58ec224     	str	r12, [lr, #0x224]
   17564: da000005     	ble	0x17580
   17568: e0850100     	add	r0, r5, r0, lsl #2
   1756c: e2833002     	add	r3, r3, #2
   17570: e1520003     	cmp	r2, r3
   17574: e580c224     	str	r12, [r0, #0x224]
   17578: c0853103     	addgt	r3, r5, r3, lsl #2
   1757c: c583c224     	strgt	r12, [r3, #0x224]
   17580: e5913014     	ldr	r3, [r1, #0x14]
   17584: e1530002     	cmp	r3, r2
   17588: da000011     	ble	0x175d4
   1758c: ed917a00     	vldr	s14, [r1]
   17590: e0433002     	sub	r3, r3, r2
   17594: edd17a04     	vldr	s15, [r1, #16]
   17598: ee063a90     	vmov	s13, r3
   1759c: e2822089     	add	r2, r2, #137
   175a0: e3a00000     	mov	r0, #0
   175a4: eef86ae6     	vcvt.f32.s32	s13, s13
   175a8: ee777ac7     	vsub.f32	s15, s15, s14
   175ac: e0852102     	add	r2, r5, r2, lsl #2
   175b0: ee060a10     	vmov	s12, r0
   175b4: e2800001     	add	r0, r0, #1
   175b8: e1500003     	cmp	r0, r3
   175bc: eef85ac6     	vcvt.f32.s32	s11, s12
   175c0: ee856aa6     	vdiv.f32	s12, s11, s13
   175c4: eef05a47     	vmov.f32	s11, s14
   175c8: eee65a27     	vfma.f32	s11, s12, s15
   175cc: ece25a01     	vstmia	r2!, {s11}
   175d0: 1afffff6     	bne	0x175b0
   175d4: e2811010     	add	r1, r1, #16
   175d8: e1510006     	cmp	r1, r6
   175dc: 1affffc6     	bne	0x174fc
   175e0: e3540000     	cmp	r4, #0
   175e4: 0a000001     	beq	0x175f0
   175e8: e1a00004     	mov	r0, r4
   175ec: ebfffa13     	bl	0x15e40    @ imm = #-0x17b4 ; _ZdlPv
   175f0: e51fe47c     	ldr	lr, [pc, #-0x47c]       @ 0x1717c
   175f4: e2854901     	add	r4, r5, #16384
   175f8: e284cf8a     	add	r12, r4, #552
   175fc: e2849f9b     	add	r9, r4, #620
   17600: eddf4b72     	vldr	d20, [pc, #456]         @ 0x177d0 ; float 3.05175853327e-05
   17604: eddf5b73     	vldr	d21, [pc, #460]         @ 0x177d8 ; float 4.65661395381e-10
   17608: e2848f9f     	add	r8, r4, #636
   1760c: e8be000f     	ldm	lr!, {r0, r1, r2, r3}
   17610: e8ac000f     	stm	r12!, {r0, r1, r2, r3}
   17614: e2847fa3     	add	r7, r4, #652
   17618: eddf2b70     	vldr	d18, [pc, #448]         @ 0x177e0 ; float 1.3411048233e-08
   1761c: eddf3b71     	vldr	d19, [pc, #452]         @ 0x177e8 ; float 1.34110464955e-08
   17620: e3a06003     	mov	r6, #3
   17624: eddf0b71     	vldr	d16, [pc, #452]         @ 0x177f0 ; float 3.43322837737e-06
   17628: eddf1b72     	vldr	d17, [pc, #456]         @ 0x177f8 ; float 0.000878906470662
   1762c: e5846224     	str	r6, [r4, #0x224]
   17630: e8be000f     	ldm	lr!, {r0, r1, r2, r3}
   17634: e8ac000f     	stm	r12!, {r0, r1, r2, r3}
   17638: e8be000f     	ldm	lr!, {r0, r1, r2, r3}
   1763c: e8ac000f     	stm	r12!, {r0, r1, r2, r3}
   17640: e89e000f     	ldm	lr, {r0, r1, r2, r3}
   17644: e88c000f     	stm	r12, {r0, r1, r2, r3}
   17648: e1a00005     	mov	r0, r5
   1764c: e5846268     	str	r6, [r4, #0x268]
   17650: f4494a8f     	vst1.32	{d20, d21}, [r9]
   17654: f4482a8f     	vst1.32	{d18, d19}, [r8]
   17658: f4470a8f     	vst1.32	{d16, d17}, [r7]
   1765c: e28dd034     	add	sp, sp, #52
   17660: ecbd8b04     	vpop	{d8, d9}
   17664: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
   17668: e1a01006     	mov	r1, r6
   1766c: e1a00004     	mov	r0, r4
   17670: ebfffcf7     	bl	0x16a54
   17674: e59d4010     	ldr	r4, [sp, #0x10]
   17678: e59d6014     	ldr	r6, [sp, #0x14]
   1767c: e1540006     	cmp	r4, r6
   17680: 0affff85     	beq	0x1749c
   17684: e2843010     	add	r3, r4, #16
   17688: e1560003     	cmp	r6, r3
   1768c: 0affff82     	beq	0x1749c
   17690: ed947a00     	vldr	s14, [r4]
   17694: e1a03004     	mov	r3, r4
   17698: edd37a04     	vldr	s15, [r3, #16]
   1769c: e1a07003     	mov	r7, r3
   176a0: eeb47a67     	vcmp.f32	s14, s15
   176a4: eef1fa10     	vmrs	APSR_nzcv, fpscr
   176a8: 1a000003     	bne	0x176bc
   176ac: e5931008     	ldr	r1, [r3, #0x8]
   176b0: e5932018     	ldr	r2, [r3, #0x18]
   176b4: e1510002     	cmp	r1, r2
   176b8: 0a000005     	beq	0x176d4
   176bc: e2832020     	add	r2, r3, #32
   176c0: e2833010     	add	r3, r3, #16
   176c4: e1560002     	cmp	r6, r2
   176c8: 0affff73     	beq	0x1749c
   176cc: eeb07a67     	vmov.f32	s14, s15
   176d0: eafffff0     	b	0x17698
   176d4: e1560003     	cmp	r6, r3
   176d8: 0affff6f     	beq	0x1749c
   176dc: e2833020     	add	r3, r3, #32
   176e0: e1560003     	cmp	r6, r3
   176e4: 0a000010     	beq	0x1772c
   176e8: e287c030     	add	r12, r7, #48
   176ec: ed977a00     	vldr	s14, [r7]
   176f0: ed5c7a04     	vldr	s15, [r12, #-16]
   176f4: eeb47a67     	vcmp.f32	s14, s15
   176f8: eef1fa10     	vmrs	APSR_nzcv, fpscr
   176fc: 1a000003     	bne	0x17710
   17700: e5972008     	ldr	r2, [r7, #0x8]
   17704: e51c3008     	ldr	r3, [r12, #-0x8]
   17708: e1520003     	cmp	r2, r3
   1770c: 0a000003     	beq	0x17720
   17710: e287e010     	add	lr, r7, #16
   17714: e91c000f     	ldmdb	r12, {r0, r1, r2, r3}
   17718: e1a0700e     	mov	r7, lr
   1771c: e88e000f     	stm	lr, {r0, r1, r2, r3}
   17720: e156000c     	cmp	r6, r12
   17724: e28cc010     	add	r12, r12, #16
   17728: 1affffef     	bne	0x176ec
   1772c: e2877010     	add	r7, r7, #16
   17730: e1560007     	cmp	r6, r7
   17734: 11a06007     	movne	r6, r7
   17738: 158d7014     	strne	r7, [sp, #0x14]
   1773c: e0466004     	sub	r6, r6, r4
   17740: e1a03246     	asr	r3, r6, #4
   17744: e3530001     	cmp	r3, #1
   17748: 1affff57     	bne	0x174ac
   1774c: eaffffa5     	b	0x175e8
   17750: e1a02004     	mov	r2, r4
   17754: eaffff1c     	b	0x173cc
   17758: e1a02004     	mov	r2, r4
   1775c: eafffe31     	b	0x17028
   17760: e1a01007     	mov	r1, r7
   17764: ebfff89e     	bl	0x159e4    @ imm = #-0x1d88 ; memmove
   17768: e1a00007     	mov	r0, r7
   1776c: ebfff9b3     	bl	0x15e40    @ imm = #-0x1934 ; _ZdlPv
   17770: eafffe58     	b	0x170d8
   17774: e3000c8c     	movw	r0, #0xc8c
   17778: e3400007     	movt	r0, #0x7
   1777c: ebfff90d     	bl	0x15bb8    @ imm = #-0x1bcc ; _ZSt20__throw_length_errorPKc
   17780: e3000c8c     	movw	r0, #0xc8c
   17784: e3400007     	movt	r0, #0x7
   17788: ebfff90a     	bl	0x15bb8    @ imm = #-0x1bd8 ; _ZSt20__throw_length_errorPKc
   1778c: e3000c8c     	movw	r0, #0xc8c
   17790: e3400007     	movt	r0, #0x7
   17794: ebfff907     	bl	0x15bb8    @ imm = #-0x1be4 ; _ZSt20__throw_length_errorPKc
   17798: e59d0010     	ldr	r0, [sp, #0x10]
   1779c: e3500000     	cmp	r0, #0
   177a0: 0a000000     	beq	0x177a8
   177a4: ebfff9a5     	bl	0x15e40    @ imm = #-0x196c ; _ZdlPv
   177a8: ebfff9ec     	bl	0x15f60    @ imm = #-0x1850 ; __cxa_end_cleanup
   177ac: e1a0a007     	mov	r10, r7
   177b0: e35a0000     	cmp	r10, #0
   177b4: 0afffffb     	beq	0x177a8
   177b8: e1a0000a     	mov	r0, r10
   177bc: ebfff99f     	bl	0x15e40    @ imm = #-0x1984 ; _ZdlPv
   177c0: eafffff8     	b	0x177a8
   177c4: eafffff8     	b	0x177ac
   177c8: eafffff8     	b	0x177b0
   177cc: e320f000     	nop
   177d0: 66 66 66 3f  	.word	0x3f666666
   177d4: 00 00 00 3f  	.word	0x3f000000
   177d8: cd cc 4c 3e  	.word	0x3e4ccccd
   177dc: 00 00 00 3e  	.word	0x3e000000
   177e0: 9a 99 99 3e  	.word	0x3e99999a
   177e4: cd cc 4c 3e  	.word	0x3e4ccccd
   177e8: 00 00 00 00  	.word	0x00000000
   177ec: cd cc 4c 3e  	.word	0x3e4ccccd
   177f0: 00 00 c8 42  	.word	0x42c80000
   177f4: cd cc cc 3e  	.word	0x3ecccccd
   177f8: 00 40 1c 46  	.word	0x461c4000
   177fc: cd cc 4c 3f  	.word	0x3f4ccccd
