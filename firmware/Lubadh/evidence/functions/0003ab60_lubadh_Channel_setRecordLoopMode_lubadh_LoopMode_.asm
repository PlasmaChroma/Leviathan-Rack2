; lubadh::Channel::setRecordLoopMode(lubadh::LoopMode)
; VA 0x3ab60 size 652

   3ab60: e59030e8     	ldr	r3, [r0, #0xe8]
   3ab64: e5933098     	ldr	r3, [r3, #0x98]
   3ab68: e1510003     	cmp	r1, r3
   3ab6c: 012fff1e     	bxeq	lr
   3ab70: e92d43f0     	push	{r4, r5, r6, r7, r8, r9, lr}
   3ab74: e2807004     	add	r7, r0, #4
   3ab78: e1a04001     	mov	r4, r1
   3ab7c: e24dd054     	sub	sp, sp, #84
   3ab80: e1a01007     	mov	r1, r7
   3ab84: e1a05000     	mov	r5, r0
   3ab88: e30226dc     	movw	r2, #0x26dc
   3ab8c: e3402007     	movt	r2, #0x7
   3ab90: e28d0020     	add	r0, sp, #32
   3ab94: ebffd00b     	bl	0x2ebc8
   3ab98: e3540000     	cmp	r4, #0
   3ab9c: e302354c     	movw	r3, #0x254c
   3aba0: e3403007     	movt	r3, #0x7
   3aba4: e3021544     	movw	r1, #0x2544
   3aba8: e3401007     	movt	r1, #0x7
   3abac: e28d0020     	add	r0, sp, #32
   3abb0: 11a01003     	movne	r1, r3
   3abb4: ebff6e24     	bl	0x1644c    @ imm = #-0x24770 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6appendEPKc
   3abb8: e28d6038     	add	r6, sp, #56
   3abbc: e1a01000     	mov	r1, r0
   3abc0: e1a00006     	mov	r0, r6
   3abc4: ebff6c0d     	bl	0x15c00    @ imm = #-0x24fcc ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EOS4_
   3abc8: e3090fec     	movw	r0, #0x9fec
   3abcc: e3400009     	movt	r0, #0x9
   3abd0: e1a01006     	mov	r1, r6
   3abd4: e3a02000     	mov	r2, #0
   3abd8: eb00d4d0     	bl	0x6ff20
   3abdc: e59d0038     	ldr	r0, [sp, #0x38]
   3abe0: e28d8040     	add	r8, sp, #64
   3abe4: e1500008     	cmp	r0, r8
   3abe8: 0a000000     	beq	0x3abf0
   3abec: ebff6c93     	bl	0x15e40    @ imm = #-0x24db4 ; _ZdlPv
   3abf0: e59d0020     	ldr	r0, [sp, #0x20]
   3abf4: e28d9028     	add	r9, sp, #40
   3abf8: e1500009     	cmp	r0, r9
   3abfc: 0a000000     	beq	0x3ac04
   3ac00: ebff6c8e     	bl	0x15e40    @ imm = #-0x24dc8 ; _ZdlPv
   3ac04: e59530e8     	ldr	r3, [r5, #0xe8]
   3ac08: e3540000     	cmp	r4, #0
   3ac0c: e5834098     	str	r4, [r3, #0x98]
   3ac10: 0a000007     	beq	0x3ac34
   3ac14: e3540001     	cmp	r4, #1
   3ac18: 1a000003     	bne	0x3ac2c
   3ac1c: e5953278     	ldr	r3, [r5, #0x278]
   3ac20: e5933004     	ldr	r3, [r3, #0x4]
   3ac24: e3530002     	cmp	r3, #2
   3ac28: 0a000004     	beq	0x3ac40
   3ac2c: e28dd054     	add	sp, sp, #84
   3ac30: e8bd83f0     	pop	{r4, r5, r6, r7, r8, r9, pc}
   3ac34: e5c5407c     	strb	r4, [r5, #0x7c]
   3ac38: e28dd054     	add	sp, sp, #84
   3ac3c: e8bd83f0     	pop	{r4, r5, r6, r7, r8, r9, pc}
   3ac40: e2850ee6     	add	r0, r5, #3680
   3ac44: e2800008     	add	r0, r0, #8
   3ac48: eb004af3     	bl	0x4d81c
   3ac4c: e5d03000     	ldrb	r3, [r0]
   3ac50: e3530000     	cmp	r3, #0
   3ac54: 0afffff4     	beq	0x3ac2c
   3ac58: e59530e8     	ldr	r3, [r5, #0xe8]
   3ac5c: e5901004     	ldr	r1, [r0, #0x4]
   3ac60: e5930014     	ldr	r0, [r3, #0x14]
   3ac64: e593201c     	ldr	r2, [r3, #0x1c]
   3ac68: e1500001     	cmp	r0, r1
   3ac6c: edd37a00     	vldr	s15, [r3]
   3ac70: a3a0e000     	movge	lr, #0
   3ac74: b3a0e001     	movlt	lr, #1
   3ac78: e1520001     	cmp	r2, r1
   3ac7c: b3a0c000     	movlt	r12, #0
   3ac80: a3a0c001     	movge	r12, #1
   3ac84: e1500002     	cmp	r0, r2
   3ac88: ba00002e     	blt	0x3ad48
   3ac8c: e19e000c     	orrs	r0, lr, r12
   3ac90: 0a00002e     	beq	0x3ad50
   3ac94: eef57a40     	vcmp.f32	s15, #0
   3ac98: e3a03001     	mov	r3, #1
   3ac9c: e5c5307c     	strb	r3, [r5, #0x7c]
   3aca0: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3aca4: a3e04000     	mvnge	r4, #0
   3aca8: e0844001     	add	r4, r4, r1
   3acac: e5854078     	str	r4, [r5, #0x78]
   3acb0: e1a01007     	mov	r1, r7
   3acb4: e28d0008     	add	r0, sp, #8
   3acb8: e302261c     	movw	r2, #0x261c
   3acbc: e3402007     	movt	r2, #0x7
   3acc0: ebffcfc0     	bl	0x2ebc8
   3acc4: e59520e8     	ldr	r2, [r5, #0xe8]
   3acc8: e3003d28     	movw	r3, #0xd28
   3accc: e3403007     	movt	r3, #0x7
   3acd0: e3061218     	movw	r1, #0x6218
   3acd4: e3401001     	movt	r1, #0x1
   3acd8: e5920054     	ldr	r0, [r2, #0x54]
   3acdc: e3a02010     	mov	r2, #16
   3ace0: e58d0000     	str	r0, [sp]
   3ace4: e28d0020     	add	r0, sp, #32
   3ace8: ebfff0ae     	bl	0x36fa8
   3acec: e28d2020     	add	r2, sp, #32
   3acf0: e28d1008     	add	r1, sp, #8
   3acf4: e1a00006     	mov	r0, r6
   3acf8: ebffcefb     	bl	0x2e8ec
   3acfc: e3090fec     	movw	r0, #0x9fec
   3ad00: e3400009     	movt	r0, #0x9
   3ad04: e1a01006     	mov	r1, r6
   3ad08: e3a02000     	mov	r2, #0
   3ad0c: eb00d483     	bl	0x6ff20
   3ad10: e59d0038     	ldr	r0, [sp, #0x38]
   3ad14: e1500008     	cmp	r0, r8
   3ad18: 0a000000     	beq	0x3ad20
   3ad1c: ebff6c47     	bl	0x15e40    @ imm = #-0x24ee4 ; _ZdlPv
   3ad20: e59d0020     	ldr	r0, [sp, #0x20]
   3ad24: e1500009     	cmp	r0, r9
   3ad28: 0a000000     	beq	0x3ad30
   3ad2c: ebff6c43     	bl	0x15e40    @ imm = #-0x24ef4 ; _ZdlPv
   3ad30: e59d0008     	ldr	r0, [sp, #0x8]
   3ad34: e28d3010     	add	r3, sp, #16
   3ad38: e1500003     	cmp	r0, r3
   3ad3c: 0affffba     	beq	0x3ac2c
   3ad40: ebff6c3e     	bl	0x15e40    @ imm = #-0x24f08 ; _ZdlPv
   3ad44: eaffffb8     	b	0x3ac2c
   3ad48: e11e000c     	tst	lr, r12
   3ad4c: 1affffd0     	bne	0x3ac94
   3ad50: eef57ac0     	vcmpe.f32	s15, #0
   3ad54: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3ad58: b5932018     	ldrlt	r2, [r3, #0x18]
   3ad5c: e5832054     	str	r2, [r3, #0x54]
   3ad60: e3a02001     	mov	r2, #1
   3ad64: e5c32058     	strb	r2, [r3, #0x58]
   3ad68: e2852078     	add	r2, r5, #120
   3ad6c: e1c305d4     	ldrd	r0, r1, [r3, #84]
   3ad70: e8860003     	stm	r6, {r0, r1}
   3ad74: e1c200f0     	strd	r0, r1, [r2]
   3ad78: eaffffcc     	b	0x3acb0
   3ad7c: e59d0038     	ldr	r0, [sp, #0x38]
   3ad80: e1500008     	cmp	r0, r8
   3ad84: 0a000000     	beq	0x3ad8c
   3ad88: ebff6c2c     	bl	0x15e40    @ imm = #-0x24f50 ; _ZdlPv
   3ad8c: e59d0020     	ldr	r0, [sp, #0x20]
   3ad90: e1500009     	cmp	r0, r9
   3ad94: 0a000000     	beq	0x3ad9c
   3ad98: ebff6c28     	bl	0x15e40    @ imm = #-0x24f60 ; _ZdlPv
   3ad9c: e59d0008     	ldr	r0, [sp, #0x8]
   3ada0: e28d3010     	add	r3, sp, #16
   3ada4: e1500003     	cmp	r0, r3
   3ada8: 0a000000     	beq	0x3adb0
   3adac: ebff6c23     	bl	0x15e40    @ imm = #-0x24f74 ; _ZdlPv
   3adb0: ebff6c6a     	bl	0x15f60    @ imm = #-0x24e58 ; __cxa_end_cleanup
   3adb4: eafffff4     	b	0x3ad8c
   3adb8: eafffff7     	b	0x3ad9c
   3adbc: e59d0020     	ldr	r0, [sp, #0x20]
   3adc0: e28d3028     	add	r3, sp, #40
   3adc4: e1500003     	cmp	r0, r3
   3adc8: 0a000000     	beq	0x3add0
   3adcc: ebff6c1b     	bl	0x15e40    @ imm = #-0x24f94 ; _ZdlPv
   3add0: ebff6c62     	bl	0x15f60    @ imm = #-0x24e78 ; __cxa_end_cleanup
   3add4: e59d0038     	ldr	r0, [sp, #0x38]
   3add8: e28d3040     	add	r3, sp, #64
   3addc: e1500003     	cmp	r0, r3
   3ade0: 0afffff5     	beq	0x3adbc
   3ade4: ebff6c15     	bl	0x15e40    @ imm = #-0x24fac ; _ZdlPv
   3ade8: eafffff3     	b	0x3adbc
