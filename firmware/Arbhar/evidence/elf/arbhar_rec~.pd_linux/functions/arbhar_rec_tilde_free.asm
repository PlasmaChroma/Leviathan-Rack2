00002a4c <arbhar_rec_tilde_free>:
    2a4c: e92d4070     	push	{r4, r5, r6, lr}
    2a50: e1a06000     	mov	r6, r0
    2a54: e24dd058     	sub	sp, sp, #88
    2a58: e5900298     	ldr	r0, [r0, #0x298]
    2a5c: ebffff4e     	bl	0x279c <.plt+0x29c>     @ imm = #-0x2c8
    2a60: e596029c     	ldr	r0, [r6, #0x29c]
    2a64: ebfffecb     	bl	0x2598 <.plt+0x98>      @ imm = #-0x4d4
    2a68: e59602a0     	ldr	r0, [r6, #0x2a0]
    2a6c: ebfffec9     	bl	0x2598 <.plt+0x98>      @ imm = #-0x4dc
    2a70: e5963250     	ldr	r3, [r6, #0x250]
    2a74: e3530000     	cmp	r3, #0
    2a78: da00000d     	ble	0x2ab4 <arbhar_rec_tilde_free+0x68> @ imm = #0x34
    2a7c: e2864faa     	add	r4, r6, #680
    2a80: e3a05000     	mov	r5, #0
    2a84: e59400c0     	ldr	r0, [r4, #0xc0]
    2a88: e3500000     	cmp	r0, #0
    2a8c: 0a000000     	beq	0x2a94 <arbhar_rec_tilde_free+0x48> @ imm = #0x0
    2a90: ebffff3b     	bl	0x2784 <.plt+0x284>     @ imm = #-0x314
    2a94: e5941060     	ldr	r1, [r4, #0x60]
    2a98: e3510000     	cmp	r1, #0
    2a9c: 1a000013     	bne	0x2af0 <arbhar_rec_tilde_free+0xa4> @ imm = #0x4c
    2aa0: e5960250     	ldr	r0, [r6, #0x250]
    2aa4: e2855001     	add	r5, r5, #1
    2aa8: e2844004     	add	r4, r4, #4
    2aac: e1500005     	cmp	r0, r5
    2ab0: cafffff3     	bgt	0x2a84 <arbhar_rec_tilde_free+0x38> @ imm = #-0x34
    2ab4: e596007c     	ldr	r0, [r6, #0x7c]
    2ab8: ebffff61     	bl	0x2844 <.plt+0x344>     @ imm = #-0x27c
    2abc: e59601e8     	ldr	r0, [r6, #0x1e8]
    2ac0: e3500000     	cmp	r0, #0
    2ac4: 0a000002     	beq	0x2ad4 <arbhar_rec_tilde_free+0x88> @ imm = #0x8
    2ac8: ebffff2a     	bl	0x2778 <.plt+0x278>     @ imm = #-0x358
    2acc: e3700001     	cmn	r0, #1
    2ad0: 0a000021     	beq	0x2b5c <arbhar_rec_tilde_free+0x110> @ imm = #0x84
    2ad4: e59641b4     	ldr	r4, [r6, #0x1b4]
    2ad8: e3a0c000     	mov	r12, #0
    2adc: e586c1e8     	str	r12, [r6, #0x1e8]
    2ae0: e154000c     	cmp	r4, r12
    2ae4: ca000008     	bgt	0x2b0c <arbhar_rec_tilde_free+0xc0> @ imm = #0x20
    2ae8: e28dd058     	add	sp, sp, #88
    2aec: e8bd8070     	pop	{r4, r5, r6, pc}
    2af0: e4940004     	ldr	r0, [r4], #4
    2af4: e2855001     	add	r5, r5, #1
    2af8: ebffff4e     	bl	0x2838 <.plt+0x338>     @ imm = #-0x2c8
    2afc: e5961250     	ldr	r1, [r6, #0x250]
    2b00: e1550001     	cmp	r5, r1
    2b04: baffffde     	blt	0x2a84 <arbhar_rec_tilde_free+0x38> @ imm = #-0x88
    2b08: eaffffe9     	b	0x2ab4 <arbhar_rec_tilde_free+0x68> @ imm = #-0x5c
    2b0c: e28d6004     	add	r6, sp, #4
    2b10: e3a01002     	mov	r1, #2
    2b14: e1a00004     	mov	r0, r4
    2b18: e1a02006     	mov	r2, r6
    2b1c: ebfffed0     	bl	0x2664 <.plt+0x164>     @ imm = #-0x4c0
    2b20: e3700001     	cmn	r0, #1
    2b24: 0affffef     	beq	0x2ae8 <arbhar_rec_tilde_free+0x9c> @ imm = #-0x44
    2b28: e59d104c     	ldr	r1, [sp, #0x4c]
    2b2c: e3510000     	cmp	r1, #0
    2b30: 1affffec     	bne	0x2ae8 <arbhar_rec_tilde_free+0x9c> @ imm = #-0x50
    2b34: e1a02006     	mov	r2, r6
    2b38: e1a00004     	mov	r0, r4
    2b3c: ebfffec8     	bl	0x2664 <.plt+0x164>     @ imm = #-0x4e0
    2b40: e3700001     	cmn	r0, #1
    2b44: 1affffe7     	bne	0x2ae8 <arbhar_rec_tilde_free+0x9c> @ imm = #-0x64
    2b48: e59fe020     	ldr	lr, [pc, #0x20]         @ 0x2b70 <arbhar_rec_tilde_free+0x124>
    2b4c: e1a01004     	mov	r1, r4
    2b50: e08f000e     	add	r0, pc, lr
    2b54: ebfffec5     	bl	0x2670 <.plt+0x170>     @ imm = #-0x4ec
    2b58: eaffffe2     	b	0x2ae8 <arbhar_rec_tilde_free+0x9c> @ imm = #-0x78
    2b5c: e59f2010     	ldr	r2, [pc, #0x10]         @ 0x2b74 <arbhar_rec_tilde_free+0x128>
    2b60: e59611e8     	ldr	r1, [r6, #0x1e8]
    2b64: e08f0002     	add	r0, pc, r2
    2b68: ebfffec0     	bl	0x2670 <.plt+0x170>     @ imm = #-0x500
    2b6c: eaffffd8     	b	0x2ad4 <arbhar_rec_tilde_free+0x88> @ imm = #-0xa0
    2b70: 58 68 00 00  	.word	0x00006858
    2b74: 30 68 00 00  	.word	0x00006830

