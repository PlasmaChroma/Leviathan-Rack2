; lubadh::OverdubRecMode::interpretButtonPress(int)
; VA 0x3a4ec size 180

   3a4ec: e92d4010     	push	{r4, lr}
   3a4f0: e3510001     	cmp	r1, #1
   3a4f4: e1a04000     	mov	r4, r0
   3a4f8: da000016     	ble	0x3a558
   3a4fc: e3510007     	cmp	r1, #7
   3a500: 18bd8010     	popne	{r4, pc}
   3a504: e5900008     	ldr	r0, [r0, #0x8]
   3a508: e3a01003     	mov	r1, #3
   3a50c: ebfffec1     	bl	0x3a018
   3a510: e5940008     	ldr	r0, [r4, #0x8]
   3a514: e2803a01     	add	r3, r0, #4096
   3a518: e5d33968     	ldrb	r3, [r3, #0x968]
   3a51c: e3530000     	cmp	r3, #0
   3a520: 0a000017     	beq	0x3a584
   3a524: e59030e8     	ldr	r3, [r0, #0xe8]
   3a528: e2800d65     	add	r0, r0, #6464
   3a52c: ed9f7a1a     	vldr	s14, [pc, #104]         @ 0x3a59c ; float 256
   3a530: e2800028     	add	r0, r0, #40
   3a534: edd37a19     	vldr	s15, [r3, #100]
   3a538: eef87ae7     	vcvt.f32.s32	s15, s15
   3a53c: ee870a27     	vdiv.f32	s0, s14, s15
   3a540: eeb10a40     	vneg.f32	s0, s0
   3a544: eb00456a     	bl	0x4baf4
   3a548: e5943008     	ldr	r3, [r4, #0x8]
   3a54c: e3a02000     	mov	r2, #0
   3a550: e5c3207c     	strb	r2, [r3, #0x7c]
   3a554: e8bd8010     	pop	{r4, pc}
   3a558: e3510000     	cmp	r1, #0
   3a55c: b8bd8010     	poplt	{r4, pc}
   3a560: e5900008     	ldr	r0, [r0, #0x8]
   3a564: e59030e8     	ldr	r3, [r0, #0xe8]
   3a568: e5933084     	ldr	r3, [r3, #0x84]
   3a56c: e3530001     	cmp	r3, #1
   3a570: 03a03000     	moveq	r3, #0
   3a574: 05d0227c     	ldrbeq	r2, [r0, #0x27c]
   3a578: 05c0227d     	strbeq	r2, [r0, #0x27d]
   3a57c: 05c0327c     	strbeq	r3, [r0, #0x27c]
   3a580: eaffffe0     	b	0x3a508
   3a584: e2800d65     	add	r0, r0, #6464
   3a588: e3a010fe     	mov	r1, #254
   3a58c: e2800028     	add	r0, r0, #40
   3a590: eb00453a     	bl	0x4ba80
   3a594: e5940008     	ldr	r0, [r4, #0x8]
   3a598: eaffffe1     	b	0x3a524
   3a59c: 00 00 80 43  	.word	0x43800000
