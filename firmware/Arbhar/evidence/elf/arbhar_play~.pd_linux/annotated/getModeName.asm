000057cc <getModeName>:
    57cc: e3500008     	cmp	r0, #8
    57d0: 908ff100     	addls	pc, pc, r0, lsl #2
    57d4: ea000023     	b	0x5868 <getModeName+0x9c> @ imm = #0x8c
    57d8: ea00001c     	b	0x5850 <getModeName+0x84> @ imm = #0x70
    57dc: ea00001e     	b	0x585c <getModeName+0x90> @ imm = #0x78
    57e0: ea000005     	b	0x57fc <getModeName+0x30> @ imm = #0x14
    57e4: ea000007     	b	0x5808 <getModeName+0x3c> @ imm = #0x1c
    57e8: ea000009     	b	0x5814 <getModeName+0x48> @ imm = #0x24
    57ec: ea00000b     	b	0x5820 <getModeName+0x54> @ imm = #0x2c
    57f0: ea00000d     	b	0x582c <getModeName+0x60> @ imm = #0x34
    57f4: ea00000f     	b	0x5838 <getModeName+0x6c> @ imm = #0x3c
    57f8: ea000011     	b	0x5844 <getModeName+0x78> @ imm = #0x44
    57fc: e59f1068     	ldr	r1, [pc, #0x68]         @ 0x586c <getModeName+0xa0>  // u32=0x73fc; f32?=4.1607354e-41
    5800: e08f0001     	add	r0, pc, r1
    5804: e12fff1e     	bx	lr
    5808: e59f0060     	ldr	r0, [pc, #0x60]         @ 0x5870 <getModeName+0xa4>  // u32=0x7400; f32?=4.16129592e-41
    580c: e08f0000     	add	r0, pc, r0
    5810: e12fff1e     	bx	lr
    5814: e59fc058     	ldr	r12, [pc, #0x58]        @ 0x5874 <getModeName+0xa8>  // u32=0x7404; f32?=4.16185644e-41
    5818: e08f000c     	add	r0, pc, r12
    581c: e12fff1e     	bx	lr
    5820: e59f3050     	ldr	r3, [pc, #0x50]         @ 0x5878 <getModeName+0xac>  // u32=0x7408; f32?=4.16241696e-41
    5824: e08f0003     	add	r0, pc, r3
    5828: e12fff1e     	bx	lr
    582c: e59f2048     	ldr	r2, [pc, #0x48]         @ 0x587c <getModeName+0xb0>  // u32=0x740c; f32?=4.16297748e-41
    5830: e08f0002     	add	r0, pc, r2
    5834: e12fff1e     	bx	lr
    5838: e59f1040     	ldr	r1, [pc, #0x40]         @ 0x5880 <getModeName+0xb4>  // u32=0x740c; f32?=4.16297748e-41
    583c: e08f0001     	add	r0, pc, r1
    5840: e12fff1e     	bx	lr
    5844: e59f0038     	ldr	r0, [pc, #0x38]         @ 0x5884 <getModeName+0xb8>  // u32=0x739c; f32?=4.14728294e-41
    5848: e08f0000     	add	r0, pc, r0
    584c: e12fff1e     	bx	lr
    5850: e59f3030     	ldr	r3, [pc, #0x30]         @ 0x5888 <getModeName+0xbc>  // u32=0x739c; f32?=4.14728294e-41
    5854: e08f0003     	add	r0, pc, r3
    5858: e12fff1e     	bx	lr
    585c: e59f2028     	ldr	r2, [pc, #0x28]         @ 0x588c <getModeName+0xc0>  // u32=0x73f4; f32?=4.15961436e-41
    5860: e08f0002     	add	r0, pc, r2
    5864: e12fff1e     	bx	lr
    5868: e12fff1e     	bx	lr
    586c: fc 73 00 00  	.word	0x000073fc
    5870: 00 74 00 00  	.word	0x00007400
    5874: 04 74 00 00  	.word	0x00007404
    5878: 08 74 00 00  	.word	0x00007408
    587c: 0c 74 00 00  	.word	0x0000740c
    5880: 0c 74 00 00  	.word	0x0000740c
    5884: 9c 73 00 00  	.word	0x0000739c
    5888: 9c 73 00 00  	.word	0x0000739c
    588c: f4 73 00 00  	.word	0x000073f4

