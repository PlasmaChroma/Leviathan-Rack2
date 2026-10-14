00002594 <getModeName>:
    2594: e3500008     	cmp	r0, #8
    2598: 908ff100     	addls	pc, pc, r0, lsl #2
    259c: ea000023     	b	0x2630 <getModeName+0x9c> @ imm = #0x8c
    25a0: ea00001c     	b	0x2618 <getModeName+0x84> @ imm = #0x70
    25a4: ea00001e     	b	0x2624 <getModeName+0x90> @ imm = #0x78
    25a8: ea000005     	b	0x25c4 <getModeName+0x30> @ imm = #0x14
    25ac: ea000007     	b	0x25d0 <getModeName+0x3c> @ imm = #0x1c
    25b0: ea000009     	b	0x25dc <getModeName+0x48> @ imm = #0x24
    25b4: ea00000b     	b	0x25e8 <getModeName+0x54> @ imm = #0x2c
    25b8: ea00000d     	b	0x25f4 <getModeName+0x60> @ imm = #0x34
    25bc: ea00000f     	b	0x2600 <getModeName+0x6c> @ imm = #0x3c
    25c0: ea000011     	b	0x260c <getModeName+0x78> @ imm = #0x44
    25c4: e59f1068     	ldr	r1, [pc, #0x68]         @ 0x2634 <getModeName+0xa0>  // u32=0x4c10; f32?=2.72860837e-41
    25c8: e08f0001     	add	r0, pc, r1
    25cc: e12fff1e     	bx	lr
    25d0: e59f0060     	ldr	r0, [pc, #0x60]         @ 0x2638 <getModeName+0xa4>  // u32=0x4c14; f32?=2.72916889e-41
    25d4: e08f0000     	add	r0, pc, r0
    25d8: e12fff1e     	bx	lr
    25dc: e59fc058     	ldr	r12, [pc, #0x58]        @ 0x263c <getModeName+0xa8>  // u32=0x4c18; f32?=2.72972941e-41
    25e0: e08f000c     	add	r0, pc, r12
    25e4: e12fff1e     	bx	lr
    25e8: e59f3050     	ldr	r3, [pc, #0x50]         @ 0x2640 <getModeName+0xac>  // u32=0x4c1c; f32?=2.73028993e-41
    25ec: e08f0003     	add	r0, pc, r3
    25f0: e12fff1e     	bx	lr
    25f4: e59f2048     	ldr	r2, [pc, #0x48]         @ 0x2644 <getModeName+0xb0>  // u32=0x4c20; f32?=2.73085045e-41
    25f8: e08f0002     	add	r0, pc, r2
    25fc: e12fff1e     	bx	lr
    2600: e59f1040     	ldr	r1, [pc, #0x40]         @ 0x2648 <getModeName+0xb4>  // u32=0x4c20; f32?=2.73085045e-41
    2604: e08f0001     	add	r0, pc, r1
    2608: e12fff1e     	bx	lr
    260c: e59f0038     	ldr	r0, [pc, #0x38]         @ 0x264c <getModeName+0xb8>  // u32=0x4bb0; f32?=2.7151559e-41
    2610: e08f0000     	add	r0, pc, r0
    2614: e12fff1e     	bx	lr
    2618: e59f3030     	ldr	r3, [pc, #0x30]         @ 0x2650 <getModeName+0xbc>  // u32=0x4bb0; f32?=2.7151559e-41
    261c: e08f0003     	add	r0, pc, r3
    2620: e12fff1e     	bx	lr
    2624: e59f2028     	ldr	r2, [pc, #0x28]         @ 0x2654 <getModeName+0xc0>  // u32=0x4c08; f32?=2.72748733e-41
    2628: e08f0002     	add	r0, pc, r2
    262c: e12fff1e     	bx	lr
    2630: e12fff1e     	bx	lr
    2634: 10 4c 00 00  	.word	0x00004c10
    2638: 14 4c 00 00  	.word	0x00004c14
    263c: 18 4c 00 00  	.word	0x00004c18
    2640: 1c 4c 00 00  	.word	0x00004c1c
    2644: 20 4c 00 00  	.word	0x00004c20
    2648: 20 4c 00 00  	.word	0x00004c20
    264c: b0 4b 00 00  	.word	0x00004bb0
    2650: b0 4b 00 00  	.word	0x00004bb0
    2654: 08 4c 00 00  	.word	0x00004c08

