00002f40 <_getChord>:
    2f40: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
    2f44: e3a02020     	mov	r2, #32
    2f48: ed2d8b04     	vpush	{d8, d9}
    2f4c: e1a05000     	mov	r5, r0
    2f50: e3a01000     	mov	r1, #0
    2f54: e2856a02     	add	r6, r5, #8192
    2f58: e2864e6f     	add	r4, r6, #1776
    2f5c: e24dd074     	sub	sp, sp, #116
    2f60: e28da010     	add	r10, sp, #16
    2f64: e28db030     	add	r11, sp, #48
    2f68: e28d8050     	add	r8, sp, #80
    2f6c: e1a0000a     	mov	r0, r10
    2f70: ebfffda3     	bl	0x2604 <.plt+0x224>     @ imm = #-0x974  // CALL memset
    2f74: e3a02020     	mov	r2, #32
    2f78: e3a01000     	mov	r1, #0
    2f7c: e1a0000b     	mov	r0, r11
    2f80: ebfffd9f     	bl	0x2604 <.plt+0x224>     @ imm = #-0x984  // CALL memset
    2f84: e59fc398     	ldr	r12, [pc, #0x398]       @ 0x3324 <_getChord+0x3e4>  // u32=0x7e1c; f32?=4.52395196e-41
    2f88: edd47a00     	vldr	s15, [r4]
    2f8c: e1a0e008     	mov	lr, r8
    2f90: e08f000c     	add	r0, pc, r12
    2f94: e2809c02     	add	r9, r0, #512
    2f98: e8b9000f     	ldm	r9!, {r0, r1, r2, r3}
    2f9c: eef57ac0     	vcmpe.f32	s15, #0
    2fa0: e8ae000f     	stm	lr!, {r0, r1, r2, r3}
    2fa4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2fa8: c3a07001     	movgt	r7, #1
    2fac: e899000f     	ldm	r9, {r0, r1, r2, r3}
    2fb0: d3a07000     	movle	r7, #0
    2fb4: cdcd7a04     	vstrgt	s15, [sp, #16]
    2fb8: e88e000f     	stm	lr, {r0, r1, r2, r3}
    2fbc: c1a03004     	movgt	r3, r4
    2fc0: c2833004     	addgt	r3, r3, #4
    2fc4: c59626f8     	ldrgt	r2, [r6, #0x6f8]
    2fc8: c5933000     	ldrgt	r3, [r3]
    2fcc: c58d2050     	strgt	r2, [sp, #0x50]
    2fd0: c58d3030     	strgt	r3, [sp, #0x30]
    2fd4: e2863e6f     	add	r3, r6, #1776
    2fd8: ed930a03     	vldr	s0, [r3, #12]
    2fdc: eeb50ac0     	vcmpe.f32	s0, #0
    2fe0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2fe4: da000008     	ble	0x300c <_getChord+0xcc> @ imm = #0x20
    2fe8: e28d1070     	add	r1, sp, #112
    2fec: e2852c27     	add	r2, r5, #9984
    2ff0: e081c107     	add	r12, r1, r7, lsl #2
    2ff4: e5960704     	ldr	r0, [r6, #0x704]
    2ff8: e5929000     	ldr	r9, [r2]
    2ffc: e2877001     	add	r7, r7, #1
    3000: ed0c0a18     	vstr	s0, [r12, #-96]
    3004: e50c0020     	str	r0, [r12, #-0x20]
    3008: e50c9040     	str	r9, [r12, #-0x40]
    300c: e2864c07     	add	r4, r6, #1792
    3010: edd40a02     	vldr	s1, [r4, #8]
    3014: eef50ac0     	vcmpe.f32	s1, #0
    3018: eef1fa10     	vmrs	APSR_nzcv, fpscr
    301c: da000009     	ble	0x3048 <_getChord+0x108> @ imm = #0x24
    3020: e28d3070     	add	r3, sp, #112
    3024: e286ec07     	add	lr, r6, #1792
    3028: e083c107     	add	r12, r3, r7, lsl #2
    302c: e28e200c     	add	r2, lr, #12
    3030: e5961710     	ldr	r1, [r6, #0x710]
    3034: e2877001     	add	r7, r7, #1
    3038: ed4c0a18     	vstr	s1, [r12, #-96]
    303c: e5920000     	ldr	r0, [r2]
    3040: e50c1020     	str	r1, [r12, #-0x20]
    3044: e50c0040     	str	r0, [r12, #-0x40]
    3048: e2869e71     	add	r9, r6, #1808
    304c: ed991a01     	vldr	s2, [r9, #4]
    3050: eeb51ac0     	vcmpe.f32	s2, #0
    3054: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3058: da000009     	ble	0x3084 <_getChord+0x144> @ imm = #0x24
    305c: e28de070     	add	lr, sp, #112
    3060: e2864e71     	add	r4, r6, #1808
    3064: e08e3107     	add	r3, lr, r7, lsl #2
    3068: e284c008     	add	r12, r4, #8
    306c: e596171c     	ldr	r1, [r6, #0x71c]
    3070: e2877001     	add	r7, r7, #1
    3074: ed031a18     	vstr	s2, [r3, #-96]
    3078: e59c2000     	ldr	r2, [r12]
    307c: e5031020     	str	r1, [r3, #-0x20]
    3080: e5032040     	str	r2, [r3, #-0x40]
    3084: e2860e72     	add	r0, r6, #1824
    3088: edd01a00     	vldr	s3, [r0]
    308c: eef51ac0     	vcmpe.f32	s3, #0
    3090: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3094: da000008     	ble	0x30bc <_getChord+0x17c> @ imm = #0x20
    3098: e28d4070     	add	r4, sp, #112
    309c: e2803004     	add	r3, r0, #4
    30a0: e084e107     	add	lr, r4, r7, lsl #2
    30a4: e596c728     	ldr	r12, [r6, #0x728]
    30a8: e5931000     	ldr	r1, [r3]
    30ac: e2877001     	add	r7, r7, #1
    30b0: ed4e1a18     	vstr	s3, [lr, #-96]
    30b4: e50ec020     	str	r12, [lr, #-0x20]
    30b8: e50e1040     	str	r1, [lr, #-0x40]
    30bc: e2862e72     	add	r2, r6, #1824
    30c0: ed922a03     	vldr	s4, [r2, #12]
    30c4: eeb52ac0     	vcmpe.f32	s4, #0
    30c8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    30cc: da000008     	ble	0x30f4 <_getChord+0x1b4> @ imm = #0x20
    30d0: e28d9070     	add	r9, sp, #112
    30d4: e2860e73     	add	r0, r6, #1840
    30d8: e0894107     	add	r4, r9, r7, lsl #2
    30dc: e596e734     	ldr	lr, [r6, #0x734]
    30e0: e5903000     	ldr	r3, [r0]
    30e4: e2877001     	add	r7, r7, #1
    30e8: ed042a18     	vstr	s4, [r4, #-96]
    30ec: e504e020     	str	lr, [r4, #-0x20]
    30f0: e5043040     	str	r3, [r4, #-0x40]
    30f4: e286ce73     	add	r12, r6, #1840
    30f8: eddc2a02     	vldr	s5, [r12, #8]
    30fc: eef52ac0     	vcmpe.f32	s5, #0
    3100: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3104: da000009     	ble	0x3130 <_getChord+0x1f0> @ imm = #0x24
    3108: e28d2070     	add	r2, sp, #112
    310c: e2861e73     	add	r1, r6, #1840
    3110: e0820107     	add	r0, r2, r7, lsl #2
    3114: e281900c     	add	r9, r1, #12
    3118: e5964740     	ldr	r4, [r6, #0x740]
    311c: e2877001     	add	r7, r7, #1
    3120: ed402a18     	vstr	s5, [r0, #-96]
    3124: e599e000     	ldr	lr, [r9]
    3128: e5004020     	str	r4, [r0, #-0x20]
    312c: e500e040     	str	lr, [r0, #-0x40]
    3130: e2863d1d     	add	r3, r6, #1856
    3134: ed933a01     	vldr	s6, [r3, #4]
    3138: eeb53ac0     	vcmpe.f32	s6, #0
    313c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3140: da000009     	ble	0x316c <_getChord+0x22c> @ imm = #0x24
    3144: e28d1070     	add	r1, sp, #112
    3148: e286cd1d     	add	r12, r6, #1856
    314c: e0810107     	add	r0, r1, r7, lsl #2
    3150: e28c2008     	add	r2, r12, #8
    3154: e596974c     	ldr	r9, [r6, #0x74c]
    3158: e2877001     	add	r7, r7, #1
    315c: ed003a18     	vstr	s6, [r0, #-96]
    3160: e5924000     	ldr	r4, [r2]
    3164: e5009020     	str	r9, [r0, #-0x20]
    3168: e5004040     	str	r4, [r0, #-0x40]
    316c: ed9f9a69     	vldr	s18, [pc, #420]         @ 0x3318 <_getChord+0x3d8>  // f32=142
    3170: e285ed9d     	add	lr, r5, #10048
    3174: e28e4010     	add	r4, lr, #16
    3178: e3a0308e     	mov	r3, #142
    317c: e58d7008     	str	r7, [sp, #0x8]
    3180: ed9f8a65     	vldr	s16, [pc, #404]         @ 0x331c <_getChord+0x3dc>  // f32=0
    3184: e58d3000     	str	r3, [sp]
    3188: e58d600c     	str	r6, [sp, #0xc]
    318c: e59d7000     	ldr	r7, [sp]
    3190: e1a0c00a     	mov	r12, r10
    3194: ecbc1a01     	vldmia	r12!, {s2}
    3198: e1a0600b     	mov	r6, r11
    319c: e2877001     	add	r7, r7, #1
    31a0: e1a00005     	mov	r0, r5
    31a4: e1a09008     	mov	r9, r8
    31a8: e28aa010     	add	r10, r10, #16
    31ac: ee037a90     	vmov	s7, r7
    31b0: e28bb010     	add	r11, r11, #16
    31b4: eef00a49     	vmov.f32	s1, s18
    31b8: e58dc004     	str	r12, [sp, #0x4]
    31bc: ed841a00     	vstr	s2, [r4]
    31c0: e2888010     	add	r8, r8, #16
    31c4: eef88ae3     	vcvt.f32.s32	s17, s7
    31c8: eeb00a48     	vmov.f32	s0, s16
    31cc: ebfffcbe     	bl	0x24cc <.plt+0xec>      @ imm = #-0xd08  // CALL _memWrite
    31d0: ecb61a01     	vldmia	r6!, {s2}
    31d4: e1a00005     	mov	r0, r5
    31d8: eef00a68     	vmov.f32	s1, s17
    31dc: ed841a01     	vstr	s2, [r4, #4]
    31e0: eeb00a48     	vmov.f32	s0, s16
    31e4: ebfffcb8     	bl	0x24cc <.plt+0xec>      @ imm = #-0xd20  // CALL _memWrite
    31e8: ed1a4a03     	vldr	s8, [r10, #-12]
    31ec: e4991004     	ldr	r1, [r9], #4
    31f0: e1a00005     	mov	r0, r5
    31f4: eef00a68     	vmov.f32	s1, s17
    31f8: ed844a03     	vstr	s8, [r4, #12]
    31fc: e5841008     	str	r1, [r4, #0x8]
    3200: eeb01a44     	vmov.f32	s2, s8
    3204: eeb00a48     	vmov.f32	s0, s16
    3208: ebfffcaf     	bl	0x24cc <.plt+0xec>      @ imm = #-0xd44  // CALL _memWrite
    320c: e2872001     	add	r2, r7, #1
    3210: ed1b1a03     	vldr	s2, [r11, #-12]
    3214: e1a00005     	mov	r0, r5
    3218: e2847024     	add	r7, r4, #36
    321c: e2844030     	add	r4, r4, #48
    3220: ee042a90     	vmov	s9, r2
    3224: ed041a08     	vstr	s2, [r4, #-32]
    3228: eeb89ae4     	vcvt.f32.s32	s18, s9
    322c: eeb00a48     	vmov.f32	s0, s16
    3230: eef00a49     	vmov.f32	s1, s18
    3234: ebfffca4     	bl	0x24cc <.plt+0xec>      @ imm = #-0xd70  // CALL _memWrite
    3238: e59d0004     	ldr	r0, [sp, #0x4]
    323c: e518300c     	ldr	r3, [r8, #-0xc]
    3240: ed905a01     	vldr	s10, [r0, #4]
    3244: e1a00005     	mov	r0, r5
    3248: e504301c     	str	r3, [r4, #-0x1c]
    324c: eef00a49     	vmov.f32	s1, s18
    3250: ed045a06     	vstr	s10, [r4, #-24]
    3254: eeb01a45     	vmov.f32	s2, s10
    3258: eeb00a48     	vmov.f32	s0, s16
    325c: ebfffc9a     	bl	0x24cc <.plt+0xec>      @ imm = #-0xd98  // CALL _memWrite
    3260: e59dc000     	ldr	r12, [sp]
    3264: ed961a01     	vldr	s2, [r6, #4]
    3268: e1a00005     	mov	r0, r5
    326c: e28c1003     	add	r1, r12, #3
    3270: e28c6004     	add	r6, r12, #4
    3274: e58d6000     	str	r6, [sp]
    3278: ee051a90     	vmov	s11, r1
    327c: ed041a05     	vstr	s2, [r4, #-20]
    3280: eef89ae5     	vcvt.f32.s32	s19, s11
    3284: eeb00a48     	vmov.f32	s0, s16
    3288: eef00a69     	vmov.f32	s1, s19
    328c: ebfffc8e     	bl	0x24cc <.plt+0xec>      @ imm = #-0xdc8  // CALL _memWrite
    3290: ed9d6a00     	vldr	s12, [sp]
    3294: e5999004     	ldr	r9, [r9, #0x4]
    3298: e1a00005     	mov	r0, r5
    329c: ed1a1a01     	vldr	s2, [r10, #-4]
    32a0: e5049010     	str	r9, [r4, #-0x10]
    32a4: eeb89ac6     	vcvt.f32.s32	s18, s12
    32a8: ed041a03     	vstr	s2, [r4, #-12]
    32ac: eef00a69     	vmov.f32	s1, s19
    32b0: eeb00a48     	vmov.f32	s0, s16
    32b4: ebfffc84     	bl	0x24cc <.plt+0xec>      @ imm = #-0xdf0  // CALL _memWrite
    32b8: ed1b1a01     	vldr	s2, [r11, #-4]
    32bc: e1a00005     	mov	r0, r5
    32c0: eef00a49     	vmov.f32	s1, s18
    32c4: ed871a01     	vstr	s2, [r7, #4]
    32c8: eeb00a48     	vmov.f32	s0, s16
    32cc: ebfffc7e     	bl	0x24cc <.plt+0xec>      @ imm = #-0xe08  // CALL _memWrite
    32d0: e59d2000     	ldr	r2, [sp]
    32d4: e5180004     	ldr	r0, [r8, #-0x4]
    32d8: e3520096     	cmp	r2, #150
    32dc: e5870008     	str	r0, [r7, #0x8]
    32e0: 1affffa9     	bne	0x318c <_getChord+0x24c> @ imm = #-0x15c
    32e4: e59d7008     	ldr	r7, [sp, #0x8]
    32e8: e1a00005     	mov	r0, r5
    32ec: e59de00c     	ldr	lr, [sp, #0xc]
    32f0: eddf0a0a     	vldr	s1, [pc, #40]           @ 0x3320 <_getChord+0x3e0>  // f32=141
    32f4: e58e76e8     	str	r7, [lr, #0x6e8]
    32f8: ee067a90     	vmov	s13, r7
    32fc: ed9f0a06     	vldr	s0, [pc, #24]           @ 0x331c <_getChord+0x3dc>  // f32=0
    3300: eeb81ae6     	vcvt.f32.s32	s2, s13
    3304: ebfffc70     	bl	0x24cc <.plt+0xec>      @ imm = #-0xe40  // CALL _memWrite
    3308: e1a00007     	mov	r0, r7
    330c: e28dd074     	add	sp, sp, #116
    3310: ecbd8b04     	vpop	{d8, d9}
    3314: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
    3318: 00 00 0e 43  	.word	0x430e0000
    331c: 00 00 00 00  	.word	0x00000000
    3320: 00 00 0d 43  	.word	0x430d0000
    3324: 1c 7e 00 00  	.word	0x00007e1c

