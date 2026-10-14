0000b6c0 <tick_getBCM2835>:
    b6c0: e92d4070     	push	{r4, r5, r6, lr}
    b6c4: e1a04000     	mov	r4, r0
    b6c8: ed2d8b02     	vpush	{d8}
    b6cc: ed9f0af5     	vldr	s0, [pc, #980]          @ 0xbaa8 <tick_getBCM2835+0x3e8>  // f32=123
    b6d0: e24dd008     	sub	sp, sp, #8
    b6d4: ebffe043     	bl	0x37e8 <.plt+0xec>      @ imm = #-0x7ef4  // CALL readFromSharedMem
    b6d8: eeb50a40     	vcmp.f32	s0, #0
    b6dc: eef1fa10     	vmrs	APSR_nzcv, fpscr
    b6e0: 1a000004     	bne	0xb6f8 <tick_getBCM2835+0x38> @ imm = #0x10
    b6e4: e5d43030     	ldrb	r3, [r4, #0x30]
    b6e8: e1a00004     	mov	r0, r4
    b6ec: e3530063     	cmp	r3, #99
    b6f0: 0a00008f     	beq	0xb934 <tick_getBCM2835+0x274> @ imm = #0x23c
    b6f4: ebffe191     	bl	0x3d40 <.plt+0x644>     @ imm = #-0x79bc  // CALL _getAllAdcs
    b6f8: e1a00004     	mov	r0, r4
    b6fc: eeb30a0b     	vmov.f32	s0, #2.700000e+01
    b700: ebffe038     	bl	0x37e8 <.plt+0xec>      @ imm = #-0x7f20  // CALL readFromSharedMem
    b704: e1a00004     	mov	r0, r4
    b708: eef08a40     	vmov.f32	s17, s0
    b70c: eeb30a0c     	vmov.f32	s0, #2.800000e+01
    b710: ebffe034     	bl	0x37e8 <.plt+0xec>      @ imm = #-0x7f30  // CALL readFromSharedMem
    b714: e5d40030     	ldrb	r0, [r4, #0x30]
    b718: e3500062     	cmp	r0, #98
    b71c: eeb08a40     	vmov.f32	s16, s0
    b720: 8a00001b     	bhi	0xb794 <tick_getBCM2835+0xd4> @ imm = #0x6c
    b724: e59f1384     	ldr	r1, [pc, #0x384]        @ 0xbab0 <tick_getBCM2835+0x3f0>  // u32=0x1bc88; f32?=1.59467765e-40
    b728: e08f2001     	add	r2, pc, r1
    b72c: eefc7ae8     	vcvt.u32.f32	s15, s17
    b730: e5d2009c     	ldrb	r0, [r2, #0x9c]
    b734: edcd7a01     	vstr	s15, [sp, #4]
    b738: e5dd5004     	ldrb	r5, [sp, #0x4]
    b73c: e1500005     	cmp	r0, r5
    b740: 05d4003c     	ldrbeq	r0, [r4, #0x3c]
    b744: 1a00003a     	bne	0xb834 <tick_getBCM2835+0x174> @ imm = #0xe8
    b748: e3500000     	cmp	r0, #0
    b74c: 0a000049     	beq	0xb878 <tick_getBCM2835+0x1b8> @ imm = #0x124
    b750: eebc0ac8     	vcvt.u32.f32	s0, s16
    b754: e59f5358     	ldr	r5, [pc, #0x358]        @ 0xbab4 <tick_getBCM2835+0x3f4>  // u32=0x1bc58; f32?=1.59400503e-40
    b758: e08f6005     	add	r6, pc, r5
    b75c: e5d6c09d     	ldrb	r12, [r6, #0x9d]
    b760: ed8d0a01     	vstr	s0, [sp, #4]
    b764: e5dde004     	ldrb	lr, [sp, #0x4]
    b768: e15c000e     	cmp	r12, lr
    b76c: 0a000008     	beq	0xb794 <tick_getBCM2835+0xd4> @ imm = #0x20
    b770: e35e0000     	cmp	lr, #0
    b774: 0a000003     	beq	0xb788 <tick_getBCM2835+0xc8> @ imm = #0xc
    b778: e5d43054     	ldrb	r3, [r4, #0x54]
    b77c: e3530000     	cmp	r3, #0
    b780: 13a03000     	movne	r3, #0
    b784: 15843058     	strne	r3, [r4, #0x58]
    b788: e59f0328     	ldr	r0, [pc, #0x328]        @ 0xbab8 <tick_getBCM2835+0x3f8>  // u32=0x1bc24; f32?=1.59327635e-40
    b78c: e08f1000     	add	r1, pc, r0
    b790: e5c1e09d     	strb	lr, [r1, #0x9d]
    b794: e3a00021     	mov	r0, #33
    b798: e2845d75     	add	r5, r4, #7488
    b79c: ebffdff9     	bl	0x3788 <.plt+0x8c>      @ imm = #-0x801c  // CALL bcm2835_gpio_lev
    b7a0: ebffe0a9     	bl	0x3a4c <.plt+0x350>     @ imm = #-0x7d5c  // CALL shiftFilter
    b7a4: e2851020     	add	r1, r5, #32
    b7a8: e1a02000     	mov	r2, r0
    b7ac: e1a00004     	mov	r0, r4
    b7b0: ebffe141     	bl	0x3cbc <.plt+0x5c0>     @ imm = #-0x7afc  // CALL _getButton
    b7b4: e3a0002a     	mov	r0, #42
    b7b8: ebffdff2     	bl	0x3788 <.plt+0x8c>      @ imm = #-0x8038  // CALL bcm2835_gpio_lev
    b7bc: ebffdfee     	bl	0x377c <.plt+0x80>      @ imm = #-0x8048  // CALL captureFilter
    b7c0: e2851030     	add	r1, r5, #48
    b7c4: e1a02000     	mov	r2, r0
    b7c8: e1a00004     	mov	r0, r4
    b7cc: ebffe13a     	bl	0x3cbc <.plt+0x5c0>     @ imm = #-0x7b18  // CALL _getButton
    b7d0: e3a00026     	mov	r0, #38
    b7d4: ebffdfeb     	bl	0x3788 <.plt+0x8c>      @ imm = #-0x8054  // CALL bcm2835_gpio_lev
    b7d8: ebffe038     	bl	0x38c0 <.plt+0x1c4>     @ imm = #-0x7f20  // CALL strikeFilter
    b7dc: e2841d76     	add	r1, r4, #7552
    b7e0: e1a02000     	mov	r2, r0
    b7e4: e1a00004     	mov	r0, r4
    b7e8: ebffe133     	bl	0x3cbc <.plt+0x5c0>     @ imm = #-0x7b34  // CALL _getButton
    b7ec: e5d42030     	ldrb	r2, [r4, #0x30]
    b7f0: e3520062     	cmp	r2, #98
    b7f4: 9a000022     	bls	0xb884 <tick_getBCM2835+0x1c4> @ imm = #0x88
    b7f8: eeb10b04     	vmov.f64	d0, #5.000000e+00
    b7fc: e2846a01     	add	r6, r4, #4096
    b800: e284cd77     	add	r12, r4, #7616
    b804: e286eedd     	add	lr, r6, #3536
    b808: e3a05000     	mov	r5, #0
    b80c: e3a03000     	mov	r3, #0
    b810: e34450a0     	movt	r5, #0x40a0
    b814: e3443040     	movt	r3, #0x4040
    b818: e58c5008     	str	r5, [r12, #0x8]
    b81c: e58e3000     	str	r3, [lr]
    b820: e59400c8     	ldr	r0, [r4, #0xc8]
    b824: e28dd008     	add	sp, sp, #8
    b828: ecbd8b02     	vpop	{d8}
    b82c: e8bd4070     	pop	{r4, r5, r6, lr}
    b830: eaffe03d     	b	0x392c <.plt+0x230>     @ imm = #-0x7f0c  // CALL clock_delay
    b834: e3a01015     	mov	r1, #21
    b838: e1a02005     	mov	r2, r5
    b83c: e1a00004     	mov	r0, r4
    b840: ebffe0a8     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x7d60  // CALL writeToSharedMem
    b844: e5d4102a     	ldrb	r1, [r4, #0x2a]
    b848: e3510001     	cmp	r1, #1
    b84c: 0a00003a     	beq	0xb93c <tick_getBCM2835+0x27c> @ imm = #0xe8
    b850: e3510002     	cmp	r1, #2
    b854: 0a000061     	beq	0xb9e0 <tick_getBCM2835+0x320> @ imm = #0x184
    b858: e3550000     	cmp	r5, #0
    b85c: 1a000076     	bne	0xba3c <tick_getBCM2835+0x37c> @ imm = #0x1d8
    b860: e5d4003c     	ldrb	r0, [r4, #0x3c]
    b864: e59f1250     	ldr	r1, [pc, #0x250]        @ 0xbabc <tick_getBCM2835+0x3fc>  // u32=0x1bb44; f32?=1.59013745e-40
    b868: e3500000     	cmp	r0, #0
    b86c: e08f2001     	add	r2, pc, r1
    b870: e5c2509c     	strb	r5, [r2, #0x9c]
    b874: 1affffb5     	bne	0xb750 <tick_getBCM2835+0x90> @ imm = #-0x12c
    b878: e1a00004     	mov	r0, r4
    b87c: ebffe11d     	bl	0x3cf8 <.plt+0x5fc>     @ imm = #-0x7b8c  // CALL led_controlForRecording
    b880: eaffffb2     	b	0xb750 <tick_getBCM2835+0x90> @ imm = #-0x138
    b884: e1a00004     	mov	r0, r4
    b888: ed9f0a87     	vldr	s0, [pc, #540]          @ 0xbaac <tick_getBCM2835+0x3ec>  // f32=109
    b88c: ebffdfd5     	bl	0x37e8 <.plt+0xec>      @ imm = #-0x80ac  // CALL readFromSharedMem
    b890: e5d400ba     	ldrb	r0, [r4, #0xba]
    b894: e3500000     	cmp	r0, #0
    b898: 13a05000     	movne	r5, #0
    b89c: eef08a40     	vmov.f32	s17, s0
    b8a0: 0a000031     	beq	0xb96c <tick_getBCM2835+0x2ac> @ imm = #0xc4
    b8a4: e59410b4     	ldr	r1, [r4, #0xb4]
    b8a8: e5d12001     	ldrb	r2, [r1, #0x1]
    b8ac: e3520000     	cmp	r2, #0
    b8b0: 0a000017     	beq	0xb914 <tick_getBCM2835+0x254> @ imm = #0x5c
    b8b4: eebd1ae8     	vcvt.s32.f32	s2, s17
    b8b8: e5d4c027     	ldrb	r12, [r4, #0x27]
    b8bc: e35c0000     	cmp	r12, #0
    b8c0: ee116a10     	vmov	r6, s2
    b8c4: 1a00002e     	bne	0xb984 <tick_getBCM2835+0x2c4> @ imm = #0xb8
    b8c8: e5d4004c     	ldrb	r0, [r4, #0x4c]
    b8cc: e3500000     	cmp	r0, #0
    b8d0: 1a00000f     	bne	0xb914 <tick_getBCM2835+0x254> @ imm = #0x3c
    b8d4: e3560000     	cmp	r6, #0
    b8d8: da00005a     	ble	0xba48 <tick_getBCM2835+0x388> @ imm = #0x168
    b8dc: e59f11dc     	ldr	r1, [pc, #0x1dc]        @ 0xbac0 <tick_getBCM2835+0x400>  // u32=0x1bad0; f32?=1.58851194e-40
    b8e0: e08f2001     	add	r2, pc, r1
    b8e4: e592c0a0     	ldr	r12, [r2, #0xa0]
    b8e8: e15c0006     	cmp	r12, r6
    b8ec: 0a000055     	beq	0xba48 <tick_getBCM2835+0x388> @ imm = #0x154
    b8f0: e5d45024     	ldrb	r5, [r4, #0x24]
    b8f4: e3550000     	cmp	r5, #0
    b8f8: 1a000055     	bne	0xba54 <tick_getBCM2835+0x394> @ imm = #0x154
    b8fc: ed9f0b67     	vldr	d0, [pc, #412]          @ 0xbaa0 <tick_getBCM2835+0x3e0>  // f64=50
    b900: e59400f8     	ldr	r0, [r4, #0xf8]
    b904: ebffe008     	bl	0x392c <.plt+0x230>     @ imm = #-0x7fe0  // CALL clock_delay
    b908: e59fe1b4     	ldr	lr, [pc, #0x1b4]        @ 0xbac4 <tick_getBCM2835+0x404>  // u32=0x1baa4; f32?=1.58789537e-40
    b90c: e08f000e     	add	r0, pc, lr
    b910: e58060a0     	str	r6, [r0, #0xa0]
    b914: e2846d77     	add	r6, r4, #7616
    b918: e3a02000     	mov	r2, #0
    b91c: e3a0106a     	mov	r1, #106
    b920: e1a00004     	mov	r0, r4
    b924: ebffe06f     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x7e44  // CALL writeToSharedMem
    b928: edd61a02     	vldr	s3, [r6, #8]
    b92c: eeb70ae1     	vcvt.f64.f32	d0, s3
    b930: eaffffba     	b	0xb820 <tick_getBCM2835+0x160> @ imm = #-0x118
    b934: ebffe056     	bl	0x3a94 <.plt+0x398>     @ imm = #-0x7ea8  // CALL _getAllAdcsDuringCalibration
    b938: eaffff6e     	b	0xb6f8 <tick_getBCM2835+0x38> @ imm = #-0x248
    b93c: e5d4e024     	ldrb	lr, [r4, #0x24]
    b940: e35e0000     	cmp	lr, #0
    b944: 1a000035     	bne	0xba20 <tick_getBCM2835+0x360> @ imm = #0xd4
    b948: e3550000     	cmp	r5, #0
    b94c: 0a000035     	beq	0xba28 <tick_getBCM2835+0x368> @ imm = #0xd4
    b950: e5d43027     	ldrb	r3, [r4, #0x27]
    b954: e1a00004     	mov	r0, r4
    b958: e3530000     	cmp	r3, #0
    b95c: 0a000042     	beq	0xba6c <tick_getBCM2835+0x3ac> @ imm = #0x108
    b960: ebffdfe5     	bl	0x38fc <.plt+0x200>     @ imm = #-0x806c  // CALL _setRecordingState
    b964: e5d4003c     	ldrb	r0, [r4, #0x3c]
    b968: eaffffbd     	b	0xb864 <tick_getBCM2835+0x1a4> @ imm = #-0x10c
    b96c: ed9f0a59     	vldr	s0, [pc, #356]          @ 0xbad8 <tick_getBCM2835+0x418>  // f32=106
    b970: e1a00004     	mov	r0, r4
    b974: ebffdf9b     	bl	0x37e8 <.plt+0xec>      @ imm = #-0x8194  // CALL readFromSharedMem
    b978: eefd0ac0     	vcvt.s32.f32	s1, s0
    b97c: ee105a90     	vmov	r5, s1
    b980: eaffffc7     	b	0xb8a4 <tick_getBCM2835+0x1e4> @ imm = #-0xe4
    b984: e35c0001     	cmp	r12, #1
    b988: 1affffe1     	bne	0xb914 <tick_getBCM2835+0x254> @ imm = #-0x7c
    b98c: e5d4e04c     	ldrb	lr, [r4, #0x4c]
    b990: e35e0000     	cmp	lr, #0
    b994: 1affffde     	bne	0xb914 <tick_getBCM2835+0x254> @ imm = #-0x88
    b998: e59f3128     	ldr	r3, [pc, #0x128]        @ 0xbac8 <tick_getBCM2835+0x408>  // u32=0x1ba14; f32?=1.5858775e-40
    b99c: e08f0003     	add	r0, pc, r3
    b9a0: e59010a0     	ldr	r1, [r0, #0xa0]
    b9a4: e3550000     	cmp	r5, #0
    b9a8: d1510006     	cmple	r1, r6
    b9ac: 0a000007     	beq	0xb9d0 <tick_getBCM2835+0x310> @ imm = #0x1c
    b9b0: e3560000     	cmp	r6, #0
    b9b4: da000048     	ble	0xbadc <tick_getBCM2835+0x41c> @ imm = #0x120
    b9b8: e1a00004     	mov	r0, r4
    b9bc: e3a01001     	mov	r1, #1
    b9c0: ebffdfcd     	bl	0x38fc <.plt+0x200>     @ imm = #-0x80cc  // CALL _setRecordingState
    b9c4: e59fc100     	ldr	r12, [pc, #0x100]       @ 0xbacc <tick_getBCM2835+0x40c>  // u32=0x9bf4; f32?=5.59454399e-41
    b9c8: e08f000c     	add	r0, pc, r12
    b9cc: ebffe069     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x7e5c  // CALL post
    b9d0: e59fe0f8     	ldr	lr, [pc, #0xf8]         @ 0xbad0 <tick_getBCM2835+0x410>  // u32=0x1b9dc; f32?=1.58509277e-40
    b9d4: e08f300e     	add	r3, pc, lr
    b9d8: e58360a0     	str	r6, [r3, #0xa0]
    b9dc: eaffffcc     	b	0xb914 <tick_getBCM2835+0x254> @ imm = #-0xd0
    b9e0: e3550000     	cmp	r5, #0
    b9e4: 0affff9d     	beq	0xb860 <tick_getBCM2835+0x1a0> @ imm = #-0x18c
    b9e8: e5d46024     	ldrb	r6, [r4, #0x24]
    b9ec: e3560000     	cmp	r6, #0
    b9f0: 1a000020     	bne	0xba78 <tick_getBCM2835+0x3b8> @ imm = #0x80
    b9f4: e5d4003c     	ldrb	r0, [r4, #0x3c]
    b9f8: e3500000     	cmp	r0, #0
    b9fc: 1affff98     	bne	0xb864 <tick_getBCM2835+0x1a4> @ imm = #-0x1a0
    ba00: eeb70a00     	vmov.f32	s0, #1.000000e+00
    ba04: e1a00004     	mov	r0, r4
    ba08: ebffe0b7     	bl	0x3cec <.plt+0x5f0>     @ imm = #-0x7d24  // CALL _setCaptureLed
    ba0c: e59400e8     	ldr	r0, [r4, #0xe8]
    ba10: eeb30b09     	vmov.f64	d0, #2.500000e+01
    ba14: ebffdfc4     	bl	0x392c <.plt+0x230>     @ imm = #-0x80f0  // CALL clock_delay
    ba18: e5d4003c     	ldrb	r0, [r4, #0x3c]
    ba1c: eaffff90     	b	0xb864 <tick_getBCM2835+0x1a4> @ imm = #-0x1c0
    ba20: e3550000     	cmp	r5, #0
    ba24: 1affff8d     	bne	0xb860 <tick_getBCM2835+0x1a0> @ imm = #-0x1cc
    ba28: e1a00004     	mov	r0, r4
    ba2c: e3a01000     	mov	r1, #0
    ba30: ebffdfb1     	bl	0x38fc <.plt+0x200>     @ imm = #-0x813c  // CALL _setRecordingState
    ba34: e5d4003c     	ldrb	r0, [r4, #0x3c]
    ba38: eaffff89     	b	0xb864 <tick_getBCM2835+0x1a4> @ imm = #-0x1dc
    ba3c: e1a00004     	mov	r0, r4
    ba40: ebffdfb6     	bl	0x3920 <.plt+0x224>     @ imm = #-0x8128  // CALL _toggleRecordingState
    ba44: eaffff85     	b	0xb860 <tick_getBCM2835+0x1a0> @ imm = #-0x1ec
    ba48: e3550000     	cmp	r5, #0
    ba4c: caffffa7     	bgt	0xb8f0 <tick_getBCM2835+0x230> @ imm = #-0x164
    ba50: eaffffac     	b	0xb908 <tick_getBCM2835+0x248> @ imm = #-0x150
    ba54: e1a00004     	mov	r0, r4
    ba58: ebffdfb0     	bl	0x3920 <.plt+0x224>     @ imm = #-0x8140  // CALL _toggleRecordingState
    ba5c: e5d43024     	ldrb	r3, [r4, #0x24]
    ba60: e3530000     	cmp	r3, #0
    ba64: 1affffa7     	bne	0xb908 <tick_getBCM2835+0x248> @ imm = #-0x164
    ba68: eaffffa3     	b	0xb8fc <tick_getBCM2835+0x23c> @ imm = #-0x174
    ba6c: ebffe0d4     	bl	0x3dc4 <.plt+0x6c8>     @ imm = #-0x7cb0  // CALL _setRecordingStateWithReset
    ba70: e5d4003c     	ldrb	r0, [r4, #0x3c]
    ba74: eaffff7a     	b	0xb864 <tick_getBCM2835+0x1a4> @ imm = #-0x218
    ba78: e3a01000     	mov	r1, #0
    ba7c: e1a00004     	mov	r0, r4
    ba80: ebffdf9d     	bl	0x38fc <.plt+0x200>     @ imm = #-0x818c  // CALL _setRecordingState
    ba84: e3a0c000     	mov	r12, #0
    ba88: e59400f8     	ldr	r0, [r4, #0xf8]
    ba8c: eeb30b09     	vmov.f64	d0, #2.500000e+01
    ba90: e584c02c     	str	r12, [r4, #0x2c]
    ba94: ebffdfa4     	bl	0x392c <.plt+0x230>     @ imm = #-0x8170  // CALL clock_delay
    ba98: e5d4003c     	ldrb	r0, [r4, #0x3c]
    ba9c: eaffff70     	b	0xb864 <tick_getBCM2835+0x1a4> @ imm = #-0x240
    baa0: 00 00 00 00  	.word	0x00000000
    baa4: 00 00 49 40  	.word	0x40490000
    baa8: 00 00 f6 42  	.word	0x42f60000
    baac: 00 00 da 42  	.word	0x42da0000
    bab0: 88 bc 01 00  	.word	0x0001bc88
    bab4: 58 bc 01 00  	.word	0x0001bc58
    bab8: 24 bc 01 00  	.word	0x0001bc24
    babc: 44 bb 01 00  	.word	0x0001bb44
    bac0: d0 ba 01 00  	.word	0x0001bad0
    bac4: a4 ba 01 00  	.word	0x0001baa4
    bac8: 14 ba 01 00  	.word	0x0001ba14
    bacc: f4 9b 00 00  	.word	0x00009bf4
    bad0: dc b9 01 00  	.word	0x0001b9dc
    bad4: d0 9a 00 00  	.word	0x00009ad0
    bad8: 00 00 d4 42  	.word	0x42d40000
    badc: ed1f0a03     	vldr	s0, [pc, #-12]          @ 0xbad8 <tick_getBCM2835+0x418>  // f32=106
    bae0: e1a00004     	mov	r0, r4
    bae4: ebffdf3f     	bl	0x37e8 <.plt+0xec>      @ imm = #-0x8304  // CALL readFromSharedMem
    bae8: eeb50ac0     	vcmpe.f32	s0, #0
    baec: eef1fa10     	vmrs	APSR_nzcv, fpscr
    baf0: caffffb0     	bgt	0xb9b8 <tick_getBCM2835+0x2f8> @ imm = #-0x140
    baf4: e1955006     	orrs	r5, r5, r6
    baf8: 1affffb4     	bne	0xb9d0 <tick_getBCM2835+0x310> @ imm = #-0x130
    bafc: e51f2030     	ldr	r2, [pc, #-0x30]        @ 0xbad4 <tick_getBCM2835+0x414>  // u32=0x9ad0; f32?=5.55362607e-41
    bb00: e08f0002     	add	r0, pc, r2
    bb04: ebffe01b     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x7f94  // CALL post
    bb08: e1a01005     	mov	r1, r5
    bb0c: e1a00004     	mov	r0, r4
    bb10: ebffdf79     	bl	0x38fc <.plt+0x200>     @ imm = #-0x821c  // CALL _setRecordingState
    bb14: eaffffad     	b	0xb9d0 <tick_getBCM2835+0x310> @ imm = #-0x14c

