; lubadh::Application::Application()
; VA 0x29fe0 size 196

   29fe0: e92d41f0     	push	{r4, r5, r6, r7, r8, lr}
   29fe4: e1a04000     	mov	r4, r0
   29fe8: eb010a64     	bl	0x6c980
   29fec: e1a00004     	mov	r0, r4
   29ff0: e3a02001     	mov	r2, #1
   29ff4: e3a01096     	mov	r1, #150
   29ff8: eb010a65     	bl	0x6c994
   29ffc: e3a02001     	mov	r2, #1
   2a000: e3a01097     	mov	r1, #151
   2a004: e1a00004     	mov	r0, r4
   2a008: eb010a61     	bl	0x6c994
   2a00c: e2846048     	add	r6, r4, #72
   2a010: e1a02004     	mov	r2, r4
   2a014: e1a00006     	mov	r0, r6
   2a018: e3a01000     	mov	r1, #0
   2a01c: eb004b48     	bl	0x3cd44
   2a020: e2845ba9     	add	r5, r4, #173056
   2a024: e1a02004     	mov	r2, r4
   2a028: e2855d06     	add	r5, r5, #384
   2a02c: e3a01001     	mov	r1, #1
   2a030: e1a00005     	mov	r0, r5
   2a034: eb004b42     	bl	0x3cd44
   2a038: e2847915     	add	r7, r4, #344064
   2a03c: e3a03000     	mov	r3, #0
   2a040: e1a01005     	mov	r1, r5
   2a044: e1a00006     	mov	r0, r6
   2a048: e5873ab8     	str	r3, [r7, #0xab8]
   2a04c: eb003431     	bl	0x37118
   2a050: e1a01006     	mov	r1, r6
   2a054: e1a00005     	mov	r0, r5
   2a058: eb00342e     	bl	0x37118
   2a05c: e1a00004     	mov	r0, r4
   2a060: e8bd81f0     	pop	{r4, r5, r6, r7, r8, pc}
   2a064: e1a00004     	mov	r0, r4
   2a068: eb0107ee     	bl	0x6c028
   2a06c: ebffafbb     	bl	0x15f60    @ imm = #-0x14114 ; __cxa_end_cleanup
   2a070: e5977ab8     	ldr	r7, [r7, #0xab8]
   2a074: e3570000     	cmp	r7, #0
   2a078: 0a000004     	beq	0x2a090
   2a07c: e1a00007     	mov	r0, r7
   2a080: eb00306e     	bl	0x36240
   2a084: e1a00007     	mov	r0, r7
   2a088: e3a010a0     	mov	r1, #160
   2a08c: ebffaf0e     	bl	0x15ccc    @ imm = #-0x143c8 ; _ZdlPvj
   2a090: e1a00005     	mov	r0, r5
   2a094: eb001cd1     	bl	0x313e0
   2a098: e1a00006     	mov	r0, r6
   2a09c: eb001ccf     	bl	0x313e0
   2a0a0: eaffffef     	b	0x2a064
