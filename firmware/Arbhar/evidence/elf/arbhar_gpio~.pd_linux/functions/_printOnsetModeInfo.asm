00006218 <_printOnsetModeInfo>:
    6218: e59030b4     	ldr	r3, [r0, #0xb4]
    621c: e92d4010     	push	{r4, lr}
    6220: e1a04000     	mov	r4, r0
    6224: e5d30000     	ldrb	r0, [r3]
    6228: ebfff6df     	bl	0x3dac <.plt+0x6b0>     @ imm = #-0x2484
    622c: e1a01000     	mov	r1, r0
    6230: e59f0080     	ldr	r0, [pc, #0x80]         @ 0x62b8 <_printOnsetModeInfo+0xa0>
    6234: e08f0000     	add	r0, pc, r0
    6238: ebfff64e     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x26c8
    623c: e59410b4     	ldr	r1, [r4, #0xb4]
    6240: e59f2074     	ldr	r2, [pc, #0x74]         @ 0x62bc <_printOnsetModeInfo+0xa4>
    6244: e5d11001     	ldrb	r1, [r1, #0x1]
    6248: e08f0002     	add	r0, pc, r2
    624c: ebfff649     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x26dc
    6250: e594c0b4     	ldr	r12, [r4, #0xb4]
    6254: e59f3064     	ldr	r3, [pc, #0x64]         @ 0x62c0 <_printOnsetModeInfo+0xa8>
    6258: e5dc1002     	ldrb	r1, [r12, #0x2]
    625c: e08f0003     	add	r0, pc, r3
    6260: ebfff644     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x26f0
    6264: e59410b4     	ldr	r1, [r4, #0xb4]
    6268: e59f0054     	ldr	r0, [pc, #0x54]         @ 0x62c4 <_printOnsetModeInfo+0xac>
    626c: e5d11003     	ldrb	r1, [r1, #0x3]
    6270: e08f0000     	add	r0, pc, r0
    6274: ebfff63f     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x2704
    6278: e59420b4     	ldr	r2, [r4, #0xb4]
    627c: e59fc044     	ldr	r12, [pc, #0x44]        @ 0x62c8 <_printOnsetModeInfo+0xb0>
    6280: e5d21004     	ldrb	r1, [r2, #0x4]
    6284: e08f000c     	add	r0, pc, r12
    6288: ebfff63a     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x2718
    628c: e59430b4     	ldr	r3, [r4, #0xb4]
    6290: e59f1034     	ldr	r1, [pc, #0x34]         @ 0x62cc <_printOnsetModeInfo+0xb4>
    6294: e08f0001     	add	r0, pc, r1
    6298: e5d31005     	ldrb	r1, [r3, #0x5]
    629c: ebfff635     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x272c
    62a0: e59440b4     	ldr	r4, [r4, #0xb4]
    62a4: e59f0024     	ldr	r0, [pc, #0x24]         @ 0x62d0 <_printOnsetModeInfo+0xb8>
    62a8: e5d41006     	ldrb	r1, [r4, #0x6]
    62ac: e08f0000     	add	r0, pc, r0
    62b0: e8bd4010     	pop	{r4, lr}
    62b4: eafff62f     	b	0x3b78 <.plt+0x47c>     @ imm = #-0x2744
    62b8: e4 ea 00 00  	.word	0x0000eae4
    62bc: dc ea 00 00  	.word	0x0000eadc
    62c0: dc ea 00 00  	.word	0x0000eadc
    62c4: e0 ea 00 00  	.word	0x0000eae0
    62c8: e4 ea 00 00  	.word	0x0000eae4
    62cc: ec ea 00 00  	.word	0x0000eaec
    62d0: ec ea 00 00  	.word	0x0000eaec

