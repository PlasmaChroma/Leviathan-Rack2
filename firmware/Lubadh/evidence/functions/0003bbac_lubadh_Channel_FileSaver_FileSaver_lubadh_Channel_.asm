; lubadh::Channel::FileSaver::FileSaver(lubadh::Channel&)
; VA 0x3bbac size 540

   3bbac: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   3bbb0: e1a05000     	mov	r5, r0
   3bbb4: e1a03001     	mov	r3, r1
   3bbb8: e24dd04c     	sub	sp, sp, #76
   3bbbc: e2812004     	add	r2, r1, #4
   3bbc0: e4853004     	str	r3, [r5], #4
   3bbc4: e1a04000     	mov	r4, r0
   3bbc8: e3041dc0     	movw	r1, #0x4dc0
   3bbcc: e3401009     	movt	r1, #0x9
   3bbd0: e1a0000d     	mov	r0, sp
   3bbd4: ebffcbe8     	bl	0x2eb7c
   3bbd8: e1a0100d     	mov	r1, sp
   3bbdc: e1a00005     	mov	r0, r5
   3bbe0: e3a02000     	mov	r2, #0
   3bbe4: eb00d0ff     	bl	0x6ffe8
   3bbe8: e5942000     	ldr	r2, [r4]
   3bbec: e28d0018     	add	r0, sp, #24
   3bbf0: e59f11c0     	ldr	r1, [pc, #0x1c0]        @ 0x3bdb8
   3bbf4: e2822004     	add	r2, r2, #4
   3bbf8: ebffcbdf     	bl	0x2eb7c
   3bbfc: e28d1018     	add	r1, sp, #24
   3bc00: e2840020     	add	r0, r4, #32
   3bc04: eb001880     	bl	0x41e0c
   3bc08: e5942000     	ldr	r2, [r4]
   3bc0c: e28d0030     	add	r0, sp, #48
   3bc10: e59fa1a4     	ldr	r10, [pc, #0x1a4]       @ 0x3bdbc
   3bc14: e59f11a4     	ldr	r1, [pc, #0x1a4]        @ 0x3bdc0
   3bc18: e2822004     	add	r2, r2, #4
   3bc1c: e584a020     	str	r10, [r4, #0x20]
   3bc20: ebffcbd5     	bl	0x2eb7c
   3bc24: e28d1030     	add	r1, sp, #48
   3bc28: e2840044     	add	r0, r4, #68
   3bc2c: eb001876     	bl	0x41e0c
   3bc30: e59d0030     	ldr	r0, [sp, #0x30]
   3bc34: e28d8038     	add	r8, sp, #56
   3bc38: e59f9184     	ldr	r9, [pc, #0x184]        @ 0x3bdc4
   3bc3c: e1500008     	cmp	r0, r8
   3bc40: e5849044     	str	r9, [r4, #0x44]
   3bc44: 0a000000     	beq	0x3bc4c
   3bc48: ebff687c     	bl	0x15e40    @ imm = #-0x25e10 ; _ZdlPv
   3bc4c: e59d0018     	ldr	r0, [sp, #0x18]
   3bc50: e28d7020     	add	r7, sp, #32
   3bc54: e1500007     	cmp	r0, r7
   3bc58: 0a000000     	beq	0x3bc60
   3bc5c: ebff6877     	bl	0x15e40    @ imm = #-0x25e24 ; _ZdlPv
   3bc60: e59d0000     	ldr	r0, [sp]
   3bc64: e28d6008     	add	r6, sp, #8
   3bc68: e1500006     	cmp	r0, r6
   3bc6c: 0a000000     	beq	0x3bc74
   3bc70: ebff6872     	bl	0x15e40    @ imm = #-0x25e38 ; _ZdlPv
   3bc74: e30227b8     	movw	r2, #0x27b8
   3bc78: e3402007     	movt	r2, #0x7
   3bc7c: e3041dc0     	movw	r1, #0x4dc0
   3bc80: e3401009     	movt	r1, #0x9
   3bc84: e1a0000d     	mov	r0, sp
   3bc88: e284b068     	add	r11, r4, #104
   3bc8c: ebffcbcd     	bl	0x2ebc8
   3bc90: e1a0100d     	mov	r1, sp
   3bc94: e1a0000b     	mov	r0, r11
   3bc98: e3a02000     	mov	r2, #0
   3bc9c: eb00d0d1     	bl	0x6ffe8
   3bca0: e30227b8     	movw	r2, #0x27b8
   3bca4: e3402007     	movt	r2, #0x7
   3bca8: e59f1108     	ldr	r1, [pc, #0x108]        @ 0x3bdb8
   3bcac: e28d0018     	add	r0, sp, #24
   3bcb0: ebffcbc4     	bl	0x2ebc8
   3bcb4: e28d1018     	add	r1, sp, #24
   3bcb8: e2840084     	add	r0, r4, #132
   3bcbc: eb001852     	bl	0x41e0c
   3bcc0: e30227b8     	movw	r2, #0x27b8
   3bcc4: e3402007     	movt	r2, #0x7
   3bcc8: e59f10f0     	ldr	r1, [pc, #0xf0]         @ 0x3bdc0
   3bccc: e28d0030     	add	r0, sp, #48
   3bcd0: e584a084     	str	r10, [r4, #0x84]
   3bcd4: ebffcbbb     	bl	0x2ebc8
   3bcd8: e28d1030     	add	r1, sp, #48
   3bcdc: e28400a8     	add	r0, r4, #168
   3bce0: eb001849     	bl	0x41e0c
   3bce4: e59d0030     	ldr	r0, [sp, #0x30]
   3bce8: e58490a8     	str	r9, [r4, #0xa8]
   3bcec: e1500008     	cmp	r0, r8
   3bcf0: 0a000000     	beq	0x3bcf8
   3bcf4: ebff6851     	bl	0x15e40    @ imm = #-0x25ebc ; _ZdlPv
   3bcf8: e59d0018     	ldr	r0, [sp, #0x18]
   3bcfc: e1500007     	cmp	r0, r7
   3bd00: 0a000000     	beq	0x3bd08
   3bd04: ebff684d     	bl	0x15e40    @ imm = #-0x25ecc ; _ZdlPv
   3bd08: e59d0000     	ldr	r0, [sp]
   3bd0c: e1500006     	cmp	r0, r6
   3bd10: 0a000000     	beq	0x3bd18
   3bd14: ebff6849     	bl	0x15e40    @ imm = #-0x25edc ; _ZdlPv
   3bd18: e1a00004     	mov	r0, r4
   3bd1c: e28dd04c     	add	sp, sp, #76
   3bd20: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
   3bd24: e59d0000     	ldr	r0, [sp]
   3bd28: e28d3008     	add	r3, sp, #8
   3bd2c: e1500003     	cmp	r0, r3
   3bd30: 0a00000e     	beq	0x3bd70
   3bd34: ebff6841     	bl	0x15e40    @ imm = #-0x25efc ; _ZdlPv
   3bd38: ea00000c     	b	0x3bd70
   3bd3c: e59d0030     	ldr	r0, [sp, #0x30]
   3bd40: e1500008     	cmp	r0, r8
   3bd44: 0a000000     	beq	0x3bd4c
   3bd48: ebff683c     	bl	0x15e40    @ imm = #-0x25f10 ; _ZdlPv
   3bd4c: e59d0018     	ldr	r0, [sp, #0x18]
   3bd50: e1500007     	cmp	r0, r7
   3bd54: 0a000000     	beq	0x3bd5c
   3bd58: ebff6838     	bl	0x15e40    @ imm = #-0x25f20 ; _ZdlPv
   3bd5c: e59d0000     	ldr	r0, [sp]
   3bd60: e1500006     	cmp	r0, r6
   3bd64: 1a000003     	bne	0x3bd78
   3bd68: e1a00005     	mov	r0, r5
   3bd6c: eb0014b5     	bl	0x41048
   3bd70: ebff687a     	bl	0x15f60    @ imm = #-0x25e18 ; __cxa_end_cleanup
   3bd74: eafffff4     	b	0x3bd4c
   3bd78: ebff6830     	bl	0x15e40    @ imm = #-0x25f40 ; _ZdlPv
   3bd7c: eafffff9     	b	0x3bd68
   3bd80: eafffff5     	b	0x3bd5c
   3bd84: eafffff7     	b	0x3bd68
   3bd88: e59d0030     	ldr	r0, [sp, #0x30]
   3bd8c: e28d3038     	add	r3, sp, #56
   3bd90: e1500003     	cmp	r0, r3
   3bd94: 0a000000     	beq	0x3bd9c
   3bd98: ebff6828     	bl	0x15e40    @ imm = #-0x25f60 ; _ZdlPv
   3bd9c: e59d0018     	ldr	r0, [sp, #0x18]
   3bda0: e28d3020     	add	r3, sp, #32
   3bda4: e1500003     	cmp	r0, r3
   3bda8: 0affffdd     	beq	0x3bd24
   3bdac: ebff6823     	bl	0x15e40    @ imm = #-0x25f74 ; _ZdlPv
   3bdb0: eaffffdb     	b	0x3bd24
   3bdb4: eafffff8     	b	0x3bd9c
   3bdb8: d8 4d 09 00  	.word	0x00094dd8
   3bdbc: e0 1e 07 00  	.word	0x00071ee0
   3bdc0: f0 4d 09 00  	.word	0x00094df0
   3bdc4: c0 1f 07 00  	.word	0x00071fc0
