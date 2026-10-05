000031ec <mapmem.constprop.5>:
    31ec: e92d4030     	push	{r4, r5, lr}
    31f0: e24dd00c     	sub	sp, sp, #12
    31f4: e3a03001     	mov	r3, #1
    31f8: e88d0006     	stm	sp, {r1, r2}
    31fc: e1a01000     	mov	r1, r0
    3200: e3a02003     	mov	r2, #3
    3204: e3a00000     	mov	r0, #0
    3208: ebfffbdc     	bl	0x2180 <.plt+0x11c>     @ imm = #-0x1090
    320c: e3700001     	cmn	r0, #1
    3210: e1a04000     	mov	r4, r0
    3214: 0a000002     	beq	0x3224 <mapmem.constprop.5+0x38> @ imm = #0x8
    3218: e1a00004     	mov	r0, r4
    321c: e28dd00c     	add	sp, sp, #12
    3220: e8bd8030     	pop	{r4, r5, pc}
    3224: e59f302c     	ldr	r3, [pc, #0x2c]         @ 0x3258 <mapmem.constprop.5+0x6c>
    3228: e5935000     	ldr	r5, [r3]
    322c: ebfffbe2     	bl	0x21bc <.plt+0x158>     @ imm = #-0x1078
    3230: e5900000     	ldr	r0, [r0]
    3234: ebfffbc5     	bl	0x2150 <.plt+0xec>      @ imm = #-0x10ec
    3238: e59f201c     	ldr	r2, [pc, #0x1c]         @ 0x325c <mapmem.constprop.5+0x70>
    323c: e59f101c     	ldr	r1, [pc, #0x1c]         @ 0x3260 <mapmem.constprop.5+0x74>
    3240: e1a03000     	mov	r3, r0
    3244: e1a00005     	mov	r0, r5
    3248: ebfffbd2     	bl	0x2198 <.plt+0x134>     @ imm = #-0x10b8
    324c: e1a00004     	mov	r0, r4
    3250: e28dd00c     	add	sp, sp, #12
    3254: e8bd8030     	pop	{r4, r5, pc}
    3258: 00 00 00 00  	.word	0x00000000
    325c: 50 77 00 00  	.word	0x00007750
    3260: 58 77 00 00  	.word	0x00007758

