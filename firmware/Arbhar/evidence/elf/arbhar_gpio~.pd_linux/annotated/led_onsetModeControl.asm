000062d4 <led_onsetModeControl>:
    62d4: e3510000     	cmp	r1, #0
    62d8: 12613007     	rsbne	r3, r1, #7
    62dc: 13a01001     	movne	r1, #1
    62e0: 0d9f7a26     	vldreq	s14, [pc, #152]         @ 0x6380 <led_onsetModeControl+0xac>
    62e4: 11a01311     	lslne	r1, r1, r3
    62e8: 1261307e     	rsbne	r3, r1, #126
    62ec: 1e071a90     	vmovne	s15, r1
    62f0: e3a01000     	mov	r1, #0
    62f4: 1e073a10     	vmovne	s14, r3
    62f8: e3441312     	movt	r1, #0x4312
    62fc: 1ef87ae7     	vcvtne.f32.s32	s15, s15
    6300: e3a03001     	mov	r3, #1
    6304: e92d4010     	push	{r4, lr}
    6308: e24dd020     	sub	sp, sp, #32
    630c: 0ddf7a1c     	vldreq	s15, [pc, #112]         @ 0x6384 <led_onsetModeControl+0xb0>
    6310: e3a04000     	mov	r4, #0
    6314: e5d02030     	ldrb	r2, [r0, #0x30]
    6318: e3444313     	movt	r4, #0x4313
    631c: e58d3000     	str	r3, [sp]
    6320: e58d100c     	str	r1, [sp, #0xc]
    6324: 1eb87ac7     	vcvtne.f32.s32	s14, s14
    6328: e3520062     	cmp	r2, #98
    632c: edcd7a01     	vstr	s15, [sp, #4]
    6330: e58d3008     	str	r3, [sp, #0x8]
    6334: e58d3010     	str	r3, [sp, #0x10]
    6338: e58d401c     	str	r4, [sp, #0x1c]
    633c: e58d3018     	str	r3, [sp, #0x18]
    6340: ed8d7a05     	vstr	s14, [sp, #20]
    6344: 9a000001     	bls	0x6350 <led_onsetModeControl+0x7c> @ imm = #0x4
    6348: e28dd020     	add	sp, sp, #32
    634c: e8bd8010     	pop	{r4, pc}
    6350: e1a0c000     	mov	r12, r0
    6354: e59f002c     	ldr	r0, [pc, #0x2c]         @ 0x6388 <led_onsetModeControl+0xb4>  // u32=0xe758; f32?=8.29905003e-41
    6358: e59c4070     	ldr	r4, [r12, #0x70]
    635c: e08f0000     	add	r0, pc, r0
    6360: ebfff4f0     	bl	0x3728 <.plt+0x2c>      @ imm = #-0x2c40  // CALL gensym
    6364: e1a0300d     	mov	r3, sp
    6368: e3a02004     	mov	r2, #4
    636c: e1a01000     	mov	r1, r0
    6370: e1a00004     	mov	r0, r4
    6374: ebfff63e     	bl	0x3c74 <.plt+0x578>     @ imm = #-0x2708  // CALL outlet_list
    6378: e28dd020     	add	sp, sp, #32
    637c: e8bd8010     	pop	{r4, pc}
    6380: 00 00 78 42  	.word	0x42780000
    6384: 00 00 80 42  	.word	0x42800000
    6388: 58 e7 00 00  	.word	0x0000e758

