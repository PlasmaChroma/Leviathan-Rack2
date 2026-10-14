000014cc <arbhar_shmem_read>:
    14cc: eebd0ac0     	vcvt.s32.f32	s0, s0
    14d0: ee103a10     	vmov	r3, s0
    14d4: e0802103     	add	r2, r0, r3, lsl #2
    14d8: e5921058     	ldr	r1, [r2, #0x58]
    14dc: e3510000     	cmp	r1, #0
    14e0: 012fff1e     	bxeq	lr
    14e4: eefd0ae0     	vcvt.s32.f32	s1, s1
    14e8: e592c08c     	ldr	r12, [r2, #0x8c]
    14ec: e590000c     	ldr	r0, [r0, #0xc]
    14f0: ee103a90     	vmov	r3, s1
    14f4: e1c32fc3     	bic	r2, r3, r3, asr #31
    14f8: e15c0002     	cmp	r12, r2
    14fc: d24c2001     	suble	r2, r12, #1
    1500: e0811102     	add	r1, r1, r2, lsl #2
    1504: ed910a00     	vldr	s0, [r1]
    1508: eafffd92     	b	0xb58 <.plt+0x164>      @ imm = #-0x9b8

