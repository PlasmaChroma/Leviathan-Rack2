; lubadh::Channel::InputBuffer::input(std::vector<float, std::allocator<float> >&)
; VA 0x38a78 size 60

   38a78: e92d4070     	push	{r4, r5, r6, lr}
   38a7c: e1a05001     	mov	r5, r1
   38a80: e1a04000     	mov	r4, r0
   38a84: e1c000d4     	ldrd	r0, r1, [r0, #4]
   38a88: e3a02010     	mov	r2, #16
   38a8c: e2411010     	sub	r1, r1, #16
   38a90: ebff73d3     	bl	0x159e4    @ imm = #-0x230b4 ; memmove
   38a94: e8950006     	ldm	r5, {r1, r2}
   38a98: e1520001     	cmp	r2, r1
   38a9c: 08bd8070     	popeq	{r4, r5, r6, pc}
   38aa0: e5940004     	ldr	r0, [r4, #0x4]
   38aa4: e0422001     	sub	r2, r2, r1
   38aa8: e8bd4070     	pop	{r4, r5, r6, lr}
   38aac: e2800010     	add	r0, r0, #16
   38ab0: eaff73cb     	b	0x159e4    @ imm = #-0x230d4
