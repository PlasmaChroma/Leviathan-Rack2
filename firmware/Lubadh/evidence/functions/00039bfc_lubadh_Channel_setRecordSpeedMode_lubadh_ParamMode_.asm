; lubadh::Channel::setRecordSpeedMode(lubadh::ParamMode)
; VA 0x39bfc size 336

   39bfc: e59030e8     	ldr	r3, [r0, #0xe8]
   39c00: e59330a0     	ldr	r3, [r3, #0xa0]
   39c04: e1510003     	cmp	r1, r3
   39c08: 012fff1e     	bxeq	lr
   39c0c: e92d4030     	push	{r4, r5, lr}
   39c10: e1a05000     	mov	r5, r0
   39c14: e1a04001     	mov	r4, r1
   39c18: e24dd034     	sub	sp, sp, #52
   39c1c: e2851004     	add	r1, r5, #4
   39c20: e1a0000d     	mov	r0, sp
   39c24: e3022580     	movw	r2, #0x2580
   39c28: e3402007     	movt	r2, #0x7
   39c2c: ebffd3e5     	bl	0x2ebc8
   39c30: e3540001     	cmp	r4, #1
   39c34: e3023574     	movw	r3, #0x2574
   39c38: e3403007     	movt	r3, #0x7
   39c3c: e302156c     	movw	r1, #0x256c
   39c40: e3401007     	movt	r1, #0x7
   39c44: e1a0000d     	mov	r0, sp
   39c48: 11a01003     	movne	r1, r3
   39c4c: ebff71fe     	bl	0x1644c    @ imm = #-0x23808 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6appendEPKc
   39c50: e1a01000     	mov	r1, r0
   39c54: e28d0018     	add	r0, sp, #24
   39c58: ebff6fe8     	bl	0x15c00    @ imm = #-0x24060 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EOS4_
   39c5c: e3090fec     	movw	r0, #0x9fec
   39c60: e3400009     	movt	r0, #0x9
   39c64: e28d1018     	add	r1, sp, #24
   39c68: e3a02000     	mov	r2, #0
   39c6c: eb00d8ab     	bl	0x6ff20
   39c70: e59d0018     	ldr	r0, [sp, #0x18]
   39c74: e28d3020     	add	r3, sp, #32
   39c78: e1500003     	cmp	r0, r3
   39c7c: 0a000000     	beq	0x39c84
   39c80: ebff706e     	bl	0x15e40    @ imm = #-0x23e48 ; _ZdlPv
   39c84: e59d0000     	ldr	r0, [sp]
   39c88: e28d3008     	add	r3, sp, #8
   39c8c: e1500003     	cmp	r0, r3
   39c90: 0a000000     	beq	0x39c98
   39c94: ebff7069     	bl	0x15e40    @ imm = #-0x23e5c ; _ZdlPv
   39c98: e59530e8     	ldr	r3, [r5, #0xe8]
   39c9c: e3540000     	cmp	r4, #0
   39ca0: e58340a0     	str	r4, [r3, #0xa0]
   39ca4: 0a00000f     	beq	0x39ce8
   39ca8: e3540001     	cmp	r4, #1
   39cac: 1a00000b     	bne	0x39ce0
   39cb0: e2850ee6     	add	r0, r5, #3680
   39cb4: e2800008     	add	r0, r0, #8
   39cb8: eb004f31     	bl	0x4d984
   39cbc: e2404004     	sub	r4, r0, #4
   39cc0: e280504c     	add	r5, r0, #76
   39cc4: e5b40004     	ldr	r0, [r4, #0x4]!
   39cc8: e3500000     	cmp	r0, #0
   39ccc: 0a000001     	beq	0x39cd8
   39cd0: eeb70a00     	vmov.f32	s0, #1.000000e+00
   39cd4: eb004890     	bl	0x4bf1c
   39cd8: e1540005     	cmp	r4, r5
   39cdc: 1afffff8     	bne	0x39cc4
   39ce0: e28dd034     	add	sp, sp, #52
   39ce4: e8bd8030     	pop	{r4, r5, pc}
   39ce8: e2850ee6     	add	r0, r5, #3680
   39cec: e2800008     	add	r0, r0, #8
   39cf0: eb004f23     	bl	0x4d984
   39cf4: e2404004     	sub	r4, r0, #4
   39cf8: e280504c     	add	r5, r0, #76
   39cfc: e5b40004     	ldr	r0, [r4, #0x4]!
   39d00: e3500000     	cmp	r0, #0
   39d04: 0a000000     	beq	0x39d0c
   39d08: eb004887     	bl	0x4bf2c
   39d0c: e1540005     	cmp	r4, r5
   39d10: 1afffff9     	bne	0x39cfc
   39d14: e28dd034     	add	sp, sp, #52
   39d18: e8bd8030     	pop	{r4, r5, pc}
   39d1c: e59d0018     	ldr	r0, [sp, #0x18]
   39d20: e28d3020     	add	r3, sp, #32
   39d24: e1500003     	cmp	r0, r3
   39d28: 0a000000     	beq	0x39d30
   39d2c: ebff7043     	bl	0x15e40    @ imm = #-0x23ef4 ; _ZdlPv
   39d30: e59d0000     	ldr	r0, [sp]
   39d34: e28d3008     	add	r3, sp, #8
   39d38: e1500003     	cmp	r0, r3
   39d3c: 0a000000     	beq	0x39d44
   39d40: ebff703e     	bl	0x15e40    @ imm = #-0x23f08 ; _ZdlPv
   39d44: ebff7085     	bl	0x15f60    @ imm = #-0x23dec ; __cxa_end_cleanup
   39d48: eafffff8     	b	0x39d30
