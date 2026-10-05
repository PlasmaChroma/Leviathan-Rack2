; lubadh::TapeAllpass::TapeAllpass()
; VA 0x4fca4 size 68

   4fca4: e92d4010     	push	{r4, lr}
   4fca8: e3a01000     	mov	r1, #0
   4fcac: e1a04000     	mov	r4, r0
   4fcb0: e3012790     	movw	r2, #0x1790
   4fcb4: e2800004     	add	r0, r0, #4
   4fcb8: ebff182d     	bl	0x15d74    @ imm = #-0x39f4c ; memset
   4fcbc: e2843d5e     	add	r3, r4, #6016
   4fcc0: eddf0b04     	vldr	d16, [pc, #16]          @ 0x4fcd8 ; float 3.37397330797e-312
   4fcc4: eddf1b05     	vldr	d17, [pc, #20]          @ 0x4fce0 ; float 7.95748421736e-312
   4fcc8: e2833014     	add	r3, r3, #20
   4fccc: e1a00004     	mov	r0, r4
   4fcd0: f4430a8f     	vst1.32	{d16, d17}, [r3]
   4fcd4: e8bd8010     	pop	{r4, pc}
   4fcd8: 44 00 00 00  	.word	0x00000044
   4fcdc: 9f 00 00 00  	.word	0x0000009f
   4fce0: fb 00 00 00  	.word	0x000000fb
   4fce4: 77 01 00 00  	.word	0x00000177
