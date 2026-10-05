0000494c <arbhar_gpio_tilde_dsp>:
    494c: e92d4030     	push	{r4, r5, lr}
    4950: e24dd00c     	sub	sp, sp, #12
    4954: e1a05001     	mov	r5, r1
    4958: e1a04000     	mov	r4, r0
    495c: ebfffd00     	bl	0x3d64 <.plt+0x668>     @ imm = #-0xc00  // CALL bcm2835_spi_begin
    4960: e3a00080     	mov	r0, #128
    4964: ebfffbc9     	bl	0x3890 <.plt+0x194>     @ imm = #-0x10dc  // CALL bcm2835_spi_setClockDivider
    4968: e595e000     	ldr	lr, [r5]
    496c: e59f0028     	ldr	r0, [pc, #0x28]         @ 0x499c <arbhar_gpio_tilde_dsp+0x50>  // u32=0x22678; f32?=1.9747098e-40
    4970: e1a02004     	mov	r2, r4
    4974: e59fc024     	ldr	r12, [pc, #0x24]        @ 0x49a0 <arbhar_gpio_tilde_dsp+0x54>  // u32=0x284; f32?=9.02436211e-43
    4978: e3a01003     	mov	r1, #3
    497c: e59e4000     	ldr	r4, [lr]
    4980: e08f0000     	add	r0, pc, r0
    4984: e59e3004     	ldr	r3, [lr, #0x4]
    4988: e790000c     	ldr	r0, [r0, r12]
    498c: e58d4000     	str	r4, [sp]
    4990: ebfffc99     	bl	0x3bfc <.plt+0x500>     @ imm = #-0xd9c  // CALL dsp_add
    4994: e28dd00c     	add	sp, sp, #12
    4998: e8bd8030     	pop	{r4, r5, pc}
    499c: 78 26 02 00  	.word	0x00022678
    49a0: 84 02 00 00  	.word	0x00000284

