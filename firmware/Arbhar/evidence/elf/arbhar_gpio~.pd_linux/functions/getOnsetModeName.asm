00004ef0 <getOnsetModeName>:
    4ef0: e2400001     	sub	r0, r0, #1
    4ef4: e3500005     	cmp	r0, #5
    4ef8: 908ff100     	addls	pc, pc, r0, lsl #2
    4efc: ea000017     	b	0x4f60 <getOnsetModeName+0x70> @ imm = #0x5c
    4f00: ea000004     	b	0x4f18 <getOnsetModeName+0x28> @ imm = #0x10
    4f04: ea000012     	b	0x4f54 <getOnsetModeName+0x64> @ imm = #0x48
    4f08: ea000005     	b	0x4f24 <getOnsetModeName+0x34> @ imm = #0x14
    4f0c: ea000007     	b	0x4f30 <getOnsetModeName+0x40> @ imm = #0x1c
    4f10: ea000009     	b	0x4f3c <getOnsetModeName+0x4c> @ imm = #0x24
    4f14: ea00000b     	b	0x4f48 <getOnsetModeName+0x58> @ imm = #0x2c
    4f18: e59f104c     	ldr	r1, [pc, #0x4c]         @ 0x4f6c <getOnsetModeName+0x7c>
    4f1c: e08f0001     	add	r0, pc, r1
    4f20: e12fff1e     	bx	lr
    4f24: e59fc044     	ldr	r12, [pc, #0x44]        @ 0x4f70 <getOnsetModeName+0x80>
    4f28: e08f000c     	add	r0, pc, r12
    4f2c: e12fff1e     	bx	lr
    4f30: e59f303c     	ldr	r3, [pc, #0x3c]         @ 0x4f74 <getOnsetModeName+0x84>
    4f34: e08f0003     	add	r0, pc, r3
    4f38: e12fff1e     	bx	lr
    4f3c: e59f2034     	ldr	r2, [pc, #0x34]         @ 0x4f78 <getOnsetModeName+0x88>
    4f40: e08f0002     	add	r0, pc, r2
    4f44: e12fff1e     	bx	lr
    4f48: e59f102c     	ldr	r1, [pc, #0x2c]         @ 0x4f7c <getOnsetModeName+0x8c>
    4f4c: e08f0001     	add	r0, pc, r1
    4f50: e12fff1e     	bx	lr
    4f54: e59f0024     	ldr	r0, [pc, #0x24]         @ 0x4f80 <getOnsetModeName+0x90>
    4f58: e08f0000     	add	r0, pc, r0
    4f5c: e12fff1e     	bx	lr
    4f60: e59f201c     	ldr	r2, [pc, #0x1c]         @ 0x4f84 <getOnsetModeName+0x94>
    4f64: e08f0002     	add	r0, pc, r2
    4f68: e12fff1e     	bx	lr
    4f6c: 48 fd 00 00  	.word	0x0000fd48
    4f70: 44 fd 00 00  	.word	0x0000fd44
    4f74: 40 fd 00 00  	.word	0x0000fd40
    4f78: 3c fd 00 00  	.word	0x0000fd3c
    4f7c: 38 fd 00 00  	.word	0x0000fd38
    4f80: 34 fd 00 00  	.word	0x0000fd34
    4f84: f8 fc 00 00  	.word	0x0000fcf8

