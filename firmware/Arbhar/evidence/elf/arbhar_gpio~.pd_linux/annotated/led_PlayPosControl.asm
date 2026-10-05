000068d0 <led_PlayPosControl>:
    68d0: e92d4030     	push	{r4, r5, lr}
    68d4: e24dd024     	sub	sp, sp, #36
    68d8: ed9f0a42     	vldr	s0, [pc, #264]          @ 0x69e8 <led_PlayPosControl+0x118>  // f32=99
    68dc: e1a05000     	mov	r5, r0
    68e0: ebfff3c0     	bl	0x37e8 <.plt+0xec>      @ imm = #-0x3100  // CALL readFromSharedMem
    68e4: e59f3114     	ldr	r3, [pc, #0x114]        @ 0x6a00 <led_PlayPosControl+0x130>  // u32=0x20ac4; f32?=1.87532971e-40
    68e8: eddf7a3f     	vldr	s15, [pc, #252]         @ 0x69ec <led_PlayPosControl+0x11c>  // f32=0.000147916668
    68ec: e08f1003     	add	r1, pc, r3
    68f0: e5912024     	ldr	r2, [r1, #0x24]
    68f4: ed9f7a3d     	vldr	s14, [pc, #244]         @ 0x69f0 <led_PlayPosControl+0x120>  // f32=36
    68f8: ee200a27     	vmul.f32	s0, s0, s15
    68fc: eeb40ac7     	vcmpe.f32	s0, s14
    6900: eef1fa10     	vmrs	APSR_nzcv, fpscr
    6904: 5e707a47     	vsubpl.f32	s15, s0, s14
    6908: 5d9f0a39     	vldrpl	s0, [pc, #228]          @ 0x69f4 <led_PlayPosControl+0x124>
    690c: 5d9f7a39     	vldrpl	s14, [pc, #228]         @ 0x69f8 <led_PlayPosControl+0x128>
    6910: 4ddf7a39     	vldrmi	s15, [pc, #228]         @ 0x69fc <led_PlayPosControl+0x12c>
    6914: 5e070a87     	vmlapl.f32	s0, s15, s14
    6918: 4e200a27     	vmulmi.f32	s0, s0, s15
    691c: eefd0ac0     	vcvt.s32.f32	s1, s0
    6920: ee104a90     	vmov	r4, s1
    6924: e1520004     	cmp	r2, r4
    6928: 0a000004     	beq	0x6940 <led_PlayPosControl+0x70> @ imm = #0x10
    692c: eeb30b09     	vmov.f64	d0, #2.500000e+01
    6930: e59500d4     	ldr	r0, [r5, #0xd4]
    6934: e3a0c001     	mov	r12, #1
    6938: e581c028     	str	r12, [r1, #0x28]
    693c: ebfff3fa     	bl	0x392c <.plt+0x230>     @ imm = #-0x3018  // CALL clock_delay
    6940: e59f00bc     	ldr	r0, [pc, #0xbc]         @ 0x6a04 <led_PlayPosControl+0x134>  // u32=0x20a6c; f32?=1.87409657e-40
    6944: e08fe000     	add	lr, pc, r0
    6948: e59e3028     	ldr	r3, [lr, #0x28]
    694c: e3530000     	cmp	r3, #0
    6950: da000014     	ble	0x69a8 <led_PlayPosControl+0xd8> @ imm = #0x50
    6954: ee014a10     	vmov	s2, r4
    6958: e5d5c030     	ldrb	r12, [r5, #0x30]
    695c: e3a015fe     	mov	r1, #1065353216
    6960: e3a02000     	mov	r2, #0
    6964: eef81ac1     	vcvt.f32.s32	s3, s2
    6968: e35c0062     	cmp	r12, #98
    696c: e344230d     	movt	r2, #0x430d
    6970: e3a00000     	mov	r0, #0
    6974: e3a03000     	mov	r3, #0
    6978: e58e0028     	str	r0, [lr, #0x28]
    697c: e344330c     	movt	r3, #0x430c
    6980: e3a0e001     	mov	lr, #1
    6984: e58d200c     	str	r2, [sp, #0xc]
    6988: e58de000     	str	lr, [sp]
    698c: e58de008     	str	lr, [sp, #0x8]
    6990: e58de010     	str	lr, [sp, #0x10]
    6994: e58de018     	str	lr, [sp, #0x18]
    6998: e58d1014     	str	r1, [sp, #0x14]
    699c: e58d301c     	str	r3, [sp, #0x1c]
    69a0: edcd1a01     	vstr	s3, [sp, #4]
    69a4: 9a000004     	bls	0x69bc <led_PlayPosControl+0xec> @ imm = #0x10
    69a8: e59fc058     	ldr	r12, [pc, #0x58]        @ 0x6a08 <led_PlayPosControl+0x138>  // u32=0x20a04; f32?=1.87263922e-40
    69ac: e08f200c     	add	r2, pc, r12
    69b0: e5824024     	str	r4, [r2, #0x24]
    69b4: e28dd024     	add	sp, sp, #36
    69b8: e8bd8030     	pop	{r4, r5, pc}
    69bc: e59f1048     	ldr	r1, [pc, #0x48]         @ 0x6a0c <led_PlayPosControl+0x13c>  // u32=0xe0f0; f32?=8.06923708e-41
    69c0: e2855a01     	add	r5, r5, #4096
    69c4: e08f0001     	add	r0, pc, r1
    69c8: e5955dac     	ldr	r5, [r5, #0xdac]
    69cc: ebfff355     	bl	0x3728 <.plt+0x2c>      @ imm = #-0x32ac  // CALL gensym
    69d0: e1a0300d     	mov	r3, sp
    69d4: e3a02004     	mov	r2, #4
    69d8: e1a01000     	mov	r1, r0
    69dc: e1a00005     	mov	r0, r5
    69e0: ebfff4a3     	bl	0x3c74 <.plt+0x578>     @ imm = #-0x2d74  // CALL outlet_list
    69e4: eaffffef     	b	0x69a8 <led_PlayPosControl+0xd8> @ imm = #-0x44
    69e8: 00 00 c6 42  	.word	0x42c60000
    69ec: 14 1a 1b 39  	.word	0x391b1a14
    69f0: 00 00 10 42  	.word	0x42100000
    69f4: 00 00 38 42  	.word	0x42380000
    69f8: 8e e3 38 3f  	.word	0x3f38e38e
    69fc: 39 8e a3 3f  	.word	0x3fa38e39
    6a00: c4 0a 02 00  	.word	0x00020ac4
    6a04: 6c 0a 02 00  	.word	0x00020a6c
    6a08: 04 0a 02 00  	.word	0x00020a04
    6a0c: f0 e0 00 00  	.word	0x0000e0f0

