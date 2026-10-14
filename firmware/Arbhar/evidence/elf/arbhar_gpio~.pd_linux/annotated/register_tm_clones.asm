00003e50 <register_tm_clones>:
    3e50: e59f0038     	ldr	r0, [pc, #0x38]         @ 0x3e90 <register_tm_clones+0x40>  // u32=0x23554; f32?=2.02801519e-40
    3e54: e59f3038     	ldr	r3, [pc, #0x38]         @ 0x3e94 <register_tm_clones+0x44>  // u32=0x23550; f32?=2.02795914e-40
    3e58: e08f0000     	add	r0, pc, r0
    3e5c: e08f3003     	add	r3, pc, r3
    3e60: e0431000     	sub	r1, r3, r0
    3e64: e59f302c     	ldr	r3, [pc, #0x2c]         @ 0x3e98 <register_tm_clones+0x48>  // u32=0x2318c; f32?=2.01445062e-40
    3e68: e1a01141     	asr	r1, r1, #2
    3e6c: e08f3003     	add	r3, pc, r3
    3e70: e0811fa1     	add	r1, r1, r1, lsr #31
    3e74: e1b010c1     	asrs	r1, r1, #1
    3e78: 012fff1e     	bxeq	lr
    3e7c: e59f2018     	ldr	r2, [pc, #0x18]         @ 0x3e9c <register_tm_clones+0x4c>  // u32=0x2d0; f32?=1.00893489e-42
    3e80: e7933002     	ldr	r3, [r3, r2]
    3e84: e3530000     	cmp	r3, #0
    3e88: 012fff1e     	bxeq	lr
    3e8c: e12fff13     	bx	r3
    3e90: 54 35 02 00  	.word	0x00023554
    3e94: 50 35 02 00  	.word	0x00023550
    3e98: 8c 31 02 00  	.word	0x0002318c
    3e9c: d0 02 00 00  	.word	0x000002d0

