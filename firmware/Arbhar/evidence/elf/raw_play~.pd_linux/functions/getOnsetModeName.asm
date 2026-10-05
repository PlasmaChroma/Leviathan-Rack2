000028f8 <getOnsetModeName>:
    28f8: e2400001     	sub	r0, r0, #1
    28fc: e3500005     	cmp	r0, #5
    2900: 908ff100     	addls	pc, pc, r0, lsl #2
    2904: ea000017     	b	0x2968 <getOnsetModeName+0x70> @ imm = #0x5c
    2908: ea000004     	b	0x2920 <getOnsetModeName+0x28> @ imm = #0x10
    290c: ea000012     	b	0x295c <getOnsetModeName+0x64> @ imm = #0x48
    2910: ea000005     	b	0x292c <getOnsetModeName+0x34> @ imm = #0x14
    2914: ea000007     	b	0x2938 <getOnsetModeName+0x40> @ imm = #0x1c
    2918: ea000009     	b	0x2944 <getOnsetModeName+0x4c> @ imm = #0x24
    291c: ea00000b     	b	0x2950 <getOnsetModeName+0x58> @ imm = #0x2c
    2920: e59f104c     	ldr	r1, [pc, #0x4c]         @ 0x2974 <getOnsetModeName+0x7c>
    2924: e08f0001     	add	r0, pc, r1
    2928: e12fff1e     	bx	lr
    292c: e59fc044     	ldr	r12, [pc, #0x44]        @ 0x2978 <getOnsetModeName+0x80>
    2930: e08f000c     	add	r0, pc, r12
    2934: e12fff1e     	bx	lr
    2938: e59f303c     	ldr	r3, [pc, #0x3c]         @ 0x297c <getOnsetModeName+0x84>
    293c: e08f0003     	add	r0, pc, r3
    2940: e12fff1e     	bx	lr
    2944: e59f2034     	ldr	r2, [pc, #0x34]         @ 0x2980 <getOnsetModeName+0x88>
    2948: e08f0002     	add	r0, pc, r2
    294c: e12fff1e     	bx	lr
    2950: e59f102c     	ldr	r1, [pc, #0x2c]         @ 0x2984 <getOnsetModeName+0x8c>
    2954: e08f0001     	add	r0, pc, r1
    2958: e12fff1e     	bx	lr
    295c: e59f0024     	ldr	r0, [pc, #0x24]         @ 0x2988 <getOnsetModeName+0x90>
    2960: e08f0000     	add	r0, pc, r0
    2964: e12fff1e     	bx	lr
    2968: e59f201c     	ldr	r2, [pc, #0x1c]         @ 0x298c <getOnsetModeName+0x94>
    296c: e08f0002     	add	r0, pc, r2
    2970: e12fff1e     	bx	lr
    2974: 00 4d 00 00  	.word	0x00004d00
    2978: fc 4c 00 00  	.word	0x00004cfc
    297c: f8 4c 00 00  	.word	0x00004cf8
    2980: f4 4c 00 00  	.word	0x00004cf4
    2984: f0 4c 00 00  	.word	0x00004cf0
    2988: ec 4c 00 00  	.word	0x00004cec
    298c: b0 4c 00 00  	.word	0x00004cb0

