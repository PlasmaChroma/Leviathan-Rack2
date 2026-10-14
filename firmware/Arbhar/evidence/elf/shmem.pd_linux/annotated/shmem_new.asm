00000c2c <shmem_new>:
     c2c: e92d4070     	push	{r4, r5, r6, lr}
     c30: e3a05000     	mov	r5, #0
     c34: ed2d8b02     	vpush	{d8}
     c38: e59f305c     	ldr	r3, [pc, #0x5c]         @ 0xc9c <shmem_new+0x70>  // u32=0x113b8; f32?=9.89092508e-41
     c3c: e59f205c     	ldr	r2, [pc, #0x5c]         @ 0xca0 <shmem_new+0x74>  // u32=0x84; f32?=1.84971397e-43
     c40: e08f3003     	add	r3, pc, r3
     c44: eef08a40     	vmov.f32	s17, s0
     c48: e7930002     	ldr	r0, [r3, r2]
     c4c: eeb08a60     	vmov.f32	s16, s1
     c50: e5900000     	ldr	r0, [r0]
     c54: ebfffed0     	bl	0x79c <.plt+0x2c>       @ imm = #-0x4c0  // CALL pd_new
     c58: e3e01000     	mvn	r1, #0
     c5c: e1a04000     	mov	r4, r0
     c60: e580101c     	str	r1, [r0, #0x1c]
     c64: e5805024     	str	r5, [r0, #0x24]
     c68: e1a01005     	mov	r1, r5
     c6c: ebfffef1     	bl	0x838 <.plt+0xc8>       @ imm = #-0x43c  // CALL outlet_new
     c70: e1a01005     	mov	r1, r5
     c74: e1a00004     	mov	r0, r4
     c78: ebfffeee     	bl	0x838 <.plt+0xc8>       @ imm = #-0x448  // CALL outlet_new
     c7c: eeb00a68     	vmov.f32	s0, s17
     c80: eef00a48     	vmov.f32	s1, s16
     c84: e5840020     	str	r0, [r4, #0x20]
     c88: e1a00004     	mov	r0, r4
     c8c: ebfffee6     	bl	0x82c <.plt+0xbc>       @ imm = #-0x468  // CALL shmem_allocate
     c90: ecbd8b02     	vpop	{d8}
     c94: e1a00004     	mov	r0, r4
     c98: e8bd8070     	pop	{r4, r5, r6, pc}
     c9c: b8 13 01 00  	.word	0x000113b8
     ca0: 84 00 00 00  	.word	0x00000084

