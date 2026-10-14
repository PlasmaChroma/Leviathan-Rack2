00002600 <getOnsetModeName>:
    2600: e2400001     	sub	r0, r0, #1
    2604: e3500005     	cmp	r0, #5
    2608: 908ff100     	addls	pc, pc, r0, lsl #2
    260c: ea000017     	b	0x2670 <getOnsetModeName+0x70> @ imm = #0x5c
    2610: ea000004     	b	0x2628 <getOnsetModeName+0x28> @ imm = #0x10
    2614: ea000012     	b	0x2664 <getOnsetModeName+0x64> @ imm = #0x48
    2618: ea000005     	b	0x2634 <getOnsetModeName+0x34> @ imm = #0x14
    261c: ea000007     	b	0x2640 <getOnsetModeName+0x40> @ imm = #0x1c
    2620: ea000009     	b	0x264c <getOnsetModeName+0x4c> @ imm = #0x24
    2624: ea00000b     	b	0x2658 <getOnsetModeName+0x58> @ imm = #0x2c
    2628: e59f104c     	ldr	r1, [pc, #0x4c]         @ 0x267c <getOnsetModeName+0x7c>
    262c: e08f0001     	add	r0, pc, r1
    2630: e12fff1e     	bx	lr
    2634: e59fc044     	ldr	r12, [pc, #0x44]        @ 0x2680 <getOnsetModeName+0x80>
    2638: e08f000c     	add	r0, pc, r12
    263c: e12fff1e     	bx	lr
    2640: e59f303c     	ldr	r3, [pc, #0x3c]         @ 0x2684 <getOnsetModeName+0x84>
    2644: e08f0003     	add	r0, pc, r3
    2648: e12fff1e     	bx	lr
    264c: e59f2034     	ldr	r2, [pc, #0x34]         @ 0x2688 <getOnsetModeName+0x88>
    2650: e08f0002     	add	r0, pc, r2
    2654: e12fff1e     	bx	lr
    2658: e59f102c     	ldr	r1, [pc, #0x2c]         @ 0x268c <getOnsetModeName+0x8c>
    265c: e08f0001     	add	r0, pc, r1
    2660: e12fff1e     	bx	lr
    2664: e59f0024     	ldr	r0, [pc, #0x24]         @ 0x2690 <getOnsetModeName+0x90>
    2668: e08f0000     	add	r0, pc, r0
    266c: e12fff1e     	bx	lr
    2670: e59f201c     	ldr	r2, [pc, #0x1c]         @ 0x2694 <getOnsetModeName+0x94>
    2674: e08f0002     	add	r0, pc, r2
    2678: e12fff1e     	bx	lr
    267c: c8 4b 00 00  	.word	0x00004bc8
    2680: c4 4b 00 00  	.word	0x00004bc4
    2684: c0 4b 00 00  	.word	0x00004bc0
    2688: bc 4b 00 00  	.word	0x00004bbc
    268c: b8 4b 00 00  	.word	0x00004bb8
    2690: b4 4b 00 00  	.word	0x00004bb4
    2694: 78 4b 00 00  	.word	0x00004b78

