00006dd4 <led_menuPagesIndicator>:
    6dd4: e92d43f0     	push	{r4, r5, r6, r7, r8, r9, lr}
    6dd8: e1a04000     	mov	r4, r0
    6ddc: ed2d8b02     	vpush	{d8}
    6de0: eeb08a40     	vmov.f32	s16, s0
    6de4: e24dd07c     	sub	sp, sp, #124
    6de8: ed9f0ac5     	vldr	s0, [pc, #788]          @ 0x7104 <led_menuPagesIndicator+0x330>  // f32=123
    6dec: ebfff27d     	bl	0x37e8 <.plt+0xec>      @ imm = #-0x360c  // CALL readFromSharedMem
    6df0: eef77a00     	vmov.f32	s15, #1.000000e+00
    6df4: eeb40ae7     	vcmpe.f32	s0, s15
    6df8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    6dfc: 4a000002     	bmi	0x6e0c <led_menuPagesIndicator+0x38> @ imm = #0x8
    6e00: e28dd07c     	add	sp, sp, #124
    6e04: ecbd8b02     	vpop	{d8}
    6e08: e8bd83f0     	pop	{r4, r5, r6, r7, r8, r9, pc}
    6e0c: e59f32f4     	ldr	r3, [pc, #0x2f4]        @ 0x7108 <led_menuPagesIndicator+0x334>  // u32=0x20598; f32?=1.85677652e-40
    6e10: e2847a01     	add	r7, r4, #4096
    6e14: e5d4203c     	ldrb	r2, [r4, #0x3c]
    6e18: e08f0003     	add	r0, pc, r3
    6e1c: e5d75de0     	ldrb	r5, [r7, #0xde0]
    6e20: ed907a0c     	vldr	s14, [r0, #48]
    6e24: eeb80ac7     	vcvt.f32.s32	s0, s14
    6e28: eeb48a40     	vcmp.f32	s16, s0
    6e2c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    6e30: 13e05000     	mvnne	r5, #0
    6e34: e3520003     	cmp	r2, #3
    6e38: 9a00006d     	bls	0x6ff4 <led_menuPagesIndicator+0x220> @ imm = #0x1b4
    6e3c: e5d4606a     	ldrb	r6, [r4, #0x6a]
    6e40: e30a9aab     	movw	r9, #0xaaab
    6e44: e59f82c0     	ldr	r8, [pc, #0x2c0]        @ 0x710c <led_menuPagesIndicator+0x338>  // u32=0xe1b8; f32?=8.09726305e-41
    6e48: e34a9aaa     	movt	r9, #0xaaaa
    6e4c: e286e002     	add	lr, r6, #2
    6e50: edcd7a0f     	vstr	s15, [sp, #60]
    6e54: e08f1008     	add	r1, pc, r8
    6e58: e2868001     	add	r8, r6, #1
    6e5c: e241cd1f     	sub	r12, r1, #1984
    6e60: e0810899     	umull	r0, r1, r9, r8
    6e64: edcd7a17     	vstr	s15, [sp, #92]
    6e68: e24c2004     	sub	r2, r12, #4
    6e6c: e0830e99     	umull	r0, r3, r9, lr
    6e70: e59f9298     	ldr	r9, [pc, #0x298]        @ 0x7110 <led_menuPagesIndicator+0x33c>  // u32=0x2046c; f32?=1.85257262e-40
    6e74: e1a000a1     	lsr	r0, r1, #1
    6e78: e08fc009     	add	r12, pc, r9
    6e7c: e0801080     	add	r1, r0, r0, lsl #1
    6e80: e1a030a3     	lsr	r3, r3, #1
    6e84: e0488001     	sub	r8, r8, r1
    6e88: e8920007     	ldm	r2, {r0, r1, r2}
    6e8c: e0839083     	add	r9, r3, r3, lsl #1
    6e90: e04e3009     	sub	r3, lr, r9
    6e94: e28de078     	add	lr, sp, #120
    6e98: e08e8108     	add	r8, lr, r8, lsl #2
    6e9c: e08e9106     	add	r9, lr, r6, lsl #2
    6ea0: e08ee103     	add	lr, lr, r3, lsl #2
    6ea4: e28d300c     	add	r3, sp, #12
    6ea8: e8830007     	stm	r3, {r0, r1, r2}
    6eac: e3a03001     	mov	r3, #1
    6eb0: e59c2008     	ldr	r2, [r12, #0x8]
    6eb4: e519106c     	ldr	r1, [r9, #-0x6c]
    6eb8: e3a09443     	mov	r9, #1124073472
    6ebc: e51e006c     	ldr	r0, [lr, #-0x6c]
    6ec0: ed586a1b     	vldr	s13, [r8, #-108]
    6ec4: e1a0e312     	lsl	lr, r2, r3
    6ec8: e0418002     	sub	r8, r1, r2
    6ecc: e58d3018     	str	r3, [sp, #0x18]
    6ed0: e58d3020     	str	r3, [sp, #0x20]
    6ed4: e3a01000     	mov	r1, #0
    6ed8: ee058a90     	vmov	s11, r8
    6edc: e58d3028     	str	r3, [sp, #0x28]
    6ee0: ee080a90     	vmov	s17, r0
    6ee4: e58d3030     	str	r3, [sp, #0x30]
    6ee8: ee04ea90     	vmov	s9, lr
    6eec: e58d3038     	str	r3, [sp, #0x38]
    6ef0: eeb85ae6     	vcvt.f32.s32	s10, s13
    6ef4: e58d3040     	str	r3, [sp, #0x40]
    6ef8: e58d3048     	str	r3, [sp, #0x48]
    6efc: e3441303     	movt	r1, #0x4303
    6f00: e58d3050     	str	r3, [sp, #0x50]
    6f04: e3a00000     	mov	r0, #0
    6f08: e58d3058     	str	r3, [sp, #0x58]
    6f0c: e344030e     	movt	r0, #0x430e
    6f10: e58d3060     	str	r3, [sp, #0x60]
    6f14: e3a08000     	mov	r8, #0
    6f18: e58d3068     	str	r3, [sp, #0x68]
    6f1c: e344830f     	movt	r8, #0x430f
    6f20: e58d9024     	str	r9, [sp, #0x24]
    6f24: e3a09000     	mov	r9, #0
    6f28: eeb86ae5     	vcvt.f32.s32	s12, s11
    6f2c: e58d1034     	str	r1, [sp, #0x34]
    6f30: e58d0044     	str	r0, [sp, #0x44]
    6f34: e344930c     	movt	r9, #0x430c
    6f38: ed8d5a13     	vstr	s10, [sp, #76]
    6f3c: e3a01000     	mov	r1, #0
    6f40: e58d8054     	str	r8, [sp, #0x54]
    6f44: e344130d     	movt	r1, #0x430d
    6f48: e58d9064     	str	r9, [sp, #0x64]
    6f4c: eef87ae8     	vcvt.f32.s32	s15, s17
    6f50: ed8d6a07     	vstr	s12, [sp, #28]
    6f54: eeb87ae4     	vcvt.f32.s32	s14, s9
    6f58: ed8d7a0b     	vstr	s14, [sp, #44]
    6f5c: edcd7a1b     	vstr	s15, [sp, #108]
    6f60: e59c000c     	ldr	r0, [r12, #0xc]
    6f64: e58d3070     	str	r3, [sp, #0x70]
    6f68: e0822000     	add	r2, r2, r0
    6f6c: e58d1074     	str	r1, [sp, #0x74]
    6f70: e3520006     	cmp	r2, #6
    6f74: e58c2008     	str	r2, [r12, #0x8]
    6f78: c3e03000     	mvngt	r3, #0
    6f7c: c58c300c     	strgt	r3, [r12, #0xc]
    6f80: ca000001     	bgt	0x6f8c <led_menuPagesIndicator+0x1b8> @ imm = #0x4
    6f84: e3520001     	cmp	r2, #1
    6f88: d58c300c     	strle	r3, [r12, #0xc]
    6f8c: eebd8ac8     	vcvt.s32.f32	s16, s16
    6f90: e5d4c030     	ldrb	r12, [r4, #0x30]
    6f94: e59f3178     	ldr	r3, [pc, #0x178]        @ 0x7114 <led_menuPagesIndicator+0x340>  // u32=0x20414; f32?=1.85133948e-40
    6f98: e35c0062     	cmp	r12, #98
    6f9c: e08f9003     	add	r9, pc, r3
    6fa0: ee188a10     	vmov	r8, s16
    6fa4: ed898a0c     	vstr	s16, [r9, #48]
    6fa8: 9a000043     	bls	0x70bc <led_menuPagesIndicator+0x2e8> @ imm = #0x10c
    6fac: e1560005     	cmp	r6, r5
    6fb0: 0a000009     	beq	0x6fdc <led_menuPagesIndicator+0x208> @ imm = #0x24
    6fb4: e2865007     	add	r5, r6, #7
    6fb8: e6ef1075     	uxtb	r1, r5
    6fbc: e3a02001     	mov	r2, #1
    6fc0: e1a00004     	mov	r0, r4
    6fc4: ebfff32d     	bl	0x3c80 <.plt+0x584>     @ imm = #-0x334c  // CALL led_backgroundControl
    6fc8: e59f1148     	ldr	r1, [pc, #0x148]        @ 0x7118 <led_menuPagesIndicator+0x344>  // u32=0x20310; f32?=1.8476961e-40
    6fcc: e3a02001     	mov	r2, #1
    6fd0: e5c76de0     	strb	r6, [r7, #0xde0]
    6fd4: e08fc001     	add	r12, pc, r1
    6fd8: e58c2008     	str	r2, [r12, #0x8]
    6fdc: e59f3138     	ldr	r3, [pc, #0x138]        @ 0x711c <led_menuPagesIndicator+0x348>  // u32=0x203d0; f32?=1.8503866e-40
    6fe0: e08f4003     	add	r4, pc, r3
    6fe4: e5848030     	str	r8, [r4, #0x30]
    6fe8: e28dd07c     	add	sp, sp, #124
    6fec: ecbd8b02     	vpop	{d8}
    6ff0: e8bd83f0     	pop	{r4, r5, r6, r7, r8, r9, pc}
    6ff4: e5d41039     	ldrb	r1, [r4, #0x39]
    6ff8: eef71a00     	vmov.f32	s3, #1.000000e+00
    6ffc: e5d46032     	ldrb	r6, [r4, #0x32]
    7000: e3a02001     	mov	r2, #1
    7004: e3510000     	cmp	r1, #0
    7008: e59f9110     	ldr	r9, [pc, #0x110]        @ 0x7120 <led_menuPagesIndicator+0x34c>  // u32=0x20378; f32?=1.84915345e-40
    700c: e1a00004     	mov	r0, r4
    7010: 15d46036     	ldrbne	r6, [r4, #0x36]
    7014: ee006a90     	vmov	s1, r6
    7018: eeb81a60     	vcvt.f32.u32	s2, s1
    701c: ee312a21     	vadd.f32	s4, s2, s3
    7020: eefc2ac2     	vcvt.u32.f32	s5, s4
    7024: eebd3ac8     	vcvt.s32.f32	s6, s16
    7028: edcd2a01     	vstr	s5, [sp, #4]
    702c: e5dd1004     	ldrb	r1, [sp, #0x4]
    7030: ee138a10     	vmov	r8, s6
    7034: ebfff311     	bl	0x3c80 <.plt+0x584>     @ imm = #-0x33bc  // CALL led_backgroundControl
    7038: e08fc009     	add	r12, pc, r9
    703c: eddc3a0c     	vldr	s7, [r12, #48]
    7040: eeb84ae3     	vcvt.f32.s32	s8, s7
    7044: eeb48a44     	vcmp.f32	s16, s8
    7048: eef1fa10     	vmrs	APSR_nzcv, fpscr
    704c: 0affffe2     	beq	0x6fdc <led_menuPagesIndicator+0x208> @ imm = #-0x78
    7050: e5d44030     	ldrb	r4, [r4, #0x30]
    7054: e3a0e001     	mov	lr, #1
    7058: e3a03000     	mov	r3, #0
    705c: e3a00000     	mov	r0, #0
    7060: e3540062     	cmp	r4, #98
    7064: e344030e     	movt	r0, #0x430e
    7068: e3a02000     	mov	r2, #0
    706c: e58de018     	str	lr, [sp, #0x18]
    7070: e344230c     	movt	r2, #0x430c
    7074: e58de020     	str	lr, [sp, #0x20]
    7078: e58d301c     	str	r3, [sp, #0x1c]
    707c: e58d302c     	str	r3, [sp, #0x2c]
    7080: e58d0024     	str	r0, [sp, #0x24]
    7084: e58de028     	str	lr, [sp, #0x28]
    7088: e58de030     	str	lr, [sp, #0x30]
    708c: e58d2034     	str	r2, [sp, #0x34]
    7090: 8affffd1     	bhi	0x6fdc <led_menuPagesIndicator+0x208> @ imm = #-0xbc
    7094: e59f5088     	ldr	r5, [pc, #0x88]         @ 0x7124 <led_menuPagesIndicator+0x350>  // u32=0xda18; f32?=7.82372959e-41
    7098: e5977dac     	ldr	r7, [r7, #0xdac]
    709c: e08f0005     	add	r0, pc, r5
    70a0: ebfff1a0     	bl	0x3728 <.plt+0x2c>      @ imm = #-0x3980  // CALL gensym
    70a4: e28d3018     	add	r3, sp, #24
    70a8: e3a02004     	mov	r2, #4
    70ac: e1a01000     	mov	r1, r0
    70b0: e1a00007     	mov	r0, r7
    70b4: ebfff2ee     	bl	0x3c74 <.plt+0x578>     @ imm = #-0x3448  // CALL outlet_list
    70b8: eaffffc7     	b	0x6fdc <led_menuPagesIndicator+0x208> @ imm = #-0xe4
    70bc: e59fe064     	ldr	lr, [pc, #0x64]         @ 0x7128 <led_menuPagesIndicator+0x354>  // u32=0xd9f0; f32?=7.81812439e-41
    70c0: e5979dac     	ldr	r9, [r7, #0xdac]
    70c4: e08f000e     	add	r0, pc, lr
    70c8: ebfff196     	bl	0x3728 <.plt+0x2c>      @ imm = #-0x39a8  // CALL gensym
    70cc: e28d3018     	add	r3, sp, #24
    70d0: e3a0200c     	mov	r2, #12
    70d4: e1a01000     	mov	r1, r0
    70d8: e1a00009     	mov	r0, r9
    70dc: ebfff2e4     	bl	0x3c74 <.plt+0x578>     @ imm = #-0x3470  // CALL outlet_list
    70e0: e1560005     	cmp	r6, r5
    70e4: 0affffbc     	beq	0x6fdc <led_menuPagesIndicator+0x208> @ imm = #-0x110
    70e8: e5d4003c     	ldrb	r0, [r4, #0x3c]
    70ec: e5d4106a     	ldrb	r1, [r4, #0x6a]
    70f0: e3500000     	cmp	r0, #0
    70f4: 12811007     	addne	r1, r1, #7
    70f8: 03a01000     	moveq	r1, #0
    70fc: 16ef1071     	uxtbne	r1, r1
    7100: eaffffad     	b	0x6fbc <led_menuPagesIndicator+0x1e8> @ imm = #-0x14c
    7104: 00 00 f6 42  	.word	0x42f60000
    7108: 98 05 02 00  	.word	0x00020598
    710c: b8 e1 00 00  	.word	0x0000e1b8
    7110: 6c 04 02 00  	.word	0x0002046c
    7114: 14 04 02 00  	.word	0x00020414
    7118: 10 03 02 00  	.word	0x00020310
    711c: d0 03 02 00  	.word	0x000203d0
    7120: 78 03 02 00  	.word	0x00020378
    7124: 18 da 00 00  	.word	0x0000da18
    7128: f0 d9 00 00  	.word	0x0000d9f0

