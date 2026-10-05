00003174 <getModeName>:
    3174: e3500008     	cmp	r0, #8
    3178: 908ff100     	addls	pc, pc, r0, lsl #2
    317c: ea000023     	b	0x3210 <getModeName+0x9c> @ imm = #0x8c
    3180: ea00001c     	b	0x31f8 <getModeName+0x84> @ imm = #0x70
    3184: ea00001e     	b	0x3204 <getModeName+0x90> @ imm = #0x78
    3188: ea000005     	b	0x31a4 <getModeName+0x30> @ imm = #0x14
    318c: ea000007     	b	0x31b0 <getModeName+0x3c> @ imm = #0x1c
    3190: ea000009     	b	0x31bc <getModeName+0x48> @ imm = #0x24
    3194: ea00000b     	b	0x31c8 <getModeName+0x54> @ imm = #0x2c
    3198: ea00000d     	b	0x31d4 <getModeName+0x60> @ imm = #0x34
    319c: ea00000f     	b	0x31e0 <getModeName+0x6c> @ imm = #0x3c
    31a0: ea000011     	b	0x31ec <getModeName+0x78> @ imm = #0x44
    31a4: e59f1068     	ldr	r1, [pc, #0x68]         @ 0x3214 <getModeName+0xa0>
    31a8: e08f0001     	add	r0, pc, r1
    31ac: e12fff1e     	bx	lr
    31b0: e59f0060     	ldr	r0, [pc, #0x60]         @ 0x3218 <getModeName+0xa4>
    31b4: e08f0000     	add	r0, pc, r0
    31b8: e12fff1e     	bx	lr
    31bc: e59fc058     	ldr	r12, [pc, #0x58]        @ 0x321c <getModeName+0xa8>
    31c0: e08f000c     	add	r0, pc, r12
    31c4: e12fff1e     	bx	lr
    31c8: e59f3050     	ldr	r3, [pc, #0x50]         @ 0x3220 <getModeName+0xac>
    31cc: e08f0003     	add	r0, pc, r3
    31d0: e12fff1e     	bx	lr
    31d4: e59f2048     	ldr	r2, [pc, #0x48]         @ 0x3224 <getModeName+0xb0>
    31d8: e08f0002     	add	r0, pc, r2
    31dc: e12fff1e     	bx	lr
    31e0: e59f1040     	ldr	r1, [pc, #0x40]         @ 0x3228 <getModeName+0xb4>
    31e4: e08f0001     	add	r0, pc, r1
    31e8: e12fff1e     	bx	lr
    31ec: e59f0038     	ldr	r0, [pc, #0x38]         @ 0x322c <getModeName+0xb8>
    31f0: e08f0000     	add	r0, pc, r0
    31f4: e12fff1e     	bx	lr
    31f8: e59f3030     	ldr	r3, [pc, #0x30]         @ 0x3230 <getModeName+0xbc>
    31fc: e08f0003     	add	r0, pc, r3
    3200: e12fff1e     	bx	lr
    3204: e59f2028     	ldr	r2, [pc, #0x28]         @ 0x3234 <getModeName+0xc0>
    3208: e08f0002     	add	r0, pc, r2
    320c: e12fff1e     	bx	lr
    3210: e12fff1e     	bx	lr
    3214: 88 63 00 00  	.word	0x00006388
    3218: 8c 63 00 00  	.word	0x0000638c
    321c: 90 63 00 00  	.word	0x00006390
    3220: 94 63 00 00  	.word	0x00006394
    3224: 98 63 00 00  	.word	0x00006398
    3228: 98 63 00 00  	.word	0x00006398
    322c: 28 63 00 00  	.word	0x00006328
    3230: 28 63 00 00  	.word	0x00006328
    3234: 80 63 00 00  	.word	0x00006380

