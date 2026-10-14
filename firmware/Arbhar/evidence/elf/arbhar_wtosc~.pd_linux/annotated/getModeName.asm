00002bec <getModeName>:
    2bec: e3500008     	cmp	r0, #8
    2bf0: 908ff100     	addls	pc, pc, r0, lsl #2
    2bf4: ea000023     	b	0x2c88 <getModeName+0x9c> @ imm = #0x8c
    2bf8: ea00001c     	b	0x2c70 <getModeName+0x84> @ imm = #0x70
    2bfc: ea00001e     	b	0x2c7c <getModeName+0x90> @ imm = #0x78
    2c00: ea000005     	b	0x2c1c <getModeName+0x30> @ imm = #0x14
    2c04: ea000007     	b	0x2c28 <getModeName+0x3c> @ imm = #0x1c
    2c08: ea000009     	b	0x2c34 <getModeName+0x48> @ imm = #0x24
    2c0c: ea00000b     	b	0x2c40 <getModeName+0x54> @ imm = #0x2c
    2c10: ea00000d     	b	0x2c4c <getModeName+0x60> @ imm = #0x34
    2c14: ea00000f     	b	0x2c58 <getModeName+0x6c> @ imm = #0x3c
    2c18: ea000011     	b	0x2c64 <getModeName+0x78> @ imm = #0x44
    2c1c: e59f1068     	ldr	r1, [pc, #0x68]         @ 0x2c8c <getModeName+0xa0>  // u32=0x5920; f32?=3.19720258e-41
    2c20: e08f0001     	add	r0, pc, r1
    2c24: e12fff1e     	bx	lr
    2c28: e59f0060     	ldr	r0, [pc, #0x60]         @ 0x2c90 <getModeName+0xa4>  // u32=0x5924; f32?=3.1977631e-41
    2c2c: e08f0000     	add	r0, pc, r0
    2c30: e12fff1e     	bx	lr
    2c34: e59fc058     	ldr	r12, [pc, #0x58]        @ 0x2c94 <getModeName+0xa8>  // u32=0x5928; f32?=3.19832361e-41
    2c38: e08f000c     	add	r0, pc, r12
    2c3c: e12fff1e     	bx	lr
    2c40: e59f3050     	ldr	r3, [pc, #0x50]         @ 0x2c98 <getModeName+0xac>  // u32=0x592c; f32?=3.19888413e-41
    2c44: e08f0003     	add	r0, pc, r3
    2c48: e12fff1e     	bx	lr
    2c4c: e59f2048     	ldr	r2, [pc, #0x48]         @ 0x2c9c <getModeName+0xb0>  // u32=0x5930; f32?=3.19944465e-41
    2c50: e08f0002     	add	r0, pc, r2
    2c54: e12fff1e     	bx	lr
    2c58: e59f1040     	ldr	r1, [pc, #0x40]         @ 0x2ca0 <getModeName+0xb4>  // u32=0x5930; f32?=3.19944465e-41
    2c5c: e08f0001     	add	r0, pc, r1
    2c60: e12fff1e     	bx	lr
    2c64: e59f0038     	ldr	r0, [pc, #0x38]         @ 0x2ca4 <getModeName+0xb8>  // u32=0x58c0; f32?=3.18375011e-41
    2c68: e08f0000     	add	r0, pc, r0
    2c6c: e12fff1e     	bx	lr
    2c70: e59f3030     	ldr	r3, [pc, #0x30]         @ 0x2ca8 <getModeName+0xbc>  // u32=0x58c0; f32?=3.18375011e-41
    2c74: e08f0003     	add	r0, pc, r3
    2c78: e12fff1e     	bx	lr
    2c7c: e59f2028     	ldr	r2, [pc, #0x28]         @ 0x2cac <getModeName+0xc0>  // u32=0x5918; f32?=3.19608154e-41
    2c80: e08f0002     	add	r0, pc, r2
    2c84: e12fff1e     	bx	lr
    2c88: e12fff1e     	bx	lr
    2c8c: 20 59 00 00  	.word	0x00005920
    2c90: 24 59 00 00  	.word	0x00005924
    2c94: 28 59 00 00  	.word	0x00005928
    2c98: 2c 59 00 00  	.word	0x0000592c
    2c9c: 30 59 00 00  	.word	0x00005930
    2ca0: 30 59 00 00  	.word	0x00005930
    2ca4: c0 58 00 00  	.word	0x000058c0
    2ca8: c0 58 00 00  	.word	0x000058c0
    2cac: 18 59 00 00  	.word	0x00005918

