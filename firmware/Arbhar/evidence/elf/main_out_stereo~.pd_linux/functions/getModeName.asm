0000253c <getModeName>:
    253c: e3500008     	cmp	r0, #8
    2540: 908ff100     	addls	pc, pc, r0, lsl #2
    2544: ea000023     	b	0x25d8 <getModeName+0x9c> @ imm = #0x8c
    2548: ea00001c     	b	0x25c0 <getModeName+0x84> @ imm = #0x70
    254c: ea00001e     	b	0x25cc <getModeName+0x90> @ imm = #0x78
    2550: ea000005     	b	0x256c <getModeName+0x30> @ imm = #0x14
    2554: ea000007     	b	0x2578 <getModeName+0x3c> @ imm = #0x1c
    2558: ea000009     	b	0x2584 <getModeName+0x48> @ imm = #0x24
    255c: ea00000b     	b	0x2590 <getModeName+0x54> @ imm = #0x2c
    2560: ea00000d     	b	0x259c <getModeName+0x60> @ imm = #0x34
    2564: ea00000f     	b	0x25a8 <getModeName+0x6c> @ imm = #0x3c
    2568: ea000011     	b	0x25b4 <getModeName+0x78> @ imm = #0x44
    256c: e59f1068     	ldr	r1, [pc, #0x68]         @ 0x25dc <getModeName+0xa0>
    2570: e08f0001     	add	r0, pc, r1
    2574: e12fff1e     	bx	lr
    2578: e59f0060     	ldr	r0, [pc, #0x60]         @ 0x25e0 <getModeName+0xa4>
    257c: e08f0000     	add	r0, pc, r0
    2580: e12fff1e     	bx	lr
    2584: e59fc058     	ldr	r12, [pc, #0x58]        @ 0x25e4 <getModeName+0xa8>
    2588: e08f000c     	add	r0, pc, r12
    258c: e12fff1e     	bx	lr
    2590: e59f3050     	ldr	r3, [pc, #0x50]         @ 0x25e8 <getModeName+0xac>
    2594: e08f0003     	add	r0, pc, r3
    2598: e12fff1e     	bx	lr
    259c: e59f2048     	ldr	r2, [pc, #0x48]         @ 0x25ec <getModeName+0xb0>
    25a0: e08f0002     	add	r0, pc, r2
    25a4: e12fff1e     	bx	lr
    25a8: e59f1040     	ldr	r1, [pc, #0x40]         @ 0x25f0 <getModeName+0xb4>
    25ac: e08f0001     	add	r0, pc, r1
    25b0: e12fff1e     	bx	lr
    25b4: e59f0038     	ldr	r0, [pc, #0x38]         @ 0x25f4 <getModeName+0xb8>
    25b8: e08f0000     	add	r0, pc, r0
    25bc: e12fff1e     	bx	lr
    25c0: e59f3030     	ldr	r3, [pc, #0x30]         @ 0x25f8 <getModeName+0xbc>
    25c4: e08f0003     	add	r0, pc, r3
    25c8: e12fff1e     	bx	lr
    25cc: e59f2028     	ldr	r2, [pc, #0x28]         @ 0x25fc <getModeName+0xc0>
    25d0: e08f0002     	add	r0, pc, r2
    25d4: e12fff1e     	bx	lr
    25d8: e12fff1e     	bx	lr
    25dc: 18 4c 00 00  	.word	0x00004c18
    25e0: 1c 4c 00 00  	.word	0x00004c1c
    25e4: 20 4c 00 00  	.word	0x00004c20
    25e8: 24 4c 00 00  	.word	0x00004c24
    25ec: 28 4c 00 00  	.word	0x00004c28
    25f0: 28 4c 00 00  	.word	0x00004c28
    25f4: b8 4b 00 00  	.word	0x00004bb8
    25f8: b8 4b 00 00  	.word	0x00004bb8
    25fc: 10 4c 00 00  	.word	0x00004c10

