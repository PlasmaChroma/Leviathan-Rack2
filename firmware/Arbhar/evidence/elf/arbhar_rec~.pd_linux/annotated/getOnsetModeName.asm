00003238 <getOnsetModeName>:
    3238: e2400001     	sub	r0, r0, #1
    323c: e3500005     	cmp	r0, #5
    3240: 908ff100     	addls	pc, pc, r0, lsl #2
    3244: ea000017     	b	0x32a8 <getOnsetModeName+0x70> @ imm = #0x5c
    3248: ea000004     	b	0x3260 <getOnsetModeName+0x28> @ imm = #0x10
    324c: ea000012     	b	0x329c <getOnsetModeName+0x64> @ imm = #0x48
    3250: ea000005     	b	0x326c <getOnsetModeName+0x34> @ imm = #0x14
    3254: ea000007     	b	0x3278 <getOnsetModeName+0x40> @ imm = #0x1c
    3258: ea000009     	b	0x3284 <getOnsetModeName+0x4c> @ imm = #0x24
    325c: ea00000b     	b	0x3290 <getOnsetModeName+0x58> @ imm = #0x2c
    3260: e59f104c     	ldr	r1, [pc, #0x4c]         @ 0x32b4 <getOnsetModeName+0x7c>  // u32=0x6338; f32?=3.5592981e-41
    3264: e08f0001     	add	r0, pc, r1
    3268: e12fff1e     	bx	lr
    326c: e59fc044     	ldr	r12, [pc, #0x44]        @ 0x32b8 <getOnsetModeName+0x80>  // u32=0x6334; f32?=3.55873758e-41
    3270: e08f000c     	add	r0, pc, r12
    3274: e12fff1e     	bx	lr
    3278: e59f303c     	ldr	r3, [pc, #0x3c]         @ 0x32bc <getOnsetModeName+0x84>  // u32=0x6330; f32?=3.55817706e-41
    327c: e08f0003     	add	r0, pc, r3
    3280: e12fff1e     	bx	lr
    3284: e59f2034     	ldr	r2, [pc, #0x34]         @ 0x32c0 <getOnsetModeName+0x88>  // u32=0x632c; f32?=3.55761654e-41
    3288: e08f0002     	add	r0, pc, r2
    328c: e12fff1e     	bx	lr
    3290: e59f102c     	ldr	r1, [pc, #0x2c]         @ 0x32c4 <getOnsetModeName+0x8c>  // u32=0x6328; f32?=3.55705602e-41
    3294: e08f0001     	add	r0, pc, r1
    3298: e12fff1e     	bx	lr
    329c: e59f0024     	ldr	r0, [pc, #0x24]         @ 0x32c8 <getOnsetModeName+0x90>  // u32=0x6324; f32?=3.5564955e-41
    32a0: e08f0000     	add	r0, pc, r0
    32a4: e12fff1e     	bx	lr
    32a8: e59f201c     	ldr	r2, [pc, #0x1c]         @ 0x32cc <getOnsetModeName+0x94>  // u32=0x62e8; f32?=3.54808771e-41
    32ac: e08f0002     	add	r0, pc, r2
    32b0: e12fff1e     	bx	lr
    32b4: 38 63 00 00  	.word	0x00006338
    32b8: 34 63 00 00  	.word	0x00006334
    32bc: 30 63 00 00  	.word	0x00006330
    32c0: 2c 63 00 00  	.word	0x0000632c
    32c4: 28 63 00 00  	.word	0x00006328
    32c8: 24 63 00 00  	.word	0x00006324
    32cc: e8 62 00 00  	.word	0x000062e8

