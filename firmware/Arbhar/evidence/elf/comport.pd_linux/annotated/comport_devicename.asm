00003340 <comport_devicename>:
    3340: e5903020     	ldr	r3, [r0, #0x20]
    3344: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
    3348: e3730001     	cmn	r3, #1
    334c: e24dd03c     	sub	sp, sp, #60
    3350: e1a05000     	mov	r5, r0
    3354: e580109c     	str	r1, [r0, #0x9c]
    3358: 0280b060     	addeq	r11, r0, #96
    335c: 02808a01     	addeq	r8, r0, #4096
    3360: 0a00001c     	beq	0x33d8 <comport_devicename+0x98> @ imm = #0x70
    3364: e2808a01     	add	r8, r0, #4096
    3368: e280b060     	add	r11, r0, #96
    336c: e59800c4     	ldr	r0, [r8, #0xc4]
    3370: ebfff5df     	bl	0xaf4 <.plt+0xec>       @ imm = #-0x2884  // CALL clock_unset
    3374: e5954020     	ldr	r4, [r5, #0x20]
    3378: e3a00001     	mov	r0, #1
    337c: e3740001     	cmn	r4, #1
    3380: e58800c8     	str	r0, [r8, #0xc8]
    3384: 0a00000b     	beq	0x33b8 <comport_devicename+0x78> @ imm = #0x2c
    3388: e1a0200b     	mov	r2, r11
    338c: e3a01000     	mov	r1, #0
    3390: e1a00004     	mov	r0, r4
    3394: ebfff5c4     	bl	0xaac <.plt+0xa4>       @ imm = #-0x28f0  // CALL tcsetattr
    3398: e1a00004     	mov	r0, r4
    339c: ebfff60d     	bl	0xbd8 <.plt+0x1d0>      @ imm = #-0x27cc  // CALL close
    33a0: e595209c     	ldr	r2, [r5, #0x9c]
    33a4: e59f6404     	ldr	r6, [pc, #0x404]        @ 0x37b0 <comport_devicename+0x470>  // u32=0x12a8; f32?=6.69260147e-42
    33a8: e1d81af0     	ldrsh	r1, [r8, #160]
    33ac: e08f0006     	add	r0, pc, r6
    33b0: e5922000     	ldr	r2, [r2]
    33b4: ebfff5e0     	bl	0xb3c <.plt+0x134>      @ imm = #-0x2880  // CALL post
    33b8: e59800e8     	ldr	r0, [r8, #0xe8]
    33bc: e3e01000     	mvn	r1, #0
    33c0: e3500000     	cmp	r0, #0
    33c4: e5851020     	str	r1, [r5, #0x20]
    33c8: e1c81ab0     	strh	r1, [r8, #160]
    33cc: 0a000001     	beq	0x33d8 <comport_devicename+0x98> @ imm = #0x4
    33d0: ed9f0af5     	vldr	s0, [pc, #980]          @ 0x37ac <comport_devicename+0x46c>  // f32=-1
    33d4: ebfff5e7     	bl	0xb78 <.plt+0x170>      @ imm = #-0x2864  // CALL outlet_float
    33d8: e3a02000     	mov	r2, #0
    33dc: e28570a0     	add	r7, r5, #160
    33e0: e28d3014     	add	r3, sp, #20
    33e4: e1a00007     	mov	r0, r7
    33e8: e1a01002     	mov	r1, r2
    33ec: e58d3008     	str	r3, [sp, #0x8]
    33f0: ebfff5da     	bl	0xb60 <.plt+0x158>      @ imm = #-0x2898  // CALL glob
    33f4: e2859024     	add	r9, r5, #36
    33f8: e58d900c     	str	r9, [sp, #0xc]
    33fc: e3500002     	cmp	r0, #2
    3400: 0a0000e3     	beq	0x3794 <comport_devicename+0x454> @ imm = #0x38c
    3404: e3500003     	cmp	r0, #3
    3408: 0a0000db     	beq	0x377c <comport_devicename+0x43c> @ imm = #0x36c
    340c: e3500001     	cmp	r0, #1
    3410: 0a0000d3     	beq	0x3764 <comport_devicename+0x424> @ imm = #0x34c
    3414: e59da014     	ldr	r10, [sp, #0x14]
    3418: e35a0000     	cmp	r10, #0
    341c: 0a00007a     	beq	0x360c <comport_devicename+0x2cc> @ imm = #0x1e8
    3420: e595309c     	ldr	r3, [r5, #0x9c]
    3424: e59d6018     	ldr	r6, [sp, #0x18]
    3428: e24a4001     	sub	r4, r10, #1
    342c: e5937000     	ldr	r7, [r3]
    3430: e5961000     	ldr	r1, [r6]
    3434: e1a00007     	mov	r0, r7
    3438: ebfff57a     	bl	0xa28 <.plt+0x20>       @ imm = #-0x2a18  // CALL strcmp
    343c: e2049007     	and	r9, r4, #7
    3440: e3a04000     	mov	r4, #0
    3444: e1500004     	cmp	r0, r4
    3448: 0a000070     	beq	0x3610 <comport_devicename+0x2d0> @ imm = #0x1c0
    344c: e3a04001     	mov	r4, #1
    3450: e154000a     	cmp	r4, r10
    3454: 0a00006c     	beq	0x360c <comport_devicename+0x2cc> @ imm = #0x1b0
    3458: e3590000     	cmp	r9, #0
    345c: 0a000037     	beq	0x3540 <comport_devicename+0x200> @ imm = #0xdc
    3460: e1590004     	cmp	r9, r4
    3464: 0a00002d     	beq	0x3520 <comport_devicename+0x1e0> @ imm = #0xb4
    3468: e3590002     	cmp	r9, #2
    346c: 0a000025     	beq	0x3508 <comport_devicename+0x1c8> @ imm = #0x94
    3470: e3590003     	cmp	r9, #3
    3474: 0a00001d     	beq	0x34f0 <comport_devicename+0x1b0> @ imm = #0x74
    3478: e3590004     	cmp	r9, #4
    347c: 0a000015     	beq	0x34d8 <comport_devicename+0x198> @ imm = #0x54
    3480: e3590005     	cmp	r9, #5
    3484: 0a00000d     	beq	0x34c0 <comport_devicename+0x180> @ imm = #0x34
    3488: e3590006     	cmp	r9, #6
    348c: 0a000005     	beq	0x34a8 <comport_devicename+0x168> @ imm = #0x14
    3490: e5b61004     	ldr	r1, [r6, #0x4]!
    3494: e1a00007     	mov	r0, r7
    3498: ebfff562     	bl	0xa28 <.plt+0x20>       @ imm = #-0x2a78  // CALL strcmp
    349c: e3500000     	cmp	r0, #0
    34a0: 0a00005a     	beq	0x3610 <comport_devicename+0x2d0> @ imm = #0x168
    34a4: e0844004     	add	r4, r4, r4
    34a8: e5b61004     	ldr	r1, [r6, #0x4]!
    34ac: e1a00007     	mov	r0, r7
    34b0: ebfff55c     	bl	0xa28 <.plt+0x20>       @ imm = #-0x2a90  // CALL strcmp
    34b4: e3500000     	cmp	r0, #0
    34b8: 0a000054     	beq	0x3610 <comport_devicename+0x2d0> @ imm = #0x150
    34bc: e2844001     	add	r4, r4, #1
    34c0: e5b61004     	ldr	r1, [r6, #0x4]!
    34c4: e1a00007     	mov	r0, r7
    34c8: ebfff556     	bl	0xa28 <.plt+0x20>       @ imm = #-0x2aa8  // CALL strcmp
    34cc: e3500000     	cmp	r0, #0
    34d0: 0a00004e     	beq	0x3610 <comport_devicename+0x2d0> @ imm = #0x138
    34d4: e2844001     	add	r4, r4, #1
    34d8: e5b61004     	ldr	r1, [r6, #0x4]!
    34dc: e1a00007     	mov	r0, r7
    34e0: ebfff550     	bl	0xa28 <.plt+0x20>       @ imm = #-0x2ac0  // CALL strcmp
    34e4: e3500000     	cmp	r0, #0
    34e8: 0a000048     	beq	0x3610 <comport_devicename+0x2d0> @ imm = #0x120
    34ec: e2844001     	add	r4, r4, #1
    34f0: e5b61004     	ldr	r1, [r6, #0x4]!
    34f4: e1a00007     	mov	r0, r7
    34f8: ebfff54a     	bl	0xa28 <.plt+0x20>       @ imm = #-0x2ad8  // CALL strcmp
    34fc: e3500000     	cmp	r0, #0
    3500: 0a000042     	beq	0x3610 <comport_devicename+0x2d0> @ imm = #0x108
    3504: e2844001     	add	r4, r4, #1
    3508: e5b61004     	ldr	r1, [r6, #0x4]!
    350c: e1a00007     	mov	r0, r7
    3510: ebfff544     	bl	0xa28 <.plt+0x20>       @ imm = #-0x2af0  // CALL strcmp
    3514: e3500000     	cmp	r0, #0
    3518: 0a00003c     	beq	0x3610 <comport_devicename+0x2d0> @ imm = #0xf0
    351c: e2844001     	add	r4, r4, #1
    3520: e5b61004     	ldr	r1, [r6, #0x4]!
    3524: e1a00007     	mov	r0, r7
    3528: ebfff53e     	bl	0xa28 <.plt+0x20>       @ imm = #-0x2b08  // CALL strcmp
    352c: e3500000     	cmp	r0, #0
    3530: 0a000036     	beq	0x3610 <comport_devicename+0x2d0> @ imm = #0xd8
    3534: e2844001     	add	r4, r4, #1
    3538: e154000a     	cmp	r4, r10
    353c: 0a000032     	beq	0x360c <comport_devicename+0x2cc> @ imm = #0xc8
    3540: e5961004     	ldr	r1, [r6, #0x4]
    3544: e1a00007     	mov	r0, r7
    3548: ebfff536     	bl	0xa28 <.plt+0x20>       @ imm = #-0x2b28  // CALL strcmp
    354c: e3500000     	cmp	r0, #0
    3550: e1a00007     	mov	r0, r7
    3554: 0a00002d     	beq	0x3610 <comport_devicename+0x2d0> @ imm = #0xb4
    3558: e5961008     	ldr	r1, [r6, #0x8]
    355c: ebfff531     	bl	0xa28 <.plt+0x20>       @ imm = #-0x2b3c  // CALL strcmp
    3560: e2844001     	add	r4, r4, #1
    3564: e1a09004     	mov	r9, r4
    3568: e3500000     	cmp	r0, #0
    356c: e1a00007     	mov	r0, r7
    3570: 0a000026     	beq	0x3610 <comport_devicename+0x2d0> @ imm = #0x98
    3574: e596100c     	ldr	r1, [r6, #0xc]
    3578: ebfff52a     	bl	0xa28 <.plt+0x20>       @ imm = #-0x2b58  // CALL strcmp
    357c: e2844001     	add	r4, r4, #1
    3580: e3500000     	cmp	r0, #0
    3584: e1a00007     	mov	r0, r7
    3588: 0a000020     	beq	0x3610 <comport_devicename+0x2d0> @ imm = #0x80
    358c: e5961010     	ldr	r1, [r6, #0x10]
    3590: ebfff524     	bl	0xa28 <.plt+0x20>       @ imm = #-0x2b70  // CALL strcmp
    3594: e2894002     	add	r4, r9, #2
    3598: e3500000     	cmp	r0, #0
    359c: e1a00007     	mov	r0, r7
    35a0: 0a00001a     	beq	0x3610 <comport_devicename+0x2d0> @ imm = #0x68
    35a4: e5961014     	ldr	r1, [r6, #0x14]
    35a8: ebfff51e     	bl	0xa28 <.plt+0x20>       @ imm = #-0x2b88  // CALL strcmp
    35ac: e2894003     	add	r4, r9, #3
    35b0: e3500000     	cmp	r0, #0
    35b4: e1a00007     	mov	r0, r7
    35b8: 0a000014     	beq	0x3610 <comport_devicename+0x2d0> @ imm = #0x50
    35bc: e5961018     	ldr	r1, [r6, #0x18]
    35c0: ebfff518     	bl	0xa28 <.plt+0x20>       @ imm = #-0x2ba0  // CALL strcmp
    35c4: e2894004     	add	r4, r9, #4
    35c8: e3500000     	cmp	r0, #0
    35cc: e1a00007     	mov	r0, r7
    35d0: 0a00000e     	beq	0x3610 <comport_devicename+0x2d0> @ imm = #0x38
    35d4: e596101c     	ldr	r1, [r6, #0x1c]
    35d8: ebfff512     	bl	0xa28 <.plt+0x20>       @ imm = #-0x2bb8  // CALL strcmp
    35dc: e2894005     	add	r4, r9, #5
    35e0: e3500000     	cmp	r0, #0
    35e4: e1a00007     	mov	r0, r7
    35e8: 0a000008     	beq	0x3610 <comport_devicename+0x2d0> @ imm = #0x20
    35ec: e5b61020     	ldr	r1, [r6, #0x20]!
    35f0: ebfff50c     	bl	0xa28 <.plt+0x20>       @ imm = #-0x2bd0  // CALL strcmp
    35f4: e2894006     	add	r4, r9, #6
    35f8: e3500000     	cmp	r0, #0
    35fc: 0a000003     	beq	0x3610 <comport_devicename+0x2d0> @ imm = #0xc
    3600: e2894007     	add	r4, r9, #7
    3604: e154000a     	cmp	r4, r10
    3608: 1affffcc     	bne	0x3540 <comport_devicename+0x200> @ imm = #-0xd0
    360c: e59f41a0     	ldr	r4, [pc, #0x1a0]        @ 0x37b4 <comport_devicename+0x474>  // u32=0x270f; f32?=1.40115833e-41
    3610: e59d0008     	ldr	r0, [sp, #0x8]
    3614: ebfff575     	bl	0xbf0 <.plt+0x1e8>      @ imm = #-0x2a2c  // CALL globfree
    3618: e595009c     	ldr	r0, [r5, #0x9c]
    361c: e59f1194     	ldr	r1, [pc, #0x194]        @ 0x37b8 <comport_devicename+0x478>  // u32=0x902; f32?=3.23139426e-42
    3620: e5900000     	ldr	r0, [r0]
    3624: ebfff52f     	bl	0xae8 <.plt+0xe0>       @ imm = #-0x2b44  // CALL open
    3628: e3700001     	cmn	r0, #1
    362c: e1a06000     	mov	r6, r0
    3630: 0a000072     	beq	0x3800 <comport_devicename+0x4c0> @ imm = #0x1c8
    3634: e3a02b02     	mov	r2, #2048
    3638: e3a01004     	mov	r1, #4
    363c: ebfff532     	bl	0xb0c <.plt+0x104>      @ imm = #-0x2b38  // CALL fcntl
    3640: e59d100c     	ldr	r1, [sp, #0xc]
    3644: e1a00006     	mov	r0, r6
    3648: ebfff56b     	bl	0xbfc <.plt+0x1f4>      @ imm = #-0x2a54  // CALL tcgetattr
    364c: e3700001     	cmn	r0, #1
    3650: 0a000060     	beq	0x37d8 <comport_devicename+0x498> @ imm = #0x180
    3654: e1a0100b     	mov	r1, r11
    3658: e1a00006     	mov	r0, r6
    365c: ebfff566     	bl	0xbfc <.plt+0x1f4>      @ imm = #-0x2a68  // CALL tcgetattr
    3660: e3700001     	cmn	r0, #1
    3664: 0a00005b     	beq	0x37d8 <comport_devicename+0x498> @ imm = #0x16c
    3668: edd87a2a     	vldr	s15, [r8, #168]
    366c: e595206c     	ldr	r2, [r5, #0x6c]
    3670: e5951064     	ldr	r1, [r5, #0x64]
    3674: e5953068     	ldr	r3, [r5, #0x68]
    3678: eebd0ae7     	vcvt.s32.f32	s0, s15
    367c: e3c2c01b     	bic	r12, r2, #27
    3680: e3c1a001     	bic	r10, r1, #1
    3684: e3c37030     	bic	r7, r3, #48
    3688: e585c06c     	str	r12, [r5, #0x6c]
    368c: e585a064     	str	r10, [r5, #0x64]
    3690: ee10ea10     	vmov	lr, s0
    3694: e35e0006     	cmp	lr, #6
    3698: 03877e89     	orreq	r7, r7, #2192
    369c: 0a000005     	beq	0x36b8 <comport_devicename+0x378> @ imm = #0x14
    36a0: e35e0007     	cmp	lr, #7
    36a4: 03877e8a     	orreq	r7, r7, #2208
    36a8: 0a000002     	beq	0x36b8 <comport_devicename+0x378> @ imm = #0x8
    36ac: e35e0005     	cmp	lr, #5
    36b0: 03877d22     	orreq	r7, r7, #2176
    36b4: 13877e8b     	orrne	r7, r7, #2224
    36b8: edd80a2c     	vldr	s1, [r8, #176]
    36bc: e59800b8     	ldr	r0, [r8, #0xb8]
    36c0: e59820b4     	ldr	r2, [r8, #0xb4]
    36c4: e5951060     	ldr	r1, [r5, #0x60]
    36c8: eebd1ae0     	vcvt.s32.f32	s2, s1
    36cc: ed980a29     	vldr	s0, [r8, #164]
    36d0: ee119a10     	vmov	r9, s2
    36d4: e3590001     	cmp	r9, #1
    36d8: 03877040     	orreq	r7, r7, #64
    36dc: 13c77040     	bicne	r7, r7, #64
    36e0: e3500001     	cmp	r0, #1
    36e4: 03877102     	orreq	r7, r7, #-2147483648
    36e8: 13c77102     	bicne	r7, r7, #-2147483648
    36ec: e3520001     	cmp	r2, #1
    36f0: 03811b07     	orreq	r1, r1, #7168
    36f4: 13c11b07     	bicne	r1, r1, #7168
    36f8: e5851060     	str	r1, [r5, #0x60]
    36fc: e1a00005     	mov	r0, r5
    3700: e5857068     	str	r7, [r5, #0x68]
    3704: ebfff6f9     	bl	0x12f0 <set_baudrate>   @ imm = #-0x241c
    3708: e3a0c000     	mov	r12, #0
    370c: e1a0200b     	mov	r2, r11
    3710: e588c0d0     	str	r12, [r8, #0xd0]
    3714: e3a01002     	mov	r1, #2
    3718: e1a00006     	mov	r0, r6
    371c: ebfff4e2     	bl	0xaac <.plt+0xa4>       @ imm = #-0x2c78  // CALL tcsetattr
    3720: e595a09c     	ldr	r10, [r5, #0x9c]
    3724: e3700001     	cmn	r0, #1
    3728: e1a0b000     	mov	r11, r0
    372c: 0a000041     	beq	0x3838 <comport_devicename+0x4f8> @ imm = #0x104
    3730: e59fe084     	ldr	lr, [pc, #0x84]         @ 0x37bc <comport_devicename+0x47c>  // u32=0xde8; f32?=4.98862253e-42
    3734: e1a01004     	mov	r1, r4
    3738: e08f000e     	add	r0, pc, lr
    373c: e59a2000     	ldr	r2, [r10]
    3740: ebfff4fd     	bl	0xb3c <.plt+0x134>      @ imm = #-0x2c0c  // CALL post
    3744: e1c84ab0     	strh	r4, [r8, #160]
    3748: e2852d43     	add	r2, r5, #4288
    374c: e5856020     	str	r6, [r5, #0x20]
    3750: e59800c4     	ldr	r0, [r8, #0xc4]
    3754: ed920b06     	vldr	d0, [r2, #24]
    3758: ebfff4ca     	bl	0xa88 <.plt+0x80>       @ imm = #-0x2cd8  // CALL clock_delay
    375c: e28dd03c     	add	sp, sp, #60
    3760: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
    3764: e59fa054     	ldr	r10, [pc, #0x54]        @ 0x37c0 <comport_devicename+0x480>  // u32=0x63c; f32?=2.23647235e-42
    3768: e1a02007     	mov	r2, r7
    376c: e08f100a     	add	r1, pc, r10
    3770: e1a00005     	mov	r0, r5
    3774: ebfff51a     	bl	0xbe4 <.plt+0x1dc>      @ imm = #-0x2b98  // CALL pd_error
    3778: eaffff25     	b	0x3414 <comport_devicename+0xd4> @ imm = #-0x36c
    377c: e59fc040     	ldr	r12, [pc, #0x40]        @ 0x37c4 <comport_devicename+0x484>  // u32=0x660; f32?=2.28691909e-42
    3780: e1a02007     	mov	r2, r7
    3784: e08f100c     	add	r1, pc, r12
    3788: e1a00005     	mov	r0, r5
    378c: ebfff514     	bl	0xbe4 <.plt+0x1dc>      @ imm = #-0x2bb0  // CALL pd_error
    3790: eaffff1f     	b	0x3414 <comport_devicename+0xd4> @ imm = #-0x384
    3794: e59fe02c     	ldr	lr, [pc, #0x2c]         @ 0x37c8 <comport_devicename+0x488>  // u32=0x630; f32?=2.21965677e-42
    3798: e1a02007     	mov	r2, r7
    379c: e08f100e     	add	r1, pc, lr
    37a0: e1a00005     	mov	r0, r5
    37a4: ebfff50e     	bl	0xbe4 <.plt+0x1dc>      @ imm = #-0x2bc8  // CALL pd_error
    37a8: eaffff19     	b	0x3414 <comport_devicename+0xd4> @ imm = #-0x39c
    37ac: 00 00 80 bf  	.word	0xbf800000
    37b0: a8 12 00 00  	.word	0x000012a8
    37b4: 0f 27 00 00  	.word	0x0000270f
    37b8: 02 09 00 00  	.word	0x00000902
    37bc: e8 0d 00 00  	.word	0x00000de8
    37c0: 3c 06 00 00  	.word	0x0000063c
    37c4: 60 06 00 00  	.word	0x00000660
    37c8: 30 06 00 00  	.word	0x00000630
    37cc: f8 0c 00 00  	.word	0x00000cf8
    37d0: 74 0c 00 00  	.word	0x00000c74
    37d4: 10 0d 00 00  	.word	0x00000d10
    37d8: e595309c     	ldr	r3, [r5, #0x9c]
    37dc: e51f7018     	ldr	r7, [pc, #-0x18]        @ 0x37cc <comport_devicename+0x48c>  // u32=0xcf8; f32?=4.6523109e-42
    37e0: e1a00005     	mov	r0, r5
    37e4: e08f1007     	add	r1, pc, r7
    37e8: e5932000     	ldr	r2, [r3]
    37ec: ebfff4fc     	bl	0xbe4 <.plt+0x1dc>      @ imm = #-0x2c10  // CALL pd_error
    37f0: e1a00006     	mov	r0, r6
    37f4: ebfff4f7     	bl	0xbd8 <.plt+0x1d0>      @ imm = #-0x2c24  // CALL close
    37f8: e3e06000     	mvn	r6, #0
    37fc: eaffffd1     	b	0x3748 <comport_devicename+0x408> @ imm = #-0xbc
    3800: ebfff4c4     	bl	0xb18 <.plt+0x110>      @ imm = #-0x2cf0  // CALL __errno_location
    3804: e595909c     	ldr	r9, [r5, #0x9c]
    3808: e599b000     	ldr	r11, [r9]
    380c: e590a000     	ldr	r10, [r0]
    3810: e1a0000a     	mov	r0, r10
    3814: ebfff4aa     	bl	0xac4 <.plt+0xbc>       @ imm = #-0x2d58  // CALL strerror
    3818: e51f1050     	ldr	r1, [pc, #-0x50]        @ 0x37d0 <comport_devicename+0x490>  // u32=0xc74; f32?=4.4673395e-42
    381c: e1a0300a     	mov	r3, r10
    3820: e1a0200b     	mov	r2, r11
    3824: e08f1001     	add	r1, pc, r1
    3828: e58d0000     	str	r0, [sp]
    382c: e1a00005     	mov	r0, r5
    3830: ebfff4eb     	bl	0xbe4 <.plt+0x1dc>      @ imm = #-0x2c54  // CALL pd_error
    3834: eaffffc3     	b	0x3748 <comport_devicename+0x408> @ imm = #-0xf4
    3838: e51f406c     	ldr	r4, [pc, #-0x6c]        @ 0x37d4 <comport_devicename+0x494>  // u32=0xd10; f32?=4.68594206e-42
    383c: e1a00005     	mov	r0, r5
    3840: e08f1004     	add	r1, pc, r4
    3844: e59a2000     	ldr	r2, [r10]
    3848: ebfff4e5     	bl	0xbe4 <.plt+0x1dc>      @ imm = #-0x2c6c  // CALL pd_error
    384c: e1a00006     	mov	r0, r6
    3850: ebfff4e0     	bl	0xbd8 <.plt+0x1d0>      @ imm = #-0x2c80  // CALL close
    3854: e1a0600b     	mov	r6, r11
    3858: eaffffba     	b	0x3748 <comport_devicename+0x408> @ imm = #-0x118

