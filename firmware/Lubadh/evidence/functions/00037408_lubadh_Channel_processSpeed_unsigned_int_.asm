; lubadh::Channel::processSpeed(unsigned int)
; VA 0x37408 size 132

   37408: e59032e0     	ldr	r3, [r0, #0x2e0]
   3740c: eeb70a00     	vmov.f32	s0, #1.000000e+00
   37410: edd07ab6     	vldr	s15, [r0, #728]
   37414: e3530000     	cmp	r3, #0
   37418: e59020e8     	ldr	r2, [r0, #0xe8]
   3741c: e92d4070     	push	{r4, r5, r6, lr}
   37420: e1a04000     	mov	r4, r0
   37424: c2433001     	subgt	r3, r3, #1
   37428: cd907ab7     	vldrgt	s14, [r0, #732]
   3742c: e1a05001     	mov	r5, r1
   37430: e5922000     	ldr	r2, [r2]
   37434: e5802028     	str	r2, [r0, #0x28]
   37438: ce777a87     	vaddgt.f32	s15, s15, s14
   3743c: c58032e0     	strgt	r3, [r0, #0x2e0]
   37440: cdc07ab6     	vstrgt	s15, [r0, #728]
   37444: e2800c4a     	add	r0, r0, #18944
   37448: e2800010     	add	r0, r0, #16
   3744c: edc47a09     	vstr	s15, [r4, #36]
   37450: eb00639b     	bl	0x502c4
   37454: e59421c4     	ldr	r2, [r4, #0x1c4]
   37458: e2840c62     	add	r0, r4, #25088
   3745c: ed840a0c     	vstr	s0, [r4, #48]
   37460: e2422001     	sub	r2, r2, #1
   37464: e5d431cd     	ldrb	r3, [r4, #0x1cd]
   37468: e3520001     	cmp	r2, #1
   3746c: e1a01005     	mov	r1, r5
   37470: e2800028     	add	r0, r0, #40
   37474: 83a02000     	movhi	r2, #0
   37478: 93a02001     	movls	r2, #1
   3747c: eeb70a00     	vmov.f32	s0, #1.000000e+00
   37480: ebfffe26     	bl	0x36d20
   37484: ed840a0d     	vstr	s0, [r4, #52]
   37488: e8bd8070     	pop	{r4, r5, r6, pc}
