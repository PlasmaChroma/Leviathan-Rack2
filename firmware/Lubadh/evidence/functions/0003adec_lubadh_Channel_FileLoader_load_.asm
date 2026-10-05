; lubadh::Channel::FileLoader::load()
; VA 0x3adec size 568

   3adec: e92d4010     	push	{r4, lr}
   3adf0: e1a04000     	mov	r4, r0
   3adf4: e30226f8     	movw	r2, #0x26f8
   3adf8: e3402007     	movt	r2, #0x7
   3adfc: e24dd098     	sub	sp, sp, #152
   3ae00: e5941000     	ldr	r1, [r4]
   3ae04: e28d0008     	add	r0, sp, #8
   3ae08: e2811004     	add	r1, r1, #4
   3ae0c: ebffcf6d     	bl	0x2ebc8
   3ae10: e5943070     	ldr	r3, [r4, #0x70]
   3ae14: e58d3000     	str	r3, [sp]
   3ae18: e3061218     	movw	r1, #0x6218
   3ae1c: e3401001     	movt	r1, #0x1
   3ae20: e3003d28     	movw	r3, #0xd28
   3ae24: e3403007     	movt	r3, #0x7
   3ae28: e28d0020     	add	r0, sp, #32
   3ae2c: e3a02010     	mov	r2, #16
   3ae30: ebfff05c     	bl	0x36fa8
   3ae34: e28d2020     	add	r2, sp, #32
   3ae38: e28d1008     	add	r1, sp, #8
   3ae3c: e28d0038     	add	r0, sp, #56
   3ae40: ebffcea9     	bl	0x2e8ec
   3ae44: e3021718     	movw	r1, #0x2718
   3ae48: e3401007     	movt	r1, #0x7
   3ae4c: e28d0038     	add	r0, sp, #56
   3ae50: ebff6d7d     	bl	0x1644c    @ imm = #-0x24a0c ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6appendEPKc
   3ae54: e1a01000     	mov	r1, r0
   3ae58: e28d0050     	add	r0, sp, #80
   3ae5c: ebff6b67     	bl	0x15c00    @ imm = #-0x25264 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EOS4_
   3ae60: e594306c     	ldr	r3, [r4, #0x6c]
   3ae64: e58d3000     	str	r3, [sp]
   3ae68: e3061218     	movw	r1, #0x6218
   3ae6c: e3401001     	movt	r1, #0x1
   3ae70: e3003d28     	movw	r3, #0xd28
   3ae74: e3403007     	movt	r3, #0x7
   3ae78: e28d0068     	add	r0, sp, #104
   3ae7c: e3a02010     	mov	r2, #16
   3ae80: ebfff048     	bl	0x36fa8
   3ae84: e28d2068     	add	r2, sp, #104
   3ae88: e28d1050     	add	r1, sp, #80
   3ae8c: e28d0080     	add	r0, sp, #128
   3ae90: ebffce95     	bl	0x2e8ec
   3ae94: e3090fec     	movw	r0, #0x9fec
   3ae98: e3400009     	movt	r0, #0x9
   3ae9c: e28d1080     	add	r1, sp, #128
   3aea0: e3a02000     	mov	r2, #0
   3aea4: eb00d41d     	bl	0x6ff20
   3aea8: e59d0080     	ldr	r0, [sp, #0x80]
   3aeac: e28d3088     	add	r3, sp, #136
   3aeb0: e1500003     	cmp	r0, r3
   3aeb4: 0a000000     	beq	0x3aebc
   3aeb8: ebff6be0     	bl	0x15e40    @ imm = #-0x25080 ; _ZdlPv
   3aebc: e59d0068     	ldr	r0, [sp, #0x68]
   3aec0: e28d3070     	add	r3, sp, #112
   3aec4: e1500003     	cmp	r0, r3
   3aec8: 0a000000     	beq	0x3aed0
   3aecc: ebff6bdb     	bl	0x15e40    @ imm = #-0x25094 ; _ZdlPv
   3aed0: e59d0050     	ldr	r0, [sp, #0x50]
   3aed4: e28d3058     	add	r3, sp, #88
   3aed8: e1500003     	cmp	r0, r3
   3aedc: 0a000000     	beq	0x3aee4
   3aee0: ebff6bd6     	bl	0x15e40    @ imm = #-0x250a8 ; _ZdlPv
   3aee4: e59d0038     	ldr	r0, [sp, #0x38]
   3aee8: e28d3040     	add	r3, sp, #64
   3aeec: e1500003     	cmp	r0, r3
   3aef0: 0a000000     	beq	0x3aef8
   3aef4: ebff6bd1     	bl	0x15e40    @ imm = #-0x250bc ; _ZdlPv
   3aef8: e59d0020     	ldr	r0, [sp, #0x20]
   3aefc: e28d3028     	add	r3, sp, #40
   3af00: e1500003     	cmp	r0, r3
   3af04: 0a000000     	beq	0x3af0c
   3af08: ebff6bcc     	bl	0x15e40    @ imm = #-0x250d0 ; _ZdlPv
   3af0c: e59d0008     	ldr	r0, [sp, #0x8]
   3af10: e28d3010     	add	r3, sp, #16
   3af14: e1500003     	cmp	r0, r3
   3af18: 0a000000     	beq	0x3af20
   3af1c: ebff6bc7     	bl	0x15e40    @ imm = #-0x250e4 ; _ZdlPv
   3af20: e5940000     	ldr	r0, [r4]
   3af24: ebfff57b     	bl	0x38518
   3af28: e5942000     	ldr	r2, [r4]
   3af2c: e5923020     	ldr	r3, [r2, #0x20]
   3af30: e2833915     	add	r3, r3, #344064
   3af34: e5d31abc     	ldrb	r1, [r3, #0xabc]
   3af38: e3510000     	cmp	r1, #0
   3af3c: 1a00000e     	bne	0x3af7c
   3af40: e5920000     	ldr	r0, [r2]
   3af44: e3a0e001     	mov	lr, #1
   3af48: e592101c     	ldr	r1, [r2, #0x1c]
   3af4c: e594c09c     	ldr	r12, [r4, #0x9c]
   3af50: e5942078     	ldr	r2, [r4, #0x78]
   3af54: e7cce000     	strb	lr, [r12, r0]
   3af58: e591e000     	ldr	lr, [r1]
   3af5c: e5d33abc     	ldrb	r3, [r3, #0xabc]
   3af60: e1c406dc     	ldrd	r0, r1, [r4, #108]
   3af64: e7cc300e     	strb	r3, [r12, lr]
   3af68: e8820003     	stm	r2, {r0, r1}
   3af6c: e28400bc     	add	r0, r4, #188
   3af70: eb00d62a     	bl	0x70820
   3af74: e28dd098     	add	sp, sp, #152
   3af78: e8bd8010     	pop	{r4, pc}
   3af7c: e592001c     	ldr	r0, [r2, #0x1c]
   3af80: ebfff564     	bl	0x38518
   3af84: e5942000     	ldr	r2, [r4]
   3af88: e5923020     	ldr	r3, [r2, #0x20]
   3af8c: e2833915     	add	r3, r3, #344064
   3af90: eaffffea     	b	0x3af40
   3af94: e59d0080     	ldr	r0, [sp, #0x80]
   3af98: e28d3088     	add	r3, sp, #136
   3af9c: e1500003     	cmp	r0, r3
   3afa0: 0a000000     	beq	0x3afa8
   3afa4: ebff6ba5     	bl	0x15e40    @ imm = #-0x2516c ; _ZdlPv
   3afa8: e59d0068     	ldr	r0, [sp, #0x68]
   3afac: e28d3070     	add	r3, sp, #112
   3afb0: e1500003     	cmp	r0, r3
   3afb4: 0a000000     	beq	0x3afbc
   3afb8: ebff6ba0     	bl	0x15e40    @ imm = #-0x25180 ; _ZdlPv
   3afbc: e59d0050     	ldr	r0, [sp, #0x50]
   3afc0: e28d3058     	add	r3, sp, #88
   3afc4: e1500003     	cmp	r0, r3
   3afc8: 0a000000     	beq	0x3afd0
   3afcc: ebff6b9b     	bl	0x15e40    @ imm = #-0x25194 ; _ZdlPv
   3afd0: e59d0038     	ldr	r0, [sp, #0x38]
   3afd4: e28d3040     	add	r3, sp, #64
   3afd8: e1500003     	cmp	r0, r3
   3afdc: 0a000000     	beq	0x3afe4
   3afe0: ebff6b96     	bl	0x15e40    @ imm = #-0x251a8 ; _ZdlPv
   3afe4: e59d0020     	ldr	r0, [sp, #0x20]
   3afe8: e28d3028     	add	r3, sp, #40
   3afec: e1500003     	cmp	r0, r3
   3aff0: 0a000000     	beq	0x3aff8
   3aff4: ebff6b91     	bl	0x15e40    @ imm = #-0x251bc ; _ZdlPv
   3aff8: e59d0008     	ldr	r0, [sp, #0x8]
   3affc: e28d3010     	add	r3, sp, #16
   3b000: e1500003     	cmp	r0, r3
   3b004: 0a000000     	beq	0x3b00c
   3b008: ebff6b8c     	bl	0x15e40    @ imm = #-0x251d0 ; _ZdlPv
   3b00c: ebff6bd3     	bl	0x15f60    @ imm = #-0x250b4 ; __cxa_end_cleanup
   3b010: eaffffe4     	b	0x3afa8
   3b014: eaffffe8     	b	0x3afbc
   3b018: eaffffec     	b	0x3afd0
   3b01c: eafffff0     	b	0x3afe4
   3b020: eafffff4     	b	0x3aff8
