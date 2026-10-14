0000f160 <arbhar_gpio_tilde_new>:
    f160: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
    f164: e3005df4     	movw	r5, #0xdf4
    f168: ed2d8b02     	vpush	{d8}
    f16c: e3a08000     	mov	r8, #0
    f170: e59f34b0     	ldr	r3, [pc, #0x4b0]        @ 0xf628 <arbhar_gpio_tilde_new+0x4c8>
    f174: e3007101     	movw	r7, #0x101
    f178: e3a0a001     	mov	r10, #1
    f17c: e3a06000     	mov	r6, #0
    f180: e08f0003     	add	r0, pc, r3
    f184: eef08a60     	vmov.f32	s17, s1
    f188: e24dd084     	sub	sp, sp, #132
    f18c: e34b6f80     	movt	r6, #0xbf80
    f190: e59001c0     	ldr	r0, [r0, #0x1c0]
    f194: eeb08a40     	vmov.f32	s16, s0
    f198: ebffd1a1     	bl	0x3824 <.plt+0x128>     @ imm = #-0xb97c
    f19c: e3a02001     	mov	r2, #1
    f1a0: e3a0e000     	mov	lr, #0
    f1a4: e344e2f0     	movt	lr, #0x42f0
    f1a8: e2809a01     	add	r9, r0, #4096
    f1ac: e2801c1e     	add	r1, r0, #7680
    f1b0: e289cedf     	add	r12, r9, #3568
    f1b4: e5c02065     	strb	r2, [r0, #0x65]
    f1b8: e1c076b6     	strh	r7, [r0, #102]
    f1bc: e2803ef2     	add	r3, r0, #3872
    f1c0: e5c080ba     	strb	r8, [r0, #0xba]
    f1c4: e1a0b00c     	mov	r11, r12
    f1c8: e589af9c     	str	r10, [r9, #0xf9c]
    f1cc: e3a02801     	mov	r2, #65536
    f1d0: e18980b5     	strh	r8, [r9, r5]
    f1d4: e1a04000     	mov	r4, r0
    f1d8: e58ce008     	str	lr, [r12, #0x8]
    f1dc: e283e004     	add	lr, r3, #4
    f1e0: e59fc444     	ldr	r12, [pc, #0x444]       @ 0xf62c <arbhar_gpio_tilde_new+0x4cc>
    f1e4: e58b600c     	str	r6, [r11, #0xc]
    f1e8: e289be9b     	add	r11, r9, #2480
    f1ec: e5816000     	str	r6, [r1]
    f1f0: e3a01001     	mov	r1, #1
    f1f4: e58020bc     	str	r2, [r0, #0xbc]
    f1f8: e08fa00c     	add	r10, pc, r12
    f1fc: e5c01075     	strb	r1, [r0, #0x75]
    f200: e28b500c     	add	r5, r11, #12
    f204: e5c08035     	strb	r8, [r0, #0x35]
    f208: e58de008     	str	lr, [sp, #0x8]
    f20c: e58d500c     	str	r5, [sp, #0xc]
    f210: ebffd2df     	bl	0x3d94 <.plt+0x698>     @ imm = #-0xb484
    f214: e59f0414     	ldr	r0, [pc, #0x414]        @ 0xf630 <arbhar_gpio_tilde_new+0x4d0>
    f218: e3a05000     	mov	r5, #0
    f21c: e5d4103c     	ldrb	r1, [r4, #0x3c]
    f220: e08f0000     	add	r0, pc, r0
    f224: ebffd253     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0xb6b4
    f228: eddf7afc     	vldr	s15, [pc, #1008]        @ 0xf620 <arbhar_gpio_tilde_new+0x4c0>
    f22c: e2891d37     	add	r1, r9, #3520
    f230: e289cedd     	add	r12, r9, #3536
    f234: e2842d75     	add	r2, r4, #7488
    f238: e3e0e000     	mvn	lr, #0
    f23c: ed9f0af8     	vldr	s0, [pc, #992]          @ 0xf624 <arbhar_gpio_tilde_new+0x4c4>
    f240: e1a0b001     	mov	r11, r1
    f244: e5c9ede0     	strb	lr, [r9, #0xde0]
    f248: e1a03002     	mov	r3, r2
    f24c: e5c48025     	strb	r8, [r4, #0x25]
    f250: e3a00401     	mov	r0, #16777216
    f254: e5c48064     	strb	r8, [r4, #0x64]
    f258: e2822020     	add	r2, r2, #32
    f25c: e5c480c4     	strb	r8, [r4, #0xc4]
    f260: e2833030     	add	r3, r3, #48
    f264: e5840027     	str	r0, [r4, #0x27]
    f268: e284ed76     	add	lr, r4, #7552
    f26c: e58450ec     	str	r5, [r4, #0xec]
    f270: e3a00b13     	mov	r0, #19456
    f274: e5892d90     	str	r2, [r9, #0xd90]
    f278: e340086f     	movt	r0, #0x86f
    f27c: e5893d94     	str	r3, [r9, #0xd94]
    f280: e3032812     	movw	r2, #0x3812
    f284: e589ed98     	str	lr, [r9, #0xd98]
    f288: e3462f5a     	movt	r2, #0x6f5a
    f28c: e5c4810e     	strb	r8, [r4, #0x10e]
    f290: e3a03001     	mov	r3, #1
    f294: e5847083     	str	r7, [r4, #0x83]
    f298: e3a0e001     	mov	lr, #1
    f29c: e5c480b9     	strb	r8, [r4, #0xb9]
    f2a0: e340e200     	movt	lr, #0x200
    f2a4: e5c43031     	strb	r3, [r4, #0x31]
    f2a8: e3007102     	movw	r7, #0x102
    f2ac: e5c48038     	strb	r8, [r4, #0x38]
    f2b0: e3407100     	movt	r7, #0x100
    f2b4: e5840105     	str	r0, [r4, #0x105]
    f2b8: e3a03c01     	mov	r3, #256
    f2bc: e5842109     	str	r2, [r4, #0x109]
    f2c0: e3403200     	movt	r3, #0x200
    f2c4: e584e087     	str	lr, [r4, #0x87]
    f2c8: e3a00003     	mov	r0, #3
    f2cc: e584708b     	str	r7, [r4, #0x8b]
    f2d0: e3400100     	movt	r0, #0x100
    f2d4: e584308f     	str	r3, [r4, #0x8f]
    f2d8: e3a02001     	mov	r2, #1
    f2dc: e5840093     	str	r0, [r4, #0x93]
    f2e0: e3402100     	movt	r2, #0x100
    f2e4: e3a00019     	mov	r0, #25
    f2e8: e5842097     	str	r2, [r4, #0x97]
    f2ec: e5c4010d     	strb	r0, [r4, #0x10d]
    f2f0: e1a02008     	mov	r2, r8
    f2f4: e3a0e004     	mov	lr, #4
    f2f8: e3402101     	movt	r2, #0x101
    f2fc: e340e100     	movt	lr, #0x100
    f300: e58420a7     	str	r2, [r4, #0xa7]
    f304: e58420af     	str	r2, [r4, #0xaf]
    f308: e3a07c01     	mov	r7, #256
    f30c: e584e09b     	str	lr, [r4, #0x9b]
    f310: e3407100     	movt	r7, #0x100
    f314: e3a0e006     	mov	lr, #6
    f318: e584709f     	str	r7, [r4, #0x9f]
    f31c: e584e0ab     	str	lr, [r4, #0xab]
    f320: e3a03005     	mov	r3, #5
    f324: e3403101     	movt	r3, #0x101
    f328: e58430a3     	str	r3, [r4, #0xa3]
    f32c: edc17a03     	vstr	s15, [r1, #12]
    f330: e1a00004     	mov	r0, r4
    f334: e59f12f8     	ldr	r1, [pc, #0x2f8]        @ 0xf634 <arbhar_gpio_tilde_new+0x4d4>
    f338: e1a07004     	mov	r7, r4
    f33c: edcb7a02     	vstr	s15, [r11, #8]
    f340: e58c6004     	str	r6, [r12, #0x4]
    f344: e59f62ec     	ldr	r6, [pc, #0x2ec]        @ 0xf638 <arbhar_gpio_tilde_new+0x4d8>
    f348: ed8c0a00     	vstr	s0, [r12]
    f34c: e79a1001     	ldr	r1, [r10, r1]
    f350: ebffd1b7     	bl	0x3a34 <.plt+0x338>     @ imm = #-0xb924
    f354: e59fc2e0     	ldr	r12, [pc, #0x2e0]       @ 0xf63c <arbhar_gpio_tilde_new+0x4dc>
    f358: e58d8010     	str	r8, [sp, #0x10]
    f35c: e08fb00c     	add	r11, pc, r12
    f360: e58400c8     	str	r0, [r4, #0xc8]
    f364: e1a00004     	mov	r0, r4
    f368: e79a1006     	ldr	r1, [r10, r6]
    f36c: ebffd1b0     	bl	0x3a34 <.plt+0x338>     @ imm = #-0xb940
    f370: e59f32c8     	ldr	r3, [pc, #0x2c8]        @ 0xf640 <arbhar_gpio_tilde_new+0x4e0>
    f374: e59f62c8     	ldr	r6, [pc, #0x2c8]        @ 0xf644 <arbhar_gpio_tilde_new+0x4e4>
    f378: e58400cc     	str	r0, [r4, #0xcc]
    f37c: e1a00004     	mov	r0, r4
    f380: e79a1003     	ldr	r1, [r10, r3]
    f384: ebffd1aa     	bl	0x3a34 <.plt+0x338>     @ imm = #-0xb958
    f388: e59f22b8     	ldr	r2, [pc, #0x2b8]        @ 0xf648 <arbhar_gpio_tilde_new+0x4e8>
    f38c: e58400d0     	str	r0, [r4, #0xd0]
    f390: e1a00004     	mov	r0, r4
    f394: e79a1002     	ldr	r1, [r10, r2]
    f398: ebffd1a5     	bl	0x3a34 <.plt+0x338>     @ imm = #-0xb96c
    f39c: e59f12a8     	ldr	r1, [pc, #0x2a8]        @ 0xf64c <arbhar_gpio_tilde_new+0x4ec>
    f3a0: e58400d4     	str	r0, [r4, #0xd4]
    f3a4: e1a00004     	mov	r0, r4
    f3a8: e79a1001     	ldr	r1, [r10, r1]
    f3ac: ebffd1a0     	bl	0x3a34 <.plt+0x338>     @ imm = #-0xb980
    f3b0: e58400d8     	str	r0, [r4, #0xd8]
    f3b4: e1a00004     	mov	r0, r4
    f3b8: e79a1006     	ldr	r1, [r10, r6]
    f3bc: ebffd19c     	bl	0x3a34 <.plt+0x338>     @ imm = #-0xb990
    f3c0: e59fc288     	ldr	r12, [pc, #0x288]       @ 0xf650 <arbhar_gpio_tilde_new+0x4f0>
    f3c4: e59f6288     	ldr	r6, [pc, #0x288]        @ 0xf654 <arbhar_gpio_tilde_new+0x4f4>
    f3c8: e58400dc     	str	r0, [r4, #0xdc]
    f3cc: e1a00004     	mov	r0, r4
    f3d0: e79a100c     	ldr	r1, [r10, r12]
    f3d4: ebffd196     	bl	0x3a34 <.plt+0x338>     @ imm = #-0xb9a8
    f3d8: e59f3278     	ldr	r3, [pc, #0x278]        @ 0xf658 <arbhar_gpio_tilde_new+0x4f8>
    f3dc: e58400e0     	str	r0, [r4, #0xe0]
    f3e0: e1a00004     	mov	r0, r4
    f3e4: e79a1003     	ldr	r1, [r10, r3]
    f3e8: ebffd191     	bl	0x3a34 <.plt+0x338>     @ imm = #-0xb9bc
    f3ec: e59f2268     	ldr	r2, [pc, #0x268]        @ 0xf65c <arbhar_gpio_tilde_new+0x4fc>
    f3f0: e58400e4     	str	r0, [r4, #0xe4]
    f3f4: e1a00004     	mov	r0, r4
    f3f8: e79a1002     	ldr	r1, [r10, r2]
    f3fc: ebffd18c     	bl	0x3a34 <.plt+0x338>     @ imm = #-0xb9d0
    f400: e59f1258     	ldr	r1, [pc, #0x258]        @ 0xf660 <arbhar_gpio_tilde_new+0x500>
    f404: e58400e8     	str	r0, [r4, #0xe8]
    f408: e1a00004     	mov	r0, r4
    f40c: e79a1001     	ldr	r1, [r10, r1]
    f410: ebffd187     	bl	0x3a34 <.plt+0x338>     @ imm = #-0xb9e4
    f414: e58400f0     	str	r0, [r4, #0xf0]
    f418: e1a00004     	mov	r0, r4
    f41c: e79a1006     	ldr	r1, [r10, r6]
    f420: ebffd183     	bl	0x3a34 <.plt+0x338>     @ imm = #-0xb9f4
    f424: e59fc238     	ldr	r12, [pc, #0x238]       @ 0xf664 <arbhar_gpio_tilde_new+0x504>
    f428: e59f6238     	ldr	r6, [pc, #0x238]        @ 0xf668 <arbhar_gpio_tilde_new+0x508>
    f42c: e58400f4     	str	r0, [r4, #0xf4]
    f430: e1a00004     	mov	r0, r4
    f434: e79a100c     	ldr	r1, [r10, r12]
    f438: ebffd17d     	bl	0x3a34 <.plt+0x338>     @ imm = #-0xba0c
    f43c: e59f3228     	ldr	r3, [pc, #0x228]        @ 0xf66c <arbhar_gpio_tilde_new+0x50c>
    f440: e58400f8     	str	r0, [r4, #0xf8]
    f444: e1a00004     	mov	r0, r4
    f448: e79a1003     	ldr	r1, [r10, r3]
    f44c: ebffd178     	bl	0x3a34 <.plt+0x338>     @ imm = #-0xba20
    f450: e59f2218     	ldr	r2, [pc, #0x218]        @ 0xf670 <arbhar_gpio_tilde_new+0x510>
    f454: e58400fc     	str	r0, [r4, #0xfc]
    f458: e1a00004     	mov	r0, r4
    f45c: e79a1002     	ldr	r1, [r10, r2]
    f460: ebffd173     	bl	0x3a34 <.plt+0x338>     @ imm = #-0xba34
    f464: ed9f0b6b     	vldr	d0, [pc, #428]          @ 0xf618 <arbhar_gpio_tilde_new+0x4b8>
    f468: e3a015fe     	mov	r1, #1065353216
    f46c: e5c48054     	strb	r8, [r4, #0x54]
    f470: e584105c     	str	r1, [r4, #0x5c]
    f474: e5845058     	str	r5, [r4, #0x58]
    f478: e5840100     	str	r0, [r4, #0x100]
    f47c: e59400d8     	ldr	r0, [r4, #0xd8]
    f480: ebffd1da     	bl	0x3bf0 <.plt+0x4f4>     @ imm = #-0xb898
    f484: e79a1006     	ldr	r1, [r10, r6]
    f488: e1a00004     	mov	r0, r4
    f48c: e2846e83     	add	r6, r4, #2096
    f490: ebffd1d0     	bl	0x3bd8 <.plt+0x4dc>     @ imm = #-0xb8c0
    f494: e300348c     	movw	r3, #0x48c
    f498: e0842003     	add	r2, r4, r3
    f49c: e58d2004     	str	r2, [sp, #0x4]
    f4a0: e2842e9f     	add	r2, r4, #2544
    f4a4: e2866008     	add	r6, r6, #8
    f4a8: e58d6018     	str	r6, [sp, #0x18]
    f4ac: e2826004     	add	r6, r2, #4
    f4b0: e58d601c     	str	r6, [sp, #0x1c]
    f4b4: e2846ebb     	add	r6, r4, #2992
    f4b8: e0893003     	add	r3, r9, r3
    f4bc: e58d3034     	str	r3, [sp, #0x34]
    f4c0: e282300c     	add	r3, r2, #12
    f4c4: e2862008     	add	r2, r6, #8
    f4c8: e3006126     	movw	r6, #0x126
    f4cc: e5845120     	str	r5, [r4, #0x120]
    f4d0: e58d3020     	str	r3, [sp, #0x20]
    f4d4: e30032ea     	movw	r3, #0x2ea
    f4d8: e18480b6     	strh	r8, [r4, r6]
    f4dc: e30062ce     	movw	r6, #0x2ce
    f4e0: e18480b3     	strh	r8, [r4, r3]
    f4e4: e3a0300f     	mov	r3, #15
    f4e8: e5845128     	str	r5, [r4, #0x128]
    f4ec: e284ce4a     	add	r12, r4, #1184
    f4f0: e18430b6     	strh	r3, [r4, r6]
    f4f4: e3006492     	movw	r6, #0x492
    f4f8: e58452e4     	str	r5, [r4, #0x2e4]
    f4fc: e3a01000     	mov	r1, #0
    f500: e58d2024     	str	r2, [sp, #0x24]
    f504: e1a02008     	mov	r2, r8
    f508: e3402fe6     	movt	r2, #0xfe6
    f50c: e58422bc     	str	r2, [r4, #0x2bc]
    f510: e5842480     	str	r2, [r4, #0x480]
    f514: e3441220     	movt	r1, #0x4220
    f518: e18430b6     	strh	r3, [r4, r6]
    f51c: e2843ed7     	add	r3, r4, #3440
    f520: e283600c     	add	r6, r3, #12
    f524: e2843d36     	add	r3, r4, #3456
    f528: e58d6028     	str	r6, [sp, #0x28]
    f52c: e2836004     	add	r6, r3, #4
    f530: e2843d3d     	add	r3, r4, #3904
    f534: e58d602c     	str	r6, [sp, #0x2c]
    f538: e2836008     	add	r6, r3, #8
    f53c: e2893e49     	add	r3, r9, #1168
    f540: e58412c8     	str	r1, [r4, #0x2c8]
    f544: e284ee66     	add	lr, r4, #1632
    f548: e58452ec     	str	r5, [r4, #0x2ec]
    f54c: e284ae67     	add	r10, r4, #1648
    f550: e58d6030     	str	r6, [sp, #0x30]
    f554: e2836004     	add	r6, r3, #4
    f558: e3a03002     	mov	r3, #2
    f55c: e34039c4     	movt	r3, #0x9c4
    f560: e58432d4     	str	r3, [r4, #0x2d4]
    f564: e58d6038     	str	r6, [sp, #0x38]
    f568: e2846e81     	add	r6, r4, #2064
    f56c: e2866004     	add	r6, r6, #4
    f570: e5840118     	str	r0, [r4, #0x118]
    f574: e59d0004     	ldr	r0, [sp, #0x4]
    f578: e5801000     	str	r1, [r0]
    f57c: e2840e4b     	add	r0, r4, #1200
    f580: e5843498     	str	r3, [r4, #0x498]
    f584: e58c5008     	str	r5, [r12, #0x8]
    f588: e300c4ae     	movw	r12, #0x4ae
    f58c: e18480bc     	strh	r8, [r4, r12]
    f590: e300c656     	movw	r12, #0x656
    f594: e5805000     	str	r5, [r0]
    f598: e3a0000f     	mov	r0, #15
    f59c: e5842644     	str	r2, [r4, #0x644]
    f5a0: e18400bc     	strh	r0, [r4, r12]
    f5a4: e2840e65     	add	r0, r4, #1616
    f5a8: e5801000     	str	r1, [r0]
    f5ac: e30009d8     	movw	r0, #0x9d8
    f5b0: e584365c     	str	r3, [r4, #0x65c]
    f5b4: e58e500c     	str	r5, [lr, #0xc]
    f5b8: e300e672     	movw	lr, #0x672
    f5bc: e18480be     	strh	r8, [r4, lr]
    f5c0: e3a0e00f     	mov	lr, #15
    f5c4: e58a5004     	str	r5, [r10, #0x4]
    f5c8: e300a81a     	movw	r10, #0x81a
    f5cc: e5842808     	str	r2, [r4, #0x808]
    f5d0: e184e0ba     	strh	lr, [r4, r10]
    f5d4: e284ee83     	add	lr, r4, #2096
    f5d8: e5861000     	str	r1, [r6]
    f5dc: e58d6014     	str	r6, [sp, #0x14]
    f5e0: e3006836     	movw	r6, #0x836
    f5e4: e5843820     	str	r3, [r4, #0x820]
    f5e8: e58e5000     	str	r5, [lr]
    f5ec: e084e000     	add	lr, r4, r0
    f5f0: e18480b6     	strh	r8, [r4, r6]
    f5f4: e0890000     	add	r0, r9, r0
    f5f8: e59d6018     	ldr	r6, [sp, #0x18]
    f5fc: e58d003c     	str	r0, [sp, #0x3c]
    f600: e3a0000f     	mov	r0, #15
    f604: e5865000     	str	r5, [r6]
    f608: e30069de     	movw	r6, #0x9de
    f60c: e58429cc     	str	r2, [r4, #0x9cc]
    f610: e18400b6     	strh	r0, [r4, r6]
    f614: ea000029     	b	0xf6c0 <arbhar_gpio_tilde_new+0x560> @ imm = #0xa4
    f618: 00 00 00 00  	.word	0x00000000
    f61c: 00 40 8f 40  	.word	0x408f4000
    f620: 6d e7 fb 3d  	.word	0x3dfbe76d
    f624: 58 39 34 3c  	.word	0x3c343958
    f628: 30 82 01 00  	.word	0x00018230
    f62c: 00 7e 01 00  	.word	0x00017e00
    f630: 54 68 00 00  	.word	0x00006854
    f634: 64 02 00 00  	.word	0x00000264
    f638: e4 02 00 00  	.word	0x000002e4
    f63c: 6c 67 00 00  	.word	0x0000676c
    f640: c8 02 00 00  	.word	0x000002c8
    f644: 68 02 00 00  	.word	0x00000268
    f648: ac 02 00 00  	.word	0x000002ac
    f64c: cc 02 00 00  	.word	0x000002cc
    f650: a0 02 00 00  	.word	0x000002a0
    f654: 60 02 00 00  	.word	0x00000260
    f658: 8c 02 00 00  	.word	0x0000028c
    f65c: 90 02 00 00  	.word	0x00000290
    f660: bc 02 00 00  	.word	0x000002bc
    f664: b0 02 00 00  	.word	0x000002b0
    f668: c4 02 00 00  	.word	0x000002c4
    f66c: d4 02 00 00  	.word	0x000002d4
    f670: 58 02 00 00  	.word	0x00000258
    f674: bc 61 00 00  	.word	0x000061bc
    f678: 60 61 00 00  	.word	0x00006160
    f67c: 30 61 00 00  	.word	0x00006130
    f680: ec 60 00 00  	.word	0x000060ec
    f684: a4 60 00 00  	.word	0x000060a4
    f688: 6c 60 00 00  	.word	0x0000606c
    f68c: 30 60 00 00  	.word	0x00006030
    f690: bc 5f 00 00  	.word	0x00005fbc
    f694: e8 5f 00 00  	.word	0x00005fe8
    f698: 74 5f 00 00  	.word	0x00005f74
    f69c: 24 5f 00 00  	.word	0x00005f24
    f6a0: f8 5e 00 00  	.word	0x00005ef8
    f6a4: c0 5e 00 00  	.word	0x00005ec0
    f6a8: 84 5e 00 00  	.word	0x00005e84
    f6ac: 5c 5e 00 00  	.word	0x00005e5c
    f6b0: 18 5e 00 00  	.word	0x00005e18
    f6b4: 98 4d 00 00  	.word	0x00004d98
    f6b8: c0 5d 00 00  	.word	0x00005dc0
    f6bc: a0 51 00 00  	.word	0x000051a0
    f6c0: e30069fa     	movw	r6, #0x9fa
    f6c4: e58e1000     	str	r1, [lr]
    f6c8: e3000b9c     	movw	r0, #0xb9c
    f6cc: e59de01c     	ldr	lr, [sp, #0x1c]
    f6d0: e58439e4     	str	r3, [r4, #0x9e4]
    f6d4: e58e5000     	str	r5, [lr]
    f6d8: e59de020     	ldr	lr, [sp, #0x20]
    f6dc: e18480b6     	strh	r8, [r4, r6]
    f6e0: e0846000     	add	r6, r4, r0
    f6e4: e0890000     	add	r0, r9, r0
    f6e8: e58d001c     	str	r0, [sp, #0x1c]
    f6ec: e3000ba2     	movw	r0, #0xba2
    f6f0: e58e5000     	str	r5, [lr]
    f6f4: e3a0e00f     	mov	lr, #15
    f6f8: e5842b90     	str	r2, [r4, #0xb90]
    f6fc: e184e0b0     	strh	lr, [r4, r0]
    f700: e59d0024     	ldr	r0, [sp, #0x24]
    f704: e5861000     	str	r1, [r6]
    f708: e58d6018     	str	r6, [sp, #0x18]
    f70c: e3006bbe     	movw	r6, #0xbbe
    f710: e5843ba8     	str	r3, [r4, #0xba8]
    f714: e5805000     	str	r5, [r0]
    f718: e2840d2f     	add	r0, r4, #3008
    f71c: e18480b6     	strh	r8, [r4, r6]
    f720: e3006d66     	movw	r6, #0xd66
    f724: e5805000     	str	r5, [r0]
    f728: e2840ed6     	add	r0, r4, #3424
    f72c: e5842d54     	str	r2, [r4, #0xd54]
    f730: e184e0b6     	strh	lr, [r4, r6]
    f734: e59d6028     	ldr	r6, [sp, #0x28]
    f738: e5801000     	str	r1, [r0]
    f73c: e3000d82     	movw	r0, #0xd82
    f740: e5843d6c     	str	r3, [r4, #0xd6c]
    f744: e5865000     	str	r5, [r6]
    f748: e59d602c     	ldr	r6, [sp, #0x2c]
    f74c: e18480b0     	strh	r8, [r4, r0]
    f750: e2840d3d     	add	r0, r4, #3904
    f754: e5865000     	str	r5, [r6]
    f758: e3006f2a     	movw	r6, #0xf2a
    f75c: e5842f18     	str	r2, [r4, #0xf18]
    f760: e184e0b6     	strh	lr, [r4, r6]
    f764: e59d6008     	ldr	r6, [sp, #0x8]
    f768: e5861000     	str	r1, [r6]
    f76c: e59d6030     	ldr	r6, [sp, #0x30]
    f770: e5843f30     	str	r3, [r4, #0xf30]
    f774: e5805000     	str	r5, [r0]
    f778: e3000f46     	movw	r0, #0xf46
    f77c: e18480b0     	strh	r8, [r4, r0]
    f780: e300010a     	movw	r0, #0x10a
    f784: e5865000     	str	r5, [r6]
    f788: e30062ce     	movw	r6, #0x2ce
    f78c: e58920dc     	str	r2, [r9, #0xdc]
    f790: e58910e8     	str	r1, [r9, #0xe8]
    f794: e58930f4     	str	r3, [r9, #0xf4]
    f798: e1c9eebe     	strh	lr, [r9, #238]
    f79c: e5895104     	str	r5, [r9, #0x104]
    f7a0: e18980b0     	strh	r8, [r9, r0]
    f7a4: e30002b2     	movw	r0, #0x2b2
    f7a8: e58922a0     	str	r2, [r9, #0x2a0]
    f7ac: e58932b8     	str	r3, [r9, #0x2b8]
    f7b0: e589510c     	str	r5, [r9, #0x10c]
    f7b4: e18980b6     	strh	r8, [r9, r6]
    f7b8: e2896e47     	add	r6, r9, #1136
    f7bc: e58912ac     	str	r1, [r9, #0x2ac]
    f7c0: e5892464     	str	r2, [r9, #0x464]
    f7c4: e189e0b0     	strh	lr, [r9, r0]
    f7c8: e3000476     	movw	r0, #0x476
    f7cc: e58952c8     	str	r5, [r9, #0x2c8]
    f7d0: e189e0b0     	strh	lr, [r9, r0]
    f7d4: e58952d0     	str	r5, [r9, #0x2d0]
    f7d8: e59d0034     	ldr	r0, [sp, #0x34]
    f7dc: e5861000     	str	r1, [r6]
    f7e0: e3006492     	movw	r6, #0x492
    f7e4: e589347c     	str	r3, [r9, #0x47c]
    f7e8: e5805000     	str	r5, [r0]
    f7ec: e59d0038     	ldr	r0, [sp, #0x38]
    f7f0: e18980b6     	strh	r8, [r9, r6]
    f7f4: e300663a     	movw	r6, #0x63a
    f7f8: e5805000     	str	r5, [r0]
    f7fc: e2890e63     	add	r0, r9, #1584
    f800: e5892628     	str	r2, [r9, #0x628]
    f804: e189e0b6     	strh	lr, [r9, r6]
    f808: e1a0600e     	mov	r6, lr
    f80c: e5801004     	str	r1, [r0, #0x4]
    f810: e2890e65     	add	r0, r9, #1616
    f814: e5893640     	str	r3, [r9, #0x640]
    f818: e5805000     	str	r5, [r0]
    f81c: e18980bc     	strh	r8, [r9, r12]
    f820: e1a0c000     	mov	r12, r0
    f824: e58c5008     	str	r5, [r12, #0x8]
    f828: e2890e7f     	add	r0, r9, #2032
    f82c: e300c7fe     	movw	r12, #0x7fe
    f830: e58927ec     	str	r2, [r9, #0x7ec]
    f834: e189e0bc     	strh	lr, [r9, r12]
    f838: e289ee81     	add	lr, r9, #2064
    f83c: e5801008     	str	r1, [r0, #0x8]
    f840: e28ec004     	add	r12, lr, #4
    f844: e2890e81     	add	r0, r9, #2064
    f848: e5893804     	str	r3, [r9, #0x804]
    f84c: e58c5000     	str	r5, [r12]
    f850: e289eeba     	add	lr, r9, #2976
    f854: e18980ba     	strh	r8, [r9, r10]
    f858: e1a0a006     	mov	r10, r6
    f85c: e580500c     	str	r5, [r0, #0xc]
    f860: e30009c2     	movw	r0, #0x9c2
    f864: e58929b0     	str	r2, [r9, #0x9b0]
    f868: e289cd35     	add	r12, r9, #3392
    f86c: e18960b0     	strh	r6, [r9, r0]
    f870: e2890e9e     	add	r0, r9, #2528
    f874: e59d600c     	ldr	r6, [sp, #0xc]
    f878: e5861000     	str	r1, [r6]
    f87c: e59d603c     	ldr	r6, [sp, #0x3c]
    f880: e58939c8     	str	r3, [r9, #0x9c8]
    f884: e5865000     	str	r5, [r6]
    f888: e30069de     	movw	r6, #0x9de
    f88c: e18980b6     	strh	r8, [r9, r6]
    f890: e1a0600a     	mov	r6, r10
    f894: e5805000     	str	r5, [r0]
    f898: e3000b86     	movw	r0, #0xb86
    f89c: e5892b74     	str	r2, [r9, #0xb74]
    f8a0: e189a0b0     	strh	r10, [r9, r0]
    f8a4: e2840d6e     	add	r0, r4, #7040
    f8a8: e300ad4a     	movw	r10, #0xd4a
    f8ac: e5801000     	str	r1, [r0]
    f8b0: e59d001c     	ldr	r0, [sp, #0x1c]
    f8b4: e5893b8c     	str	r3, [r9, #0xb8c]
    f8b8: e5805000     	str	r5, [r0]
    f8bc: e3000ba2     	movw	r0, #0xba2
    f8c0: e18980b0     	strh	r8, [r9, r0]
    f8c4: e51f0258     	ldr	r0, [pc, #-0x258]       @ 0xf674 <arbhar_gpio_tilde_new+0x514>
    f8c8: e58e5004     	str	r5, [lr, #0x4]
    f8cc: e08f0000     	add	r0, pc, r0
    f8d0: e5892d38     	str	r2, [r9, #0xd38]
    f8d4: e18960ba     	strh	r6, [r9, r10]
    f8d8: e3a06a07     	mov	r6, #28672
    f8dc: e58c1004     	str	r1, [r12, #0x4]
    f8e0: e3446500     	movt	r6, #0x4500
    f8e4: e5893d50     	str	r3, [r9, #0xd50]
    f8e8: ebffcf8e     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xc1c8
    f8ec: e51fa27c     	ldr	r10, [pc, #-0x27c]      @ 0xf678 <arbhar_gpio_tilde_new+0x518>
    f8f0: e3a02002     	mov	r2, #2
    f8f4: e300c2ce     	movw	r12, #0x2ce
    f8f8: e5c482d0     	strb	r8, [r4, #0x2d0]
    f8fc: e1a03002     	mov	r3, r2
    f900: e18420bc     	strh	r2, [r4, r12]
    f904: e3403005     	movt	r3, #0x5
    f908: e5c4211c     	strb	r2, [r4, #0x11c]
    f90c: e3a01a0f     	mov	r1, #61440
    f910: e58432d4     	str	r3, [r4, #0x2d4]
    f914: e3441500     	movt	r1, #0x4500
    f918: e58452d8     	str	r5, [r4, #0x2d8]
    f91c: e58412c0     	str	r1, [r4, #0x2c0]
    f920: e5845128     	str	r5, [r4, #0x128]
    f924: e58452c8     	str	r5, [r4, #0x2c8]
    f928: e58402dc     	str	r0, [r4, #0x2dc]
    f92c: e08f000a     	add	r0, pc, r10
    f930: ebffcf7c     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xc210
    f934: e59dc004     	ldr	r12, [sp, #0x4]
    f938: e2842d12     	add	r2, r4, #1152
    f93c: e2843e49     	add	r3, r4, #1168
    f940: e300149a     	movw	r1, #0x49a
    f944: e3a0e004     	mov	lr, #4
    f948: e5c4e2e0     	strb	lr, [r4, #0x2e0]
    f94c: e58404a0     	str	r0, [r4, #0x4a0]
    f950: e51f02dc     	ldr	r0, [pc, #-0x2dc]       @ 0xf67c <arbhar_gpio_tilde_new+0x51c>
    f954: e5826004     	str	r6, [r2, #0x4]
    f958: e3a060fa     	mov	r6, #250
    f95c: e5c48494     	strb	r8, [r4, #0x494]
    f960: e08f0000     	add	r0, pc, r0
    f964: e583500c     	str	r5, [r3, #0xc]
    f968: e1a0a006     	mov	r10, r6
    f96c: e58452ec     	str	r5, [r4, #0x2ec]
    f970: e58c5000     	str	r5, [r12]
    f974: e18460b1     	strh	r6, [r4, r1]
    f978: e2846e65     	add	r6, r4, #1616
    f97c: ebffcf69     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xc25c
    f980: e2843e4a     	add	r3, r4, #1184
    f984: e3a0c007     	mov	r12, #7
    f988: e3a01a0d     	mov	r1, #53248
    f98c: e5c4c4a4     	strb	r12, [r4, #0x4a4]
    f990: e3441500     	movt	r1, #0x4500
    f994: e1a0200c     	mov	r2, r12
    f998: e34029c4     	movt	r2, #0x9c4
    f99c: e5840664     	str	r0, [r4, #0x664]
    f9a0: e51f0328     	ldr	r0, [pc, #-0x328]       @ 0xf680 <arbhar_gpio_tilde_new+0x520>
    f9a4: e58311a8     	str	r1, [r3, #0x1a8]
    f9a8: e08f0000     	add	r0, pc, r0
    f9ac: e5c48658     	strb	r8, [r4, #0x658]
    f9b0: e58351c0     	str	r5, [r3, #0x1c0]
    f9b4: e5835010     	str	r5, [r3, #0x10]
    f9b8: e5865000     	str	r5, [r6]
    f9bc: e3a06000     	mov	r6, #0
    f9c0: e584265c     	str	r2, [r4, #0x65c]
    f9c4: e34462c8     	movt	r6, #0x42c8
    f9c8: ebffcf56     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xc2a8
    f9cc: e59de014     	ldr	lr, [sp, #0x14]
    f9d0: e2843e66     	add	r3, r4, #1632
    f9d4: e3002822     	movw	r2, #0x822
    f9d8: e3a0100e     	mov	r1, #14
    f9dc: e3a0ca02     	mov	r12, #8192
    f9e0: e5c41668     	strb	r1, [r4, #0x668]
    f9e4: e344c4c5     	movt	r12, #0x44c5
    f9e8: e5840828     	str	r0, [r4, #0x828]
    f9ec: e51f0370     	ldr	r0, [pc, #-0x370]       @ 0xf684 <arbhar_gpio_tilde_new+0x524>
    f9f0: e583c1ac     	str	r12, [r3, #0x1ac]
    f9f4: e08f0000     	add	r0, pc, r0
    f9f8: e5c4881c     	strb	r8, [r4, #0x81c]
    f9fc: e58351c4     	str	r5, [r3, #0x1c4]
    fa00: e5835014     	str	r5, [r3, #0x14]
    fa04: e58e6000     	str	r6, [lr]
    fa08: e184a0b2     	strh	r10, [r4, r2]
    fa0c: ebffcf45     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xc2ec
    fa10: e51f6390     	ldr	r6, [pc, #-0x390]       @ 0xf688 <arbhar_gpio_tilde_new+0x528>
    fa14: e2843e82     	add	r3, r4, #2080
    fa18: e30029e6     	movw	r2, #0x9e6
    fa1c: e3a0c00a     	mov	r12, #10
    fa20: e3a01902     	mov	r1, #32768
    fa24: e5c4c82c     	strb	r12, [r4, #0x82c]
    fa28: e34414d0     	movt	r1, #0x44d0
    fa2c: e58409ec     	str	r0, [r4, #0x9ec]
    fa30: e08f0006     	add	r0, pc, r6
    fa34: e58311b0     	str	r1, [r3, #0x1b0]
    fa38: e5c489e0     	strb	r8, [r4, #0x9e0]
    fa3c: e58351c8     	str	r5, [r3, #0x1c8]
    fa40: e5835018     	str	r5, [r3, #0x18]
    fa44: e184a0b2     	strh	r10, [r4, r2]
    fa48: ebffcf36     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xc328
    fa4c: e59d6018     	ldr	r6, [sp, #0x18]
    fa50: e51fc3cc     	ldr	r12, [pc, #-0x3cc]      @ 0xf68c <arbhar_gpio_tilde_new+0x52c>
    fa54: e1a03004     	mov	r3, r4
    fa58: e3002baa     	movw	r2, #0xbaa
    fa5c: e3a01a02     	mov	r1, #8192
    fa60: e34414d5     	movt	r1, #0x44d5
    fa64: e5840bb0     	str	r0, [r4, #0xbb0]
    fa68: e3a00001     	mov	r0, #1
    fa6c: e5e309f0     	strb	r0, [r3, #0x9f0]!
    fa70: e08f000c     	add	r0, pc, r12
    fa74: e58311a4     	str	r1, [r3, #0x1a4]
    fa78: e5c48ba4     	strb	r8, [r4, #0xba4]
    fa7c: e58351bc     	str	r5, [r3, #0x1bc]
    fa80: e583500c     	str	r5, [r3, #0xc]
    fa84: e184a0b2     	strh	r10, [r4, r2]
    fa88: e3a0aa06     	mov	r10, #24576
    fa8c: e5865000     	str	r5, [r6]
    fa90: e344a4c9     	movt	r10, #0x44c9
    fa94: ebffcf23     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xc374
    fa98: e2843ebb     	add	r3, r4, #2992
    fa9c: e3a0100d     	mov	r1, #13
    faa0: e3a02a0d     	mov	r2, #53248
    faa4: e5c41bb4     	strb	r1, [r4, #0xbb4]
    faa8: e3442501     	movt	r2, #0x4501
    faac: e51f6424     	ldr	r6, [pc, #-0x424]       @ 0xf690 <arbhar_gpio_tilde_new+0x530>
    fab0: e5840d74     	str	r0, [r4, #0xd74]
    fab4: e51f0428     	ldr	r0, [pc, #-0x428]       @ 0xf694 <arbhar_gpio_tilde_new+0x534>
    fab8: e58321a8     	str	r2, [r3, #0x1a8]
    fabc: e08f0000     	add	r0, pc, r0
    fac0: e5c48d68     	strb	r8, [r4, #0xd68]
    fac4: e58351c0     	str	r5, [r3, #0x1c0]
    fac8: e5835010     	str	r5, [r3, #0x10]
    facc: ebffcf15     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xc3ac
    fad0: e59d3008     	ldr	r3, [sp, #0x8]
    fad4: e284ced7     	add	r12, r4, #3440
    fad8: e3001f2a     	movw	r1, #0xf2a
    fadc: e5c48d78     	strb	r8, [r4, #0xd78]
    fae0: e3a0200f     	mov	r2, #15
    fae4: e3402032     	movt	r2, #0x32
    fae8: e5840f38     	str	r0, [r4, #0xf38]
    faec: e08f0006     	add	r0, pc, r6
    faf0: e58ca1ac     	str	r10, [r12, #0x1ac]
    faf4: e2846ef3     	add	r6, r4, #3888
    faf8: e5c48f2c     	strb	r8, [r4, #0xf2c]
    fafc: e3a0a903     	mov	r10, #49152
    fb00: e58c51c4     	str	r5, [r12, #0x1c4]
    fb04: e344a4d2     	movt	r10, #0x44d2
    fb08: e58c5014     	str	r5, [r12, #0x14]
    fb0c: e3a0c00f     	mov	r12, #15
    fb10: e5835000     	str	r5, [r3]
    fb14: e184c0b1     	strh	r12, [r4, r1]
    fb18: e5842f30     	str	r2, [r4, #0xf30]
    fb1c: ebffcf01     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xc3fc
    fb20: e51f3490     	ldr	r3, [pc, #-0x490]       @ 0xf698 <arbhar_gpio_tilde_new+0x538>
    fb24: e3a0201e     	mov	r2, #30
    fb28: e3402fe6     	movt	r2, #0xfe6
    fb2c: e58900fc     	str	r0, [r9, #0xfc]
    fb30: e3a00006     	mov	r0, #6
    fb34: e5c40f3c     	strb	r0, [r4, #0xf3c]
    fb38: e08f0003     	add	r0, pc, r3
    fb3c: e586a1b0     	str	r10, [r6, #0x1b0]
    fb40: e3a0a000     	mov	r10, #0
    fb44: e5c980f0     	strb	r8, [r9, #0xf0]
    fb48: e344a320     	movt	r10, #0x4320
    fb4c: e58651c8     	str	r5, [r6, #0x1c8]
    fb50: e5865018     	str	r5, [r6, #0x18]
    fb54: e58920dc     	str	r2, [r9, #0xdc]
    fb58: ebffcef2     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xc438
    fb5c: e51f64c8     	ldr	r6, [pc, #-0x4c8]       @ 0xf69c <arbhar_gpio_tilde_new+0x53c>
    fb60: e3a0c003     	mov	r12, #3
    fb64: e5c982b4     	strb	r8, [r9, #0x2b4]
    fb68: e5c9c100     	strb	r12, [r9, #0x100]
    fb6c: e3a01a06     	mov	r1, #24576
    fb70: e589a2ac     	str	r10, [r9, #0x2ac]
    fb74: e34414d0     	movt	r1, #0x44d0
    fb78: e58952bc     	str	r5, [r9, #0x2bc]
    fb7c: e289ad12     	add	r10, r9, #1152
    fb80: e58912a4     	str	r1, [r9, #0x2a4]
    fb84: e589510c     	str	r5, [r9, #0x10c]
    fb88: e58902c0     	str	r0, [r9, #0x2c0]
    fb8c: e08f0006     	add	r0, pc, r6
    fb90: ebffcee4     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xc470
    fb94: e2893e46     	add	r3, r9, #1120
    fb98: e2842d52     	add	r2, r4, #5248
    fb9c: e3a0c00c     	mov	r12, #12
    fba0: e3a01a0e     	mov	r1, #57344
    fba4: e5c9c2c4     	strb	r12, [r9, #0x2c4]
    fba8: e34414cb     	movt	r1, #0x44cb
    fbac: e3a06005     	mov	r6, #5
    fbb0: e5890484     	str	r0, [r9, #0x484]
    fbb4: e51f051c     	ldr	r0, [pc, #-0x51c]       @ 0xf6a0 <arbhar_gpio_tilde_new+0x540>
    fbb8: e5831008     	str	r1, [r3, #0x8]
    fbbc: e08f0000     	add	r0, pc, r0
    fbc0: e5c98478     	strb	r8, [r9, #0x478]
    fbc4: e5825000     	str	r5, [r2]
    fbc8: e58952d0     	str	r5, [r9, #0x2d0]
    fbcc: ebffced5     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xc4ac
    fbd0: e51f3534     	ldr	r3, [pc, #-0x534]       @ 0xf6a4 <arbhar_gpio_tilde_new+0x544>
    fbd4: e5c96488     	strb	r6, [r9, #0x488]
    fbd8: e3a01901     	mov	r1, #16384
    fbdc: e34414da     	movt	r1, #0x44da
    fbe0: e3a0c019     	mov	r12, #25
    fbe4: e3a02021     	mov	r2, #33
    fbe8: e34020fa     	movt	r2, #0xfa
    fbec: e3a06903     	mov	r6, #49152
    fbf0: e34464d3     	movt	r6, #0x44d3
    fbf4: e5890648     	str	r0, [r9, #0x648]
    fbf8: e08f0003     	add	r0, pc, r3
    fbfc: e58a11ac     	str	r1, [r10, #0x1ac]
    fc00: e5c9863c     	strb	r8, [r9, #0x63c]
    fc04: e58a51c4     	str	r5, [r10, #0x1c4]
    fc08: e58a5014     	str	r5, [r10, #0x14]
    fc0c: e300a63a     	movw	r10, #0x63a
    fc10: e189c0ba     	strh	r12, [r9, r10]
    fc14: e3a0a008     	mov	r10, #8
    fc18: e5892640     	str	r2, [r9, #0x640]
    fc1c: ebffcec1     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xc4fc
    fc20: e2892d19     	add	r2, r9, #1600
    fc24: e3a01009     	mov	r1, #9
    fc28: e5c9164c     	strb	r1, [r9, #0x64c]
    fc2c: e589080c     	str	r0, [r9, #0x80c]
    fc30: e51f0590     	ldr	r0, [pc, #-0x590]       @ 0xf6a8 <arbhar_gpio_tilde_new+0x548>
    fc34: e58261b0     	str	r6, [r2, #0x1b0]
    fc38: e08f0000     	add	r0, pc, r0
    fc3c: e5c98800     	strb	r8, [r9, #0x800]
    fc40: e58251c8     	str	r5, [r2, #0x1c8]
    fc44: e5825018     	str	r5, [r2, #0x18]
    fc48: ebffceb6     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xc528
    fc4c: e59d200c     	ldr	r2, [sp, #0xc]
    fc50: e51f65ac     	ldr	r6, [pc, #-0x5ac]       @ 0xf6ac <arbhar_gpio_tilde_new+0x54c>
    fc54: e1a03009     	mov	r3, r9
    fc58: e3a0ca02     	mov	r12, #8192
    fc5c: e344c4d1     	movt	r12, #0x44d1
    fc60: e58909d0     	str	r0, [r9, #0x9d0]
    fc64: e08f0006     	add	r0, pc, r6
    fc68: e5e3a810     	strb	r10, [r3, #0x810]!
    fc6c: e3a0a000     	mov	r10, #0
    fc70: e583c1a4     	str	r12, [r3, #0x1a4]
    fc74: e344a4cc     	movt	r10, #0x44cc
    fc78: e5c989c4     	strb	r8, [r9, #0x9c4]
    fc7c: e2896eb9     	add	r6, r9, #2960
    fc80: e58351bc     	str	r5, [r3, #0x1bc]
    fc84: e583500c     	str	r5, [r3, #0xc]
    fc88: e5825000     	str	r5, [r2]
    fc8c: ebffcea5     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xc56c
    fc90: e2893e9d     	add	r3, r9, #2512
    fc94: e284cd6e     	add	r12, r4, #7040
    fc98: e3a0100b     	mov	r1, #11
    fc9c: e5c919d4     	strb	r1, [r9, #0x9d4]
    fca0: e5890b94     	str	r0, [r9, #0xb94]
    fca4: e51f05fc     	ldr	r0, [pc, #-0x5fc]       @ 0xf6b0 <arbhar_gpio_tilde_new+0x550>
    fca8: e583a1a8     	str	r10, [r3, #0x1a8]
    fcac: e08f0000     	add	r0, pc, r0
    fcb0: e5c98b88     	strb	r8, [r9, #0xb88]
    fcb4: e58351c0     	str	r5, [r3, #0x1c0]
    fcb8: e5835010     	str	r5, [r3, #0x10]
    fcbc: e58c5000     	str	r5, [r12]
    fcc0: ebffce98     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xc5a0
    fcc4: e3a0200f     	mov	r2, #15
    fcc8: e3a03901     	mov	r3, #16384
    fccc: e5c92b98     	strb	r2, [r9, #0xb98]
    fcd0: e34434c8     	movt	r3, #0x44c8
    fcd4: e5890d58     	str	r0, [r9, #0xd58]
    fcd8: e58631ac     	str	r3, [r6, #0x1ac]
    fcdc: e5c98d4c     	strb	r8, [r9, #0xd4c]
    fce0: e59d8010     	ldr	r8, [sp, #0x10]
    fce4: e58651c4     	str	r5, [r6, #0x1c4]
    fce8: e5865014     	str	r5, [r6, #0x14]
    fcec: e1a02008     	mov	r2, r8
    fcf0: e5d7111c     	ldrb	r1, [r7, #0x11c]
    fcf4: e2888001     	add	r8, r8, #1
    fcf8: e1a0000b     	mov	r0, r11
    fcfc: ebffcf9d     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0xc18c
    fd00: e3580010     	cmp	r8, #16
    fd04: e2877f71     	add	r7, r7, #452
    fd08: 1afffff7     	bne	0xfcec <arbhar_gpio_tilde_new+0xb8c> @ imm = #-0x24
    fd0c: e51f5660     	ldr	r5, [pc, #-0x660]       @ 0xf6b4 <arbhar_gpio_tilde_new+0x554>
    fd10: e284e083     	add	lr, r4, #131
    fd14: e51f0664     	ldr	r0, [pc, #-0x664]       @ 0xf6b8 <arbhar_gpio_tilde_new+0x558>
    fd18: e3a0a000     	mov	r10, #0
    fd1c: e08fb005     	add	r11, pc, r5
    fd20: e584e0b4     	str	lr, [r4, #0xb4]
    fd24: e5d41083     	ldrb	r1, [r4, #0x83]
    fd28: e08f0000     	add	r0, pc, r0
    fd2c: ebffcf91     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0xc1bc
    fd30: e1a0000b     	mov	r0, r11
    fd34: e589adc0     	str	r10, [r9, #0xdc0]
    fd38: e2845c1e     	add	r5, r4, #7680
    fd3c: ebffce79     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xc61c
    fd40: e51f668c     	ldr	r6, [pc, #-0x68c]       @ 0xf6bc <arbhar_gpio_tilde_new+0x55c>
    fd44: e28d8040     	add	r8, sp, #64
    fd48: e1a01000     	mov	r1, r0
    fd4c: e1a00004     	mov	r0, r4
    fd50: ebffcfa0     	bl	0x3bd8 <.plt+0x4dc>     @ imm = #-0xc180
    fd54: e5890dac     	str	r0, [r9, #0xdac]
    fd58: e1a0000b     	mov	r0, r11
    fd5c: ebffce71     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xc63c
    fd60: e1a01000     	mov	r1, r0
    fd64: e1a00004     	mov	r0, r4
    fd68: ebffcf9a     	bl	0x3bd8 <.plt+0x4dc>     @ imm = #-0xc198
    fd6c: e584006c     	str	r0, [r4, #0x6c]
    fd70: e1a0000b     	mov	r0, r11
    fd74: ebffce6b     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xc654
    fd78: e1a01000     	mov	r1, r0
    fd7c: e1a00004     	mov	r0, r4
    fd80: ebffcf94     	bl	0x3bd8 <.plt+0x4dc>     @ imm = #-0xc1b0
    fd84: e5840070     	str	r0, [r4, #0x70]
    fd88: e1a0000b     	mov	r0, r11
    fd8c: ebffce65     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xc66c
    fd90: e1a01000     	mov	r1, r0
    fd94: e1a00004     	mov	r0, r4
    fd98: ebffcf8e     	bl	0x3bd8 <.plt+0x4dc>     @ imm = #-0xc1c8
    fd9c: e5890db0     	str	r0, [r9, #0xdb0]
    fda0: e1a0000b     	mov	r0, r11
    fda4: ebffce5f     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xc684
    fda8: e1a01000     	mov	r1, r0
    fdac: e1a00004     	mov	r0, r4
    fdb0: ebffcf88     	bl	0x3bd8 <.plt+0x4dc>     @ imm = #-0xc1e0
    fdb4: e5890db4     	str	r0, [r9, #0xdb4]
    fdb8: e1a0000b     	mov	r0, r11
    fdbc: ebffce59     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xc69c
    fdc0: e28db074     	add	r11, sp, #116
    fdc4: e1a01000     	mov	r1, r0
    fdc8: e1a00004     	mov	r0, r4
    fdcc: ebffcf81     	bl	0x3bd8 <.plt+0x4dc>     @ imm = #-0xc1fc
    fdd0: eef00a68     	vmov.f32	s1, s17
    fdd4: eeb00a48     	vmov.f32	s0, s16
    fdd8: e5890db8     	str	r0, [r9, #0xdb8]
    fddc: e1a00004     	mov	r0, r4
    fde0: ebffcfd0     	bl	0x3d28 <.plt+0x62c>     @ imm = #-0xc0c0
    fde4: e3001dd8     	movw	r1, #0xdd8
    fde8: e189a0b1     	strh	r10, [r9, r1]
    fdec: e1a0200a     	mov	r2, r10
    fdf0: e3a01025     	mov	r1, #37
    fdf4: e1a00004     	mov	r0, r4
    fdf8: ebffcf3a     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0xc318
    fdfc: e3a02001     	mov	r2, #1
    fe00: e3a01028     	mov	r1, #40
    fe04: e1a00004     	mov	r0, r4
    fe08: ebffcf36     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0xc328
    fe0c: e3a0c001     	mov	r12, #1
    fe10: e3a02b02     	mov	r2, #2048
    fe14: e5c4c04c     	strb	r12, [r4, #0x4c]
    fe18: e3a01032     	mov	r1, #50
    fe1c: e1a00004     	mov	r0, r4
    fe20: ebffcf30     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0xc340
    fe24: e3a02b02     	mov	r2, #2048
    fe28: e3a01033     	mov	r1, #51
    fe2c: e1a00004     	mov	r0, r4
    fe30: ebffcf2c     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0xc350
    fe34: e3a02b02     	mov	r2, #2048
    fe38: e3a01034     	mov	r1, #52
    fe3c: e1a00004     	mov	r0, r4
    fe40: ebffcf28     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0xc360
    fe44: e3a02b02     	mov	r2, #2048
    fe48: e3a01035     	mov	r1, #53
    fe4c: e1a00004     	mov	r0, r4
    fe50: ebffcf24     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0xc370
    fe54: e1a0200a     	mov	r2, r10
    fe58: e3a0109e     	mov	r1, #158
    fe5c: e1a00004     	mov	r0, r4
    fe60: ebffcf20     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0xc380
    fe64: e1a00004     	mov	r0, r4
    fe68: ebffced9     	bl	0x39d4 <.plt+0x2d8>     @ imm = #-0xc49c
    fe6c: e08f2006     	add	r2, pc, r6
    fe70: e2423e66     	sub	r3, r2, #1632
    fe74: e2427e63     	sub	r7, r2, #1584
    fe78: e243e008     	sub	lr, r3, #8
    fe7c: e28d6054     	add	r6, sp, #84
    fe80: e285c008     	add	r12, r5, #8
    fe84: e247700c     	sub	r7, r7, #12
    fe88: e8be000f     	ldm	lr!, {r0, r1, r2, r3}
    fe8c: e1a05006     	mov	r5, r6
    fe90: e1a0a008     	mov	r10, r8
    fe94: e8a6000f     	stm	r6!, {r0, r1, r2, r3}
    fe98: e8be000f     	ldm	lr!, {r0, r1, r2, r3}
    fe9c: e8a6000f     	stm	r6!, {r0, r1, r2, r3}
    fea0: e89e0007     	ldm	lr, {r0, r1, r2}
    fea4: e3a0e00b     	mov	lr, #11
    fea8: e8860007     	stm	r6, {r0, r1, r2}
    feac: e8b7000f     	ldm	r7!, {r0, r1, r2, r3}
    feb0: e5976000     	ldr	r6, [r7]
    feb4: e8aa000f     	stm	r10!, {r0, r1, r2, r3}
    feb8: e3a00005     	mov	r0, #5
    febc: e58a6000     	str	r6, [r10]
    fec0: e3a01001     	mov	r1, #1
    fec4: e589eed0     	str	lr, [r9, #0xed0]
    fec8: e5891fa4     	str	r1, [r9, #0xfa4]
    fecc: e5890fa0     	str	r0, [r9, #0xfa0]
    fed0: e1a09005     	mov	r9, r5
    fed4: e28cc010     	add	r12, r12, #16
    fed8: e8b9000f     	ldm	r9!, {r0, r1, r2, r3}
    fedc: e2855010     	add	r5, r5, #16
    fee0: e159000b     	cmp	r9, r11
    fee4: e50c0010     	str	r0, [r12, #-0x10]
    fee8: e50c100c     	str	r1, [r12, #-0xc]
    feec: e50c2008     	str	r2, [r12, #-0x8]
    fef0: e50c3004     	str	r3, [r12, #-0x4]
    fef4: 1afffff5     	bne	0xfed0 <arbhar_gpio_tilde_new+0xd70> @ imm = #-0x2c
    fef8: e8b50007     	ldm	r5!, {r0, r1, r2}
    fefc: e3017ed4     	movw	r7, #0x1ed4
    ff00: e084e007     	add	lr, r4, r7
    ff04: e58c0000     	str	r0, [r12]
    ff08: e58c1004     	str	r1, [r12, #0x4]
    ff0c: e58c2008     	str	r2, [r12, #0x8]
    ff10: e8b8000f     	ldm	r8!, {r0, r1, r2, r3}
    ff14: e7840007     	str	r0, [r4, r7]
    ff18: e1a00004     	mov	r0, r4
    ff1c: e58e1004     	str	r1, [lr, #0x4]
    ff20: e58e2008     	str	r2, [lr, #0x8]
    ff24: e58e300c     	str	r3, [lr, #0xc]
    ff28: e5982000     	ldr	r2, [r8]
    ff2c: e58e2010     	str	r2, [lr, #0x10]
    ff30: e28dd084     	add	sp, sp, #132
    ff34: ecbd8b02     	vpop	{d8}
    ff38: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}

