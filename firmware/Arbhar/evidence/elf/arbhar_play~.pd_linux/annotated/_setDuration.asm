00002ed0 <_setDuration>:
    2ed0: eefd7ac0     	vcvt.s32.f32	s15, s0
    2ed4: ee171a90     	vmov	r1, s15
    2ed8: e3510000     	cmp	r1, #0
    2edc: da000001     	ble	0x2ee8 <_setDuration+0x18> @ imm = #0x4
    2ee0: edc07a29     	vstr	s15, [r0, #164]
    2ee4: e12fff1e     	bx	lr
    2ee8: e59f0004     	ldr	r0, [pc, #0x4]          @ 0x2ef4 <_setDuration+0x24>  // u32=0x9c20; f32?=5.6007097e-41
    2eec: e08f0000     	add	r0, pc, r0
    2ef0: eafffdc9     	b	0x261c <.plt+0x23c>     @ imm = #-0x8dc  // CALL post
    2ef4: 20 9c 00 00  	.word	0x00009c20

