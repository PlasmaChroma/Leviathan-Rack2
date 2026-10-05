; lubadh::Application::~Application()
; VA 0x2a0a4 size 120

   2a0a4: e92d4070     	push	{r4, r5, r6, lr}
   2a0a8: e2805915     	add	r5, r0, #344064
   2a0ac: e3a03000     	mov	r3, #0
   2a0b0: e1a04000     	mov	r4, r0
   2a0b4: e5956ab8     	ldr	r6, [r5, #0xab8]
   2a0b8: e5853ab8     	str	r3, [r5, #0xab8]
   2a0bc: e1560003     	cmp	r6, r3
   2a0c0: 0a00000c     	beq	0x2a0f8
   2a0c4: e1a00006     	mov	r0, r6
   2a0c8: eb00305c     	bl	0x36240
   2a0cc: e1a00006     	mov	r0, r6
   2a0d0: e3a010a0     	mov	r1, #160
   2a0d4: ebffaefc     	bl	0x15ccc    @ imm = #-0x14410 ; _ZdlPvj
   2a0d8: e5955ab8     	ldr	r5, [r5, #0xab8]
   2a0dc: e3550000     	cmp	r5, #0
   2a0e0: 0a000004     	beq	0x2a0f8
   2a0e4: e1a00005     	mov	r0, r5
   2a0e8: eb003054     	bl	0x36240
   2a0ec: e1a00005     	mov	r0, r5
   2a0f0: e3a010a0     	mov	r1, #160
   2a0f4: ebffaef4     	bl	0x15ccc    @ imm = #-0x14430 ; _ZdlPvj
   2a0f8: e2840ba9     	add	r0, r4, #173056
   2a0fc: e2800d06     	add	r0, r0, #384
   2a100: eb001cb6     	bl	0x313e0
   2a104: e2840048     	add	r0, r4, #72
   2a108: eb001cb4     	bl	0x313e0
   2a10c: e1a00004     	mov	r0, r4
   2a110: eb0107c4     	bl	0x6c028
   2a114: e1a00004     	mov	r0, r4
   2a118: e8bd8070     	pop	{r4, r5, r6, pc}
