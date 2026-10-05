; lubadh::Channel::setPlaybackLoopMode(lubadh::LoopMode)
; VA 0x39ae8 size 276

   39ae8: e59030e8     	ldr	r3, [r0, #0xe8]
   39aec: e593308c     	ldr	r3, [r3, #0x8c]
   39af0: e1510003     	cmp	r1, r3
   39af4: 012fff1e     	bxeq	lr
   39af8: e92d4030     	push	{r4, r5, lr}
   39afc: e1a05000     	mov	r5, r0
   39b00: e1a04001     	mov	r4, r1
   39b04: e24dd034     	sub	sp, sp, #52
   39b08: e2851004     	add	r1, r5, #4
   39b0c: e1a0000d     	mov	r0, sp
   39b10: e3022554     	movw	r2, #0x2554
   39b14: e3402007     	movt	r2, #0x7
   39b18: ebffd42a     	bl	0x2ebc8
   39b1c: e3540000     	cmp	r4, #0
   39b20: e302354c     	movw	r3, #0x254c
   39b24: e3403007     	movt	r3, #0x7
   39b28: e3021544     	movw	r1, #0x2544
   39b2c: e3401007     	movt	r1, #0x7
   39b30: e1a0000d     	mov	r0, sp
   39b34: 11a01003     	movne	r1, r3
   39b38: ebff7243     	bl	0x1644c    @ imm = #-0x236f4 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6appendEPKc
   39b3c: e1a01000     	mov	r1, r0
   39b40: e28d0018     	add	r0, sp, #24
   39b44: ebff702d     	bl	0x15c00    @ imm = #-0x23f4c ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EOS4_
   39b48: e3090fec     	movw	r0, #0x9fec
   39b4c: e3400009     	movt	r0, #0x9
   39b50: e28d1018     	add	r1, sp, #24
   39b54: e3a02000     	mov	r2, #0
   39b58: eb00d8f0     	bl	0x6ff20
   39b5c: e59d0018     	ldr	r0, [sp, #0x18]
   39b60: e28d3020     	add	r3, sp, #32
   39b64: e1500003     	cmp	r0, r3
   39b68: 0a000000     	beq	0x39b70
   39b6c: ebff70b3     	bl	0x15e40    @ imm = #-0x23d34 ; _ZdlPv
   39b70: e59d0000     	ldr	r0, [sp]
   39b74: e28d3008     	add	r3, sp, #8
   39b78: e1500003     	cmp	r0, r3
   39b7c: 0a000000     	beq	0x39b84
   39b80: ebff70ae     	bl	0x15e40    @ imm = #-0x23d48 ; _ZdlPv
   39b84: e59530e8     	ldr	r3, [r5, #0xe8]
   39b88: e3540000     	cmp	r4, #0
   39b8c: e583408c     	str	r4, [r3, #0x8c]
   39b90: 0a000007     	beq	0x39bb4
   39b94: e3540001     	cmp	r4, #1
   39b98: 1a000003     	bne	0x39bac
   39b9c: e59330b0     	ldr	r3, [r3, #0xb0]
   39ba0: e1a00005     	mov	r0, r5
   39ba4: e5931010     	ldr	r1, [r3, #0x10]
   39ba8: ebffff5c     	bl	0x39920
   39bac: e28dd034     	add	sp, sp, #52
   39bb0: e8bd8030     	pop	{r4, r5, pc}
   39bb4: e59330b0     	ldr	r3, [r3, #0xb0]
   39bb8: e1a00005     	mov	r0, r5
   39bbc: e593100c     	ldr	r1, [r3, #0xc]
   39bc0: ebffff56     	bl	0x39920
   39bc4: e28dd034     	add	sp, sp, #52
   39bc8: e8bd8030     	pop	{r4, r5, pc}
   39bcc: e59d0018     	ldr	r0, [sp, #0x18]
   39bd0: e28d3020     	add	r3, sp, #32
   39bd4: e1500003     	cmp	r0, r3
   39bd8: 0a000000     	beq	0x39be0
   39bdc: ebff7097     	bl	0x15e40    @ imm = #-0x23da4 ; _ZdlPv
   39be0: e59d0000     	ldr	r0, [sp]
   39be4: e28d3008     	add	r3, sp, #8
   39be8: e1500003     	cmp	r0, r3
   39bec: 0a000000     	beq	0x39bf4
   39bf0: ebff7092     	bl	0x15e40    @ imm = #-0x23db8 ; _ZdlPv
   39bf4: ebff70d9     	bl	0x15f60    @ imm = #-0x23c9c ; __cxa_end_cleanup
   39bf8: eafffff8     	b	0x39be0
