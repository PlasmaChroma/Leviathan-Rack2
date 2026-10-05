; lubadh::TapeFlutter::~TapeFlutter()
; VA 0x3fd80 size 144

   3fd80: e2803a01     	add	r3, r0, #4096
   3fd84: e92d4010     	push	{r4, lr}
   3fd88: e1a04000     	mov	r4, r0
   3fd8c: e593080c     	ldr	r0, [r3, #0x80c]
   3fd90: e3500000     	cmp	r0, #0
   3fd94: 0a000000     	beq	0x3fd9c
   3fd98: ebff5828     	bl	0x15e40    @ imm = #-0x29f60 ; _ZdlPv
   3fd9c: e2840e47     	add	r0, r4, #1136
   3fda0: e280000c     	add	r0, r0, #12
   3fda4: ebff581c     	bl	0x15e1c    @ imm = #-0x29f90 ; _ZNSt13random_device7_M_finiEv
   3fda8: e594006c     	ldr	r0, [r4, #0x6c]
   3fdac: e3500000     	cmp	r0, #0
   3fdb0: 0a000000     	beq	0x3fdb8
   3fdb4: ebff5821     	bl	0x15e40    @ imm = #-0x29f7c ; _ZdlPv
   3fdb8: e5940060     	ldr	r0, [r4, #0x60]
   3fdbc: e3500000     	cmp	r0, #0
   3fdc0: 0a000000     	beq	0x3fdc8
   3fdc4: ebff581d     	bl	0x15e40    @ imm = #-0x29f8c ; _ZdlPv
   3fdc8: e5940054     	ldr	r0, [r4, #0x54]
   3fdcc: e3500000     	cmp	r0, #0
   3fdd0: 0a000000     	beq	0x3fdd8
   3fdd4: ebff5819     	bl	0x15e40    @ imm = #-0x29f9c ; _ZdlPv
   3fdd8: e5940040     	ldr	r0, [r4, #0x40]
   3fddc: e3500000     	cmp	r0, #0
   3fde0: 0a000000     	beq	0x3fde8
   3fde4: ebff5815     	bl	0x15e40    @ imm = #-0x29fac ; _ZdlPv
   3fde8: e5940034     	ldr	r0, [r4, #0x34]
   3fdec: e3500000     	cmp	r0, #0
   3fdf0: 0a000000     	beq	0x3fdf8
   3fdf4: ebff5811     	bl	0x15e40    @ imm = #-0x29fbc ; _ZdlPv
   3fdf8: e5940028     	ldr	r0, [r4, #0x28]
   3fdfc: e3500000     	cmp	r0, #0
   3fe00: 0a000000     	beq	0x3fe08
   3fe04: ebff580d     	bl	0x15e40    @ imm = #-0x29fcc ; _ZdlPv
   3fe08: e1a00004     	mov	r0, r4
   3fe0c: e8bd8010     	pop	{r4, pc}
