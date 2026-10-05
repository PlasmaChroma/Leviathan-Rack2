00002f54 <makingWndArray>:
    2f54: eef76a00     	vmov.f32	s13, #1.000000e+00
    2f58: e59f3420     	ldr	r3, [pc, #0x420]        @ 0x3380 <makingWndArray+0x42c>  // u32=0x16098; f32?=1.26486805e-40
    2f5c: e59f2420     	ldr	r2, [pc, #0x420]        @ 0x3384 <makingWndArray+0x430>  // u32=0x114; f32?=3.86758376e-43
    2f60: e08f3003     	add	r3, pc, r3
    2f64: e59f041c     	ldr	r0, [pc, #0x41c]        @ 0x3388 <makingWndArray+0x434>  // u32=0x3cbc; f32?=2.17873885e-41
    2f68: e92d43f0     	push	{r4, r5, r6, r7, r8, r9, lr}
    2f6c: e08f4000     	add	r4, pc, r0
    2f70: e793c002     	ldr	r12, [r3, r2]
    2f74: e2845b02     	add	r5, r4, #2048
    2f78: eddf3bf8     	vldr	d19, [pc, #992]         @ 0x3360 <makingWndArray+0x40c>  // f64=0.02
    2f7c: e285000c     	add	r0, r5, #12
    2f80: e28c6bcb     	add	r6, r12, #207872
    2f84: e3e0e031     	mvn	lr, #49
    2f88: e28650bc     	add	r5, r6, #188
    2f8c: ed9f6af9     	vldr	s12, [pc, #996]         @ 0x3378 <makingWndArray+0x424>  // f32=200
    2f90: eddf2bf4     	vldr	d18, [pc, #976]         @ 0x3368 <makingWndArray+0x414>  // f64=0.90000000000000002
    2f94: eddf1bf5     	vldr	d17, [pc, #980]         @ 0x3370 <makingWndArray+0x41c>  // f64=0.10000000000000001
    2f98: ee07ea90     	vmov	s15, lr
    2f9c: eeb85be7     	vcvt.f64.s32	d5, s15
    2fa0: ee255b23     	vmul.f64	d5, d5, d19
    2fa4: eeb70bc5     	vcvt.f32.f64	s0, d5
    2fa8: eeb14a40     	vneg.f32	s8, s0
    2fac: ee705a26     	vadd.f32	s11, s0, s13
    2fb0: ee764ac0     	vsub.f32	s9, s13, s0
    2fb4: eef74ac4     	vcvt.f64.f32	d20, s8
    2fb8: eeb50ac0     	vcmpe.f32	s0, #0
    2fbc: ee241ba2     	vmul.f64	d1, d20, d18
    2fc0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2fc4: ee640aa5     	vmul.f32	s1, s9, s11
    2fc8: ee602a06     	vmul.f32	s5, s0, s12
    2fcc: 9a00005b     	bls	0x3140 <makingWndArray+0x1ec> @ imm = #0x16c
    2fd0: e59f13b4     	ldr	r1, [pc, #0x3b4]        @ 0x338c <makingWndArray+0x438>  // u32=0x3c48; f32?=2.16248379e-41
    2fd4: e1a0700c     	mov	r7, r12
    2fd8: eddf1ae7     	vldr	s3, [pc, #924]          @ 0x337c <makingWndArray+0x428>  // f32=0
    2fdc: e3004202     	movw	r4, #0x202
    2fe0: e08f2001     	add	r2, pc, r1
    2fe4: e3a08001     	mov	r8, #1
    2fe8: e3001203     	movw	r1, #0x203
    2fec: e4923004     	ldr	r3, [r2], #4
    2ff0: ece71a01     	vstmia	r7!, {s3}
    2ff4: ea000039     	b	0x30e0 <makingWndArray+0x18c> @ imm = #0xe4
    2ff8: e3580f7e     	cmp	r8, #504
    2ffc: def03a66     	vmovle.f32	s7, s13
    3000: ca000048     	bgt	0x3128 <makingWndArray+0x1d4> @ imm = #0x120
    3004: ee018a10     	vmov	s2, r8
    3008: e2883001     	add	r3, r8, #1
    300c: e1a06002     	mov	r6, r2
    3010: ecf64a01     	vldmia	r6!, {s9}
    3014: eeb82ac1     	vcvt.f32.s32	s4, s2
    3018: ee327a22     	vadd.f32	s14, s4, s5
    301c: eebd3ac7     	vcvt.s32.f32	s6, s14
    3020: ee138a10     	vmov	r8, s6
    3024: ee607a24     	vmul.f32	s15, s0, s9
    3028: e1580001     	cmp	r8, r1
    302c: a1a08001     	movge	r8, r1
    3030: e0802108     	add	r2, r0, r8, lsl #2
    3034: ed925a00     	vldr	s10, [r2]
    3038: ee407a85     	vmla.f32	s15, s1, s10
    303c: eef47ae6     	vcmpe.f32	s15, s13
    3040: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3044: 8ef07a66     	vmovhi.f32	s15, s13
    3048: eef57ac0     	vcmpe.f32	s15, #0
    304c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3050: bef07a61     	vmovlt.f32	s15, s3
    3054: e3530009     	cmp	r3, #9
    3058: ee274aa3     	vmul.f32	s8, s15, s7
    305c: eca74a01     	vstmia	r7!, {s8}
    3060: da00002b     	ble	0x3114 <makingWndArray+0x1c0> @ imm = #0xac
    3064: e3530f7e     	cmp	r3, #504
    3068: c0442003     	subgt	r2, r4, r3
    306c: def03a66     	vmovle.f32	s7, s13
    3070: ce042a10     	vmovgt	s8, r2
    3074: cef80bc4     	vcvtgt.f64.s32	d16, s8
    3078: ce600ba1     	vmulgt.f64	d16, d16, d17
    307c: cef73be0     	vcvtgt.f32.f64	s7, d16
    3080: ee013a10     	vmov	s2, r3
    3084: e2838001     	add	r8, r3, #1
    3088: e1a02006     	mov	r2, r6
    308c: ecf25a01     	vldmia	r2!, {s11}
    3090: eeb82ac1     	vcvt.f32.s32	s4, s2
    3094: ee724a22     	vadd.f32	s9, s4, s5
    3098: eebd7ae4     	vcvt.s32.f32	s14, s9
    309c: ee173a10     	vmov	r3, s14
    30a0: ee203a25     	vmul.f32	s6, s0, s11
    30a4: e1530001     	cmp	r3, r1
    30a8: a1a03001     	movge	r3, r1
    30ac: e0803103     	add	r3, r0, r3, lsl #2
    30b0: edd37a00     	vldr	s15, [r3]
    30b4: ee003aa7     	vmla.f32	s6, s1, s15
    30b8: eeb43ae6     	vcmpe.f32	s6, s13
    30bc: eef1fa10     	vmrs	APSR_nzcv, fpscr
    30c0: 8eb03a66     	vmovhi.f32	s6, s13
    30c4: eeb53ac0     	vcmpe.f32	s6, #0
    30c8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    30cc: beb03a61     	vmovlt.f32	s6, s3
    30d0: e1580001     	cmp	r8, r1
    30d4: ee235a23     	vmul.f32	s10, s6, s7
    30d8: eca75a01     	vstmia	r7!, {s10}
    30dc: 0a000006     	beq	0x30fc <makingWndArray+0x1a8> @ imm = #0x18
    30e0: e3580009     	cmp	r8, #9
    30e4: caffffc3     	bgt	0x2ff8 <makingWndArray+0xa4> @ imm = #-0xf4
    30e8: ee058a90     	vmov	s11, r8
    30ec: eef8abe5     	vcvt.f64.s32	d26, s11
    30f0: ee6a0ba1     	vmul.f64	d16, d26, d17
    30f4: eef73be0     	vcvt.f32.f64	s7, d16
    30f8: eaffffc1     	b	0x3004 <makingWndArray+0xb0> @ imm = #-0xfc
    30fc: e28ccb02     	add	r12, r12, #2048
    3100: e28ee001     	add	lr, lr, #1
    3104: e28cc00c     	add	r12, r12, #12
    3108: e155000c     	cmp	r5, r12
    310c: 1affffa1     	bne	0x2f98 <makingWndArray+0x44> @ imm = #-0x17c
    3110: e8bd83f0     	pop	{r4, r5, r6, r7, r8, r9, pc}
    3114: ee033a90     	vmov	s7, r3
    3118: eef8bbe3     	vcvt.f64.s32	d27, s7
    311c: ee6b0ba1     	vmul.f64	d16, d27, d17
    3120: eef73be0     	vcvt.f32.f64	s7, d16
    3124: eaffffd5     	b	0x3080 <makingWndArray+0x12c> @ imm = #-0xac
    3128: e0446008     	sub	r6, r4, r8
    312c: ee036a90     	vmov	s7, r6
    3130: eef89be3     	vcvt.f64.s32	d25, s7
    3134: ee690ba1     	vmul.f64	d16, d25, d17
    3138: eef73be0     	vcvt.f32.f64	s7, d16
    313c: eaffffb0     	b	0x3004 <makingWndArray+0xb0> @ imm = #-0x140
    3140: e59f7248     	ldr	r7, [pc, #0x248]        @ 0x3390 <makingWndArray+0x43c>  // u32=0x3adc; f32?=2.11147653e-41
    3144: e1a0900c     	mov	r9, r12
    3148: e59f3244     	ldr	r3, [pc, #0x244]        @ 0x3394 <makingWndArray+0x440>  // u32=0x5acc; f32?=3.25717815e-41
    314c: e08f8007     	add	r8, pc, r7
    3150: ed9f2a89     	vldr	s4, [pc, #548]          @ 0x337c <makingWndArray+0x428>  // f32=0
    3154: e08f4003     	add	r4, pc, r3
    3158: e2882b02     	add	r2, r8, #2048
    315c: e3a03000     	mov	r3, #0
    3160: e282200c     	add	r2, r2, #12
    3164: e2446efe     	sub	r6, r4, #4064
    3168: e3008202     	movw	r8, #0x202
    316c: e3007203     	movw	r7, #0x203
    3170: ea000072     	b	0x3340 <makingWndArray+0x3ec> @ imm = #0x1c8
    3174: e3530f7e     	cmp	r3, #504
    3178: c0481003     	subgt	r1, r8, r3
    317c: def02a66     	vmovle.f32	s5, s13
    3180: ce041a10     	vmovgt	s8, r1
    3184: cef80bc4     	vcvtgt.f64.s32	d16, s8
    3188: ce600ba1     	vmulgt.f64	d16, d16, d17
    318c: cef72be0     	vcvtgt.f32.f64	s5, d16
    3190: ecb27a01     	vldmia	r2!, {s14}
    3194: e1a04006     	mov	r4, r6
    3198: e2833001     	add	r3, r3, #1
    319c: e1a01009     	mov	r1, r9
    31a0: ecb43a01     	vldmia	r4!, {s6}
    31a4: ee657a87     	vmul.f32	s15, s11, s14
    31a8: eef75ac3     	vcvt.f64.f32	d21, s6
    31ac: eef76ae7     	vcvt.f64.f32	d22, s15
    31b0: ee456b81     	vmla.f64	d22, d21, d1
    31b4: eeb75be6     	vcvt.f32.f64	s10, d22
    31b8: eeb55ac0     	vcmpe.f32	s10, #0
    31bc: eef1fa10     	vmrs	APSR_nzcv, fpscr
    31c0: beb05a42     	vmovlt.f32	s10, s4
    31c4: e3530009     	cmp	r3, #9
    31c8: ee250a22     	vmul.f32	s0, s10, s5
    31cc: eca10a01     	vstmia	r1!, {s0}
    31d0: da00007f     	ble	0x33d4 <makingWndArray+0x480> @ imm = #0x1fc
    31d4: e3530f7e     	cmp	r3, #504
    31d8: c0486003     	subgt	r6, r8, r3
    31dc: def02a66     	vmovle.f32	s5, s13
    31e0: ce006a10     	vmovgt	s0, r6
    31e4: cef86bc0     	vcvtgt.f64.s32	d22, s0
    31e8: ce666ba1     	vmulgt.f64	d22, d22, d17
    31ec: cef72be6     	vcvtgt.f32.f64	s5, d22
    31f0: edd20a00     	vldr	s1, [r2]
    31f4: e2839001     	add	r9, r3, #1
    31f8: edd44a00     	vldr	s9, [r4]
    31fc: ee254aa0     	vmul.f32	s8, s11, s1
    3200: eef79ae4     	vcvt.f64.f32	d25, s9
    3204: eef7aac4     	vcvt.f64.f32	d26, s8
    3208: ee49ab81     	vmla.f64	d26, d25, d1
    320c: eeb77bea     	vcvt.f32.f64	s14, d26
    3210: eeb57ac0     	vcmpe.f32	s14, #0
    3214: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3218: beb07a42     	vmovlt.f32	s14, s4
    321c: e3590009     	cmp	r9, #9
    3220: ee273a22     	vmul.f32	s6, s14, s5
    3224: ed813a00     	vstr	s6, [r1]
    3228: da000064     	ble	0x33c0 <makingWndArray+0x46c> @ imm = #0x190
    322c: e3590f7e     	cmp	r9, #504
    3230: c0489009     	subgt	r9, r8, r9
    3234: def04a66     	vmovle.f32	s9, s13
    3238: ce039a10     	vmovgt	s6, r9
    323c: cef8abc3     	vcvtgt.f64.s32	d26, s6
    3240: ce6aaba1     	vmulgt.f64	d26, d26, d17
    3244: cef74bea     	vcvtgt.f32.f64	s9, d26
    3248: ed925a01     	vldr	s10, [r2, #4]
    324c: e2836002     	add	r6, r3, #2
    3250: ed940a01     	vldr	s0, [r4, #4]
    3254: ee650a85     	vmul.f32	s1, s11, s10
    3258: eef7dac0     	vcvt.f64.f32	d29, s0
    325c: eef7eae0     	vcvt.f64.f32	d30, s1
    3260: ee4deb81     	vmla.f64	d30, d29, d1
    3264: eeb74bee     	vcvt.f32.f64	s8, d30
    3268: eeb54ac0     	vcmpe.f32	s8, #0
    326c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3270: beb04a42     	vmovlt.f32	s8, s4
    3274: e3560009     	cmp	r6, #9
    3278: ee247a24     	vmul.f32	s14, s8, s9
    327c: ed817a01     	vstr	s14, [r1, #4]
    3280: da000049     	ble	0x33ac <makingWndArray+0x458> @ imm = #0x124
    3284: e3560f7e     	cmp	r6, #504
    3288: c0486006     	subgt	r6, r8, r6
    328c: def04a66     	vmovle.f32	s9, s13
    3290: ce076a10     	vmovgt	s14, r6
    3294: cef8ebc7     	vcvtgt.f64.s32	d30, s14
    3298: ce6eeba1     	vmulgt.f64	d30, d30, d17
    329c: cef74bee     	vcvtgt.f32.f64	s9, d30
    32a0: ed923a02     	vldr	s6, [r2, #8]
    32a4: e2839003     	add	r9, r3, #3
    32a8: edd47a02     	vldr	s15, [r4, #8]
    32ac: ee255a83     	vmul.f32	s10, s11, s6
    32b0: eef75ae7     	vcvt.f64.f32	d21, s15
    32b4: eef70ac5     	vcvt.f64.f32	d16, s10
    32b8: ee450b81     	vmla.f64	d16, d21, d1
    32bc: eeb70be0     	vcvt.f32.f64	s0, d16
    32c0: eeb50ac0     	vcmpe.f32	s0, #0
    32c4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    32c8: beb00a42     	vmovlt.f32	s0, s4
    32cc: e3590009     	cmp	r9, #9
    32d0: ee600a24     	vmul.f32	s1, s0, s9
    32d4: edc10a02     	vstr	s1, [r1, #8]
    32d8: da00002e     	ble	0x3398 <makingWndArray+0x444> @ imm = #0xb8
    32dc: e3590f7e     	cmp	r9, #504
    32e0: c0489009     	subgt	r9, r8, r9
    32e4: def04a66     	vmovle.f32	s9, s13
    32e8: ce009a90     	vmovgt	s1, r9
    32ec: cef80be0     	vcvtgt.f64.s32	d16, s1
    32f0: ce600ba1     	vmulgt.f64	d16, d16, d17
    32f4: cef74be0     	vcvtgt.f32.f64	s9, d16
    32f8: ed927a03     	vldr	s14, [r2, #12]
    32fc: e2833004     	add	r3, r3, #4
    3300: e2822010     	add	r2, r2, #16
    3304: e2846010     	add	r6, r4, #16
    3308: e2819010     	add	r9, r1, #16
    330c: ed943a03     	vldr	s6, [r4, #12]
    3310: ee657a87     	vmul.f32	s15, s11, s14
    3314: eef78ac3     	vcvt.f64.f32	d24, s6
    3318: eef70ae7     	vcvt.f64.f32	d16, s15
    331c: ee480b81     	vmla.f64	d16, d24, d1
    3320: eeb75be0     	vcvt.f32.f64	s10, d16
    3324: eeb55ac0     	vcmpe.f32	s10, #0
    3328: eef1fa10     	vmrs	APSR_nzcv, fpscr
    332c: beb05a42     	vmovlt.f32	s10, s4
    3330: e1530007     	cmp	r3, r7
    3334: ee254a24     	vmul.f32	s8, s10, s9
    3338: ed814a03     	vstr	s8, [r1, #12]
    333c: 0affff6e     	beq	0x30fc <makingWndArray+0x1a8> @ imm = #-0x248
    3340: e3530009     	cmp	r3, #9
    3344: caffff8a     	bgt	0x3174 <makingWndArray+0x220> @ imm = #-0x1d8
    3348: ee023a90     	vmov	s5, r3
    334c: eef80be2     	vcvt.f64.s32	d16, s5
    3350: ee204ba1     	vmul.f64	d4, d16, d17
    3354: eef72bc4     	vcvt.f32.f64	s5, d4
    3358: eaffff8c     	b	0x3190 <makingWndArray+0x23c> @ imm = #-0x1d0
    335c: e320f000     	nop
    3360: 7b 14 ae 47  	.word	0x47ae147b
    3364: e1 7a 94 3f  	.word	0x3f947ae1
    3368: cd cc cc cc  	.word	0xcccccccd
    336c: cc cc ec 3f  	.word	0x3feccccc
    3370: 9a 99 99 99  	.word	0x9999999a
    3374: 99 99 b9 3f  	.word	0x3fb99999
    3378: 00 00 48 43  	.word	0x43480000
    337c: 00 00 00 00  	.word	0x00000000
    3380: 98 60 01 00  	.word	0x00016098
    3384: 14 01 00 00  	.word	0x00000114
    3388: bc 3c 00 00  	.word	0x00003cbc
    338c: 48 3c 00 00  	.word	0x00003c48
    3390: dc 3a 00 00  	.word	0x00003adc
    3394: cc 5a 00 00  	.word	0x00005acc
    3398: ee049a10     	vmov	s8, r9
    339c: eef86bc4     	vcvt.f64.s32	d22, s8
    33a0: ee667ba1     	vmul.f64	d23, d22, d17
    33a4: eef74be7     	vcvt.f32.f64	s9, d23
    33a8: eaffffd2     	b	0x32f8 <makingWndArray+0x3a4> @ imm = #-0xb8
    33ac: ee046a90     	vmov	s9, r6
    33b0: eef8fbe4     	vcvt.f64.s32	d31, s9
    33b4: ee6f4ba1     	vmul.f64	d20, d31, d17
    33b8: eef74be4     	vcvt.f32.f64	s9, d20
    33bc: eaffffb7     	b	0x32a0 <makingWndArray+0x34c> @ imm = #-0x124
    33c0: ee079a90     	vmov	s15, r9
    33c4: eef8bbe7     	vcvt.f64.s32	d27, s15
    33c8: ee6bcba1     	vmul.f64	d28, d27, d17
    33cc: eef74bec     	vcvt.f32.f64	s9, d28
    33d0: eaffff9c     	b	0x3248 <makingWndArray+0x2f4> @ imm = #-0x190
    33d4: ee043a90     	vmov	s9, r3
    33d8: eef87be4     	vcvt.f64.s32	d23, s9
    33dc: ee678ba1     	vmul.f64	d24, d23, d17
    33e0: eef72be8     	vcvt.f32.f64	s5, d24
    33e4: eaffff81     	b	0x31f0 <makingWndArray+0x29c> @ imm = #-0x1fc

