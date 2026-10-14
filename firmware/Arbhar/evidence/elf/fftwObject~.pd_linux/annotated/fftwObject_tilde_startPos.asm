00000e3c <fftwObject_tilde_startPos>:
     e3c: eddf6af7     	vldr	s13, [pc, #988]         @ 0x1220 <fftwObject_tilde_startPos+0x3e4>  // f32=0
     e40: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
     e44: e2806866     	add	r6, r0, #6684672
     e48: eeb77a00     	vmov.f32	s14, #1.000000e+00
     e4c: e2864ecf     	add	r4, r6, #3312
     e50: e5961d14     	ldr	r1, [r6, #0xd14]
     e54: e2863ece     	add	r3, r6, #3296
     e58: edd41a00     	vldr	s3, [r4]
     e5c: e1a09000     	mov	r9, r0
     e60: e2410001     	sub	r0, r1, #1
     e64: e2865a01     	add	r5, r6, #4096
     e68: ee071a90     	vmov	s15, r1
     e6c: e595c03c     	ldr	r12, [r5, #0x3c]
     e70: ed2d8b08     	vpush	{d8, d9, d10, d11}
     e74: e5957040     	ldr	r7, [r5, #0x40]
     e78: ed959a12     	vldr	s18, [r5, #72]
     e7c: e24dd00c     	sub	sp, sp, #12
     e80: eeb40ae6     	vcmpe.f32	s0, s13
     e84: ed958a13     	vldr	s16, [r5, #76]
     e88: eef1fa10     	vmrs	APSR_nzcv, fpscr
     e8c: beb00a66     	vmovlt.f32	s0, s13
     e90: eeb40ac7     	vcmpe.f32	s0, s14
     e94: eef80ae7     	vcvt.f32.s32	s1, s15
     e98: eef1fa10     	vmrs	APSR_nzcv, fpscr
     e9c: 8eb00a47     	vmovhi.f32	s0, s14
     ea0: ee201a80     	vmul.f32	s2, s1, s0
     ea4: ed830a03     	vstr	s0, [r3, #12]
     ea8: ee202aa1     	vmul.f32	s4, s1, s3
     eac: eebd0ac1     	vcvt.s32.f32	s0, s2
     eb0: eefd2ac2     	vcvt.s32.f32	s5, s4
     eb4: ee102a10     	vmov	r2, s0
     eb8: ee128a90     	vmov	r8, s5
     ebc: e1520000     	cmp	r2, r0
     ec0: a2412002     	subge	r2, r1, #2
     ec4: e5862d00     	str	r2, [r6, #0xd00]
     ec8: e3580002     	cmp	r8, #2
     ecc: b3a08002     	movlt	r8, #2
     ed0: e082b008     	add	r11, r2, r8
     ed4: e15b0001     	cmp	r11, r1
     ed8: a1a0b001     	movge	r11, r1
     edc: e04b4002     	sub	r4, r11, r2
     ee0: e1540008     	cmp	r4, r8
     ee4: e586bd04     	str	r11, [r6, #0xd04]
     ee8: a1a04008     	movge	r4, r8
     eec: e154000c     	cmp	r4, r12
     ef0: e5864d10     	str	r4, [r6, #0xd10]
     ef4: e084e584     	add	lr, r4, r4, lsl #11
     ef8: b1a0a004     	movlt	r10, r4
     efc: a1a0a00c     	movge	r10, r12
     f00: e1540007     	cmp	r4, r7
     f04: e586ed18     	str	lr, [r6, #0xd18]
     f08: e585a03c     	str	r10, [r5, #0x3c]
     f0c: b1a08004     	movlt	r8, r4
     f10: a1a08007     	movge	r8, r7
     f14: e35a0000     	cmp	r10, #0
     f18: e0443008     	sub	r3, r4, r8
     f1c: e5858040     	str	r8, [r5, #0x40]
     f20: e58d3004     	str	r3, [sp, #0x4]
     f24: da00010a     	ble	0x1354 <fftwObject_tilde_startPos+0x518> @ imm = #0x428
     f28: eeb73a00     	vmov.f32	s6, #1.000000e+00
     f2c: e21a1003     	ands	r1, r10, #3
     f30: ee03ca90     	vmov	s7, r12
     f34: e3000d1c     	movw	r0, #0xd1c
     f38: e3400066     	movt	r0, #0x66
     f3c: e3a05000     	mov	r5, #0
     f40: e089b000     	add	r11, r9, r0
     f44: eeb84ae3     	vcvt.f32.s32	s8, s7
     f48: eeb7bac9     	vcvt.f64.f32	d11, s18
     f4c: eec38a04     	vdiv.f32	s17, s6, s8
     f50: 0a00001d     	beq	0xfcc <fftwObject_tilde_startPos+0x190> @ imm = #0x74
     f54: e3510001     	cmp	r1, #1
     f58: 0a000010     	beq	0xfa0 <fftwObject_tilde_startPos+0x164> @ imm = #0x40
     f5c: e3510002     	cmp	r1, #2
     f60: 0a000005     	beq	0xf7c <fftwObject_tilde_startPos+0x140> @ imm = #0x14
     f64: ed9f0bab     	vldr	d0, [pc, #684]          @ 0x1218 <fftwObject_tilde_startPos+0x3dc>  // f64=0
     f68: e3a05001     	mov	r5, #1
     f6c: eeb01b4b     	vmov.f64	d1, d11
     f70: ebfffde3     	bl	0x704 <.plt+0xec>       @ imm = #-0x874  // CALL __pow_finite
     f74: eef74bc0     	vcvt.f32.f64	s9, d0
     f78: eceb4a01     	vstmia	r11!, {s9}
     f7c: ee055a10     	vmov	s10, r5
     f80: e2855001     	add	r5, r5, #1
     f84: eeb01b4b     	vmov.f64	d1, d11
     f88: eef85ac5     	vcvt.f32.s32	s11, s10
     f8c: ee256aa8     	vmul.f32	s12, s11, s17
     f90: eeb70ac6     	vcvt.f64.f32	d0, s12
     f94: ebfffdda     	bl	0x704 <.plt+0xec>       @ imm = #-0x898  // CALL __pow_finite
     f98: eef79bc0     	vcvt.f32.f64	s19, d0
     f9c: eceb9a01     	vstmia	r11!, {s19}
     fa0: ee0a5a10     	vmov	s20, r5
     fa4: e2855001     	add	r5, r5, #1
     fa8: eeb01b4b     	vmov.f64	d1, d11
     fac: eef8aaca     	vcvt.f32.s32	s21, s20
     fb0: ee2a7aa8     	vmul.f32	s14, s21, s17
     fb4: eeb70ac7     	vcvt.f64.f32	d0, s14
     fb8: ebfffdd1     	bl	0x704 <.plt+0xec>       @ imm = #-0x8bc  // CALL __pow_finite
     fbc: e15a0005     	cmp	r10, r5
     fc0: eef76bc0     	vcvt.f32.f64	s13, d0
     fc4: eceb6a01     	vstmia	r11!, {s13}
     fc8: 0a000028     	beq	0x1070 <fftwObject_tilde_startPos+0x234> @ imm = #0xa0
     fcc: e2852001     	add	r2, r5, #1
     fd0: e1a0800b     	mov	r8, r11
     fd4: e28bb010     	add	r11, r11, #16
     fd8: ee075a90     	vmov	s15, r5
     fdc: eeb01b4b     	vmov.f64	d1, d11
     fe0: ee092a10     	vmov	s18, r2
     fe4: eef80ae7     	vcvt.f32.s32	s1, s15
     fe8: ee200aa8     	vmul.f32	s0, s1, s17
     fec: eeb70ac0     	vcvt.f64.f32	d0, s0
     ff0: ebfffdc3     	bl	0x704 <.plt+0xec>       @ imm = #-0x8f4  // CALL __pow_finite
     ff4: eeb82ac9     	vcvt.f32.s32	s4, s18
     ff8: ee622a28     	vmul.f32	s5, s4, s17
     ffc: eeb01b4b     	vmov.f64	d1, d11
    1000: eeb73bc0     	vcvt.f32.f64	s6, d0
    1004: eeb70ae2     	vcvt.f64.f32	d0, s5
    1008: eca83a01     	vstmia	r8!, {s6}
    100c: ebfffdbc     	bl	0x704 <.plt+0xec>       @ imm = #-0x910  // CALL __pow_finite
    1010: ee19ca10     	vmov	r12, s18
    1014: eeb01b4b     	vmov.f64	d1, d11
    1018: e28c3001     	add	r3, r12, #1
    101c: ee033a90     	vmov	s7, r3
    1020: eeb84ae3     	vcvt.f32.s32	s8, s7
    1024: ee644a28     	vmul.f32	s9, s8, s17
    1028: eeb75bc0     	vcvt.f32.f64	s10, d0
    102c: eeb70ae4     	vcvt.f64.f32	d0, s9
    1030: ed0b5a03     	vstr	s10, [r11, #-12]
    1034: ebfffdb2     	bl	0x704 <.plt+0xec>       @ imm = #-0x938  // CALL __pow_finite
    1038: e2851003     	add	r1, r5, #3
    103c: e2855004     	add	r5, r5, #4
    1040: ee051a90     	vmov	s11, r1
    1044: eeb86ae5     	vcvt.f32.s32	s12, s11
    1048: ee669a28     	vmul.f32	s19, s12, s17
    104c: eeb01b4b     	vmov.f64	d1, d11
    1050: eeb7abc0     	vcvt.f32.f64	s20, d0
    1054: eeb70ae9     	vcvt.f64.f32	d0, s19
    1058: ed88aa01     	vstr	s20, [r8, #4]
    105c: ebfffda8     	bl	0x704 <.plt+0xec>       @ imm = #-0x960  // CALL __pow_finite
    1060: e15a0005     	cmp	r10, r5
    1064: eeb71bc0     	vcvt.f32.f64	s2, d0
    1068: ed0b1a01     	vstr	s2, [r11, #-4]
    106c: 1affffd6     	bne	0xfcc <fftwObject_tilde_startPos+0x190> @ imm = #-0xa8
    1070: e59de004     	ldr	lr, [sp, #0x4]
    1074: e15e000a     	cmp	lr, r10
    1078: da00002e     	ble	0x1138 <fftwObject_tilde_startPos+0x2fc> @ imm = #0xb8
    107c: e3085347     	movw	r5, #0x8347
    1080: e3405019     	movt	r5, #0x19
    1084: e08aa005     	add	r10, r10, r5
    1088: e3002d1c     	movw	r2, #0xd1c
    108c: e3402066     	movt	r2, #0x66
    1090: e3a085fe     	mov	r8, #1065353216
    1094: e089c002     	add	r12, r9, r2
    1098: e089310a     	add	r3, r9, r10, lsl #2
    109c: e08c110e     	add	r1, r12, lr, lsl #2
    10a0: e041b003     	sub	r11, r1, r3
    10a4: e24be004     	sub	lr, r11, #4
    10a8: e1a0012e     	lsr	r0, lr, #2
    10ac: e2805001     	add	r5, r0, #1
    10b0: e215a007     	ands	r10, r5, #7
    10b4: 0a000013     	beq	0x1108 <fftwObject_tilde_startPos+0x2cc> @ imm = #0x4c
    10b8: e35a0001     	cmp	r10, #1
    10bc: 0a00000e     	beq	0x10fc <fftwObject_tilde_startPos+0x2c0> @ imm = #0x38
    10c0: e35a0002     	cmp	r10, #2
    10c4: 0a00000b     	beq	0x10f8 <fftwObject_tilde_startPos+0x2bc> @ imm = #0x2c
    10c8: e35a0003     	cmp	r10, #3
    10cc: 0a000008     	beq	0x10f4 <fftwObject_tilde_startPos+0x2b8> @ imm = #0x20
    10d0: e35a0004     	cmp	r10, #4
    10d4: 0a000005     	beq	0x10f0 <fftwObject_tilde_startPos+0x2b4> @ imm = #0x14
    10d8: e35a0005     	cmp	r10, #5
    10dc: 0a000002     	beq	0x10ec <fftwObject_tilde_startPos+0x2b0> @ imm = #0x8
    10e0: e35a0006     	cmp	r10, #6
    10e4: 1a000098     	bne	0x134c <fftwObject_tilde_startPos+0x510> @ imm = #0x260
    10e8: e4838004     	str	r8, [r3], #4
    10ec: e4838004     	str	r8, [r3], #4
    10f0: e4838004     	str	r8, [r3], #4
    10f4: e4838004     	str	r8, [r3], #4
    10f8: e4838004     	str	r8, [r3], #4
    10fc: e4838004     	str	r8, [r3], #4
    1100: e1510003     	cmp	r1, r3
    1104: 0a00000b     	beq	0x1138 <fftwObject_tilde_startPos+0x2fc> @ imm = #0x2c
    1108: e1a02003     	mov	r2, r3
    110c: e2833020     	add	r3, r3, #32
    1110: e4828004     	str	r8, [r2], #4
    1114: e503801c     	str	r8, [r3, #-0x1c]
    1118: e5828004     	str	r8, [r2, #0x4]
    111c: e5038014     	str	r8, [r3, #-0x14]
    1120: e5038010     	str	r8, [r3, #-0x10]
    1124: e503800c     	str	r8, [r3, #-0xc]
    1128: e5038008     	str	r8, [r3, #-0x8]
    112c: e5038004     	str	r8, [r3, #-0x4]
    1130: e1510003     	cmp	r1, r3
    1134: 1afffff3     	bne	0x1108 <fftwObject_tilde_startPos+0x2cc> @ imm = #-0x34
    1138: e3570000     	cmp	r7, #0
    113c: da00006b     	ble	0x12f0 <fftwObject_tilde_startPos+0x4b4> @ imm = #0x1ac
    1140: eef7ba00     	vmov.f32	s23, #1.000000e+00
    1144: e59d1004     	ldr	r1, [sp, #0x4]
    1148: ee017a90     	vmov	s3, r7
    114c: e3088347     	movw	r8, #0x8347
    1150: e3408019     	movt	r8, #0x19
    1154: e217c003     	ands	r12, r7, #3
    1158: e081b008     	add	r11, r1, r8
    115c: e3a08000     	mov	r8, #0
    1160: e089510b     	add	r5, r9, r11, lsl #2
    1164: eef88ae1     	vcvt.f32.s32	s17, s3
    1168: eef80be1     	vcvt.f64.s32	d16, s3
    116c: ee8bbaa8     	vdiv.f32	s22, s23, s17
    1170: ee809ba0     	vdiv.f64	d9, d16, d16
    1174: eeb78ac8     	vcvt.f64.f32	d8, s16
    1178: eeb7ab00     	vmov.f64	d10, #1.000000e+00
    117c: 0a00002b     	beq	0x1230 <fftwObject_tilde_startPos+0x3f4> @ imm = #0xac
    1180: e35c0001     	cmp	r12, #1
    1184: 0a000014     	beq	0x11dc <fftwObject_tilde_startPos+0x3a0> @ imm = #0x50
    1188: e35c0002     	cmp	r12, #2
    118c: 0a000007     	beq	0x11b0 <fftwObject_tilde_startPos+0x374> @ imm = #0x1c
    1190: ed9f0b20     	vldr	d0, [pc, #128]          @ 0x1218 <fftwObject_tilde_startPos+0x3dc>  // f64=0
    1194: e3a08001     	mov	r8, #1
    1198: eeb01b48     	vmov.f64	d1, d8
    119c: ebfffd58     	bl	0x704 <.plt+0xec>       @ imm = #-0xaa0  // CALL __pow_finite
    11a0: ee7a1b40     	vsub.f64	d17, d10, d0
    11a4: ee290b21     	vmul.f64	d0, d9, d17
    11a8: eeb77bc0     	vcvt.f32.f64	s14, d0
    11ac: eca57a01     	vstmia	r5!, {s14}
    11b0: ee068a90     	vmov	s13, r8
    11b4: e2888001     	add	r8, r8, #1
    11b8: eeb01b48     	vmov.f64	d1, d8
    11bc: eef87ae6     	vcvt.f32.s32	s15, s13
    11c0: ee670a8b     	vmul.f32	s1, s15, s22
    11c4: eeb70ae0     	vcvt.f64.f32	d0, s1
    11c8: ebfffd4d     	bl	0x704 <.plt+0xec>       @ imm = #-0xacc  // CALL __pow_finite
    11cc: ee7a2b40     	vsub.f64	d18, d10, d0
    11d0: ee290b22     	vmul.f64	d0, d9, d18
    11d4: eeb70bc0     	vcvt.f32.f64	s0, d0
    11d8: eca50a01     	vstmia	r5!, {s0}
    11dc: ee028a10     	vmov	s4, r8
    11e0: e2888001     	add	r8, r8, #1
    11e4: eeb01b48     	vmov.f64	d1, d8
    11e8: eef82ac2     	vcvt.f32.s32	s5, s4
    11ec: ee223a8b     	vmul.f32	s6, s5, s22
    11f0: eeb70ac3     	vcvt.f64.f32	d0, s6
    11f4: ebfffd42     	bl	0x704 <.plt+0xec>       @ imm = #-0xaf8  // CALL __pow_finite
    11f8: e1570008     	cmp	r7, r8
    11fc: ee7a3b40     	vsub.f64	d19, d10, d0
    1200: ee290b23     	vmul.f64	d0, d9, d19
    1204: eef73bc0     	vcvt.f32.f64	s7, d0
    1208: ece53a01     	vstmia	r5!, {s7}
    120c: 0a000037     	beq	0x12f0 <fftwObject_tilde_startPos+0x4b4> @ imm = #0xdc
    1210: ea000006     	b	0x1230 <fftwObject_tilde_startPos+0x3f4> @ imm = #0x18
    1214: e320f000     	nop
    1218: 00 00 00 00  	.word	0x00000000
    121c: 00 00 00 00  	.word	0x00000000
    1220: 00 00 00 00  	.word	0x00000000
    1224: 34 2f 00 00  	.word	0x00002f34
    1228: 24 2f 00 00  	.word	0x00002f24
    122c: 18 2f 00 00  	.word	0x00002f18
    1230: e2889001     	add	r9, r8, #1
    1234: e1a0a005     	mov	r10, r5
    1238: e2855010     	add	r5, r5, #16
    123c: ee048a10     	vmov	s8, r8
    1240: eeb01b48     	vmov.f64	d1, d8
    1244: eef84ac4     	vcvt.f32.s32	s9, s8
    1248: ee245a8b     	vmul.f32	s10, s9, s22
    124c: eeb70ac5     	vcvt.f64.f32	d0, s10
    1250: ebfffd2b     	bl	0x704 <.plt+0xec>       @ imm = #-0xb54  // CALL __pow_finite
    1254: ee059a90     	vmov	s11, r9
    1258: eeb86ae5     	vcvt.f32.s32	s12, s11
    125c: ee66ba0b     	vmul.f32	s23, s12, s22
    1260: eeb01b48     	vmov.f64	d1, d8
    1264: ee7a4b40     	vsub.f64	d20, d10, d0
    1268: ee695b24     	vmul.f64	d21, d9, d20
    126c: eeb70aeb     	vcvt.f64.f32	d0, s23
    1270: eeb77be5     	vcvt.f32.f64	s14, d21
    1274: ecaa7a01     	vstmia	r10!, {s14}
    1278: ebfffd21     	bl	0x704 <.plt+0xec>       @ imm = #-0xb7c  // CALL __pow_finite
    127c: e2880002     	add	r0, r8, #2
    1280: ee010a10     	vmov	s2, r0
    1284: eef86ac1     	vcvt.f32.s32	s13, s2
    1288: ee667a8b     	vmul.f32	s15, s13, s22
    128c: eeb01b48     	vmov.f64	d1, d8
    1290: ee7a6b40     	vsub.f64	d22, d10, d0
    1294: ee697b26     	vmul.f64	d23, d9, d22
    1298: eeb70ae7     	vcvt.f64.f32	d0, s15
    129c: eeb72be7     	vcvt.f32.f64	s4, d23
    12a0: ed052a03     	vstr	s4, [r5, #-12]
    12a4: ebfffd16     	bl	0x704 <.plt+0xec>       @ imm = #-0xba8  // CALL __pow_finite
    12a8: e2882003     	add	r2, r8, #3
    12ac: e2888004     	add	r8, r8, #4
    12b0: ee012a90     	vmov	s3, r2
    12b4: eef82ae1     	vcvt.f32.s32	s5, s3
    12b8: ee223a8b     	vmul.f32	s6, s5, s22
    12bc: eeb01b48     	vmov.f64	d1, d8
    12c0: ee7a8b40     	vsub.f64	d24, d10, d0
    12c4: ee699b28     	vmul.f64	d25, d9, d24
    12c8: eeb70ac3     	vcvt.f64.f32	d0, s6
    12cc: eef73be9     	vcvt.f32.f64	s7, d25
    12d0: edca3a01     	vstr	s7, [r10, #4]
    12d4: ebfffd0a     	bl	0x704 <.plt+0xec>       @ imm = #-0xbd8  // CALL __pow_finite
    12d8: e1570008     	cmp	r7, r8
    12dc: ee7aab40     	vsub.f64	d26, d10, d0
    12e0: ee290b2a     	vmul.f64	d0, d9, d26
    12e4: eef70bc0     	vcvt.f32.f64	s1, d0
    12e8: ed450a01     	vstr	s1, [r5, #-4]
    12ec: 1affffcf     	bne	0x1230 <fftwObject_tilde_startPos+0x3f4> @ imm = #-0xc4
    12f0: e51f70d4     	ldr	r7, [pc, #-0xd4]        @ 0x1224 <fftwObject_tilde_startPos+0x3e8>  // u32=0x2f34; f32?=1.69332906e-41
    12f4: ee094a90     	vmov	s19, r4
    12f8: eeb80ae9     	vcvt.f32.s32	s0, s19
    12fc: ebfffd09     	bl	0x728 <.plt+0x110>      @ imm = #-0xbdc  // CALL postfloat
    1300: e08f0007     	add	r0, pc, r7
    1304: ebfffcef     	bl	0x6c8 <.plt+0xb0>       @ imm = #-0xc44  // CALL post
    1308: e5964d00     	ldr	r4, [r6, #0xd00]
    130c: ee084a90     	vmov	s17, r4
    1310: eeb80ae8     	vcvt.f32.s32	s0, s17
    1314: ebfffd03     	bl	0x728 <.plt+0x110>      @ imm = #-0xbf4  // CALL postfloat
    1318: e51f30f8     	ldr	r3, [pc, #-0xf8]        @ 0x1228 <fftwObject_tilde_startPos+0x3ec>  // u32=0x2f24; f32?=1.69108699e-41
    131c: e08f0003     	add	r0, pc, r3
    1320: ebfffce8     	bl	0x6c8 <.plt+0xb0>       @ imm = #-0xc60  // CALL post
    1324: e5966d04     	ldr	r6, [r6, #0xd04]
    1328: ee0b6a10     	vmov	s22, r6
    132c: eeb80acb     	vcvt.f32.s32	s0, s22
    1330: ebfffcfc     	bl	0x728 <.plt+0x110>      @ imm = #-0xc10  // CALL postfloat
    1334: e51fc110     	ldr	r12, [pc, #-0x110]      @ 0x122c <fftwObject_tilde_startPos+0x3f0>  // u32=0x2f18; f32?=1.68940543e-41
    1338: e08f000c     	add	r0, pc, r12
    133c: e28dd00c     	add	sp, sp, #12
    1340: ecbd8b08     	vpop	{d8, d9, d10, d11}
    1344: e8bd4ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
    1348: eafffcde     	b	0x6c8 <.plt+0xb0>       @ imm = #-0xc88  // CALL post
    134c: e4838004     	str	r8, [r3], #4
    1350: eaffff64     	b	0x10e8 <fftwObject_tilde_startPos+0x2ac> @ imm = #-0x270
    1354: e3a0a000     	mov	r10, #0
    1358: eaffff44     	b	0x1070 <fftwObject_tilde_startPos+0x234> @ imm = #-0x2f0

