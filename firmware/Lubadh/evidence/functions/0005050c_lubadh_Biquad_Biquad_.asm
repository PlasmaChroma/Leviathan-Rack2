; lubadh::Biquad::~Biquad()
; VA 0x5050c size 64

   5050c: e92d4010     	push	{r4, lr}
   50510: e1a04000     	mov	r4, r0
   50514: e590001c     	ldr	r0, [r0, #0x1c]
   50518: e3500000     	cmp	r0, #0
   5051c: 0a000000     	beq	0x50524
   50520: ebff1646     	bl	0x15e40    @ imm = #-0x3a6e8 ; _ZdlPv
   50524: e5940010     	ldr	r0, [r4, #0x10]
   50528: e3500000     	cmp	r0, #0
   5052c: 0a000000     	beq	0x50534
   50530: ebff1642     	bl	0x15e40    @ imm = #-0x3a6f8 ; _ZdlPv
   50534: e5940004     	ldr	r0, [r4, #0x4]
   50538: e3500000     	cmp	r0, #0
   5053c: 0a000000     	beq	0x50544
   50540: ebff163e     	bl	0x15e40    @ imm = #-0x3a708 ; _ZdlPv
   50544: e1a00004     	mov	r0, r4
   50548: e8bd8010     	pop	{r4, pc}
