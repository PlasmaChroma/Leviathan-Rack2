000025d8 <getOnsetModeName>:
    25d8: e2400001     	sub	r0, r0, #1
    25dc: e3500005     	cmp	r0, #5
    25e0: 908ff100     	addls	pc, pc, r0, lsl #2
    25e4: ea000017     	b	0x2648 <getOnsetModeName+0x70> @ imm = #0x5c
    25e8: ea000004     	b	0x2600 <getOnsetModeName+0x28> @ imm = #0x10
    25ec: ea000012     	b	0x263c <getOnsetModeName+0x64> @ imm = #0x48
    25f0: ea000005     	b	0x260c <getOnsetModeName+0x34> @ imm = #0x14
    25f4: ea000007     	b	0x2618 <getOnsetModeName+0x40> @ imm = #0x1c
    25f8: ea000009     	b	0x2624 <getOnsetModeName+0x4c> @ imm = #0x24
    25fc: ea00000b     	b	0x2630 <getOnsetModeName+0x58> @ imm = #0x2c
    2600: e59f104c     	ldr	r1, [pc, #0x4c]         @ 0x2654 <getOnsetModeName+0x7c>  // u32=0x4fa0; f32?=2.85640679e-41
    2604: e08f0001     	add	r0, pc, r1
    2608: e12fff1e     	bx	lr
    260c: e59fc044     	ldr	r12, [pc, #0x44]        @ 0x2658 <getOnsetModeName+0x80>  // u32=0x4f9c; f32?=2.85584627e-41
    2610: e08f000c     	add	r0, pc, r12
    2614: e12fff1e     	bx	lr
    2618: e59f303c     	ldr	r3, [pc, #0x3c]         @ 0x265c <getOnsetModeName+0x84>  // u32=0x4f98; f32?=2.85528575e-41
    261c: e08f0003     	add	r0, pc, r3
    2620: e12fff1e     	bx	lr
    2624: e59f2034     	ldr	r2, [pc, #0x34]         @ 0x2660 <getOnsetModeName+0x88>  // u32=0x4f94; f32?=2.85472523e-41
    2628: e08f0002     	add	r0, pc, r2
    262c: e12fff1e     	bx	lr
    2630: e59f102c     	ldr	r1, [pc, #0x2c]         @ 0x2664 <getOnsetModeName+0x8c>  // u32=0x4f90; f32?=2.85416471e-41
    2634: e08f0001     	add	r0, pc, r1
    2638: e12fff1e     	bx	lr
    263c: e59f0024     	ldr	r0, [pc, #0x24]         @ 0x2668 <getOnsetModeName+0x90>  // u32=0x4f8c; f32?=2.85360419e-41
    2640: e08f0000     	add	r0, pc, r0
    2644: e12fff1e     	bx	lr
    2648: e59f201c     	ldr	r2, [pc, #0x1c]         @ 0x266c <getOnsetModeName+0x94>  // u32=0x4f50; f32?=2.8451964e-41
    264c: e08f0002     	add	r0, pc, r2
    2650: e12fff1e     	bx	lr
    2654: a0 4f 00 00  	.word	0x00004fa0
    2658: 9c 4f 00 00  	.word	0x00004f9c
    265c: 98 4f 00 00  	.word	0x00004f98
    2660: 94 4f 00 00  	.word	0x00004f94
    2664: 90 4f 00 00  	.word	0x00004f90
    2668: 8c 4f 00 00  	.word	0x00004f8c
    266c: 50 4f 00 00  	.word	0x00004f50

