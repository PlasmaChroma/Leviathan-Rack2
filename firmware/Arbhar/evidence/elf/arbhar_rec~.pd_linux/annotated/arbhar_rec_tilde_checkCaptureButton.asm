00004a14 <arbhar_rec_tilde_checkCaptureButton>:
    4a14: e92d4070     	push	{r4, r5, r6, lr}
    4a18: e3a01000     	mov	r1, #0
    4a1c: ed9f0a79     	vldr	s0, [pc, #484]          @ 0x4c08 <arbhar_rec_tilde_checkCaptureButton+0x1f4>  // f32=33
    4a20: e1a04000     	mov	r4, r0
    4a24: ebfff6d2     	bl	0x2574 <.plt+0x74>      @ imm = #-0x24b8  // CALL readFromSharedMem
    4a28: e1a00004     	mov	r0, r4
    4a2c: e3a01000     	mov	r1, #0
    4a30: eefd7ac0     	vcvt.s32.f32	s15, s0
    4a34: eeb30a0f     	vmov.f32	s0, #3.100000e+01
    4a38: ee175a90     	vmov	r5, s15
    4a3c: ebfff6cc     	bl	0x2574 <.plt+0x74>      @ imm = #-0x24d0  // CALL readFromSharedMem
    4a40: e59f31cc     	ldr	r3, [pc, #0x1cc]        @ 0x4c14 <arbhar_rec_tilde_checkCaptureButton+0x200>  // u32=0x15764; f32?=1.23185345e-40
    4a44: e08f0003     	add	r0, pc, r3
    4a48: e5901018     	ldr	r1, [r0, #0x18]
    4a4c: e1510005     	cmp	r1, r5
    4a50: eebd0ac0     	vcvt.s32.f32	s0, s0
    4a54: ed840a9f     	vstr	s0, [r4, #636]
    4a58: 08bd8070     	popeq	{r4, r5, r6, pc}
    4a5c: e3550000     	cmp	r5, #0
    4a60: da000025     	ble	0x4afc <arbhar_rec_tilde_checkCaptureButton+0xe8> @ imm = #0x94
    4a64: e3a01000     	mov	r1, #0
    4a68: e1a00004     	mov	r0, r4
    4a6c: ed9f0a66     	vldr	s0, [pc, #408]          @ 0x4c0c <arbhar_rec_tilde_checkCaptureButton+0x1f8>  // f32=249
    4a70: ebfff6bf     	bl	0x2574 <.plt+0x74>      @ imm = #-0x2504  // CALL readFromSharedMem
    4a74: e3a01000     	mov	r1, #0
    4a78: e1a00004     	mov	r0, r4
    4a7c: eefd1ac0     	vcvt.s32.f32	s3, s0
    4a80: ed9f0a62     	vldr	s0, [pc, #392]          @ 0x4c10 <arbhar_rec_tilde_checkCaptureButton+0x1fc>  // f32=34
    4a84: ee116a90     	vmov	r6, s3
    4a88: ebfff6b9     	bl	0x2574 <.plt+0x74>      @ imm = #-0x251c  // CALL readFromSharedMem
    4a8c: e3a02c53     	mov	r2, #21248
    4a90: e3402007     	movt	r2, #0x7
    4a94: e5940048     	ldr	r0, [r4, #0x48]
    4a98: e1560002     	cmp	r6, r2
    4a9c: c3a06000     	movgt	r6, #0
    4aa0: e3500001     	cmp	r0, #1
    4aa4: eebd2ac0     	vcvt.s32.f32	s4, s0
    4aa8: ee121a10     	vmov	r1, s4
    4aac: ed842aa2     	vstr	s4, [r4, #648]
    4ab0: 0a000045     	beq	0x4bcc <arbhar_rec_tilde_checkCaptureButton+0x1b8> @ imm = #0x114
    4ab4: e3510000     	cmp	r1, #0
    4ab8: e3a0e001     	mov	lr, #1
    4abc: e584e284     	str	lr, [r4, #0x284]
    4ac0: 11a01006     	movne	r1, r6
    4ac4: e5841080     	str	r1, [r4, #0x80]
    4ac8: e594c078     	ldr	r12, [r4, #0x78]
    4acc: e5943288     	ldr	r3, [r4, #0x288]
    4ad0: e35c0001     	cmp	r12, #1
    4ad4: 0a00002d     	beq	0x4b90 <arbhar_rec_tilde_checkCaptureButton+0x17c> @ imm = #0xb4
    4ad8: e3530000     	cmp	r3, #0
    4adc: e3a00001     	mov	r0, #1
    4ae0: e5840284     	str	r0, [r4, #0x284]
    4ae4: 11a03006     	movne	r3, r6
    4ae8: e5843080     	str	r3, [r4, #0x80]
    4aec: e59f0124     	ldr	r0, [pc, #0x124]        @ 0x4c18 <arbhar_rec_tilde_checkCaptureButton+0x204>  // u32=0x156b8; f32?=1.22944322e-40
    4af0: e08fe000     	add	lr, pc, r0
    4af4: e58e5018     	str	r5, [lr, #0x18]
    4af8: e8bd8070     	pop	{r4, r5, r6, pc}
    4afc: e5942048     	ldr	r2, [r4, #0x48]
    4b00: e3520001     	cmp	r2, #1
    4b04: 0a000011     	beq	0x4b50 <arbhar_rec_tilde_checkCaptureButton+0x13c> @ imm = #0x44
    4b08: e594e078     	ldr	lr, [r4, #0x78]
    4b0c: e35e0001     	cmp	lr, #1
    4b10: 1afffff5     	bne	0x4aec <arbhar_rec_tilde_checkCaptureButton+0xd8> @ imm = #-0x2c
    4b14: e1c424d8     	ldrd	r2, r3, [r4, #72]
    4b18: e5941288     	ldr	r1, [r4, #0x288]
    4b1c: e5940074     	ldr	r0, [r4, #0x74]
    4b20: e1e02002     	mvn	r2, r2
    4b24: e3510000     	cmp	r1, #0
    4b28: e1a0cfa2     	lsr	r12, r2, #31
    4b2c: e0833000     	add	r3, r3, r0
    4b30: e59402a0     	ldr	r0, [r4, #0x2a0]
    4b34: ee01ca10     	vmov	s2, r12
    4b38: e5843070     	str	r3, [r4, #0x70]
    4b3c: d3a03000     	movle	r3, #0
    4b40: e584328c     	str	r3, [r4, #0x28c]
    4b44: eeb80ac1     	vcvt.f32.s32	s0, s2
    4b48: ebfff725     	bl	0x27e4 <.plt+0x2e4>     @ imm = #-0x236c  // CALL outlet_float
    4b4c: eaffffe6     	b	0x4aec <arbhar_rec_tilde_checkCaptureButton+0xd8> @ imm = #-0x68
    4b50: e594c078     	ldr	r12, [r4, #0x78]
    4b54: e5941288     	ldr	r1, [r4, #0x288]
    4b58: e594e01c     	ldr	lr, [r4, #0x1c]
    4b5c: e1e0000c     	mvn	r0, r12
    4b60: e5943044     	ldr	r3, [r4, #0x44]
    4b64: e1a02fa0     	lsr	r2, r0, #31
    4b68: e3510000     	cmp	r1, #0
    4b6c: e08ec003     	add	r12, lr, r3
    4b70: e59402a0     	ldr	r0, [r4, #0x2a0]
    4b74: ee002a90     	vmov	s1, r2
    4b78: e584c040     	str	r12, [r4, #0x40]
    4b7c: d3a0c000     	movle	r12, #0
    4b80: e584c28c     	str	r12, [r4, #0x28c]
    4b84: eeb80ae0     	vcvt.f32.s32	s0, s1
    4b88: ebfff715     	bl	0x27e4 <.plt+0x2e4>     @ imm = #-0x23ac  // CALL outlet_float
    4b8c: eaffffdd     	b	0x4b08 <arbhar_rec_tilde_checkCaptureButton+0xf4> @ imm = #-0x8c
    4b90: e5942048     	ldr	r2, [r4, #0x48]
    4b94: e3530000     	cmp	r3, #0
    4b98: e594e04c     	ldr	lr, [r4, #0x4c]
    4b9c: e5941074     	ldr	r1, [r4, #0x74]
    4ba0: e1e0c002     	mvn	r12, r2
    4ba4: e59402a0     	ldr	r0, [r4, #0x2a0]
    4ba8: e1a03fac     	lsr	r3, r12, #31
    4bac: e08e2001     	add	r2, lr, r1
    4bb0: e5842070     	str	r2, [r4, #0x70]
    4bb4: d3a02000     	movle	r2, #0
    4bb8: ee033a10     	vmov	s6, r3
    4bbc: e584228c     	str	r2, [r4, #0x28c]
    4bc0: eeb80ac3     	vcvt.f32.s32	s0, s6
    4bc4: ebfff706     	bl	0x27e4 <.plt+0x2e4>     @ imm = #-0x23e8  // CALL outlet_float
    4bc8: eaffffc7     	b	0x4aec <arbhar_rec_tilde_checkCaptureButton+0xd8> @ imm = #-0xe4
    4bcc: e594c078     	ldr	r12, [r4, #0x78]
    4bd0: e3510000     	cmp	r1, #0
    4bd4: e594001c     	ldr	r0, [r4, #0x1c]
    4bd8: e5943044     	ldr	r3, [r4, #0x44]
    4bdc: e1e0200c     	mvn	r2, r12
    4be0: e1a01fa2     	lsr	r1, r2, #31
    4be4: e083e000     	add	lr, r3, r0
    4be8: e59402a0     	ldr	r0, [r4, #0x2a0]
    4bec: ee021a90     	vmov	s5, r1
    4bf0: e584e040     	str	lr, [r4, #0x40]
    4bf4: d3a0e000     	movle	lr, #0
    4bf8: e584e28c     	str	lr, [r4, #0x28c]
    4bfc: eeb80ae2     	vcvt.f32.s32	s0, s5
    4c00: ebfff6f7     	bl	0x27e4 <.plt+0x2e4>     @ imm = #-0x2424  // CALL outlet_float
    4c04: eaffffaf     	b	0x4ac8 <arbhar_rec_tilde_checkCaptureButton+0xb4> @ imm = #-0x144
    4c08: 00 00 04 42  	.word	0x42040000
    4c0c: 00 00 79 43  	.word	0x43790000
    4c10: 00 00 08 42  	.word	0x42080000
    4c14: 64 57 01 00  	.word	0x00015764
    4c18: b8 56 01 00  	.word	0x000156b8

