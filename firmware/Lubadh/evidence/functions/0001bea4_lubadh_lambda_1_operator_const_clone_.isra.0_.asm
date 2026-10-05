; lubadh::{lambda()#1}::operator()() const [clone .isra.0]
; VA 0x1bea4 size 2864

   1bea4: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   1bea8: e3a01000     	mov	r1, #0
   1beac: e304229c     	movw	r2, #0x429c
   1beb0: ed2d8b04     	vpush	{d8, d9}
   1beb4: e24dd901     	sub	sp, sp, #16384
   1beb8: e24dd034     	sub	sp, sp, #52
   1bebc: e1a05000     	mov	r5, r0
   1bec0: ebffe7ab     	bl	0x15d74    @ imm = #-0x6154 ; memset
   1bec4: e1a03005     	mov	r3, r5
   1bec8: f2c04050     	vmov.i32	q10, #0x0
   1becc: e285e020     	add	lr, r5, #32
   1bed0: eddf2be0     	vldr	d18, [pc, #896]         @ 0x1c258 ; float 5.30498947741e-313
   1bed4: eddf3be1     	vldr	d19, [pc, #900]         @ 0x1c260 ; float 8.48798316386e-314
   1bed8: e3a09000     	mov	r9, #0
   1bedc: eddf0be1     	vldr	d16, [pc, #900]         @ 0x1c268 ; float 3.16202013338e-322
   1bee0: eddf1be2     	vldr	d17, [pc, #904]         @ 0x1c270 ; float 4.24399158193e-314
   1bee4: e2858044     	add	r8, r5, #68
   1bee8: f4434a8d     	vst1.32	{d20, d21}, [r3]!
   1beec: e1a01009     	mov	r1, r9
   1bef0: e3a02e1e     	mov	r2, #480
   1bef4: f4432a8f     	vst1.32	{d18, d19}, [r3]
   1bef8: e1a00008     	mov	r0, r8
   1befc: e3a03c05     	mov	r3, #1280
   1bf00: f44e0a8f     	vst1.32	{d16, d17}, [lr]
   1bf04: e30ccccd     	movw	r12, #0xcccd
   1bf08: e343cdcc     	movt	r12, #0x3dcc
   1bf0c: e5853030     	str	r3, [r5, #0x30]
   1bf10: e1a06009     	mov	r6, r9
   1bf14: e585c034     	str	r12, [r5, #0x34]
   1bf18: e1a07008     	mov	r7, r8
   1bf1c: e58d8004     	str	r8, [sp, #0x4]
   1bf20: ebffe793     	bl	0x15d74    @ imm = #-0x61b4 ; memset
   1bf24: eddf2bd3     	vldr	d18, [pc, #844]         @ 0x1c278 ; float -2.00000143424
   1bf28: eddf3bd4     	vldr	d19, [pc, #848]         @ 0x1c280 ; float -3.0517599896e-05
   1bf2c: e2853058     	add	r3, r5, #88
   1bf30: eddf0bd4     	vldr	d16, [pc, #848]         @ 0x1c288 ; float 0.00781250183354
   1bf34: eddf1bd5     	vldr	d17, [pc, #852]         @ 0x1c290 ; float 512.00012207
   1bf38: e1a01009     	mov	r1, r9
   1bf3c: e28d0030     	add	r0, sp, #48
   1bf40: e3a02901     	mov	r2, #16384
   1bf44: f4482a8f     	vst1.32	{d18, d19}, [r8]
   1bf48: e1a04009     	mov	r4, r9
   1bf4c: f4430a8f     	vst1.32	{d16, d17}, [r3]
   1bf50: ebffe787     	bl	0x15d74    @ imm = #-0x61e4 ; memset
   1bf54: e2853f89     	add	r3, r5, #548
   1bf58: e58d9020     	str	r9, [sp, #0x20]
   1bf5c: e58d3008     	str	r3, [sp, #0x8]
   1bf60: e28d3020     	add	r3, sp, #32
   1bf64: e58d9024     	str	r9, [sp, #0x24]
   1bf68: e58d300c     	str	r3, [sp, #0xc]
   1bf6c: e58d9028     	str	r9, [sp, #0x28]
   1bf70: ea00000c     	b	0x1bfa8
   1bf74: e5973000     	ldr	r3, [r7]
   1bf78: e4843004     	str	r3, [r4], #4
   1bf7c: e58d4024     	str	r4, [sp, #0x24]
   1bf80: ed998a00     	vldr	s16, [r9]
   1bf84: e2877004     	add	r7, r7, #4
   1bf88: e1560004     	cmp	r6, r4
   1bf8c: eeb18a48     	vneg.f32	s16, s16
   1bf90: 0a00000e     	beq	0x1bfd0
   1bf94: e59d3008     	ldr	r3, [sp, #0x8]
   1bf98: eca48a01     	vstmia	r4!, {s16}
   1bf9c: e1570003     	cmp	r7, r3
   1bfa0: e58d4024     	str	r4, [sp, #0x24]
   1bfa4: 0a000026     	beq	0x1c044
   1bfa8: e1540006     	cmp	r4, r6
   1bfac: e1a09007     	mov	r9, r7
   1bfb0: 1affffef     	bne	0x1bf74
   1bfb4: e59d000c     	ldr	r0, [sp, #0xc]
   1bfb8: e1a01004     	mov	r1, r4
   1bfbc: e1a02007     	mov	r2, r7
   1bfc0: eb009017     	bl	0x40024
   1bfc4: e59d4024     	ldr	r4, [sp, #0x24]
   1bfc8: e59d6028     	ldr	r6, [sp, #0x28]
   1bfcc: eaffffeb     	b	0x1bf80
   1bfd0: e59da020     	ldr	r10, [sp, #0x20]
   1bfd4: e046900a     	sub	r9, r6, r10
   1bfd8: e1a06149     	asr	r6, r9, #2
   1bfdc: e376021e     	cmn	r6, #-536870911
   1bfe0: 0a00025c     	beq	0x1c958
   1bfe4: e3560000     	cmp	r6, #0
   1bfe8: 0a000042     	beq	0x1c0f8
   1bfec: e1560086     	cmp	r6, r6, lsl #1
   1bff0: e1a06086     	lsl	r6, r6, #1
   1bff4: 83e0610e     	mvnhi	r6, #-2147483645
   1bff8: 9a000047     	bls	0x1c11c
   1bffc: e1a00006     	mov	r0, r6
   1c000: ebffe641     	bl	0x1590c     @ imm = #-0x66fc ; _Znwj
   1c004: e1a0b000     	mov	r11, r0
   1c008: e0806006     	add	r6, r0, r6
   1c00c: e08b3009     	add	r3, r11, r9
   1c010: e2894004     	add	r4, r9, #4
   1c014: e3590000     	cmp	r9, #0
   1c018: e08b4004     	add	r4, r11, r4
   1c01c: ed838a00     	vstr	s16, [r3]
   1c020: ca000036     	bgt	0x1c100
   1c024: e35a0000     	cmp	r10, #0
   1c028: 1a000038     	bne	0x1c110
   1c02c: e59d3008     	ldr	r3, [sp, #0x8]
   1c030: e58db020     	str	r11, [sp, #0x20]
   1c034: e1570003     	cmp	r7, r3
   1c038: e58d4024     	str	r4, [sp, #0x24]
   1c03c: e58d6028     	str	r6, [sp, #0x28]
   1c040: 1affffd8     	bne	0x1bfa8
   1c044: e59d6020     	ldr	r6, [sp, #0x20]
   1c048: e1560004     	cmp	r6, r4
   1c04c: 0a00006b     	beq	0x1c200
   1c050: e0449006     	sub	r9, r4, r6
   1c054: e1a01004     	mov	r1, r4
   1c058: e1a00006     	mov	r0, r6
   1c05c: e3a03000     	mov	r3, #0
   1c060: e1a02149     	asr	r2, r9, #2
   1c064: e2867004     	add	r7, r6, #4
   1c068: e16f2f12     	clz	r2, r2
   1c06c: e262201f     	rsb	r2, r2, #31
   1c070: e1a02082     	lsl	r2, r2, #1
   1c074: eb004b3a     	bl	0x2ed64
   1c078: e3590040     	cmp	r9, #64
   1c07c: da000036     	ble	0x1c15c
   1c080: e2869040     	add	r9, r6, #64
   1c084: ea000009     	b	0x1c0b0
   1c088: e1560007     	cmp	r6, r7
   1c08c: 0a000003     	beq	0x1c0a0
   1c090: e0472006     	sub	r2, r7, r6
   1c094: e1a01006     	mov	r1, r6
   1c098: e2860004     	add	r0, r6, #4
   1c09c: ebffe650     	bl	0x159e4    @ imm = #-0x66c0 ; memmove
   1c0a0: ed868a00     	vstr	s16, [r6]
   1c0a4: e2877004     	add	r7, r7, #4
   1c0a8: e1590007     	cmp	r9, r7
   1c0ac: 0a00003f     	beq	0x1c1b0
   1c0b0: ed978a00     	vldr	s16, [r7]
   1c0b4: edd67a00     	vldr	s15, [r6]
   1c0b8: eeb48ae7     	vcmpe.f32	s16, s15
   1c0bc: eef1fa10     	vmrs	APSR_nzcv, fpscr
   1c0c0: 4afffff0     	bmi	0x1c088
   1c0c4: ed577a01     	vldr	s15, [r7, #-4]
   1c0c8: e2473004     	sub	r3, r7, #4
   1c0cc: eeb48ae7     	vcmpe.f32	s16, s15
   1c0d0: eef1fa10     	vmrs	APSR_nzcv, fpscr
   1c0d4: 5a000162     	bpl	0x1c664
   1c0d8: e1a02003     	mov	r2, r3
   1c0dc: edc37a01     	vstr	s15, [r3, #4]
   1c0e0: ed737a01     	vldmdb	r3!, {s15}
   1c0e4: eeb48ae7     	vcmpe.f32	s16, s15
   1c0e8: eef1fa10     	vmrs	APSR_nzcv, fpscr
   1c0ec: 4afffff9     	bmi	0x1c0d8
   1c0f0: ed828a00     	vstr	s16, [r2]
   1c0f4: eaffffea     	b	0x1c0a4
   1c0f8: e3a06004     	mov	r6, #4
   1c0fc: eaffffbe     	b	0x1bffc
   1c100: e1a02009     	mov	r2, r9
   1c104: e1a0100a     	mov	r1, r10
   1c108: e1a0000b     	mov	r0, r11
   1c10c: ebffe634     	bl	0x159e4    @ imm = #-0x6730 ; memmove
   1c110: e1a0000a     	mov	r0, r10
   1c114: ebffe749     	bl	0x15e40    @ imm = #-0x62dc ; _ZdlPv
   1c118: eaffffc3     	b	0x1c02c
   1c11c: e3560000     	cmp	r6, #0
   1c120: 01a0b006     	moveq	r11, r6
   1c124: 0affffb8     	beq	0x1c00c
   1c128: e3e0320e     	mvn	r3, #-536870912
   1c12c: e1560003     	cmp	r6, r3
   1c130: 21a06003     	movhs	r6, r3
   1c134: e1a06106     	lsl	r6, r6, #2
   1c138: eaffffaf     	b	0x1bffc
   1c13c: e1570006     	cmp	r7, r6
   1c140: 0a000003     	beq	0x1c154
   1c144: e0472006     	sub	r2, r7, r6
   1c148: e1a01006     	mov	r1, r6
   1c14c: e2860004     	add	r0, r6, #4
   1c150: ebffe623     	bl	0x159e4    @ imm = #-0x6774 ; memmove
   1c154: ed868a00     	vstr	s16, [r6]
   1c158: e2877004     	add	r7, r7, #4
   1c15c: e1540007     	cmp	r4, r7
   1c160: 0a000026     	beq	0x1c200
   1c164: ed978a00     	vldr	s16, [r7]
   1c168: e1a02007     	mov	r2, r7
   1c16c: edd67a00     	vldr	s15, [r6]
   1c170: eeb48ae7     	vcmpe.f32	s16, s15
   1c174: eef1fa10     	vmrs	APSR_nzcv, fpscr
   1c178: 4affffef     	bmi	0x1c13c
   1c17c: ed577a01     	vldr	s15, [r7, #-4]
   1c180: e2473004     	sub	r3, r7, #4
   1c184: eeb48ae7     	vcmpe.f32	s16, s15
   1c188: eef1fa10     	vmrs	APSR_nzcv, fpscr
   1c18c: 5a000005     	bpl	0x1c1a8
   1c190: e1a02003     	mov	r2, r3
   1c194: edc37a01     	vstr	s15, [r3, #4]
   1c198: ed737a01     	vldmdb	r3!, {s15}
   1c19c: eeb48ae7     	vcmpe.f32	s16, s15
   1c1a0: eef1fa10     	vmrs	APSR_nzcv, fpscr
   1c1a4: 4afffff9     	bmi	0x1c190
   1c1a8: ed828a00     	vstr	s16, [r2]
   1c1ac: eaffffe9     	b	0x1c158
   1c1b0: e1590004     	cmp	r9, r4
   1c1b4: 0a000011     	beq	0x1c200
   1c1b8: e286603c     	add	r6, r6, #60
   1c1bc: e1a03009     	mov	r3, r9
   1c1c0: e1a01003     	mov	r1, r3
   1c1c4: e1a02006     	mov	r2, r6
   1c1c8: ecb37a01     	vldmia	r3!, {s14}
   1c1cc: ecf67a01     	vldmia	r6!, {s15}
   1c1d0: eeb47ae7     	vcmpe.f32	s14, s15
   1c1d4: eef1fa10     	vmrs	APSR_nzcv, fpscr
   1c1d8: 5a000005     	bpl	0x1c1f4
   1c1dc: e1a01002     	mov	r1, r2
   1c1e0: edc27a01     	vstr	s15, [r2, #4]
   1c1e4: ed727a01     	vldmdb	r2!, {s15}
   1c1e8: eeb47ae7     	vcmpe.f32	s14, s15
   1c1ec: eef1fa10     	vmrs	APSR_nzcv, fpscr
   1c1f0: 4afffff9     	bmi	0x1c1dc
   1c1f4: e1540003     	cmp	r4, r3
   1c1f8: ed817a00     	vstr	s14, [r1]
   1c1fc: 1affffef     	bne	0x1c1c0
   1c200: e59d6020     	ldr	r6, [sp, #0x20]
   1c204: e59dc024     	ldr	r12, [sp, #0x24]
   1c208: e156000c     	cmp	r6, r12
   1c20c: 0a000103     	beq	0x1c620
   1c210: e1a01006     	mov	r1, r6
   1c214: e2862004     	add	r2, r6, #4
   1c218: ea000005     	b	0x1c234
   1c21c: edd27a00     	vldr	s15, [r2]
   1c220: e2822004     	add	r2, r2, #4
   1c224: ecb17a01     	vldmia	r1!, {s14}
   1c228: eeb47a67     	vcmp.f32	s14, s15
   1c22c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   1c230: 0a00001c     	beq	0x1c2a8
   1c234: e1a03001     	mov	r3, r1
   1c238: e15c0002     	cmp	r12, r2
   1c23c: 1afffff6     	bne	0x1c21c
   1c240: e3a02e1e     	mov	r2, #480
   1c244: e3a01000     	mov	r1, #0
   1c248: e1a00008     	mov	r0, r8
   1c24c: e1a0400c     	mov	r4, r12
   1c250: ebffe6c7     	bl	0x15d74    @ imm = #-0x64e4 ; memset
   1c254: ea00002a     	b	0x1c304
   1c258: 00 00 00 00  	.word	0x00000000
   1c25c: 19 00 00 00  	.word	0x00000019
   1c260: 00 00 00 00  	.word	0x00000000
   1c264: 04 00 00 00  	.word	0x00000004
   1c268: 40 00 00 00  	.word	0x00000040
   1c26c: 00 00 00 00  	.word	0x00000000
   1c270: 00 00 00 00  	.word	0x00000000
   1c274: 02 00 00 00  	.word	0x00000002
   1c278: 00 00 80 c0  	.word	0xc0800000
   1c27c: 00 00 00 c0  	.word	0xc0000000
   1c280: 00 00 80 bf  	.word	0xbf800000
   1c284: 00 00 00 bf  	.word	0xbf000000
   1c288: 00 00 00 3f  	.word	0x3f000000
   1c28c: 00 00 80 3f  	.word	0x3f800000
   1c290: 00 00 00 40  	.word	0x40000000
   1c294: 00 00 80 40  	.word	0x40800000
   1c298: b4 30 07 00  	.word	0x000730b4
   1c29c: c4 30 07 00  	.word	0x000730c4
   1c2a0: 00 f0 7f 45  	.word	0x457ff000
   1c2a4: 00 00 00 00  	.word	0x00000000
   1c2a8: e15c0003     	cmp	r12, r3
   1c2ac: 0affffe3     	beq	0x1c240
   1c2b0: e2832008     	add	r2, r3, #8
   1c2b4: e15c0002     	cmp	r12, r2
   1c2b8: 0a000007     	beq	0x1c2dc
   1c2bc: ecf27a01     	vldmia	r2!, {s15}
   1c2c0: ed937a00     	vldr	s14, [r3]
   1c2c4: eeb47a67     	vcmp.f32	s14, s15
   1c2c8: eef1fa10     	vmrs	APSR_nzcv, fpscr
   1c2cc: 1dc37a01     	vstrne	s15, [r3, #4]
   1c2d0: 12833004     	addne	r3, r3, #4
   1c2d4: e15c0002     	cmp	r12, r2
   1c2d8: 1afffff7     	bne	0x1c2bc
   1c2dc: e2834004     	add	r4, r3, #4
   1c2e0: e3a02e1e     	mov	r2, #480
   1c2e4: e3a01000     	mov	r1, #0
   1c2e8: e1a00008     	mov	r0, r8
   1c2ec: e15c0004     	cmp	r12, r4
   1c2f0: 0a00019e     	beq	0x1c970
   1c2f4: e58d4024     	str	r4, [sp, #0x24]
   1c2f8: ebffe69d     	bl	0x15d74    @ imm = #-0x658c ; memset
   1c2fc: e1560004     	cmp	r6, r4
   1c300: 0a000003     	beq	0x1c314
   1c304: e0442006     	sub	r2, r4, r6
   1c308: e1a00008     	mov	r0, r8
   1c30c: e1a01006     	mov	r1, r6
   1c310: ebffe5b3     	bl	0x159e4    @ imm = #-0x6934 ; memmove
   1c314: e1a00006     	mov	r0, r6
   1c318: ebffe6c8     	bl	0x15e40    @ imm = #-0x64e0 ; _ZdlPv
   1c31c: e5953040     	ldr	r3, [r5, #0x40]
   1c320: e3530002     	cmp	r3, #2
   1c324: ca000098     	bgt	0x1c58c
   1c328: e3530000     	cmp	r3, #0
   1c32c: ca0000c2     	bgt	0x1c63c
   1c330: 1a00009b     	bne	0x1c5a4
   1c334: f2c00010     	vmov.i32	d16, #0x0
   1c338: e3a00e7a     	mov	r0, #1952
   1c33c: e58d3018     	str	r3, [sp, #0x18]
   1c340: edcd0b04     	vstr	d16, [sp, #16]
   1c344: ebffe570     	bl	0x1590c     @ imm = #-0x6a40 ; _Znwj
   1c348: e59d6010     	ldr	r6, [sp, #0x10]
   1c34c: e1a04000     	mov	r4, r0
   1c350: e59d2014     	ldr	r2, [sp, #0x14]
   1c354: e0422006     	sub	r2, r2, r6
   1c358: e3520000     	cmp	r2, #0
   1c35c: ca0000c2     	bgt	0x1c66c
   1c360: e3560000     	cmp	r6, #0
   1c364: 1a0000c2     	bne	0x1c674
   1c368: f2c00010     	vmov.i32	d16, #0x0
   1c36c: ee814b90     	vdup.32	d17, r4
   1c370: e28d3024     	add	r3, sp, #36
   1c374: e3a02000     	mov	r2, #0
   1c378: e34c2080     	movt	r2, #0xc080
   1c37c: e58d2020     	str	r2, [sp, #0x20]
   1c380: e28d2030     	add	r2, sp, #48
   1c384: e28d0010     	add	r0, sp, #16
   1c388: f443078f     	vst1.32	{d16}, [r3]
   1c38c: e2421010     	sub	r1, r2, #16
   1c390: e2844e7a     	add	r4, r4, #1952
   1c394: e3a03000     	mov	r3, #0
   1c398: e58d4018     	str	r4, [sp, #0x18]
   1c39c: e58d302c     	str	r3, [sp, #0x2c]
   1c3a0: edcd1b04     	vstr	d17, [sp, #16]
   1c3a4: ebfffdc8     	bl	0x1bacc
   1c3a8: e59d8014     	ldr	r8, [sp, #0x14]
   1c3ac: eef18a00     	vmov.f32	s17, #4.000000e+00
   1c3b0: e59d4018     	ldr	r4, [sp, #0x18]
   1c3b4: ed1f9a47     	vldr	s18, [pc, #-284]        @ 0x1c2a0 ; float 4095
   1c3b8: ed5f9a47     	vldr	s19, [pc, #-284]        @ 0x1c2a4 ; float 0
   1c3bc: ea000008     	b	0x1c3e4
   1c3c0: e1cd20d4     	ldrd	r2, r3, [sp, #4]
   1c3c4: e5886004     	str	r6, [r8, #0x4]
   1c3c8: e588700c     	str	r7, [r8, #0xc]
   1c3cc: e2888010     	add	r8, r8, #16
   1c3d0: ed088a04     	vstr	s16, [r8, #-16]
   1c3d4: e1530002     	cmp	r3, r2
   1c3d8: ed487a02     	vstr	s15, [r8, #-8]
   1c3dc: e58d8014     	str	r8, [sp, #0x14]
   1c3e0: 0a000036     	beq	0x1c4c0
   1c3e4: e59d3004     	ldr	r3, [sp, #0x4]
   1c3e8: eeb47a00     	vmov.f32	s14, #1.250000e-01
   1c3ec: e3002fff     	movw	r2, #0xfff
   1c3f0: ecb38a01     	vldmia	r3!, {s16}
   1c3f4: ee787a28     	vadd.f32	s15, s16, s17
   1c3f8: e58d3004     	str	r3, [sp, #0x4]
   1c3fc: ee677a87     	vmul.f32	s15, s15, s14
   1c400: eeb07a69     	vmov.f32	s14, s19
   1c404: eea77a89     	vfma.f32	s14, s15, s18
   1c408: eefd7ac7     	vcvt.s32.f32	s15, s14
   1c40c: ee173a90     	vmov	r3, s15
   1c410: edcd7a03     	vstr	s15, [sp, #12]
   1c414: e2436032     	sub	r6, r3, #50
   1c418: e2837032     	add	r7, r3, #50
   1c41c: e1560002     	cmp	r6, r2
   1c420: a1a06002     	movge	r6, r2
   1c424: e1570002     	cmp	r7, r2
   1c428: a1a07002     	movge	r7, r2
   1c42c: e1540008     	cmp	r4, r8
   1c430: e1c66fc6     	bic	r6, r6, r6, asr #31
   1c434: e1c77fc7     	bic	r7, r7, r7, asr #31
   1c438: 1affffe0     	bne	0x1c3c0
   1c43c: e59da010     	ldr	r10, [sp, #0x10]
   1c440: e044900a     	sub	r9, r4, r10
   1c444: e1a04249     	asr	r4, r9, #4
   1c448: e374037e     	cmn	r4, #-134217727
   1c44c: 0a000144     	beq	0x1c964
   1c450: e3540000     	cmp	r4, #0
   1c454: 0a000098     	beq	0x1c6bc
   1c458: e1540084     	cmp	r4, r4, lsl #1
   1c45c: e1a04084     	lsl	r4, r4, #1
   1c460: 83e0413e     	mvnhi	r4, #-2147483633
   1c464: 9a00008c     	bls	0x1c69c
   1c468: e1a00004     	mov	r0, r4
   1c46c: ebffe526     	bl	0x1590c     @ imm = #-0x6b68 ; _Znwj
   1c470: e1a0b000     	mov	r11, r0
   1c474: e0804004     	add	r4, r0, r4
   1c478: e08b2009     	add	r2, r11, r9
   1c47c: e2893010     	add	r3, r9, #16
   1c480: e08b8003     	add	r8, r11, r3
   1c484: e59d300c     	ldr	r3, [sp, #0xc]
   1c488: e3590000     	cmp	r9, #0
   1c48c: e5826004     	str	r6, [r2, #0x4]
   1c490: e5823008     	str	r3, [r2, #0x8]
   1c494: e582700c     	str	r7, [r2, #0xc]
   1c498: ed828a00     	vstr	s16, [r2]
   1c49c: ca000077     	bgt	0x1c680
   1c4a0: e35a0000     	cmp	r10, #0
   1c4a4: 1a000079     	bne	0x1c690
   1c4a8: e1cd20d4     	ldrd	r2, r3, [sp, #4]
   1c4ac: e58db010     	str	r11, [sp, #0x10]
   1c4b0: e58d8014     	str	r8, [sp, #0x14]
   1c4b4: e1530002     	cmp	r3, r2
   1c4b8: e58d4018     	str	r4, [sp, #0x18]
   1c4bc: 1affffc8     	bne	0x1c3e4
   1c4c0: e51f3230     	ldr	r3, [pc, #-0x230]       @ 0x1c298
   1c4c4: e28dc020     	add	r12, sp, #32
   1c4c8: e893000f     	ldm	r3, {r0, r1, r2, r3}
   1c4cc: e88c000f     	stm	r12, {r0, r1, r2, r3}
   1c4d0: e1a0100c     	mov	r1, r12
   1c4d4: e28d0010     	add	r0, sp, #16
   1c4d8: ebfffd7b     	bl	0x1bacc
   1c4dc: e59d6010     	ldr	r6, [sp, #0x10]
   1c4e0: e59d8014     	ldr	r8, [sp, #0x14]
   1c4e4: e1560008     	cmp	r6, r8
   1c4e8: 0a000078     	beq	0x1c6d0
   1c4ec: e0484006     	sub	r4, r8, r6
   1c4f0: e1a01008     	mov	r1, r8
   1c4f4: e1a00006     	mov	r0, r6
   1c4f8: e3a03000     	mov	r3, #0
   1c4fc: e1a02244     	asr	r2, r4, #4
   1c500: e16f2f12     	clz	r2, r2
   1c504: e262201f     	rsb	r2, r2, #31
   1c508: e1a02082     	lsl	r2, r2, #1
   1c50c: ebfffdde     	bl	0x1bc8c
   1c510: e3540c01     	cmp	r4, #256
   1c514: da00006a     	ble	0x1c6c4
   1c518: e2864c01     	add	r4, r6, #256
   1c51c: e1a00006     	mov	r0, r6
   1c520: e1a01004     	mov	r1, r4
   1c524: e28d6020     	add	r6, sp, #32
   1c528: ebfffda3     	bl	0x1bbbc
   1c52c: e1a0e004     	mov	lr, r4
   1c530: e158000e     	cmp	r8, lr
   1c534: e1a0700e     	mov	r7, lr
   1c538: 0a000064     	beq	0x1c6d0
   1c53c: e89e000f     	ldm	lr, {r0, r1, r2, r3}
   1c540: e886000f     	stm	r6, {r0, r1, r2, r3}
   1c544: e59e9008     	ldr	r9, [lr, #0x8]
   1c548: e51e3008     	ldr	r3, [lr, #-0x8]
   1c54c: e1530009     	cmp	r3, r9
   1c550: da000008     	ble	0x1c578
   1c554: e24ec010     	sub	r12, lr, #16
   1c558: e28c4010     	add	r4, r12, #16
   1c55c: e1a0700c     	mov	r7, r12
   1c560: e89c000f     	ldm	r12, {r0, r1, r2, r3}
   1c564: e24cc010     	sub	r12, r12, #16
   1c568: e884000f     	stm	r4, {r0, r1, r2, r3}
   1c56c: e59c3008     	ldr	r3, [r12, #0x8]
   1c570: e1530009     	cmp	r3, r9
   1c574: cafffff7     	bgt	0x1c558
   1c578: e58d9028     	str	r9, [sp, #0x28]
   1c57c: e28ee010     	add	lr, lr, #16
   1c580: e896000f     	ldm	r6, {r0, r1, r2, r3}
   1c584: e887000f     	stm	r7, {r0, r1, r2, r3}
   1c588: eaffffe8     	b	0x1c530
   1c58c: e3530003     	cmp	r3, #3
   1c590: 1a000003     	bne	0x1c5a4
   1c594: e59d0008     	ldr	r0, [sp, #0x8]
   1c598: e28d1030     	add	r1, sp, #48
   1c59c: e3a02901     	mov	r2, #16384
   1c5a0: ebffe6a7     	bl	0x16044    @ imm = #-0x6564 ; memcpy
   1c5a4: e51fe310     	ldr	lr, [pc, #-0x310]       @ 0x1c29c
   1c5a8: e2854901     	add	r4, r5, #16384
   1c5ac: e284cf8a     	add	r12, r4, #552
   1c5b0: e2849f9b     	add	r9, r4, #620
   1c5b4: eddf4bf9     	vldr	d20, [pc, #996]         @ 0x1c9a0 ; float 3.05175853327e-05
   1c5b8: eddf5bfa     	vldr	d21, [pc, #1000]        @ 0x1c9a8 ; float 4.65661395381e-10
   1c5bc: e2848f9f     	add	r8, r4, #636
   1c5c0: e8be000f     	ldm	lr!, {r0, r1, r2, r3}
   1c5c4: e8ac000f     	stm	r12!, {r0, r1, r2, r3}
   1c5c8: e2847fa3     	add	r7, r4, #652
   1c5cc: eddf2bf7     	vldr	d18, [pc, #988]         @ 0x1c9b0 ; float 1.3411048233e-08
   1c5d0: eddf3bf8     	vldr	d19, [pc, #992]         @ 0x1c9b8 ; float 1.34110464955e-08
   1c5d4: e3a06003     	mov	r6, #3
   1c5d8: eddf0bf8     	vldr	d16, [pc, #992]         @ 0x1c9c0 ; float 3.43322837737e-06
   1c5dc: eddf1bf9     	vldr	d17, [pc, #996]         @ 0x1c9c8 ; float 0.000878906470662
   1c5e0: e5846224     	str	r6, [r4, #0x224]
   1c5e4: e8be000f     	ldm	lr!, {r0, r1, r2, r3}
   1c5e8: e8ac000f     	stm	r12!, {r0, r1, r2, r3}
   1c5ec: e8be000f     	ldm	lr!, {r0, r1, r2, r3}
   1c5f0: e8ac000f     	stm	r12!, {r0, r1, r2, r3}
   1c5f4: e89e000f     	ldm	lr, {r0, r1, r2, r3}
   1c5f8: e88c000f     	stm	r12, {r0, r1, r2, r3}
   1c5fc: e1a00005     	mov	r0, r5
   1c600: e5846268     	str	r6, [r4, #0x268]
   1c604: f4494a8f     	vst1.32	{d20, d21}, [r9]
   1c608: f4482a8f     	vst1.32	{d18, d19}, [r8]
   1c60c: f4470a8f     	vst1.32	{d16, d17}, [r7]
   1c610: e28dd901     	add	sp, sp, #16384
   1c614: e28dd034     	add	sp, sp, #52
   1c618: ecbd8b04     	vpop	{d8, d9}
   1c61c: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
   1c620: e1a00008     	mov	r0, r8
   1c624: e3a02e1e     	mov	r2, #480
   1c628: e3a01000     	mov	r1, #0
   1c62c: ebffe5d0     	bl	0x15d74    @ imm = #-0x68c0 ; memset
   1c630: e3560000     	cmp	r6, #0
   1c634: 0affff38     	beq	0x1c31c
   1c638: eaffff35     	b	0x1c314
   1c63c: e2852c42     	add	r2, r5, #16896
   1c640: e59d3008     	ldr	r3, [sp, #0x8]
   1c644: e2822024     	add	r2, r2, #36
   1c648: ed9f7ae0     	vldr	s14, [pc, #896]         @ 0x1c9d0 ; float 0.00195360206999
   1c64c: eef97a00     	vmov.f32	s15, #-4.000000e+00
   1c650: ece37a01     	vstmia	r3!, {s15}
   1c654: ee777a87     	vadd.f32	s15, s15, s14
   1c658: e1530002     	cmp	r3, r2
   1c65c: 1afffffb     	bne	0x1c650
   1c660: eaffffcf     	b	0x1c5a4
   1c664: e1a02007     	mov	r2, r7
   1c668: eafffea0     	b	0x1c0f0
   1c66c: e1a01006     	mov	r1, r6
   1c670: ebffe4db     	bl	0x159e4    @ imm = #-0x6c94 ; memmove
   1c674: e1a00006     	mov	r0, r6
   1c678: ebffe5f0     	bl	0x15e40    @ imm = #-0x6840 ; _ZdlPv
   1c67c: eaffff39     	b	0x1c368
   1c680: e1a02009     	mov	r2, r9
   1c684: e1a0100a     	mov	r1, r10
   1c688: e1a0000b     	mov	r0, r11
   1c68c: ebffe4d4     	bl	0x159e4    @ imm = #-0x6cb0 ; memmove
   1c690: e1a0000a     	mov	r0, r10
   1c694: ebffe5e9     	bl	0x15e40    @ imm = #-0x685c ; _ZdlPv
   1c698: eaffff82     	b	0x1c4a8
   1c69c: e3540000     	cmp	r4, #0
   1c6a0: 01a0b004     	moveq	r11, r4
   1c6a4: 0affff73     	beq	0x1c478
   1c6a8: e3e0333e     	mvn	r3, #-134217728
   1c6ac: e1540003     	cmp	r4, r3
   1c6b0: 21a04003     	movhs	r4, r3
   1c6b4: e1a04204     	lsl	r4, r4, #4
   1c6b8: eaffff6a     	b	0x1c468
   1c6bc: e3a04010     	mov	r4, #16
   1c6c0: eaffff68     	b	0x1c468
   1c6c4: e1a01008     	mov	r1, r8
   1c6c8: e1a00006     	mov	r0, r6
   1c6cc: ebfffd3a     	bl	0x1bbbc
   1c6d0: e59de010     	ldr	lr, [sp, #0x10]
   1c6d4: e59d7014     	ldr	r7, [sp, #0x14]
   1c6d8: e15e0007     	cmp	lr, r7
   1c6dc: 0a000097     	beq	0x1c940
   1c6e0: e28e3010     	add	r3, lr, #16
   1c6e4: e1570003     	cmp	r7, r3
   1c6e8: 0a000094     	beq	0x1c940
   1c6ec: ed9e7a00     	vldr	s14, [lr]
   1c6f0: e1a0300e     	mov	r3, lr
   1c6f4: edd37a04     	vldr	s15, [r3, #16]
   1c6f8: e1a06003     	mov	r6, r3
   1c6fc: eef47a47     	vcmp.f32	s15, s14
   1c700: eef1fa10     	vmrs	APSR_nzcv, fpscr
   1c704: 1a000003     	bne	0x1c718
   1c708: e5931008     	ldr	r1, [r3, #0x8]
   1c70c: e5932018     	ldr	r2, [r3, #0x18]
   1c710: e1510002     	cmp	r1, r2
   1c714: 0a000005     	beq	0x1c730
   1c718: e2832020     	add	r2, r3, #32
   1c71c: e2833010     	add	r3, r3, #16
   1c720: e1570002     	cmp	r7, r2
   1c724: 0a000085     	beq	0x1c940
   1c728: eeb07a67     	vmov.f32	s14, s15
   1c72c: eafffff0     	b	0x1c6f4
   1c730: e1570003     	cmp	r7, r3
   1c734: 0a000081     	beq	0x1c940
   1c738: e2833020     	add	r3, r3, #32
   1c73c: e1570003     	cmp	r7, r3
   1c740: 0a000010     	beq	0x1c788
   1c744: e286c030     	add	r12, r6, #48
   1c748: ed967a00     	vldr	s14, [r6]
   1c74c: ed5c7a04     	vldr	s15, [r12, #-16]
   1c750: eeb47a67     	vcmp.f32	s14, s15
   1c754: eef1fa10     	vmrs	APSR_nzcv, fpscr
   1c758: 1a000003     	bne	0x1c76c
   1c75c: e5962008     	ldr	r2, [r6, #0x8]
   1c760: e51c3008     	ldr	r3, [r12, #-0x8]
   1c764: e1520003     	cmp	r2, r3
   1c768: 0a000003     	beq	0x1c77c
   1c76c: e2864010     	add	r4, r6, #16
   1c770: e91c000f     	ldmdb	r12, {r0, r1, r2, r3}
   1c774: e1a06004     	mov	r6, r4
   1c778: e884000f     	stm	r4, {r0, r1, r2, r3}
   1c77c: e157000c     	cmp	r7, r12
   1c780: e28cc010     	add	r12, r12, #16
   1c784: 1affffef     	bne	0x1c748
   1c788: e2866010     	add	r6, r6, #16
   1c78c: e1570006     	cmp	r7, r6
   1c790: 158d6014     	strne	r6, [sp, #0x14]
   1c794: e59dc014     	ldr	r12, [sp, #0x14]
   1c798: e04cc00e     	sub	r12, r12, lr
   1c79c: e1a0324c     	asr	r3, r12, #4
   1c7a0: e3530001     	cmp	r3, #1
   1c7a4: 0a00005c     	beq	0x1c91c
   1c7a8: e24ec010     	sub	r12, lr, #16
   1c7ac: e1a06203     	lsl	r6, r3, #4
   1c7b0: e08cc203     	add	r12, r12, r3, lsl #4
   1c7b4: e1a0300e     	mov	r3, lr
   1c7b8: e593100c     	ldr	r1, [r3, #0xc]
   1c7bc: e5932014     	ldr	r2, [r3, #0x14]
   1c7c0: e1510002     	cmp	r1, r2
   1c7c4: da000008     	ble	0x1c7ec
   1c7c8: e5930008     	ldr	r0, [r3, #0x8]
   1c7cc: e5931018     	ldr	r1, [r3, #0x18]
   1c7d0: e0412000     	sub	r2, r1, r0
   1c7d4: e0822fa2     	add	r2, r2, r2, lsr #31
   1c7d8: e1a020c2     	asr	r2, r2, #1
   1c7dc: e0820000     	add	r0, r2, r0
   1c7e0: e0412002     	sub	r2, r1, r2
   1c7e4: e583000c     	str	r0, [r3, #0xc]
   1c7e8: e5832014     	str	r2, [r3, #0x14]
   1c7ec: e2833010     	add	r3, r3, #16
   1c7f0: e15c0003     	cmp	r12, r3
   1c7f4: 1affffef     	bne	0x1c7b8
   1c7f8: e24e400c     	sub	r4, lr, #12
   1c7fc: e28e0004     	add	r0, lr, #4
   1c800: e0844006     	add	r4, r4, r6
   1c804: e5903000     	ldr	r3, [r0]
   1c808: e5901008     	ldr	r1, [r0, #0x8]
   1c80c: e1530001     	cmp	r3, r1
   1c810: aa000028     	bge	0x1c8b8
   1c814: e2832089     	add	r2, r3, #137
   1c818: e2816089     	add	r6, r1, #137
   1c81c: e240c004     	sub	r12, r0, #4
   1c820: e0417003     	sub	r7, r1, r3
   1c824: e0856106     	add	r6, r5, r6, lsl #2
   1c828: e0852102     	add	r2, r5, r2, lsl #2
   1c82c: e1520000     	cmp	r2, r0
   1c830: 315c0006     	cmplo	r12, r6
   1c834: e2476001     	sub	r6, r7, #1
   1c838: 23a0c001     	movhs	r12, #1
   1c83c: 33a0c000     	movlo	r12, #0
   1c840: e3560007     	cmp	r6, #7
   1c844: 93a0c000     	movls	r12, #0
   1c848: 820cc001     	andhi	r12, r12, #1
   1c84c: e35c0000     	cmp	r12, #0
   1c850: 0a000034     	beq	0x1c928
   1c854: e5106004     	ldr	r6, [r0, #-0x4]
   1c858: e1a08127     	lsr	r8, r7, #2
   1c85c: e3a0c000     	mov	r12, #0
   1c860: eea06b90     	vdup.32	q8, r6
   1c864: e28cc001     	add	r12, r12, #1
   1c868: f4420a8d     	vst1.32	{d16, d17}, [r2]!
   1c86c: e15c0008     	cmp	r12, r8
   1c870: 1afffffb     	bne	0x1c864
   1c874: e3c7c003     	bic	r12, r7, #3
   1c878: e08c2003     	add	r2, r12, r3
   1c87c: e15c0007     	cmp	r12, r7
   1c880: 0a00000c     	beq	0x1c8b8
   1c884: e085c102     	add	r12, r5, r2, lsl #2
   1c888: e5106004     	ldr	r6, [r0, #-0x4]
   1c88c: e2823001     	add	r3, r2, #1
   1c890: e1510003     	cmp	r1, r3
   1c894: e58c6224     	str	r6, [r12, #0x224]
   1c898: da000006     	ble	0x1c8b8
   1c89c: e0853103     	add	r3, r5, r3, lsl #2
   1c8a0: e2822002     	add	r2, r2, #2
   1c8a4: e1510002     	cmp	r1, r2
   1c8a8: e5836224     	str	r6, [r3, #0x224]
   1c8ac: c0852102     	addgt	r2, r5, r2, lsl #2
   1c8b0: c5103004     	ldrgt	r3, [r0, #-0x4]
   1c8b4: c5823224     	strgt	r3, [r2, #0x224]
   1c8b8: e5903010     	ldr	r3, [r0, #0x10]
   1c8bc: e1510003     	cmp	r1, r3
   1c8c0: aa000010     	bge	0x1c908
   1c8c4: e0432001     	sub	r2, r3, r1
   1c8c8: ee072a90     	vmov	s15, r2
   1c8cc: e2813089     	add	r3, r1, #137
   1c8d0: e3a01000     	mov	r1, #0
   1c8d4: eef86ae7     	vcvt.f32.s32	s13, s15
   1c8d8: e0853103     	add	r3, r5, r3, lsl #2
   1c8dc: ee071a90     	vmov	s15, r1
   1c8e0: ed907a03     	vldr	s14, [r0, #12]
   1c8e4: e2811001     	add	r1, r1, #1
   1c8e8: eef85ae7     	vcvt.f32.s32	s11, s15
   1c8ec: ed507a01     	vldr	s15, [r0, #-4]
   1c8f0: e1510002     	cmp	r1, r2
   1c8f4: ee377a67     	vsub.f32	s14, s14, s15
   1c8f8: ee856aa6     	vdiv.f32	s12, s11, s13
   1c8fc: eee67a07     	vfma.f32	s15, s12, s14
   1c900: ece37a01     	vstmia	r3!, {s15}
   1c904: 1afffff4     	bne	0x1c8dc
   1c908: e2800010     	add	r0, r0, #16
   1c90c: e1500004     	cmp	r0, r4
   1c910: 1affffbb     	bne	0x1c804
   1c914: e35e0000     	cmp	lr, #0
   1c918: 0affff21     	beq	0x1c5a4
   1c91c: e1a0000e     	mov	r0, lr
   1c920: ebffe546     	bl	0x15e40    @ imm = #-0x6ae8 ; _ZdlPv
   1c924: eaffff1e     	b	0x1c5a4
   1c928: e510c004     	ldr	r12, [r0, #-0x4]
   1c92c: e2833001     	add	r3, r3, #1
   1c930: e1510003     	cmp	r1, r3
   1c934: e482c004     	str	r12, [r2], #4
   1c938: 1afffffa     	bne	0x1c928
   1c93c: eaffffdd     	b	0x1c8b8
   1c940: e59dc014     	ldr	r12, [sp, #0x14]
   1c944: e04cc00e     	sub	r12, r12, lr
   1c948: e1a0324c     	asr	r3, r12, #4
   1c94c: e3530001     	cmp	r3, #1
   1c950: 1affff94     	bne	0x1c7a8
   1c954: eaffffee     	b	0x1c914
   1c958: e3000c8c     	movw	r0, #0xc8c
   1c95c: e3400007     	movt	r0, #0x7
   1c960: ebffe494     	bl	0x15bb8    @ imm = #-0x6db0 ; _ZSt20__throw_length_errorPKc
   1c964: e3000c8c     	movw	r0, #0xc8c
   1c968: e3400007     	movt	r0, #0x7
   1c96c: ebffe491     	bl	0x15bb8    @ imm = #-0x6dbc ; _ZSt20__throw_length_errorPKc
   1c970: ebffe4ff     	bl	0x15d74    @ imm = #-0x6c04 ; memset
   1c974: eafffe62     	b	0x1c304
   1c978: e59d0020     	ldr	r0, [sp, #0x20]
   1c97c: e3500000     	cmp	r0, #0
   1c980: 0a000000     	beq	0x1c988
   1c984: ebffe52d     	bl	0x15e40    @ imm = #-0x6b4c ; _ZdlPv
   1c988: ebffe574     	bl	0x15f60    @ imm = #-0x6a30 ; __cxa_end_cleanup
   1c98c: e59d0010     	ldr	r0, [sp, #0x10]
   1c990: e3500000     	cmp	r0, #0
   1c994: 1afffffa     	bne	0x1c984
   1c998: eafffffa     	b	0x1c988
   1c99c: e320f000     	nop
   1c9a0: 66 66 66 3f  	.word	0x3f666666
   1c9a4: 00 00 00 3f  	.word	0x3f000000
   1c9a8: cd cc 4c 3e  	.word	0x3e4ccccd
   1c9ac: 00 00 00 3e  	.word	0x3e000000
   1c9b0: 9a 99 99 3e  	.word	0x3e99999a
   1c9b4: cd cc 4c 3e  	.word	0x3e4ccccd
   1c9b8: 00 00 00 00  	.word	0x00000000
   1c9bc: cd cc 4c 3e  	.word	0x3e4ccccd
   1c9c0: 00 00 c8 42  	.word	0x42c80000
   1c9c4: cd cc cc 3e  	.word	0x3ecccccd
   1c9c8: 00 40 1c 46  	.word	0x461c4000
   1c9cc: cd cc 4c 3f  	.word	0x3f4ccccd
   1c9d0: 01 08 00 3b  	.word	0x3b000801
