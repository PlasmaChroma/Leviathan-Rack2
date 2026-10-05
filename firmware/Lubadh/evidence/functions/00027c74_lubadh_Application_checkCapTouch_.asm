; lubadh::Application::checkCapTouch()
; VA 0x27c74 size 148

   27c74: e92d4010     	push	{r4, lr}
   27c78: e3a0101e     	mov	r1, #30
   27c7c: e1a04000     	mov	r4, r0
   27c80: e24dd018     	sub	sp, sp, #24
   27c84: eb01110e     	bl	0x6c0c4
   27c88: e3500000     	cmp	r0, #0
   27c8c: 1a000001     	bne	0x27c98
   27c90: e28dd018     	add	sp, sp, #24
   27c94: e8bd8010     	pop	{r4, pc}
   27c98: e1a0000d     	mov	r0, sp
   27c9c: e3001cd4     	movw	r1, #0xcd4
   27ca0: e3401007     	movt	r1, #0x7
   27ca4: ebffffbf     	bl	0x27ba8
   27ca8: e3090fec     	movw	r0, #0x9fec
   27cac: e3400009     	movt	r0, #0x9
   27cb0: e1a0100d     	mov	r1, sp
   27cb4: e3a02000     	mov	r2, #0
   27cb8: eb012098     	bl	0x6ff20
   27cbc: e59d0000     	ldr	r0, [sp]
   27cc0: e28d3008     	add	r3, sp, #8
   27cc4: e1500003     	cmp	r0, r3
   27cc8: 0a000000     	beq	0x27cd0
   27ccc: ebffb85b     	bl	0x15e40    @ imm = #-0x11e94 ; _ZdlPv
   27cd0: e2842a2a     	add	r2, r4, #172032
   27cd4: e594111c     	ldr	r1, [r4, #0x11c]
   27cd8: e3a03000     	mov	r3, #0
   27cdc: e5922654     	ldr	r2, [r2, #0x654]
   27ce0: e581303c     	str	r3, [r1, #0x3c]
   27ce4: e582303c     	str	r3, [r2, #0x3c]
   27ce8: e28dd018     	add	sp, sp, #24
   27cec: e8bd8010     	pop	{r4, pc}
   27cf0: e59d0000     	ldr	r0, [sp]
   27cf4: e28d3008     	add	r3, sp, #8
   27cf8: e1500003     	cmp	r0, r3
   27cfc: 0a000000     	beq	0x27d04
   27d00: ebffb84e     	bl	0x15e40    @ imm = #-0x11ec8 ; _ZdlPv
   27d04: ebffb895     	bl	0x15f60    @ imm = #-0x11dac ; __cxa_end_cleanup
