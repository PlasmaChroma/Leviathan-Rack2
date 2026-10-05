; lubadh::Application::interpretADCs()
; VA 0x2830c size 152

   2830c: e92d41f0     	push	{r4, r5, r6, r7, r8, lr}
   28310: e2807a2a     	add	r7, r0, #172032
   28314: e2808915     	add	r8, r0, #344064
   28318: e2806048     	add	r6, r0, #72
   2831c: e1a04000     	mov	r4, r0
   28320: e2805ba9     	add	r5, r0, #173056
   28324: e59714e0     	ldr	r1, [r7, #0x4e0]
   28328: e2855d06     	add	r5, r5, #384
   2832c: e1a00006     	mov	r0, r6
   28330: eb003c76     	bl	0x37510
   28334: e5981a18     	ldr	r1, [r8, #0xa18]
   28338: e1a00005     	mov	r0, r5
   2833c: eb003c73     	bl	0x37510
   28340: e594114c     	ldr	r1, [r4, #0x14c]
   28344: e1a00006     	mov	r0, r6
   28348: e3a02000     	mov	r2, #0
   2834c: e2411001     	sub	r1, r1, #1
   28350: e3510001     	cmp	r1, #1
   28354: 83a01000     	movhi	r1, #0
   28358: 93a01001     	movls	r1, #1
   2835c: eb003cd2     	bl	0x376ac
   28360: e5971684     	ldr	r1, [r7, #0x684]
   28364: e1a00005     	mov	r0, r5
   28368: e3a02000     	mov	r2, #0
   2836c: e2411001     	sub	r1, r1, #1
   28370: e3510001     	cmp	r1, #1
   28374: 83a01000     	movhi	r1, #0
   28378: 93a01001     	movls	r1, #1
   2837c: eb003cca     	bl	0x376ac
   28380: e59714f0     	ldr	r1, [r7, #0x4f0]
   28384: e1a00006     	mov	r0, r6
   28388: e3a02000     	mov	r2, #0
   2838c: eb003dac     	bl	0x37a44
   28390: e5981a28     	ldr	r1, [r8, #0xa28]
   28394: e1a00005     	mov	r0, r5
   28398: e3a02000     	mov	r2, #0
   2839c: e8bd41f0     	pop	{r4, r5, r6, r7, r8, lr}
   283a0: ea003da7     	b	0x37a44
