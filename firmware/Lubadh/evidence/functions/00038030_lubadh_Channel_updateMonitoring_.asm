; lubadh::Channel::updateMonitoring()
; VA 0x38030 size 112

   38030: e59030e8     	ldr	r3, [r0, #0xe8]
   38034: e5933084     	ldr	r3, [r3, #0x84]
   38038: e3530001     	cmp	r3, #1
   3803c: 0a000008     	beq	0x38064
   38040: e3530002     	cmp	r3, #2
   38044: 0a00000c     	beq	0x3807c
   38048: e3530000     	cmp	r3, #0
   3804c: 112fff1e     	bxne	lr
   38050: e5d0227c     	ldrb	r2, [r0, #0x27c]
   38054: e3a03000     	mov	r3, #0
   38058: e5c0227d     	strb	r2, [r0, #0x27d]
   3805c: e5c0327c     	strb	r3, [r0, #0x27c]
   38060: e12fff1e     	bx	lr
   38064: e5903278     	ldr	r3, [r0, #0x278]
   38068: e5933004     	ldr	r3, [r3, #0x4]
   3806c: e3530002     	cmp	r3, #2
   38070: 8a000007     	bhi	0x38094
   38074: e3530000     	cmp	r3, #0
   38078: 0afffff4     	beq	0x38050
   3807c: e5d0327c     	ldrb	r3, [r0, #0x27c]
   38080: e3a02001     	mov	r2, #1
   38084: e5c0227c     	strb	r2, [r0, #0x27c]
   38088: e0233002     	eor	r3, r3, r2
   3808c: e5c0327d     	strb	r3, [r0, #0x27d]
   38090: e12fff1e     	bx	lr
   38094: e3530003     	cmp	r3, #3
   38098: 0affffec     	beq	0x38050
   3809c: e12fff1e     	bx	lr
