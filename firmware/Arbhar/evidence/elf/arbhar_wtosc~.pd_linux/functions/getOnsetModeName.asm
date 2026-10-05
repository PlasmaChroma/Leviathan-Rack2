00002cb0 <getOnsetModeName>:
    2cb0: e2400001     	sub	r0, r0, #1
    2cb4: e3500005     	cmp	r0, #5
    2cb8: 908ff100     	addls	pc, pc, r0, lsl #2
    2cbc: ea000017     	b	0x2d20 <getOnsetModeName+0x70> @ imm = #0x5c
    2cc0: ea000004     	b	0x2cd8 <getOnsetModeName+0x28> @ imm = #0x10
    2cc4: ea000012     	b	0x2d14 <getOnsetModeName+0x64> @ imm = #0x48
    2cc8: ea000005     	b	0x2ce4 <getOnsetModeName+0x34> @ imm = #0x14
    2ccc: ea000007     	b	0x2cf0 <getOnsetModeName+0x40> @ imm = #0x1c
    2cd0: ea000009     	b	0x2cfc <getOnsetModeName+0x4c> @ imm = #0x24
    2cd4: ea00000b     	b	0x2d08 <getOnsetModeName+0x58> @ imm = #0x2c
    2cd8: e59f104c     	ldr	r1, [pc, #0x4c]         @ 0x2d2c <getOnsetModeName+0x7c>
    2cdc: e08f0001     	add	r0, pc, r1
    2ce0: e12fff1e     	bx	lr
    2ce4: e59fc044     	ldr	r12, [pc, #0x44]        @ 0x2d30 <getOnsetModeName+0x80>
    2ce8: e08f000c     	add	r0, pc, r12
    2cec: e12fff1e     	bx	lr
    2cf0: e59f303c     	ldr	r3, [pc, #0x3c]         @ 0x2d34 <getOnsetModeName+0x84>
    2cf4: e08f0003     	add	r0, pc, r3
    2cf8: e12fff1e     	bx	lr
    2cfc: e59f2034     	ldr	r2, [pc, #0x34]         @ 0x2d38 <getOnsetModeName+0x88>
    2d00: e08f0002     	add	r0, pc, r2
    2d04: e12fff1e     	bx	lr
    2d08: e59f102c     	ldr	r1, [pc, #0x2c]         @ 0x2d3c <getOnsetModeName+0x8c>
    2d0c: e08f0001     	add	r0, pc, r1
    2d10: e12fff1e     	bx	lr
    2d14: e59f0024     	ldr	r0, [pc, #0x24]         @ 0x2d40 <getOnsetModeName+0x90>
    2d18: e08f0000     	add	r0, pc, r0
    2d1c: e12fff1e     	bx	lr
    2d20: e59f201c     	ldr	r2, [pc, #0x1c]         @ 0x2d44 <getOnsetModeName+0x94>
    2d24: e08f0002     	add	r0, pc, r2
    2d28: e12fff1e     	bx	lr
    2d2c: d0 58 00 00  	.word	0x000058d0
    2d30: cc 58 00 00  	.word	0x000058cc
    2d34: c8 58 00 00  	.word	0x000058c8
    2d38: c4 58 00 00  	.word	0x000058c4
    2d3c: c0 58 00 00  	.word	0x000058c0
    2d40: bc 58 00 00  	.word	0x000058bc
    2d44: 80 58 00 00  	.word	0x00005880

