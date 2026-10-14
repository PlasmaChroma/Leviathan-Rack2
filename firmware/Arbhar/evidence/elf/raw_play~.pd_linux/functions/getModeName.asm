00002834 <getModeName>:
    2834: e3500008     	cmp	r0, #8
    2838: 908ff100     	addls	pc, pc, r0, lsl #2
    283c: ea000023     	b	0x28d0 <getModeName+0x9c> @ imm = #0x8c
    2840: ea00001c     	b	0x28b8 <getModeName+0x84> @ imm = #0x70
    2844: ea00001e     	b	0x28c4 <getModeName+0x90> @ imm = #0x78
    2848: ea000005     	b	0x2864 <getModeName+0x30> @ imm = #0x14
    284c: ea000007     	b	0x2870 <getModeName+0x3c> @ imm = #0x1c
    2850: ea000009     	b	0x287c <getModeName+0x48> @ imm = #0x24
    2854: ea00000b     	b	0x2888 <getModeName+0x54> @ imm = #0x2c
    2858: ea00000d     	b	0x2894 <getModeName+0x60> @ imm = #0x34
    285c: ea00000f     	b	0x28a0 <getModeName+0x6c> @ imm = #0x3c
    2860: ea000011     	b	0x28ac <getModeName+0x78> @ imm = #0x44
    2864: e59f1068     	ldr	r1, [pc, #0x68]         @ 0x28d4 <getModeName+0xa0>
    2868: e08f0001     	add	r0, pc, r1
    286c: e12fff1e     	bx	lr
    2870: e59f0060     	ldr	r0, [pc, #0x60]         @ 0x28d8 <getModeName+0xa4>
    2874: e08f0000     	add	r0, pc, r0
    2878: e12fff1e     	bx	lr
    287c: e59fc058     	ldr	r12, [pc, #0x58]        @ 0x28dc <getModeName+0xa8>
    2880: e08f000c     	add	r0, pc, r12
    2884: e12fff1e     	bx	lr
    2888: e59f3050     	ldr	r3, [pc, #0x50]         @ 0x28e0 <getModeName+0xac>
    288c: e08f0003     	add	r0, pc, r3
    2890: e12fff1e     	bx	lr
    2894: e59f2048     	ldr	r2, [pc, #0x48]         @ 0x28e4 <getModeName+0xb0>
    2898: e08f0002     	add	r0, pc, r2
    289c: e12fff1e     	bx	lr
    28a0: e59f1040     	ldr	r1, [pc, #0x40]         @ 0x28e8 <getModeName+0xb4>
    28a4: e08f0001     	add	r0, pc, r1
    28a8: e12fff1e     	bx	lr
    28ac: e59f0038     	ldr	r0, [pc, #0x38]         @ 0x28ec <getModeName+0xb8>
    28b0: e08f0000     	add	r0, pc, r0
    28b4: e12fff1e     	bx	lr
    28b8: e59f3030     	ldr	r3, [pc, #0x30]         @ 0x28f0 <getModeName+0xbc>
    28bc: e08f0003     	add	r0, pc, r3
    28c0: e12fff1e     	bx	lr
    28c4: e59f2028     	ldr	r2, [pc, #0x28]         @ 0x28f4 <getModeName+0xc0>
    28c8: e08f0002     	add	r0, pc, r2
    28cc: e12fff1e     	bx	lr
    28d0: e12fff1e     	bx	lr
    28d4: 50 4d 00 00  	.word	0x00004d50
    28d8: 54 4d 00 00  	.word	0x00004d54
    28dc: 58 4d 00 00  	.word	0x00004d58
    28e0: 5c 4d 00 00  	.word	0x00004d5c
    28e4: 60 4d 00 00  	.word	0x00004d60
    28e8: 60 4d 00 00  	.word	0x00004d60
    28ec: f0 4c 00 00  	.word	0x00004cf0
    28f0: f0 4c 00 00  	.word	0x00004cf0
    28f4: 48 4d 00 00  	.word	0x00004d48

