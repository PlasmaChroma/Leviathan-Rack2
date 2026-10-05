; lubadh::Channel::initHardware()
; VA 0x37084 size 148

   37084: e5903000     	ldr	r3, [r0]
   37088: e3530000     	cmp	r3, #0
   3708c: 0a00000d     	beq	0x370c8
   37090: e3530001     	cmp	r3, #1
   37094: 112fff1e     	bxne	lr
   37098: e2800a2a     	add	r0, r0, #172032
   3709c: e3082d89     	movw	r2, #0x8d89
   370a0: e349218f     	movt	r2, #0x918f
   370a4: e2801d12     	add	r1, r0, #1152
   370a8: eddf0b12     	vldr	d16, [pc, #72]          @ 0x370f8 ; float 1.51693947382e-134
   370ac: eddf1b13     	vldr	d17, [pc, #76]          @ 0x37100 ; float -1.98878707915e-272
   370b0: e3093593     	movw	r3, #0x9593
   370b4: e3493997     	movt	r3, #0x9997
   370b8: f4410adf     	vst1.64	{d16, d17}, [r1:64]
   370bc: e5802490     	str	r2, [r0, #0x490]
   370c0: e5803494     	str	r3, [r0, #0x494]
   370c4: e12fff1e     	bx	lr
   370c8: e2800a2a     	add	r0, r0, #172032
   370cc: e3082c88     	movw	r2, #0x8c88
   370d0: e349208e     	movt	r2, #0x908e
   370d4: e2801d12     	add	r1, r0, #1152
   370d8: eddf0b0a     	vldr	d16, [pc, #40]          @ 0x37108 ; float 9.72811825286e-154
   370dc: eddf1b0b     	vldr	d17, [pc, #44]          @ 0x37110 ; float -2.60989664381e-277
   370e0: e3093492     	movw	r3, #0x9492
   370e4: e3493896     	movt	r3, #0x9896
   370e8: f4410adf     	vst1.64	{d16, d17}, [r1:64]
   370ec: e5802490     	str	r2, [r0, #0x490]
   370f0: e5803494     	str	r3, [r0, #0x494]
   370f4: e12fff1e     	bx	lr
   370f8: 00 01 02 03  	.word	0x03020100
   370fc: 29 0d 26 24  	.word	0x24260d29
   37100: 22 2b 17 25  	.word	0x25172b22
   37104: 83 84 85 87  	.word	0x87858483
   37108: 05 04 07 06  	.word	0x06070405
   3710c: 28 16 2a 20  	.word	0x202a1628
   37110: 21 1b 19 23  	.word	0x23191b21
   37114: 80 81 82 86  	.word	0x86828180
