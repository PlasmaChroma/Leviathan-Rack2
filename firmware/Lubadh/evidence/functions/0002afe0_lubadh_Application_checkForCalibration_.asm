; lubadh::Application::checkForCalibration()
; VA 0x2afe0 size 92

   2afe0: e92d4010     	push	{r4, lr}
   2afe4: e1a04000     	mov	r4, r0
   2afe8: ebfff4ed     	bl	0x283a4
   2afec: e594314c     	ldr	r3, [r4, #0x14c]
   2aff0: e2433001     	sub	r3, r3, #1
   2aff4: e3530001     	cmp	r3, #1
   2aff8: 88bd8010     	pophi	{r4, pc}
   2affc: e5943164     	ldr	r3, [r4, #0x164]
   2b000: e2433001     	sub	r3, r3, #1
   2b004: e3530001     	cmp	r3, #1
   2b008: 88bd8010     	pophi	{r4, pc}
   2b00c: e2842a2a     	add	r2, r4, #172032
   2b010: e5923684     	ldr	r3, [r2, #0x684]
   2b014: e2433001     	sub	r3, r3, #1
   2b018: e3530001     	cmp	r3, #1
   2b01c: 88bd8010     	pophi	{r4, pc}
   2b020: e592369c     	ldr	r3, [r2, #0x69c]
   2b024: e2433001     	sub	r3, r3, #1
   2b028: e3530001     	cmp	r3, #1
   2b02c: 88bd8010     	pophi	{r4, pc}
   2b030: e1a00004     	mov	r0, r4
   2b034: e8bd4010     	pop	{r4, lr}
   2b038: eafffd28     	b	0x2a4e0
