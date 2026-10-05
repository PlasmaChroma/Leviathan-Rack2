; idsp::MonoPlate::_process(float&)
; VA 0x496c0 size 3016

   496c0: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   496c4: e1a04000     	mov	r4, r0
   496c8: e5903f74     	ldr	r3, [r0, #0xf74]
   496cc: ed2d8b0c     	vpush	{d8, d9, d10, d11, d12, d13}
   496d0: e24dd04c     	sub	sp, sp, #76
   496d4: edd1ba00     	vldr	s23, [r1]
   496d8: e3530001     	cmp	r3, #1
   496dc: e58d1044     	str	r1, [sp, #0x44]
   496e0: 0a0002d2     	beq	0x4a230
   496e4: e3530002     	cmp	r3, #2
   496e8: 0a0002c1     	beq	0x4a1f4
   496ec: e3530000     	cmp	r3, #0
   496f0: 0a0002b2     	beq	0x4a1c0
   496f4: e2846a09     	add	r6, r4, #36864
   496f8: e5963ed8     	ldr	r3, [r6, #0xed8]
   496fc: e5962edc     	ldr	r2, [r6, #0xedc]
   49700: e2431f6d     	sub	r1, r3, #436
   49704: e58d3004     	str	r3, [sp, #0x4]
   49708: e2411001     	sub	r1, r1, #1
   4970c: e58d2008     	str	r2, [sp, #0x8]
   49710: e3510000     	cmp	r1, #0
   49714: b0811002     	addlt	r1, r1, r2
   49718: ba000001     	blt	0x49724
   4971c: e1510002     	cmp	r1, r2
   49720: a0411002     	subge	r1, r1, r2
   49724: e5962ed4     	ldr	r2, [r6, #0xed4]
   49728: e59d3004     	ldr	r3, [sp, #0x4]
   4972c: e5922000     	ldr	r2, [r2]
   49730: e2433c13     	sub	r3, r3, #4864
   49734: e2433019     	sub	r3, r3, #25
   49738: e3530000     	cmp	r3, #0
   4973c: e0821101     	add	r1, r2, r1, lsl #2
   49740: ed91ba00     	vldr	s22, [r1]
   49744: e59d1008     	ldr	r1, [sp, #0x8]
   49748: b0833001     	addlt	r3, r3, r1
   4974c: ba000001     	blt	0x49758
   49750: e1530001     	cmp	r3, r1
   49754: a0433001     	subge	r3, r3, r1
   49758: e284e903     	add	lr, r4, #49152
   4975c: e0823103     	add	r3, r2, r3, lsl #2
   49760: eef47a00     	vmov.f32	s15, #1.250000e-01
   49764: e59e1d38     	ldr	r1, [lr, #0xd38]
   49768: ed938a00     	vldr	s16, [r3]
   4976c: e2413ea5     	sub	r3, r1, #2640
   49770: e58d103c     	str	r1, [sp, #0x3c]
   49774: e243300f     	sub	r3, r3, #15
   49778: e59e1d3c     	ldr	r1, [lr, #0xd3c]
   4977c: e3530000     	cmp	r3, #0
   49780: ee288a27     	vmul.f32	s16, s16, s15
   49784: b0833001     	addlt	r3, r3, r1
   49788: e58d1028     	str	r1, [sp, #0x28]
   4978c: ba000001     	blt	0x49798
   49790: e1530001     	cmp	r3, r1
   49794: a0433001     	subge	r3, r3, r1
   49798: e59e1d34     	ldr	r1, [lr, #0xd34]
   4979c: e2849a11     	add	r9, r4, #69632
   497a0: eddf7ac7     	vldr	s15, [pc, #796]         @ 0x49ac4 ; float 0.300000011921
   497a4: e5911000     	ldr	r1, [r1]
   497a8: e5990de0     	ldr	r0, [r9, #0xde0]
   497ac: e58d1040     	str	r1, [sp, #0x40]
   497b0: e0813103     	add	r3, r1, r3, lsl #2
   497b4: e2401ecd     	sub	r1, r0, #3280
   497b8: e599cde4     	ldr	r12, [r9, #0xde4]
   497bc: e2411002     	sub	r1, r1, #2
   497c0: e3510000     	cmp	r1, #0
   497c4: e58d0014     	str	r0, [sp, #0x14]
   497c8: edd3aa00     	vldr	s21, [r3]
   497cc: b081100c     	addlt	r1, r1, r12
   497d0: e58dc024     	str	r12, [sp, #0x24]
   497d4: ee6aaaa7     	vmul.f32	s21, s21, s15
   497d8: ba000001     	blt	0x497e4
   497dc: e151000c     	cmp	r1, r12
   497e0: a041100c     	subge	r1, r1, r12
   497e4: e5993ddc     	ldr	r3, [r9, #0xddc]
   497e8: e2847a19     	add	r7, r4, #102400
   497ec: eddf7ab5     	vldr	s15, [pc, #724]         @ 0x49ac8 ; float 0.20000000298
   497f0: e593c000     	ldr	r12, [r3]
   497f4: e59738a8     	ldr	r3, [r7, #0x8a8]
   497f8: e58d3010     	str	r3, [sp, #0x10]
   497fc: e08c1101     	add	r1, r12, r1, lsl #2
   49800: e2433d33     	sub	r3, r3, #3264
   49804: e59708ac     	ldr	r0, [r7, #0x8ac]
   49808: e2433008     	sub	r3, r3, #8
   4980c: e3530000     	cmp	r3, #0
   49810: e58d0020     	str	r0, [sp, #0x20]
   49814: ed91aa00     	vldr	s20, [r1]
   49818: b0833000     	addlt	r3, r3, r0
   4981c: ee2aaa27     	vmul.f32	s20, s20, s15
   49820: ba000001     	blt	0x4982c
   49824: e1530000     	cmp	r3, r0
   49828: a0433000     	subge	r3, r3, r0
   4982c: e59718a4     	ldr	r1, [r7, #0x8a4]
   49830: e2840a1d     	add	r0, r4, #118784
   49834: eef57a00     	vmov.f32	s15, #2.500000e-01
   49838: e5911000     	ldr	r1, [r1]
   4983c: e5905d2c     	ldr	r5, [r0, #0xd2c]
   49840: e58d1038     	str	r1, [sp, #0x38]
   49844: e0813103     	add	r3, r1, r3, lsl #2
   49848: e2451e13     	sub	r1, r5, #304
   4984c: e5908d30     	ldr	r8, [r0, #0xd30]
   49850: e2411003     	sub	r1, r1, #3
   49854: e3510000     	cmp	r1, #0
   49858: e58d5030     	str	r5, [sp, #0x30]
   4985c: edd39a00     	vldr	s19, [r3]
   49860: b0811008     	addlt	r1, r1, r8
   49864: e58d801c     	str	r8, [sp, #0x1c]
   49868: ee699aa7     	vmul.f32	s19, s19, s15
   4986c: ba000001     	blt	0x49878
   49870: e1510008     	cmp	r1, r8
   49874: a0411008     	subge	r1, r1, r8
   49878: e5903d28     	ldr	r3, [r0, #0xd28]
   4987c: e2845a23     	add	r5, r4, #143360
   49880: eddf7a91     	vldr	s15, [pc, #580]         @ 0x49acc ; float 0.875299990177
   49884: e5933000     	ldr	r3, [r3]
   49888: e5958cd4     	ldr	r8, [r5, #0xcd4]
   4988c: e58d3034     	str	r3, [sp, #0x34]
   49890: e0831101     	add	r1, r3, r1, lsl #2
   49894: e2483e6d     	sub	r3, r8, #1744
   49898: e595acd8     	ldr	r10, [r5, #0xcd8]
   4989c: e2433009     	sub	r3, r3, #9
   498a0: e3530000     	cmp	r3, #0
   498a4: e58d800c     	str	r8, [sp, #0xc]
   498a8: ed919a00     	vldr	s18, [r1]
   498ac: b083300a     	addlt	r3, r3, r10
   498b0: e58da018     	str	r10, [sp, #0x18]
   498b4: ee299a27     	vmul.f32	s18, s18, s15
   498b8: ba000001     	blt	0x498c4
   498bc: e153000a     	cmp	r3, r10
   498c0: a043300a     	subge	r3, r3, r10
   498c4: e5951cd0     	ldr	r1, [r5, #0xcd0]
   498c8: eef67a00     	vmov.f32	s15, #5.000000e-01
   498cc: e594af58     	ldr	r10, [r4, #0xf58]
   498d0: e5948f5c     	ldr	r8, [r4, #0xf5c]
   498d4: e5911000     	ldr	r1, [r1]
   498d8: e59aa000     	ldr	r10, [r10]
   498dc: e594bf64     	ldr	r11, [r4, #0xf64]
   498e0: e0813103     	add	r3, r1, r3, lsl #2
   498e4: e58d102c     	str	r1, [sp, #0x2c]
   498e8: e08aa108     	add	r10, r10, r8, lsl #2
   498ec: e35b0001     	cmp	r11, #1
   498f0: edd38a00     	vldr	s17, [r3]
   498f4: ed9a7a00     	vldr	s14, [r10]
   498f8: ee688aa7     	vmul.f32	s17, s17, s15
   498fc: 0a000221     	beq	0x4a188
   49900: e35b0002     	cmp	r11, #2
   49904: 0a000210     	beq	0x4a14c
   49908: e35b0000     	cmp	r11, #0
   4990c: 0a000201     	beq	0x4a118
   49910: e5943f60     	ldr	r3, [r4, #0xf60]
   49914: e2888001     	add	r8, r8, #1
   49918: edcaba00     	vstr	s23, [r10]
   4991c: e1580003     	cmp	r8, r3
   49920: e5848f5c     	str	r8, [r4, #0xf5c]
   49924: 23a03000     	movhs	r3, #0
   49928: 25843f5c     	strhs	r3, [r4, #0xf5c]
   4992c: e2843a01     	add	r3, r4, #4096
   49930: e5938330     	ldr	r8, [r3, #0x330]
   49934: e593b334     	ldr	r11, [r3, #0x334]
   49938: edd36acf     	vldr	s13, [r3, #828]
   4993c: e5988000     	ldr	r8, [r8]
   49940: e28ba001     	add	r10, r11, #1
   49944: e593160c     	ldr	r1, [r3, #0x60c]
   49948: e088810b     	add	r8, r8, r11, lsl #2
   4994c: e593b338     	ldr	r11, [r3, #0x338]
   49950: e15a000b     	cmp	r10, r11
   49954: e593b608     	ldr	r11, [r3, #0x608]
   49958: edd87a00     	vldr	s15, [r8]
   4995c: eea77aa6     	vfma.f32	s14, s15, s13
   49960: ed887a00     	vstr	s14, [r8]
   49964: 23a08000     	movhs	r8, #0
   49968: e583a334     	str	r10, [r3, #0x334]
   4996c: e283ac06     	add	r10, r3, #1536
   49970: 25838334     	strhs	r8, [r3, #0x334]
   49974: eee67ac7     	vfms.f32	s15, s13, s14
   49978: e5938604     	ldr	r8, [r3, #0x604]
   4997c: edda6a04     	vldr	s13, [r10, #16]
   49980: e28ba001     	add	r10, r11, #1
   49984: e15a0001     	cmp	r10, r1
   49988: e5931fe0     	ldr	r1, [r3, #0xfe0]
   4998c: e5988000     	ldr	r8, [r8]
   49990: e088810b     	add	r8, r8, r11, lsl #2
   49994: e593bfdc     	ldr	r11, [r3, #0xfdc]
   49998: ed987a00     	vldr	s14, [r8]
   4999c: eee77a26     	vfma.f32	s15, s14, s13
   499a0: edc87a00     	vstr	s15, [r8]
   499a4: 23a08000     	movhs	r8, #0
   499a8: e583a608     	str	r10, [r3, #0x608]
   499ac: e283aefd     	add	r10, r3, #4048
   499b0: 25838608     	strhs	r8, [r3, #0x608]
   499b4: eea67ae7     	vfms.f32	s14, s13, s15
   499b8: e5938fd8     	ldr	r8, [r3, #0xfd8]
   499bc: edda6a05     	vldr	s13, [r10, #20]
   499c0: e28ba001     	add	r10, r11, #1
   499c4: e15a0001     	cmp	r10, r1
   499c8: e5988000     	ldr	r8, [r8]
   499cc: e088810b     	add	r8, r8, r11, lsl #2
   499d0: edd87a00     	vldr	s15, [r8]
   499d4: eea77aa6     	vfma.f32	s14, s15, s13
   499d8: ed887a00     	vstr	s14, [r8]
   499dc: 23a08000     	movhs	r8, #0
   499e0: e583afdc     	str	r10, [r3, #0xfdc]
   499e4: e284ac27     	add	r10, r4, #9984
   499e8: 25838fdc     	strhs	r8, [r3, #0xfdc]
   499ec: e2843a02     	add	r3, r4, #8192
   499f0: eee67ac7     	vfms.f32	s15, s13, s14
   499f4: e593870c     	ldr	r8, [r3, #0x70c]
   499f8: e593b710     	ldr	r11, [r3, #0x710]
   499fc: ed9a7a06     	vldr	s14, [r10, #24]
   49a00: e5988000     	ldr	r8, [r8]
   49a04: e28ba001     	add	r10, r11, #1
   49a08: e5931714     	ldr	r1, [r3, #0x714]
   49a0c: e088810b     	add	r8, r8, r11, lsl #2
   49a10: e15a0001     	cmp	r10, r1
   49a14: e59d1014     	ldr	r1, [sp, #0x14]
   49a18: edd8ca00     	vldr	s25, [r8]
   49a1c: e08cc101     	add	r12, r12, r1, lsl #2
   49a20: e59d100c     	ldr	r1, [sp, #0xc]
   49a24: eeec7a87     	vfma.f32	s15, s25, s14
   49a28: edc87a00     	vstr	s15, [r8]
   49a2c: 23a08000     	movhs	r8, #0
   49a30: e583a710     	str	r10, [r3, #0x710]
   49a34: eee7ca67     	vfms.f32	s25, s14, s15
   49a38: 25838710     	strhs	r8, [r3, #0x710]
   49a3c: e2853ecd     	add	r3, r5, #3280
   49a40: e59d802c     	ldr	r8, [sp, #0x2c]
   49a44: ed9c7a00     	vldr	s14, [r12]
   49a48: edd36a03     	vldr	s13, [r3, #12]
   49a4c: e0881101     	add	r1, r8, r1, lsl #2
   49a50: e59d8004     	ldr	r8, [sp, #0x4]
   49a54: e5963ee0     	ldr	r3, [r6, #0xee0]
   49a58: ee277a26     	vmul.f32	s14, s14, s13
   49a5c: e082b108     	add	r11, r2, r8, lsl #2
   49a60: e2852ecf     	add	r2, r5, #3312
   49a64: eeb0da6c     	vmov.f32	s26, s25
   49a68: e3530001     	cmp	r3, #1
   49a6c: ed827a00     	vstr	s14, [r2]
   49a70: edd17a00     	vldr	s15, [r1]
   49a74: ee677aa6     	vmul.f32	s15, s15, s13
   49a78: edc27a01     	vstr	s15, [r2, #4]
   49a7c: ed9b7a00     	vldr	s14, [r11]
   49a80: 0a000196     	beq	0x4a0e0
   49a84: e3530002     	cmp	r3, #2
   49a88: 0a000185     	beq	0x4a0a4
   49a8c: e3530000     	cmp	r3, #0
   49a90: 0a000176     	beq	0x4a070
   49a94: e59d3038     	ldr	r3, [sp, #0x38]
   49a98: e59d2010     	ldr	r2, [sp, #0x10]
   49a9c: e0838102     	add	r8, r3, r2, lsl #2
   49aa0: e59738b0     	ldr	r3, [r7, #0x8b0]
   49aa4: e3530001     	cmp	r3, #1
   49aa8: edd87a00     	vldr	s15, [r8]
   49aac: 0a000161     	beq	0x4a038
   49ab0: e3530002     	cmp	r3, #2
   49ab4: 0a000150     	beq	0x49ffc
   49ab8: e3530000     	cmp	r3, #0
   49abc: 0a000141     	beq	0x49fc8
   49ac0: ea000004     	b	0x49ad8
   49ac4: 9a 99 99 3e  	.word	0x3e99999a
   49ac8: cd cc 4c 3e  	.word	0x3e4ccccd
   49acc: a9 13 60 3f  	.word	0x3f6013a9
   49ad0: 00 00 fe 42  	.word	0x42fe0000
   49ad4: ad aa 2a 3e  	.word	0x3e2aaaad
   49ad8: e59d303c     	ldr	r3, [sp, #0x3c]
   49adc: e28e2ed3     	add	r2, lr, #3376
   49ae0: e59da040     	ldr	r10, [sp, #0x40]
   49ae4: e2822004     	add	r2, r2, #4
   49ae8: e58d202c     	str	r2, [sp, #0x2c]
   49aec: e59d2028     	ldr	r2, [sp, #0x28]
   49af0: e08aa103     	add	r10, r10, r3, lsl #2
   49af4: e2833001     	add	r3, r3, #1
   49af8: e1530002     	cmp	r3, r2
   49afc: e59d202c     	ldr	r2, [sp, #0x2c]
   49b00: 23a03000     	movhs	r3, #0
   49b04: edda6a00     	vldr	s13, [r10]
   49b08: ed926a03     	vldr	s12, [r2, #12]
   49b0c: e59d2024     	ldr	r2, [sp, #0x24]
   49b10: eea67a86     	vfma.f32	s14, s13, s12
   49b14: eee66a47     	vfms.f32	s13, s12, s14
   49b18: ed8a7a00     	vstr	s14, [r10]
   49b1c: e58e3d38     	str	r3, [lr, #0xd38]
   49b20: e59d3014     	ldr	r3, [sp, #0x14]
   49b24: e59de01c     	ldr	lr, [sp, #0x1c]
   49b28: e2833001     	add	r3, r3, #1
   49b2c: e1530002     	cmp	r3, r2
   49b30: e59d2034     	ldr	r2, [sp, #0x34]
   49b34: 23a03000     	movhs	r3, #0
   49b38: edcc6a00     	vstr	s13, [r12]
   49b3c: e59dc030     	ldr	r12, [sp, #0x30]
   49b40: e5893de0     	str	r3, [r9, #0xde0]
   49b44: e2803ed2     	add	r3, r0, #3360
   49b48: e2849a03     	add	r9, r4, #12288
   49b4c: e082210c     	add	r2, r2, r12, lsl #2
   49b50: e28cc001     	add	r12, r12, #1
   49b54: edd36a05     	vldr	s13, [r3, #20]
   49b58: e15c000e     	cmp	r12, lr
   49b5c: 23a03000     	movhs	r3, #0
   49b60: ed927a00     	vldr	s14, [r2]
   49b64: eee77a26     	vfma.f32	s15, s14, s13
   49b68: eea67ae7     	vfms.f32	s14, s13, s15
   49b6c: edc27a00     	vstr	s15, [r2]
   49b70: 25803d2c     	strhs	r3, [r0, #0xd2c]
   49b74: 3580cd2c     	strlo	r12, [r0, #0xd2c]
   49b78: e59d300c     	ldr	r3, [sp, #0xc]
   49b7c: e59d2018     	ldr	r2, [sp, #0x18]
   49b80: e2833001     	add	r3, r3, #1
   49b84: e1530002     	cmp	r3, r2
   49b88: 23a03000     	movhs	r3, #0
   49b8c: ed817a00     	vstr	s14, [r1]
   49b90: ed99ca1c     	vldr	s24, [r9, #112]
   49b94: ed990a1b     	vldr	s0, [r9, #108]
   49b98: e5853cd4     	str	r3, [r5, #0xcd4]
   49b9c: e2853ecf     	add	r3, r5, #3312
   49ba0: ee3cca00     	vadd.f32	s24, s24, s0
   49ba4: edd3ca01     	vldr	s25, [r3, #4]
   49ba8: eeb00a4c     	vmov.f32	s0, s24
   49bac: ee7dca2c     	vadd.f32	s25, s26, s25
   49bb0: ebff31fb     	bl	0x163a4    @ imm = #-0x33814 ; floorf
   49bb4: ee3cca40     	vsub.f32	s24, s24, s0
   49bb8: ed5f6a3c     	vldr	s13, [pc, #-240]        @ 0x49ad0 ; float 127
   49bbc: e5991054     	ldr	r1, [r9, #0x54]
   49bc0: ed997a19     	vldr	s14, [r9, #100]
   49bc4: ee6c6a26     	vmul.f32	s13, s24, s13
   49bc8: e5990058     	ldr	r0, [r9, #0x58]
   49bcc: e5912004     	ldr	r2, [r1, #0x4]
   49bd0: ed89ca1c     	vstr	s24, [r9, #112]
   49bd4: e2423001     	sub	r3, r2, #1
   49bd8: ee053a90     	vmov	s11, r3
   49bdc: eebc6ae6     	vcvt.u32.f32	s12, s13
   49be0: eef85a65     	vcvt.f32.u32	s11, s11
   49be4: ee163a10     	vmov	r3, s12
   49be8: eeb86a46     	vcvt.f32.u32	s12, s12
   49bec: e0843103     	add	r3, r4, r3, lsl #2
   49bf0: ee766ac6     	vsub.f32	s13, s13, s12
   49bf4: e2833a03     	add	r3, r3, #12288
   49bf8: edd37a1e     	vldr	s15, [r3, #120]
   49bfc: ed936a1f     	vldr	s12, [r3, #124]
   49c00: ee366a67     	vsub.f32	s12, s12, s15
   49c04: eee67a86     	vfma.f32	s15, s13, s12
   49c08: ee062a90     	vmov	s13, r2
   49c0c: eef86a66     	vcvt.f32.u32	s13, s13
   49c10: ee677a87     	vmul.f32	s15, s15, s14
   49c14: ee070a10     	vmov	s14, r0
   49c18: eeb87a47     	vcvt.f32.u32	s14, s14
   49c1c: eef47ae5     	vcmpe.f32	s15, s11
   49c20: eef1fa10     	vmrs	APSR_nzcv, fpscr
   49c24: ce777ae5     	vsubgt.f32	s15, s15, s11
   49c28: ee767ae7     	vsub.f32	s15, s13, s15
   49c2c: ee377a67     	vsub.f32	s14, s14, s15
   49c30: eefd7ac7     	vcvt.s32.f32	s15, s14
   49c34: ee173a90     	vmov	r3, s15
   49c38: eef87ae7     	vcvt.f32.s32	s15, s15
   49c3c: e253e001     	subs	lr, r3, #1
   49c40: ee377a67     	vsub.f32	s14, s14, s15
   49c44: 408ee002     	addmi	lr, lr, r2
   49c48: 4a000001     	bmi	0x49c54
   49c4c: e15e0002     	cmp	lr, r2
   49c50: a04ee002     	subge	lr, lr, r2
   49c54: e5911000     	ldr	r1, [r1]
   49c58: e3530000     	cmp	r3, #0
   49c5c: e283c001     	add	r12, r3, #1
   49c60: e081e10e     	add	lr, r1, lr, lsl #2
   49c64: ed9e4a00     	vldr	s8, [lr]
   49c68: ba000008     	blt	0x49c90
   49c6c: e1530002     	cmp	r3, r2
   49c70: a043e002     	subge	lr, r3, r2
   49c74: b1a0e003     	movlt	lr, r3
   49c78: e081e10e     	add	lr, r1, lr, lsl #2
   49c7c: e152000c     	cmp	r2, r12
   49c80: edde7a00     	vldr	s15, [lr]
   49c84: ca000072     	bgt	0x49e54
   49c88: e04cc002     	sub	r12, r12, r2
   49c8c: ea000005     	b	0x49ca8
   49c90: e083e002     	add	lr, r3, r2
   49c94: e35c0000     	cmp	r12, #0
   49c98: b08cc002     	addlt	r12, r12, r2
   49c9c: e081e10e     	add	lr, r1, lr, lsl #2
   49ca0: edde7a00     	vldr	s15, [lr]
   49ca4: aa00016f     	bge	0x4a268
   49ca8: e081c10c     	add	r12, r1, r12, lsl #2
   49cac: e2933002     	adds	r3, r3, #2
   49cb0: 40833002     	addmi	r3, r3, r2
   49cb4: eddc6a00     	vldr	s13, [r12]
   49cb8: 5a000068     	bpl	0x49e60
   49cbc: e0813103     	add	r3, r1, r3, lsl #2
   49cc0: eef05a00     	vmov.f32	s11, #2.000000e+00
   49cc4: ee766ae7     	vsub.f32	s13, s13, s15
   49cc8: eeb03a08     	vmov.f32	s6, #3.000000e+00
   49ccc: eef74a00     	vmov.f32	s9, #1.000000e+00
   49cd0: ed1f5a81     	vldr	s10, [pc, #-516]        @ 0x49ad4 ; float 0.166666701436
   49cd4: ed936a00     	vldr	s12, [r3]
   49cd8: e0811100     	add	r1, r1, r0, lsl #2
   49cdc: ee744ac7     	vsub.f32	s9, s9, s14
   49ce0: e599305c     	ldr	r3, [r9, #0x5c]
   49ce4: e2800001     	add	r0, r0, #1
   49ce8: e59d2008     	ldr	r2, [sp, #0x8]
   49cec: eef03a46     	vmov.f32	s7, s12
   49cf0: ee366a44     	vsub.f32	s12, s12, s8
   49cf4: eee43a25     	vfma.f32	s7, s8, s11
   49cf8: eea66ac3     	vfms.f32	s12, s13, s6
   49cfc: ee255a64     	vnmul.f32	s10, s10, s9
   49d00: e1500003     	cmp	r0, r3
   49d04: 23a03000     	movhs	r3, #0
   49d08: eef05a63     	vmov.f32	s11, s7
   49d0c: edd93a18     	vldr	s7, [r9, #96]
   49d10: eee75ac3     	vfms.f32	s11, s15, s6
   49d14: eee75a06     	vfma.f32	s11, s14, s12
   49d18: eee56a25     	vfma.f32	s13, s10, s11
   49d1c: eee77a26     	vfma.f32	s15, s14, s13
   49d20: eee3caa7     	vfma.f32	s25, s7, s15
   49d24: eee37aec     	vfms.f32	s15, s7, s25
   49d28: edc1ca00     	vstr	s25, [r1]
   49d2c: e5890058     	str	r0, [r9, #0x58]
   49d30: 25893058     	strhs	r3, [r9, #0x58]
   49d34: e2849b49     	add	r9, r4, #74752
   49d38: e59d3004     	ldr	r3, [sp, #0x4]
   49d3c: e2833001     	add	r3, r3, #1
   49d40: e1530002     	cmp	r3, r2
   49d44: 23a03000     	movhs	r3, #0
   49d48: edcb7a00     	vstr	s15, [r11]
   49d4c: ed99ca0b     	vldr	s24, [r9, #44]
   49d50: ed990a0a     	vldr	s0, [r9, #40]
   49d54: e5863ed8     	str	r3, [r6, #0xed8]
   49d58: e2853ecf     	add	r3, r5, #3312
   49d5c: e2846a12     	add	r6, r4, #73728
   49d60: ee3cca00     	vadd.f32	s24, s24, s0
   49d64: edd3ca00     	vldr	s25, [r3]
   49d68: eeb00a4c     	vmov.f32	s0, s24
   49d6c: ee7dca2c     	vadd.f32	s25, s26, s25
   49d70: ebff318b     	bl	0x163a4    @ imm = #-0x339d4 ; floorf
   49d74: ee3cca40     	vsub.f32	s24, s24, s0
   49d78: ed5f6aac     	vldr	s13, [pc, #-688]        @ 0x49ad0 ; float 127
   49d7c: e2863e41     	add	r3, r6, #1040
   49d80: e5961410     	ldr	r1, [r6, #0x410]
   49d84: e5960414     	ldr	r0, [r6, #0x414]
   49d88: ee6c6a26     	vmul.f32	s13, s24, s13
   49d8c: ed937a04     	vldr	s14, [r3, #16]
   49d90: e5912004     	ldr	r2, [r1, #0x4]
   49d94: ed89ca0b     	vstr	s24, [r9, #44]
   49d98: e2423001     	sub	r3, r2, #1
   49d9c: ee053a90     	vmov	s11, r3
   49da0: eebc6ae6     	vcvt.u32.f32	s12, s13
   49da4: eef85a65     	vcvt.f32.u32	s11, s11
   49da8: ee163a10     	vmov	r3, s12
   49dac: eeb86a46     	vcvt.f32.u32	s12, s12
   49db0: e0844103     	add	r4, r4, r3, lsl #2
   49db4: ee766ac6     	vsub.f32	s13, s13, s12
   49db8: e2843b49     	add	r3, r4, #74752
   49dbc: edd37a0d     	vldr	s15, [r3, #52]
   49dc0: ed936a0e     	vldr	s12, [r3, #56]
   49dc4: ee366a67     	vsub.f32	s12, s12, s15
   49dc8: eee67a86     	vfma.f32	s15, s13, s12
   49dcc: ee062a90     	vmov	s13, r2
   49dd0: eef86a66     	vcvt.f32.u32	s13, s13
   49dd4: ee677a87     	vmul.f32	s15, s15, s14
   49dd8: ee070a10     	vmov	s14, r0
   49ddc: eeb87a47     	vcvt.f32.u32	s14, s14
   49de0: eef47ae5     	vcmpe.f32	s15, s11
   49de4: eef1fa10     	vmrs	APSR_nzcv, fpscr
   49de8: ce777ae5     	vsubgt.f32	s15, s15, s11
   49dec: ee767ae7     	vsub.f32	s15, s13, s15
   49df0: ee377a67     	vsub.f32	s14, s14, s15
   49df4: eefd7ac7     	vcvt.s32.f32	s15, s14
   49df8: ee173a90     	vmov	r3, s15
   49dfc: eef87ae7     	vcvt.f32.s32	s15, s15
   49e00: e253e001     	subs	lr, r3, #1
   49e04: ee377a67     	vsub.f32	s14, s14, s15
   49e08: 408ee002     	addmi	lr, lr, r2
   49e0c: 4a000001     	bmi	0x49e18
   49e10: e15e0002     	cmp	lr, r2
   49e14: a04ee002     	subge	lr, lr, r2
   49e18: e5911000     	ldr	r1, [r1]
   49e1c: e3530000     	cmp	r3, #0
   49e20: e283c001     	add	r12, r3, #1
   49e24: e081e10e     	add	lr, r1, lr, lsl #2
   49e28: ed9e4a00     	vldr	s8, [lr]
   49e2c: ba00000e     	blt	0x49e6c
   49e30: e1530002     	cmp	r3, r2
   49e34: a043e002     	subge	lr, r3, r2
   49e38: b1a0e003     	movlt	lr, r3
   49e3c: e081e10e     	add	lr, r1, lr, lsl #2
   49e40: e152000c     	cmp	r2, r12
   49e44: edde6a00     	vldr	s13, [lr]
   49e48: ca000058     	bgt	0x49fb0
   49e4c: e04cc002     	sub	r12, r12, r2
   49e50: ea00000b     	b	0x49e84
   49e54: e081c10c     	add	r12, r1, r12, lsl #2
   49e58: e2833002     	add	r3, r3, #2
   49e5c: eddc6a00     	vldr	s13, [r12]
   49e60: e1520003     	cmp	r2, r3
   49e64: d0433002     	suble	r3, r3, r2
   49e68: eaffff93     	b	0x49cbc
   49e6c: e083e002     	add	lr, r3, r2
   49e70: e35c0000     	cmp	r12, #0
   49e74: b08cc002     	addlt	r12, r12, r2
   49e78: e081e10e     	add	lr, r1, lr, lsl #2
   49e7c: edde6a00     	vldr	s13, [lr]
   49e80: aa0000fb     	bge	0x4a274
   49e84: e081c10c     	add	r12, r1, r12, lsl #2
   49e88: e2933002     	adds	r3, r3, #2
   49e8c: 40833002     	addmi	r3, r3, r2
   49e90: eddc7a00     	vldr	s15, [r12]
   49e94: 5a000048     	bpl	0x49fbc
   49e98: e0813103     	add	r3, r1, r3, lsl #2
   49e9c: eef05a00     	vmov.f32	s11, #2.000000e+00
   49ea0: ee777ae6     	vsub.f32	s15, s15, s13
   49ea4: eef03a08     	vmov.f32	s7, #3.000000e+00
   49ea8: eef74a00     	vmov.f32	s9, #1.000000e+00
   49eac: ed1f5af8     	vldr	s10, [pc, #-992]        @ 0x49ad4 ; float 0.166666701436
   49eb0: ed936a00     	vldr	s12, [r3]
   49eb4: e2863e41     	add	r3, r6, #1040
   49eb8: ee744ac7     	vsub.f32	s9, s9, s14
   49ebc: e0811100     	add	r1, r1, r0, lsl #2
   49ec0: e2800001     	add	r0, r0, #1
   49ec4: e59d2020     	ldr	r2, [sp, #0x20]
   49ec8: eeb03a46     	vmov.f32	s6, s12
   49ecc: ee366a44     	vsub.f32	s12, s12, s8
   49ed0: eea43a25     	vfma.f32	s6, s8, s11
   49ed4: eea76ae3     	vfms.f32	s12, s15, s7
   49ed8: ee255a64     	vnmul.f32	s10, s10, s9
   49edc: ed934a03     	vldr	s8, [r3, #12]
   49ee0: e5963418     	ldr	r3, [r6, #0x418]
   49ee4: e1500003     	cmp	r0, r3
   49ee8: 23a03000     	movhs	r3, #0
   49eec: eef05a43     	vmov.f32	s11, s6
   49ef0: eee65ae3     	vfms.f32	s11, s13, s7
   49ef4: eee75a06     	vfma.f32	s11, s14, s12
   49ef8: eee57a25     	vfma.f32	s15, s10, s11
   49efc: ed9f5adf     	vldr	s10, [pc, #892]         @ 0x4a280 ; float 9.42477798462
   49f00: eef75a00     	vmov.f32	s11, #1.000000e+00
   49f04: eee76a27     	vfma.f32	s13, s14, s15
   49f08: ee787a0b     	vadd.f32	s15, s16, s22
   49f0c: ed9f7adc     	vldr	s14, [pc, #880]         @ 0x4a284 ; float 28.2743339539
   49f10: ee777aea     	vsub.f32	s15, s15, s21
   49f14: eee4ca26     	vfma.f32	s25, s8, s13
   49f18: ee777a8a     	vadd.f32	s15, s15, s20
   49f1c: ee777ae9     	vsub.f32	s15, s15, s19
   49f20: eee46a6c     	vfms.f32	s13, s8, s25
   49f24: edc1ca00     	vstr	s25, [r1]
   49f28: e5860414     	str	r0, [r6, #0x414]
   49f2c: ee777ac9     	vsub.f32	s15, s15, s18
   49f30: 25863414     	strhs	r3, [r6, #0x414]
   49f34: e59d3010     	ldr	r3, [sp, #0x10]
   49f38: e2833001     	add	r3, r3, #1
   49f3c: ee777ae8     	vsub.f32	s15, s15, s17
   49f40: e1530002     	cmp	r3, r2
   49f44: 23a03000     	movhs	r3, #0
   49f48: edc86a00     	vstr	s13, [r8]
   49f4c: e58738a8     	str	r3, [r7, #0x8a8]
   49f50: e2853ece     	add	r3, r5, #3296
   49f54: ed936a03     	vldr	s12, [r3, #12]
   49f58: edd36a02     	vldr	s13, [r3, #8]
   49f5c: ee677a86     	vmul.f32	s15, s15, s12
   49f60: eee67aab     	vfma.f32	s15, s13, s23
   49f64: ee676aa7     	vmul.f32	s13, s15, s15
   49f68: ee366a87     	vadd.f32	s12, s13, s14
   49f6c: eea67a85     	vfma.f32	s14, s13, s10
   49f70: ee667a27     	vmul.f32	s15, s12, s15
   49f74: eef06a47     	vmov.f32	s13, s14
   49f78: ee877aa6     	vdiv.f32	s14, s15, s13
   49f7c: eeb47ae5     	vcmpe.f32	s14, s11
   49f80: eef1fa10     	vmrs	APSR_nzcv, fpscr
   49f84: 5eb07a65     	vmovpl.f32	s14, s11
   49f88: 5a000003     	bpl	0x49f9c
   49f8c: eeff7a00     	vmov.f32	s15, #-1.000000e+00
   49f90: eeb47ae7     	vcmpe.f32	s14, s15
   49f94: eef1fa10     	vmrs	APSR_nzcv, fpscr
   49f98: deb07a67     	vmovle.f32	s14, s15
   49f9c: e59d3044     	ldr	r3, [sp, #0x44]
   49fa0: ed837a00     	vstr	s14, [r3]
   49fa4: e28dd04c     	add	sp, sp, #76
   49fa8: ecbd8b0c     	vpop	{d8, d9, d10, d11, d12, d13}
   49fac: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
   49fb0: e081c10c     	add	r12, r1, r12, lsl #2
   49fb4: e2833002     	add	r3, r3, #2
   49fb8: eddc7a00     	vldr	s15, [r12]
   49fbc: e1520003     	cmp	r2, r3
   49fc0: d0433002     	suble	r3, r3, r2
   49fc4: eaffffb3     	b	0x49e98
   49fc8: e2873e8b     	add	r3, r7, #2224
   49fcc: eef75a00     	vmov.f32	s11, #1.000000e+00
   49fd0: ed936a01     	vldr	s12, [r3, #4]
   49fd4: edd36a02     	vldr	s13, [r3, #8]
   49fd8: edd34a03     	vldr	s9, [r3, #12]
   49fdc: ee355ac6     	vsub.f32	s10, s11, s12
   49fe0: ee366a25     	vadd.f32	s12, s12, s11
   49fe4: ee776aa6     	vadd.f32	s13, s15, s13
   49fe8: edc37a02     	vstr	s15, [r3, #8]
   49fec: eee46ac5     	vfms.f32	s13, s9, s10
   49ff0: eec67a86     	vdiv.f32	s15, s13, s12
   49ff4: edc37a03     	vstr	s15, [r3, #12]
   49ff8: eafffeb6     	b	0x49ad8
   49ffc: e2873e8b     	add	r3, r7, #2224
   4a000: eef75a00     	vmov.f32	s11, #1.000000e+00
   4a004: ed936a01     	vldr	s12, [r3, #4]
   4a008: edd36a02     	vldr	s13, [r3, #8]
   4a00c: edd34a03     	vldr	s9, [r3, #12]
   4a010: ee355ac6     	vsub.f32	s10, s11, s12
   4a014: ee366a25     	vadd.f32	s12, s12, s11
   4a018: ee776aa6     	vadd.f32	s13, s15, s13
   4a01c: edc37a02     	vstr	s15, [r3, #8]
   4a020: eee46ac5     	vfms.f32	s13, s9, s10
   4a024: eec65a86     	vdiv.f32	s11, s13, s12
   4a028: ee777ae5     	vsub.f32	s15, s15, s11
   4a02c: edc35a03     	vstr	s11, [r3, #12]
   4a030: ee757ae7     	vsub.f32	s15, s11, s15
   4a034: eafffea7     	b	0x49ad8
   4a038: e2873e8b     	add	r3, r7, #2224
   4a03c: eef75a00     	vmov.f32	s11, #1.000000e+00
   4a040: ed936a01     	vldr	s12, [r3, #4]
   4a044: edd36a02     	vldr	s13, [r3, #8]
   4a048: edd34a03     	vldr	s9, [r3, #12]
   4a04c: ee355ac6     	vsub.f32	s10, s11, s12
   4a050: ee366a25     	vadd.f32	s12, s12, s11
   4a054: ee776aa6     	vadd.f32	s13, s15, s13
   4a058: edc37a02     	vstr	s15, [r3, #8]
   4a05c: eee46ac5     	vfms.f32	s13, s9, s10
   4a060: eec65a86     	vdiv.f32	s11, s13, s12
   4a064: ee777ae5     	vsub.f32	s15, s15, s11
   4a068: edc35a03     	vstr	s11, [r3, #12]
   4a06c: eafffe99     	b	0x49ad8
   4a070: e2863eee     	add	r3, r6, #3808
   4a074: eeb76a00     	vmov.f32	s12, #1.000000e+00
   4a078: edd36a01     	vldr	s13, [r3, #4]
   4a07c: edd37a02     	vldr	s15, [r3, #8]
   4a080: ed935a03     	vldr	s10, [r3, #12]
   4a084: ee765a66     	vsub.f32	s11, s12, s13
   4a088: ee766a86     	vadd.f32	s13, s13, s12
   4a08c: ee777a27     	vadd.f32	s15, s14, s15
   4a090: ed837a02     	vstr	s14, [r3, #8]
   4a094: eee57a65     	vfms.f32	s15, s10, s11
   4a098: ee877aa6     	vdiv.f32	s14, s15, s13
   4a09c: ed837a03     	vstr	s14, [r3, #12]
   4a0a0: eafffe7b     	b	0x49a94
   4a0a4: e2863eee     	add	r3, r6, #3808
   4a0a8: eeb76a00     	vmov.f32	s12, #1.000000e+00
   4a0ac: edd36a01     	vldr	s13, [r3, #4]
   4a0b0: edd37a02     	vldr	s15, [r3, #8]
   4a0b4: ed935a03     	vldr	s10, [r3, #12]
   4a0b8: ee765a66     	vsub.f32	s11, s12, s13
   4a0bc: ee766a86     	vadd.f32	s13, s13, s12
   4a0c0: ee777a27     	vadd.f32	s15, s14, s15
   4a0c4: ed837a02     	vstr	s14, [r3, #8]
   4a0c8: eee57a65     	vfms.f32	s15, s10, s11
   4a0cc: ee876aa6     	vdiv.f32	s12, s15, s13
   4a0d0: ee377a46     	vsub.f32	s14, s14, s12
   4a0d4: ed836a03     	vstr	s12, [r3, #12]
   4a0d8: ee367a47     	vsub.f32	s14, s12, s14
   4a0dc: eafffe6c     	b	0x49a94
   4a0e0: e2863eee     	add	r3, r6, #3808
   4a0e4: eeb76a00     	vmov.f32	s12, #1.000000e+00
   4a0e8: edd36a01     	vldr	s13, [r3, #4]
   4a0ec: edd37a02     	vldr	s15, [r3, #8]
   4a0f0: ed935a03     	vldr	s10, [r3, #12]
   4a0f4: ee765a66     	vsub.f32	s11, s12, s13
   4a0f8: ee766a86     	vadd.f32	s13, s13, s12
   4a0fc: ee777a27     	vadd.f32	s15, s14, s15
   4a100: ed837a02     	vstr	s14, [r3, #8]
   4a104: eee57a65     	vfms.f32	s15, s10, s11
   4a108: ee876aa6     	vdiv.f32	s12, s15, s13
   4a10c: ee377a46     	vsub.f32	s14, s14, s12
   4a110: ed836a03     	vstr	s12, [r3, #12]
   4a114: eafffe5e     	b	0x49a94
   4a118: e2843ef6     	add	r3, r4, #3936
   4a11c: eeb76a00     	vmov.f32	s12, #1.000000e+00
   4a120: edd36a02     	vldr	s13, [r3, #8]
   4a124: edd37a03     	vldr	s15, [r3, #12]
   4a128: ed935a04     	vldr	s10, [r3, #16]
   4a12c: ee765a66     	vsub.f32	s11, s12, s13
   4a130: ee766a86     	vadd.f32	s13, s13, s12
   4a134: ee777a27     	vadd.f32	s15, s14, s15
   4a138: ed837a03     	vstr	s14, [r3, #12]
   4a13c: eee57a65     	vfms.f32	s15, s10, s11
   4a140: ee877aa6     	vdiv.f32	s14, s15, s13
   4a144: ed837a04     	vstr	s14, [r3, #16]
   4a148: eafffdf0     	b	0x49910
   4a14c: e2843ef6     	add	r3, r4, #3936
   4a150: eeb76a00     	vmov.f32	s12, #1.000000e+00
   4a154: edd36a02     	vldr	s13, [r3, #8]
   4a158: edd37a03     	vldr	s15, [r3, #12]
   4a15c: ed935a04     	vldr	s10, [r3, #16]
   4a160: ee765a66     	vsub.f32	s11, s12, s13
   4a164: ee766a86     	vadd.f32	s13, s13, s12
   4a168: ee777a27     	vadd.f32	s15, s14, s15
   4a16c: ed837a03     	vstr	s14, [r3, #12]
   4a170: eee57a65     	vfms.f32	s15, s10, s11
   4a174: ee876aa6     	vdiv.f32	s12, s15, s13
   4a178: ee377a46     	vsub.f32	s14, s14, s12
   4a17c: ed836a04     	vstr	s12, [r3, #16]
   4a180: ee367a47     	vsub.f32	s14, s12, s14
   4a184: eafffde1     	b	0x49910
   4a188: e2843ef6     	add	r3, r4, #3936
   4a18c: eeb76a00     	vmov.f32	s12, #1.000000e+00
   4a190: edd36a02     	vldr	s13, [r3, #8]
   4a194: edd37a03     	vldr	s15, [r3, #12]
   4a198: ed935a04     	vldr	s10, [r3, #16]
   4a19c: ee765a66     	vsub.f32	s11, s12, s13
   4a1a0: ee766a86     	vadd.f32	s13, s13, s12
   4a1a4: ee777a27     	vadd.f32	s15, s14, s15
   4a1a8: ed837a03     	vstr	s14, [r3, #12]
   4a1ac: eee57a65     	vfms.f32	s15, s10, s11
   4a1b0: ee876aa6     	vdiv.f32	s12, s15, s13
   4a1b4: ee377a46     	vsub.f32	s14, s14, s12
   4a1b8: ed836a04     	vstr	s12, [r3, #16]
   4a1bc: eafffdd3     	b	0x49910
   4a1c0: e2803ef7     	add	r3, r0, #3952
   4a1c4: eef76a00     	vmov.f32	s13, #1.000000e+00
   4a1c8: ed937a02     	vldr	s14, [r3, #8]
   4a1cc: edd37a03     	vldr	s15, [r3, #12]
   4a1d0: edd35a04     	vldr	s11, [r3, #16]
   4a1d4: ee366ac7     	vsub.f32	s12, s13, s14
   4a1d8: ee377a26     	vadd.f32	s14, s14, s13
   4a1dc: ee7b7aa7     	vadd.f32	s15, s23, s15
   4a1e0: edc3ba03     	vstr	s23, [r3, #12]
   4a1e4: eee57ac6     	vfms.f32	s15, s11, s12
   4a1e8: eec7ba87     	vdiv.f32	s23, s15, s14
   4a1ec: edc3ba04     	vstr	s23, [r3, #16]
   4a1f0: eafffd3f     	b	0x496f4
   4a1f4: e2803ef7     	add	r3, r0, #3952
   4a1f8: eef76a00     	vmov.f32	s13, #1.000000e+00
   4a1fc: ed937a02     	vldr	s14, [r3, #8]
   4a200: edd37a03     	vldr	s15, [r3, #12]
   4a204: edd35a04     	vldr	s11, [r3, #16]
   4a208: ee366ac7     	vsub.f32	s12, s13, s14
   4a20c: ee377a26     	vadd.f32	s14, s14, s13
   4a210: ee7b7aa7     	vadd.f32	s15, s23, s15
   4a214: edc3ba03     	vstr	s23, [r3, #12]
   4a218: eee57ac6     	vfms.f32	s15, s11, s12
   4a21c: eec76a87     	vdiv.f32	s13, s15, s14
   4a220: ee7bbae6     	vsub.f32	s23, s23, s13
   4a224: edc36a04     	vstr	s13, [r3, #16]
   4a228: ee76baeb     	vsub.f32	s23, s13, s23
   4a22c: eafffd30     	b	0x496f4
   4a230: e2803ef7     	add	r3, r0, #3952
   4a234: eef76a00     	vmov.f32	s13, #1.000000e+00
   4a238: ed937a02     	vldr	s14, [r3, #8]
   4a23c: edd37a03     	vldr	s15, [r3, #12]
   4a240: edd35a04     	vldr	s11, [r3, #16]
   4a244: ee366ac7     	vsub.f32	s12, s13, s14
   4a248: ee377a26     	vadd.f32	s14, s14, s13
   4a24c: ee7b7aa7     	vadd.f32	s15, s23, s15
   4a250: edc3ba03     	vstr	s23, [r3, #12]
   4a254: eee57ac6     	vfms.f32	s15, s11, s12
   4a258: eec76a87     	vdiv.f32	s13, s15, s14
   4a25c: ee7bbae6     	vsub.f32	s23, s23, s13
   4a260: edc36a04     	vstr	s13, [r3, #16]
   4a264: eafffd22     	b	0x496f4
   4a268: e152000c     	cmp	r2, r12
   4a26c: dafffe85     	ble	0x49c88
   4a270: eafffe8c     	b	0x49ca8
   4a274: e152000c     	cmp	r2, r12
   4a278: dafffef3     	ble	0x49e4c
   4a27c: eaffff00     	b	0x49e84
   4a280: e4 cb 16 41  	.word	0x4116cbe4
   4a284: d6 31 e2 41  	.word	0x41e231d6
