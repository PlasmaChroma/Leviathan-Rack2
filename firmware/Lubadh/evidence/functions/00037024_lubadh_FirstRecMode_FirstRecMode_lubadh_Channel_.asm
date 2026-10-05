; lubadh::FirstRecMode::FirstRecMode(lubadh::Channel&)
; VA 0x37024 size 40

   37024: e52de004     	str	lr, [sp, #-0x4]!
   37028: e3a0c001     	mov	r12, #1
   3702c: e59fe014     	ldr	lr, [pc, #0x14]         @ 0x37048
   37030: e3a02000     	mov	r2, #0
   37034: e5801008     	str	r1, [r0, #0x8]
   37038: e580e000     	str	lr, [r0]
   3703c: e580c004     	str	r12, [r0, #0x4]
   37040: e5c0200c     	strb	r2, [r0, #0xc]
   37044: e49df004     	ldr	pc, [sp], #4
   37048: 30 2f 07 00  	.word	0x00072f30
