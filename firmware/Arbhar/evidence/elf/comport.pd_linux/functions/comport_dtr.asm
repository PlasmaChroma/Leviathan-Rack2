00001fb4 <comport_dtr>:
    1fb4: e92d40f0     	push	{r4, r5, r6, r7, lr}
    1fb8: e24dd00c     	sub	sp, sp, #12
    1fbc: e5905020     	ldr	r5, [r0, #0x20]
    1fc0: e3750001     	cmn	r5, #1
    1fc4: 0a000021     	beq	0x2050 <comport_dtr+0x9c> @ imm = #0x84
    1fc8: eefd7ac0     	vcvt.s32.f32	s15, s0
    1fcc: e28d7004     	add	r7, sp, #4
    1fd0: e1a04000     	mov	r4, r0
    1fd4: e1a02007     	mov	r2, r7
    1fd8: e59f1078     	ldr	r1, [pc, #0x78]         @ 0x2058 <comport_dtr+0xa4>
    1fdc: e1a00005     	mov	r0, r5
    1fe0: ee176a90     	vmov	r6, s15
    1fe4: ebfffaaa     	bl	0xa94 <.plt+0x8c>       @ imm = #-0x1558
    1fe8: e59d3004     	ldr	r3, [sp, #0x4]
    1fec: e1a02007     	mov	r2, r7
    1ff0: e3560000     	cmp	r6, #0
    1ff4: 03c33002     	biceq	r3, r3, #2
    1ff8: 13833002     	orrne	r3, r3, #2
    1ffc: e1a00005     	mov	r0, r5
    2000: e59f1054     	ldr	r1, [pc, #0x54]         @ 0x205c <comport_dtr+0xa8>
    2004: e58d3004     	str	r3, [sp, #0x4]
    2008: ebfffaa1     	bl	0xa94 <.plt+0x8c>       @ imm = #-0x157c
    200c: e5940020     	ldr	r0, [r4, #0x20]
    2010: e3700001     	cmn	r0, #1
    2014: 0a00000d     	beq	0x2050 <comport_dtr+0x9c> @ imm = #0x34
    2018: e2841a01     	add	r1, r4, #4096
    201c: e59120e0     	ldr	r2, [r1, #0xe0]
    2020: e3520000     	cmp	r2, #0
    2024: da000009     	ble	0x2050 <comport_dtr+0x9c> @ imm = #0x24
    2028: e296e000     	adds	lr, r6, #0
    202c: 13a0e001     	movne	lr, #1
    2030: e594c09c     	ldr	r12, [r4, #0x9c]
    2034: ee00ea10     	vmov	s0, lr
    2038: e59f5020     	ldr	r5, [pc, #0x20]         @ 0x2060 <comport_dtr+0xac>
    203c: e59c1000     	ldr	r1, [r12]
    2040: e08f0005     	add	r0, pc, r5
    2044: eeb87bc0     	vcvt.f64.s32	d7, s0
    2048: ec532b17     	vmov	r2, r3, d7
    204c: ebfffaba     	bl	0xb3c <.plt+0x134>      @ imm = #-0x1518
    2050: e28dd00c     	add	sp, sp, #12
    2054: e8bd80f0     	pop	{r4, r5, r6, r7, pc}
    2058: 15 54 00 00  	.word	0x00005415
    205c: 18 54 00 00  	.word	0x00005418
    2060: 54 26 00 00  	.word	0x00002654

