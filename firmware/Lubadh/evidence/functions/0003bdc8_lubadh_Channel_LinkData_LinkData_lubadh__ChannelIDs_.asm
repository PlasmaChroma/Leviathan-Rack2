; lubadh::Channel::LinkData::LinkData(lubadh::_ChannelIDs)
; VA 0x3bdc8 size 612

   3bdc8: e59f3250     	ldr	r3, [pc, #0x250]        @ 0x3c020
   3bdcc: e3a02018     	mov	r2, #24
   3bdd0: f2c72f50     	vmov.f32	q9, #1.000000e+00
   3bdd4: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   3bdd8: e1a04000     	mov	r4, r0
   3bddc: eddf0b8b     	vldr	d16, [pc, #556]         @ 0x3c010 ; float 4.24399158242e-314
   3bde0: eddf1b8c     	vldr	d17, [pc, #560]         @ 0x3c018 ; float 8.48798316534e-314
   3bde4: e2800014     	add	r0, r0, #20
   3bde8: e24dd024     	sub	sp, sp, #36
   3bdec: e0223192     	mla	r2, r2, r1, r3
   3bdf0: e3a0c5fe     	mov	r12, #1065353216
   3bdf4: e2831030     	add	r1, r3, #48
   3bdf8: e584c010     	str	r12, [r4, #0x10]
   3bdfc: e3a03005     	mov	r3, #5
   3be00: f4442a8f     	vst1.32	{d18, d19}, [r4]
   3be04: e2846030     	add	r6, r4, #48
   3be08: f4400a8f     	vst1.32	{d16, d17}, [r0]
   3be0c: e3a00006     	mov	r0, #6
   3be10: e5843024     	str	r3, [r4, #0x24]
   3be14: e3a03007     	mov	r3, #7
   3be18: e5840028     	str	r0, [r4, #0x28]
   3be1c: e28d0004     	add	r0, sp, #4
   3be20: e584302c     	str	r3, [r4, #0x2c]
   3be24: ebffcb54     	bl	0x2eb7c
   3be28: e1a00006     	mov	r0, r6
   3be2c: e28d1004     	add	r1, sp, #4
   3be30: eb0017f5     	bl	0x41e0c
   3be34: e59d0004     	ldr	r0, [sp, #0x4]
   3be38: e28d300c     	add	r3, sp, #12
   3be3c: e59f71e0     	ldr	r7, [pc, #0x1e0]        @ 0x3c024
   3be40: e1500003     	cmp	r0, r3
   3be44: e5847030     	str	r7, [r4, #0x30]
   3be48: 0a000000     	beq	0x3be50
   3be4c: ebff67fb     	bl	0x15e40    @ imm = #-0x26014 ; _ZdlPv
   3be50: e59fe1d0     	ldr	lr, [pc, #0x1d0]        @ 0x3c028
   3be54: e28dc004     	add	r12, sp, #4
   3be58: f2c01011     	vmov.i32	d17, #0x1
   3be5c: f2c00010     	vmov.i32	d16, #0x0
   3be60: e284505c     	add	r5, r4, #92
   3be64: e3a08000     	mov	r8, #0
   3be68: e30c9ccd     	movw	r9, #0xcccd
   3be6c: e3439d4c     	movt	r9, #0x3d4c
   3be70: e8be000f     	ldm	lr!, {r0, r1, r2, r3}
   3be74: e8ac000f     	stm	r12!, {r0, r1, r2, r3}
   3be78: e2843070     	add	r3, r4, #112
   3be7c: e5c48058     	strb	r8, [r4, #0x58]
   3be80: f445178f     	vst1.32	{d17}, [r5]
   3be84: e3a05001     	mov	r5, #1
   3be88: e5849068     	str	r9, [r4, #0x68]
   3be8c: e89e0007     	ldm	lr, {r0, r1, r2}
   3be90: e88c0007     	stm	r12, {r0, r1, r2}
   3be94: e3a0001c     	mov	r0, #28
   3be98: e5845064     	str	r5, [r4, #0x64]
   3be9c: f443078f     	vst1.32	{d16}, [r3]
   3bea0: e5848078     	str	r8, [r4, #0x78]
   3bea4: ebff6698     	bl	0x1590c     @ imm = #-0x265a0 ; _Znwj
   3bea8: e28dc004     	add	r12, sp, #4
   3beac: e1a0e000     	mov	lr, r0
   3beb0: e5840070     	str	r0, [r4, #0x70]
   3beb4: e30552a8     	movw	r5, #0x52a8
   3beb8: e3405009     	movt	r5, #0x9
   3bebc: e284808c     	add	r8, r4, #140
   3bec0: e8bc000f     	ldm	r12!, {r0, r1, r2, r3}
   3bec4: e58e0000     	str	r0, [lr]
   3bec8: e58e1004     	str	r1, [lr, #0x4]
   3becc: e28e901c     	add	r9, lr, #28
   3bed0: e58e2008     	str	r2, [lr, #0x8]
   3bed4: e3a0a000     	mov	r10, #0
   3bed8: f2c00050     	vmov.i32	q8, #0x0
   3bedc: e58e300c     	str	r3, [lr, #0xc]
   3bee0: e5849078     	str	r9, [r4, #0x78]
   3bee4: e3a03000     	mov	r3, #0
   3bee8: e8bc0007     	ldm	r12!, {r0, r1, r2}
   3beec: e58e2018     	str	r2, [lr, #0x18]
   3bef0: e5952040     	ldr	r2, [r5, #0x40]
   3bef4: e306b666     	movw	r11, #0x6666
   3bef8: e343bf66     	movt	r11, #0x3f66
   3befc: e58e0010     	str	r0, [lr, #0x10]
   3bf00: e2422003     	sub	r2, r2, #3
   3bf04: e3a00002     	mov	r0, #2
   3bf08: e16f2f12     	clz	r2, r2
   3bf0c: e5840084     	str	r0, [r4, #0x84]
   3bf10: e58e1014     	str	r1, [lr, #0x14]
   3bf14: e3a01000     	mov	r1, #0
   3bf18: e3441248     	movt	r1, #0x4248
   3bf1c: e1a022a2     	lsr	r2, r2, #5
   3bf20: e5849074     	str	r9, [r4, #0x74]
   3bf24: e3a00e1e     	mov	r0, #480
   3bf28: e5c4307c     	strb	r3, [r4, #0x7c]
   3bf2c: e1c438b8     	strh	r3, [r4, #136]
   3bf30: f4480a8f     	vst1.32	{d16, d17}, [r8]
   3bf34: e5c420c0     	strb	r2, [r4, #0xc0]
   3bf38: e584309c     	str	r3, [r4, #0x9c]
   3bf3c: e58430a0     	str	r3, [r4, #0xa0]
   3bf40: e58450b0     	str	r5, [r4, #0xb0]
   3bf44: e58430b4     	str	r3, [r4, #0xb4]
   3bf48: e58430b8     	str	r3, [r4, #0xb8]
   3bf4c: e58430bc     	str	r3, [r4, #0xbc]
   3bf50: e584b0a4     	str	r11, [r4, #0xa4]
   3bf54: e584a0a8     	str	r10, [r4, #0xa8]
   3bf58: e58410ac     	str	r1, [r4, #0xac]
   3bf5c: e5942034     	ldr	r2, [r4, #0x34]
   3bf60: e5823000     	str	r3, [r2]
   3bf64: ebff6668     	bl	0x1590c     @ imm = #-0x26660 ; _Znwj
   3bf68: e59460b4     	ldr	r6, [r4, #0xb4]
   3bf6c: e1a05000     	mov	r5, r0
   3bf70: e59420b8     	ldr	r2, [r4, #0xb8]
   3bf74: e0422006     	sub	r2, r2, r6
   3bf78: e3520000     	cmp	r2, #0
   3bf7c: ca000009     	bgt	0x3bfa8
   3bf80: e3560000     	cmp	r6, #0
   3bf84: 1a000009     	bne	0x3bfb0
   3bf88: ee805b90     	vdup.32	d16, r5
   3bf8c: e28430b4     	add	r3, r4, #180
   3bf90: e2855e1e     	add	r5, r5, #480
   3bf94: e1a00004     	mov	r0, r4
   3bf98: f443078f     	vst1.32	{d16}, [r3]
   3bf9c: e58450bc     	str	r5, [r4, #0xbc]
   3bfa0: e28dd024     	add	sp, sp, #36
   3bfa4: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
   3bfa8: e1a01006     	mov	r1, r6
   3bfac: ebff668c     	bl	0x159e4    @ imm = #-0x265d0 ; memmove
   3bfb0: e1a00006     	mov	r0, r6
   3bfb4: ebff67a1     	bl	0x15e40    @ imm = #-0x2617c ; _ZdlPv
   3bfb8: eafffff2     	b	0x3bf88
   3bfbc: e59400b4     	ldr	r0, [r4, #0xb4]
   3bfc0: e3500000     	cmp	r0, #0
   3bfc4: 0a000000     	beq	0x3bfcc
   3bfc8: ebff679c     	bl	0x15e40    @ imm = #-0x26190 ; _ZdlPv
   3bfcc: e5940070     	ldr	r0, [r4, #0x70]
   3bfd0: e3500000     	cmp	r0, #0
   3bfd4: 1a00000a     	bne	0x3c004
   3bfd8: e1a00006     	mov	r0, r6
   3bfdc: e5847030     	str	r7, [r4, #0x30]
   3bfe0: ebffcc75     	bl	0x2f1bc
   3bfe4: ebff67dd     	bl	0x15f60    @ imm = #-0x2608c ; __cxa_end_cleanup
   3bfe8: eafffff7     	b	0x3bfcc
   3bfec: e59d0004     	ldr	r0, [sp, #0x4]
   3bff0: e28d300c     	add	r3, sp, #12
   3bff4: e1500003     	cmp	r0, r3
   3bff8: 0afffff9     	beq	0x3bfe4
   3bffc: ebff678f     	bl	0x15e40    @ imm = #-0x261c4 ; _ZdlPv
   3c000: eafffff7     	b	0x3bfe4
   3c004: ebff678d     	bl	0x15e40    @ imm = #-0x261cc ; _ZdlPv
   3c008: eafffff2     	b	0x3bfd8
   3c00c: e320f000     	nop
   3c010: 01 00 00 00  	.word	0x00000001
   3c014: 02 00 00 00  	.word	0x00000002
   3c018: 03 00 00 00  	.word	0x00000003
   3c01c: 04 00 00 00  	.word	0x00000004
   3c020: 08 4e 09 00  	.word	0x00094e08
   3c024: e0 1e 07 00  	.word	0x00071ee0
   3c028: f8 2f 07 00  	.word	0x00072ff8
