000023d4 <stereo_in_tilde_dsp>:
    23d4: e5913000     	ldr	r3, [r1]
    23d8: e1a0c001     	mov	r12, r1
    23dc: e52de004     	str	lr, [sp, #-0x4]!
    23e0: e1a02000     	mov	r2, r0
    23e4: e593e000     	ldr	lr, [r3]
    23e8: e24dd014     	sub	sp, sp, #20
    23ec: e59c000c     	ldr	r0, [r12, #0xc]
    23f0: e3a01006     	mov	r1, #6
    23f4: e5933004     	ldr	r3, [r3, #0x4]
    23f8: e58de00c     	str	lr, [sp, #0xc]
    23fc: e590e004     	ldr	lr, [r0, #0x4]
    2400: e59c0008     	ldr	r0, [r12, #0x8]
    2404: e59cc004     	ldr	r12, [r12, #0x4]
    2408: e58de008     	str	lr, [sp, #0x8]
    240c: e590e004     	ldr	lr, [r0, #0x4]
    2410: e59f0018     	ldr	r0, [pc, #0x18]         @ 0x2430 <stereo_in_tilde_dsp+0x5c>  // u32=0x908; f32?=3.23980205e-42
    2414: e58de004     	str	lr, [sp, #0x4]
    2418: e08f0000     	add	r0, pc, r0
    241c: e59cc004     	ldr	r12, [r12, #0x4]
    2420: e58dc000     	str	r12, [sp]
    2424: ebffff76     	bl	0x2204 <.plt+0x1a0>     @ imm = #-0x228  // CALL dsp_add
    2428: e28dd014     	add	sp, sp, #20
    242c: e49df004     	ldr	pc, [sp], #4
    2430: 08 09 00 00  	.word	0x00000908

