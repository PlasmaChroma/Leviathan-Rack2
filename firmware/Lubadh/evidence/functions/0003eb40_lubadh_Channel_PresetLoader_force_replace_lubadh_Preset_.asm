; lubadh::Channel::PresetLoader::force_replace(lubadh::Preset&)
; VA 0x3eb40 size 920

   3eb40: e92d43f0     	push	{r4, r5, r6, r7, r8, r9, lr}
   3eb44: e1a05000     	mov	r5, r0
   3eb48: e5914000     	ldr	r4, [r1]
   3eb4c: e24dd084     	sub	sp, sp, #132
   3eb50: e3540001     	cmp	r4, #1
   3eb54: 8a000009     	bhi	0x3eb80
   3eb58: e590302c     	ldr	r3, [r0, #0x2c]
   3eb5c: e304229c     	movw	r2, #0x429c
   3eb60: e0243492     	mla	r4, r2, r4, r3
   3eb64: e1a00004     	mov	r0, r4
   3eb68: ebff5d35     	bl	0x16044    @ imm = #-0x28b2c ; memcpy
   3eb6c: e5950000     	ldr	r0, [r5]
   3eb70: e1a01004     	mov	r1, r4
   3eb74: ebfffc85     	bl	0x3dd90
   3eb78: e28dd084     	add	sp, sp, #132
   3eb7c: e8bd83f0     	pop	{r4, r5, r6, r7, r8, r9, pc}
   3eb80: e3a00008     	mov	r0, #8
   3eb84: e1a06001     	mov	r6, r1
   3eb88: ebff5bfb     	bl	0x15b7c    @ imm = #-0x29014 ; __cxa_allocate_exception
   3eb8c: e3023ab0     	movw	r3, #0x2ab0
   3eb90: e3403007     	movt	r3, #0x7
   3eb94: e1a08000     	mov	r8, r0
   3eb98: e3061218     	movw	r1, #0x6218
   3eb9c: e3401001     	movt	r1, #0x1
   3eba0: e28d0020     	add	r0, sp, #32
   3eba4: e3a02010     	mov	r2, #16
   3eba8: e58d4000     	str	r4, [sp]
   3ebac: ebffe0fd     	bl	0x36fa8
   3ebb0: e3022860     	movw	r2, #0x2860
   3ebb4: e3402007     	movt	r2, #0x7
   3ebb8: e28d0020     	add	r0, sp, #32
   3ebbc: e3a01000     	mov	r1, #0
   3ebc0: ebff5c53     	bl	0x15d14    @ imm = #-0x28eb4 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6insertEjPKc
   3ebc4: e1a01000     	mov	r1, r0
   3ebc8: e28d0038     	add	r0, sp, #56
   3ebcc: ebff5c0b     	bl	0x15c00    @ imm = #-0x28fd4 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EOS4_
   3ebd0: e3021888     	movw	r1, #0x2888
   3ebd4: e3401007     	movt	r1, #0x7
   3ebd8: e28d0038     	add	r0, sp, #56
   3ebdc: ebff5e1a     	bl	0x1644c    @ imm = #-0x28798 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6appendEPKc
   3ebe0: e28d9050     	add	r9, sp, #80
   3ebe4: e1a01000     	mov	r1, r0
   3ebe8: e1a00009     	mov	r0, r9
   3ebec: ebff5c03     	bl	0x15c00    @ imm = #-0x28ff4 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EOS4_
   3ebf0: e3a02002     	mov	r2, #2
   3ebf4: e3023ab0     	movw	r3, #0x2ab0
   3ebf8: e3403007     	movt	r3, #0x7
   3ebfc: e58d2000     	str	r2, [sp]
   3ec00: e3061218     	movw	r1, #0x6218
   3ec04: e3401001     	movt	r1, #0x1
   3ec08: e28d0008     	add	r0, sp, #8
   3ec0c: e3a02010     	mov	r2, #16
   3ec10: ebffe0e4     	bl	0x36fa8
   3ec14: e28d7068     	add	r7, sp, #104
   3ec18: e28d2008     	add	r2, sp, #8
   3ec1c: e1a01009     	mov	r1, r9
   3ec20: e1a00007     	mov	r0, r7
   3ec24: ebffbf30     	bl	0x2e8ec
   3ec28: e1a01007     	mov	r1, r7
   3ec2c: e1a00008     	mov	r0, r8
   3ec30: ebff5d5d     	bl	0x161ac    @ imm = #-0x28a8c ; _ZNSt12out_of_rangeC1ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE
   3ec34: e59d0068     	ldr	r0, [sp, #0x68]
   3ec38: e28d3070     	add	r3, sp, #112
   3ec3c: e1500003     	cmp	r0, r3
   3ec40: 0a000000     	beq	0x3ec48
   3ec44: ebff5c7d     	bl	0x15e40    @ imm = #-0x28e0c ; _ZdlPv
   3ec48: e59d0008     	ldr	r0, [sp, #0x8]
   3ec4c: e28d3010     	add	r3, sp, #16
   3ec50: e1500003     	cmp	r0, r3
   3ec54: 0a000000     	beq	0x3ec5c
   3ec58: ebff5c78     	bl	0x15e40    @ imm = #-0x28e20 ; _ZdlPv
   3ec5c: e59d0050     	ldr	r0, [sp, #0x50]
   3ec60: e28d3058     	add	r3, sp, #88
   3ec64: e1500003     	cmp	r0, r3
   3ec68: 0a000000     	beq	0x3ec70
   3ec6c: ebff5c73     	bl	0x15e40    @ imm = #-0x28e34 ; _ZdlPv
   3ec70: e59d0038     	ldr	r0, [sp, #0x38]
   3ec74: e28d3040     	add	r3, sp, #64
   3ec78: e1500003     	cmp	r0, r3
   3ec7c: 0a000000     	beq	0x3ec84
   3ec80: ebff5c6e     	bl	0x15e40    @ imm = #-0x28e48 ; _ZdlPv
   3ec84: e59d0020     	ldr	r0, [sp, #0x20]
   3ec88: e28d3028     	add	r3, sp, #40
   3ec8c: e1500003     	cmp	r0, r3
   3ec90: 0a000000     	beq	0x3ec98
   3ec94: ebff5c69     	bl	0x15e40    @ imm = #-0x28e5c ; _ZdlPv
   3ec98: e3062110     	movw	r2, #0x6110
   3ec9c: e3402001     	movt	r2, #0x1
   3eca0: e30e1df8     	movw	r1, #0xedf8
   3eca4: e3401008     	movt	r1, #0x8
   3eca8: e1a00008     	mov	r0, r8
   3ecac: ebff5d9b     	bl	0x16320    @ imm = #-0x28994 ; __cxa_throw
   3ecb0: e1a07000     	mov	r7, r0
   3ecb4: e1a09001     	mov	r9, r1
   3ecb8: e1a00008     	mov	r0, r8
   3ecbc: ebff5c0b     	bl	0x15cf0    @ imm = #-0x28fd4 ; __cxa_free_exception
   3ecc0: e1a00007     	mov	r0, r7
   3ecc4: e3590001     	cmp	r9, #1
   3ecc8: 1a000066     	bne	0x3ee68
   3eccc: ebff5b8c     	bl	0x15b04    @ imm = #-0x291d0 ; __cxa_begin_catch
   3ecd0: e5903000     	ldr	r3, [r0]
   3ecd4: e28d7068     	add	r7, sp, #104
   3ecd8: e5933008     	ldr	r3, [r3, #0x8]
   3ecdc: e12fff33     	blx	r3
   3ece0: e1a01000     	mov	r1, r0
   3ece4: e1a00007     	mov	r0, r7
   3ece8: ebffe056     	bl	0x36e48
   3ecec: e3090fec     	movw	r0, #0x9fec
   3ecf0: e3400009     	movt	r0, #0x9
   3ecf4: e3a02002     	mov	r2, #2
   3ecf8: e1a01007     	mov	r1, r7
   3ecfc: eb00c487     	bl	0x6ff20
   3ed00: e59d0068     	ldr	r0, [sp, #0x68]
   3ed04: e28d8070     	add	r8, sp, #112
   3ed08: e1500008     	cmp	r0, r8
   3ed0c: 1a00005e     	bne	0x3ee8c
   3ed10: e5951000     	ldr	r1, [r5]
   3ed14: e3022894     	movw	r2, #0x2894
   3ed18: e3402007     	movt	r2, #0x7
   3ed1c: e28d0038     	add	r0, sp, #56
   3ed20: e2811004     	add	r1, r1, #4
   3ed24: ebffbfa7     	bl	0x2ebc8
   3ed28: e28d9050     	add	r9, sp, #80
   3ed2c: e3003d28     	movw	r3, #0xd28
   3ed30: e3403007     	movt	r3, #0x7
   3ed34: e3061218     	movw	r1, #0x6218
   3ed38: e3401001     	movt	r1, #0x1
   3ed3c: e1a00009     	mov	r0, r9
   3ed40: e3a02010     	mov	r2, #16
   3ed44: e58d4000     	str	r4, [sp]
   3ed48: ebffe096     	bl	0x36fa8
   3ed4c: e1a02009     	mov	r2, r9
   3ed50: e28d1038     	add	r1, sp, #56
   3ed54: e1a00007     	mov	r0, r7
   3ed58: ebffbee3     	bl	0x2e8ec
   3ed5c: e3090fec     	movw	r0, #0x9fec
   3ed60: e3400009     	movt	r0, #0x9
   3ed64: e1a01007     	mov	r1, r7
   3ed68: e3a02002     	mov	r2, #2
   3ed6c: eb00c46b     	bl	0x6ff20
   3ed70: e59d0068     	ldr	r0, [sp, #0x68]
   3ed74: e1500008     	cmp	r0, r8
   3ed78: 0a000000     	beq	0x3ed80
   3ed7c: ebff5c2f     	bl	0x15e40    @ imm = #-0x28f44 ; _ZdlPv
   3ed80: e59d0050     	ldr	r0, [sp, #0x50]
   3ed84: e28d3058     	add	r3, sp, #88
   3ed88: e1500003     	cmp	r0, r3
   3ed8c: 0a000000     	beq	0x3ed94
   3ed90: ebff5c2a     	bl	0x15e40    @ imm = #-0x28f58 ; _ZdlPv
   3ed94: e59d0038     	ldr	r0, [sp, #0x38]
   3ed98: e28d3040     	add	r3, sp, #64
   3ed9c: e1500003     	cmp	r0, r3
   3eda0: 0a000000     	beq	0x3eda8
   3eda4: ebff5c25     	bl	0x15e40    @ imm = #-0x28f6c ; _ZdlPv
   3eda8: e1a01006     	mov	r1, r6
   3edac: e304229c     	movw	r2, #0x429c
   3edb0: e595002c     	ldr	r0, [r5, #0x2c]
   3edb4: ebff5ca2     	bl	0x16044    @ imm = #-0x28d78 ; memcpy
   3edb8: ebff5d2b     	bl	0x1626c    @ imm = #-0x28b54 ; __cxa_end_catch
   3edbc: e595402c     	ldr	r4, [r5, #0x2c]
   3edc0: eaffff69     	b	0x3eb6c
   3edc4: e1a07000     	mov	r7, r0
   3edc8: e1a09001     	mov	r9, r1
   3edcc: e59d0008     	ldr	r0, [sp, #0x8]
   3edd0: e28d3010     	add	r3, sp, #16
   3edd4: e1500003     	cmp	r0, r3
   3edd8: 0a000000     	beq	0x3ede0
   3eddc: ebff5c17     	bl	0x15e40    @ imm = #-0x28fa4 ; _ZdlPv
   3ede0: e59d0050     	ldr	r0, [sp, #0x50]
   3ede4: e28d3058     	add	r3, sp, #88
   3ede8: e1500003     	cmp	r0, r3
   3edec: 0a000000     	beq	0x3edf4
   3edf0: ebff5c12     	bl	0x15e40    @ imm = #-0x28fb8 ; _ZdlPv
   3edf4: e59d0038     	ldr	r0, [sp, #0x38]
   3edf8: e28d3040     	add	r3, sp, #64
   3edfc: e1500003     	cmp	r0, r3
   3ee00: 0a000000     	beq	0x3ee08
   3ee04: ebff5c0d     	bl	0x15e40    @ imm = #-0x28fcc ; _ZdlPv
   3ee08: e59d0020     	ldr	r0, [sp, #0x20]
   3ee0c: e28d3028     	add	r3, sp, #40
   3ee10: e1500003     	cmp	r0, r3
   3ee14: 0affffa7     	beq	0x3ecb8
   3ee18: ebff5c08     	bl	0x15e40    @ imm = #-0x28fe0 ; _ZdlPv
   3ee1c: eaffffa5     	b	0x3ecb8
   3ee20: e1a07000     	mov	r7, r0
   3ee24: e1a09001     	mov	r9, r1
   3ee28: eaffffec     	b	0x3ede0
   3ee2c: e59d3068     	ldr	r3, [sp, #0x68]
   3ee30: e28d2070     	add	r2, sp, #112
   3ee34: e1a07000     	mov	r7, r0
   3ee38: e1a09001     	mov	r9, r1
   3ee3c: e1530002     	cmp	r3, r2
   3ee40: 0affffe1     	beq	0x3edcc
   3ee44: e1a00003     	mov	r0, r3
   3ee48: ebff5bfc     	bl	0x15e40    @ imm = #-0x29010 ; _ZdlPv
   3ee4c: eaffffde     	b	0x3edcc
   3ee50: e59d0038     	ldr	r0, [sp, #0x38]
   3ee54: e28d3040     	add	r3, sp, #64
   3ee58: e1500003     	cmp	r0, r3
   3ee5c: 0a000000     	beq	0x3ee64
   3ee60: ebff5bf6     	bl	0x15e40    @ imm = #-0x29028 ; _ZdlPv
   3ee64: ebff5d00     	bl	0x1626c    @ imm = #-0x28c00 ; __cxa_end_catch
   3ee68: ebff5c3c     	bl	0x15f60    @ imm = #-0x28f10 ; __cxa_end_cleanup
   3ee6c: e1a07000     	mov	r7, r0
   3ee70: e1a09001     	mov	r9, r1
   3ee74: eaffffde     	b	0x3edf4
   3ee78: e1a07000     	mov	r7, r0
   3ee7c: e1a09001     	mov	r9, r1
   3ee80: eaffffe0     	b	0x3ee08
   3ee84: e1a09001     	mov	r9, r1
   3ee88: eaffff8d     	b	0x3ecc4
   3ee8c: ebff5beb     	bl	0x15e40    @ imm = #-0x29054 ; _ZdlPv
   3ee90: eaffff9e     	b	0x3ed10
   3ee94: e59d0068     	ldr	r0, [sp, #0x68]
   3ee98: e1500008     	cmp	r0, r8
   3ee9c: 0a000000     	beq	0x3eea4
   3eea0: ebff5be6     	bl	0x15e40    @ imm = #-0x29068 ; _ZdlPv
   3eea4: e59d0050     	ldr	r0, [sp, #0x50]
   3eea8: e28d3058     	add	r3, sp, #88
   3eeac: e1500003     	cmp	r0, r3
   3eeb0: 0affffe6     	beq	0x3ee50
   3eeb4: ebff5be1     	bl	0x15e40    @ imm = #-0x2907c ; _ZdlPv
   3eeb8: eaffffe4     	b	0x3ee50
   3eebc: eafffff8     	b	0x3eea4
   3eec0: e59d0068     	ldr	r0, [sp, #0x68]
   3eec4: e28d3070     	add	r3, sp, #112
   3eec8: e1500003     	cmp	r0, r3
   3eecc: 1affffe3     	bne	0x3ee60
   3eed0: eaffffe3     	b	0x3ee64
   3eed4: eaffffe2     	b	0x3ee64
