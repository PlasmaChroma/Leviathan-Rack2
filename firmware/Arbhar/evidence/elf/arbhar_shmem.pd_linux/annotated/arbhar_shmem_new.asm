00001910 <arbhar_shmem_new>:
    1910: e92d4070     	push	{r4, r5, r6, lr}
    1914: e59f519c     	ldr	r5, [pc, #0x19c]        @ 0x1ab8 <arbhar_shmem_new+0x1a8>  // u32=0x116d8; f32?=1.0003029e-40
    1918: ed2d8b02     	vpush	{d8}
    191c: e59f3198     	ldr	r3, [pc, #0x198]        @ 0x1abc <arbhar_shmem_new+0x1ac>  // u32=0xc8; f32?=2.80259693e-43
    1920: e08f5005     	add	r5, pc, r5
    1924: ed9f8a54     	vldr	s16, [pc, #336]         @ 0x1a7c <arbhar_shmem_new+0x16c>  // f32=624000
    1928: e7950003     	ldr	r0, [r5, r3]
    192c: e5900000     	ldr	r0, [r0]
    1930: ebfffc3d     	bl	0xa2c <.plt+0x38>       @ imm = #-0xf0c  // CALL pd_new
    1934: e59f1184     	ldr	r1, [pc, #0x184]        @ 0x1ac0 <arbhar_shmem_new+0x1b0>  // u32=0xb0; f32?=2.4662853e-43
    1938: e7951001     	ldr	r1, [r5, r1]
    193c: e1a04000     	mov	r4, r0
    1940: ebfffc60     	bl	0xac8 <.plt+0xd4>       @ imm = #-0xe80  // CALL clock_new
    1944: e3e02000     	mvn	r2, #0
    1948: eddf0a4c     	vldr	s1, [pc, #304]          @ 0x1a80 <arbhar_shmem_new+0x170>  // f32=250
    194c: e584201c     	str	r2, [r4, #0x1c]
    1950: e3a01000     	mov	r1, #0
    1954: ed9f0a4a     	vldr	s0, [pc, #296]          @ 0x1a84 <arbhar_shmem_new+0x174>  // f32=61019
    1958: e5840050     	str	r0, [r4, #0x50]
    195c: e1a00004     	mov	r0, r4
    1960: ebfffc61     	bl	0xaec <.plt+0xf8>       @ imm = #-0xe7c  // CALL _memAllocate
    1964: eef00a48     	vmov.f32	s1, s16
    1968: e1a00004     	mov	r0, r4
    196c: e3a01001     	mov	r1, #1
    1970: ed9f0a44     	vldr	s0, [pc, #272]          @ 0x1a88 <arbhar_shmem_new+0x178>  // f32=90521
    1974: ebfffc5c     	bl	0xaec <.plt+0xf8>       @ imm = #-0xe90  // CALL _memAllocate
    1978: e1a00004     	mov	r0, r4
    197c: e3a01002     	mov	r1, #2
    1980: eef00a48     	vmov.f32	s1, s16
    1984: ed9f0a40     	vldr	s0, [pc, #256]          @ 0x1a8c <arbhar_shmem_new+0x17c>  // f32=90522
    1988: ebfffc57     	bl	0xaec <.plt+0xf8>       @ imm = #-0xea4  // CALL _memAllocate
    198c: e1a00004     	mov	r0, r4
    1990: e3a01003     	mov	r1, #3
    1994: eef00a48     	vmov.f32	s1, s16
    1998: ed9f0a3c     	vldr	s0, [pc, #240]          @ 0x1a90 <arbhar_shmem_new+0x180>  // f32=90523
    199c: ebfffc52     	bl	0xaec <.plt+0xf8>       @ imm = #-0xeb8  // CALL _memAllocate
    19a0: e1a00004     	mov	r0, r4
    19a4: e3a01004     	mov	r1, #4
    19a8: eef00a48     	vmov.f32	s1, s16
    19ac: ed9f0a38     	vldr	s0, [pc, #224]          @ 0x1a94 <arbhar_shmem_new+0x184>  // f32=90524
    19b0: ebfffc4d     	bl	0xaec <.plt+0xf8>       @ imm = #-0xecc  // CALL _memAllocate
    19b4: e1a00004     	mov	r0, r4
    19b8: e3a01005     	mov	r1, #5
    19bc: eef00a48     	vmov.f32	s1, s16
    19c0: ed9f0a34     	vldr	s0, [pc, #208]          @ 0x1a98 <arbhar_shmem_new+0x188>  // f32=90525
    19c4: ebfffc48     	bl	0xaec <.plt+0xf8>       @ imm = #-0xee0  // CALL _memAllocate
    19c8: e1a00004     	mov	r0, r4
    19cc: e3a01006     	mov	r1, #6
    19d0: eef00a48     	vmov.f32	s1, s16
    19d4: ed9f0a30     	vldr	s0, [pc, #192]          @ 0x1a9c <arbhar_shmem_new+0x18c>  // f32=90526
    19d8: ebfffc43     	bl	0xaec <.plt+0xf8>       @ imm = #-0xef4  // CALL _memAllocate
    19dc: e1a00004     	mov	r0, r4
    19e0: e3a01007     	mov	r1, #7
    19e4: eef00a48     	vmov.f32	s1, s16
    19e8: ed9f0a2c     	vldr	s0, [pc, #176]          @ 0x1aa0 <arbhar_shmem_new+0x190>  // f32=90527
    19ec: ebfffc3e     	bl	0xaec <.plt+0xf8>       @ imm = #-0xf08  // CALL _memAllocate
    19f0: e1a00004     	mov	r0, r4
    19f4: e3a01008     	mov	r1, #8
    19f8: eef00a48     	vmov.f32	s1, s16
    19fc: ed9f0a28     	vldr	s0, [pc, #160]          @ 0x1aa4 <arbhar_shmem_new+0x194>  // f32=90528
    1a00: ebfffc39     	bl	0xaec <.plt+0xf8>       @ imm = #-0xf1c  // CALL _memAllocate
    1a04: e1a00004     	mov	r0, r4
    1a08: e3a01009     	mov	r1, #9
    1a0c: eef00a48     	vmov.f32	s1, s16
    1a10: ed9f0a24     	vldr	s0, [pc, #144]          @ 0x1aa8 <arbhar_shmem_new+0x198>  // f32=90529
    1a14: ebfffc34     	bl	0xaec <.plt+0xf8>       @ imm = #-0xf30  // CALL _memAllocate
    1a18: e1a00004     	mov	r0, r4
    1a1c: e3a0100a     	mov	r1, #10
    1a20: eef00a48     	vmov.f32	s1, s16
    1a24: ed9f0a20     	vldr	s0, [pc, #128]          @ 0x1aac <arbhar_shmem_new+0x19c>  // f32=90530
    1a28: ebfffc2f     	bl	0xaec <.plt+0xf8>       @ imm = #-0xf44  // CALL _memAllocate
    1a2c: e1a00004     	mov	r0, r4
    1a30: e3a0100b     	mov	r1, #11
    1a34: eef00a48     	vmov.f32	s1, s16
    1a38: ed9f0a1c     	vldr	s0, [pc, #112]          @ 0x1ab0 <arbhar_shmem_new+0x1a0>  // f32=90531
    1a3c: ebfffc2a     	bl	0xaec <.plt+0xf8>       @ imm = #-0xf58  // CALL _memAllocate
    1a40: e1a00004     	mov	r0, r4
    1a44: e3a0100c     	mov	r1, #12
    1a48: eef00a48     	vmov.f32	s1, s16
    1a4c: ed9f0a18     	vldr	s0, [pc, #96]           @ 0x1ab4 <arbhar_shmem_new+0x1a4>  // f32=90532
    1a50: ebfffc25     	bl	0xaec <.plt+0xf8>       @ imm = #-0xf6c  // CALL _memAllocate
    1a54: e3a01000     	mov	r1, #0
    1a58: e1a00004     	mov	r0, r4
    1a5c: ebfffc34     	bl	0xb34 <.plt+0x140>      @ imm = #-0xf30  // CALL outlet_new
    1a60: e1a00004     	mov	r0, r4
    1a64: e3a01000     	mov	r1, #0
    1a68: ebfffc31     	bl	0xb34 <.plt+0x140>      @ imm = #-0xf3c  // CALL outlet_new
    1a6c: ecbd8b02     	vpop	{d8}
    1a70: e5840054     	str	r0, [r4, #0x54]
    1a74: e1a00004     	mov	r0, r4
    1a78: e8bd8070     	pop	{r4, r5, r6, pc}
    1a7c: 00 58 18 49  	.word	0x49185800
    1a80: 00 00 7a 43  	.word	0x437a0000
    1a84: 00 5b 6e 47  	.word	0x476e5b00
    1a88: 80 cc b0 47  	.word	0x47b0cc80
    1a8c: 00 cd b0 47  	.word	0x47b0cd00
    1a90: 80 cd b0 47  	.word	0x47b0cd80
    1a94: 00 ce b0 47  	.word	0x47b0ce00
    1a98: 80 ce b0 47  	.word	0x47b0ce80
    1a9c: 00 cf b0 47  	.word	0x47b0cf00
    1aa0: 80 cf b0 47  	.word	0x47b0cf80
    1aa4: 00 d0 b0 47  	.word	0x47b0d000
    1aa8: 80 d0 b0 47  	.word	0x47b0d080
    1aac: 00 d1 b0 47  	.word	0x47b0d100
    1ab0: 80 d1 b0 47  	.word	0x47b0d180
    1ab4: 00 d2 b0 47  	.word	0x47b0d200
    1ab8: d8 16 01 00  	.word	0x000116d8
    1abc: c8 00 00 00  	.word	0x000000c8
    1ac0: b0 00 00 00  	.word	0x000000b0

