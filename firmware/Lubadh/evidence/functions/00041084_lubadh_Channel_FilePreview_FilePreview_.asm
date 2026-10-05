; lubadh::Channel::FilePreview::~FilePreview()
; VA 0x41084 size 200

   41084: e92d4070     	push	{r4, r5, r6, lr}
   41088: e1a04000     	mov	r4, r0
   4108c: e59f30a8     	ldr	r3, [pc, #0xa8]         @ 0x4113c
   41090: e2805098     	add	r5, r0, #152
   41094: e5843098     	str	r3, [r4, #0x98]
   41098: e1a00005     	mov	r0, r5
   4109c: ebffb929     	bl	0x2f548
   410a0: e1a00005     	mov	r0, r5
   410a4: ebffb980     	bl	0x2f6ac
   410a8: e59400a0     	ldr	r0, [r4, #0xa0]
   410ac: e28430a8     	add	r3, r4, #168
   410b0: e1500003     	cmp	r0, r3
   410b4: 0a000000     	beq	0x410bc
   410b8: ebff5360     	bl	0x15e40    @ imm = #-0x2b280 ; _ZdlPv
   410bc: e1a00004     	mov	r0, r4
   410c0: e59f3078     	ldr	r3, [pc, #0x78]         @ 0x41140
   410c4: e2845050     	add	r5, r4, #80
   410c8: e5a03074     	str	r3, [r0, #0x74]!
   410cc: ebffb83a     	bl	0x2f1bc
   410d0: e59f306c     	ldr	r3, [pc, #0x6c]         @ 0x41144
   410d4: e5843050     	str	r3, [r4, #0x50]
   410d8: e1a00005     	mov	r0, r5
   410dc: ebffb9f2     	bl	0x2f8ac
   410e0: e1a00005     	mov	r0, r5
   410e4: ebffba49     	bl	0x2fa10
   410e8: e5940058     	ldr	r0, [r4, #0x58]
   410ec: e2843060     	add	r3, r4, #96
   410f0: e1500003     	cmp	r0, r3
   410f4: 0a000000     	beq	0x410fc
   410f8: ebff5350     	bl	0x15e40    @ imm = #-0x2b2c0 ; _ZdlPv
   410fc: e2840034     	add	r0, r4, #52
   41100: e2845004     	add	r5, r4, #4
   41104: eb00bce1     	bl	0x70490
   41108: e59f3038     	ldr	r3, [pc, #0x38]         @ 0x41148
   4110c: e5843004     	str	r3, [r4, #0x4]
   41110: e1a00005     	mov	r0, r5
   41114: ebffbabd     	bl	0x2fc10
   41118: e1a00005     	mov	r0, r5
   4111c: ebffbb15     	bl	0x2fd78
   41120: e594000c     	ldr	r0, [r4, #0xc]
   41124: e2843014     	add	r3, r4, #20
   41128: e1500003     	cmp	r0, r3
   4112c: 0a000000     	beq	0x41134
   41130: ebff5342     	bl	0x15e40    @ imm = #-0x2b2f8 ; _ZdlPv
   41134: e1a00004     	mov	r0, r4
   41138: e8bd8070     	pop	{r4, r5, r6, pc}
   4113c: 50 1f 07 00  	.word	0x00071f50
   41140: b0 1f 07 00  	.word	0x00071fb0
   41144: 90 1f 07 00  	.word	0x00071f90
   41148: a0 1f 07 00  	.word	0x00071fa0
