; lubadh::Channel::setPlaybackTrackMode(lubadh::ParamMode)
; VA 0x39920 size 456

   39920: e59030e8     	ldr	r3, [r0, #0xe8]
   39924: e5933094     	ldr	r3, [r3, #0x94]
   39928: e1510003     	cmp	r1, r3
   3992c: 012fff1e     	bxeq	lr
   39930: e92d4070     	push	{r4, r5, r6, lr}
   39934: e1a05000     	mov	r5, r0
   39938: e1a04001     	mov	r4, r1
   3993c: e24dd030     	sub	sp, sp, #48
   39940: e2851004     	add	r1, r5, #4
   39944: e1a0000d     	mov	r0, sp
   39948: e3022524     	movw	r2, #0x2524
   3994c: e3402007     	movt	r2, #0x7
   39950: ebffd49c     	bl	0x2ebc8
   39954: e3540000     	cmp	r4, #0
   39958: e3023514     	movw	r3, #0x2514
   3995c: e3403007     	movt	r3, #0x7
   39960: e3021508     	movw	r1, #0x2508
   39964: e3401007     	movt	r1, #0x7
   39968: e1a0000d     	mov	r0, sp
   3996c: 11a01003     	movne	r1, r3
   39970: ebff72b5     	bl	0x1644c    @ imm = #-0x2352c ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6appendEPKc
   39974: e1a01000     	mov	r1, r0
   39978: e28d0018     	add	r0, sp, #24
   3997c: ebff709f     	bl	0x15c00    @ imm = #-0x23d84 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EOS4_
   39980: e3090fec     	movw	r0, #0x9fec
   39984: e3400009     	movt	r0, #0x9
   39988: e28d1018     	add	r1, sp, #24
   3998c: e3a02000     	mov	r2, #0
   39990: eb00d962     	bl	0x6ff20
   39994: e59d0018     	ldr	r0, [sp, #0x18]
   39998: e28d3020     	add	r3, sp, #32
   3999c: e1500003     	cmp	r0, r3
   399a0: 0a000000     	beq	0x399a8
   399a4: ebff7125     	bl	0x15e40    @ imm = #-0x23b6c ; _ZdlPv
   399a8: e59d0000     	ldr	r0, [sp]
   399ac: e28d3008     	add	r3, sp, #8
   399b0: e1500003     	cmp	r0, r3
   399b4: 0a000000     	beq	0x399bc
   399b8: ebff7120     	bl	0x15e40    @ imm = #-0x23b80 ; _ZdlPv
   399bc: e59530e8     	ldr	r3, [r5, #0xe8]
   399c0: e3540000     	cmp	r4, #0
   399c4: e5834094     	str	r4, [r3, #0x94]
   399c8: 0a000020     	beq	0x39a50
   399cc: e3540001     	cmp	r4, #1
   399d0: 1a00001c     	bne	0x39a48
   399d4: e2850fda     	add	r0, r5, #872
   399d8: eb004fe9     	bl	0x4d984
   399dc: e59530e8     	ldr	r3, [r5, #0xe8]
   399e0: e2404004     	sub	r4, r0, #4
   399e4: e280604c     	add	r6, r0, #76
   399e8: e5b40004     	ldr	r0, [r4, #0x4]!
   399ec: e3500000     	cmp	r0, #0
   399f0: 0a000002     	beq	0x39a00
   399f4: ed930a02     	vldr	s0, [r3, #8]
   399f8: eb004947     	bl	0x4bf1c
   399fc: e59530e8     	ldr	r3, [r5, #0xe8]
   39a00: e1540006     	cmp	r4, r6
   39a04: 1afffff7     	bne	0x399e8
   39a08: e59330a0     	ldr	r3, [r3, #0xa0]
   39a0c: e3530001     	cmp	r3, #1
   39a10: 0a00000c     	beq	0x39a48
   39a14: e2850ee6     	add	r0, r5, #3680
   39a18: e2800008     	add	r0, r0, #8
   39a1c: eb004fd8     	bl	0x4d984
   39a20: e2404004     	sub	r4, r0, #4
   39a24: e280604c     	add	r6, r0, #76
   39a28: e5b40004     	ldr	r0, [r4, #0x4]!
   39a2c: e3500000     	cmp	r0, #0
   39a30: 0a000002     	beq	0x39a40
   39a34: e59530e8     	ldr	r3, [r5, #0xe8]
   39a38: ed930a02     	vldr	s0, [r3, #8]
   39a3c: eb004936     	bl	0x4bf1c
   39a40: e1540006     	cmp	r4, r6
   39a44: 1afffff7     	bne	0x39a28
   39a48: e28dd030     	add	sp, sp, #48
   39a4c: e8bd8070     	pop	{r4, r5, r6, pc}
   39a50: e2850fda     	add	r0, r5, #872
   39a54: eb004fca     	bl	0x4d984
   39a58: e2404004     	sub	r4, r0, #4
   39a5c: e280604c     	add	r6, r0, #76
   39a60: e5b40004     	ldr	r0, [r4, #0x4]!
   39a64: e3500000     	cmp	r0, #0
   39a68: 0a000000     	beq	0x39a70
   39a6c: eb00492e     	bl	0x4bf2c
   39a70: e1540006     	cmp	r4, r6
   39a74: 1afffff9     	bne	0x39a60
   39a78: e59530e8     	ldr	r3, [r5, #0xe8]
   39a7c: e59330a0     	ldr	r3, [r3, #0xa0]
   39a80: e3530001     	cmp	r3, #1
   39a84: 0affffef     	beq	0x39a48
   39a88: e2850ee6     	add	r0, r5, #3680
   39a8c: e2800008     	add	r0, r0, #8
   39a90: eb004fbb     	bl	0x4d984
   39a94: e2404004     	sub	r4, r0, #4
   39a98: e280504c     	add	r5, r0, #76
   39a9c: e5b40004     	ldr	r0, [r4, #0x4]!
   39aa0: e3500000     	cmp	r0, #0
   39aa4: 0a000000     	beq	0x39aac
   39aa8: eb00491f     	bl	0x4bf2c
   39aac: e1550004     	cmp	r5, r4
   39ab0: 1afffff9     	bne	0x39a9c
   39ab4: eaffffe3     	b	0x39a48
   39ab8: e59d0018     	ldr	r0, [sp, #0x18]
   39abc: e28d3020     	add	r3, sp, #32
   39ac0: e1500003     	cmp	r0, r3
   39ac4: 0a000000     	beq	0x39acc
   39ac8: ebff70dc     	bl	0x15e40    @ imm = #-0x23c90 ; _ZdlPv
   39acc: e59d0000     	ldr	r0, [sp]
   39ad0: e28d3008     	add	r3, sp, #8
   39ad4: e1500003     	cmp	r0, r3
   39ad8: 0a000000     	beq	0x39ae0
   39adc: ebff70d7     	bl	0x15e40    @ imm = #-0x23ca4 ; _ZdlPv
   39ae0: ebff711e     	bl	0x15f60    @ imm = #-0x23b88 ; __cxa_end_cleanup
   39ae4: eafffff8     	b	0x39acc
