00001438 <arbhar_shmem_clear>:
    1438: eefd7ac0     	vcvt.s32.f32	s15, s0
    143c: e92d4030     	push	{r4, r5, lr}
    1440: e1a05000     	mov	r5, r0
    1444: ed2d8b02     	vpush	{d8}
    1448: ee173a90     	vmov	r3, s15
    144c: e24dd014     	sub	sp, sp, #20
    1450: eeb08a40     	vmov.f32	s16, s0
    1454: e0800103     	add	r0, r0, r3, lsl #2
    1458: e590208c     	ldr	r2, [r0, #0x8c]
    145c: e3520000     	cmp	r2, #0
    1460: da000003     	ble	0x1474 <arbhar_shmem_clear+0x3c> @ imm = #0xc
    1464: e1a02102     	lsl	r2, r2, #2
    1468: e5900058     	ldr	r0, [r0, #0x58]
    146c: e3a01000     	mov	r1, #0
    1470: ebfffda3     	bl	0xb04 <.plt+0x110>      @ imm = #-0x974
    1474: e59f1048     	ldr	r1, [pc, #0x48]         @ 0x14c4 <arbhar_shmem_clear+0x8c>
    1478: e3a04002     	mov	r4, #2
    147c: e58d4000     	str	r4, [sp]
    1480: e08f0001     	add	r0, pc, r1
    1484: ebfffd5f     	bl	0xa08 <.plt+0x14>       @ imm = #-0xa84
    1488: e59fc038     	ldr	r12, [pc, #0x38]        @ 0x14c8 <arbhar_shmem_clear+0x90>
    148c: e3a03001     	mov	r3, #1
    1490: e5955054     	ldr	r5, [r5, #0x54]
    1494: ed8d8a03     	vstr	s16, [sp, #12]
    1498: e98d0009     	stmib	sp, {r0, r3}
    149c: e08f000c     	add	r0, pc, r12
    14a0: ebfffd58     	bl	0xa08 <.plt+0x14>       @ imm = #-0xaa0
    14a4: e1a02004     	mov	r2, r4
    14a8: e1a0300d     	mov	r3, sp
    14ac: e1a01000     	mov	r1, r0
    14b0: e1a00005     	mov	r0, r5
    14b4: ebfffdad     	bl	0xb70 <.plt+0x17c>      @ imm = #-0x94c
    14b8: e28dd014     	add	sp, sp, #20
    14bc: ecbd8b02     	vpop	{d8}
    14c0: e8bd8030     	pop	{r4, r5, pc}
    14c4: 74 17 00 00  	.word	0x00001774
    14c8: 50 17 00 00  	.word	0x00001750

