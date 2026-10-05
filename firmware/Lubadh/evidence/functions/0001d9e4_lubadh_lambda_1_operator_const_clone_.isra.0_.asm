; lubadh::{lambda()#1}::operator()() const [clone .isra.0]
; VA 0x1d9e4 size 2756

   1d9e4: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   1d9e8: e3a01000     	mov	r1, #0
   1d9ec: e304229c     	movw	r2, #0x429c
   1d9f0: ed2d8b04     	vpush	{d8, d9}
   1d9f4: e24dd034     	sub	sp, sp, #52
   1d9f8: e1a05000     	mov	r5, r0
   1d9fc: ebffe0dc     	bl	0x15d74    @ imm = #-0x7c90 ; memset
   1da00: eddf0bf4     	vldr	d16, [pc, #976]         @ 0x1ddd8 ; float 5.30498947741e-313
   1da04: eddf1bf5     	vldr	d17, [pc, #980]         @ 0x1dde0 ; float 8.48798316386e-314
   1da08: e1a03005     	mov	r3, r5
   1da0c: eddf2bf5     	vldr	d18, [pc, #980]         @ 0x1dde8 ; float 3.16202013338e-322
   1da10: eddf3bf6     	vldr	d19, [pc, #984]         @ 0x1ddf0 ; float 4.24399158193e-314
   1da14: e2850020     	add	r0, r5, #32
   1da18: f2c04050     	vmov.i32	q10, #0x0
   1da1c: e2856044     	add	r6, r5, #68
   1da20: e3a01000     	mov	r1, #0
   1da24: e3a02c05     	mov	r2, #1280
   1da28: f4402a8f     	vst1.32	{d18, d19}, [r0]
   1da2c: e1a00006     	mov	r0, r6
   1da30: e1a04001     	mov	r4, r1
   1da34: f4434a8d     	vst1.32	{d20, d21}, [r3]!
   1da38: e30ccccd     	movw	r12, #0xcccd
   1da3c: e343cdcc     	movt	r12, #0x3dcc
   1da40: f4430a8f     	vst1.32	{d16, d17}, [r3]
   1da44: e1a08006     	mov	r8, r6
   1da48: e1a09004     	mov	r9, r4
   1da4c: e5852030     	str	r2, [r5, #0x30]
   1da50: e3a02e1e     	mov	r2, #480
   1da54: e585c034     	str	r12, [r5, #0x34]
   1da58: ebffe0c5     	bl	0x15d74    @ imm = #-0x7cec ; memset
   1da5c: eddf2be5     	vldr	d18, [pc, #916]         @ 0x1ddf8 ; float -2.00000143424
   1da60: eddf3be6     	vldr	d19, [pc, #920]         @ 0x1de00 ; float -3.0517599896e-05
   1da64: e2853058     	add	r3, r5, #88
   1da68: eddf0be6     	vldr	d16, [pc, #920]         @ 0x1de08 ; float 0.00781250183354
   1da6c: eddf1be7     	vldr	d17, [pc, #924]         @ 0x1de10 ; float 512.00012207
   1da70: e1a07004     	mov	r7, r4
   1da74: f4462a8f     	vst1.32	{d18, d19}, [r6]
   1da78: f4430a8f     	vst1.32	{d16, d17}, [r3]
   1da7c: ea00000e     	b	0x1dabc
   1da80: e1a0b009     	mov	r11, r9
   1da84: ed988a00     	vldr	s16, [r8]
   1da88: e1a0a007     	mov	r10, r7
   1da8c: ecab8a01     	vstmia	r11!, {s16}
   1da90: eeb18a48     	vneg.f32	s16, s16
   1da94: e154000b     	cmp	r4, r11
   1da98: 0a000025     	beq	0x1db34
   1da9c: e1a0900b     	mov	r9, r11
   1daa0: e1a0700a     	mov	r7, r10
   1daa4: eca98a01     	vstmia	r9!, {s16}
   1daa8: e2853f89     	add	r3, r5, #548
   1daac: e2888004     	add	r8, r8, #4
   1dab0: e1580003     	cmp	r8, r3
   1dab4: e58d300c     	str	r3, [sp, #0xc]
   1dab8: 0a000058     	beq	0x1dc20
   1dabc: e1540009     	cmp	r4, r9
   1dac0: 1affffee     	bne	0x1da80
   1dac4: e0449007     	sub	r9, r4, r7
   1dac8: e1a04149     	asr	r4, r9, #2
   1dacc: e374021e     	cmn	r4, #-536870911
   1dad0: 0a000251     	beq	0x1e41c
   1dad4: e3540000     	cmp	r4, #0
   1dad8: 0a00003d     	beq	0x1dbd4
   1dadc: e1540084     	cmp	r4, r4, lsl #1
   1dae0: e1a04084     	lsl	r4, r4, #1
   1dae4: 83e0410e     	mvnhi	r4, #-2147483645
   1dae8: 9a000031     	bls	0x1dbb4
   1daec: e1a00004     	mov	r0, r4
   1daf0: ebffdf85     	bl	0x1590c     @ imm = #-0x81ec ; _Znwj
   1daf4: e1a0a000     	mov	r10, r0
   1daf8: e0804004     	add	r4, r0, r4
   1dafc: ed988a00     	vldr	s16, [r8]
   1db00: e08a3009     	add	r3, r10, r9
   1db04: e289b004     	add	r11, r9, #4
   1db08: e3590000     	cmp	r9, #0
   1db0c: e08ab00b     	add	r11, r10, r11
   1db10: ed838a00     	vstr	s16, [r3]
   1db14: ca00001f     	bgt	0x1db98
   1db18: e3570000     	cmp	r7, #0
   1db1c: 0affffdb     	beq	0x1da90
   1db20: e1a00007     	mov	r0, r7
   1db24: ebffe0c5     	bl	0x15e40    @ imm = #-0x7cec ; _ZdlPv
   1db28: eeb18a48     	vneg.f32	s16, s16
   1db2c: e154000b     	cmp	r4, r11
   1db30: 1affffd9     	bne	0x1da9c
   1db34: e044b00a     	sub	r11, r4, r10
   1db38: e1a0414b     	asr	r4, r11, #2
   1db3c: e374021e     	cmn	r4, #-536870911
   1db40: 0a000238     	beq	0x1e428
   1db44: e3540000     	cmp	r4, #0
   1db48: 0a000032     	beq	0x1dc18
   1db4c: e1540084     	cmp	r4, r4, lsl #1
   1db50: e1a04084     	lsl	r4, r4, #1
   1db54: 83e0410e     	mvnhi	r4, #-2147483645
   1db58: 9a000026     	bls	0x1dbf8
   1db5c: e1a00004     	mov	r0, r4
   1db60: ebffdf69     	bl	0x1590c     @ imm = #-0x825c ; _Znwj
   1db64: e1a07000     	mov	r7, r0
   1db68: e0804004     	add	r4, r0, r4
   1db6c: e087300b     	add	r3, r7, r11
   1db70: e28b9004     	add	r9, r11, #4
   1db74: e35b0000     	cmp	r11, #0
   1db78: e0879009     	add	r9, r7, r9
   1db7c: ed838a00     	vstr	s16, [r3]
   1db80: ca000015     	bgt	0x1dbdc
   1db84: e35a0000     	cmp	r10, #0
   1db88: 0affffc6     	beq	0x1daa8
   1db8c: e1a0000a     	mov	r0, r10
   1db90: ebffe0aa     	bl	0x15e40    @ imm = #-0x7d58 ; _ZdlPv
   1db94: eaffffc3     	b	0x1daa8
   1db98: e1a02009     	mov	r2, r9
   1db9c: e1a01007     	mov	r1, r7
   1dba0: e1a0000a     	mov	r0, r10
   1dba4: ebffdf8e     	bl	0x159e4    @ imm = #-0x81c8 ; memmove
   1dba8: e1a00007     	mov	r0, r7
   1dbac: ebffe0a3     	bl	0x15e40    @ imm = #-0x7d74 ; _ZdlPv
   1dbb0: eaffffdc     	b	0x1db28
   1dbb4: e3540000     	cmp	r4, #0
   1dbb8: 01a0a004     	moveq	r10, r4
   1dbbc: 0affffce     	beq	0x1dafc
   1dbc0: e3e0320e     	mvn	r3, #-536870912
   1dbc4: e1540003     	cmp	r4, r3
   1dbc8: 21a04003     	movhs	r4, r3
   1dbcc: e1a04104     	lsl	r4, r4, #2
   1dbd0: eaffffc5     	b	0x1daec
   1dbd4: e3a04004     	mov	r4, #4
   1dbd8: eaffffc3     	b	0x1daec
   1dbdc: e1a0200b     	mov	r2, r11
   1dbe0: e1a0100a     	mov	r1, r10
   1dbe4: e1a00007     	mov	r0, r7
   1dbe8: ebffdf7d     	bl	0x159e4    @ imm = #-0x820c ; memmove
   1dbec: e1a0000a     	mov	r0, r10
   1dbf0: ebffe092     	bl	0x15e40    @ imm = #-0x7db8 ; _ZdlPv
   1dbf4: eaffffab     	b	0x1daa8
   1dbf8: e3540000     	cmp	r4, #0
   1dbfc: 01a07004     	moveq	r7, r4
   1dc00: 0affffd9     	beq	0x1db6c
   1dc04: e3e0320e     	mvn	r3, #-536870912
   1dc08: e1540003     	cmp	r4, r3
   1dc0c: 21a04003     	movhs	r4, r3
   1dc10: e1a04104     	lsl	r4, r4, #2
   1dc14: eaffffd0     	b	0x1db5c
   1dc18: e3a04004     	mov	r4, #4
   1dc1c: eaffffce     	b	0x1db5c
   1dc20: e1590007     	cmp	r9, r7
   1dc24: 0a000039     	beq	0x1dd10
   1dc28: e0494007     	sub	r4, r9, r7
   1dc2c: e1a01009     	mov	r1, r9
   1dc30: e1a00007     	mov	r0, r7
   1dc34: e3a03000     	mov	r3, #0
   1dc38: e1a02144     	asr	r2, r4, #2
   1dc3c: e2878004     	add	r8, r7, #4
   1dc40: e16f2f12     	clz	r2, r2
   1dc44: e262201f     	rsb	r2, r2, #31
   1dc48: e1a02082     	lsl	r2, r2, #1
   1dc4c: eb004444     	bl	0x2ed64
   1dc50: e3540040     	cmp	r4, #64
   1dc54: ca0000e9     	bgt	0x1e000
   1dc58: e1580009     	cmp	r8, r9
   1dc5c: 0a00012b     	beq	0x1e110
   1dc60: e1a04008     	mov	r4, r8
   1dc64: ea000009     	b	0x1dc90
   1dc68: e1540007     	cmp	r4, r7
   1dc6c: 0a000003     	beq	0x1dc80
   1dc70: e0442007     	sub	r2, r4, r7
   1dc74: e1a01007     	mov	r1, r7
   1dc78: e2870004     	add	r0, r7, #4
   1dc7c: ebffdf58     	bl	0x159e4    @ imm = #-0x82a0 ; memmove
   1dc80: ed878a00     	vstr	s16, [r7]
   1dc84: e2844004     	add	r4, r4, #4
   1dc88: e1540009     	cmp	r4, r9
   1dc8c: 0a00011f     	beq	0x1e110
   1dc90: ed948a00     	vldr	s16, [r4]
   1dc94: edd77a00     	vldr	s15, [r7]
   1dc98: eeb48ae7     	vcmpe.f32	s16, s15
   1dc9c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   1dca0: 4afffff0     	bmi	0x1dc68
   1dca4: ed547a01     	vldr	s15, [r4, #-4]
   1dca8: e2443004     	sub	r3, r4, #4
   1dcac: eeb48ae7     	vcmpe.f32	s16, s15
   1dcb0: eef1fa10     	vmrs	APSR_nzcv, fpscr
   1dcb4: 5a0001d1     	bpl	0x1e400
   1dcb8: e1a02003     	mov	r2, r3
   1dcbc: edc37a01     	vstr	s15, [r3, #4]
   1dcc0: ed737a01     	vldmdb	r3!, {s15}
   1dcc4: eeb48ae7     	vcmpe.f32	s16, s15
   1dcc8: eef1fa10     	vmrs	APSR_nzcv, fpscr
   1dccc: 4afffff9     	bmi	0x1dcb8
   1dcd0: ed828a00     	vstr	s16, [r2]
   1dcd4: eaffffea     	b	0x1dc84
   1dcd8: e1530009     	cmp	r3, r9
   1dcdc: 0a00000b     	beq	0x1dd10
   1dce0: e2832008     	add	r2, r3, #8
   1dce4: e1590002     	cmp	r9, r2
   1dce8: 0a000007     	beq	0x1dd0c
   1dcec: ecf27a01     	vldmia	r2!, {s15}
   1dcf0: ed937a00     	vldr	s14, [r3]
   1dcf4: eeb47a67     	vcmp.f32	s14, s15
   1dcf8: eef1fa10     	vmrs	APSR_nzcv, fpscr
   1dcfc: 1dc37a01     	vstrne	s15, [r3, #4]
   1dd00: 12833004     	addne	r3, r3, #4
   1dd04: e1590002     	cmp	r9, r2
   1dd08: 1afffff7     	bne	0x1dcec
   1dd0c: e2839004     	add	r9, r3, #4
   1dd10: e3a02e1e     	mov	r2, #480
   1dd14: e3a01000     	mov	r1, #0
   1dd18: e1a00006     	mov	r0, r6
   1dd1c: ebffe014     	bl	0x15d74    @ imm = #-0x7fb0 ; memset
   1dd20: e1590007     	cmp	r9, r7
   1dd24: 0a000003     	beq	0x1dd38
   1dd28: e0492007     	sub	r2, r9, r7
   1dd2c: e1a01007     	mov	r1, r7
   1dd30: e1a00006     	mov	r0, r6
   1dd34: ebffe0c2     	bl	0x16044    @ imm = #-0x7cf8 ; memcpy
   1dd38: e3570000     	cmp	r7, #0
   1dd3c: 0a000001     	beq	0x1dd48
   1dd40: e1a00007     	mov	r0, r7
   1dd44: ebffe03d     	bl	0x15e40    @ imm = #-0x7f0c ; _ZdlPv
   1dd48: f2c00010     	vmov.i32	d16, #0x0
   1dd4c: e3a00e7a     	mov	r0, #1952
   1dd50: e3a03000     	mov	r3, #0
   1dd54: e58d3018     	str	r3, [sp, #0x18]
   1dd58: edcd0b04     	vstr	d16, [sp, #16]
   1dd5c: ebffdeea     	bl	0x1590c     @ imm = #-0x8458 ; _Znwj
   1dd60: e59d7010     	ldr	r7, [sp, #0x10]
   1dd64: e1a04000     	mov	r4, r0
   1dd68: e59d2014     	ldr	r2, [sp, #0x14]
   1dd6c: e0422007     	sub	r2, r2, r7
   1dd70: e3520000     	cmp	r2, #0
   1dd74: ca0001a3     	bgt	0x1e408
   1dd78: e3570000     	cmp	r7, #0
   1dd7c: 1a0001a3     	bne	0x1e410
   1dd80: f2c00010     	vmov.i32	d16, #0x0
   1dd84: ee814b90     	vdup.32	d17, r4
   1dd88: e28d2024     	add	r2, sp, #36
   1dd8c: e2840e7a     	add	r0, r4, #1952
   1dd90: e28d1020     	add	r1, sp, #32
   1dd94: e58d0018     	str	r0, [sp, #0x18]
   1dd98: e28d0010     	add	r0, sp, #16
   1dd9c: e3a03000     	mov	r3, #0
   1dda0: e34c3080     	movt	r3, #0xc080
   1dda4: f442078f     	vst1.32	{d16}, [r2]
   1dda8: edcd1b04     	vstr	d17, [sp, #16]
   1ddac: e58d3020     	str	r3, [sp, #0x20]
   1ddb0: e3a03000     	mov	r3, #0
   1ddb4: e58d302c     	str	r3, [sp, #0x2c]
   1ddb8: ebfffde7     	bl	0x1d55c
   1ddbc: e59d8014     	ldr	r8, [sp, #0x14]
   1ddc0: eef18a00     	vmov.f32	s17, #4.000000e+00
   1ddc4: e59d4018     	ldr	r4, [sp, #0x18]
   1ddc8: ed9f9a12     	vldr	s18, [pc, #72]          @ 0x1de18 ; float 4095
   1ddcc: eddf9a12     	vldr	s19, [pc, #72]          @ 0x1de1c ; float 0
   1ddd0: e58d6004     	str	r6, [sp, #0x4]
   1ddd4: ea00001d     	b	0x1de50
   1ddd8: 00 00 00 00  	.word	0x00000000
   1dddc: 19 00 00 00  	.word	0x00000019
   1dde0: 00 00 00 00  	.word	0x00000000
   1dde4: 04 00 00 00  	.word	0x00000004
   1dde8: 40 00 00 00  	.word	0x00000040
   1ddec: 00 00 00 00  	.word	0x00000000
   1ddf0: 00 00 00 00  	.word	0x00000000
   1ddf4: 02 00 00 00  	.word	0x00000002
   1ddf8: 00 00 80 c0  	.word	0xc0800000
   1ddfc: 00 00 00 c0  	.word	0xc0000000
   1de00: 00 00 80 bf  	.word	0xbf800000
   1de04: 00 00 00 bf  	.word	0xbf000000
   1de08: 00 00 00 3f  	.word	0x3f000000
   1de0c: 00 00 80 3f  	.word	0x3f800000
   1de10: 00 00 00 40  	.word	0x40000000
   1de14: 00 00 80 40  	.word	0x40800000
   1de18: 00 f0 7f 45  	.word	0x457ff000
   1de1c: 00 00 00 00  	.word	0x00000000
   1de20: 38 32 07 00  	.word	0x00073238
   1de24: 48 32 07 00  	.word	0x00073248
   1de28: e59d300c     	ldr	r3, [sp, #0xc]
   1de2c: e2888010     	add	r8, r8, #16
   1de30: e59d2004     	ldr	r2, [sp, #0x4]
   1de34: ed088a04     	vstr	s16, [r8, #-16]
   1de38: e508600c     	str	r6, [r8, #-0xc]
   1de3c: e1530002     	cmp	r3, r2
   1de40: ed487a02     	vstr	s15, [r8, #-8]
   1de44: e5087004     	str	r7, [r8, #-0x4]
   1de48: e58d8014     	str	r8, [sp, #0x14]
   1de4c: 0a000037     	beq	0x1df30
   1de50: e59d3004     	ldr	r3, [sp, #0x4]
   1de54: eeb47a00     	vmov.f32	s14, #1.250000e-01
   1de58: e3002fff     	movw	r2, #0xfff
   1de5c: ecb38a01     	vldmia	r3!, {s16}
   1de60: ee787a28     	vadd.f32	s15, s16, s17
   1de64: e58d3004     	str	r3, [sp, #0x4]
   1de68: ee677a87     	vmul.f32	s15, s15, s14
   1de6c: eeb07a69     	vmov.f32	s14, s19
   1de70: eea77a89     	vfma.f32	s14, s15, s18
   1de74: eefd7ac7     	vcvt.s32.f32	s15, s14
   1de78: ee173a90     	vmov	r3, s15
   1de7c: edcd7a02     	vstr	s15, [sp, #8]
   1de80: e2436032     	sub	r6, r3, #50
   1de84: e2837032     	add	r7, r3, #50
   1de88: e1560002     	cmp	r6, r2
   1de8c: a1a06002     	movge	r6, r2
   1de90: e1570002     	cmp	r7, r2
   1de94: a1a07002     	movge	r7, r2
   1de98: e1580004     	cmp	r8, r4
   1de9c: e1c66fc6     	bic	r6, r6, r6, asr #31
   1dea0: e1c77fc7     	bic	r7, r7, r7, asr #31
   1dea4: 1affffdf     	bne	0x1de28
   1dea8: e59da010     	ldr	r10, [sp, #0x10]
   1deac: e048900a     	sub	r9, r8, r10
   1deb0: e1a04249     	asr	r4, r9, #4
   1deb4: e374037e     	cmn	r4, #-134217727
   1deb8: 0a00015d     	beq	0x1e434
   1debc: e3540000     	cmp	r4, #0
   1dec0: 0a00007c     	beq	0x1e0b8
   1dec4: e1540084     	cmp	r4, r4, lsl #1
   1dec8: e1a04084     	lsl	r4, r4, #1
   1decc: 83e0413e     	mvnhi	r4, #-2147483633
   1ded0: 9a000070     	bls	0x1e098
   1ded4: e1a00004     	mov	r0, r4
   1ded8: ebffde8b     	bl	0x1590c     @ imm = #-0x85d4 ; _Znwj
   1dedc: e1a0b000     	mov	r11, r0
   1dee0: e0804004     	add	r4, r0, r4
   1dee4: e08b2009     	add	r2, r11, r9
   1dee8: e2893010     	add	r3, r9, #16
   1deec: e08b8003     	add	r8, r11, r3
   1def0: e59d3008     	ldr	r3, [sp, #0x8]
   1def4: e3590000     	cmp	r9, #0
   1def8: e5826004     	str	r6, [r2, #0x4]
   1defc: e5823008     	str	r3, [r2, #0x8]
   1df00: e582700c     	str	r7, [r2, #0xc]
   1df04: ed828a00     	vstr	s16, [r2]
   1df08: ca00005b     	bgt	0x1e07c
   1df0c: e35a0000     	cmp	r10, #0
   1df10: 1a00005d     	bne	0x1e08c
   1df14: e59d300c     	ldr	r3, [sp, #0xc]
   1df18: e59d2004     	ldr	r2, [sp, #0x4]
   1df1c: e58db010     	str	r11, [sp, #0x10]
   1df20: e1530002     	cmp	r3, r2
   1df24: e58d8014     	str	r8, [sp, #0x14]
   1df28: e58d4018     	str	r4, [sp, #0x18]
   1df2c: 1affffc7     	bne	0x1de50
   1df30: e51f3118     	ldr	r3, [pc, #-0x118]       @ 0x1de20
   1df34: e28dc020     	add	r12, sp, #32
   1df38: e893000f     	ldm	r3, {r0, r1, r2, r3}
   1df3c: e88c000f     	stm	r12, {r0, r1, r2, r3}
   1df40: e28d0010     	add	r0, sp, #16
   1df44: e1a0100c     	mov	r1, r12
   1df48: ebfffd83     	bl	0x1d55c
   1df4c: e59d4010     	ldr	r4, [sp, #0x10]
   1df50: e59d6014     	ldr	r6, [sp, #0x14]
   1df54: e1540006     	cmp	r4, r6
   1df58: 0a000079     	beq	0x1e144
   1df5c: e0467004     	sub	r7, r6, r4
   1df60: e1a01006     	mov	r1, r6
   1df64: e1a00004     	mov	r0, r4
   1df68: e3a03000     	mov	r3, #0
   1df6c: e1a02247     	asr	r2, r7, #4
   1df70: e16f2f12     	clz	r2, r2
   1df74: e262201f     	rsb	r2, r2, #31
   1df78: e1a02082     	lsl	r2, r2, #1
   1df7c: ebfffe12     	bl	0x1d7cc
   1df80: e3570c01     	cmp	r7, #256
   1df84: da0000e1     	ble	0x1e310
   1df88: e2847c01     	add	r7, r4, #256
   1df8c: e1a00004     	mov	r0, r4
   1df90: e1a01007     	mov	r1, r7
   1df94: ebfffdd8     	bl	0x1d6fc
   1df98: e1a0e007     	mov	lr, r7
   1df9c: e156000e     	cmp	r6, lr
   1dfa0: e1a0700e     	mov	r7, lr
   1dfa4: 0a0000dc     	beq	0x1e31c
   1dfa8: e28dc020     	add	r12, sp, #32
   1dfac: e89e000f     	ldm	lr, {r0, r1, r2, r3}
   1dfb0: e88c000f     	stm	r12, {r0, r1, r2, r3}
   1dfb4: e59e8008     	ldr	r8, [lr, #0x8]
   1dfb8: e51e3008     	ldr	r3, [lr, #-0x8]
   1dfbc: e1580003     	cmp	r8, r3
   1dfc0: aa000008     	bge	0x1dfe8
   1dfc4: e24ec010     	sub	r12, lr, #16
   1dfc8: e28c4010     	add	r4, r12, #16
   1dfcc: e1a0700c     	mov	r7, r12
   1dfd0: e89c000f     	ldm	r12, {r0, r1, r2, r3}
   1dfd4: e24cc010     	sub	r12, r12, #16
   1dfd8: e884000f     	stm	r4, {r0, r1, r2, r3}
   1dfdc: e59c3008     	ldr	r3, [r12, #0x8]
   1dfe0: e1530008     	cmp	r3, r8
   1dfe4: cafffff7     	bgt	0x1dfc8
   1dfe8: e28d3020     	add	r3, sp, #32
   1dfec: e58d8028     	str	r8, [sp, #0x28]
   1dff0: e28ee010     	add	lr, lr, #16
   1dff4: e893000f     	ldm	r3, {r0, r1, r2, r3}
   1dff8: e887000f     	stm	r7, {r0, r1, r2, r3}
   1dffc: eaffffe6     	b	0x1df9c
   1e000: e287a040     	add	r10, r7, #64
   1e004: e1a04008     	mov	r4, r8
   1e008: ea000009     	b	0x1e034
   1e00c: e1540007     	cmp	r4, r7
   1e010: 0a000003     	beq	0x1e024
   1e014: e0442007     	sub	r2, r4, r7
   1e018: e1a01007     	mov	r1, r7
   1e01c: e2870004     	add	r0, r7, #4
   1e020: ebffde6f     	bl	0x159e4    @ imm = #-0x8644 ; memmove
   1e024: ed878a00     	vstr	s16, [r7]
   1e028: e2844004     	add	r4, r4, #4
   1e02c: e15a0004     	cmp	r10, r4
   1e030: 0a000022     	beq	0x1e0c0
   1e034: ed948a00     	vldr	s16, [r4]
   1e038: edd77a00     	vldr	s15, [r7]
   1e03c: eeb48ae7     	vcmpe.f32	s16, s15
   1e040: eef1fa10     	vmrs	APSR_nzcv, fpscr
   1e044: 4afffff0     	bmi	0x1e00c
   1e048: ed547a01     	vldr	s15, [r4, #-4]
   1e04c: e2443004     	sub	r3, r4, #4
   1e050: eeb48ae7     	vcmpe.f32	s16, s15
   1e054: eef1fa10     	vmrs	APSR_nzcv, fpscr
   1e058: 5a0000e6     	bpl	0x1e3f8
   1e05c: e1a02003     	mov	r2, r3
   1e060: edc37a01     	vstr	s15, [r3, #4]
   1e064: ed737a01     	vldmdb	r3!, {s15}
   1e068: eeb48ae7     	vcmpe.f32	s16, s15
   1e06c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   1e070: 4afffff9     	bmi	0x1e05c
   1e074: ed828a00     	vstr	s16, [r2]
   1e078: eaffffea     	b	0x1e028
   1e07c: e1a02009     	mov	r2, r9
   1e080: e1a0100a     	mov	r1, r10
   1e084: e1a0000b     	mov	r0, r11
   1e088: ebffde55     	bl	0x159e4    @ imm = #-0x86ac ; memmove
   1e08c: e1a0000a     	mov	r0, r10
   1e090: ebffdf6a     	bl	0x15e40    @ imm = #-0x8258 ; _ZdlPv
   1e094: eaffff9e     	b	0x1df14
   1e098: e3540000     	cmp	r4, #0
   1e09c: 01a0b004     	moveq	r11, r4
   1e0a0: 0affff8f     	beq	0x1dee4
   1e0a4: e3e0333e     	mvn	r3, #-134217728
   1e0a8: e1540003     	cmp	r4, r3
   1e0ac: 21a04003     	movhs	r4, r3
   1e0b0: e1a04204     	lsl	r4, r4, #4
   1e0b4: eaffff86     	b	0x1ded4
   1e0b8: e3a04010     	mov	r4, #16
   1e0bc: eaffff84     	b	0x1ded4
   1e0c0: e15a0009     	cmp	r10, r9
   1e0c4: 0a000011     	beq	0x1e110
   1e0c8: e1a0300a     	mov	r3, r10
   1e0cc: e287103c     	add	r1, r7, #60
   1e0d0: e1a00003     	mov	r0, r3
   1e0d4: e1a02001     	mov	r2, r1
   1e0d8: ecb37a01     	vldmia	r3!, {s14}
   1e0dc: ecf17a01     	vldmia	r1!, {s15}
   1e0e0: eeb47ae7     	vcmpe.f32	s14, s15
   1e0e4: eef1fa10     	vmrs	APSR_nzcv, fpscr
   1e0e8: 5a000005     	bpl	0x1e104
   1e0ec: e1a00002     	mov	r0, r2
   1e0f0: edc27a01     	vstr	s15, [r2, #4]
   1e0f4: ed727a01     	vldmdb	r2!, {s15}
   1e0f8: eeb47ae7     	vcmpe.f32	s14, s15
   1e0fc: eef1fa10     	vmrs	APSR_nzcv, fpscr
   1e100: 4afffff9     	bmi	0x1e0ec
   1e104: e1590003     	cmp	r9, r3
   1e108: ed807a00     	vstr	s14, [r0]
   1e10c: 1affffef     	bne	0x1e0d0
   1e110: e1a02008     	mov	r2, r8
   1e114: e1a01007     	mov	r1, r7
   1e118: ea000005     	b	0x1e134
   1e11c: edd27a00     	vldr	s15, [r2]
   1e120: e2822004     	add	r2, r2, #4
   1e124: ecb17a01     	vldmia	r1!, {s14}
   1e128: eeb47a67     	vcmp.f32	s14, s15
   1e12c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   1e130: 0afffee8     	beq	0x1dcd8
   1e134: e1a03001     	mov	r3, r1
   1e138: e1520009     	cmp	r2, r9
   1e13c: 1afffff6     	bne	0x1e11c
   1e140: eafffef2     	b	0x1dd10
   1e144: e0466004     	sub	r6, r6, r4
   1e148: e1a03246     	asr	r3, r6, #4
   1e14c: e3530001     	cmp	r3, #1
   1e150: 0a00004c     	beq	0x1e288
   1e154: e2446010     	sub	r6, r4, #16
   1e158: e1a01004     	mov	r1, r4
   1e15c: e0866203     	add	r6, r6, r3, lsl #4
   1e160: e1a03004     	mov	r3, r4
   1e164: e593000c     	ldr	r0, [r3, #0xc]
   1e168: e5932014     	ldr	r2, [r3, #0x14]
   1e16c: e1500002     	cmp	r0, r2
   1e170: da000008     	ble	0x1e198
   1e174: e593c008     	ldr	r12, [r3, #0x8]
   1e178: e5930018     	ldr	r0, [r3, #0x18]
   1e17c: e040200c     	sub	r2, r0, r12
   1e180: e0822fa2     	add	r2, r2, r2, lsr #31
   1e184: e1a020c2     	asr	r2, r2, #1
   1e188: e082c00c     	add	r12, r2, r12
   1e18c: e0402002     	sub	r2, r0, r2
   1e190: e583c00c     	str	r12, [r3, #0xc]
   1e194: e5832014     	str	r2, [r3, #0x14]
   1e198: e2833010     	add	r3, r3, #16
   1e19c: e1530006     	cmp	r3, r6
   1e1a0: 1affffef     	bne	0x1e164
   1e1a4: e5913004     	ldr	r3, [r1, #0x4]
   1e1a8: e591200c     	ldr	r2, [r1, #0xc]
   1e1ac: e1530002     	cmp	r3, r2
   1e1b0: aa00001c     	bge	0x1e228
   1e1b4: e042e003     	sub	lr, r2, r3
   1e1b8: e591c000     	ldr	r12, [r1]
   1e1bc: e24e0001     	sub	r0, lr, #1
   1e1c0: e3500002     	cmp	r0, #2
   1e1c4: 9a00000c     	bls	0x1e1fc
   1e1c8: e2830089     	add	r0, r3, #137
   1e1cc: eea0cb90     	vdup.32	q8, r12
   1e1d0: e1a0812e     	lsr	r8, lr, #2
   1e1d4: e3a07000     	mov	r7, #0
   1e1d8: e0850100     	add	r0, r5, r0, lsl #2
   1e1dc: e2877001     	add	r7, r7, #1
   1e1e0: f4400a8d     	vst1.32	{d16, d17}, [r0]!
   1e1e4: e1570008     	cmp	r7, r8
   1e1e8: 1afffffb     	bne	0x1e1dc
   1e1ec: e3ce0003     	bic	r0, lr, #3
   1e1f0: e0833000     	add	r3, r3, r0
   1e1f4: e15e0000     	cmp	lr, r0
   1e1f8: 0a00000a     	beq	0x1e228
   1e1fc: e085e103     	add	lr, r5, r3, lsl #2
   1e200: e2830001     	add	r0, r3, #1
   1e204: e1520000     	cmp	r2, r0
   1e208: e58ec224     	str	r12, [lr, #0x224]
   1e20c: da000005     	ble	0x1e228
   1e210: e0850100     	add	r0, r5, r0, lsl #2
   1e214: e2833002     	add	r3, r3, #2
   1e218: e1520003     	cmp	r2, r3
   1e21c: e580c224     	str	r12, [r0, #0x224]
   1e220: c0853103     	addgt	r3, r5, r3, lsl #2
   1e224: c583c224     	strgt	r12, [r3, #0x224]
   1e228: e5913014     	ldr	r3, [r1, #0x14]
   1e22c: e1530002     	cmp	r3, r2
   1e230: da000011     	ble	0x1e27c
   1e234: ed917a00     	vldr	s14, [r1]
   1e238: e0433002     	sub	r3, r3, r2
   1e23c: edd17a04     	vldr	s15, [r1, #16]
   1e240: ee063a90     	vmov	s13, r3
   1e244: e2822089     	add	r2, r2, #137
   1e248: e3a00000     	mov	r0, #0
   1e24c: eef86ae6     	vcvt.f32.s32	s13, s13
   1e250: ee777ac7     	vsub.f32	s15, s15, s14
   1e254: e0852102     	add	r2, r5, r2, lsl #2
   1e258: ee060a10     	vmov	s12, r0
   1e25c: e2800001     	add	r0, r0, #1
   1e260: e1500003     	cmp	r0, r3
   1e264: eef85ac6     	vcvt.f32.s32	s11, s12
   1e268: ee856aa6     	vdiv.f32	s12, s11, s13
   1e26c: eef05a47     	vmov.f32	s11, s14
   1e270: eee65a27     	vfma.f32	s11, s12, s15
   1e274: ece25a01     	vstmia	r2!, {s11}
   1e278: 1afffff6     	bne	0x1e258
   1e27c: e2811010     	add	r1, r1, #16
   1e280: e1510006     	cmp	r1, r6
   1e284: 1affffc6     	bne	0x1e1a4
   1e288: e3540000     	cmp	r4, #0
   1e28c: 0a000001     	beq	0x1e298
   1e290: e1a00004     	mov	r0, r4
   1e294: ebffdee9     	bl	0x15e40    @ imm = #-0x845c ; _ZdlPv
   1e298: e51fe47c     	ldr	lr, [pc, #-0x47c]       @ 0x1de24
   1e29c: e2854901     	add	r4, r5, #16384
   1e2a0: e284cf8a     	add	r12, r4, #552
   1e2a4: e2849f9b     	add	r9, r4, #620
   1e2a8: eddf4b72     	vldr	d20, [pc, #456]         @ 0x1e478 ; float 3.05175853327e-05
   1e2ac: eddf5b73     	vldr	d21, [pc, #460]         @ 0x1e480 ; float 4.65661395381e-10
   1e2b0: e2848f9f     	add	r8, r4, #636
   1e2b4: e8be000f     	ldm	lr!, {r0, r1, r2, r3}
   1e2b8: e8ac000f     	stm	r12!, {r0, r1, r2, r3}
   1e2bc: e2847fa3     	add	r7, r4, #652
   1e2c0: eddf2b70     	vldr	d18, [pc, #448]         @ 0x1e488 ; float 1.3411048233e-08
   1e2c4: eddf3b71     	vldr	d19, [pc, #452]         @ 0x1e490 ; float 1.34110464955e-08
   1e2c8: e3a06003     	mov	r6, #3
   1e2cc: eddf0b71     	vldr	d16, [pc, #452]         @ 0x1e498 ; float 3.43322837737e-06
   1e2d0: eddf1b72     	vldr	d17, [pc, #456]         @ 0x1e4a0 ; float 0.000878906470662
   1e2d4: e5846224     	str	r6, [r4, #0x224]
   1e2d8: e8be000f     	ldm	lr!, {r0, r1, r2, r3}
   1e2dc: e8ac000f     	stm	r12!, {r0, r1, r2, r3}
   1e2e0: e8be000f     	ldm	lr!, {r0, r1, r2, r3}
   1e2e4: e8ac000f     	stm	r12!, {r0, r1, r2, r3}
   1e2e8: e89e000f     	ldm	lr, {r0, r1, r2, r3}
   1e2ec: e88c000f     	stm	r12, {r0, r1, r2, r3}
   1e2f0: e1a00005     	mov	r0, r5
   1e2f4: e5846268     	str	r6, [r4, #0x268]
   1e2f8: f4494a8f     	vst1.32	{d20, d21}, [r9]
   1e2fc: f4482a8f     	vst1.32	{d18, d19}, [r8]
   1e300: f4470a8f     	vst1.32	{d16, d17}, [r7]
   1e304: e28dd034     	add	sp, sp, #52
   1e308: ecbd8b04     	vpop	{d8, d9}
   1e30c: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
   1e310: e1a01006     	mov	r1, r6
   1e314: e1a00004     	mov	r0, r4
   1e318: ebfffcf7     	bl	0x1d6fc
   1e31c: e59d4010     	ldr	r4, [sp, #0x10]
   1e320: e59d6014     	ldr	r6, [sp, #0x14]
   1e324: e1540006     	cmp	r4, r6
   1e328: 0affff85     	beq	0x1e144
   1e32c: e2843010     	add	r3, r4, #16
   1e330: e1560003     	cmp	r6, r3
   1e334: 0affff82     	beq	0x1e144
   1e338: ed947a00     	vldr	s14, [r4]
   1e33c: e1a03004     	mov	r3, r4
   1e340: edd37a04     	vldr	s15, [r3, #16]
   1e344: e1a07003     	mov	r7, r3
   1e348: eeb47a67     	vcmp.f32	s14, s15
   1e34c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   1e350: 1a000003     	bne	0x1e364
   1e354: e5931008     	ldr	r1, [r3, #0x8]
   1e358: e5932018     	ldr	r2, [r3, #0x18]
   1e35c: e1510002     	cmp	r1, r2
   1e360: 0a000005     	beq	0x1e37c
   1e364: e2832020     	add	r2, r3, #32
   1e368: e2833010     	add	r3, r3, #16
   1e36c: e1560002     	cmp	r6, r2
   1e370: 0affff73     	beq	0x1e144
   1e374: eeb07a67     	vmov.f32	s14, s15
   1e378: eafffff0     	b	0x1e340
   1e37c: e1560003     	cmp	r6, r3
   1e380: 0affff6f     	beq	0x1e144
   1e384: e2833020     	add	r3, r3, #32
   1e388: e1560003     	cmp	r6, r3
   1e38c: 0a000010     	beq	0x1e3d4
   1e390: e287c030     	add	r12, r7, #48
   1e394: ed977a00     	vldr	s14, [r7]
   1e398: ed5c7a04     	vldr	s15, [r12, #-16]
   1e39c: eeb47a67     	vcmp.f32	s14, s15
   1e3a0: eef1fa10     	vmrs	APSR_nzcv, fpscr
   1e3a4: 1a000003     	bne	0x1e3b8
   1e3a8: e5972008     	ldr	r2, [r7, #0x8]
   1e3ac: e51c3008     	ldr	r3, [r12, #-0x8]
   1e3b0: e1520003     	cmp	r2, r3
   1e3b4: 0a000003     	beq	0x1e3c8
   1e3b8: e287e010     	add	lr, r7, #16
   1e3bc: e91c000f     	ldmdb	r12, {r0, r1, r2, r3}
   1e3c0: e1a0700e     	mov	r7, lr
   1e3c4: e88e000f     	stm	lr, {r0, r1, r2, r3}
   1e3c8: e156000c     	cmp	r6, r12
   1e3cc: e28cc010     	add	r12, r12, #16
   1e3d0: 1affffef     	bne	0x1e394
   1e3d4: e2877010     	add	r7, r7, #16
   1e3d8: e1560007     	cmp	r6, r7
   1e3dc: 11a06007     	movne	r6, r7
   1e3e0: 158d7014     	strne	r7, [sp, #0x14]
   1e3e4: e0466004     	sub	r6, r6, r4
   1e3e8: e1a03246     	asr	r3, r6, #4
   1e3ec: e3530001     	cmp	r3, #1
   1e3f0: 1affff57     	bne	0x1e154
   1e3f4: eaffffa5     	b	0x1e290
   1e3f8: e1a02004     	mov	r2, r4
   1e3fc: eaffff1c     	b	0x1e074
   1e400: e1a02004     	mov	r2, r4
   1e404: eafffe31     	b	0x1dcd0
   1e408: e1a01007     	mov	r1, r7
   1e40c: ebffdd74     	bl	0x159e4    @ imm = #-0x8a30 ; memmove
   1e410: e1a00007     	mov	r0, r7
   1e414: ebffde89     	bl	0x15e40    @ imm = #-0x85dc ; _ZdlPv
   1e418: eafffe58     	b	0x1dd80
   1e41c: e3000c8c     	movw	r0, #0xc8c
   1e420: e3400007     	movt	r0, #0x7
   1e424: ebffdde3     	bl	0x15bb8    @ imm = #-0x8874 ; _ZSt20__throw_length_errorPKc
   1e428: e3000c8c     	movw	r0, #0xc8c
   1e42c: e3400007     	movt	r0, #0x7
   1e430: ebffdde0     	bl	0x15bb8    @ imm = #-0x8880 ; _ZSt20__throw_length_errorPKc
   1e434: e3000c8c     	movw	r0, #0xc8c
   1e438: e3400007     	movt	r0, #0x7
   1e43c: ebffdddd     	bl	0x15bb8    @ imm = #-0x888c ; _ZSt20__throw_length_errorPKc
   1e440: e59d0010     	ldr	r0, [sp, #0x10]
   1e444: e3500000     	cmp	r0, #0
   1e448: 0a000000     	beq	0x1e450
   1e44c: ebffde7b     	bl	0x15e40    @ imm = #-0x8614 ; _ZdlPv
   1e450: ebffdec2     	bl	0x15f60    @ imm = #-0x84f8 ; __cxa_end_cleanup
   1e454: e1a0a007     	mov	r10, r7
   1e458: e35a0000     	cmp	r10, #0
   1e45c: 0afffffb     	beq	0x1e450
   1e460: e1a0000a     	mov	r0, r10
   1e464: ebffde75     	bl	0x15e40    @ imm = #-0x862c ; _ZdlPv
   1e468: eafffff8     	b	0x1e450
   1e46c: eafffff8     	b	0x1e454
   1e470: eafffff8     	b	0x1e458
   1e474: e320f000     	nop
   1e478: 66 66 66 3f  	.word	0x3f666666
   1e47c: 00 00 00 3f  	.word	0x3f000000
   1e480: cd cc 4c 3e  	.word	0x3e4ccccd
   1e484: 00 00 00 3e  	.word	0x3e000000
   1e488: 9a 99 99 3e  	.word	0x3e99999a
   1e48c: cd cc 4c 3e  	.word	0x3e4ccccd
   1e490: 00 00 00 00  	.word	0x00000000
   1e494: cd cc 4c 3e  	.word	0x3e4ccccd
   1e498: 00 00 c8 42  	.word	0x42c80000
   1e49c: cd cc cc 3e  	.word	0x3ecccccd
   1e4a0: 00 40 1c 46  	.word	0x461c4000
   1e4a4: cd cc 4c 3f  	.word	0x3f4ccccd
