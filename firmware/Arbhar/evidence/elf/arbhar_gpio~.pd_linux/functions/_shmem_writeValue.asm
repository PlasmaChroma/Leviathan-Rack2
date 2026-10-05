00007258 <_shmem_writeValue>:
    7258: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
    725c: e2809a01     	add	r9, r0, #4096
    7260: e24dd014     	sub	sp, sp, #20
    7264: e5991dc0     	ldr	r1, [r9, #0xdc0]
    7268: e3510000     	cmp	r1, #0
    726c: 0a0000d9     	beq	0x75d8 <_shmem_writeValue+0x380> @ imm = #0x364
    7270: e3520000     	cmp	r2, #0
    7274: da0000d2     	ble	0x75c4 <_shmem_writeValue+0x36c> @ imm = #0x348
    7278: e1a04003     	mov	r4, r3
    727c: e2123003     	ands	r3, r2, #3
    7280: e1a07000     	mov	r7, r0
    7284: e1a06002     	mov	r6, r2
    7288: e3a05000     	mov	r5, #0
    728c: 0a000032     	beq	0x735c <_shmem_writeValue+0x104> @ imm = #0xc8
    7290: e3530001     	cmp	r3, #1
    7294: 0a00001f     	beq	0x7318 <_shmem_writeValue+0xc0> @ imm = #0x7c
    7298: e3530002     	cmp	r3, #2
    729c: 0a00000e     	beq	0x72dc <_shmem_writeValue+0x84> @ imm = #0x38
    72a0: e5940000     	ldr	r0, [r4]
    72a4: e3500002     	cmp	r0, #2
    72a8: 1a00005f     	bne	0x742c <_shmem_writeValue+0x1d4> @ imm = #0x17c
    72ac: e1a02004     	mov	r2, r4
    72b0: e1a01006     	mov	r1, r6
    72b4: e1a00005     	mov	r0, r5
    72b8: ebfff26a     	bl	0x3c68 <.plt+0x56c>     @ imm = #-0x3658
    72bc: e5998dc4     	ldr	r8, [r9, #0xdc4]
    72c0: e1a03005     	mov	r3, r5
    72c4: e1a02005     	mov	r2, r5
    72c8: e3a05001     	mov	r5, #1
    72cc: e58d8000     	str	r8, [sp]
    72d0: e1a01000     	mov	r1, r0
    72d4: e1a00007     	mov	r0, r7
    72d8: ebfff1ba     	bl	0x39c8 <.plt+0x2cc>     @ imm = #-0x3918
    72dc: e7942185     	ldr	r2, [r4, r5, lsl #3]
    72e0: e3520002     	cmp	r2, #2
    72e4: 1a000050     	bne	0x742c <_shmem_writeValue+0x1d4> @ imm = #0x140
    72e8: e1a02004     	mov	r2, r4
    72ec: e1a01006     	mov	r1, r6
    72f0: e1a00005     	mov	r0, r5
    72f4: e2855001     	add	r5, r5, #1
    72f8: ebfff25a     	bl	0x3c68 <.plt+0x56c>     @ imm = #-0x3698
    72fc: e599adc4     	ldr	r10, [r9, #0xdc4]
    7300: e3a03000     	mov	r3, #0
    7304: e1a02003     	mov	r2, r3
    7308: e58da000     	str	r10, [sp]
    730c: e1a01000     	mov	r1, r0
    7310: e1a00007     	mov	r0, r7
    7314: ebfff1ab     	bl	0x39c8 <.plt+0x2cc>     @ imm = #-0x3954
    7318: e794b185     	ldr	r11, [r4, r5, lsl #3]
    731c: e35b0002     	cmp	r11, #2
    7320: 1a000041     	bne	0x742c <_shmem_writeValue+0x1d4> @ imm = #0x104
    7324: e1a00005     	mov	r0, r5
    7328: e2855001     	add	r5, r5, #1
    732c: e1a02004     	mov	r2, r4
    7330: e1a01006     	mov	r1, r6
    7334: ebfff24b     	bl	0x3c68 <.plt+0x56c>     @ imm = #-0x36d4
    7338: e599cdc4     	ldr	r12, [r9, #0xdc4]
    733c: e3a03000     	mov	r3, #0
    7340: e1a02003     	mov	r2, r3
    7344: e58dc000     	str	r12, [sp]
    7348: e1a01000     	mov	r1, r0
    734c: e1a00007     	mov	r0, r7
    7350: ebfff19c     	bl	0x39c8 <.plt+0x2cc>     @ imm = #-0x3990
    7354: e1560005     	cmp	r6, r5
    7358: 0a000033     	beq	0x742c <_shmem_writeValue+0x1d4> @ imm = #0xcc
    735c: e7943185     	ldr	r3, [r4, r5, lsl #3]
    7360: e1a02004     	mov	r2, r4
    7364: e1a01006     	mov	r1, r6
    7368: e1a00005     	mov	r0, r5
    736c: e3530002     	cmp	r3, #2
    7370: e2858001     	add	r8, r5, #1
    7374: 1a00002c     	bne	0x742c <_shmem_writeValue+0x1d4> @ imm = #0xb0
    7378: ebfff23a     	bl	0x3c68 <.plt+0x56c>     @ imm = #-0x3718
    737c: e5991dc4     	ldr	r1, [r9, #0xdc4]
    7380: e3a03000     	mov	r3, #0
    7384: e1a02003     	mov	r2, r3
    7388: e58d1000     	str	r1, [sp]
    738c: e1a01000     	mov	r1, r0
    7390: e1a00007     	mov	r0, r7
    7394: ebfff18b     	bl	0x39c8 <.plt+0x2cc>     @ imm = #-0x39d4
    7398: e794a188     	ldr	r10, [r4, r8, lsl #3]
    739c: e1a02004     	mov	r2, r4
    73a0: e1a01006     	mov	r1, r6
    73a4: e35a0002     	cmp	r10, #2
    73a8: e1a00008     	mov	r0, r8
    73ac: 1a00001e     	bne	0x742c <_shmem_writeValue+0x1d4> @ imm = #0x78
    73b0: ebfff22c     	bl	0x3c68 <.plt+0x56c>     @ imm = #-0x3750
    73b4: e599bdc4     	ldr	r11, [r9, #0xdc4]
    73b8: e3a03000     	mov	r3, #0
    73bc: e1a02003     	mov	r2, r3
    73c0: e58db000     	str	r11, [sp]
    73c4: e1a01000     	mov	r1, r0
    73c8: e1a00007     	mov	r0, r7
    73cc: ebfff17d     	bl	0x39c8 <.plt+0x2cc>     @ imm = #-0x3a0c
    73d0: e288c001     	add	r12, r8, #1
    73d4: e1a02004     	mov	r2, r4
    73d8: e1a01006     	mov	r1, r6
    73dc: e794818c     	ldr	r8, [r4, r12, lsl #3]
    73e0: e1a0000c     	mov	r0, r12
    73e4: e3580002     	cmp	r8, #2
    73e8: 1a00000f     	bne	0x742c <_shmem_writeValue+0x1d4> @ imm = #0x3c
    73ec: ebfff21d     	bl	0x3c68 <.plt+0x56c>     @ imm = #-0x378c
    73f0: e5991dc4     	ldr	r1, [r9, #0xdc4]
    73f4: e3a03000     	mov	r3, #0
    73f8: e1a02003     	mov	r2, r3
    73fc: e58d1000     	str	r1, [sp]
    7400: e1a01000     	mov	r1, r0
    7404: e1a00007     	mov	r0, r7
    7408: ebfff16e     	bl	0x39c8 <.plt+0x2cc>     @ imm = #-0x3a48
    740c: e2853003     	add	r3, r5, #3
    7410: e1a02004     	mov	r2, r4
    7414: e1a01006     	mov	r1, r6
    7418: e794a183     	ldr	r10, [r4, r3, lsl #3]
    741c: e1a00003     	mov	r0, r3
    7420: e2855004     	add	r5, r5, #4
    7424: e35a0002     	cmp	r10, #2
    7428: 0affffc1     	beq	0x7334 <_shmem_writeValue+0xdc> @ imm = #-0xfc
    742c: e3560001     	cmp	r6, #1
    7430: 0a000002     	beq	0x7440 <_shmem_writeValue+0x1e8> @ imm = #0x8
    7434: e5940000     	ldr	r0, [r4]
    7438: e3500001     	cmp	r0, #1
    743c: 0a000001     	beq	0x7448 <_shmem_writeValue+0x1f0> @ imm = #0x4
    7440: e28dd014     	add	sp, sp, #20
    7444: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
    7448: e1a02004     	mov	r2, r4
    744c: e3a00000     	mov	r0, #0
    7450: e1a01006     	mov	r1, r6
    7454: ebfff248     	bl	0x3d7c <.plt+0x680>     @ imm = #-0x36e0
    7458: e5940008     	ldr	r0, [r4, #0x8]
    745c: e3500002     	cmp	r0, #2
    7460: eebd0ac0     	vcvt.s32.f32	s0, s0
    7464: ee102a10     	vmov	r2, s0
    7468: e1c2bfc2     	bic	r11, r2, r2, asr #31
    746c: 0a00005e     	beq	0x75ec <_shmem_writeValue+0x394> @ imm = #0x178
    7470: e3500001     	cmp	r0, #1
    7474: 1afffff1     	bne	0x7440 <_shmem_writeValue+0x1e8> @ imm = #-0x3c
    7478: e599edc4     	ldr	lr, [r9, #0xdc4]
    747c: e04e800b     	sub	r8, lr, r11
    7480: e1560008     	cmp	r6, r8
    7484: d2468001     	suble	r8, r6, #1
    7488: e3580000     	cmp	r8, #0
    748c: daffffeb     	ble	0x7440 <_shmem_writeValue+0x1e8> @ imm = #-0x54
    7490: e218c003     	ands	r12, r8, #3
    7494: e1a0510b     	lsl	r5, r11, #2
    7498: e3a07000     	mov	r7, #0
    749c: 0a000020     	beq	0x7524 <_shmem_writeValue+0x2cc> @ imm = #0x80
    74a0: e35c0001     	cmp	r12, #1
    74a4: 0a000013     	beq	0x74f8 <_shmem_writeValue+0x2a0> @ imm = #0x4c
    74a8: e35c0002     	cmp	r12, #2
    74ac: 0a000008     	beq	0x74d4 <_shmem_writeValue+0x27c> @ imm = #0x20
    74b0: e5993dc0     	ldr	r3, [r9, #0xdc0]
    74b4: e3a07001     	mov	r7, #1
    74b8: e1a00007     	mov	r0, r7
    74bc: e1a02004     	mov	r2, r4
    74c0: e1a01006     	mov	r1, r6
    74c4: e083a005     	add	r10, r3, r5
    74c8: ebfff22b     	bl	0x3d7c <.plt+0x680>     @ imm = #-0x3754
    74cc: e2855004     	add	r5, r5, #4
    74d0: ed8a0a00     	vstr	s0, [r10]
    74d4: e2877001     	add	r7, r7, #1
    74d8: e1a02004     	mov	r2, r4
    74dc: e1a01006     	mov	r1, r6
    74e0: e599bdc0     	ldr	r11, [r9, #0xdc0]
    74e4: e1a00007     	mov	r0, r7
    74e8: ebfff223     	bl	0x3d7c <.plt+0x680>     @ imm = #-0x3774
    74ec: e08ba005     	add	r10, r11, r5
    74f0: e2855004     	add	r5, r5, #4
    74f4: ed8a0a00     	vstr	s0, [r10]
    74f8: e2877001     	add	r7, r7, #1
    74fc: e599edc0     	ldr	lr, [r9, #0xdc0]
    7500: e1a02004     	mov	r2, r4
    7504: e1a01006     	mov	r1, r6
    7508: e1a00007     	mov	r0, r7
    750c: e08eb005     	add	r11, lr, r5
    7510: ebfff219     	bl	0x3d7c <.plt+0x680>     @ imm = #-0x379c
    7514: e1570008     	cmp	r7, r8
    7518: e2855004     	add	r5, r5, #4
    751c: ed8b0a00     	vstr	s0, [r11]
    7520: 0affffc6     	beq	0x7440 <_shmem_writeValue+0x1e8> @ imm = #-0xe8
    7524: e599cdc0     	ldr	r12, [r9, #0xdc0]
    7528: e2870001     	add	r0, r7, #1
    752c: e1a02004     	mov	r2, r4
    7530: e1a01006     	mov	r1, r6
    7534: e08c3005     	add	r3, r12, r5
    7538: e58d300c     	str	r3, [sp, #0xc]
    753c: ebfff20e     	bl	0x3d7c <.plt+0x680>     @ imm = #-0x37c8
    7540: e59dc00c     	ldr	r12, [sp, #0xc]
    7544: e599bdc0     	ldr	r11, [r9, #0xdc0]
    7548: e2870002     	add	r0, r7, #2
    754c: e1a02004     	mov	r2, r4
    7550: e1a01006     	mov	r1, r6
    7554: e285a004     	add	r10, r5, #4
    7558: e08bb00a     	add	r11, r11, r10
    755c: ed8c0a00     	vstr	s0, [r12]
    7560: ebfff205     	bl	0x3d7c <.plt+0x680>     @ imm = #-0x37ec
    7564: e599adc0     	ldr	r10, [r9, #0xdc0]
    7568: e2853008     	add	r3, r5, #8
    756c: e2870003     	add	r0, r7, #3
    7570: e1a02004     	mov	r2, r4
    7574: e1a01006     	mov	r1, r6
    7578: e08ac003     	add	r12, r10, r3
    757c: e58dc00c     	str	r12, [sp, #0xc]
    7580: e2877004     	add	r7, r7, #4
    7584: e285a00c     	add	r10, r5, #12
    7588: e2855010     	add	r5, r5, #16
    758c: ed8b0a00     	vstr	s0, [r11]
    7590: ebfff1f9     	bl	0x3d7c <.plt+0x680>     @ imm = #-0x381c
    7594: e59d300c     	ldr	r3, [sp, #0xc]
    7598: e599bdc0     	ldr	r11, [r9, #0xdc0]
    759c: e1a00007     	mov	r0, r7
    75a0: e1a02004     	mov	r2, r4
    75a4: e1a01006     	mov	r1, r6
    75a8: e08bb00a     	add	r11, r11, r10
    75ac: ed830a00     	vstr	s0, [r3]
    75b0: ebfff1f1     	bl	0x3d7c <.plt+0x680>     @ imm = #-0x383c
    75b4: e1570008     	cmp	r7, r8
    75b8: ed8b0a00     	vstr	s0, [r11]
    75bc: 1affffd8     	bne	0x7524 <_shmem_writeValue+0x2cc> @ imm = #-0xa0
    75c0: eaffff9e     	b	0x7440 <_shmem_writeValue+0x1e8> @ imm = #-0x188
    75c4: e59f40b0     	ldr	r4, [pc, #0xb0]         @ 0x767c <_shmem_writeValue+0x424>
    75c8: e08f1004     	add	r1, pc, r4
    75cc: e28dd014     	add	sp, sp, #20
    75d0: e8bd4ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
    75d4: eafff1f7     	b	0x3db8 <.plt+0x6bc>     @ imm = #-0x3824
    75d8: e59f60a0     	ldr	r6, [pc, #0xa0]         @ 0x7680 <_shmem_writeValue+0x428>
    75dc: e08f0006     	add	r0, pc, r6
    75e0: e28dd014     	add	sp, sp, #20
    75e4: e8bd4ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
    75e8: eafff0f3     	b	0x39bc <.plt+0x2c0>     @ imm = #-0x3c34
    75ec: e3560002     	cmp	r6, #2
    75f0: e5999dc4     	ldr	r9, [r9, #0xdc4]
    75f4: 0a000002     	beq	0x7604 <_shmem_writeValue+0x3ac> @ imm = #0x8
    75f8: e5941010     	ldr	r1, [r4, #0x10]
    75fc: e3510001     	cmp	r1, #1
    7600: 0a00000b     	beq	0x7634 <_shmem_writeValue+0x3dc> @ imm = #0x2c
    7604: e3a08000     	mov	r8, #0
    7608: e1a02004     	mov	r2, r4
    760c: e1a01006     	mov	r1, r6
    7610: e3a00001     	mov	r0, #1
    7614: ebfff193     	bl	0x3c68 <.plt+0x56c>     @ imm = #-0x39b4
    7618: e58d9000     	str	r9, [sp]
    761c: e1a0300b     	mov	r3, r11
    7620: e1a02008     	mov	r2, r8
    7624: e1a01000     	mov	r1, r0
    7628: e1a00007     	mov	r0, r7
    762c: ebfff0e5     	bl	0x39c8 <.plt+0x2cc>     @ imm = #-0x3c6c
    7630: eaffff82     	b	0x7440 <_shmem_writeValue+0x1e8> @ imm = #-0x1f8
    7634: e1a02004     	mov	r2, r4
    7638: e1a01006     	mov	r1, r6
    763c: ebfff1ce     	bl	0x3d7c <.plt+0x680>     @ imm = #-0x38c8
    7640: e3560003     	cmp	r6, #3
    7644: eefd0ac0     	vcvt.s32.f32	s1, s0
    7648: ee102a90     	vmov	r2, s1
    764c: e1c28fc2     	bic	r8, r2, r2, asr #31
    7650: 0affffec     	beq	0x7608 <_shmem_writeValue+0x3b0> @ imm = #-0x50
    7654: e594e010     	ldr	lr, [r4, #0x10]
    7658: e35e0001     	cmp	lr, #1
    765c: 1affffe9     	bne	0x7608 <_shmem_writeValue+0x3b0> @ imm = #-0x5c
    7660: e1a02004     	mov	r2, r4
    7664: e1a01006     	mov	r1, r6
    7668: e3a00003     	mov	r0, #3
    766c: ebfff1c2     	bl	0x3d7c <.plt+0x680>     @ imm = #-0x38f8
    7670: eefd7ac0     	vcvt.s32.f32	s15, s0
    7674: ee179a90     	vmov	r9, s15
    7678: eaffffe2     	b	0x7608 <_shmem_writeValue+0x3b0> @ imm = #-0x78
    767c: 84 d8 00 00  	.word	0x0000d884
    7680: 3c d8 00 00  	.word	0x0000d83c

