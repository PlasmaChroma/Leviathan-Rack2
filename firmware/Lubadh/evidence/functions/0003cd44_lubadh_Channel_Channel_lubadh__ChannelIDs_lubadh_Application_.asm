; lubadh::Channel::Channel(lubadh::_ChannelIDs, lubadh::Application&)
; VA 0x3cd44 size 4172

   3cd44: e3a03018     	mov	r3, #24
   3cd48: e59fc730     	ldr	r12, [pc, #0x730]       @ 0x3d480
   3cd4c: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   3cd50: e1a06002     	mov	r6, r2
   3cd54: e1a04000     	mov	r4, r0
   3cd58: e0030193     	mul	r3, r3, r1
   3cd5c: ed2d8b02     	vpush	{d8}
   3cd60: e280e00c     	add	lr, r0, #12
   3cd64: e1a05000     	mov	r5, r0
   3cd68: e08c2003     	add	r2, r12, r3
   3cd6c: e4801004     	str	r1, [r0], #4
   3cd70: e24ddf95     	sub	sp, sp, #596
   3cd74: e584e004     	str	lr, [r4, #0x4]
   3cd78: e79c1003     	ldr	r1, [r12, r3]
   3cd7c: e5922004     	ldr	r2, [r2, #0x4]
   3cd80: e58d0014     	str	r0, [sp, #0x14]
   3cd84: e0812002     	add	r2, r1, r2
   3cd88: e58de040     	str	lr, [sp, #0x40]
   3cd8c: ebffe859     	bl	0x36ef8
   3cd90: e4951024     	ldr	r1, [r5], #36
   3cd94: e5846020     	str	r6, [r4, #0x20]
   3cd98: e1a00005     	mov	r0, r5
   3cd9c: ebfffc09     	bl	0x3bdc8
   3cda0: e28400f0     	add	r0, r4, #240
   3cda4: e58450e8     	str	r5, [r4, #0xe8]
   3cda8: ebffe712     	bl	0x369f8
   3cdac: e2840c01     	add	r0, r4, #256
   3cdb0: ebffe710     	bl	0x369f8
   3cdb4: e2840f46     	add	r0, r4, #280
   3cdb8: ebffe70e     	bl	0x369f8
   3cdbc: e2843f4a     	add	r3, r4, #296
   3cdc0: e59f26bc     	ldr	r2, [pc, #0x6bc]        @ 0x3d484
   3cdc4: e1a00003     	mov	r0, r3
   3cdc8: e5842118     	str	r2, [r4, #0x118]
   3cdcc: e58d303c     	str	r3, [sp, #0x3c]
   3cdd0: eb00bb60     	bl	0x6bb58
   3cdd4: e59f36ac     	ldr	r3, [pc, #0x6ac]        @ 0x3d488
   3cdd8: e2840e19     	add	r0, r4, #400
   3cddc: e5843128     	str	r3, [r4, #0x128]
   3cde0: ebffe704     	bl	0x369f8
   3cde4: e2840e1a     	add	r0, r4, #416
   3cde8: ebffe702     	bl	0x369f8
   3cdec: e2840e1b     	add	r0, r4, #432
   3cdf0: ebffe700     	bl	0x369f8
   3cdf4: e2840d07     	add	r0, r4, #448
   3cdf8: ebffe6fe     	bl	0x369f8
   3cdfc: f2c00050     	vmov.i32	q8, #0x0
   3ce00: e2842f7b     	add	r2, r4, #492
   3ce04: f2c02010     	vmov.i32	d18, #0x0
   3ce08: e2843f7f     	add	r3, r4, #508
   3ce0c: e3a01000     	mov	r1, #0
   3ce10: e2840f79     	add	r0, r4, #484
   3ce14: e58411e4     	str	r1, [r4, #0x1e4]
   3ce18: e58411e8     	str	r1, [r4, #0x1e8]
   3ce1c: edc42b74     	vstr	d18, [r4, #464]
   3ce20: f4420a8f     	vst1.32	{d16, d17}, [r2]
   3ce24: f4430a8f     	vst1.32	{d16, d17}, [r3]
   3ce28: e58d0044     	str	r0, [sp, #0x44]
   3ce2c: eb000d9c     	bl	0x404a4
   3ce30: f2c00050     	vmov.i32	q8, #0x0
   3ce34: e2842f85     	add	r2, r4, #532
   3ce38: e2843f89     	add	r3, r4, #548
   3ce3c: e3a01000     	mov	r1, #0
   3ce40: e2840f83     	add	r0, r4, #524
   3ce44: e584120c     	str	r1, [r4, #0x20c]
   3ce48: e5841210     	str	r1, [r4, #0x210]
   3ce4c: f4420a8f     	vst1.32	{d16, d17}, [r2]
   3ce50: f4430a8f     	vst1.32	{d16, d17}, [r3]
   3ce54: e58d0018     	str	r0, [sp, #0x18]
   3ce58: eb000d91     	bl	0x404a4
   3ce5c: e2840f8e     	add	r0, r4, #568
   3ce60: e2845fb9     	add	r5, r4, #740
   3ce64: e58d501c     	str	r5, [sp, #0x1c]
   3ce68: ebff6481     	bl	0x16074    @ imm = #-0x26dfc ; _ZNSt6chrono3_V212steady_clock3nowEv
   3ce6c: f2c00050     	vmov.i32	q8, #0x0
   3ce70: e2842d0b     	add	r2, r4, #704
   3ce74: e59f3610     	ldr	r3, [pc, #0x610]        @ 0x3d48c
   3ce78: e3a01000     	mov	r1, #0
   3ce7c: e5843244     	str	r3, [r4, #0x244]
   3ce80: e283000c     	add	r0, r3, #12
   3ce84: e584424c     	str	r4, [r4, #0x24c]
   3ce88: e5840250     	str	r0, [r4, #0x250]
   3ce8c: e2830018     	add	r0, r3, #24
   3ce90: e5844258     	str	r4, [r4, #0x258]
   3ce94: e2833024     	add	r3, r3, #36
   3ce98: e5840260     	str	r0, [r4, #0x260]
   3ce9c: e3a00001     	mov	r0, #1
   3cea0: e584326c     	str	r3, [r4, #0x26c]
   3cea4: e3a03000     	mov	r3, #0
   3cea8: e5840254     	str	r0, [r4, #0x254]
   3ceac: e3a00002     	mov	r0, #2
   3ceb0: e5844268     	str	r4, [r4, #0x268]
   3ceb4: e5840264     	str	r0, [r4, #0x264]
   3ceb8: e3a00003     	mov	r0, #3
   3cebc: e5844274     	str	r4, [r4, #0x274]
   3cec0: e5840270     	str	r0, [r4, #0x270]
   3cec4: e1a00005     	mov	r0, r5
   3cec8: e5843248     	str	r3, [r4, #0x248]
   3cecc: e5c4325c     	strb	r3, [r4, #0x25c]
   3ced0: f4420a8f     	vst1.32	{d16, d17}, [r2]
   3ced4: e58432d0     	str	r3, [r4, #0x2d0]
   3ced8: e58432d4     	str	r3, [r4, #0x2d4]
   3cedc: e58432e0     	str	r3, [r4, #0x2e0]
   3cee0: e58412d8     	str	r1, [r4, #0x2d8]
   3cee4: e58412dc     	str	r1, [r4, #0x2dc]
   3cee8: eb003b26     	bl	0x4bb88
   3ceec: e2840fda     	add	r0, r4, #872
   3cef0: eb003edc     	bl	0x4ca68
   3cef4: e2840ee6     	add	r0, r4, #3680
   3cef8: e2800008     	add	r0, r0, #8
   3cefc: eb003ed9     	bl	0x4ca68
   3cf00: e2840d65     	add	r0, r4, #6464
   3cf04: e2800028     	add	r0, r0, #40
   3cf08: eb003ace     	bl	0x4ba48
   3cf0c: e2843d66     	add	r3, r4, #6528
   3cf10: f2c01010     	vmov.i32	d17, #0x0
   3cf14: f2c00010     	vmov.i32	d16, #0x0
   3cf18: e1a02003     	mov	r2, r3
   3cf1c: e2833008     	add	r3, r3, #8
   3cf20: e2820010     	add	r0, r2, #16
   3cf24: edc21b00     	vstr	d17, [r2]
   3cf28: e2842a01     	add	r2, r4, #4096
   3cf2c: edc30b00     	vstr	d16, [r3]
   3cf30: e58d2010     	str	r2, [sp, #0x10]
   3cf34: eb003858     	bl	0x4b09c
   3cf38: e2840d66     	add	r0, r4, #6528
   3cf3c: e280001c     	add	r0, r0, #28
   3cf40: eb003855     	bl	0x4b09c
   3cf44: e2840d66     	add	r0, r4, #6528
   3cf48: e2800028     	add	r0, r0, #40
   3cf4c: eb003852     	bl	0x4b09c
   3cf50: e2840d66     	add	r0, r4, #6528
   3cf54: e2800034     	add	r0, r0, #52
   3cf58: eb00384f     	bl	0x4b09c
   3cf5c: e2843d67     	add	r3, r4, #6592
   3cf60: e58d3020     	str	r3, [sp, #0x20]
   3cf64: e1a00003     	mov	r0, r3
   3cf68: eb00492c     	bl	0x4f420
   3cf6c: e2843d67     	add	r3, r4, #6592
   3cf70: e2833038     	add	r3, r3, #56
   3cf74: e58d3030     	str	r3, [sp, #0x30]
   3cf78: e1a00003     	mov	r0, r3
   3cf7c: eb004944     	bl	0x4f494
   3cf80: e2840d69     	add	r0, r4, #6720
   3cf84: e2800020     	add	r0, r0, #32
   3cf88: eb004b45     	bl	0x4fca4
   3cf8c: e2843c32     	add	r3, r4, #12800
   3cf90: e2833004     	add	r3, r3, #4
   3cf94: e58d3034     	str	r3, [sp, #0x34]
   3cf98: e1a00003     	mov	r0, r3
   3cf9c: eb00493c     	bl	0x4f494
   3cfa0: e2840dc9     	add	r0, r4, #12864
   3cfa4: e280002c     	add	r0, r0, #44
   3cfa8: eb004b3d     	bl	0x4fca4
   3cfac: e2843c4a     	add	r3, r4, #18944
   3cfb0: e2833010     	add	r3, r3, #16
   3cfb4: e58d3038     	str	r3, [sp, #0x38]
   3cfb8: e1a00003     	mov	r0, r3
   3cfbc: eb00487f     	bl	0x4f1c0
   3cfc0: e2840c62     	add	r0, r4, #25088
   3cfc4: e1a01004     	mov	r1, r4
   3cfc8: e2800028     	add	r0, r0, #40
   3cfcc: ebffe6e1     	bl	0x36b58
   3cfd0: e2840c62     	add	r0, r4, #25088
   3cfd4: e2800048     	add	r0, r0, #72
   3cfd8: eb004baf     	bl	0x4fe9c
   3cfdc: e2843c62     	add	r3, r4, #25088
   3cfe0: e2833054     	add	r3, r3, #84
   3cfe4: e58d3024     	str	r3, [sp, #0x24]
   3cfe8: e1a00003     	mov	r0, r3
   3cfec: eb004baa     	bl	0x4fe9c
   3cff0: e2845a07     	add	r5, r4, #28672
   3cff4: e284ac62     	add	r10, r4, #25088
   3cff8: e28aa060     	add	r10, r10, #96
   3cffc: e3a03ff5     	mov	r3, #980
   3d000: e3a02ef5     	mov	r2, #3920
   3d004: e3a01000     	mov	r1, #0
   3d008: e58531b4     	str	r3, [r5, #0x1b4]
   3d00c: e1a0000a     	mov	r0, r10
   3d010: e585a1b0     	str	r10, [r5, #0x1b0]
   3d014: e2848c71     	add	r8, r4, #28928
   3d018: ebff6355     	bl	0x15d74    @ imm = #-0x272ac ; memset
   3d01c: eddf2bf9     	vldr	d18, [pc, #996]         @ 0x3d408 ; float 2.07955587515e-311
   3d020: eddf1bfa     	vldr	d17, [pc, #1000]        @ 0x3d410 ; float 5.31344206118e-315
   3d024: e285cf76     	add	r12, r5, #472
   3d028: eddf0bfa     	vldr	d16, [pc, #1000]        @ 0x3d418 ; float 5.60433832857e-315
   3d02c: e2851f6f     	add	r1, r5, #444
   3d030: e2852f72     	add	r2, r5, #456
   3d034: e3a0b000     	mov	r11, #0
   3d038: e28880e4     	add	r8, r8, #228
   3d03c: e3a0e001     	mov	lr, #1
   3d040: e3a07000     	mov	r7, #0
   3d044: e28a3ef5     	add	r3, r10, #3920
   3d048: e1a00008     	mov	r0, r8
   3d04c: e58531b8     	str	r3, [r5, #0x1b8]
   3d050: f441278f     	vst1.32	{d18}, [r1]
   3d054: e3a030e9     	mov	r3, #233
   3d058: e585b1c4     	str	r11, [r5, #0x1c4]
   3d05c: e1a0100b     	mov	r1, r11
   3d060: f442178f     	vst1.32	{d17}, [r2]
   3d064: e3a02fe9     	mov	r2, #932
   3d068: e585e1d4     	str	lr, [r5, #0x1d4]
   3d06c: e2846c75     	add	r6, r4, #29952
   3d070: e58571d0     	str	r7, [r5, #0x1d0]
   3d074: e28660a0     	add	r6, r6, #160
   3d078: f44c078f     	vst1.32	{d16}, [r12]
   3d07c: e2849b1e     	add	r9, r4, #30720
   3d080: e585358c     	str	r3, [r5, #0x58c]
   3d084: e2899074     	add	r9, r9, #116
   3d088: e5858588     	str	r8, [r5, #0x588]
   3d08c: e2888fe9     	add	r8, r8, #932
   3d090: e58571e0     	str	r7, [r5, #0x1e0]
   3d094: ebff6336     	bl	0x15d74    @ imm = #-0x27328 ; memset
   3d098: e2852e59     	add	r2, r5, #1424
   3d09c: eddf0bdf     	vldr	d16, [pc, #892]         @ 0x3d420 ; float 4.94425019295e-312
   3d0a0: e2822004     	add	r2, r2, #4
   3d0a4: e3a030af     	mov	r3, #175
   3d0a8: e5858590     	str	r8, [r5, #0x590]
   3d0ac: e1a00006     	mov	r0, r6
   3d0b0: e1a0100b     	mov	r1, r11
   3d0b4: f442078f     	vst1.32	{d16}, [r2]
   3d0b8: e3a02faf     	mov	r2, #700
   3d0bc: e5853860     	str	r3, [r5, #0x860]
   3d0c0: e30089bc     	movw	r8, #0x9bc
   3d0c4: e585685c     	str	r6, [r5, #0x85c]
   3d0c8: e2866faf     	add	r6, r6, #700
   3d0cc: ebff6328     	bl	0x15d74    @ imm = #-0x27360 ; memset
   3d0d0: e2852e86     	add	r2, r5, #2144
   3d0d4: e284c902     	add	r12, r4, #32768
   3d0d8: eddf0bd2     	vldr	d16, [pc, #840]         @ 0x3d428 ; float 3.71349263419e-312
   3d0dc: e2822008     	add	r2, r2, #8
   3d0e0: e5856864     	str	r6, [r5, #0x864]
   3d0e4: e300326f     	movw	r3, #0x26f
   3d0e8: e1a00009     	mov	r0, r9
   3d0ec: e1a0100b     	mov	r1, r11
   3d0f0: e58dc004     	str	r12, [sp, #0x4]
   3d0f4: f442078f     	vst1.32	{d16}, [r2]
   3d0f8: e1a02008     	mov	r2, r8
   3d0fc: e58c9230     	str	r9, [r12, #0x230]
   3d100: e0898008     	add	r8, r9, r8
   3d104: e58c3234     	str	r3, [r12, #0x234]
   3d108: e2846c82     	add	r6, r4, #33280
   3d10c: ebff6318     	bl	0x15d74    @ imm = #-0x273a0 ; memset
   3d110: e59d1004     	ldr	r1, [sp, #0x4]
   3d114: eddf0bc5     	vldr	d16, [pc, #788]         @ 0x3d430 ; float 1.32200337777e-311
   3d118: e2866048     	add	r6, r6, #72
   3d11c: e1a09001     	mov	r9, r1
   3d120: e2812f8f     	add	r2, r1, #572
   3d124: e5818238     	str	r8, [r1, #0x238]
   3d128: e30031c7     	movw	r3, #0x1c7
   3d12c: e1a00006     	mov	r0, r6
   3d130: e1a0100b     	mov	r1, r11
   3d134: f442078f     	vst1.32	{d16}, [r2]
   3d138: e300871c     	movw	r8, #0x71c
   3d13c: e5893968     	str	r3, [r9, #0x968]
   3d140: e1a02008     	mov	r2, r8
   3d144: e5896964     	str	r6, [r9, #0x964]
   3d148: e0866008     	add	r6, r6, r8
   3d14c: ebff6308     	bl	0x15d74    @ imm = #-0x273e0 ; memset
   3d150: e284ca09     	add	r12, r4, #36864
   3d154: eddf0bb7     	vldr	d16, [pc, #732]         @ 0x3d438 ; float 9.65508084889e-312
   3d158: e2891e97     	add	r1, r9, #2416
   3d15c: e28a3c27     	add	r3, r10, #9984
   3d160: e589696c     	str	r6, [r9, #0x96c]
   3d164: e283301c     	add	r3, r3, #28
   3d168: e1a0600c     	mov	r6, r12
   3d16c: f441078f     	vst1.32	{d16}, [r1]
   3d170: e3a02f93     	mov	r2, #588
   3d174: e1a00003     	mov	r0, r3
   3d178: e1a0100b     	mov	r1, r11
   3d17c: e58c32ac     	str	r3, [r12, #0x2ac]
   3d180: e58c22b0     	str	r2, [r12, #0x2b0]
   3d184: e3a02e93     	mov	r2, #2352
   3d188: e58dc00c     	str	r12, [sp, #0xc]
   3d18c: ebff62f8     	bl	0x15d74    @ imm = #-0x27420 ; memset
   3d190: e2861fae     	add	r1, r6, #696
   3d194: eddf0ba9     	vldr	d16, [pc, #676]         @ 0x3d440 ; float 1.24773352509e-311
   3d198: e28d2050     	add	r2, sp, #80
   3d19c: eddf4ab5     	vldr	s9, [pc, #724]          @ 0x3d478 ; float 127
   3d1a0: e2803e93     	add	r3, r0, #2352
   3d1a4: eeb67a08     	vmov.f32	s14, #7.500000e-01
   3d1a8: e58632b4     	str	r3, [r6, #0x2b4]
   3d1ac: eeb55a00     	vmov.f32	s10, #2.500000e-01
   3d1b0: f441078f     	vst1.32	{d16}, [r1]
   3d1b4: eef05a00     	vmov.f32	s11, #2.000000e+00
   3d1b8: eeb76a00     	vmov.f32	s12, #1.000000e+00
   3d1bc: e58672c8     	str	r7, [r6, #0x2c8]
   3d1c0: ee07ba90     	vmov	s15, r11
   3d1c4: eef86ae7     	vcvt.f32.s32	s13, s15
   3d1c8: eec67aa4     	vdiv.f32	s15, s13, s9
   3d1cc: eef47ac7     	vcmpe.f32	s15, s14
   3d1d0: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3d1d4: 5a000221     	bpl	0x3da60
   3d1d8: ee777ac5     	vsub.f32	s15, s15, s10
   3d1dc: eef06a46     	vmov.f32	s13, s12
   3d1e0: e28bb001     	add	r11, r11, #1
   3d1e4: e35b0080     	cmp	r11, #128
   3d1e8: eef07ae7     	vabs.f32	s15, s15
   3d1ec: eee76ae5     	vfms.f32	s13, s15, s11
   3d1f0: ece26a01     	vstmia	r2!, {s13}
   3d1f4: 1afffff1     	bne	0x3d1c0
   3d1f8: e59d000c     	ldr	r0, [sp, #0xc]
   3d1fc: f2c00010     	vmov.i32	d16, #0x0
   3d200: e2846801     	add	r6, r4, #65536
   3d204: e2849b25     	add	r9, r4, #37888
   3d208: e2803e2d     	add	r3, r0, #720
   3d20c: e28990d8     	add	r9, r9, #216
   3d210: e3a02c02     	mov	r2, #512
   3d214: e28d1050     	add	r1, sp, #80
   3d218: e2800fb6     	add	r0, r0, #728
   3d21c: e3068c54     	movw	r8, #0x6c54
   3d220: f443078f     	vst1.32	{d16}, [r3]
   3d224: e3a0b000     	mov	r11, #0
   3d228: ebff6385     	bl	0x16044    @ imm = #-0x271ec ; memcpy
   3d22c: e1a02008     	mov	r2, r8
   3d230: e586912c     	str	r9, [r6, #0x12c]
   3d234: e3013b15     	movw	r3, #0x1b15
   3d238: e1a00009     	mov	r0, r9
   3d23c: e5863130     	str	r3, [r6, #0x130]
   3d240: e3a01000     	mov	r1, #0
   3d244: e0898008     	add	r8, r9, r8
   3d248: ebff62c9     	bl	0x15d74    @ imm = #-0x274dc ; memset
   3d24c: e284ea12     	add	lr, r4, #73728
   3d250: eddf1b7c     	vldr	d17, [pc, #496]         @ 0x3d448 ; float 1.47117968188e-310
   3d254: e2861f4e     	add	r1, r6, #312
   3d258: e2862f51     	add	r2, r6, #324
   3d25c: eddf0b7b     	vldr	d16, [pc, #492]         @ 0x3d450 ; float 5.35132316613e-315
   3d260: e2863e15     	add	r3, r6, #336
   3d264: e5868134     	str	r8, [r6, #0x134]
   3d268: e300cb8f     	movw	r12, #0xb8f
   3d26c: f441178f     	vst1.32	{d17}, [r1]
   3d270: e3029e3c     	movw	r9, #0x2e3c
   3d274: e586b140     	str	r11, [r6, #0x140]
   3d278: e3a07000     	mov	r7, #0
   3d27c: f442078f     	vst1.32	{d16}, [r2]
   3d280: e1a00003     	mov	r0, r3
   3d284: e586714c     	str	r7, [r6, #0x14c]
   3d288: e1a02009     	mov	r2, r9
   3d28c: e58e3f8c     	str	r3, [lr, #0xf8c]
   3d290: e58ecf90     	str	r12, [lr, #0xf90]
   3d294: e1a0100b     	mov	r1, r11
   3d298: e58de008     	str	lr, [sp, #0x8]
   3d29c: ebff62b4     	bl	0x15d74    @ imm = #-0x27530 ; memset
   3d2a0: e59de008     	ldr	lr, [sp, #0x8]
   3d2a4: e2846906     	add	r6, r4, #98304
   3d2a8: eddf0b6a     	vldr	d16, [pc, #424]         @ 0x3d458 ; float 6.27898554547e-311
   3d2ac: e2848b4b     	add	r8, r4, #76800
   3d2b0: e28e1ef9     	add	r1, lr, #3984
   3d2b4: e2888fe9     	add	r8, r8, #932
   3d2b8: e2811008     	add	r1, r1, #8
   3d2bc: e0803009     	add	r3, r0, r9
   3d2c0: e301c424     	movw	r12, #0x1424
   3d2c4: e58e3f94     	str	r3, [lr, #0xf94]
   3d2c8: e1a00008     	mov	r0, r8
   3d2cc: e3059090     	movw	r9, #0x5090
   3d2d0: f441078f     	vst1.32	{d16}, [r1]
   3d2d4: e1a02009     	mov	r2, r9
   3d2d8: e586c038     	str	r12, [r6, #0x38]
   3d2dc: e1a0100b     	mov	r1, r11
   3d2e0: e5868034     	str	r8, [r6, #0x34]
   3d2e4: e0888009     	add	r8, r8, r9
   3d2e8: ebff62a1     	bl	0x15d74    @ imm = #-0x2757c ; memset
   3d2ec: eddf0b5b     	vldr	d16, [pc, #364]         @ 0x3d460 ; float 1.09410102982e-310
   3d2f0: e2861040     	add	r1, r6, #64
   3d2f4: e28a0b47     	add	r0, r10, #72704
   3d2f8: e3a03f62     	mov	r3, #392
   3d2fc: e2800f7a     	add	r0, r0, #488
   3d300: e3a02e62     	mov	r2, #1568
   3d304: e586803c     	str	r8, [r6, #0x3c]
   3d308: f441078f     	vst1.32	{d16}, [r1]
   3d30c: e1a0100b     	mov	r1, r11
   3d310: e586366c     	str	r3, [r6, #0x66c]
   3d314: e5860668     	str	r0, [r6, #0x668]
   3d318: ebff6295     	bl	0x15d74    @ imm = #-0x275ac ; memset
   3d31c: e2862e67     	add	r2, r6, #1648
   3d320: e2822004     	add	r2, r2, #4
   3d324: eddf0b4f     	vldr	d16, [pc, #316]         @ 0x3d468 ; float 8.31822350058e-312
   3d328: e286ce67     	add	r12, r6, #1648
   3d32c: e2860e66     	add	r0, r6, #1632
   3d330: e1a0100b     	mov	r1, r11
   3d334: e28d3050     	add	r3, sp, #80
   3d338: eddf4a4e     	vldr	s9, [pc, #312]          @ 0x3d478 ; float 127
   3d33c: e2800008     	add	r0, r0, #8
   3d340: eeb67a08     	vmov.f32	s14, #7.500000e-01
   3d344: e5860670     	str	r0, [r6, #0x670]
   3d348: eeb55a00     	vmov.f32	s10, #2.500000e-01
   3d34c: f442078f     	vst1.32	{d16}, [r2]
   3d350: eef05a00     	vmov.f32	s11, #2.000000e+00
   3d354: eeb76a00     	vmov.f32	s12, #1.000000e+00
   3d358: e58c7014     	str	r7, [r12, #0x14]
   3d35c: ee071a90     	vmov	s15, r1
   3d360: eef86ae7     	vcvt.f32.s32	s13, s15
   3d364: eec67aa4     	vdiv.f32	s15, s13, s9
   3d368: eef47ac7     	vcmpe.f32	s15, s14
   3d36c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3d370: 5a0001b3     	bpl	0x3da44
   3d374: ee777ac5     	vsub.f32	s15, s15, s10
   3d378: eef06a46     	vmov.f32	s13, s12
   3d37c: e2811001     	add	r1, r1, #1
   3d380: e3510080     	cmp	r1, #128
   3d384: eef07ae7     	vabs.f32	s15, s15
   3d388: eee76ae5     	vfms.f32	s13, s15, s11
   3d38c: ece36a01     	vstmia	r3!, {s13}
   3d390: 1afffff1     	bne	0x3d35c
   3d394: f2c00010     	vmov.i32	d16, #0x0
   3d398: e2863d1a     	add	r3, r6, #1664
   3d39c: e283300c     	add	r3, r3, #12
   3d3a0: e2847a1f     	add	r7, r4, #126976
   3d3a4: e2860e69     	add	r0, r6, #1680
   3d3a8: e2848b62     	add	r8, r4, #100352
   3d3ac: e2888094     	add	r8, r8, #148
   3d3b0: e3a02c02     	mov	r2, #512
   3d3b4: f443078f     	vst1.32	{d16}, [r3]
   3d3b8: e28d1050     	add	r1, sp, #80
   3d3bc: e2800004     	add	r0, r0, #4
   3d3c0: e307a268     	movw	r10, #0x7268
   3d3c4: ebff631e     	bl	0x16044    @ imm = #-0x27388 ; memcpy
   3d3c8: e1a0200a     	mov	r2, r10
   3d3cc: e5878afc     	str	r8, [r7, #0xafc]
   3d3d0: e3013c9a     	movw	r3, #0x1c9a
   3d3d4: e1a00008     	mov	r0, r8
   3d3d8: e5873b00     	str	r3, [r7, #0xb00]
   3d3dc: e3a01000     	mov	r1, #0
   3d3e0: e2849a23     	add	r9, r4, #143360
   3d3e4: ebff6262     	bl	0x15d74    @ imm = #-0x27678 ; memset
   3d3e8: e2872c0b     	add	r2, r7, #2816
   3d3ec: e287ceb1     	add	r12, r7, #2832
   3d3f0: e287eeb1     	add	lr, r7, #2832
   3d3f4: eddf1b1d     	vldr	d17, [pc, #116]         @ 0x3d470 ; float 1.55372531814e-310
   3d3f8: e28cc004     	add	r12, r12, #4
   3d3fc: ed9f8a1e     	vldr	s16, [pc, #120]         @ 0x3d47c ; float 0
   3d400: e2822008     	add	r2, r2, #8
   3d404: ea000021     	b	0x3d490
   3d408: 00 00 00 00  	.word	0x00000000
   3d40c: d4 03 00 00  	.word	0x000003d4
   3d410: e6 1a 1a 40  	.word	0x401a1ae6
   3d414: 00 00 00 00  	.word	0x00000000
   3d418: 52 83 9c 43  	.word	0x439c8352
   3d41c: 00 00 00 00  	.word	0x00000000
   3d420: 00 00 00 00  	.word	0x00000000
   3d424: e9 00 00 00  	.word	0x000000e9
   3d428: 00 00 00 00  	.word	0x00000000
   3d42c: af 00 00 00  	.word	0x000000af
   3d430: 00 00 00 00  	.word	0x00000000
   3d434: 6f 02 00 00  	.word	0x0000026f
   3d438: 00 00 00 00  	.word	0x00000000
   3d43c: c7 01 00 00  	.word	0x000001c7
   3d440: 00 00 00 00  	.word	0x00000000
   3d444: 4c 02 00 00  	.word	0x0000024c
   3d448: 00 00 00 00  	.word	0x00000000
   3d44c: 15 1b 00 00  	.word	0x00001b15
   3d450: fb 18 8f 40  	.word	0x408f18fb
   3d454: 00 00 00 00  	.word	0x00000000
   3d458: 00 00 00 00  	.word	0x00000000
   3d45c: 8f 0b 00 00  	.word	0x00000b8f
   3d460: 00 00 00 00  	.word	0x00000000
   3d464: 24 14 00 00  	.word	0x00001424
   3d468: 00 00 00 00  	.word	0x00000000
   3d46c: 88 01 00 00  	.word	0x00000188
   3d470: 00 00 00 00  	.word	0x00000000
   3d474: 9a 1c 00 00  	.word	0x00001c9a
   3d478: 00 00 fe 42  	.word	0x42fe0000
   3d47c: 00 00 00 00  	.word	0x00000000
   3d480: 08 4e 09 00  	.word	0x00094e08
   3d484: 14 23 07 00  	.word	0x00072314
   3d488: ec ee 08 00  	.word	0x0008eeec
   3d48c: 24 2f 07 00  	.word	0x00072f24
   3d490: eddf0bfc     	vldr	d16, [pc, #1008]        @ 0x3d888 ; float 5.35132316613e-315
   3d494: e2843b7e     	add	r3, r4, #129024
   3d498: e2833e32     	add	r3, r3, #800
   3d49c: e3a01000     	mov	r1, #0
   3d4a0: e088800a     	add	r8, r8, r10
   3d4a4: e1a00003     	mov	r0, r3
   3d4a8: e5878b04     	str	r8, [r7, #0xb04]
   3d4ac: e301a118     	movw	r10, #0x1118
   3d4b0: f442178f     	vst1.32	{d17}, [r2]
   3d4b4: e3048460     	movw	r8, #0x4460
   3d4b8: e5871b10     	str	r1, [r7, #0xb10]
   3d4bc: e1a02008     	mov	r2, r8
   3d4c0: f44c078f     	vst1.32	{d16}, [r12]
   3d4c4: e284ba29     	add	r11, r4, #167936
   3d4c8: ed8e8a03     	vstr	s16, [lr, #12]
   3d4cc: e2847b8f     	add	r7, r4, #146432
   3d4d0: e5893f80     	str	r3, [r9, #0xf80]
   3d4d4: e2877fe6     	add	r7, r7, #920
   3d4d8: e589af84     	str	r10, [r9, #0xf84]
   3d4dc: e305af90     	movw	r10, #0x5f90
   3d4e0: ebff6223     	bl	0x15d74    @ imm = #-0x27774 ; memset
   3d4e4: e2892d3e     	add	r2, r9, #3968
   3d4e8: eddf0be8     	vldr	d16, [pc, #928]         @ 0x3d890 ; float 9.28585358126e-311
   3d4ec: e282200c     	add	r2, r2, #12
   3d4f0: e0803008     	add	r3, r0, r8
   3d4f4: e3a01000     	mov	r1, #0
   3d4f8: e5893f88     	str	r3, [r9, #0xf88]
   3d4fc: e30187e4     	movw	r8, #0x17e4
   3d500: f442078f     	vst1.32	{d16}, [r2]
   3d504: e1a00007     	mov	r0, r7
   3d508: e1a0200a     	mov	r2, r10
   3d50c: e58b7f28     	str	r7, [r11, #0xf28]
   3d510: e58b8f2c     	str	r8, [r11, #0xf2c]
   3d514: e087700a     	add	r7, r7, r10
   3d518: ebff6215     	bl	0x15d74    @ imm = #-0x277ac ; memset
   3d51c: e2893ef9     	add	r3, r9, #3984
   3d520: eddf0bdc     	vldr	d16, [pc, #880]         @ 0x3d898 ; float 0
   3d524: eddf1bdd     	vldr	d17, [pc, #884]         @ 0x3d8a0 ; float 0.00781250184809
   3d528: e28b9ef3     	add	r9, r11, #3888
   3d52c: e28b0ef5     	add	r0, r11, #3920
   3d530: e285ce59     	add	r12, r5, #1424
   3d534: e28bed3d     	add	lr, r11, #3904
   3d538: e289900c     	add	r9, r9, #12
   3d53c: e2855e87     	add	r5, r5, #2160
   3d540: e2831004     	add	r1, r3, #4
   3d544: e59d3004     	ldr	r3, [sp, #0x4]
   3d548: e286ae67     	add	r10, r6, #1648
   3d54c: e58b7f30     	str	r7, [r11, #0xf30]
   3d550: e30d788a     	movw	r7, #0xd88a
   3d554: e343776e     	movt	r7, #0x376e
   3d558: e58b8f38     	str	r8, [r11, #0xf38]
   3d55c: e2833e97     	add	r3, r3, #2416
   3d560: e58d102c     	str	r1, [sp, #0x2c]
   3d564: e3a01000     	mov	r1, #0
   3d568: e58b1f34     	str	r1, [r11, #0xf34]
   3d56c: f4490a8f     	vst1.32	{d16, d17}, [r9]
   3d570: e3a015fd     	mov	r1, #1061158912
   3d574: e59d2008     	ldr	r2, [sp, #0x8]
   3d578: ed8e8a03     	vstr	s16, [lr, #12]
   3d57c: e3a09000     	mov	r9, #0
   3d580: e3439f20     	movt	r9, #0x3f20
   3d584: ed808a00     	vstr	s16, [r0]
   3d588: ed808a01     	vstr	s16, [r0, #4]
   3d58c: e2822efa     	add	r2, r2, #4000
   3d590: e58c100c     	str	r1, [r12, #0xc]
   3d594: e3038333     	movw	r8, #0x3333
   3d598: e3438eb3     	movt	r8, #0x3eb3
   3d59c: e5851000     	str	r1, [r5]
   3d5a0: e59d1004     	ldr	r1, [sp, #0x4]
   3d5a4: e3a0c000     	mov	r12, #0
   3d5a8: e344c248     	movt	r12, #0x4248
   3d5ac: e58d2008     	str	r2, [sp, #0x8]
   3d5b0: e28aa00c     	add	r10, r10, #12
   3d5b4: e58da028     	str	r10, [sp, #0x28]
   3d5b8: e5819244     	str	r9, [r1, #0x244]
   3d5bc: e286ad1a     	add	r10, r6, #1664
   3d5c0: e5839008     	str	r9, [r3, #0x8]
   3d5c4: e3a0e43f     	mov	lr, #1056964608
   3d5c8: e59d300c     	ldr	r3, [sp, #0xc]
   3d5cc: e28aa008     	add	r10, r10, #8
   3d5d0: e2866d1a     	add	r6, r6, #1664
   3d5d4: eddf7abd     	vldr	s15, [pc, #756]         @ 0x3d8d0 ; float 0.47499999404
   3d5d8: e30b599c     	movw	r5, #0xb99c
   3d5dc: e343574c     	movt	r5, #0x374c
   3d5e0: e58372cc     	str	r7, [r3, #0x2cc]
   3d5e4: e28d0050     	add	r0, sp, #80
   3d5e8: e58382c0     	str	r8, [r3, #0x2c0]
   3d5ec: e583c2c4     	str	r12, [r3, #0x2c4]
   3d5f0: e59d3008     	ldr	r3, [sp, #0x8]
   3d5f4: e59d7028     	ldr	r7, [sp, #0x28]
   3d5f8: e59d2014     	ldr	r2, [sp, #0x14]
   3d5fc: e583e000     	str	lr, [r3]
   3d600: e59d302c     	ldr	r3, [sp, #0x2c]
   3d604: edc77a00     	vstr	s15, [r7]
   3d608: e59f12c8     	ldr	r1, [pc, #0x2c8]        @ 0x3d8d8
   3d60c: e58a5000     	str	r5, [r10]
   3d610: e586c000     	str	r12, [r6]
   3d614: e583e000     	str	lr, [r3]
   3d618: ebffc557     	bl	0x2eb7c
   3d61c: e59d1050     	ldr	r1, [sp, #0x50]
   3d620: e2846ba7     	add	r6, r4, #171008
   3d624: e59d2054     	ldr	r2, [sp, #0x54]
   3d628: e2865fda     	add	r5, r6, #872
   3d62c: e59f32a8     	ldr	r3, [pc, #0x2a8]        @ 0x3d8dc
   3d630: e2860e36     	add	r0, r6, #864
   3d634: e58b3f58     	str	r3, [r11, #0xf58]
   3d638: e0812002     	add	r2, r1, r2
   3d63c: e3a03000     	mov	r3, #0
   3d640: e58b5f60     	str	r5, [r11, #0xf60]
   3d644: e58b3f5c     	str	r3, [r11, #0xf5c]
   3d648: e2868fd6     	add	r8, r6, #856
   3d64c: ebffe629     	bl	0x36ef8
   3d650: e3e03000     	mvn	r3, #0
   3d654: e1a00008     	mov	r0, r8
   3d658: e58b3f78     	str	r3, [r11, #0xf78]
   3d65c: eb000bce     	bl	0x4059c
   3d660: e1a00008     	mov	r0, r8
   3d664: eb000d14     	bl	0x40abc
   3d668: e1a00008     	mov	r0, r8
   3d66c: eb000de5     	bl	0x40e08
   3d670: e59d0050     	ldr	r0, [sp, #0x50]
   3d674: e28d3058     	add	r3, sp, #88
   3d678: e1500003     	cmp	r0, r3
   3d67c: e59f325c     	ldr	r3, [pc, #0x25c]        @ 0x3d8e0
   3d680: e58b3f58     	str	r3, [r11, #0xf58]
   3d684: 0a000000     	beq	0x3d68c
   3d688: ebff61ec     	bl	0x15e40    @ imm = #-0x27850 ; _ZdlPv
   3d68c: e1a01004     	mov	r1, r4
   3d690: e2860fdf     	add	r0, r6, #892
   3d694: ebfffd29     	bl	0x3cb40
   3d698: e2860ff9     	add	r0, r6, #996
   3d69c: ebffecd4     	bl	0x389f4
   3d6a0: e2860ffe     	add	r0, r6, #1016
   3d6a4: ebffecd2     	bl	0x389f4
   3d6a8: e2845a2a     	add	r5, r4, #172032
   3d6ac: e3a03000     	mov	r3, #0
   3d6b0: f2c00050     	vmov.i32	q8, #0x0
   3d6b4: e285001c     	add	r0, r5, #28
   3d6b8: e285c00c     	add	r12, r5, #12
   3d6bc: e285202c     	add	r2, r5, #44
   3d6c0: e2859048     	add	r9, r5, #72
   3d6c4: e1a01004     	mov	r1, r4
   3d6c8: f44c0a8f     	vst1.32	{d16, d17}, [r12]
   3d6cc: f4400a8f     	vst1.32	{d16, d17}, [r0]
   3d6d0: e1a00009     	mov	r0, r9
   3d6d4: f4420a8f     	vst1.32	{d16, d17}, [r2]
   3d6d8: e585303c     	str	r3, [r5, #0x3c]
   3d6dc: e5853040     	str	r3, [r5, #0x40]
   3d6e0: e5853044     	str	r3, [r5, #0x44]
   3d6e4: ebfffba9     	bl	0x3c590
   3d6e8: e2853e1f     	add	r3, r5, #496
   3d6ec: e1a01004     	mov	r1, r4
   3d6f0: e1a00003     	mov	r0, r3
   3d6f4: e58d3004     	str	r3, [sp, #0x4]
   3d6f8: ebfffa4b     	bl	0x3c02c
   3d6fc: e2853fbe     	add	r3, r5, #760
   3d700: e1a01004     	mov	r1, r4
   3d704: e1a00003     	mov	r0, r3
   3d708: e58d3008     	str	r3, [sp, #0x8]
   3d70c: ebfffb03     	bl	0x3c320
   3d710: e2853fed     	add	r3, r5, #948
   3d714: e1a01004     	mov	r1, r4
   3d718: e1a00003     	mov	r0, r3
   3d71c: e58d300c     	str	r3, [sp, #0xc]
   3d720: ebfff921     	bl	0x3bbac
   3d724: f2c00010     	vmov.i32	d16, #0x0
   3d728: e2853e4e     	add	r3, r5, #1248
   3d72c: e2833008     	add	r3, r3, #8
   3d730: e2847ba9     	add	r7, r4, #173056
   3d734: e3a02000     	mov	r2, #0
   3d738: e28700f8     	add	r0, r7, #248
   3d73c: ed9f0a64     	vldr	s0, [pc, #400]          @ 0x3d8d4 ; float 0.899999976158
   3d740: e3a01002     	mov	r1, #2
   3d744: f443078f     	vst1.32	{d16}, [r3]
   3d748: e58524f0     	str	r2, [r5, #0x4f0]
   3d74c: eb00ba66     	bl	0x6c0ec
   3d750: e3a01002     	mov	r1, #2
   3d754: e2870f42     	add	r0, r7, #264
   3d758: eeb60a00     	vmov.f32	s0, #5.000000e-01
   3d75c: eb00ba62     	bl	0x6c0ec
   3d760: e3a01002     	mov	r1, #2
   3d764: e2870f46     	add	r0, r7, #280
   3d768: eeb60a00     	vmov.f32	s0, #5.000000e-01
   3d76c: eb00ba5e     	bl	0x6c0ec
   3d770: e3a01002     	mov	r1, #2
   3d774: e2870f4a     	add	r0, r7, #296
   3d778: eeb60a00     	vmov.f32	s0, #5.000000e-01
   3d77c: eb00ba5a     	bl	0x6c0ec
   3d780: e59fe15c     	ldr	lr, [pc, #0x15c]        @ 0x3d8e4
   3d784: e28dc050     	add	r12, sp, #80
   3d788: f2c01010     	vmov.i32	d17, #0x0
   3d78c: e285ae4d     	add	r10, r5, #1232
   3d790: eddf0b44     	vldr	d16, [pc, #272]         @ 0x3d8a8 ; float 2.12199579294e-314
   3d794: e28aa004     	add	r10, r10, #4
   3d798: e8be000f     	ldm	lr!, {r0, r1, r2, r3}
   3d79c: e8ac000f     	stm	r12!, {r0, r1, r2, r3}
   3d7a0: e2853e4e     	add	r3, r5, #1248
   3d7a4: e3a02000     	mov	r2, #0
   3d7a8: e58420ec     	str	r2, [r4, #0xec]
   3d7ac: e58524dc     	str	r2, [r5, #0x4dc]
   3d7b0: edc31b00     	vstr	d17, [r3]
   3d7b4: e8be000f     	ldm	lr!, {r0, r1, r2, r3}
   3d7b8: e8ac000f     	stm	r12!, {r0, r1, r2, r3}
   3d7bc: f44a078f     	vst1.32	{d16}, [r10]
   3d7c0: e8be000f     	ldm	lr!, {r0, r1, r2, r3}
   3d7c4: e8ac000f     	stm	r12!, {r0, r1, r2, r3}
   3d7c8: e89e000f     	ldm	lr, {r0, r1, r2, r3}
   3d7cc: e88c000f     	stm	r12, {r0, r1, r2, r3}
   3d7d0: e28d1050     	add	r1, sp, #80
   3d7d4: e28700e8     	add	r0, r7, #232
   3d7d8: e28d2090     	add	r2, sp, #144
   3d7dc: e3a03000     	mov	r3, #0
   3d7e0: eb000a68     	bl	0x40188
   3d7e4: e2852e4b     	add	r2, r5, #1200
   3d7e8: e2851e49     	add	r1, r5, #1168
   3d7ec: eddf4b2f     	vldr	d20, [pc, #188]         @ 0x3d8b0 ; float 5.43230922614e-311
   3d7f0: eddf5b30     	vldr	d21, [pc, #192]         @ 0x3d8b8 ; float 0
   3d7f4: e2853e4a     	add	r3, r5, #1184
   3d7f8: eddf2b30     	vldr	d18, [pc, #192]         @ 0x3d8c0 ; float 0
   3d7fc: eddf3b31     	vldr	d19, [pc, #196]         @ 0x3d8c8 ; float 2.16443570678e-312
   3d800: e2822008     	add	r2, r2, #8
   3d804: f2c00050     	vmov.i32	q8, #0x0
   3d808: edc14b02     	vstr	d20, [r1, #8]
   3d80c: edc15b04     	vstr	d21, [r1, #16]
   3d810: e59d0024     	ldr	r0, [sp, #0x24]
   3d814: edc32b02     	vstr	d18, [r3, #8]
   3d818: edc33b04     	vstr	d19, [r3, #16]
   3d81c: e3a01c01     	mov	r1, #256
   3d820: e3a03000     	mov	r3, #0
   3d824: f4420adf     	vst1.64	{d16, d17}, [r2:64]
   3d828: eef60a08     	vmov.f32	s1, #7.500000e-01
   3d82c: eeb60a00     	vmov.f32	s0, #5.000000e-01
   3d830: e58534c8     	str	r3, [r5, #0x4c8]
   3d834: e58534cc     	str	r3, [r5, #0x4cc]
   3d838: e58514f4     	str	r1, [r5, #0x4f4]
   3d83c: e5843110     	str	r3, [r4, #0x110]
   3d840: eb0049e6     	bl	0x4ffe0
   3d844: e28d0048     	add	r0, sp, #72
   3d848: e3a07001     	mov	r7, #1
   3d84c: e58574d0     	str	r7, [r5, #0x4d0]
   3d850: ebff6030     	bl	0x15918     @ imm = #-0x27f40 ; _ZNSt6chrono3_V212system_clock3nowEv
   3d854: e59430e8     	ldr	r3, [r4, #0xe8]
   3d858: e1cd04d8     	ldrd	r0, r1, [sp, #72]
   3d85c: e58401d0     	str	r0, [r4, #0x1d0]
   3d860: e58411d4     	str	r1, [r4, #0x1d4]
   3d864: e59330b0     	ldr	r3, [r3, #0xb0]
   3d868: e5932020     	ldr	r2, [r3, #0x20]
   3d86c: e3520000     	cmp	r2, #0
   3d870: da000081     	ble	0x3da7c
   3d874: e593101c     	ldr	r1, [r3, #0x1c]
   3d878: e59d0018     	ldr	r0, [sp, #0x18]
   3d87c: eb002767     	bl	0x47620
   3d880: ea000019     	b	0x3d8ec
   3d884: e320f000     	nop
   3d888: fb 18 8f 40  	.word	0x408f18fb
   3d88c: 00 00 00 00  	.word	0x00000000
   3d890: 00 00 00 00  	.word	0x00000000
   3d894: 18 11 00 00  	.word	0x00001118
   3d898: 00 00 00 00  	.word	0x00000000
   3d89c: 00 00 00 00  	.word	0x00000000
   3d8a0: 00 00 80 3f  	.word	0x3f800000
   3d8a4: 00 00 80 3f  	.word	0x3f800000
   3d8a8: 04 00 00 00  	.word	0x00000004
   3d8ac: 01 00 00 00  	.word	0x00000001
   3d8b0: 00 0a 00 00  	.word	0x00000a00
   3d8b4: 00 0a 00 00  	.word	0x00000a00
   3d8b8: 00 00 00 00  	.word	0x00000000
   3d8bc: 00 00 00 00  	.word	0x00000000
   3d8c0: 00 00 00 00  	.word	0x00000000
   3d8c4: 00 00 00 00  	.word	0x00000000
   3d8c8: 00 00 00 00  	.word	0x00000000
   3d8cc: 66 00 00 00  	.word	0x00000066
   3d8d0: 33 33 f3 3e  	.word	0x3ef33333
   3d8d4: 66 66 66 3f  	.word	0x3f666666
   3d8d8: a8 50 09 00  	.word	0x000950a8
   3d8dc: f0 1e 07 00  	.word	0x00071ef0
   3d8e0: 94 2e 07 00  	.word	0x00072e94
   3d8e4: 1c 30 07 00  	.word	0x0007301c
   3d8e8: 00 f0 7f 45  	.word	0x457ff000
   3d8ec: e3a02000     	mov	r2, #0
   3d8f0: e59d001c     	ldr	r0, [sp, #0x1c]
   3d8f4: e1a01002     	mov	r1, r2
   3d8f8: e5c421e0     	strb	r2, [r4, #0x1e0]
   3d8fc: eb003a1e     	bl	0x4c17c
   3d900: e59430e8     	ldr	r3, [r4, #0xe8]
   3d904: e30c2ccd     	movw	r2, #0xcccd
   3d908: e3432d4c     	movt	r2, #0x3d4c
   3d90c: e584208c     	str	r2, [r4, #0x8c]
   3d910: eef16a00     	vmov.f32	s13, #4.000000e+00
   3d914: ed1f7a0d     	vldr	s14, [pc, #-52]         @ 0x3d8e8 ; float 4095
   3d918: e3a01000     	mov	r1, #0
   3d91c: e1a00004     	mov	r0, r4
   3d920: edd37a1a     	vldr	s15, [r3, #104]
   3d924: ee677aa6     	vmul.f32	s15, s15, s13
   3d928: ee677a87     	vmul.f32	s15, s15, s14
   3d92c: eefd7ae7     	vcvt.s32.f32	s15, s15
   3d930: ee173a90     	vmov	r3, s15
   3d934: e58534b4     	str	r3, [r5, #0x4b4]
   3d938: ebfff1b6     	bl	0x3a018
   3d93c: e2843f9f     	add	r3, r4, #636
   3d940: e59412cc     	ldr	r1, [r4, #0x2cc]
   3d944: e3000101     	movw	r0, #0x101
   3d948: e3a02000     	mov	r2, #0
   3d94c: e59d7010     	ldr	r7, [sp, #0x10]
   3d950: e1c300b0     	strh	r0, [r3]
   3d954: e3a03c01     	mov	r3, #256
   3d958: e3403001     	movt	r3, #0x1
   3d95c: e5843284     	str	r3, [r4, #0x284]
   3d960: e3a03000     	mov	r3, #0
   3d964: e5c432ac     	strb	r3, [r4, #0x2ac]
   3d968: e5c4328a     	strb	r3, [r4, #0x28a]
   3d96c: e5c432b0     	strb	r3, [r4, #0x2b0]
   3d970: e5c432bc     	strb	r3, [r4, #0x2bc]
   3d974: e584329c     	str	r3, [r4, #0x29c]
   3d978: e5c43298     	strb	r3, [r4, #0x298]
   3d97c: e5c43240     	strb	r3, [r4, #0x240]
   3d980: e5c4328c     	strb	r3, [r4, #0x28c]
   3d984: e59430e8     	ldr	r3, [r4, #0xe8]
   3d988: e58422b4     	str	r2, [r4, #0x2b4]
   3d98c: e5842280     	str	r2, [r4, #0x280]
   3d990: e59330b0     	ldr	r3, [r3, #0xb0]
   3d994: eddf0be9     	vldr	d16, [pc, #932]         @ 0x3dd40 ; float 4.24399158193e-314
   3d998: e2833901     	add	r3, r3, #16384
   3d99c: eddf7af3     	vldr	s15, [pc, #972]         @ 0x3dd70 ; float 49170.2539062
   3d9a0: e59422d0     	ldr	r2, [r4, #0x2d0]
   3d9a4: edc40ba4     	vstr	d16, [r4, #656]
   3d9a8: ed930aa3     	vldr	s0, [r3, #652]
   3d9ac: e1510002     	cmp	r1, r2
   3d9b0: ed938aa5     	vldr	s16, [r3, #660]
   3d9b4: e3a02003     	mov	r2, #3
   3d9b8: 158412d0     	strne	r1, [r4, #0x2d0]
   3d9bc: ee800a27     	vdiv.f32	s0, s0, s15
   3d9c0: e59d0020     	ldr	r0, [sp, #0x20]
   3d9c4: eddf0aea     	vldr	s1, [pc, #936]          @ 0x3dd74 ; float 0.20000000298
   3d9c8: e58729e8     	str	r2, [r7, #0x9e8]
   3d9cc: eb0033e7     	bl	0x4a970
   3d9d0: ed9f7ae6     	vldr	s14, [pc, #920]         @ 0x3dd70 ; float 49170.2539062
   3d9d4: eef72b00     	vmov.f64	d18, #1.000000e+00
   3d9d8: eddf1bda     	vldr	d17, [pc, #872]         @ 0x3dd48 ; float 3.14159265359
   3d9dc: e2873e9e     	add	r3, r7, #2528
   3d9e0: e5942000     	ldr	r2, [r4]
   3d9e4: eec87a07     	vdiv.f32	s15, s16, s14
   3d9e8: e3520000     	cmp	r2, #0
   3d9ec: eef70ae7     	vcvt.f64.f32	d16, s15
   3d9f0: ee600ba1     	vmul.f64	d16, d16, d17
   3d9f4: eec21ba0     	vdiv.f64	d17, d18, d16
   3d9f8: eef77be1     	vcvt.f32.f64	s15, d17
   3d9fc: edc37a03     	vstr	s15, [r3, #12]
   3da00: 0a000021     	beq	0x3da8c
   3da04: e3520001     	cmp	r2, #1
   3da08: 1a000009     	bne	0x3da34
   3da0c: eddf0bcf     	vldr	d16, [pc, #828]         @ 0x3dd50 ; float 1.51693947382e-134
   3da10: eddf1bd0     	vldr	d17, [pc, #832]         @ 0x3dd58 ; float -1.98878707915e-272
   3da14: e2851d12     	add	r1, r5, #1152
   3da18: e3082d89     	movw	r2, #0x8d89
   3da1c: e349218f     	movt	r2, #0x918f
   3da20: e3093593     	movw	r3, #0x9593
   3da24: e3493997     	movt	r3, #0x9997
   3da28: f4410adf     	vst1.64	{d16, d17}, [r1:64]
   3da2c: e5852490     	str	r2, [r5, #0x490]
   3da30: e5853494     	str	r3, [r5, #0x494]
   3da34: e1a00004     	mov	r0, r4
   3da38: e28ddf95     	add	sp, sp, #596
   3da3c: ecbd8b02     	vpop	{d8}
   3da40: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
   3da44: ee777ac7     	vsub.f32	s15, s15, s14
   3da48: e2811001     	add	r1, r1, #1
   3da4c: e3510080     	cmp	r1, #128
   3da50: ee777aa7     	vadd.f32	s15, s15, s15
   3da54: ece37a01     	vstmia	r3!, {s15}
   3da58: 1afffe3f     	bne	0x3d35c
   3da5c: eafffe4c     	b	0x3d394
   3da60: ee777ac7     	vsub.f32	s15, s15, s14
   3da64: e28bb001     	add	r11, r11, #1
   3da68: e35b0080     	cmp	r11, #128
   3da6c: ee777aa7     	vadd.f32	s15, s15, s15
   3da70: ece27a01     	vstmia	r2!, {s15}
   3da74: 1afffdd1     	bne	0x3d1c0
   3da78: eafffdde     	b	0x3d1f8
   3da7c: e59d0018     	ldr	r0, [sp, #0x18]
   3da80: e1a01007     	mov	r1, r7
   3da84: eb0026e5     	bl	0x47620
   3da88: eaffff97     	b	0x3d8ec
   3da8c: eddf0bb3     	vldr	d16, [pc, #716]         @ 0x3dd60 ; float 9.72811825286e-154
   3da90: eddf1bb4     	vldr	d17, [pc, #720]         @ 0x3dd68 ; float -2.60989664381e-277
   3da94: e2851d12     	add	r1, r5, #1152
   3da98: e1a00004     	mov	r0, r4
   3da9c: e3082c88     	movw	r2, #0x8c88
   3daa0: e349208e     	movt	r2, #0x908e
   3daa4: e3093492     	movw	r3, #0x9492
   3daa8: e3493896     	movt	r3, #0x9896
   3daac: f4410adf     	vst1.64	{d16, d17}, [r1:64]
   3dab0: e5852490     	str	r2, [r5, #0x490]
   3dab4: e5853494     	str	r3, [r5, #0x494]
   3dab8: e28ddf95     	add	sp, sp, #596
   3dabc: ecbd8b02     	vpop	{d8}
   3dac0: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
   3dac4: e5940004     	ldr	r0, [r4, #0x4]
   3dac8: e59d3040     	ldr	r3, [sp, #0x40]
   3dacc: e1530000     	cmp	r3, r0
   3dad0: 0a000000     	beq	0x3dad8
   3dad4: ebff60d9     	bl	0x15e40    @ imm = #-0x27c9c ; _ZdlPv
   3dad8: ebff6120     	bl	0x15f60    @ imm = #-0x27b80 ; __cxa_end_cleanup
   3dadc: e59f3294     	ldr	r3, [pc, #0x294]        @ 0x3dd78
   3dae0: e5843118     	str	r3, [r4, #0x118]
   3dae4: e59f3290     	ldr	r3, [pc, #0x290]        @ 0x3dd7c
   3dae8: e59d003c     	ldr	r0, [sp, #0x3c]
   3daec: e5843128     	str	r3, [r4, #0x128]
   3daf0: eb00b868     	bl	0x6bc98
   3daf4: e59400d8     	ldr	r0, [r4, #0xd8]
   3daf8: e3500000     	cmp	r0, #0
   3dafc: 0a000000     	beq	0x3db04
   3db00: ebff60ce     	bl	0x15e40    @ imm = #-0x27cc8 ; _ZdlPv
   3db04: e5940094     	ldr	r0, [r4, #0x94]
   3db08: e3500000     	cmp	r0, #0
   3db0c: 0a000000     	beq	0x3db14
   3db10: ebff60ca     	bl	0x15e40    @ imm = #-0x27cd8 ; _ZdlPv
   3db14: e1a00004     	mov	r0, r4
   3db18: e59f3260     	ldr	r3, [pc, #0x260]        @ 0x3dd80
   3db1c: e5a03054     	str	r3, [r0, #0x54]!
   3db20: ebffc5a5     	bl	0x2f1bc
   3db24: eaffffe6     	b	0x3dac4
   3db28: eafffff1     	b	0x3daf4
   3db2c: e59504e8     	ldr	r0, [r5, #0x4e8]
   3db30: e3500000     	cmp	r0, #0
   3db34: 1a00007b     	bne	0x3dd28
   3db38: e59d300c     	ldr	r3, [sp, #0xc]
   3db3c: e2830068     	add	r0, r3, #104
   3db40: eb000d40     	bl	0x41048
   3db44: e2850fee     	add	r0, r5, #952
   3db48: eb000d3e     	bl	0x41048
   3db4c: e59d0008     	ldr	r0, [sp, #0x8]
   3db50: eb000d4b     	bl	0x41084
   3db54: e59d0004     	ldr	r0, [sp, #0x4]
   3db58: eb000d7b     	bl	0x4114c
   3db5c: e1a00009     	mov	r0, r9
   3db60: eb000db1     	bl	0x4122c
   3db64: e595003c     	ldr	r0, [r5, #0x3c]
   3db68: e3500000     	cmp	r0, #0
   3db6c: 0a000000     	beq	0x3db74
   3db70: ebff60b2     	bl	0x15e40    @ imm = #-0x27d38 ; _ZdlPv
   3db74: e5950030     	ldr	r0, [r5, #0x30]
   3db78: e3500000     	cmp	r0, #0
   3db7c: 0a000000     	beq	0x3db84
   3db80: ebff60ae     	bl	0x15e40    @ imm = #-0x27d48 ; _ZdlPv
   3db84: e5950024     	ldr	r0, [r5, #0x24]
   3db88: e3500000     	cmp	r0, #0
   3db8c: 0a000000     	beq	0x3db94
   3db90: ebff60aa     	bl	0x15e40    @ imm = #-0x27d58 ; _ZdlPv
   3db94: e5950018     	ldr	r0, [r5, #0x18]
   3db98: e3500000     	cmp	r0, #0
   3db9c: 0a000000     	beq	0x3dba4
   3dba0: ebff60a6     	bl	0x15e40    @ imm = #-0x27d68 ; _ZdlPv
   3dba4: e595000c     	ldr	r0, [r5, #0xc]
   3dba8: e3500000     	cmp	r0, #0
   3dbac: 0a000000     	beq	0x3dbb4
   3dbb0: ebff60a2     	bl	0x15e40    @ imm = #-0x27d78 ; _ZdlPv
   3dbb4: e59b0ffc     	ldr	r0, [r11, #0xffc]
   3dbb8: e3500000     	cmp	r0, #0
   3dbbc: 0a000000     	beq	0x3dbc4
   3dbc0: ebff609e     	bl	0x15e40    @ imm = #-0x27d88 ; _ZdlPv
   3dbc4: e59b0fe8     	ldr	r0, [r11, #0xfe8]
   3dbc8: e3500000     	cmp	r0, #0
   3dbcc: 0a000000     	beq	0x3dbd4
   3dbd0: ebff609a     	bl	0x15e40    @ imm = #-0x27d98 ; _ZdlPv
   3dbd4: e59f31a8     	ldr	r3, [pc, #0x1a8]        @ 0x3dd84
   3dbd8: e2865d0f     	add	r5, r6, #960
   3dbdc: e58b3fc0     	str	r3, [r11, #0xfc0]
   3dbe0: e1a00005     	mov	r0, r5
   3dbe4: ebffcc4a     	bl	0x30d14
   3dbe8: e1a00005     	mov	r0, r5
   3dbec: ebffcca2     	bl	0x30e7c
   3dbf0: e59b0fc8     	ldr	r0, [r11, #0xfc8]
   3dbf4: e2863e3d     	add	r3, r6, #976
   3dbf8: e1500003     	cmp	r0, r3
   3dbfc: 0a000000     	beq	0x3dc04
   3dc00: ebff608e     	bl	0x15e40    @ imm = #-0x27dc8 ; _ZdlPv
   3dc04: e2860fe9     	add	r0, r6, #932
   3dc08: e2865d0e     	add	r5, r6, #896
   3dc0c: eb00ca1f     	bl	0x70490
   3dc10: e59f3170     	ldr	r3, [pc, #0x170]        @ 0x3dd88
   3dc14: e1a00005     	mov	r0, r5
   3dc18: e58b3f80     	str	r3, [r11, #0xf80]
   3dc1c: ebffc9b1     	bl	0x302e8
   3dc20: e1a00005     	mov	r0, r5
   3dc24: ebffca08     	bl	0x3044c
   3dc28: e59b0f88     	ldr	r0, [r11, #0xf88]
   3dc2c: e2866e39     	add	r6, r6, #912
   3dc30: e1500006     	cmp	r0, r6
   3dc34: 1a000036     	bne	0x3dd14
   3dc38: e59f314c     	ldr	r3, [pc, #0x14c]        @ 0x3dd8c
   3dc3c: e1a00008     	mov	r0, r8
   3dc40: e58b3f58     	str	r3, [r11, #0xf58]
   3dc44: ebffdbfa     	bl	0x34c34
   3dc48: e59d0038     	ldr	r0, [sp, #0x38]
   3dc4c: eb00084b     	bl	0x3fd80
   3dc50: e59d0034     	ldr	r0, [sp, #0x34]
   3dc54: eb000839     	bl	0x3fd40
   3dc58: e59d0030     	ldr	r0, [sp, #0x30]
   3dc5c: eb000837     	bl	0x3fd40
   3dc60: e59d3010     	ldr	r3, [sp, #0x10]
   3dc64: e59309dc     	ldr	r0, [r3, #0x9dc]
   3dc68: e3500000     	cmp	r0, #0
   3dc6c: 0a000000     	beq	0x3dc74
   3dc70: ebff6072     	bl	0x15e40    @ imm = #-0x27e38 ; _ZdlPv
   3dc74: e59d3010     	ldr	r3, [sp, #0x10]
   3dc78: e59309d0     	ldr	r0, [r3, #0x9d0]
   3dc7c: e3500000     	cmp	r0, #0
   3dc80: 0a000000     	beq	0x3dc88
   3dc84: ebff606d     	bl	0x15e40    @ imm = #-0x27e4c ; _ZdlPv
   3dc88: e59d3010     	ldr	r3, [sp, #0x10]
   3dc8c: e59309c4     	ldr	r0, [r3, #0x9c4]
   3dc90: e3500000     	cmp	r0, #0
   3dc94: 0a000000     	beq	0x3dc9c
   3dc98: ebff6068     	bl	0x15e40    @ imm = #-0x27e60 ; _ZdlPv
   3dc9c: e59402cc     	ldr	r0, [r4, #0x2cc]
   3dca0: e3500000     	cmp	r0, #0
   3dca4: 0a000000     	beq	0x3dcac
   3dca8: ebff6064     	bl	0x15e40    @ imm = #-0x27e70 ; _ZdlPv
   3dcac: e59402c0     	ldr	r0, [r4, #0x2c0]
   3dcb0: e3500000     	cmp	r0, #0
   3dcb4: 0a000000     	beq	0x3dcbc
   3dcb8: ebff6060     	bl	0x15e40    @ imm = #-0x27e80 ; _ZdlPv
   3dcbc: e59d0018     	ldr	r0, [sp, #0x18]
   3dcc0: eb00091e     	bl	0x40140
   3dcc4: e59d0044     	ldr	r0, [sp, #0x44]
   3dcc8: eb00091c     	bl	0x40140
   3dccc: eaffff82     	b	0x3dadc
   3dcd0: eaffff9d     	b	0x3db4c
   3dcd4: eaffffa2     	b	0x3db64
   3dcd8: eaffffb9     	b	0x3dbc4
   3dcdc: eaffffdd     	b	0x3dc58
   3dce0: e59b0f60     	ldr	r0, [r11, #0xf60]
   3dce4: e1550000     	cmp	r5, r0
   3dce8: 0a000000     	beq	0x3dcf0
   3dcec: ebff6053     	bl	0x15e40    @ imm = #-0x27eb4 ; _ZdlPv
   3dcf0: e59d0050     	ldr	r0, [sp, #0x50]
   3dcf4: e28d3058     	add	r3, sp, #88
   3dcf8: e1500003     	cmp	r0, r3
   3dcfc: 0affffd1     	beq	0x3dc48
   3dd00: ebff604e     	bl	0x15e40    @ imm = #-0x27ec8 ; _ZdlPv
   3dd04: eaffffcf     	b	0x3dc48
   3dd08: eaffff91     	b	0x3db54
   3dd0c: eaffffb0     	b	0x3dbd4
   3dd10: eaffffc8     	b	0x3dc38
   3dd14: ebff6049     	bl	0x15e40    @ imm = #-0x27edc ; _ZdlPv
   3dd18: eaffffc6     	b	0x3dc38
   3dd1c: eaffffde     	b	0x3dc9c
   3dd20: eaffffe7     	b	0x3dcc4
   3dd24: eaffffcd     	b	0x3dc60
   3dd28: ebff6044     	bl	0x15e40    @ imm = #-0x27ef0 ; _ZdlPv
   3dd2c: eaffff81     	b	0x3db38
   3dd30: eaffff89     	b	0x3db5c
   3dd34: eaffffc3     	b	0x3dc48
   3dd38: eaffffec     	b	0x3dcf0
   3dd3c: eaffffc3     	b	0x3dc50
   3dd40: 00 00 00 00  	.word	0x00000000
   3dd44: 02 00 00 00  	.word	0x00000002
   3dd48: 18 2d 44 54  	.word	0x54442d18
   3dd4c: fb 21 09 40  	.word	0x400921fb
   3dd50: 00 01 02 03  	.word	0x03020100
   3dd54: 29 0d 26 24  	.word	0x24260d29
   3dd58: 22 2b 17 25  	.word	0x25172b22
   3dd5c: 83 84 85 87  	.word	0x87858483
   3dd60: 05 04 07 06  	.word	0x06070405
   3dd64: 28 16 2a 20  	.word	0x202a1628
   3dd68: 21 1b 19 23  	.word	0x23191b21
   3dd6c: 80 81 82 86  	.word	0x86828180
   3dd70: 41 12 40 47  	.word	0x47401241
   3dd74: cd cc 4c 3e  	.word	0x3e4ccccd
   3dd78: 14 23 07 00  	.word	0x00072314
   3dd7c: ec ee 08 00  	.word	0x0008eeec
   3dd80: e0 1e 07 00  	.word	0x00071ee0
   3dd84: 10 1f 07 00  	.word	0x00071f10
   3dd88: 00 1f 07 00  	.word	0x00071f00
   3dd8c: 94 2e 07 00  	.word	0x00072e94
