; lubadh::Application::setup()
; VA 0x2b03c size 508

   2b03c: e92d40f0     	push	{r4, r5, r6, r7, lr}
   2b040: e2805915     	add	r5, r0, #344064
   2b044: e3a06000     	mov	r6, #0
   2b048: ed2d8b02     	vpush	{d8}
   2b04c: e24dd01c     	sub	sp, sp, #28
   2b050: e1a04000     	mov	r4, r0
   2b054: e3011124     	movw	r1, #0x1124
   2b058: e3401007     	movt	r1, #0x7
   2b05c: e1a0000d     	mov	r0, sp
   2b060: e5c56abc     	strb	r6, [r5, #0xabc]
   2b064: e5856ac0     	str	r6, [r5, #0xac0]
   2b068: ebfff2ce     	bl	0x27ba8
   2b06c: e3090fec     	movw	r0, #0x9fec
   2b070: e3400009     	movt	r0, #0x9
   2b074: e1a02006     	mov	r2, r6
   2b078: e1a0100d     	mov	r1, sp
   2b07c: eb0113a7     	bl	0x6ff20
   2b080: e59d0000     	ldr	r0, [sp]
   2b084: e28d7008     	add	r7, sp, #8
   2b088: e1500007     	cmp	r0, r7
   2b08c: 0a000000     	beq	0x2b094
   2b090: ebffab6a     	bl	0x15e40    @ imm = #-0x15258 ; _ZdlPv
   2b094: e1a00004     	mov	r0, r4
   2b098: ebfff4c1     	bl	0x283a4
   2b09c: e594314c     	ldr	r3, [r4, #0x14c]
   2b0a0: e2433001     	sub	r3, r3, #1
   2b0a4: e3530001     	cmp	r3, #1
   2b0a8: 9a000041     	bls	0x2b1b4
   2b0ac: e3a0100c     	mov	r1, #12
   2b0b0: e1a00004     	mov	r0, r4
   2b0b4: eb010402     	bl	0x6c0c4
   2b0b8: e2506000     	subs	r6, r0, #0
   2b0bc: 1a00002d     	bne	0x2b178
   2b0c0: e1a0000d     	mov	r0, sp
   2b0c4: e3011140     	movw	r1, #0x1140
   2b0c8: e3401007     	movt	r1, #0x7
   2b0cc: ebfff2b5     	bl	0x27ba8
   2b0d0: e3090fec     	movw	r0, #0x9fec
   2b0d4: e3400009     	movt	r0, #0x9
   2b0d8: e1a02006     	mov	r2, r6
   2b0dc: e1a0100d     	mov	r1, sp
   2b0e0: eb01138e     	bl	0x6ff20
   2b0e4: e59d0000     	ldr	r0, [sp]
   2b0e8: e1500007     	cmp	r0, r7
   2b0ec: 0a000000     	beq	0x2b0f4
   2b0f0: ebffab52     	bl	0x15e40    @ imm = #-0x152b8 ; _ZdlPv
   2b0f4: eeb78a00     	vmov.f32	s16, #1.000000e+00
   2b0f8: e3a000a0     	mov	r0, #160
   2b0fc: ebffaa02     	bl	0x1590c     @ imm = #-0x157f8 ; _Znwj
   2b100: e1a01004     	mov	r1, r4
   2b104: eeb00a48     	vmov.f32	s0, s16
   2b108: e1a04000     	mov	r4, r0
   2b10c: eb002b2f     	bl	0x35dd0
   2b110: e5956ab8     	ldr	r6, [r5, #0xab8]
   2b114: e5854ab8     	str	r4, [r5, #0xab8]
   2b118: e3560000     	cmp	r6, #0
   2b11c: 0a000004     	beq	0x2b134
   2b120: e1a00006     	mov	r0, r6
   2b124: eb002c45     	bl	0x36240
   2b128: e1a00006     	mov	r0, r6
   2b12c: e3a010a0     	mov	r1, #160
   2b130: ebffaae5     	bl	0x15ccc    @ imm = #-0x1546c ; _ZdlPvj
   2b134: e1a0000d     	mov	r0, sp
   2b138: e30111a0     	movw	r1, #0x11a0
   2b13c: e3401007     	movt	r1, #0x7
   2b140: ebfff298     	bl	0x27ba8
   2b144: e3090fec     	movw	r0, #0x9fec
   2b148: e3400009     	movt	r0, #0x9
   2b14c: e1a0100d     	mov	r1, sp
   2b150: e3a02000     	mov	r2, #0
   2b154: eb011371     	bl	0x6ff20
   2b158: e59d0000     	ldr	r0, [sp]
   2b15c: e1500007     	cmp	r0, r7
   2b160: 0a000000     	beq	0x2b168
   2b164: ebffab35     	bl	0x15e40    @ imm = #-0x1532c ; _ZdlPv
   2b168: e3a00000     	mov	r0, #0
   2b16c: e28dd01c     	add	sp, sp, #28
   2b170: ecbd8b02     	vpop	{d8}
   2b174: e8bd80f0     	pop	{r4, r5, r6, r7, pc}
   2b178: e1a0000d     	mov	r0, sp
   2b17c: e3011174     	movw	r1, #0x1174
   2b180: e3401007     	movt	r1, #0x7
   2b184: ebfff287     	bl	0x27ba8
   2b188: e3090fec     	movw	r0, #0x9fec
   2b18c: e3400009     	movt	r0, #0x9
   2b190: e3a02000     	mov	r2, #0
   2b194: e1a0100d     	mov	r1, sp
   2b198: eb011360     	bl	0x6ff20
   2b19c: e59d0000     	ldr	r0, [sp]
   2b1a0: e1500007     	cmp	r0, r7
   2b1a4: 0a000000     	beq	0x2b1ac
   2b1a8: ebffab24     	bl	0x15e40    @ imm = #-0x15370 ; _ZdlPv
   2b1ac: ed9f8a20     	vldr	s16, [pc, #128]         @ 0x2b234 ; float 3.09999990463
   2b1b0: eaffffd0     	b	0x2b0f8
   2b1b4: e5943164     	ldr	r3, [r4, #0x164]
   2b1b8: e2433001     	sub	r3, r3, #1
   2b1bc: e3530001     	cmp	r3, #1
   2b1c0: 8affffb9     	bhi	0x2b0ac
   2b1c4: e2842a2a     	add	r2, r4, #172032
   2b1c8: e5923684     	ldr	r3, [r2, #0x684]
   2b1cc: e2433001     	sub	r3, r3, #1
   2b1d0: e3530001     	cmp	r3, #1
   2b1d4: 8affffb4     	bhi	0x2b0ac
   2b1d8: e592369c     	ldr	r3, [r2, #0x69c]
   2b1dc: e2433001     	sub	r3, r3, #1
   2b1e0: e3530001     	cmp	r3, #1
   2b1e4: 8affffb0     	bhi	0x2b0ac
   2b1e8: e1a00004     	mov	r0, r4
   2b1ec: ebfffcbb     	bl	0x2a4e0
   2b1f0: eaffffad     	b	0x2b0ac
   2b1f4: e59d0000     	ldr	r0, [sp]
   2b1f8: e1500007     	cmp	r0, r7
   2b1fc: 0a000000     	beq	0x2b204
   2b200: ebffab0e     	bl	0x15e40    @ imm = #-0x153c8 ; _ZdlPv
   2b204: ebffab55     	bl	0x15f60    @ imm = #-0x152ac ; __cxa_end_cleanup
   2b208: e59d0000     	ldr	r0, [sp]
   2b20c: e28d3008     	add	r3, sp, #8
   2b210: e1500003     	cmp	r0, r3
   2b214: 1afffff9     	bne	0x2b200
   2b218: eafffff9     	b	0x2b204
   2b21c: eafffff4     	b	0x2b1f4
   2b220: eafffff3     	b	0x2b1f4
   2b224: e1a00004     	mov	r0, r4
   2b228: e3a010a0     	mov	r1, #160
   2b22c: ebffaaa6     	bl	0x15ccc    @ imm = #-0x15568 ; _ZdlPvj
   2b230: ebffab4a     	bl	0x15f60    @ imm = #-0x152d8 ; __cxa_end_cleanup
   2b234: 66 66 46 40  	.word	0x40466666
