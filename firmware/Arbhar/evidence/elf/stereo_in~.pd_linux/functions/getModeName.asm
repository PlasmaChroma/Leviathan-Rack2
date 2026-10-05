00002514 <getModeName>:
    2514: e3500008     	cmp	r0, #8
    2518: 908ff100     	addls	pc, pc, r0, lsl #2
    251c: ea000023     	b	0x25b0 <getModeName+0x9c> @ imm = #0x8c
    2520: ea00001c     	b	0x2598 <getModeName+0x84> @ imm = #0x70
    2524: ea00001e     	b	0x25a4 <getModeName+0x90> @ imm = #0x78
    2528: ea000005     	b	0x2544 <getModeName+0x30> @ imm = #0x14
    252c: ea000007     	b	0x2550 <getModeName+0x3c> @ imm = #0x1c
    2530: ea000009     	b	0x255c <getModeName+0x48> @ imm = #0x24
    2534: ea00000b     	b	0x2568 <getModeName+0x54> @ imm = #0x2c
    2538: ea00000d     	b	0x2574 <getModeName+0x60> @ imm = #0x34
    253c: ea00000f     	b	0x2580 <getModeName+0x6c> @ imm = #0x3c
    2540: ea000011     	b	0x258c <getModeName+0x78> @ imm = #0x44
    2544: e59f1068     	ldr	r1, [pc, #0x68]         @ 0x25b4 <getModeName+0xa0>
    2548: e08f0001     	add	r0, pc, r1
    254c: e12fff1e     	bx	lr
    2550: e59f0060     	ldr	r0, [pc, #0x60]         @ 0x25b8 <getModeName+0xa4>
    2554: e08f0000     	add	r0, pc, r0
    2558: e12fff1e     	bx	lr
    255c: e59fc058     	ldr	r12, [pc, #0x58]        @ 0x25bc <getModeName+0xa8>
    2560: e08f000c     	add	r0, pc, r12
    2564: e12fff1e     	bx	lr
    2568: e59f3050     	ldr	r3, [pc, #0x50]         @ 0x25c0 <getModeName+0xac>
    256c: e08f0003     	add	r0, pc, r3
    2570: e12fff1e     	bx	lr
    2574: e59f2048     	ldr	r2, [pc, #0x48]         @ 0x25c4 <getModeName+0xb0>
    2578: e08f0002     	add	r0, pc, r2
    257c: e12fff1e     	bx	lr
    2580: e59f1040     	ldr	r1, [pc, #0x40]         @ 0x25c8 <getModeName+0xb4>
    2584: e08f0001     	add	r0, pc, r1
    2588: e12fff1e     	bx	lr
    258c: e59f0038     	ldr	r0, [pc, #0x38]         @ 0x25cc <getModeName+0xb8>
    2590: e08f0000     	add	r0, pc, r0
    2594: e12fff1e     	bx	lr
    2598: e59f3030     	ldr	r3, [pc, #0x30]         @ 0x25d0 <getModeName+0xbc>
    259c: e08f0003     	add	r0, pc, r3
    25a0: e12fff1e     	bx	lr
    25a4: e59f2028     	ldr	r2, [pc, #0x28]         @ 0x25d4 <getModeName+0xc0>
    25a8: e08f0002     	add	r0, pc, r2
    25ac: e12fff1e     	bx	lr
    25b0: e12fff1e     	bx	lr
    25b4: f0 4f 00 00  	.word	0x00004ff0
    25b8: f4 4f 00 00  	.word	0x00004ff4
    25bc: f8 4f 00 00  	.word	0x00004ff8
    25c0: fc 4f 00 00  	.word	0x00004ffc
    25c4: 00 50 00 00  	.word	0x00005000
    25c8: 00 50 00 00  	.word	0x00005000
    25cc: 90 4f 00 00  	.word	0x00004f90
    25d0: 90 4f 00 00  	.word	0x00004f90
    25d4: e8 4f 00 00  	.word	0x00004fe8

