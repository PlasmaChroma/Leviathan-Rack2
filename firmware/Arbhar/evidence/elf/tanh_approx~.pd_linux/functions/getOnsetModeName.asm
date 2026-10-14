00002658 <getOnsetModeName>:
    2658: e2400001     	sub	r0, r0, #1
    265c: e3500005     	cmp	r0, #5
    2660: 908ff100     	addls	pc, pc, r0, lsl #2
    2664: ea000017     	b	0x26c8 <getOnsetModeName+0x70> @ imm = #0x5c
    2668: ea000004     	b	0x2680 <getOnsetModeName+0x28> @ imm = #0x10
    266c: ea000012     	b	0x26bc <getOnsetModeName+0x64> @ imm = #0x48
    2670: ea000005     	b	0x268c <getOnsetModeName+0x34> @ imm = #0x14
    2674: ea000007     	b	0x2698 <getOnsetModeName+0x40> @ imm = #0x1c
    2678: ea000009     	b	0x26a4 <getOnsetModeName+0x4c> @ imm = #0x24
    267c: ea00000b     	b	0x26b0 <getOnsetModeName+0x58> @ imm = #0x2c
    2680: e59f104c     	ldr	r1, [pc, #0x4c]         @ 0x26d4 <getOnsetModeName+0x7c>
    2684: e08f0001     	add	r0, pc, r1
    2688: e12fff1e     	bx	lr
    268c: e59fc044     	ldr	r12, [pc, #0x44]        @ 0x26d8 <getOnsetModeName+0x80>
    2690: e08f000c     	add	r0, pc, r12
    2694: e12fff1e     	bx	lr
    2698: e59f303c     	ldr	r3, [pc, #0x3c]         @ 0x26dc <getOnsetModeName+0x84>
    269c: e08f0003     	add	r0, pc, r3
    26a0: e12fff1e     	bx	lr
    26a4: e59f2034     	ldr	r2, [pc, #0x34]         @ 0x26e0 <getOnsetModeName+0x88>
    26a8: e08f0002     	add	r0, pc, r2
    26ac: e12fff1e     	bx	lr
    26b0: e59f102c     	ldr	r1, [pc, #0x2c]         @ 0x26e4 <getOnsetModeName+0x8c>
    26b4: e08f0001     	add	r0, pc, r1
    26b8: e12fff1e     	bx	lr
    26bc: e59f0024     	ldr	r0, [pc, #0x24]         @ 0x26e8 <getOnsetModeName+0x90>
    26c0: e08f0000     	add	r0, pc, r0
    26c4: e12fff1e     	bx	lr
    26c8: e59f201c     	ldr	r2, [pc, #0x1c]         @ 0x26ec <getOnsetModeName+0x94>
    26cc: e08f0002     	add	r0, pc, r2
    26d0: e12fff1e     	bx	lr
    26d4: c0 4b 00 00  	.word	0x00004bc0
    26d8: bc 4b 00 00  	.word	0x00004bbc
    26dc: b8 4b 00 00  	.word	0x00004bb8
    26e0: b4 4b 00 00  	.word	0x00004bb4
    26e4: b0 4b 00 00  	.word	0x00004bb0
    26e8: ac 4b 00 00  	.word	0x00004bac
    26ec: 70 4b 00 00  	.word	0x00004b70

