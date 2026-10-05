; lubadh::Channel::FileLoader::~FileLoader()
; VA 0x4114c size 224

   4114c: e59f30c4     	ldr	r3, [pc, #0xc4]         @ 0x41218
   41150: e92d4070     	push	{r4, r5, r6, lr}
   41154: e1a04000     	mov	r4, r0
   41158: e2805098     	add	r5, r0, #152
   4115c: e5a030d8     	str	r3, [r0, #0xd8]!
   41160: ebffb815     	bl	0x2f1bc
   41164: e28400bc     	add	r0, r4, #188
   41168: eb00bcc8     	bl	0x70490
   4116c: e59f30a8     	ldr	r3, [pc, #0xa8]         @ 0x4121c
   41170: e5843098     	str	r3, [r4, #0x98]
   41174: e1a00005     	mov	r0, r5
   41178: ebffb8f2     	bl	0x2f548
   4117c: e1a00005     	mov	r0, r5
   41180: ebffb949     	bl	0x2f6ac
   41184: e59400a0     	ldr	r0, [r4, #0xa0]
   41188: e28430a8     	add	r3, r4, #168
   4118c: e1500003     	cmp	r0, r3
   41190: 0a000000     	beq	0x41198
   41194: ebff5329     	bl	0x15e40    @ imm = #-0x2b35c ; _ZdlPv
   41198: e59f3080     	ldr	r3, [pc, #0x80]         @ 0x41220
   4119c: e2845074     	add	r5, r4, #116
   411a0: e5843074     	str	r3, [r4, #0x74]
   411a4: e1a00005     	mov	r0, r5
   411a8: ebffb9bf     	bl	0x2f8ac
   411ac: e1a00005     	mov	r0, r5
   411b0: ebffba16     	bl	0x2fa10
   411b4: e594007c     	ldr	r0, [r4, #0x7c]
   411b8: e2843084     	add	r3, r4, #132
   411bc: e1500003     	cmp	r0, r3
   411c0: 0a000000     	beq	0x411c8
   411c4: ebff531d     	bl	0x15e40    @ imm = #-0x2b38c ; _ZdlPv
   411c8: e59f3054     	ldr	r3, [pc, #0x54]         @ 0x41224
   411cc: e2845048     	add	r5, r4, #72
   411d0: e5843048     	str	r3, [r4, #0x48]
   411d4: e1a00005     	mov	r0, r5
   411d8: ebffbb66     	bl	0x2ff78
   411dc: e1a00005     	mov	r0, r5
   411e0: ebffbbbd     	bl	0x300dc
   411e4: e5940050     	ldr	r0, [r4, #0x50]
   411e8: e2843058     	add	r3, r4, #88
   411ec: e1500003     	cmp	r0, r3
   411f0: 0a000000     	beq	0x411f8
   411f4: ebff5311     	bl	0x15e40    @ imm = #-0x2b3bc ; _ZdlPv
   411f8: e284002c     	add	r0, r4, #44
   411fc: eb00bca3     	bl	0x70490
   41200: e1a00004     	mov	r0, r4
   41204: e59f301c     	ldr	r3, [pc, #0x1c]         @ 0x41228
   41208: e5a03004     	str	r3, [r0, #0x4]!
   4120c: ebffb7ea     	bl	0x2f1bc
   41210: e1a00004     	mov	r0, r4
   41214: e8bd8070     	pop	{r4, r5, r6, pc}
   41218: e0 1e 07 00  	.word	0x00071ee0
   4121c: 50 1f 07 00  	.word	0x00071f50
   41220: 90 1f 07 00  	.word	0x00071f90
   41224: 80 1f 07 00  	.word	0x00071f80
   41228: 70 1f 07 00  	.word	0x00071f70
