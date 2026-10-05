; lubadh::Channel::FileLoader::FileLoader(lubadh::Channel&)
; VA 0x3c02c size 756

   3c02c: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   3c030: e1a05000     	mov	r5, r0
   3c034: e1a03001     	mov	r3, r1
   3c038: e24dd01c     	sub	sp, sp, #28
   3c03c: e2812004     	add	r2, r1, #4
   3c040: e4853004     	str	r3, [r5], #4
   3c044: e1a04000     	mov	r4, r0
   3c048: e59f129c     	ldr	r1, [pc, #0x29c]        @ 0x3c2ec
   3c04c: e1a0000d     	mov	r0, sp
   3c050: ebffcac9     	bl	0x2eb7c
   3c054: e1a0100d     	mov	r1, sp
   3c058: e1a00005     	mov	r0, r5
   3c05c: eb00176a     	bl	0x41e0c
   3c060: e59d0000     	ldr	r0, [sp]
   3c064: e28d3008     	add	r3, sp, #8
   3c068: e1500003     	cmp	r0, r3
   3c06c: e59f327c     	ldr	r3, [pc, #0x27c]        @ 0x3c2f0
   3c070: e5843004     	str	r3, [r4, #0x4]
   3c074: 0a000000     	beq	0x3c07c
   3c078: ebff6770     	bl	0x15e40    @ imm = #-0x26240 ; _ZdlPv
   3c07c: e284902c     	add	r9, r4, #44
   3c080: e3a02000     	mov	r2, #0
   3c084: e59f1268     	ldr	r1, [pc, #0x268]        @ 0x3c2f4
   3c088: e1a00009     	mov	r0, r9
   3c08c: e5842028     	str	r2, [r4, #0x28]
   3c090: eb00cfd4     	bl	0x6ffe8
   3c094: e3048dc0     	movw	r8, #0x4dc0
   3c098: e3408009     	movt	r8, #0x9
   3c09c: e59f2254     	ldr	r2, [pc, #0x254]        @ 0x3c2f8
   3c0a0: e3a03000     	mov	r3, #0
   3c0a4: e5842048     	str	r2, [r4, #0x48]
   3c0a8: e2847058     	add	r7, r4, #88
   3c0ac: e59810c0     	ldr	r1, [r8, #0xc0]
   3c0b0: e2840050     	add	r0, r4, #80
   3c0b4: e59820c4     	ldr	r2, [r8, #0xc4]
   3c0b8: e2846048     	add	r6, r4, #72
   3c0bc: e584304c     	str	r3, [r4, #0x4c]
   3c0c0: e0812002     	add	r2, r1, r2
   3c0c4: e5847050     	str	r7, [r4, #0x50]
   3c0c8: ebffeb8a     	bl	0x36ef8
   3c0cc: e3e03000     	mvn	r3, #0
   3c0d0: e1a00006     	mov	r0, r6
   3c0d4: e5843068     	str	r3, [r4, #0x68]
   3c0d8: eb0024c4     	bl	0x453f0
   3c0dc: e1a00006     	mov	r0, r6
   3c0e0: eb00260a     	bl	0x45910
   3c0e4: e1a00006     	mov	r0, r6
   3c0e8: eb0026db     	bl	0x45c5c
   3c0ec: e59f1208     	ldr	r1, [pc, #0x208]        @ 0x3c2fc
   3c0f0: e284306c     	add	r3, r4, #108
   3c0f4: e598e0d8     	ldr	lr, [r8, #0xd8]
   3c0f8: e3a0c000     	mov	r12, #0
   3c0fc: e59f71fc     	ldr	r7, [pc, #0x1fc]        @ 0x3c300
   3c100: e284a084     	add	r10, r4, #132
   3c104: e59820dc     	ldr	r2, [r8, #0xdc]
   3c108: e8910003     	ldm	r1, {r0, r1}
   3c10c: e5847048     	str	r7, [r4, #0x48]
   3c110: e8830003     	stm	r3, {r0, r1}
   3c114: e08e2002     	add	r2, lr, r2
   3c118: e59f31e4     	ldr	r3, [pc, #0x1e4]        @ 0x3c304
   3c11c: e1a0100e     	mov	r1, lr
   3c120: e284007c     	add	r0, r4, #124
   3c124: e5843074     	str	r3, [r4, #0x74]
   3c128: e584c078     	str	r12, [r4, #0x78]
   3c12c: e2847074     	add	r7, r4, #116
   3c130: e584a07c     	str	r10, [r4, #0x7c]
   3c134: ebffeb6f     	bl	0x36ef8
   3c138: e3e03000     	mvn	r3, #0
   3c13c: e1a00007     	mov	r0, r7
   3c140: e5843094     	str	r3, [r4, #0x94]
   3c144: eb002754     	bl	0x45e9c
   3c148: e1a00007     	mov	r0, r7
   3c14c: eb00289a     	bl	0x463bc
   3c150: e1a00007     	mov	r0, r7
   3c154: eb00296b     	bl	0x46708
   3c158: e59810f0     	ldr	r1, [r8, #0xf0]
   3c15c: e3a03000     	mov	r3, #0
   3c160: e59820f4     	ldr	r2, [r8, #0xf4]
   3c164: e284a0a8     	add	r10, r4, #168
   3c168: e59f0198     	ldr	r0, [pc, #0x198]        @ 0x3c308
   3c16c: e2848098     	add	r8, r4, #152
   3c170: e59fc194     	ldr	r12, [pc, #0x194]       @ 0x3c30c
   3c174: e0812002     	add	r2, r1, r2
   3c178: e5840074     	str	r0, [r4, #0x74]
   3c17c: e28400a0     	add	r0, r4, #160
   3c180: e584c098     	str	r12, [r4, #0x98]
   3c184: e584309c     	str	r3, [r4, #0x9c]
   3c188: e584a0a0     	str	r10, [r4, #0xa0]
   3c18c: ebffeb59     	bl	0x36ef8
   3c190: e3e03000     	mvn	r3, #0
   3c194: e1a00008     	mov	r0, r8
   3c198: e58430b8     	str	r3, [r4, #0xb8]
   3c19c: eb0021e8     	bl	0x44944
   3c1a0: e1a00008     	mov	r0, r8
   3c1a4: eb00232e     	bl	0x44e64
   3c1a8: e1a00008     	mov	r0, r8
   3c1ac: eb0023ff     	bl	0x451b0
   3c1b0: e59fa158     	ldr	r10, [pc, #0x158]       @ 0x3c310
   3c1b4: e284b0bc     	add	r11, r4, #188
   3c1b8: e59f1154     	ldr	r1, [pc, #0x154]        @ 0x3c314
   3c1bc: e1a0000b     	mov	r0, r11
   3c1c0: e3a02000     	mov	r2, #0
   3c1c4: e584a098     	str	r10, [r4, #0x98]
   3c1c8: eb00cf86     	bl	0x6ffe8
   3c1cc: e59f1144     	ldr	r1, [pc, #0x144]        @ 0x3c318
   3c1d0: e28400d8     	add	r0, r4, #216
   3c1d4: eb00170c     	bl	0x41e0c
   3c1d8: e594304c     	ldr	r3, [r4, #0x4c]
   3c1dc: f2c02010     	vmov.i32	d18, #0x0
   3c1e0: f2c00050     	vmov.i32	q8, #0x0
   3c1e4: e5940008     	ldr	r0, [r4, #0x8]
   3c1e8: e28410fc     	add	r1, r4, #252
   3c1ec: e59fc128     	ldr	r12, [pc, #0x128]       @ 0x3c31c
   3c1f0: e584c0d8     	str	r12, [r4, #0xd8]
   3c1f4: e3a02000     	mov	r2, #0
   3c1f8: f441278f     	vst1.32	{d18}, [r1]
   3c1fc: e5802000     	str	r2, [r0]
   3c200: e1a00004     	mov	r0, r4
   3c204: e5842028     	str	r2, [r4, #0x28]
   3c208: f4430a0d     	vst1.8	{d16, d17}, [r3]!
   3c20c: f4430a0d     	vst1.8	{d16, d17}, [r3]!
   3c210: f4430a0f     	vst1.8	{d16, d17}, [r3]
   3c214: e283300c     	add	r3, r3, #12
   3c218: f443070f     	vst1.8	{d16}, [r3]
   3c21c: e594309c     	ldr	r3, [r4, #0x9c]
   3c220: e5c32000     	strb	r2, [r3]
   3c224: e5c32001     	strb	r2, [r3, #0x1]
   3c228: e5c42104     	strb	r2, [r4, #0x104]
   3c22c: e28dd01c     	add	sp, sp, #28
   3c230: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
   3c234: e59d0000     	ldr	r0, [sp]
   3c238: e28d3008     	add	r3, sp, #8
   3c23c: e1500003     	cmp	r0, r3
   3c240: 0a000014     	beq	0x3c298
   3c244: ebff66fd     	bl	0x15e40    @ imm = #-0x2640c ; _ZdlPv
   3c248: ea000012     	b	0x3c298
   3c24c: e1a0000b     	mov	r0, r11
   3c250: eb00d08e     	bl	0x70490
   3c254: e1a00008     	mov	r0, r8
   3c258: e584a098     	str	r10, [r4, #0x98]
   3c25c: ebffe0ce     	bl	0x3459c
   3c260: e59f30a0     	ldr	r3, [pc, #0xa0]         @ 0x3c308
   3c264: e1a00007     	mov	r0, r7
   3c268: e5843074     	str	r3, [r4, #0x74]
   3c26c: ebffe133     	bl	0x34740
   3c270: e59f3088     	ldr	r3, [pc, #0x88]         @ 0x3c300
   3c274: e1a00006     	mov	r0, r6
   3c278: e5843048     	str	r3, [r4, #0x48]
   3c27c: ebffe2d5     	bl	0x34dd8
   3c280: e1a00009     	mov	r0, r9
   3c284: eb00d081     	bl	0x70490
   3c288: e59f3060     	ldr	r3, [pc, #0x60]         @ 0x3c2f0
   3c28c: e1a00005     	mov	r0, r5
   3c290: e5843004     	str	r3, [r4, #0x4]
   3c294: ebffcbc8     	bl	0x2f1bc
   3c298: ebff6730     	bl	0x15f60    @ imm = #-0x26340 ; __cxa_end_cleanup
   3c29c: eaffffec     	b	0x3c254
   3c2a0: e59400a0     	ldr	r0, [r4, #0xa0]
   3c2a4: e15a0000     	cmp	r10, r0
   3c2a8: 0affffec     	beq	0x3c260
   3c2ac: ebff66e3     	bl	0x15e40    @ imm = #-0x26474 ; _ZdlPv
   3c2b0: eaffffea     	b	0x3c260
   3c2b4: eaffffe9     	b	0x3c260
   3c2b8: e594007c     	ldr	r0, [r4, #0x7c]
   3c2bc: e15a0000     	cmp	r10, r0
   3c2c0: 0affffea     	beq	0x3c270
   3c2c4: ebff66dd     	bl	0x15e40    @ imm = #-0x2648c ; _ZdlPv
   3c2c8: eaffffe8     	b	0x3c270
   3c2cc: eaffffe7     	b	0x3c270
   3c2d0: e5940050     	ldr	r0, [r4, #0x50]
   3c2d4: e1570000     	cmp	r7, r0
   3c2d8: 0affffe8     	beq	0x3c280
   3c2dc: ebff66d7     	bl	0x15e40    @ imm = #-0x264a4 ; _ZdlPv
   3c2e0: eaffffe6     	b	0x3c280
   3c2e4: eaffffe5     	b	0x3c280
   3c2e8: eaffffe6     	b	0x3c288
   3c2ec: 50 4e 09 00  	.word	0x00094e50
   3c2f0: 70 1f 07 00  	.word	0x00071f70
   3c2f4: 68 4e 09 00  	.word	0x00094e68
   3c2f8: 80 1f 07 00  	.word	0x00071f80
   3c2fc: 14 30 07 00  	.word	0x00073014
   3c300: f4 2e 07 00  	.word	0x00072ef4
   3c304: 90 1f 07 00  	.word	0x00071f90
   3c308: 04 2f 07 00  	.word	0x00072f04
   3c30c: 50 1f 07 00  	.word	0x00071f50
   3c310: e4 2e 07 00  	.word	0x00072ee4
   3c314: c8 4e 09 00  	.word	0x00094ec8
   3c318: e0 4e 09 00  	.word	0x00094ee0
   3c31c: e0 1e 07 00  	.word	0x00071ee0
