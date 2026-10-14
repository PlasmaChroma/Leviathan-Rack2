00001f50 <arbhar_shmem_set>:
    1f50: e3520001     	cmp	r2, #1
    1f54: da00008e     	ble	0x2194 <arbhar_shmem_set+0x244> @ imm = #0x238
    1f58: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
    1f5c: e24dd014     	sub	sp, sp, #20
    1f60: e5935000     	ldr	r5, [r3]
    1f64: e3550001     	cmp	r5, #1
    1f68: 1a000084     	bne	0x2180 <arbhar_shmem_set+0x230> @ imm = #0x210
    1f6c: e1a06002     	mov	r6, r2
    1f70: e1a07000     	mov	r7, r0
    1f74: e1a02003     	mov	r2, r3
    1f78: e1a01006     	mov	r1, r6
    1f7c: e3a00000     	mov	r0, #0
    1f80: e1a04003     	mov	r4, r3
    1f84: ebfffb02     	bl	0xb94 <.plt+0x1a0>      @ imm = #-0x13f8  // CALL atom_getfloatarg
    1f88: eefd7ac0     	vcvt.s32.f32	s15, s0
    1f8c: ee179a90     	vmov	r9, s15
    1f90: e087a109     	add	r10, r7, r9, lsl #2
    1f94: e59a3058     	ldr	r3, [r10, #0x58]
    1f98: e3530000     	cmp	r3, #0
    1f9c: 0a0000dd     	beq	0x2318 <arbhar_shmem_set+0x3c8> @ imm = #0x374
    1fa0: e2460001     	sub	r0, r6, #1
    1fa4: e3a0b000     	mov	r11, #0
    1fa8: e2101003     	ands	r1, r0, #3
    1fac: 0a000035     	beq	0x2088 <arbhar_shmem_set+0x138> @ imm = #0xd4
    1fb0: e3510001     	cmp	r1, #1
    1fb4: 0a000021     	beq	0x2040 <arbhar_shmem_set+0xf0> @ imm = #0x84
    1fb8: e3510002     	cmp	r1, #2
    1fbc: 0a00000f     	beq	0x2000 <arbhar_shmem_set+0xb0> @ imm = #0x3c
    1fc0: e594c008     	ldr	r12, [r4, #0x8]
    1fc4: e35c0002     	cmp	r12, #2
    1fc8: 1a000065     	bne	0x2164 <arbhar_shmem_set+0x214> @ imm = #0x194
    1fcc: e1a00005     	mov	r0, r5
    1fd0: e1a02004     	mov	r2, r4
    1fd4: e1a01006     	mov	r1, r6
    1fd8: e1a0500c     	mov	r5, r12
    1fdc: ebfffae0     	bl	0xb64 <.plt+0x170>      @ imm = #-0x1480  // CALL atom_getsymbolarg
    1fe0: e59a208c     	ldr	r2, [r10, #0x8c]
    1fe4: e1a0300b     	mov	r3, r11
    1fe8: e58db000     	str	r11, [sp]
    1fec: e1a01009     	mov	r1, r9
    1ff0: e58d2004     	str	r2, [sp, #0x4]
    1ff4: e1a02000     	mov	r2, r0
    1ff8: e1a00007     	mov	r0, r7
    1ffc: ebfffaab     	bl	0xab0 <.plt+0xbc>       @ imm = #-0x1554  // CALL arbhar_shmem_set_tab
    2000: e794e185     	ldr	lr, [r4, r5, lsl #3]
    2004: e35e0002     	cmp	lr, #2
    2008: 1a000055     	bne	0x2164 <arbhar_shmem_set+0x214> @ imm = #0x154
    200c: e1a02004     	mov	r2, r4
    2010: e1a01006     	mov	r1, r6
    2014: e1a00005     	mov	r0, r5
    2018: e2855001     	add	r5, r5, #1
    201c: ebfffad0     	bl	0xb64 <.plt+0x170>      @ imm = #-0x14c0  // CALL atom_getsymbolarg
    2020: e59ac08c     	ldr	r12, [r10, #0x8c]
    2024: e3a03000     	mov	r3, #0
    2028: e58db000     	str	r11, [sp]
    202c: e1a01009     	mov	r1, r9
    2030: e58dc004     	str	r12, [sp, #0x4]
    2034: e1a02000     	mov	r2, r0
    2038: e1a00007     	mov	r0, r7
    203c: ebfffa9b     	bl	0xab0 <.plt+0xbc>       @ imm = #-0x1594  // CALL arbhar_shmem_set_tab
    2040: e7943185     	ldr	r3, [r4, r5, lsl #3]
    2044: e3530002     	cmp	r3, #2
    2048: 1a000045     	bne	0x2164 <arbhar_shmem_set+0x214> @ imm = #0x114
    204c: e1a00005     	mov	r0, r5
    2050: e2855001     	add	r5, r5, #1
    2054: e1a02004     	mov	r2, r4
    2058: e1a01006     	mov	r1, r6
    205c: ebfffac0     	bl	0xb64 <.plt+0x170>      @ imm = #-0x1500  // CALL atom_getsymbolarg
    2060: e59a208c     	ldr	r2, [r10, #0x8c]
    2064: e3a03000     	mov	r3, #0
    2068: e58db000     	str	r11, [sp]
    206c: e1a01009     	mov	r1, r9
    2070: e58d2004     	str	r2, [sp, #0x4]
    2074: e1a02000     	mov	r2, r0
    2078: e1a00007     	mov	r0, r7
    207c: ebfffa8b     	bl	0xab0 <.plt+0xbc>       @ imm = #-0x15d4  // CALL arbhar_shmem_set_tab
    2080: e1560005     	cmp	r6, r5
    2084: 0a000036     	beq	0x2164 <arbhar_shmem_set+0x214> @ imm = #0xd8
    2088: e794e185     	ldr	lr, [r4, r5, lsl #3]
    208c: e1a02004     	mov	r2, r4
    2090: e1a01006     	mov	r1, r6
    2094: e1a00005     	mov	r0, r5
    2098: e35e0002     	cmp	lr, #2
    209c: e2858001     	add	r8, r5, #1
    20a0: 1a00002f     	bne	0x2164 <arbhar_shmem_set+0x214> @ imm = #0xbc
    20a4: ebfffaae     	bl	0xb64 <.plt+0x170>      @ imm = #-0x1548  // CALL atom_getsymbolarg
    20a8: e59ac08c     	ldr	r12, [r10, #0x8c]
    20ac: e3a03000     	mov	r3, #0
    20b0: e1a01009     	mov	r1, r9
    20b4: e58db000     	str	r11, [sp]
    20b8: e58dc004     	str	r12, [sp, #0x4]
    20bc: e1a02000     	mov	r2, r0
    20c0: e1a00007     	mov	r0, r7
    20c4: ebfffa79     	bl	0xab0 <.plt+0xbc>       @ imm = #-0x161c  // CALL arbhar_shmem_set_tab
    20c8: e7943188     	ldr	r3, [r4, r8, lsl #3]
    20cc: e1a02004     	mov	r2, r4
    20d0: e1a01006     	mov	r1, r6
    20d4: e3530002     	cmp	r3, #2
    20d8: e1a00008     	mov	r0, r8
    20dc: 1a000020     	bne	0x2164 <arbhar_shmem_set+0x214> @ imm = #0x80
    20e0: ebfffa9f     	bl	0xb64 <.plt+0x170>      @ imm = #-0x1584  // CALL atom_getsymbolarg
    20e4: e59a208c     	ldr	r2, [r10, #0x8c]
    20e8: e2888001     	add	r8, r8, #1
    20ec: e1a01009     	mov	r1, r9
    20f0: e58db000     	str	r11, [sp]
    20f4: e3a03000     	mov	r3, #0
    20f8: e58d2004     	str	r2, [sp, #0x4]
    20fc: e1a02000     	mov	r2, r0
    2100: e1a00007     	mov	r0, r7
    2104: ebfffa69     	bl	0xab0 <.plt+0xbc>       @ imm = #-0x165c  // CALL arbhar_shmem_set_tab
    2108: e794c188     	ldr	r12, [r4, r8, lsl #3]
    210c: e1a02004     	mov	r2, r4
    2110: e1a01006     	mov	r1, r6
    2114: e35c0002     	cmp	r12, #2
    2118: e1a00008     	mov	r0, r8
    211c: 1a000010     	bne	0x2164 <arbhar_shmem_set+0x214> @ imm = #0x40
    2120: ebfffa8f     	bl	0xb64 <.plt+0x170>      @ imm = #-0x15c4  // CALL atom_getsymbolarg
    2124: e59a208c     	ldr	r2, [r10, #0x8c]
    2128: e3a03000     	mov	r3, #0
    212c: e1a01009     	mov	r1, r9
    2130: e58db000     	str	r11, [sp]
    2134: e58d2004     	str	r2, [sp, #0x4]
    2138: e1a02000     	mov	r2, r0
    213c: e1a00007     	mov	r0, r7
    2140: ebfffa5a     	bl	0xab0 <.plt+0xbc>       @ imm = #-0x1698  // CALL arbhar_shmem_set_tab
    2144: e2853003     	add	r3, r5, #3
    2148: e1a02004     	mov	r2, r4
    214c: e1a01006     	mov	r1, r6
    2150: e794c183     	ldr	r12, [r4, r3, lsl #3]
    2154: e1a00003     	mov	r0, r3
    2158: e2855004     	add	r5, r5, #4
    215c: e35c0002     	cmp	r12, #2
    2160: 0affffbd     	beq	0x205c <arbhar_shmem_set+0x10c> @ imm = #-0x10c
    2164: e3560002     	cmp	r6, #2
    2168: 0a000002     	beq	0x2178 <arbhar_shmem_set+0x228> @ imm = #0x8
    216c: e7940189     	ldr	r0, [r4, r9, lsl #3]
    2170: e3500001     	cmp	r0, #1
    2174: 0a000009     	beq	0x21a0 <arbhar_shmem_set+0x250> @ imm = #0x24
    2178: e28dd014     	add	sp, sp, #20
    217c: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
    2180: e59f6238     	ldr	r6, [pc, #0x238]        @ 0x23c0 <arbhar_shmem_set+0x470>  // u32=0xc08; f32?=4.31599927e-42
    2184: e08f0006     	add	r0, pc, r6
    2188: e28dd014     	add	sp, sp, #20
    218c: e8bd4ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
    2190: eafffa40     	b	0xa98 <.plt+0xa4>       @ imm = #-0x1700  // CALL error
    2194: e59fc228     	ldr	r12, [pc, #0x228]       @ 0x23c4 <arbhar_shmem_set+0x474>  // u32=0xb7c; f32?=4.11981749e-42
    2198: e08f100c     	add	r1, pc, r12
    219c: eafffa7f     	b	0xba0 <.plt+0x1ac>      @ imm = #-0x1604  // CALL pd_error
    21a0: e1a01006     	mov	r1, r6
    21a4: e1a02004     	mov	r2, r4
    21a8: ebfffa79     	bl	0xb94 <.plt+0x1a0>      @ imm = #-0x161c  // CALL atom_getfloatarg
    21ac: e5940010     	ldr	r0, [r4, #0x10]
    21b0: e3500002     	cmp	r0, #2
    21b4: eebd0ac0     	vcvt.s32.f32	s0, s0
    21b8: ee101a10     	vmov	r1, s0
    21bc: e1c15fc1     	bic	r5, r1, r1, asr #31
    21c0: 0a000057     	beq	0x2324 <arbhar_shmem_set+0x3d4> @ imm = #0x15c
    21c4: e3500001     	cmp	r0, #1
    21c8: 1affffea     	bne	0x2178 <arbhar_shmem_set+0x228> @ imm = #-0x58
    21cc: e597808c     	ldr	r8, [r7, #0x8c]
    21d0: e0488005     	sub	r8, r8, r5
    21d4: e1560008     	cmp	r6, r8
    21d8: d2468001     	suble	r8, r6, #1
    21dc: e3580000     	cmp	r8, #0
    21e0: daffffe4     	ble	0x2178 <arbhar_shmem_set+0x228> @ imm = #-0x70
    21e4: e2189003     	ands	r9, r8, #3
    21e8: e1a05105     	lsl	r5, r5, #2
    21ec: e3a07000     	mov	r7, #0
    21f0: 0a000020     	beq	0x2278 <arbhar_shmem_set+0x328> @ imm = #0x80
    21f4: e3590001     	cmp	r9, #1
    21f8: 0a000013     	beq	0x224c <arbhar_shmem_set+0x2fc> @ imm = #0x4c
    21fc: e3590002     	cmp	r9, #2
    2200: 0a000008     	beq	0x2228 <arbhar_shmem_set+0x2d8> @ imm = #0x20
    2204: e3a07001     	mov	r7, #1
    2208: e1a02004     	mov	r2, r4
    220c: e1a00007     	mov	r0, r7
    2210: e1a01006     	mov	r1, r6
    2214: e59ab058     	ldr	r11, [r10, #0x58]
    2218: ebfffa5d     	bl	0xb94 <.plt+0x1a0>      @ imm = #-0x168c  // CALL atom_getfloatarg
    221c: e08b9005     	add	r9, r11, r5
    2220: e2855004     	add	r5, r5, #4
    2224: ed890a00     	vstr	s0, [r9]
    2228: e2877001     	add	r7, r7, #1
    222c: e59ae058     	ldr	lr, [r10, #0x58]
    2230: e1a02004     	mov	r2, r4
    2234: e1a01006     	mov	r1, r6
    2238: e1a00007     	mov	r0, r7
    223c: e08eb005     	add	r11, lr, r5
    2240: ebfffa53     	bl	0xb94 <.plt+0x1a0>      @ imm = #-0x16b4  // CALL atom_getfloatarg
    2244: e2855004     	add	r5, r5, #4
    2248: ed8b0a00     	vstr	s0, [r11]
    224c: e2877001     	add	r7, r7, #1
    2250: e59a3058     	ldr	r3, [r10, #0x58]
    2254: e1a02004     	mov	r2, r4
    2258: e1a01006     	mov	r1, r6
    225c: e1a00007     	mov	r0, r7
    2260: e0839005     	add	r9, r3, r5
    2264: ebfffa4a     	bl	0xb94 <.plt+0x1a0>      @ imm = #-0x16d8  // CALL atom_getfloatarg
    2268: e1570008     	cmp	r7, r8
    226c: e2855004     	add	r5, r5, #4
    2270: ed890a00     	vstr	s0, [r9]
    2274: 0affffbf     	beq	0x2178 <arbhar_shmem_set+0x228> @ imm = #-0x104
    2278: e59ac058     	ldr	r12, [r10, #0x58]
    227c: e2870001     	add	r0, r7, #1
    2280: e1a02004     	mov	r2, r4
    2284: e1a01006     	mov	r1, r6
    2288: e08ce005     	add	lr, r12, r5
    228c: e58de00c     	str	lr, [sp, #0xc]
    2290: ebfffa3f     	bl	0xb94 <.plt+0x1a0>      @ imm = #-0x1704  // CALL atom_getfloatarg
    2294: e59d900c     	ldr	r9, [sp, #0xc]
    2298: e59ab058     	ldr	r11, [r10, #0x58]
    229c: e2853004     	add	r3, r5, #4
    22a0: e2870002     	add	r0, r7, #2
    22a4: e1a02004     	mov	r2, r4
    22a8: e1a01006     	mov	r1, r6
    22ac: e08bb003     	add	r11, r11, r3
    22b0: ed890a00     	vstr	s0, [r9]
    22b4: ebfffa36     	bl	0xb94 <.plt+0x1a0>      @ imm = #-0x1728  // CALL atom_getfloatarg
    22b8: e59ac058     	ldr	r12, [r10, #0x58]
    22bc: e2853008     	add	r3, r5, #8
    22c0: e2870003     	add	r0, r7, #3
    22c4: e1a02004     	mov	r2, r4
    22c8: e1a01006     	mov	r1, r6
    22cc: e08c9003     	add	r9, r12, r3
    22d0: e58d900c     	str	r9, [sp, #0xc]
    22d4: e2877004     	add	r7, r7, #4
    22d8: ed8b0a00     	vstr	s0, [r11]
    22dc: ebfffa2c     	bl	0xb94 <.plt+0x1a0>      @ imm = #-0x1750  // CALL atom_getfloatarg
    22e0: e59d300c     	ldr	r3, [sp, #0xc]
    22e4: e59ab058     	ldr	r11, [r10, #0x58]
    22e8: e285c00c     	add	r12, r5, #12
    22ec: e1a00007     	mov	r0, r7
    22f0: e1a02004     	mov	r2, r4
    22f4: e1a01006     	mov	r1, r6
    22f8: e08b900c     	add	r9, r11, r12
    22fc: e2855010     	add	r5, r5, #16
    2300: ed830a00     	vstr	s0, [r3]
    2304: ebfffa22     	bl	0xb94 <.plt+0x1a0>      @ imm = #-0x1778  // CALL atom_getfloatarg
    2308: e1570008     	cmp	r7, r8
    230c: ed890a00     	vstr	s0, [r9]
    2310: 1affffd8     	bne	0x2278 <arbhar_shmem_set+0x328> @ imm = #-0xa0
    2314: eaffff97     	b	0x2178 <arbhar_shmem_set+0x228> @ imm = #-0x1a4
    2318: e59f40a8     	ldr	r4, [pc, #0xa8]         @ 0x23c8 <arbhar_shmem_set+0x478>  // u32=0x764; f32?=2.65125669e-42
    231c: e08f0004     	add	r0, pc, r4
    2320: eaffff98     	b	0x2188 <arbhar_shmem_set+0x238> @ imm = #-0x1a0
    2324: e3560003     	cmp	r6, #3
    2328: e59aa08c     	ldr	r10, [r10, #0x8c]
    232c: 03a08000     	moveq	r8, #0
    2330: 0a000008     	beq	0x2358 <arbhar_shmem_set+0x408> @ imm = #0x20
    2334: e5942018     	ldr	r2, [r4, #0x18]
    2338: e3520001     	cmp	r2, #1
    233c: 13a08000     	movne	r8, #0
    2340: 0a000016     	beq	0x23a0 <arbhar_shmem_set+0x450> @ imm = #0x58
    2344: e3560004     	cmp	r6, #4
    2348: 0a000002     	beq	0x2358 <arbhar_shmem_set+0x408> @ imm = #0x8
    234c: e5940020     	ldr	r0, [r4, #0x20]
    2350: e3500001     	cmp	r0, #1
    2354: 0a00000a     	beq	0x2384 <arbhar_shmem_set+0x434> @ imm = #0x28
    2358: e1a02004     	mov	r2, r4
    235c: e1a01006     	mov	r1, r6
    2360: e3a00001     	mov	r0, #1
    2364: ebfff9fe     	bl	0xb64 <.plt+0x170>      @ imm = #-0x1808  // CALL atom_getsymbolarg
    2368: e88d0420     	stm	sp, {r5, r10}
    236c: e1a03008     	mov	r3, r8
    2370: e1a01009     	mov	r1, r9
    2374: e1a02000     	mov	r2, r0
    2378: e1a00007     	mov	r0, r7
    237c: ebfff9cb     	bl	0xab0 <.plt+0xbc>       @ imm = #-0x18d4  // CALL arbhar_shmem_set_tab
    2380: eaffff7c     	b	0x2178 <arbhar_shmem_set+0x228> @ imm = #-0x210
    2384: e1a02004     	mov	r2, r4
    2388: e1a01006     	mov	r1, r6
    238c: e3a00004     	mov	r0, #4
    2390: ebfff9ff     	bl	0xb94 <.plt+0x1a0>      @ imm = #-0x1804  // CALL atom_getfloatarg
    2394: eebd1ac0     	vcvt.s32.f32	s2, s0
    2398: ee11aa10     	vmov	r10, s2
    239c: eaffffed     	b	0x2358 <arbhar_shmem_set+0x408> @ imm = #-0x4c
    23a0: e1a01006     	mov	r1, r6
    23a4: e1a02004     	mov	r2, r4
    23a8: e3a00003     	mov	r0, #3
    23ac: ebfff9f8     	bl	0xb94 <.plt+0x1a0>      @ imm = #-0x1820  // CALL atom_getfloatarg
    23b0: eefd0ac0     	vcvt.s32.f32	s1, s0
    23b4: ee101a90     	vmov	r1, s1
    23b8: e1c18fc1     	bic	r8, r1, r1, asr #31
    23bc: eaffffe0     	b	0x2344 <arbhar_shmem_set+0x3f4> @ imm = #-0x80
    23c0: 08 0c 00 00  	.word	0x00000c08
    23c4: 7c 0b 00 00  	.word	0x00000b7c
    23c8: 64 07 00 00  	.word	0x00000764

