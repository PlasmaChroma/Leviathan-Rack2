00005890 <getOnsetModeName>:
    5890: e2400001     	sub	r0, r0, #1
    5894: e3500005     	cmp	r0, #5
    5898: 908ff100     	addls	pc, pc, r0, lsl #2
    589c: ea000017     	b	0x5900 <getOnsetModeName+0x70> @ imm = #0x5c
    58a0: ea000004     	b	0x58b8 <getOnsetModeName+0x28> @ imm = #0x10
    58a4: ea000012     	b	0x58f4 <getOnsetModeName+0x64> @ imm = #0x48
    58a8: ea000005     	b	0x58c4 <getOnsetModeName+0x34> @ imm = #0x14
    58ac: ea000007     	b	0x58d0 <getOnsetModeName+0x40> @ imm = #0x1c
    58b0: ea000009     	b	0x58dc <getOnsetModeName+0x4c> @ imm = #0x24
    58b4: ea00000b     	b	0x58e8 <getOnsetModeName+0x58> @ imm = #0x2c
    58b8: e59f104c     	ldr	r1, [pc, #0x4c]         @ 0x590c <getOnsetModeName+0x7c>  // u32=0x73ac; f32?=4.14952501e-41
    58bc: e08f0001     	add	r0, pc, r1
    58c0: e12fff1e     	bx	lr
    58c4: e59fc044     	ldr	r12, [pc, #0x44]        @ 0x5910 <getOnsetModeName+0x80>  // u32=0x73a8; f32?=4.14896449e-41
    58c8: e08f000c     	add	r0, pc, r12
    58cc: e12fff1e     	bx	lr
    58d0: e59f303c     	ldr	r3, [pc, #0x3c]         @ 0x5914 <getOnsetModeName+0x84>  // u32=0x73a4; f32?=4.14840397e-41
    58d4: e08f0003     	add	r0, pc, r3
    58d8: e12fff1e     	bx	lr
    58dc: e59f2034     	ldr	r2, [pc, #0x34]         @ 0x5918 <getOnsetModeName+0x88>  // u32=0x73a0; f32?=4.14784345e-41
    58e0: e08f0002     	add	r0, pc, r2
    58e4: e12fff1e     	bx	lr
    58e8: e59f102c     	ldr	r1, [pc, #0x2c]         @ 0x591c <getOnsetModeName+0x8c>  // u32=0x739c; f32?=4.14728294e-41
    58ec: e08f0001     	add	r0, pc, r1
    58f0: e12fff1e     	bx	lr
    58f4: e59f0024     	ldr	r0, [pc, #0x24]         @ 0x5920 <getOnsetModeName+0x90>  // u32=0x7398; f32?=4.14672242e-41
    58f8: e08f0000     	add	r0, pc, r0
    58fc: e12fff1e     	bx	lr
    5900: e59f201c     	ldr	r2, [pc, #0x1c]         @ 0x5924 <getOnsetModeName+0x94>  // u32=0x735c; f32?=4.13831462e-41
    5904: e08f0002     	add	r0, pc, r2
    5908: e12fff1e     	bx	lr
    590c: ac 73 00 00  	.word	0x000073ac
    5910: a8 73 00 00  	.word	0x000073a8
    5914: a4 73 00 00  	.word	0x000073a4
    5918: a0 73 00 00  	.word	0x000073a0
    591c: 9c 73 00 00  	.word	0x0000739c
    5920: 98 73 00 00  	.word	0x00007398
    5924: 5c 73 00 00  	.word	0x0000735c

