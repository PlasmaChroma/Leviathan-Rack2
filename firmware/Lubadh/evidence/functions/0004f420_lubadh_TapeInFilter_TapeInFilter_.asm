; lubadh::TapeInFilter::TapeInFilter()
; VA 0x4f420 size 84

   4f420: e92d4010     	push	{r4, lr}
   4f424: e1a04000     	mov	r4, r0
   4f428: ebffefcf     	bl	0x4b36c
   4f42c: e284002c     	add	r0, r4, #44
   4f430: ebffef19     	bl	0x4b09c
   4f434: e3a03003     	mov	r3, #3
   4f438: eddf0a0b     	vldr	s1, [pc, #44]           @ 0x4f46c ; float 0.20000000298
   4f43c: e1a00004     	mov	r0, r4
   4f440: ed9f0a0a     	vldr	s0, [pc, #40]           @ 0x4f470 ; float 0.000406749983085
   4f444: e5843028     	str	r3, [r4, #0x28]
   4f448: ebffed48     	bl	0x4a970
   4f44c: e1a00004     	mov	r0, r4
   4f450: e305366f     	movw	r3, #0x566f
   4f454: e3433f48     	movt	r3, #0x3f48
   4f458: e584302c     	str	r3, [r4, #0x2c]
   4f45c: e8bd8010     	pop	{r4, pc}
   4f460: e1a00004     	mov	r0, r4
   4f464: eb000428     	bl	0x5050c
   4f468: ebff1abc     	bl	0x15f60    @ imm = #-0x39510 ; __cxa_end_cleanup
   4f46c: cd cc 4c 3e  	.word	0x3e4ccccd
   4f470: 0f 41 d5 39  	.word	0x39d5410f
