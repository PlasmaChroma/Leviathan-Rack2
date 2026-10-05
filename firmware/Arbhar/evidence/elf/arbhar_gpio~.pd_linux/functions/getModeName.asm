00004e2c <getModeName>:
    4e2c: e3500008     	cmp	r0, #8
    4e30: 908ff100     	addls	pc, pc, r0, lsl #2
    4e34: ea000023     	b	0x4ec8 <getModeName+0x9c> @ imm = #0x8c
    4e38: ea00001c     	b	0x4eb0 <getModeName+0x84> @ imm = #0x70
    4e3c: ea00001e     	b	0x4ebc <getModeName+0x90> @ imm = #0x78
    4e40: ea000005     	b	0x4e5c <getModeName+0x30> @ imm = #0x14
    4e44: ea000007     	b	0x4e68 <getModeName+0x3c> @ imm = #0x1c
    4e48: ea000009     	b	0x4e74 <getModeName+0x48> @ imm = #0x24
    4e4c: ea00000b     	b	0x4e80 <getModeName+0x54> @ imm = #0x2c
    4e50: ea00000d     	b	0x4e8c <getModeName+0x60> @ imm = #0x34
    4e54: ea00000f     	b	0x4e98 <getModeName+0x6c> @ imm = #0x3c
    4e58: ea000011     	b	0x4ea4 <getModeName+0x78> @ imm = #0x44
    4e5c: e59f1068     	ldr	r1, [pc, #0x68]         @ 0x4ecc <getModeName+0xa0>
    4e60: e08f0001     	add	r0, pc, r1
    4e64: e12fff1e     	bx	lr
    4e68: e59f0060     	ldr	r0, [pc, #0x60]         @ 0x4ed0 <getModeName+0xa4>
    4e6c: e08f0000     	add	r0, pc, r0
    4e70: e12fff1e     	bx	lr
    4e74: e59fc058     	ldr	r12, [pc, #0x58]        @ 0x4ed4 <getModeName+0xa8>
    4e78: e08f000c     	add	r0, pc, r12
    4e7c: e12fff1e     	bx	lr
    4e80: e59f3050     	ldr	r3, [pc, #0x50]         @ 0x4ed8 <getModeName+0xac>
    4e84: e08f0003     	add	r0, pc, r3
    4e88: e12fff1e     	bx	lr
    4e8c: e59f2048     	ldr	r2, [pc, #0x48]         @ 0x4edc <getModeName+0xb0>
    4e90: e08f0002     	add	r0, pc, r2
    4e94: e12fff1e     	bx	lr
    4e98: e59f1040     	ldr	r1, [pc, #0x40]         @ 0x4ee0 <getModeName+0xb4>
    4e9c: e08f0001     	add	r0, pc, r1
    4ea0: e12fff1e     	bx	lr
    4ea4: e59f0038     	ldr	r0, [pc, #0x38]         @ 0x4ee4 <getModeName+0xb8>
    4ea8: e08f0000     	add	r0, pc, r0
    4eac: e12fff1e     	bx	lr
    4eb0: e59f3030     	ldr	r3, [pc, #0x30]         @ 0x4ee8 <getModeName+0xbc>
    4eb4: e08f0003     	add	r0, pc, r3
    4eb8: e12fff1e     	bx	lr
    4ebc: e59f2028     	ldr	r2, [pc, #0x28]         @ 0x4eec <getModeName+0xc0>
    4ec0: e08f0002     	add	r0, pc, r2
    4ec4: e12fff1e     	bx	lr
    4ec8: e12fff1e     	bx	lr
    4ecc: 98 fd 00 00  	.word	0x0000fd98
    4ed0: 9c fd 00 00  	.word	0x0000fd9c
    4ed4: a0 fd 00 00  	.word	0x0000fda0
    4ed8: a4 fd 00 00  	.word	0x0000fda4
    4edc: a8 fd 00 00  	.word	0x0000fda8
    4ee0: a8 fd 00 00  	.word	0x0000fda8
    4ee4: 38 fd 00 00  	.word	0x0000fd38
    4ee8: 38 fd 00 00  	.word	0x0000fd38
    4eec: 90 fd 00 00  	.word	0x0000fd90

