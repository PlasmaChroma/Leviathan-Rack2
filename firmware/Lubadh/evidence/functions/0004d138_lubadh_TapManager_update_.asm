; lubadh::TapManager::update()
; VA 0x4d138 size 972

   4d138: e92d41f0     	push	{r4, r5, r6, r7, r8, lr}
   4d13c: e2804eaa     	add	r4, r0, #2720
   4d140: e1a07000     	mov	r7, r0
   4d144: e2805eab     	add	r5, r0, #2736
   4d148: e1a06004     	mov	r6, r4
   4d14c: e4960004     	ldr	r0, [r6], #4
   4d150: e3500000     	cmp	r0, #0
   4d154: 0a000000     	beq	0x4d15c
   4d158: ebfffcd4     	bl	0x4c4b0
   4d15c: e1560005     	cmp	r6, r5
   4d160: 1afffff9     	bne	0x4d14c
   4d164: e0452004     	sub	r2, r5, r4
   4d168: e1a03242     	asr	r3, r2, #4
   4d16c: e1a02142     	asr	r2, r2, #2
   4d170: e3530000     	cmp	r3, #0
   4d174: da000033     	ble	0x4d248
   4d178: e28330aa     	add	r3, r3, #170
   4d17c: e0872203     	add	r2, r7, r3, lsl #4
   4d180: e5943000     	ldr	r3, [r4]
   4d184: e3530000     	cmp	r3, #0
   4d188: 0a00005c     	beq	0x4d300
   4d18c: e5d31000     	ldrb	r1, [r3]
   4d190: e3510000     	cmp	r1, #0
   4d194: 1a000002     	bne	0x4d1a4
   4d198: e5d31084     	ldrb	r1, [r3, #0x84]
   4d19c: e3510000     	cmp	r1, #0
   4d1a0: 0a00004d     	beq	0x4d2dc
   4d1a4: e5943004     	ldr	r3, [r4, #0x4]
   4d1a8: e2840004     	add	r0, r4, #4
   4d1ac: e3530000     	cmp	r3, #0
   4d1b0: 0a000080     	beq	0x4d3b8
   4d1b4: e5d31000     	ldrb	r1, [r3]
   4d1b8: e3510000     	cmp	r1, #0
   4d1bc: 1a000002     	bne	0x4d1cc
   4d1c0: e5d31084     	ldrb	r1, [r3, #0x84]
   4d1c4: e3510000     	cmp	r1, #0
   4d1c8: 0a000071     	beq	0x4d394
   4d1cc: e5943008     	ldr	r3, [r4, #0x8]
   4d1d0: e2840008     	add	r0, r4, #8
   4d1d4: e3530000     	cmp	r3, #0
   4d1d8: 0a000076     	beq	0x4d3b8
   4d1dc: e5d31000     	ldrb	r1, [r3]
   4d1e0: e3510000     	cmp	r1, #0
   4d1e4: 1a00000b     	bne	0x4d218
   4d1e8: e5d31084     	ldrb	r1, [r3, #0x84]
   4d1ec: e3510000     	cmp	r1, #0
   4d1f0: 1a000008     	bne	0x4d218
   4d1f4: e5d31108     	ldrb	r1, [r3, #0x108]
   4d1f8: e3510000     	cmp	r1, #0
   4d1fc: 1a000005     	bne	0x4d218
   4d200: e5d3118c     	ldrb	r1, [r3, #0x18c]
   4d204: e3510000     	cmp	r1, #0
   4d208: 1a000002     	bne	0x4d218
   4d20c: e5d33210     	ldrb	r3, [r3, #0x210]
   4d210: e3530000     	cmp	r3, #0
   4d214: 0a000067     	beq	0x4d3b8
   4d218: e594300c     	ldr	r3, [r4, #0xc]
   4d21c: e284000c     	add	r0, r4, #12
   4d220: e3530000     	cmp	r3, #0
   4d224: 0a000063     	beq	0x4d3b8
   4d228: e5d31000     	ldrb	r1, [r3]
   4d22c: e3510000     	cmp	r1, #0
   4d230: 0a000064     	beq	0x4d3c8
   4d234: e2844010     	add	r4, r4, #16
   4d238: e1520004     	cmp	r2, r4
   4d23c: 1affffcf     	bne	0x4d180
   4d240: e0452004     	sub	r2, r5, r4
   4d244: e1a02142     	asr	r2, r2, #2
   4d248: e3520002     	cmp	r2, #2
   4d24c: 0a00008c     	beq	0x4d484
   4d250: e3520003     	cmp	r2, #3
   4d254: 0a00006b     	beq	0x4d408
   4d258: e3520001     	cmp	r2, #1
   4d25c: 18bd81f0     	popne	{r4, r5, r6, r7, r8, pc}
   4d260: e5942000     	ldr	r2, [r4]
   4d264: e3520000     	cmp	r2, #0
   4d268: 0a000024     	beq	0x4d300
   4d26c: e1a03002     	mov	r3, r2
   4d270: e2821fa5     	add	r1, r2, #660
   4d274: e4d30084     	ldrb	r0, [r3], #132
   4d278: e3500000     	cmp	r0, #0
   4d27c: 1a00000f     	bne	0x4d2c0
   4d280: e5d20084     	ldrb	r0, [r2, #0x84]
   4d284: e2823f42     	add	r3, r2, #264
   4d288: e3500000     	cmp	r0, #0
   4d28c: 1a00000b     	bne	0x4d2c0
   4d290: e5d20108     	ldrb	r0, [r2, #0x108]
   4d294: e2823f63     	add	r3, r2, #396
   4d298: e3500000     	cmp	r0, #0
   4d29c: 1a000007     	bne	0x4d2c0
   4d2a0: e5d2018c     	ldrb	r0, [r2, #0x18c]
   4d2a4: e2823e21     	add	r3, r2, #528
   4d2a8: e3500000     	cmp	r0, #0
   4d2ac: 1a000003     	bne	0x4d2c0
   4d2b0: e5d22210     	ldrb	r2, [r2, #0x210]
   4d2b4: e1a03001     	mov	r3, r1
   4d2b8: e3520000     	cmp	r2, #0
   4d2bc: 0a00000f     	beq	0x4d300
   4d2c0: e1510003     	cmp	r1, r3
   4d2c4: 08bd81f0     	popeq	{r4, r5, r6, r7, r8, pc}
   4d2c8: e2832084     	add	r2, r3, #132
   4d2cc: e2833f42     	add	r3, r3, #264
   4d2d0: e1510002     	cmp	r1, r2
   4d2d4: 1afffff9     	bne	0x4d2c0
   4d2d8: e8bd81f0     	pop	{r4, r5, r6, r7, r8, pc}
   4d2dc: e5d31108     	ldrb	r1, [r3, #0x108]
   4d2e0: e3510000     	cmp	r1, #0
   4d2e4: 1affffae     	bne	0x4d1a4
   4d2e8: e5d3118c     	ldrb	r1, [r3, #0x18c]
   4d2ec: e3510000     	cmp	r1, #0
   4d2f0: 1affffab     	bne	0x4d1a4
   4d2f4: e5d33210     	ldrb	r3, [r3, #0x210]
   4d2f8: e3530000     	cmp	r3, #0
   4d2fc: 1affffa8     	bne	0x4d1a4
   4d300: e1550004     	cmp	r5, r4
   4d304: 08bd81f0     	popeq	{r4, r5, r6, r7, r8, pc}
   4d308: e2841004     	add	r1, r4, #4
   4d30c: e1550001     	cmp	r5, r1
   4d310: 1a000003     	bne	0x4d324
   4d314: ea000018     	b	0x4d37c
   4d318: e4843004     	str	r3, [r4], #4
   4d31c: e1550001     	cmp	r5, r1
   4d320: 0a000013     	beq	0x4d374
   4d324: e4913004     	ldr	r3, [r1], #4
   4d328: e3530000     	cmp	r3, #0
   4d32c: 0afffffa     	beq	0x4d31c
   4d330: e5d3c000     	ldrb	r12, [r3]
   4d334: e35c0000     	cmp	r12, #0
   4d338: 1afffff6     	bne	0x4d318
   4d33c: e5d32084     	ldrb	r2, [r3, #0x84]
   4d340: e3520000     	cmp	r2, #0
   4d344: 1afffff3     	bne	0x4d318
   4d348: e5d32108     	ldrb	r2, [r3, #0x108]
   4d34c: e3520000     	cmp	r2, #0
   4d350: 1afffff0     	bne	0x4d318
   4d354: e5d3218c     	ldrb	r2, [r3, #0x18c]
   4d358: e3520000     	cmp	r2, #0
   4d35c: 1affffed     	bne	0x4d318
   4d360: e5d32210     	ldrb	r2, [r3, #0x210]
   4d364: e3520000     	cmp	r2, #0
   4d368: 1affffea     	bne	0x4d318
   4d36c: e1550001     	cmp	r5, r1
   4d370: 1affffeb     	bne	0x4d324
   4d374: e1550004     	cmp	r5, r4
   4d378: 08bd81f0     	popeq	{r4, r5, r6, r7, r8, pc}
   4d37c: e0472004     	sub	r2, r7, r4
   4d380: e1a00004     	mov	r0, r4
   4d384: e2822eab     	add	r2, r2, #2736
   4d388: e3a01000     	mov	r1, #0
   4d38c: e8bd41f0     	pop	{r4, r5, r6, r7, r8, lr}
   4d390: eaff2277     	b	0x15d74    @ imm = #-0x37624
   4d394: e5d31108     	ldrb	r1, [r3, #0x108]
   4d398: e3510000     	cmp	r1, #0
   4d39c: 1affff8a     	bne	0x4d1cc
   4d3a0: e5d3118c     	ldrb	r1, [r3, #0x18c]
   4d3a4: e3510000     	cmp	r1, #0
   4d3a8: 1affff87     	bne	0x4d1cc
   4d3ac: e5d33210     	ldrb	r3, [r3, #0x210]
   4d3b0: e3530000     	cmp	r3, #0
   4d3b4: 1affff84     	bne	0x4d1cc
   4d3b8: e1a04000     	mov	r4, r0
   4d3bc: e1550004     	cmp	r5, r4
   4d3c0: 1affffd0     	bne	0x4d308
   4d3c4: e8bd81f0     	pop	{r4, r5, r6, r7, r8, pc}
   4d3c8: e5d31084     	ldrb	r1, [r3, #0x84]
   4d3cc: e3510000     	cmp	r1, #0
   4d3d0: 1affff97     	bne	0x4d234
   4d3d4: e5d31108     	ldrb	r1, [r3, #0x108]
   4d3d8: e3510000     	cmp	r1, #0
   4d3dc: 1affff94     	bne	0x4d234
   4d3e0: e5d3118c     	ldrb	r1, [r3, #0x18c]
   4d3e4: e3510000     	cmp	r1, #0
   4d3e8: 1affff91     	bne	0x4d234
   4d3ec: e5d33210     	ldrb	r3, [r3, #0x210]
   4d3f0: e3530000     	cmp	r3, #0
   4d3f4: 0affffef     	beq	0x4d3b8
   4d3f8: e2844010     	add	r4, r4, #16
   4d3fc: e1520004     	cmp	r2, r4
   4d400: 1affff5e     	bne	0x4d180
   4d404: eaffff8d     	b	0x4d240
   4d408: e5942000     	ldr	r2, [r4]
   4d40c: e3520000     	cmp	r2, #0
   4d410: 0affffba     	beq	0x4d300
   4d414: e1a03002     	mov	r3, r2
   4d418: e2821fa5     	add	r1, r2, #660
   4d41c: e4d30084     	ldrb	r0, [r3], #132
   4d420: e3500000     	cmp	r0, #0
   4d424: 1a00000f     	bne	0x4d468
   4d428: e5d20084     	ldrb	r0, [r2, #0x84]
   4d42c: e2823f42     	add	r3, r2, #264
   4d430: e3500000     	cmp	r0, #0
   4d434: 1a00000b     	bne	0x4d468
   4d438: e5d20108     	ldrb	r0, [r2, #0x108]
   4d43c: e2823f63     	add	r3, r2, #396
   4d440: e3500000     	cmp	r0, #0
   4d444: 1a000007     	bne	0x4d468
   4d448: e5d2018c     	ldrb	r0, [r2, #0x18c]
   4d44c: e2823e21     	add	r3, r2, #528
   4d450: e3500000     	cmp	r0, #0
   4d454: 1a000003     	bne	0x4d468
   4d458: e5d22210     	ldrb	r2, [r2, #0x210]
   4d45c: e1a03001     	mov	r3, r1
   4d460: e3520000     	cmp	r2, #0
   4d464: 0affffa5     	beq	0x4d300
   4d468: e1510003     	cmp	r1, r3
   4d46c: 0a000003     	beq	0x4d480
   4d470: e2832084     	add	r2, r3, #132
   4d474: e2833f42     	add	r3, r3, #264
   4d478: e1510002     	cmp	r1, r2
   4d47c: 1afffff9     	bne	0x4d468
   4d480: e2844004     	add	r4, r4, #4
   4d484: e5942000     	ldr	r2, [r4]
   4d488: e3520000     	cmp	r2, #0
   4d48c: 0affff9b     	beq	0x4d300
   4d490: e1a03002     	mov	r3, r2
   4d494: e2821fa5     	add	r1, r2, #660
   4d498: e4d30084     	ldrb	r0, [r3], #132
   4d49c: e3500000     	cmp	r0, #0
   4d4a0: 1a00000f     	bne	0x4d4e4
   4d4a4: e5d20084     	ldrb	r0, [r2, #0x84]
   4d4a8: e2823f42     	add	r3, r2, #264
   4d4ac: e3500000     	cmp	r0, #0
   4d4b0: 1a00000b     	bne	0x4d4e4
   4d4b4: e5d20108     	ldrb	r0, [r2, #0x108]
   4d4b8: e2823f63     	add	r3, r2, #396
   4d4bc: e3500000     	cmp	r0, #0
   4d4c0: 1a000007     	bne	0x4d4e4
   4d4c4: e5d2018c     	ldrb	r0, [r2, #0x18c]
   4d4c8: e2823e21     	add	r3, r2, #528
   4d4cc: e3500000     	cmp	r0, #0
   4d4d0: 1a000003     	bne	0x4d4e4
   4d4d4: e5d22210     	ldrb	r2, [r2, #0x210]
   4d4d8: e1a03001     	mov	r3, r1
   4d4dc: e3520000     	cmp	r2, #0
   4d4e0: 0affff86     	beq	0x4d300
   4d4e4: e1510003     	cmp	r1, r3
   4d4e8: 0a000003     	beq	0x4d4fc
   4d4ec: e2832084     	add	r2, r3, #132
   4d4f0: e2833f42     	add	r3, r3, #264
   4d4f4: e1510002     	cmp	r1, r2
   4d4f8: 1afffff9     	bne	0x4d4e4
   4d4fc: e2844004     	add	r4, r4, #4
   4d500: eaffff56     	b	0x4d260
