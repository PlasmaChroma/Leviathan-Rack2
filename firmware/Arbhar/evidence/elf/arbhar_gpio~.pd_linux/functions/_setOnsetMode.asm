00006424 <_setOnsetMode>:
    6424: ee071a90     	vmov	s15, r1
    6428: e59030b4     	ldr	r3, [r0, #0xb4]
    642c: ed9f7a7b     	vldr	s14, [pc, #492]         @ 0x6620 <_setOnsetMode+0x1fc>
    6430: e92d47f0     	push	{r4, r5, r6, r7, r8, r9, r10, lr}
    6434: e1a05000     	mov	r5, r0
    6438: eef86ae7     	vcvt.f32.s32	s13, s15
    643c: e5d02030     	ldrb	r2, [r0, #0x30]
    6440: ed9f0a77     	vldr	s0, [pc, #476]          @ 0x6624 <_setOnsetMode+0x200>
    6444: e24dd010     	sub	sp, sp, #16
    6448: e5d060b9     	ldrb	r6, [r0, #0xb9]
    644c: e3520062     	cmp	r2, #98
    6450: e5d37000     	ldrb	r7, [r3]
    6454: ee770a66     	vsub.f32	s1, s14, s13
    6458: ee201a80     	vmul.f32	s2, s1, s0
    645c: eef60b00     	vmov.f64	d16, #5.000000e-01
    6460: eeb72ac1     	vcvt.f64.f32	d2, s2
    6464: ee327b20     	vadd.f64	d7, d2, d16
    6468: eefc1bc7     	vcvt.u32.f64	s3, d7
    646c: ee114a90     	vmov	r4, s3
    6470: e2840001     	add	r0, r4, #1
    6474: e6ef4070     	uxtb	r4, r0
    6478: 8a00002f     	bhi	0x653c <_setOnsetMode+0x118> @ imm = #0xbc
    647c: e59f11a4     	ldr	r1, [pc, #0x1a4]        @ 0x6628 <_setOnsetMode+0x204>
    6480: e08f8001     	add	r8, pc, r1
    6484: e5d89020     	ldrb	r9, [r8, #0x20]
    6488: e1590004     	cmp	r9, r4
    648c: 0a000052     	beq	0x65dc <_setOnsetMode+0x1b8> @ imm = #0x148
    6490: e59fa194     	ldr	r10, [pc, #0x194]       @ 0x662c <_setOnsetMode+0x208>
    6494: e266c001     	rsb	r12, r6, #1
    6498: e08f900a     	add	r9, pc, r10
    649c: e6ef807c     	uxtb	r8, r12
    64a0: e5d9e003     	ldrb	lr, [r9, #0x3]
    64a4: e15e0008     	cmp	lr, r8
    64a8: 1a00000c     	bne	0x64e0 <_setOnsetMode+0xbc> @ imm = #0x30
    64ac: e1570004     	cmp	r7, r4
    64b0: 0a000058     	beq	0x6618 <_setOnsetMode+0x1f4> @ imm = #0x160
    64b4: e3560000     	cmp	r6, #0
    64b8: 1a000028     	bne	0x6560 <_setOnsetMode+0x13c> @ imm = #0xa0
    64bc: e59fa16c     	ldr	r10, [pc, #0x16c]       @ 0x6630 <_setOnsetMode+0x20c>
    64c0: e1a01007     	mov	r1, r7
    64c4: e5c560b9     	strb	r6, [r5, #0xb9]
    64c8: e1a00005     	mov	r0, r5
    64cc: e08f200a     	add	r2, pc, r10
    64d0: e5c24020     	strb	r4, [r2, #0x20]
    64d4: ebfff59e     	bl	0x3b54 <.plt+0x458>     @ imm = #-0x2988
    64d8: e28dd010     	add	sp, sp, #16
    64dc: e8bd87f0     	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
    64e0: e59f214c     	ldr	r2, [pc, #0x14c]        @ 0x6634 <_setOnsetMode+0x210>
    64e4: e1a01008     	mov	r1, r8
    64e8: e08f0002     	add	r0, pc, r2
    64ec: ebfff5a1     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x297c
    64f0: ee028a90     	vmov	s5, r8
    64f4: e59f013c     	ldr	r0, [pc, #0x13c]        @ 0x6638 <_setOnsetMode+0x214>
    64f8: e3a03001     	mov	r3, #1
    64fc: eeb83a62     	vcvt.f32.u32	s6, s5
    6500: e58d3000     	str	r3, [sp]
    6504: e08f0000     	add	r0, pc, r0
    6508: e58d3008     	str	r3, [sp, #0x8]
    650c: e595a070     	ldr	r10, [r5, #0x70]
    6510: e3a01000     	mov	r1, #0
    6514: e344131d     	movt	r1, #0x431d
    6518: e58d100c     	str	r1, [sp, #0xc]
    651c: ed8d3a01     	vstr	s6, [sp, #4]
    6520: ebfff480     	bl	0x3728 <.plt+0x2c>      @ imm = #-0x2e00
    6524: e1a0300d     	mov	r3, sp
    6528: e3a02002     	mov	r2, #2
    652c: e1a01000     	mov	r1, r0
    6530: e1a0000a     	mov	r0, r10
    6534: ebfff5ce     	bl	0x3c74 <.plt+0x578>     @ imm = #-0x28c8
    6538: e5c98003     	strb	r8, [r9, #0x3]
    653c: e1570004     	cmp	r7, r4
    6540: 0a000023     	beq	0x65d4 <_setOnsetMode+0x1b0> @ imm = #0x8c
    6544: e3560000     	cmp	r6, #0
    6548: 0affffdb     	beq	0x64bc <_setOnsetMode+0x98> @ imm = #-0x94
    654c: e59f90e8     	ldr	r9, [pc, #0xe8]         @ 0x663c <_setOnsetMode+0x218>
    6550: e08fc009     	add	r12, pc, r9
    6554: e5dc8020     	ldrb	r8, [r12, #0x20]
    6558: e1580004     	cmp	r8, r4
    655c: 0affffd6     	beq	0x64bc <_setOnsetMode+0x98> @ imm = #-0xa8
    6560: e284e00f     	add	lr, r4, #15
    6564: e3a07001     	mov	r7, #1
    6568: e3a0a002     	mov	r10, #2
    656c: e1a0118e     	lsl	r1, lr, #3
    6570: e0852001     	add	r2, r5, r1
    6574: e2813003     	add	r3, r1, #3
    6578: e0850003     	add	r0, r5, r3
    657c: e58500b4     	str	r0, [r5, #0xb4]
    6580: e5d2900a     	ldrb	r9, [r2, #0xa]
    6584: ee039a90     	vmov	s7, r9
    6588: eeb80a63     	vcvt.f32.u32	s0, s7
    658c: ebfff4f8     	bl	0x3974 <.plt+0x278>     @ imm = #-0x2c20
    6590: e1a00005     	mov	r0, r5
    6594: ebfff565     	bl	0x3b30 <.plt+0x434>     @ imm = #-0x2a6c
    6598: e59fc0a0     	ldr	r12, [pc, #0xa0]        @ 0x6640 <_setOnsetMode+0x21c>
    659c: e5c57038     	strb	r7, [r5, #0x38]
    65a0: e08f000c     	add	r0, pc, r12
    65a4: e58da000     	str	r10, [sp]
    65a8: ebfff45e     	bl	0x3728 <.plt+0x2c>      @ imm = #-0x2e88
    65ac: ee044a10     	vmov	s8, r4
    65b0: e5d58030     	ldrb	r8, [r5, #0x30]
    65b4: eef84a44     	vcvt.f32.u32	s9, s8
    65b8: e58d7008     	str	r7, [sp, #0x8]
    65bc: e3580062     	cmp	r8, #98
    65c0: edcd4a03     	vstr	s9, [sp, #12]
    65c4: e58d0004     	str	r0, [sp, #0x4]
    65c8: 9a000007     	bls	0x65ec <_setOnsetMode+0x1c8> @ imm = #0x1c
    65cc: e1a07004     	mov	r7, r4
    65d0: eaffffb9     	b	0x64bc <_setOnsetMode+0x98> @ imm = #-0x11c
    65d4: e3a06001     	mov	r6, #1
    65d8: eaffffdb     	b	0x654c <_setOnsetMode+0x128> @ imm = #-0x94
    65dc: e1570004     	cmp	r7, r4
    65e0: 1affffd7     	bne	0x6544 <_setOnsetMode+0x120> @ imm = #-0xa4
    65e4: e3a06001     	mov	r6, #1
    65e8: eaffffb3     	b	0x64bc <_setOnsetMode+0x98> @ imm = #-0x134
    65ec: e59f7050     	ldr	r7, [pc, #0x50]         @ 0x6644 <_setOnsetMode+0x220>
    65f0: e285ea01     	add	lr, r5, #4096
    65f4: e08f0007     	add	r0, pc, r7
    65f8: e59e9db4     	ldr	r9, [lr, #0xdb4]
    65fc: ebfff449     	bl	0x3728 <.plt+0x2c>      @ imm = #-0x2edc
    6600: e1a0200a     	mov	r2, r10
    6604: e1a0300d     	mov	r3, sp
    6608: e1a01000     	mov	r1, r0
    660c: e1a00009     	mov	r0, r9
    6610: ebfff597     	bl	0x3c74 <.plt+0x578>     @ imm = #-0x29a4
    6614: eaffffec     	b	0x65cc <_setOnsetMode+0x1a8> @ imm = #-0x50
    6618: e3a06001     	mov	r6, #1
    661c: eaffffcf     	b	0x6560 <_setOnsetMode+0x13c> @ imm = #-0xc4
    6620: 00 f0 7f 45  	.word	0x457ff000
    6624: 02 0c c0 3a  	.word	0x3ac00c02
    6628: 30 0f 02 00  	.word	0x00020f30
    662c: 4c 0e 02 00  	.word	0x00020e4c
    6630: e4 0e 02 00  	.word	0x00020ee4
    6634: cc e8 00 00  	.word	0x0000e8cc
    6638: b0 e5 00 00  	.word	0x0000e5b0
    663c: 60 0e 02 00  	.word	0x00020e60
    6640: 24 e8 00 00  	.word	0x0000e824
    6644: c0 e4 00 00  	.word	0x0000e4c0

