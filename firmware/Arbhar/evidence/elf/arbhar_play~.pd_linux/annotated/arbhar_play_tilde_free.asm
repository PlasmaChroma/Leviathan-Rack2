00003648 <arbhar_play_tilde_free>:
    3648: e92d4070     	push	{r4, r5, r6, lr}
    364c: e2805a02     	add	r5, r0, #8192
    3650: e1a04000     	mov	r4, r0
    3654: e24dd058     	sub	sp, sp, #88
    3658: e5950894     	ldr	r0, [r5, #0x894]
    365c: e3500000     	cmp	r0, #0
    3660: 0a000002     	beq	0x3670 <arbhar_play_tilde_free+0x28> @ imm = #0x8
    3664: ebfffbf5     	bl	0x2640 <.plt+0x260>     @ imm = #-0x102c  // CALL shmdt
    3668: e3700001     	cmn	r0, #1
    366c: 0a000036     	beq	0x374c <arbhar_play_tilde_free+0x104> @ imm = #0xd8
    3670: e5956860     	ldr	r6, [r5, #0x860]
    3674: e3a03000     	mov	r3, #0
    3678: e5853894     	str	r3, [r5, #0x894]
    367c: e1560003     	cmp	r6, r3
    3680: ca00001d     	bgt	0x36fc <arbhar_play_tilde_free+0xb4> @ imm = #0x74
    3684: e594001c     	ldr	r0, [r4, #0x1c]
    3688: ebfffc19     	bl	0x26f4 <.plt+0x314>     @ imm = #-0xf9c  // CALL clock_free
    368c: e5940020     	ldr	r0, [r4, #0x20]
    3690: ebfffc17     	bl	0x26f4 <.plt+0x314>     @ imm = #-0xfa4  // CALL clock_free
    3694: e5940024     	ldr	r0, [r4, #0x24]
    3698: ebfffc15     	bl	0x26f4 <.plt+0x314>     @ imm = #-0xfac  // CALL clock_free
    369c: e594002c     	ldr	r0, [r4, #0x2c]
    36a0: ebfffc13     	bl	0x26f4 <.plt+0x314>     @ imm = #-0xfb4  // CALL clock_free
    36a4: e5940028     	ldr	r0, [r4, #0x28]
    36a8: ebfffc11     	bl	0x26f4 <.plt+0x314>     @ imm = #-0xfbc  // CALL clock_free
    36ac: e59400e0     	ldr	r0, [r4, #0xe0]
    36b0: ebfffb6a     	bl	0x2460 <.plt+0x80>      @ imm = #-0x1258  // CALL outlet_free
    36b4: e59400e4     	ldr	r0, [r4, #0xe4]
    36b8: ebfffb68     	bl	0x2460 <.plt+0x80>      @ imm = #-0x1260  // CALL outlet_free
    36bc: e59400e8     	ldr	r0, [r4, #0xe8]
    36c0: ebfffb66     	bl	0x2460 <.plt+0x80>      @ imm = #-0x1268  // CALL outlet_free
    36c4: e59400ec     	ldr	r0, [r4, #0xec]
    36c8: ebfffb64     	bl	0x2460 <.plt+0x80>      @ imm = #-0x1270  // CALL outlet_free
    36cc: e59400f0     	ldr	r0, [r4, #0xf0]
    36d0: ebfffb62     	bl	0x2460 <.plt+0x80>      @ imm = #-0x1278  // CALL outlet_free
    36d4: e59400f4     	ldr	r0, [r4, #0xf4]
    36d8: ebfffb60     	bl	0x2460 <.plt+0x80>      @ imm = #-0x1280  // CALL outlet_free
    36dc: e59400f8     	ldr	r0, [r4, #0xf8]
    36e0: ebfffb5e     	bl	0x2460 <.plt+0x80>      @ imm = #-0x1288  // CALL outlet_free
    36e4: e59400fc     	ldr	r0, [r4, #0xfc]
    36e8: ebfffb5c     	bl	0x2460 <.plt+0x80>      @ imm = #-0x1290  // CALL outlet_free
    36ec: e5940100     	ldr	r0, [r4, #0x100]
    36f0: ebfffb5a     	bl	0x2460 <.plt+0x80>      @ imm = #-0x1298  // CALL outlet_free
    36f4: e28dd058     	add	sp, sp, #88
    36f8: e8bd8070     	pop	{r4, r5, r6, pc}
    36fc: e28d5004     	add	r5, sp, #4
    3700: e3a01002     	mov	r1, #2
    3704: e1a00006     	mov	r0, r6
    3708: e1a02005     	mov	r2, r5
    370c: ebfffb89     	bl	0x2538 <.plt+0x158>     @ imm = #-0x11dc  // CALL shmctl
    3710: e3700001     	cmn	r0, #1
    3714: 0affffda     	beq	0x3684 <arbhar_play_tilde_free+0x3c> @ imm = #-0x98
    3718: e59d104c     	ldr	r1, [sp, #0x4c]
    371c: e3510000     	cmp	r1, #0
    3720: 1affffd7     	bne	0x3684 <arbhar_play_tilde_free+0x3c> @ imm = #-0xa4
    3724: e1a02005     	mov	r2, r5
    3728: e1a00006     	mov	r0, r6
    372c: ebfffb81     	bl	0x2538 <.plt+0x158>     @ imm = #-0x11fc  // CALL shmctl
    3730: e3700001     	cmn	r0, #1
    3734: 1affffd2     	bne	0x3684 <arbhar_play_tilde_free+0x3c> @ imm = #-0xb8
    3738: e59f2020     	ldr	r2, [pc, #0x20]         @ 0x3760 <arbhar_play_tilde_free+0x118>  // u32=0x944c; f32?=5.31988949e-41
    373c: e1a01006     	mov	r1, r6
    3740: e08f0002     	add	r0, pc, r2
    3744: ebfffb7e     	bl	0x2544 <.plt+0x164>     @ imm = #-0x1208  // CALL error
    3748: eaffffcd     	b	0x3684 <arbhar_play_tilde_free+0x3c> @ imm = #-0xcc
    374c: e59f0010     	ldr	r0, [pc, #0x10]         @ 0x3764 <arbhar_play_tilde_free+0x11c>  // u32=0x9424; f32?=5.3142843e-41
    3750: e5951894     	ldr	r1, [r5, #0x894]
    3754: e08f0000     	add	r0, pc, r0
    3758: ebfffb79     	bl	0x2544 <.plt+0x164>     @ imm = #-0x121c  // CALL error
    375c: eaffffc3     	b	0x3670 <arbhar_play_tilde_free+0x28> @ imm = #-0xf4
    3760: 4c 94 00 00  	.word	0x0000944c
    3764: 24 94 00 00  	.word	0x00009424

