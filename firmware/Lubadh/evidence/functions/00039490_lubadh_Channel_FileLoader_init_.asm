; lubadh::Channel::FileLoader::init()
; VA 0x39490 size 212

   39490: e92d4070     	push	{r4, r5, r6, lr}
   39494: e1a04000     	mov	r4, r0
   39498: e3022480     	movw	r2, #0x2480
   3949c: e3402007     	movt	r2, #0x7
   394a0: e24dd018     	sub	sp, sp, #24
   394a4: e5941000     	ldr	r1, [r4]
   394a8: e1a0000d     	mov	r0, sp
   394ac: e2811004     	add	r1, r1, #4
   394b0: ebffd5c4     	bl	0x2ebc8
   394b4: e3090fec     	movw	r0, #0x9fec
   394b8: e3400009     	movt	r0, #0x9
   394bc: e1a0100d     	mov	r1, sp
   394c0: e3a02000     	mov	r2, #0
   394c4: eb00da95     	bl	0x6ff20
   394c8: e59d0000     	ldr	r0, [sp]
   394cc: e28d3008     	add	r3, sp, #8
   394d0: e1500003     	cmp	r0, r3
   394d4: 0a000000     	beq	0x394dc
   394d8: ebff7258     	bl	0x15e40    @ imm = #-0x236a0 ; _ZdlPv
   394dc: e5940000     	ldr	r0, [r4]
   394e0: e3a02000     	mov	r2, #0
   394e4: e594309c     	ldr	r3, [r4, #0x9c]
   394e8: e3a0e001     	mov	lr, #1
   394ec: e5901020     	ldr	r1, [r0, #0x20]
   394f0: e590c000     	ldr	r12, [r0]
   394f4: e284002c     	add	r0, r4, #44
   394f8: e2816a2a     	add	r6, r1, #172032
   394fc: e5915048     	ldr	r5, [r1, #0x48]
   39500: e5961580     	ldr	r1, [r6, #0x580]
   39504: e7c32005     	strb	r2, [r3, r5]
   39508: e7c32001     	strb	r2, [r3, r1]
   3950c: e7c3e00c     	strb	lr, [r3, r12]
   39510: eb00dcc2     	bl	0x70820
   39514: e5943000     	ldr	r3, [r4]
   39518: e3a00003     	mov	r0, #3
   3951c: e3a01004     	mov	r1, #4
   39520: e2832a2a     	add	r2, r3, #172032
   39524: e593301c     	ldr	r3, [r3, #0x1c]
   39528: e592c4d0     	ldr	r12, [r2, #0x4d0]
   3952c: e2833a2a     	add	r3, r3, #172032
   39530: e584c0fc     	str	r12, [r4, #0xfc]
   39534: e593c4d0     	ldr	r12, [r3, #0x4d0]
   39538: e584c100     	str	r12, [r4, #0x100]
   3953c: e58204d0     	str	r0, [r2, #0x4d0]
   39540: e58314d0     	str	r1, [r3, #0x4d0]
   39544: e28dd018     	add	sp, sp, #24
   39548: e8bd8070     	pop	{r4, r5, r6, pc}
   3954c: e59d0000     	ldr	r0, [sp]
   39550: e28d3008     	add	r3, sp, #8
   39554: e1500003     	cmp	r0, r3
   39558: 0a000000     	beq	0x39560
   3955c: ebff7237     	bl	0x15e40    @ imm = #-0x23724 ; _ZdlPv
   39560: ebff727e     	bl	0x15f60    @ imm = #-0x23608 ; __cxa_end_cleanup
