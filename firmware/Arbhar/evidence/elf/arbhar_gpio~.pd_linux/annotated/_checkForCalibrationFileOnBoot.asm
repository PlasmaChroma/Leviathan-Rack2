0000f090 <_checkForCalibrationFileOnBoot>:
    f090: e59f1040     	ldr	r1, [pc, #0x40]         @ 0xf0d8 <_checkForCalibrationFileOnBoot+0x48>  // u32=0x6970; f32?=3.78238481e-41
    f094: e59f0040     	ldr	r0, [pc, #0x40]         @ 0xf0dc <_checkForCalibrationFileOnBoot+0x4c>  // u32=0x6970; f32?=3.78238481e-41
    f098: e92d4010     	push	{r4, lr}
    f09c: e08f1001     	add	r1, pc, r1
    f0a0: e08f0000     	add	r0, pc, r0
    f0a4: ebffd1c3     	bl	0x37b8 <.plt+0xbc>      @ imm = #-0xb8f4  // CALL fopen
    f0a8: e3500000     	cmp	r0, #0
    f0ac: 0a000004     	beq	0xf0c4 <_checkForCalibrationFileOnBoot+0x34> @ imm = #0x10
    f0b0: e59f2028     	ldr	r2, [pc, #0x28]         @ 0xf0e0 <_checkForCalibrationFileOnBoot+0x50>  // u32=0x6998; f32?=3.78799001e-41
    f0b4: e08f0002     	add	r0, pc, r2
    f0b8: ebffd2ae     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0xb548  // CALL post
    f0bc: e3a00000     	mov	r0, #0
    f0c0: e8bd8010     	pop	{r4, pc}
    f0c4: e59f3018     	ldr	r3, [pc, #0x18]         @ 0xf0e4 <_checkForCalibrationFileOnBoot+0x54>  // u32=0x695c; f32?=3.77958222e-41
    f0c8: e08f0003     	add	r0, pc, r3
    f0cc: ebffd2a9     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0xb55c  // CALL post
    f0d0: e3a00001     	mov	r0, #1
    f0d4: e8bd8010     	pop	{r4, pc}
    f0d8: 70 69 00 00  	.word	0x00006970
    f0dc: 70 69 00 00  	.word	0x00006970
    f0e0: 98 69 00 00  	.word	0x00006998
    f0e4: 5c 69 00 00  	.word	0x0000695c

