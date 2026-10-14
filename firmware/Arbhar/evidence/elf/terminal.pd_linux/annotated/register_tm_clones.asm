00000720 <register_tm_clones>:
     720: e59f0038     	ldr	r0, [pc, #0x38]         @ 0x760 <register_tm_clones+0x40>  // u32=0x1094c; f32?=9.51705865e-41
     724: e59f3038     	ldr	r3, [pc, #0x38]         @ 0x764 <register_tm_clones+0x44>  // u32=0x10948; f32?=9.51649813e-41
     728: e08f0000     	add	r0, pc, r0
     72c: e08f3003     	add	r3, pc, r3
     730: e0431000     	sub	r1, r3, r0
     734: e59f302c     	ldr	r3, [pc, #0x2c]         @ 0x768 <register_tm_clones+0x48>  // u32=0x108bc; f32?=9.49687995e-41
     738: e1a01141     	asr	r1, r1, #2
     73c: e08f3003     	add	r3, pc, r3
     740: e0811fa1     	add	r1, r1, r1, lsr #31
     744: e1b010c1     	asrs	r1, r1, #1
     748: 012fff1e     	bxeq	lr
     74c: e59f2018     	ldr	r2, [pc, #0x18]         @ 0x76c <register_tm_clones+0x4c>  // u32=0x70; f32?=1.56945428e-43
     750: e7933002     	ldr	r3, [r3, r2]
     754: e3530000     	cmp	r3, #0
     758: 012fff1e     	bxeq	lr
     75c: e12fff13     	bx	r3
     760: 4c 09 01 00  	.word	0x0001094c
     764: 48 09 01 00  	.word	0x00010948
     768: bc 08 01 00  	.word	0x000108bc
     76c: 70 00 00 00  	.word	0x00000070

