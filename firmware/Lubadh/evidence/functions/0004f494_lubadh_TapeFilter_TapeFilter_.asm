; lubadh::TapeFilter::TapeFilter()
; VA 0x4f494 size 144

   4f494: e92d4010     	push	{r4, lr}
   4f498: e1a04000     	mov	r4, r0
   4f49c: e2800004     	add	r0, r0, #4
   4f4a0: ebffeefd     	bl	0x4b09c
   4f4a4: e2840010     	add	r0, r4, #16
   4f4a8: ebffeefb     	bl	0x4b09c
   4f4ac: e284001c     	add	r0, r4, #28
   4f4b0: ebffeef9     	bl	0x4b09c
   4f4b4: e2840028     	add	r0, r4, #40
   4f4b8: ebffeef7     	bl	0x4b09c
   4f4bc: e2840034     	add	r0, r4, #52
   4f4c0: ebffeef5     	bl	0x4b09c
   4f4c4: e2842040     	add	r2, r4, #64
   4f4c8: f2c00050     	vmov.i32	q8, #0x0
   4f4cc: e2843050     	add	r3, r4, #80
   4f4d0: e3a0e000     	mov	lr, #0
   4f4d4: e3080388     	movw	r0, #0x8388
   4f4d8: e344031c     	movt	r0, #0x431c
   4f4dc: e30acf5f     	movw	r12, #0xaf5f
   4f4e0: e344c2d0     	movt	r12, #0x42d0
   4f4e4: e3061c0b     	movw	r1, #0x6c0b
   4f4e8: e34411fa     	movt	r1, #0x41fa
   4f4ec: f4420a8f     	vst1.32	{d16, d17}, [r2]
   4f4f0: e3062c0b     	movw	r2, #0x6c0b
   4f4f4: e344217a     	movt	r2, #0x417a
   4f4f8: f4430a8f     	vst1.32	{d16, d17}, [r3]
   4f4fc: e30f32b2     	movw	r3, #0xf2b2
   4f500: e34430a6     	movt	r3, #0x40a6
   4f504: e5840004     	str	r0, [r4, #0x4]
   4f508: e584e060     	str	lr, [r4, #0x60]
   4f50c: e1a00004     	mov	r0, r4
   4f510: e584c010     	str	r12, [r4, #0x10]
   4f514: e584101c     	str	r1, [r4, #0x1c]
   4f518: e5842028     	str	r2, [r4, #0x28]
   4f51c: e5843034     	str	r3, [r4, #0x34]
   4f520: e8bd8010     	pop	{r4, pc}
