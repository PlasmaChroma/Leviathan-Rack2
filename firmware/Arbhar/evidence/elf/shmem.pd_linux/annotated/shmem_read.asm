00000a0c <shmem_read>:
     a0c: e5902024     	ldr	r2, [r0, #0x24]
     a10: e3520000     	cmp	r2, #0
     a14: 012fff1e     	bxeq	lr
     a18: eebd0ac0     	vcvt.s32.f32	s0, s0
     a1c: e5901028     	ldr	r1, [r0, #0x28]
     a20: e590000c     	ldr	r0, [r0, #0xc]
     a24: ee103a10     	vmov	r3, s0
     a28: e1c3cfc3     	bic	r12, r3, r3, asr #31
     a2c: e151000c     	cmp	r1, r12
     a30: d241c001     	suble	r12, r1, #1
     a34: e082210c     	add	r2, r2, r12, lsl #2
     a38: ed920a00     	vldr	s0, [r2]
     a3c: eaffff86     	b	0x85c <.plt+0xec>       @ imm = #-0x1e8  // CALL outlet_float

