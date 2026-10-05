0000619c <arbhar_play_tilde_new>:
    619c: e59f3510     	ldr	r3, [pc, #0x510]        @ 0x66b4 <arbhar_play_tilde_new+0x518>
    61a0: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
    61a4: e08f0003     	add	r0, pc, r3
    61a8: e24dd09c     	sub	sp, sp, #156
    61ac: e1a0a001     	mov	r10, r1
    61b0: e5900020     	ldr	r0, [r0, #0x20]
    61b4: e24aa001     	sub	r10, r10, #1
    61b8: ebfff0ab     	bl	0x246c <.plt+0x8c>      @ imm = #-0x3d54
    61bc: e59f14f4     	ldr	r1, [pc, #0x4f4]        @ 0x66b8 <arbhar_play_tilde_new+0x51c>
    61c0: e3a09000     	mov	r9, #0
    61c4: e30b7b80     	movw	r7, #0xbb80
    61c8: e08f1001     	add	r1, pc, r1
    61cc: e3a06000     	mov	r6, #0
    61d0: e1a05009     	mov	r5, r9
    61d4: e3a045fe     	mov	r4, #1065353216
    61d8: e1a08000     	mov	r8, r0
    61dc: e280ba02     	add	r11, r0, #8192
    61e0: ebfff0dd     	bl	0x255c <.plt+0x17c>     @ imm = #-0x3c8c
    61e4: e59f24d0     	ldr	r2, [pc, #0x4d0]        @ 0x66bc <arbhar_play_tilde_new+0x520>
    61e8: e59fc4d0     	ldr	r12, [pc, #0x4d0]       @ 0x66c0 <arbhar_play_tilde_new+0x524>
    61ec: e08f1002     	add	r1, pc, r2
    61f0: e08f300c     	add	r3, pc, r12
    61f4: e58d3004     	str	r3, [sp, #0x4]
    61f8: e588001c     	str	r0, [r8, #0x1c]
    61fc: e1a00008     	mov	r0, r8
    6200: ebfff0d5     	bl	0x255c <.plt+0x17c>     @ imm = #-0x3cac
    6204: e59f34b8     	ldr	r3, [pc, #0x4b8]        @ 0x66c4 <arbhar_play_tilde_new+0x528>
    6208: e08f1003     	add	r1, pc, r3
    620c: e5880020     	str	r0, [r8, #0x20]
    6210: e1a00008     	mov	r0, r8
    6214: ebfff0d0     	bl	0x255c <.plt+0x17c>     @ imm = #-0x3cc0
    6218: e59f14a8     	ldr	r1, [pc, #0x4a8]        @ 0x66c8 <arbhar_play_tilde_new+0x52c>
    621c: e08f1001     	add	r1, pc, r1
    6220: e5880024     	str	r0, [r8, #0x24]
    6224: e1a00008     	mov	r0, r8
    6228: ebfff0cb     	bl	0x255c <.plt+0x17c>     @ imm = #-0x3cd4
    622c: e59f2498     	ldr	r2, [pc, #0x498]        @ 0x66cc <arbhar_play_tilde_new+0x530>
    6230: e08f1002     	add	r1, pc, r2
    6234: e5880028     	str	r0, [r8, #0x28]
    6238: e1a00008     	mov	r0, r8
    623c: ebfff0c6     	bl	0x255c <.plt+0x17c>     @ imm = #-0x3ce8
    6240: e3a02000     	mov	r2, #0
    6244: e3a03101     	mov	r3, #1073741824
    6248: e588002c     	str	r0, [r8, #0x2c]
    624c: e59f047c     	ldr	r0, [pc, #0x47c]        @ 0x66d0 <arbhar_play_tilde_new+0x534>
    6250: e08f0000     	add	r0, pc, r0
    6254: ebfff0f0     	bl	0x261c <.plt+0x23c>     @ imm = #-0x3c40
    6258: e59fc474     	ldr	r12, [pc, #0x474]       @ 0x66d4 <arbhar_play_tilde_new+0x538>
    625c: e3a01004     	mov	r1, #4
    6260: e08f000c     	add	r0, pc, r12
    6264: ebfff0ec     	bl	0x261c <.plt+0x23c>     @ imm = #-0x3c50
    6268: e15a0009     	cmp	r10, r9
    626c: e1a01009     	mov	r1, r9
    6270: eddf0afd     	vldr	s1, [pc, #1012]         @ 0x666c <arbhar_play_tilde_new+0x4d0>
    6274: d3a0a002     	movle	r10, #2
    6278: e1a00008     	mov	r0, r8
    627c: e588a09c     	str	r10, [r8, #0x9c]
    6280: e3a0a001     	mov	r10, #1
    6284: e58b9894     	str	r9, [r11, #0x894]
    6288: ed9f0af8     	vldr	s0, [pc, #992]          @ 0x6670 <arbhar_play_tilde_new+0x4d4>
    628c: ebfff0c4     	bl	0x25a4 <.plt+0x1c4>     @ imm = #-0x3cf0
    6290: e1a00008     	mov	r0, r8
    6294: e3a01001     	mov	r1, #1
    6298: eddf0af5     	vldr	s1, [pc, #980]          @ 0x6674 <arbhar_play_tilde_new+0x4d8>
    629c: ed9f0af5     	vldr	s0, [pc, #980]          @ 0x6678 <arbhar_play_tilde_new+0x4dc>
    62a0: ebfff0bf     	bl	0x25a4 <.plt+0x1c4>     @ imm = #-0x3d04
    62a4: e1a00008     	mov	r0, r8
    62a8: e3a01002     	mov	r1, #2
    62ac: eddf0af0     	vldr	s1, [pc, #960]          @ 0x6674 <arbhar_play_tilde_new+0x4d8>
    62b0: ed9f0af1     	vldr	s0, [pc, #964]          @ 0x667c <arbhar_play_tilde_new+0x4e0>
    62b4: ebfff0ba     	bl	0x25a4 <.plt+0x1c4>     @ imm = #-0x3d18
    62b8: e1a00008     	mov	r0, r8
    62bc: e3a01003     	mov	r1, #3
    62c0: eddf0aeb     	vldr	s1, [pc, #940]          @ 0x6674 <arbhar_play_tilde_new+0x4d8>
    62c4: ed9f0aed     	vldr	s0, [pc, #948]          @ 0x6680 <arbhar_play_tilde_new+0x4e4>
    62c8: ebfff0b5     	bl	0x25a4 <.plt+0x1c4>     @ imm = #-0x3d2c
    62cc: e1a00008     	mov	r0, r8
    62d0: e3a01004     	mov	r1, #4
    62d4: eddf0ae6     	vldr	s1, [pc, #920]          @ 0x6674 <arbhar_play_tilde_new+0x4d8>
    62d8: ed9f0ae9     	vldr	s0, [pc, #932]          @ 0x6684 <arbhar_play_tilde_new+0x4e8>
    62dc: ebfff0b0     	bl	0x25a4 <.plt+0x1c4>     @ imm = #-0x3d40
    62e0: e1a00008     	mov	r0, r8
    62e4: e3a01005     	mov	r1, #5
    62e8: eddf0ae1     	vldr	s1, [pc, #900]          @ 0x6674 <arbhar_play_tilde_new+0x4d8>
    62ec: ed9f0ae5     	vldr	s0, [pc, #916]          @ 0x6688 <arbhar_play_tilde_new+0x4ec>
    62f0: ebfff0ab     	bl	0x25a4 <.plt+0x1c4>     @ imm = #-0x3d54
    62f4: e1a00008     	mov	r0, r8
    62f8: e3a01006     	mov	r1, #6
    62fc: eddf0adc     	vldr	s1, [pc, #880]          @ 0x6674 <arbhar_play_tilde_new+0x4d8>
    6300: ed9f0ae1     	vldr	s0, [pc, #900]          @ 0x668c <arbhar_play_tilde_new+0x4f0>
    6304: ebfff0a6     	bl	0x25a4 <.plt+0x1c4>     @ imm = #-0x3d68
    6308: e1a00008     	mov	r0, r8
    630c: e3a01007     	mov	r1, #7
    6310: eddf0ad7     	vldr	s1, [pc, #860]          @ 0x6674 <arbhar_play_tilde_new+0x4d8>
    6314: ed9f0add     	vldr	s0, [pc, #884]          @ 0x6690 <arbhar_play_tilde_new+0x4f4>
    6318: ebfff0a1     	bl	0x25a4 <.plt+0x1c4>     @ imm = #-0x3d7c
    631c: e1a00008     	mov	r0, r8
    6320: e3a01008     	mov	r1, #8
    6324: eddf0ad2     	vldr	s1, [pc, #840]          @ 0x6674 <arbhar_play_tilde_new+0x4d8>
    6328: ed9f0ad9     	vldr	s0, [pc, #868]          @ 0x6694 <arbhar_play_tilde_new+0x4f8>
    632c: ebfff09c     	bl	0x25a4 <.plt+0x1c4>     @ imm = #-0x3d90
    6330: e1a00008     	mov	r0, r8
    6334: e3a01009     	mov	r1, #9
    6338: eddf0acd     	vldr	s1, [pc, #820]          @ 0x6674 <arbhar_play_tilde_new+0x4d8>
    633c: ed9f0ad5     	vldr	s0, [pc, #852]          @ 0x6698 <arbhar_play_tilde_new+0x4fc>
    6340: ebfff097     	bl	0x25a4 <.plt+0x1c4>     @ imm = #-0x3da4
    6344: e1a00008     	mov	r0, r8
    6348: e3a0100a     	mov	r1, #10
    634c: eddf0ac8     	vldr	s1, [pc, #800]          @ 0x6674 <arbhar_play_tilde_new+0x4d8>
    6350: ed9f0ad1     	vldr	s0, [pc, #836]          @ 0x669c <arbhar_play_tilde_new+0x500>
    6354: ebfff092     	bl	0x25a4 <.plt+0x1c4>     @ imm = #-0x3db8
    6358: e1a00008     	mov	r0, r8
    635c: e3a0100b     	mov	r1, #11
    6360: eddf0ac3     	vldr	s1, [pc, #780]          @ 0x6674 <arbhar_play_tilde_new+0x4d8>
    6364: ed9f0acd     	vldr	s0, [pc, #820]          @ 0x66a0 <arbhar_play_tilde_new+0x504>
    6368: ebfff08d     	bl	0x25a4 <.plt+0x1c4>     @ imm = #-0x3dcc
    636c: e3a0100c     	mov	r1, #12
    6370: e1a00008     	mov	r0, r8
    6374: eddf0abe     	vldr	s1, [pc, #760]          @ 0x6674 <arbhar_play_tilde_new+0x4d8>
    6378: ed9f0ac9     	vldr	s0, [pc, #804]          @ 0x66a4 <arbhar_play_tilde_new+0x508>
    637c: ebfff088     	bl	0x25a4 <.plt+0x1c4>     @ imm = #-0x3de0
    6380: e1a00009     	mov	r0, r9
    6384: ebfff04a     	bl	0x24b4 <.plt+0xd4>      @ imm = #-0x3ed8
    6388: ebfff08b     	bl	0x25bc <.plt+0x1dc>     @ imm = #-0x3dd4
    638c: ebfff04b     	bl	0x24c0 <.plt+0xe0>      @ imm = #-0x3ed4
    6390: e59f0340     	ldr	r0, [pc, #0x340]        @ 0x66d8 <arbhar_play_tilde_new+0x53c>
    6394: e28b2e85     	add	r2, r11, #2128
    6398: e3003203     	movw	r3, #0x203
    639c: e58830d4     	str	r3, [r8, #0xd4]
    63a0: e08f3000     	add	r3, pc, r0
    63a4: e3a0c101     	mov	r12, #1073741824
    63a8: e3a01024     	mov	r1, #36
    63ac: e28d0030     	add	r0, sp, #48
    63b0: e58b16c8     	str	r1, [r11, #0x6c8]
    63b4: e588c0b0     	str	r12, [r8, #0xb0]
    63b8: e2431fe5     	sub	r1, r3, #916
    63bc: e58890ac     	str	r9, [r8, #0xac]
    63c0: e588a0a8     	str	r10, [r8, #0xa8]
    63c4: e58b9630     	str	r9, [r11, #0x630]
    63c8: e28b9e63     	add	r9, r11, #1584
    63cc: e58870a4     	str	r7, [r8, #0xa4]
    63d0: e5826008     	str	r6, [r2, #0x8]
    63d4: e3a02068     	mov	r2, #104
    63d8: ebfff02c     	bl	0x2490 <.plt+0xb0>      @ imm = #-0x3f50
    63dc: e3a02068     	mov	r2, #104
    63e0: e1a01000     	mov	r1, r0
    63e4: e2890008     	add	r0, r9, #8
    63e8: ebfff028     	bl	0x2490 <.plt+0xb0>      @ imm = #-0x3f60
    63ec: e28830e8     	add	r3, r8, #232
    63f0: e3a0e002     	mov	lr, #2
    63f4: e3a0c00c     	mov	r12, #12
    63f8: e58d8008     	str	r8, [sp, #0x8]
    63fc: e58bc6a0     	str	r12, [r11, #0x6a0]
    6400: e5885104     	str	r5, [r8, #0x104]
    6404: e5887108     	str	r7, [r8, #0x108]
    6408: e5886124     	str	r6, [r8, #0x124]
    640c: e588512c     	str	r5, [r8, #0x12c]
    6410: e588513c     	str	r5, [r8, #0x13c]
    6414: e5885158     	str	r5, [r8, #0x158]
    6418: e588411c     	str	r4, [r8, #0x11c]
    641c: e5884120     	str	r4, [r8, #0x120]
    6420: e5884118     	str	r4, [r8, #0x118]
    6424: e5884154     	str	r4, [r8, #0x154]
    6428: e588a178     	str	r10, [r8, #0x178]
    642c: e588717c     	str	r7, [r8, #0x17c]
    6430: e5886198     	str	r6, [r8, #0x198]
    6434: e58851a0     	str	r5, [r8, #0x1a0]
    6438: e58851b0     	str	r5, [r8, #0x1b0]
    643c: e5884190     	str	r4, [r8, #0x190]
    6440: e58851cc     	str	r5, [r8, #0x1cc]
    6444: e5884194     	str	r4, [r8, #0x194]
    6448: e588418c     	str	r4, [r8, #0x18c]
    644c: e58841c8     	str	r4, [r8, #0x1c8]
    6450: e583e104     	str	lr, [r3, #0x104]
    6454: e28e8001     	add	r8, lr, #1
    6458: e28ea003     	add	r10, lr, #3
    645c: e28e9004     	add	r9, lr, #4
    6460: e28ee005     	add	lr, lr, #5
    6464: e2832e1d     	add	r2, r3, #464
    6468: e35e0052     	cmp	lr, #82
    646c: e5837108     	str	r7, [r3, #0x108]
    6470: e5836124     	str	r6, [r3, #0x124]
    6474: e2880001     	add	r0, r8, #1
    6478: e583512c     	str	r5, [r3, #0x12c]
    647c: e2833f91     	add	r3, r3, #580
    6480: e5035108     	str	r5, [r3, #-0x108]
    6484: e5034128     	str	r4, [r3, #-0x128]
    6488: e50350ec     	str	r5, [r3, #-0xec]
    648c: e5034124     	str	r4, [r3, #-0x124]
    6490: e503412c     	str	r4, [r3, #-0x12c]
    6494: e50340f0     	str	r4, [r3, #-0xf0]
    6498: e50380cc     	str	r8, [r3, #-0xcc]
    649c: e50370c8     	str	r7, [r3, #-0xc8]
    64a0: e50360ac     	str	r6, [r3, #-0xac]
    64a4: e50350a4     	str	r5, [r3, #-0xa4]
    64a8: e5035094     	str	r5, [r3, #-0x94]
    64ac: e50340b4     	str	r4, [r3, #-0xb4]
    64b0: e5035078     	str	r5, [r3, #-0x78]
    64b4: e50340b0     	str	r4, [r3, #-0xb0]
    64b8: e50340b8     	str	r4, [r3, #-0xb8]
    64bc: e503407c     	str	r4, [r3, #-0x7c]
    64c0: e5030058     	str	r0, [r3, #-0x58]
    64c4: e5037054     	str	r7, [r3, #-0x54]
    64c8: e5036038     	str	r6, [r3, #-0x38]
    64cc: e5035030     	str	r5, [r3, #-0x30]
    64d0: e5035020     	str	r5, [r3, #-0x20]
    64d4: e5034040     	str	r4, [r3, #-0x40]
    64d8: e5035004     	str	r5, [r3, #-0x4]
    64dc: e503403c     	str	r4, [r3, #-0x3c]
    64e0: e5034044     	str	r4, [r3, #-0x44]
    64e4: e5034008     	str	r4, [r3, #-0x8]
    64e8: e583a01c     	str	r10, [r3, #0x1c]
    64ec: e5837020     	str	r7, [r3, #0x20]
    64f0: e583603c     	str	r6, [r3, #0x3c]
    64f4: e5835044     	str	r5, [r3, #0x44]
    64f8: e5835054     	str	r5, [r3, #0x54]
    64fc: e5834034     	str	r4, [r3, #0x34]
    6500: e5835070     	str	r5, [r3, #0x70]
    6504: e5834038     	str	r4, [r3, #0x38]
    6508: e5834030     	str	r4, [r3, #0x30]
    650c: e583406c     	str	r4, [r3, #0x6c]
    6510: e5839090     	str	r9, [r3, #0x90]
    6514: e5827108     	str	r7, [r2, #0x108]
    6518: e5826124     	str	r6, [r2, #0x124]
    651c: e582512c     	str	r5, [r2, #0x12c]
    6520: e582513c     	str	r5, [r2, #0x13c]
    6524: e582411c     	str	r4, [r2, #0x11c]
    6528: e5825158     	str	r5, [r2, #0x158]
    652c: e5824120     	str	r4, [r2, #0x120]
    6530: e5824118     	str	r4, [r2, #0x118]
    6534: e5824154     	str	r4, [r2, #0x154]
    6538: 1affffc4     	bne	0x6450 <arbhar_play_tilde_new+0x2b4> @ imm = #-0xf0
    653c: ed9f1a59     	vldr	s2, [pc, #356]          @ 0x66a8 <arbhar_play_tilde_new+0x50c>
    6540: e3a07001     	mov	r7, #1
    6544: e59d8008     	ldr	r8, [sp, #0x8]
    6548: e59f118c     	ldr	r1, [pc, #0x18c]        @ 0x66dc <arbhar_play_tilde_new+0x540>
    654c: e58b56d8     	str	r5, [r11, #0x6d8]
    6550: e58b56dc     	str	r5, [r11, #0x6dc]
    6554: e1a00008     	mov	r0, r8
    6558: e58840c4     	str	r4, [r8, #0xc4]
    655c: eeb00a41     	vmov.f32	s0, s2
    6560: e58860c8     	str	r6, [r8, #0xc8]
    6564: e08f9001     	add	r9, pc, r1
    6568: e58840cc     	str	r4, [r8, #0xcc]
    656c: eddf0a4e     	vldr	s1, [pc, #312]          @ 0x66ac <arbhar_play_tilde_new+0x510>
    6570: ebffefd5     	bl	0x24cc <.plt+0xec>      @ imm = #-0x40ac
    6574: e1a00008     	mov	r0, r8
    6578: ed981a33     	vldr	s2, [r8, #204]
    657c: eddf0a4b     	vldr	s1, [pc, #300]          @ 0x66b0 <arbhar_play_tilde_new+0x514>
    6580: ed9f0a48     	vldr	s0, [pc, #288]          @ 0x66a8 <arbhar_play_tilde_new+0x50c>
    6584: ebffefd0     	bl	0x24cc <.plt+0xec>      @ imm = #-0x40c0
    6588: e59fc150     	ldr	r12, [pc, #0x150]       @ 0x66e0 <arbhar_play_tilde_new+0x544>
    658c: e59da004     	ldr	r10, [sp, #0x4]
    6590: e1a00008     	mov	r0, r8
    6594: e58850b4     	str	r5, [r8, #0xb4]
    6598: e58870bc     	str	r7, [r8, #0xbc]
    659c: e58b56b4     	str	r5, [r11, #0x6b4]
    65a0: e58840c0     	str	r4, [r8, #0xc0]
    65a4: e58860dc     	str	r6, [r8, #0xdc]
    65a8: e58b562c     	str	r5, [r11, #0x62c]
    65ac: e79aa00c     	ldr	r10, [r10, r12]
    65b0: e1a0100a     	mov	r1, r10
    65b4: ebfff02a     	bl	0x2664 <.plt+0x284>     @ imm = #-0x3f58
    65b8: e1a0100a     	mov	r1, r10
    65bc: e58800e0     	str	r0, [r8, #0xe0]
    65c0: e1a00008     	mov	r0, r8
    65c4: ebfff026     	bl	0x2664 <.plt+0x284>     @ imm = #-0x3f68
    65c8: e1a0100a     	mov	r1, r10
    65cc: e58800e4     	str	r0, [r8, #0xe4]
    65d0: e1a00008     	mov	r0, r8
    65d4: ebfff022     	bl	0x2664 <.plt+0x284>     @ imm = #-0x3f78
    65d8: e1a0100a     	mov	r1, r10
    65dc: e58800e8     	str	r0, [r8, #0xe8]
    65e0: e1a00008     	mov	r0, r8
    65e4: ebfff01e     	bl	0x2664 <.plt+0x284>     @ imm = #-0x3f88
    65e8: e59d2004     	ldr	r2, [sp, #0x4]
    65ec: e59f30f0     	ldr	r3, [pc, #0xf0]         @ 0x66e4 <arbhar_play_tilde_new+0x548>
    65f0: e58800ec     	str	r0, [r8, #0xec]
    65f4: e1a00008     	mov	r0, r8
    65f8: e792a003     	ldr	r10, [r2, r3]
    65fc: e1a0100a     	mov	r1, r10
    6600: ebfff017     	bl	0x2664 <.plt+0x284>     @ imm = #-0x3fa4
    6604: e58800f0     	str	r0, [r8, #0xf0]
    6608: e1a00009     	mov	r0, r9
    660c: ebffef78     	bl	0x23f4 <.plt+0x14>      @ imm = #-0x4220
    6610: e1a01000     	mov	r1, r0
    6614: e1a00008     	mov	r0, r8
    6618: ebfff011     	bl	0x2664 <.plt+0x284>     @ imm = #-0x3fbc
    661c: e1a0100a     	mov	r1, r10
    6620: e58800f4     	str	r0, [r8, #0xf4]
    6624: e1a00008     	mov	r0, r8
    6628: ebfff00d     	bl	0x2664 <.plt+0x284>     @ imm = #-0x3fcc
    662c: e58800f8     	str	r0, [r8, #0xf8]
    6630: e1a00009     	mov	r0, r9
    6634: ebffef6e     	bl	0x23f4 <.plt+0x14>      @ imm = #-0x4248
    6638: e28b9e6a     	add	r9, r11, #1696
    663c: e1a01000     	mov	r1, r0
    6640: e1a00008     	mov	r0, r8
    6644: ebfff006     	bl	0x2664 <.plt+0x284>     @ imm = #-0x3fe8
    6648: e1a0100a     	mov	r1, r10
    664c: e58800fc     	str	r0, [r8, #0xfc]
    6650: e1a00008     	mov	r0, r8
    6654: ebfff002     	bl	0x2664 <.plt+0x284>     @ imm = #-0x3ff8
    6658: e2893004     	add	r3, r9, #4
    665c: e58d3004     	str	r3, [sp, #0x4]
    6660: e28bee63     	add	lr, r11, #1584
    6664: e1a01009     	mov	r1, r9
    6668: ea00001e     	b	0x66e8 <arbhar_play_tilde_new+0x54c> @ imm = #0x78
    666c: 00 00 7a 43  	.word	0x437a0000
    6670: 00 5b 6e 47  	.word	0x476e5b00
    6674: 00 58 18 49  	.word	0x49185800
    6678: 80 cc b0 47  	.word	0x47b0cc80
    667c: 00 cd b0 47  	.word	0x47b0cd00
    6680: 80 cd b0 47  	.word	0x47b0cd80
    6684: 00 ce b0 47  	.word	0x47b0ce00
    6688: 80 ce b0 47  	.word	0x47b0ce80
    668c: 00 cf b0 47  	.word	0x47b0cf00
    6690: 80 cf b0 47  	.word	0x47b0cf80
    6694: 00 d0 b0 47  	.word	0x47b0d000
    6698: 80 d0 b0 47  	.word	0x47b0d080
    669c: 00 d1 b0 47  	.word	0x47b0d100
    66a0: 80 d1 b0 47  	.word	0x47b0d180
    66a4: 00 d2 b0 47  	.word	0x47b0d200
    66a8: 00 00 00 00  	.word	0x00000000
    66ac: 00 00 70 42  	.word	0x42700000
    66b0: 00 00 74 42  	.word	0x42740000
    66b4: f0 7f 01 00  	.word	0x00017ff0
    66b8: 70 c6 ff ff  	.word	0xffffc670
    66bc: 60 18 00 00  	.word	0x00001860
    66c0: 08 7e 01 00  	.word	0x00017e08
    66c4: 58 d5 ff ff  	.word	0xffffd558
    66c8: 04 17 00 00  	.word	0x00001704
    66cc: 90 18 00 00  	.word	0x00001890
    66d0: a4 6a 00 00  	.word	0x00006aa4
    66d4: ac 6a 00 00  	.word	0x00006aac
    66d8: 04 6a 00 00  	.word	0x00006a04
    66dc: b8 67 00 00  	.word	0x000067b8
    66e0: 40 01 00 00  	.word	0x00000140
    66e4: 3c 01 00 00  	.word	0x0000013c
    66e8: e1a0c009     	mov	r12, r9
    66ec: e28b2e75     	add	r2, r11, #1872
    66f0: e28b9e75     	add	r9, r11, #1872
    66f4: e28b3e6b     	add	r3, r11, #1712
    66f8: e282a004     	add	r10, r2, #4
    66fc: e289200c     	add	r2, r9, #12
    6700: e58da008     	str	r10, [sp, #0x8]
    6704: e58d200c     	str	r2, [sp, #0xc]
    6708: e28bae76     	add	r10, r11, #1888
    670c: e28b2e76     	add	r2, r11, #1888
    6710: e28a9008     	add	r9, r10, #8
    6714: e282a00c     	add	r10, r2, #12
    6718: e58d9010     	str	r9, [sp, #0x10]
    671c: e58da014     	str	r10, [sp, #0x14]
    6720: e28b9e77     	add	r9, r11, #1904
    6724: e28bae77     	add	r10, r11, #1904
    6728: e2892004     	add	r2, r9, #4
    672c: e28a9008     	add	r9, r10, #8
    6730: e58d2018     	str	r2, [sp, #0x18]
    6734: e58d901c     	str	r9, [sp, #0x1c]
    6738: e28b2d1e     	add	r2, r11, #1920
    673c: e28b9d1e     	add	r9, r11, #1920
    6740: e282a004     	add	r10, r2, #4
    6744: e289200c     	add	r2, r9, #12
    6748: e58da020     	str	r10, [sp, #0x20]
    674c: e58d2024     	str	r2, [sp, #0x24]
    6750: e28bae79     	add	r10, r11, #1936
    6754: e28b2e79     	add	r2, r11, #1936
    6758: e28a9008     	add	r9, r10, #8
    675c: e282a00c     	add	r10, r2, #12
    6760: e58d9028     	str	r9, [sp, #0x28]
    6764: e58da02c     	str	r10, [sp, #0x2c]
    6768: e28b2e76     	add	r2, r11, #1888
    676c: e28bae7a     	add	r10, r11, #1952
    6770: e28ee004     	add	lr, lr, #4
    6774: e1a0900a     	mov	r9, r10
    6778: e5880100     	str	r0, [r8, #0x100]
    677c: e59d0004     	ldr	r0, [sp, #0x4]
    6780: e58e6000     	str	r6, [lr]
    6784: e5804000     	str	r4, [r0]
    6788: e5814008     	str	r4, [r1, #0x8]
    678c: e58c400c     	str	r4, [r12, #0xc]
    6790: e59dc008     	ldr	r12, [sp, #0x8]
    6794: e5836000     	str	r6, [r3]
    6798: e5834008     	str	r4, [r3, #0x8]
    679c: e3a04c53     	mov	r4, #21248
    67a0: e58b56cc     	str	r5, [r11, #0x6cc]
    67a4: e3404007     	movt	r4, #0x7
    67a8: e58b76bc     	str	r7, [r11, #0x6bc]
    67ac: e588403c     	str	r4, [r8, #0x3c]
    67b0: e5884040     	str	r4, [r8, #0x40]
    67b4: e5884044     	str	r4, [r8, #0x44]
    67b8: e5884048     	str	r4, [r8, #0x48]
    67bc: e588404c     	str	r4, [r8, #0x4c]
    67c0: e5884050     	str	r4, [r8, #0x50]
    67c4: e2884d9e     	add	r4, r8, #10112
    67c8: e58b56e8     	str	r5, [r11, #0x6e8]
    67cc: e28b5e75     	add	r5, r11, #1872
    67d0: e5856000     	str	r6, [r5]
    67d4: e28b5e79     	add	r5, r11, #1936
    67d8: e58c6000     	str	r6, [r12]
    67dc: e58b7758     	str	r7, [r11, #0x758]
    67e0: e59d300c     	ldr	r3, [sp, #0xc]
    67e4: e59d0010     	ldr	r0, [sp, #0x10]
    67e8: e59d1014     	ldr	r1, [sp, #0x14]
    67ec: e5836000     	str	r6, [r3]
    67f0: e59dc01c     	ldr	r12, [sp, #0x1c]
    67f4: e5826000     	str	r6, [r2]
    67f8: e58b7764     	str	r7, [r11, #0x764]
    67fc: e5806000     	str	r6, [r0]
    6800: e1a00008     	mov	r0, r8
    6804: e59d8018     	ldr	r8, [sp, #0x18]
    6808: e5816000     	str	r6, [r1]
    680c: e59d3020     	ldr	r3, [sp, #0x20]
    6810: e58b7770     	str	r7, [r11, #0x770]
    6814: e59d2024     	ldr	r2, [sp, #0x24]
    6818: e5886000     	str	r6, [r8]
    681c: e59d1028     	ldr	r1, [sp, #0x28]
    6820: e58c6000     	str	r6, [r12]
    6824: e58b777c     	str	r7, [r11, #0x77c]
    6828: e5846000     	str	r6, [r4]
    682c: e59d402c     	ldr	r4, [sp, #0x2c]
    6830: e5836000     	str	r6, [r3]
    6834: e58b7788     	str	r7, [r11, #0x788]
    6838: e5826000     	str	r6, [r2]
    683c: e5856000     	str	r6, [r5]
    6840: e58b7794     	str	r7, [r11, #0x794]
    6844: e5816000     	str	r6, [r1]
    6848: e5846000     	str	r6, [r4]
    684c: e58b77a0     	str	r7, [r11, #0x7a0]
    6850: e58a6004     	str	r6, [r10, #0x4]
    6854: e5896008     	str	r6, [r9, #0x8]
    6858: e58b77ac     	str	r7, [r11, #0x7ac]
    685c: e58b7850     	str	r7, [r11, #0x850]
    6860: e28dd09c     	add	sp, sp, #156
    6864: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}

