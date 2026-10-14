000050e4 <makingWndArray>:
    50e4: eef76a00     	vmov.f32	s13, #1.000000e+00
    50e8: e59f3420     	ldr	r3, [pc, #0x420]        @ 0x5510 <makingWndArray+0x42c>
    50ec: e59f2420     	ldr	r2, [pc, #0x420]        @ 0x5514 <makingWndArray+0x430>
    50f0: e08f3003     	add	r3, pc, r3
    50f4: e59f041c     	ldr	r0, [pc, #0x41c]        @ 0x5518 <makingWndArray+0x434>
    50f8: e92d43f0     	push	{r4, r5, r6, r7, r8, r9, lr}
    50fc: e08f4000     	add	r4, pc, r0
    5100: e793c002     	ldr	r12, [r3, r2]
    5104: e2845e81     	add	r5, r4, #2064
    5108: eddf3bf8     	vldr	d19, [pc, #992]         @ 0x54f0 <makingWndArray+0x40c>
    510c: e285000c     	add	r0, r5, #12
    5110: e28c6bcb     	add	r6, r12, #207872
    5114: e3e0e031     	mvn	lr, #49
    5118: e28650bc     	add	r5, r6, #188
    511c: ed9f6af9     	vldr	s12, [pc, #996]         @ 0x5508 <makingWndArray+0x424>
    5120: eddf2bf4     	vldr	d18, [pc, #976]         @ 0x54f8 <makingWndArray+0x414>
    5124: eddf1bf5     	vldr	d17, [pc, #980]         @ 0x5500 <makingWndArray+0x41c>
    5128: ee07ea90     	vmov	s15, lr
    512c: eeb85be7     	vcvt.f64.s32	d5, s15
    5130: ee255b23     	vmul.f64	d5, d5, d19
    5134: eeb70bc5     	vcvt.f32.f64	s0, d5
    5138: eeb14a40     	vneg.f32	s8, s0
    513c: ee705a26     	vadd.f32	s11, s0, s13
    5140: ee764ac0     	vsub.f32	s9, s13, s0
    5144: eef74ac4     	vcvt.f64.f32	d20, s8
    5148: eeb50ac0     	vcmpe.f32	s0, #0
    514c: ee241ba2     	vmul.f64	d1, d20, d18
    5150: eef1fa10     	vmrs	APSR_nzcv, fpscr
    5154: ee640aa5     	vmul.f32	s1, s9, s11
    5158: ee602a06     	vmul.f32	s5, s0, s12
    515c: 9a00005b     	bls	0x52d0 <makingWndArray+0x1ec> @ imm = #0x16c
    5160: eddf1ae9     	vldr	s3, [pc, #932]          @ 0x550c <makingWndArray+0x428>
    5164: e1a0700c     	mov	r7, r12
    5168: e59f13ac     	ldr	r1, [pc, #0x3ac]        @ 0x551c <makingWndArray+0x438>
    516c: e3004202     	movw	r4, #0x202
    5170: e3a08001     	mov	r8, #1
    5174: e08f6001     	add	r6, pc, r1
    5178: e3001203     	movw	r1, #0x203
    517c: e2862014     	add	r2, r6, #20
    5180: ece71a01     	vstmia	r7!, {s3}
    5184: ea000039     	b	0x5270 <makingWndArray+0x18c> @ imm = #0xe4
    5188: e3580f7e     	cmp	r8, #504
    518c: def03a66     	vmovle.f32	s7, s13
    5190: ca000048     	bgt	0x52b8 <makingWndArray+0x1d4> @ imm = #0x120
    5194: ee018a10     	vmov	s2, r8
    5198: e2883001     	add	r3, r8, #1
    519c: e1a06002     	mov	r6, r2
    51a0: ecf64a01     	vldmia	r6!, {s9}
    51a4: eeb82ac1     	vcvt.f32.s32	s4, s2
    51a8: ee327a22     	vadd.f32	s14, s4, s5
    51ac: eebd3ac7     	vcvt.s32.f32	s6, s14
    51b0: ee138a10     	vmov	r8, s6
    51b4: ee607a24     	vmul.f32	s15, s0, s9
    51b8: e1580001     	cmp	r8, r1
    51bc: a1a08001     	movge	r8, r1
    51c0: e0802108     	add	r2, r0, r8, lsl #2
    51c4: ed925a00     	vldr	s10, [r2]
    51c8: ee407a85     	vmla.f32	s15, s1, s10
    51cc: eef47ae6     	vcmpe.f32	s15, s13
    51d0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    51d4: 8ef07a66     	vmovhi.f32	s15, s13
    51d8: eef57ac0     	vcmpe.f32	s15, #0
    51dc: eef1fa10     	vmrs	APSR_nzcv, fpscr
    51e0: bef07a61     	vmovlt.f32	s15, s3
    51e4: e3530009     	cmp	r3, #9
    51e8: ee274aa3     	vmul.f32	s8, s15, s7
    51ec: eca74a01     	vstmia	r7!, {s8}
    51f0: da00002b     	ble	0x52a4 <makingWndArray+0x1c0> @ imm = #0xac
    51f4: e3530f7e     	cmp	r3, #504
    51f8: c0442003     	subgt	r2, r4, r3
    51fc: def03a66     	vmovle.f32	s7, s13
    5200: ce042a10     	vmovgt	s8, r2
    5204: cef80bc4     	vcvtgt.f64.s32	d16, s8
    5208: ce600ba1     	vmulgt.f64	d16, d16, d17
    520c: cef73be0     	vcvtgt.f32.f64	s7, d16
    5210: ee013a10     	vmov	s2, r3
    5214: e2838001     	add	r8, r3, #1
    5218: e1a02006     	mov	r2, r6
    521c: ecf25a01     	vldmia	r2!, {s11}
    5220: eeb82ac1     	vcvt.f32.s32	s4, s2
    5224: ee724a22     	vadd.f32	s9, s4, s5
    5228: eebd7ae4     	vcvt.s32.f32	s14, s9
    522c: ee173a10     	vmov	r3, s14
    5230: ee203a25     	vmul.f32	s6, s0, s11
    5234: e1530001     	cmp	r3, r1
    5238: a1a03001     	movge	r3, r1
    523c: e0803103     	add	r3, r0, r3, lsl #2
    5240: edd37a00     	vldr	s15, [r3]
    5244: ee003aa7     	vmla.f32	s6, s1, s15
    5248: eeb43ae6     	vcmpe.f32	s6, s13
    524c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    5250: 8eb03a66     	vmovhi.f32	s6, s13
    5254: eeb53ac0     	vcmpe.f32	s6, #0
    5258: eef1fa10     	vmrs	APSR_nzcv, fpscr
    525c: beb03a61     	vmovlt.f32	s6, s3
    5260: e1580001     	cmp	r8, r1
    5264: ee235a23     	vmul.f32	s10, s6, s7
    5268: eca75a01     	vstmia	r7!, {s10}
    526c: 0a000006     	beq	0x528c <makingWndArray+0x1a8> @ imm = #0x18
    5270: e3580009     	cmp	r8, #9
    5274: caffffc3     	bgt	0x5188 <makingWndArray+0xa4> @ imm = #-0xf4
    5278: ee058a90     	vmov	s11, r8
    527c: eef8abe5     	vcvt.f64.s32	d26, s11
    5280: ee6a0ba1     	vmul.f64	d16, d26, d17
    5284: eef73be0     	vcvt.f32.f64	s7, d16
    5288: eaffffc1     	b	0x5194 <makingWndArray+0xb0> @ imm = #-0xfc
    528c: e28ccb02     	add	r12, r12, #2048
    5290: e28ee001     	add	lr, lr, #1
    5294: e28cc00c     	add	r12, r12, #12
    5298: e155000c     	cmp	r5, r12
    529c: 1affffa1     	bne	0x5128 <makingWndArray+0x44> @ imm = #-0x17c
    52a0: e8bd83f0     	pop	{r4, r5, r6, r7, r8, r9, pc}
    52a4: ee033a90     	vmov	s7, r3
    52a8: eef8bbe3     	vcvt.f64.s32	d27, s7
    52ac: ee6b0ba1     	vmul.f64	d16, d27, d17
    52b0: eef73be0     	vcvt.f32.f64	s7, d16
    52b4: eaffffd5     	b	0x5210 <makingWndArray+0x12c> @ imm = #-0xac
    52b8: e0443008     	sub	r3, r4, r8
    52bc: ee033a90     	vmov	s7, r3
    52c0: eef89be3     	vcvt.f64.s32	d25, s7
    52c4: ee690ba1     	vmul.f64	d16, d25, d17
    52c8: eef73be0     	vcvt.f32.f64	s7, d16
    52cc: eaffffb0     	b	0x5194 <makingWndArray+0xb0> @ imm = #-0x140
    52d0: e59f7248     	ldr	r7, [pc, #0x248]        @ 0x5520 <makingWndArray+0x43c>
    52d4: e1a0900c     	mov	r9, r12
    52d8: e59f3244     	ldr	r3, [pc, #0x244]        @ 0x5524 <makingWndArray+0x440>
    52dc: e08f8007     	add	r8, pc, r7
    52e0: ed9f2a89     	vldr	s4, [pc, #548]          @ 0x550c <makingWndArray+0x428>
    52e4: e08f4003     	add	r4, pc, r3
    52e8: e2882e81     	add	r2, r8, #2064
    52ec: e3a03000     	mov	r3, #0
    52f0: e282200c     	add	r2, r2, #12
    52f4: e2446efd     	sub	r6, r4, #4048
    52f8: e3008202     	movw	r8, #0x202
    52fc: e3007203     	movw	r7, #0x203
    5300: ea000072     	b	0x54d0 <makingWndArray+0x3ec> @ imm = #0x1c8
    5304: e3530f7e     	cmp	r3, #504
    5308: c0481003     	subgt	r1, r8, r3
    530c: def02a66     	vmovle.f32	s5, s13
    5310: ce041a10     	vmovgt	s8, r1
    5314: cef80bc4     	vcvtgt.f64.s32	d16, s8
    5318: ce600ba1     	vmulgt.f64	d16, d16, d17
    531c: cef72be0     	vcvtgt.f32.f64	s5, d16
    5320: ecb27a01     	vldmia	r2!, {s14}
    5324: e1a04006     	mov	r4, r6
    5328: e2833001     	add	r3, r3, #1
    532c: e1a01009     	mov	r1, r9
    5330: ecb43a01     	vldmia	r4!, {s6}
    5334: ee657a87     	vmul.f32	s15, s11, s14
    5338: eef75ac3     	vcvt.f64.f32	d21, s6
    533c: eef76ae7     	vcvt.f64.f32	d22, s15
    5340: ee456b81     	vmla.f64	d22, d21, d1
    5344: eeb75be6     	vcvt.f32.f64	s10, d22
    5348: eeb55ac0     	vcmpe.f32	s10, #0
    534c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    5350: beb05a42     	vmovlt.f32	s10, s4
    5354: e3530009     	cmp	r3, #9
    5358: ee250a22     	vmul.f32	s0, s10, s5
    535c: eca10a01     	vstmia	r1!, {s0}
    5360: da00007f     	ble	0x5564 <makingWndArray+0x480> @ imm = #0x1fc
    5364: e3530f7e     	cmp	r3, #504
    5368: c0486003     	subgt	r6, r8, r3
    536c: def02a66     	vmovle.f32	s5, s13
    5370: ce006a10     	vmovgt	s0, r6
    5374: cef86bc0     	vcvtgt.f64.s32	d22, s0
    5378: ce666ba1     	vmulgt.f64	d22, d22, d17
    537c: cef72be6     	vcvtgt.f32.f64	s5, d22
    5380: edd20a00     	vldr	s1, [r2]
    5384: e2839001     	add	r9, r3, #1
    5388: edd44a00     	vldr	s9, [r4]
    538c: ee254aa0     	vmul.f32	s8, s11, s1
    5390: eef79ae4     	vcvt.f64.f32	d25, s9
    5394: eef7aac4     	vcvt.f64.f32	d26, s8
    5398: ee49ab81     	vmla.f64	d26, d25, d1
    539c: eeb77bea     	vcvt.f32.f64	s14, d26
    53a0: eeb57ac0     	vcmpe.f32	s14, #0
    53a4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    53a8: beb07a42     	vmovlt.f32	s14, s4
    53ac: e3590009     	cmp	r9, #9
    53b0: ee273a22     	vmul.f32	s6, s14, s5
    53b4: ed813a00     	vstr	s6, [r1]
    53b8: da000064     	ble	0x5550 <makingWndArray+0x46c> @ imm = #0x190
    53bc: e3590f7e     	cmp	r9, #504
    53c0: c0489009     	subgt	r9, r8, r9
    53c4: def04a66     	vmovle.f32	s9, s13
    53c8: ce039a10     	vmovgt	s6, r9
    53cc: cef8abc3     	vcvtgt.f64.s32	d26, s6
    53d0: ce6aaba1     	vmulgt.f64	d26, d26, d17
    53d4: cef74bea     	vcvtgt.f32.f64	s9, d26
    53d8: ed925a01     	vldr	s10, [r2, #4]
    53dc: e2836002     	add	r6, r3, #2
    53e0: ed940a01     	vldr	s0, [r4, #4]
    53e4: ee650a85     	vmul.f32	s1, s11, s10
    53e8: eef7dac0     	vcvt.f64.f32	d29, s0
    53ec: eef7eae0     	vcvt.f64.f32	d30, s1
    53f0: ee4deb81     	vmla.f64	d30, d29, d1
    53f4: eeb74bee     	vcvt.f32.f64	s8, d30
    53f8: eeb54ac0     	vcmpe.f32	s8, #0
    53fc: eef1fa10     	vmrs	APSR_nzcv, fpscr
    5400: beb04a42     	vmovlt.f32	s8, s4
    5404: e3560009     	cmp	r6, #9
    5408: ee247a24     	vmul.f32	s14, s8, s9
    540c: ed817a01     	vstr	s14, [r1, #4]
    5410: da000049     	ble	0x553c <makingWndArray+0x458> @ imm = #0x124
    5414: e3560f7e     	cmp	r6, #504
    5418: c0486006     	subgt	r6, r8, r6
    541c: def04a66     	vmovle.f32	s9, s13
    5420: ce076a10     	vmovgt	s14, r6
    5424: cef8ebc7     	vcvtgt.f64.s32	d30, s14
    5428: ce6eeba1     	vmulgt.f64	d30, d30, d17
    542c: cef74bee     	vcvtgt.f32.f64	s9, d30
    5430: ed923a02     	vldr	s6, [r2, #8]
    5434: e2839003     	add	r9, r3, #3
    5438: edd47a02     	vldr	s15, [r4, #8]
    543c: ee255a83     	vmul.f32	s10, s11, s6
    5440: eef75ae7     	vcvt.f64.f32	d21, s15
    5444: eef70ac5     	vcvt.f64.f32	d16, s10
    5448: ee450b81     	vmla.f64	d16, d21, d1
    544c: eeb70be0     	vcvt.f32.f64	s0, d16
    5450: eeb50ac0     	vcmpe.f32	s0, #0
    5454: eef1fa10     	vmrs	APSR_nzcv, fpscr
    5458: beb00a42     	vmovlt.f32	s0, s4
    545c: e3590009     	cmp	r9, #9
    5460: ee600a24     	vmul.f32	s1, s0, s9
    5464: edc10a02     	vstr	s1, [r1, #8]
    5468: da00002e     	ble	0x5528 <makingWndArray+0x444> @ imm = #0xb8
    546c: e3590f7e     	cmp	r9, #504
    5470: c0489009     	subgt	r9, r8, r9
    5474: def04a66     	vmovle.f32	s9, s13
    5478: ce009a90     	vmovgt	s1, r9
    547c: cef80be0     	vcvtgt.f64.s32	d16, s1
    5480: ce600ba1     	vmulgt.f64	d16, d16, d17
    5484: cef74be0     	vcvtgt.f32.f64	s9, d16
    5488: ed927a03     	vldr	s14, [r2, #12]
    548c: e2833004     	add	r3, r3, #4
    5490: e2822010     	add	r2, r2, #16
    5494: e2846010     	add	r6, r4, #16
    5498: e2819010     	add	r9, r1, #16
    549c: ed943a03     	vldr	s6, [r4, #12]
    54a0: ee657a87     	vmul.f32	s15, s11, s14
    54a4: eef78ac3     	vcvt.f64.f32	d24, s6
    54a8: eef70ae7     	vcvt.f64.f32	d16, s15
    54ac: ee480b81     	vmla.f64	d16, d24, d1
    54b0: eeb75be0     	vcvt.f32.f64	s10, d16
    54b4: eeb55ac0     	vcmpe.f32	s10, #0
    54b8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    54bc: beb05a42     	vmovlt.f32	s10, s4
    54c0: e1530007     	cmp	r3, r7
    54c4: ee254a24     	vmul.f32	s8, s10, s9
    54c8: ed814a03     	vstr	s8, [r1, #12]
    54cc: 0affff6e     	beq	0x528c <makingWndArray+0x1a8> @ imm = #-0x248
    54d0: e3530009     	cmp	r3, #9
    54d4: caffff8a     	bgt	0x5304 <makingWndArray+0x220> @ imm = #-0x1d8
    54d8: ee023a90     	vmov	s5, r3
    54dc: eef80be2     	vcvt.f64.s32	d16, s5
    54e0: ee204ba1     	vmul.f64	d4, d16, d17
    54e4: eef72bc4     	vcvt.f32.f64	s5, d4
    54e8: eaffff8c     	b	0x5320 <makingWndArray+0x23c> @ imm = #-0x1d0
    54ec: e320f000     	nop
    54f0: 7b 14 ae 47  	.word	0x47ae147b
    54f4: e1 7a 94 3f  	.word	0x3f947ae1
    54f8: cd cc cc cc  	.word	0xcccccccd
    54fc: cc cc ec 3f  	.word	0x3feccccc
    5500: 9a 99 99 99  	.word	0x9999999a
    5504: 99 99 b9 3f  	.word	0x3fb99999
    5508: 00 00 48 43  	.word	0x43480000
    550c: 00 00 00 00  	.word	0x00000000
    5510: 08 1f 02 00  	.word	0x00021f08
    5514: b4 02 00 00  	.word	0x000002b4
    5518: 18 df 00 00  	.word	0x0000df18
    551c: a0 de 00 00  	.word	0x0000dea0
    5520: 38 dd 00 00  	.word	0x0000dd38
    5524: 28 fd 00 00  	.word	0x0000fd28
    5528: ee049a10     	vmov	s8, r9
    552c: eef86bc4     	vcvt.f64.s32	d22, s8
    5530: ee667ba1     	vmul.f64	d23, d22, d17
    5534: eef74be7     	vcvt.f32.f64	s9, d23
    5538: eaffffd2     	b	0x5488 <makingWndArray+0x3a4> @ imm = #-0xb8
    553c: ee046a90     	vmov	s9, r6
    5540: eef8fbe4     	vcvt.f64.s32	d31, s9
    5544: ee6f4ba1     	vmul.f64	d20, d31, d17
    5548: eef74be4     	vcvt.f32.f64	s9, d20
    554c: eaffffb7     	b	0x5430 <makingWndArray+0x34c> @ imm = #-0x124
    5550: ee079a90     	vmov	s15, r9
    5554: eef8bbe7     	vcvt.f64.s32	d27, s15
    5558: ee6bcba1     	vmul.f64	d28, d27, d17
    555c: eef74bec     	vcvt.f32.f64	s9, d28
    5560: eaffff9c     	b	0x53d8 <makingWndArray+0x2f4> @ imm = #-0x190
    5564: ee043a90     	vmov	s9, r3
    5568: eef87be4     	vcvt.f64.s32	d23, s9
    556c: ee678ba1     	vmul.f64	d24, d23, d17
    5570: eef72be8     	vcvt.f32.f64	s5, d24
    5574: eaffff81     	b	0x5380 <makingWndArray+0x29c> @ imm = #-0x1fc

