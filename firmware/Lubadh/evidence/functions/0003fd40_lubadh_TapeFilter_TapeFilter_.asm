; lubadh::TapeFilter::~TapeFilter()
; VA 0x3fd40 size 64

   3fd40: e92d4010     	push	{r4, lr}
   3fd44: e1a04000     	mov	r4, r0
   3fd48: e5900058     	ldr	r0, [r0, #0x58]
   3fd4c: e3500000     	cmp	r0, #0
   3fd50: 0a000000     	beq	0x3fd58
   3fd54: ebff5839     	bl	0x15e40    @ imm = #-0x29f1c ; _ZdlPv
   3fd58: e594004c     	ldr	r0, [r4, #0x4c]
   3fd5c: e3500000     	cmp	r0, #0
   3fd60: 0a000000     	beq	0x3fd68
   3fd64: ebff5835     	bl	0x15e40    @ imm = #-0x29f2c ; _ZdlPv
   3fd68: e5940040     	ldr	r0, [r4, #0x40]
   3fd6c: e3500000     	cmp	r0, #0
   3fd70: 0a000000     	beq	0x3fd78
   3fd74: ebff5831     	bl	0x15e40    @ imm = #-0x29f3c ; _ZdlPv
   3fd78: e1a00004     	mov	r0, r4
   3fd7c: e8bd8010     	pop	{r4, pc}
