00001060 <comport_help>:
    1060: e92d4070     	push	{r4, r5, r6, lr}
    1064: e2804a01     	add	r4, r0, #4096
    1068: e1a05000     	mov	r5, r0
    106c: edd47a29     	vldr	s15, [r4, #164]
    1070: e59f004c     	ldr	r0, [pc, #0x4c]         @ 0x10c4 <comport_help+0x64>
    1074: e1d41af0     	ldrsh	r1, [r4, #160]
    1078: e08f0000     	add	r0, pc, r0
    107c: eeb77ae7     	vcvt.f64.f32	d7, s15
    1080: ec532b17     	vmov	r2, r3, d7
    1084: ebfffeac     	bl	0xb3c <.plt+0x134>      @ imm = #-0x550
    1088: e1d43ab0     	ldrh	r3, [r4, #160]
    108c: e3530062     	cmp	r3, #98
    1090: 8a000004     	bhi	0x10a8 <comport_help+0x48> @ imm = #0x10
    1094: e595109c     	ldr	r1, [r5, #0x9c]
    1098: e59f2028     	ldr	r2, [pc, #0x28]         @ 0x10c8 <comport_help+0x68>
    109c: e5911000     	ldr	r1, [r1]
    10a0: e08f0002     	add	r0, pc, r2
    10a4: ebfffea4     	bl	0xb3c <.plt+0x134>      @ imm = #-0x570
    10a8: e59f601c     	ldr	r6, [pc, #0x1c]         @ 0x10cc <comport_help+0x6c>
    10ac: e08f0006     	add	r0, pc, r6
    10b0: ebfffea1     	bl	0xb3c <.plt+0x134>      @ imm = #-0x57c
    10b4: e59fc014     	ldr	r12, [pc, #0x14]        @ 0x10d0 <comport_help+0x70>
    10b8: e8bd4070     	pop	{r4, r5, r6, lr}
    10bc: e08f000c     	add	r0, pc, r12
    10c0: eafffe9d     	b	0xb3c <.plt+0x134>      @ imm = #-0x58c
    10c4: cc 2d 00 00  	.word	0x00002dcc
    10c8: c8 2d 00 00  	.word	0x00002dc8
    10cc: cc 2d 00 00  	.word	0x00002dcc
    10d0: c8 2d 00 00  	.word	0x00002dc8

