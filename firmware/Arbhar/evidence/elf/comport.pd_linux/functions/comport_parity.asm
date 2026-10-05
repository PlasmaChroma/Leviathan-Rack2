00001100 <comport_parity>:
    1100: eefd7ac0     	vcvt.s32.f32	s15, s0
    1104: e92d4070     	push	{r4, r5, r6, lr}
    1108: ed2d8b02     	vpush	{d8}
    110c: ee173a90     	vmov	r3, s15
    1110: e3730001     	cmn	r3, #1
    1114: 0a000023     	beq	0x11a8 <comport_parity+0xa8> @ imm = #0x8c
    1118: e3530001     	cmp	r3, #1
    111c: 0a00001b     	beq	0x1190 <comport_parity+0x90> @ imm = #0x6c
    1120: ed9f8a2d     	vldr	s16, [pc, #180]         @ 0x11dc <comport_parity+0xdc>
    1124: e5901068     	ldr	r1, [r0, #0x68]
    1128: e3c12c01     	bic	r2, r1, #256
    112c: e5802068     	str	r2, [r0, #0x68]
    1130: e5901020     	ldr	r1, [r0, #0x20]
    1134: e3710001     	cmn	r1, #1
    1138: 0a000012     	beq	0x1188 <comport_parity+0x88> @ imm = #0x48
    113c: e1a04000     	mov	r4, r0
    1140: e2842060     	add	r2, r4, #96
    1144: e1a00001     	mov	r0, r1
    1148: e3a01002     	mov	r1, #2
    114c: ebfffe56     	bl	0xaac <.plt+0xa4>       @ imm = #-0x6a8
    1150: e3700001     	cmn	r0, #1
    1154: 0a000018     	beq	0x11bc <comport_parity+0xbc> @ imm = #0x60
    1158: e2845a01     	add	r5, r4, #4096
    115c: e59500e0     	ldr	r0, [r5, #0xe0]
    1160: e3500000     	cmp	r0, #0
    1164: da000006     	ble	0x1184 <comport_parity+0x84> @ imm = #0x18
    1168: eeb77ac8     	vcvt.f64.f32	d7, s16
    116c: e594e09c     	ldr	lr, [r4, #0x9c]
    1170: e59f6070     	ldr	r6, [pc, #0x70]         @ 0x11e8 <comport_parity+0xe8>
    1174: e59e1000     	ldr	r1, [lr]
    1178: e08f0006     	add	r0, pc, r6
    117c: ec532b17     	vmov	r2, r3, d7
    1180: ebfffe6d     	bl	0xb3c <.plt+0x134>      @ imm = #-0x64c
    1184: ed858a2b     	vstr	s16, [r5, #172]
    1188: ecbd8b02     	vpop	{d8}
    118c: e8bd8070     	pop	{r4, r5, r6, pc}
    1190: e5904068     	ldr	r4, [r0, #0x68]
    1194: ed9f8a11     	vldr	s16, [pc, #68]          @ 0x11e0 <comport_parity+0xe0>
    1198: e3c45c02     	bic	r5, r4, #512
    119c: e3856c01     	orr	r6, r5, #256
    11a0: e5806068     	str	r6, [r0, #0x68]
    11a4: eaffffe1     	b	0x1130 <comport_parity+0x30> @ imm = #-0x7c
    11a8: e590c068     	ldr	r12, [r0, #0x68]
    11ac: ed9f8a0c     	vldr	s16, [pc, #48]          @ 0x11e4 <comport_parity+0xe4>
    11b0: e38c3c03     	orr	r3, r12, #768
    11b4: e5803068     	str	r3, [r0, #0x68]
    11b8: eaffffdc     	b	0x1130 <comport_parity+0x30> @ imm = #-0x90
    11bc: ecbd8b02     	vpop	{d8}
    11c0: e1a00004     	mov	r0, r4
    11c4: e594209c     	ldr	r2, [r4, #0x9c]
    11c8: e59fc01c     	ldr	r12, [pc, #0x1c]        @ 0x11ec <comport_parity+0xec>
    11cc: e8bd4070     	pop	{r4, r5, r6, lr}
    11d0: e08f100c     	add	r1, pc, r12
    11d4: e5922000     	ldr	r2, [r2]
    11d8: eafffe81     	b	0xbe4 <.plt+0x1dc>      @ imm = #-0x5fc
    11dc: 00 00 00 00  	.word	0x00000000
    11e0: 00 00 80 3f  	.word	0x3f800000
    11e4: 00 00 80 bf  	.word	0xbf800000
    11e8: f0 30 00 00  	.word	0x000030f0
    11ec: c4 30 00 00  	.word	0x000030c4

