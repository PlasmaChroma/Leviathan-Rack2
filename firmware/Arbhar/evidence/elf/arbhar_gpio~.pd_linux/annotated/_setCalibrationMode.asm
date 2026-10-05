0000f0e8 <_setCalibrationMode>:
    f0e8: e92d4070     	push	{r4, r5, r6, lr}
    f0ec: e1a04000     	mov	r4, r0
    f0f0: ebffd246     	bl	0x3a10 <.plt+0x314>     @ imm = #-0xb6e8  // CALL _checkForCalibrationFileOnBoot
    f0f4: e59f3058     	ldr	r3, [pc, #0x58]         @ 0xf154 <_setCalibrationMode+0x6c>  // u32=0x182b4; f32?=1.38722943e-40
    f0f8: e1a05000     	mov	r5, r0
    f0fc: e08f0003     	add	r0, pc, r3
    f100: e59011bc     	ldr	r1, [r0, #0x1bc]
    f104: e3510000     	cmp	r1, #0
    f108: da000004     	ble	0xf120 <_setCalibrationMode+0x38> @ imm = #0x10
    f10c: e2842a01     	add	r2, r4, #4096
    f110: ee075a90     	vmov	s15, r5
    f114: e5920db8     	ldr	r0, [r2, #0xdb8]
    f118: eeb80ae7     	vcvt.f32.s32	s0, s15
    f11c: ebffd2cb     	bl	0x3c50 <.plt+0x554>     @ imm = #-0xb4d4  // CALL outlet_float
    f120: e3550001     	cmp	r5, #1
    f124: e59fc02c     	ldr	r12, [pc, #0x2c]        @ 0xf158 <_setCalibrationMode+0x70>  // u32=0x6930; f32?=3.7734165e-41
    f128: 03a01063     	moveq	r1, #99
    f12c: 15d41030     	ldrbne	r1, [r4, #0x30]
    f130: 05c41030     	strbeq	r1, [r4, #0x30]
    f134: e08f000c     	add	r0, pc, r12
    f138: ebffd28e     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0xb5c8  // CALL post
    f13c: e59f3018     	ldr	r3, [pc, #0x18]         @ 0xf15c <_setCalibrationMode+0x74>  // u32=0x18270; f32?=1.38627654e-40
    f140: e08f0003     	add	r0, pc, r3
    f144: e59011bc     	ldr	r1, [r0, #0x1bc]
    f148: e2812001     	add	r2, r1, #1
    f14c: e58021bc     	str	r2, [r0, #0x1bc]
    f150: e8bd8070     	pop	{r4, r5, r6, pc}
    f154: b4 82 01 00  	.word	0x000182b4
    f158: 30 69 00 00  	.word	0x00006930
    f15c: 70 82 01 00  	.word	0x00018270

