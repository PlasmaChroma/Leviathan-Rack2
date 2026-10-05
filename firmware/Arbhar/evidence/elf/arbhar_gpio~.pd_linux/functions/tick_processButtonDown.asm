0000bcfc <tick_processButtonDown>:
    bcfc: e92d47f0     	push	{r4, r5, r6, r7, r8, r9, r10, lr}
    bd00: e2805d75     	add	r5, r0, #7488
    bd04: e5d03030     	ldrb	r3, [r0, #0x30]
    bd08: e285501c     	add	r5, r5, #28
    bd0c: e1a04000     	mov	r4, r0
    bd10: e24dd030     	sub	sp, sp, #48
    bd14: e3530062     	cmp	r3, #98
    bd18: e2856004     	add	r6, r5, #4
    bd1c: e1a00005     	mov	r0, r5
    bd20: 8a000045     	bhi	0xbe3c <tick_processButtonDown+0x140> @ imm = #0x114
    bd24: ebffdff6     	bl	0x3d04 <.plt+0x608>     @ imm = #-0x8028
    bd28: e2847a01     	add	r7, r4, #4096
    bd2c: e1a00005     	mov	r0, r5
    bd30: ebffdf7b     	bl	0x3b24 <.plt+0x428>     @ imm = #-0x8214
    bd34: e1a00005     	mov	r0, r5
    bd38: ebffdec5     	bl	0x3854 <.plt+0x158>     @ imm = #-0x84ec
    bd3c: e5d70d5c     	ldrb	r0, [r7, #0xd5c]
    bd40: e3500004     	cmp	r0, #4
    bd44: 0a00005e     	beq	0xbec4 <tick_processButtonDown+0x1c8> @ imm = #0x178
    bd48: e3500003     	cmp	r0, #3
    bd4c: 8a000063     	bhi	0xbee0 <tick_processButtonDown+0x1e4> @ imm = #0x18c
    bd50: e1a00006     	mov	r0, r6
    bd54: ebffdefd     	bl	0x3950 <.plt+0x254>     @ imm = #-0x840c
    bd58: e3500001     	cmp	r0, #1
    bd5c: 0a00021b     	beq	0xc5d0 <tick_processButtonDown+0x8d4> @ imm = #0x86c
    bd60: e5d7ad5c     	ldrb	r10, [r7, #0xd5c]
    bd64: e35a0003     	cmp	r10, #3
    bd68: 8a000063     	bhi	0xbefc <tick_processButtonDown+0x200> @ imm = #0x18c
    bd6c: e1a00006     	mov	r0, r6
    bd70: ebffdef6     	bl	0x3950 <.plt+0x254>     @ imm = #-0x8428
    bd74: e3500001     	cmp	r0, #1
    bd78: 0a00025d     	beq	0xc6f4 <tick_processButtonDown+0x9f8> @ imm = #0x974
    bd7c: e5d7cd5c     	ldrb	r12, [r7, #0xd5c]
    bd80: e35c0003     	cmp	r12, #3
    bd84: 8a00005c     	bhi	0xbefc <tick_processButtonDown+0x200> @ imm = #0x170
    bd88: e1a00006     	mov	r0, r6
    bd8c: ebffdeef     	bl	0x3950 <.plt+0x254>     @ imm = #-0x8444
    bd90: e3500001     	cmp	r0, #1
    bd94: e1a09000     	mov	r9, r0
    bd98: 0a000275     	beq	0xc774 <tick_processButtonDown+0xa78> @ imm = #0x9d4
    bd9c: e5d71d5c     	ldrb	r1, [r7, #0xd5c]
    bda0: e3510003     	cmp	r1, #3
    bda4: 8a000054     	bhi	0xbefc <tick_processButtonDown+0x200> @ imm = #0x150
    bda8: e1a00006     	mov	r0, r6
    bdac: ebffdee7     	bl	0x3950 <.plt+0x254>     @ imm = #-0x8464
    bdb0: e3500001     	cmp	r0, #1
    bdb4: e1a08000     	mov	r8, r0
    bdb8: 0a00028e     	beq	0xc7f8 <tick_processButtonDown+0xafc> @ imm = #0xa38
    bdbc: e5d70d5c     	ldrb	r0, [r7, #0xd5c]
    bdc0: e3500003     	cmp	r0, #3
    bdc4: 8a00004c     	bhi	0xbefc <tick_processButtonDown+0x200> @ imm = #0x130
    bdc8: e1a00006     	mov	r0, r6
    bdcc: ebffdedf     	bl	0x3950 <.plt+0x254>     @ imm = #-0x8484
    bdd0: e3500001     	cmp	r0, #1
    bdd4: 0a0002bb     	beq	0xc8c8 <tick_processButtonDown+0xbcc> @ imm = #0xaec
    bdd8: e5d7ad5c     	ldrb	r10, [r7, #0xd5c]
    bddc: e35a0003     	cmp	r10, #3
    bde0: 8a000045     	bhi	0xbefc <tick_processButtonDown+0x200> @ imm = #0x114
    bde4: e1a00006     	mov	r0, r6
    bde8: ebffded8     	bl	0x3950 <.plt+0x254>     @ imm = #-0x84a0
    bdec: e3500001     	cmp	r0, #1
    bdf0: e1a09000     	mov	r9, r0
    bdf4: 0a0002c5     	beq	0xc910 <tick_processButtonDown+0xc14> @ imm = #0xb14
    bdf8: e5d7cd5c     	ldrb	r12, [r7, #0xd5c]
    bdfc: e35c0003     	cmp	r12, #3
    be00: 8a00003d     	bhi	0xbefc <tick_processButtonDown+0x200> @ imm = #0xf4
    be04: e1a00006     	mov	r0, r6
    be08: ebffded0     	bl	0x3950 <.plt+0x254>     @ imm = #-0x84c0
    be0c: e2508000     	subs	r8, r0, #0
    be10: 0a0000f8     	beq	0xc1f8 <tick_processButtonDown+0x4fc> @ imm = #0x3e0
    be14: e5d72d5c     	ldrb	r2, [r7, #0xd5c]
    be18: e3520000     	cmp	r2, #0
    be1c: 0a0000b8     	beq	0xc104 <tick_processButtonDown+0x408> @ imm = #0x2e0
    be20: e3520003     	cmp	r2, #3
    be24: 8a000034     	bhi	0xbefc <tick_processButtonDown+0x200> @ imm = #0xd0
    be28: e5d4103c     	ldrb	r1, [r4, #0x3c]
    be2c: e1a00004     	mov	r0, r4
    be30: ebffdede     	bl	0x39b0 <.plt+0x2b4>     @ imm = #-0x8488
    be34: e28dd030     	add	sp, sp, #48
    be38: e8bd87f0     	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
    be3c: ebffdfb0     	bl	0x3d04 <.plt+0x608>     @ imm = #-0x8140
    be40: e1a00005     	mov	r0, r5
    be44: ebffdf36     	bl	0x3b24 <.plt+0x428>     @ imm = #-0x8328
    be48: e1a00005     	mov	r0, r5
    be4c: ebffde80     	bl	0x3854 <.plt+0x158>     @ imm = #-0x8600
    be50: e5d41030     	ldrb	r1, [r4, #0x30]
    be54: e2857014     	add	r7, r5, #20
    be58: e2848d76     	add	r8, r4, #7552
    be5c: e3510064     	cmp	r1, #100
    be60: 0a0000ad     	beq	0xc11c <tick_processButtonDown+0x420> @ imm = #0x2b4
    be64: e59fc578     	ldr	r12, [pc, #0x578]       @ 0xc3e4 <tick_processButtonDown+0x6e8>
    be68: e2844a01     	add	r4, r4, #4096
    be6c: e08f000c     	add	r0, pc, r12
    be70: ebffdf40     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x8300
    be74: e5d40d5c     	ldrb	r0, [r4, #0xd5c]
    be78: ebffdf38     	bl	0x3b60 <.plt+0x464>     @ imm = #-0x8320
    be7c: e1a05000     	mov	r5, r0
    be80: e1a00006     	mov	r0, r6
    be84: ebffdeb1     	bl	0x3950 <.plt+0x254>     @ imm = #-0x853c
    be88: e1a06000     	mov	r6, r0
    be8c: e1a00007     	mov	r0, r7
    be90: ebffdeae     	bl	0x3950 <.plt+0x254>     @ imm = #-0x8548
    be94: e1a07000     	mov	r7, r0
    be98: e1a00008     	mov	r0, r8
    be9c: ebffdeab     	bl	0x3950 <.plt+0x254>     @ imm = #-0x8554
    bea0: e59f8540     	ldr	r8, [pc, #0x540]        @ 0xc3e8 <tick_processButtonDown+0x6ec>
    bea4: e1a03007     	mov	r3, r7
    bea8: e1a02006     	mov	r2, r6
    beac: e1a01005     	mov	r1, r5
    beb0: e58d0000     	str	r0, [sp]
    beb4: e08f0008     	add	r0, pc, r8
    beb8: ebffdf2e     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x8348
    bebc: e28dd030     	add	sp, sp, #48
    bec0: e8bd87f0     	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
    bec4: e1a00006     	mov	r0, r6
    bec8: ebffdea0     	bl	0x3950 <.plt+0x254>     @ imm = #-0x8580
    becc: e3500000     	cmp	r0, #0
    bed0: 0a000168     	beq	0xc478 <tick_processButtonDown+0x77c> @ imm = #0x5a0
    bed4: e5d70d5c     	ldrb	r0, [r7, #0xd5c]
    bed8: e3500003     	cmp	r0, #3
    bedc: 9affff9b     	bls	0xbd50 <tick_processButtonDown+0x54> @ imm = #-0x194
    bee0: e1a00006     	mov	r0, r6
    bee4: ebffde99     	bl	0x3950 <.plt+0x254>     @ imm = #-0x859c
    bee8: e3500001     	cmp	r0, #1
    beec: 0a00011b     	beq	0xc360 <tick_processButtonDown+0x664> @ imm = #0x46c
    bef0: e5d7ed5c     	ldrb	lr, [r7, #0xd5c]
    bef4: e35e0003     	cmp	lr, #3
    bef8: 9affff94     	bls	0xbd50 <tick_processButtonDown+0x54> @ imm = #-0x1b0
    befc: e1a00006     	mov	r0, r6
    bf00: ebffde92     	bl	0x3950 <.plt+0x254>     @ imm = #-0x85b8
    bf04: e3500001     	cmp	r0, #1
    bf08: 0a0002b1     	beq	0xc9d4 <tick_processButtonDown+0xcd8> @ imm = #0xac4
    bf0c: e5d71d5c     	ldrb	r1, [r7, #0xd5c]
    bf10: e3510003     	cmp	r1, #3
    bf14: 9affffc3     	bls	0xbe28 <tick_processButtonDown+0x12c> @ imm = #-0xf4
    bf18: e1a00006     	mov	r0, r6
    bf1c: ebffde8b     	bl	0x3950 <.plt+0x254>     @ imm = #-0x85d4
    bf20: e3500001     	cmp	r0, #1
    bf24: 8affffbf     	bhi	0xbe28 <tick_processButtonDown+0x12c> @ imm = #-0x104
    bf28: e2855014     	add	r5, r5, #20
    bf2c: e1a00005     	mov	r0, r5
    bf30: ebffde86     	bl	0x3950 <.plt+0x254>     @ imm = #-0x85e8
    bf34: e3500001     	cmp	r0, #1
    bf38: 8affffba     	bhi	0xbe28 <tick_processButtonDown+0x12c> @ imm = #-0x118
    bf3c: e2848d76     	add	r8, r4, #7552
    bf40: e1a00008     	mov	r0, r8
    bf44: ebffde81     	bl	0x3950 <.plt+0x254>     @ imm = #-0x85fc
    bf48: e3500001     	cmp	r0, #1
    bf4c: 8affffb5     	bhi	0xbe28 <tick_processButtonDown+0x12c> @ imm = #-0x12c
    bf50: e1a00005     	mov	r0, r5
    bf54: ebffde5f     	bl	0x38d8 <.plt+0x1dc>     @ imm = #-0x8684
    bf58: e3500000     	cmp	r0, #0
    bf5c: 0a000005     	beq	0xbf78 <tick_processButtonDown+0x27c> @ imm = #0x14
    bf60: e5d4306a     	ldrb	r3, [r4, #0x6a]
    bf64: e2430001     	sub	r0, r3, #1
    bf68: e6efe070     	uxtb	lr, r0
    bf6c: e35e00ff     	cmp	lr, #255
    bf70: 03a0e002     	moveq	lr, #2
    bf74: e5c4e06a     	strb	lr, [r4, #0x6a]
    bf78: e1a00008     	mov	r0, r8
    bf7c: ebffde55     	bl	0x38d8 <.plt+0x1dc>     @ imm = #-0x86ac
    bf80: e3500000     	cmp	r0, #0
    bf84: 0a000005     	beq	0xbfa0 <tick_processButtonDown+0x2a4> @ imm = #0x14
    bf88: e5d4c06a     	ldrb	r12, [r4, #0x6a]
    bf8c: e28c9001     	add	r9, r12, #1
    bf90: e6efa079     	uxtb	r10, r9
    bf94: e35a0002     	cmp	r10, #2
    bf98: 83a0a000     	movhi	r10, #0
    bf9c: e5c4a06a     	strb	r10, [r4, #0x6a]
    bfa0: e1a00006     	mov	r0, r6
    bfa4: ebffde4b     	bl	0x38d8 <.plt+0x1dc>     @ imm = #-0x86d4
    bfa8: e3500000     	cmp	r0, #0
    bfac: 0affff9d     	beq	0xbe28 <tick_processButtonDown+0x12c> @ imm = #-0x18c
    bfb0: e5d42034     	ldrb	r2, [r4, #0x34]
    bfb4: e5976d68     	ldr	r6, [r7, #0xd68]
    bfb8: e5971d6c     	ldr	r1, [r7, #0xd6c]
    bfbc: e35200ff     	cmp	r2, #255
    bfc0: e046a001     	sub	r10, r6, r1
    bfc4: 0a0002fb     	beq	0xcbb8 <tick_processButtonDown+0xebc> @ imm = #0xbec
    bfc8: e35a00c7     	cmp	r10, #199
    bfcc: e5d4506a     	ldrb	r5, [r4, #0x6a]
    bfd0: 9a0002bd     	bls	0xcacc <tick_processButtonDown+0xdd0> @ imm = #0xaf4
    bfd4: e3550000     	cmp	r5, #0
    bfd8: 0a000318     	beq	0xcc40 <tick_processButtonDown+0xf44> @ imm = #0xc60
    bfdc: e3a02000     	mov	r2, #0
    bfe0: e3a01078     	mov	r1, #120
    bfe4: e1a00004     	mov	r0, r4
    bfe8: ebffdebe     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x8508
    bfec: e3a02000     	mov	r2, #0
    bff0: e3a010e7     	mov	r1, #231
    bff4: e1a00004     	mov	r0, r4
    bff8: ebffdeba     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x8518
    bffc: e3a02000     	mov	r2, #0
    c000: e3a01076     	mov	r1, #118
    c004: e1a00004     	mov	r0, r4
    c008: ebffdeb6     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x8528
    c00c: e5d46033     	ldrb	r6, [r4, #0x33]
    c010: e35600ff     	cmp	r6, #255
    c014: 0a0002e1     	beq	0xcba0 <tick_processButtonDown+0xea4> @ imm = #0xb84
    c018: e35a00c7     	cmp	r10, #199
    c01c: 8affff81     	bhi	0xbe28 <tick_processButtonDown+0x12c> @ imm = #-0x1fc
    c020: e5d4806a     	ldrb	r8, [r4, #0x6a]
    c024: e3580001     	cmp	r8, #1
    c028: 0a00032e     	beq	0xcce8 <tick_processButtonDown+0xfec> @ imm = #0xcb8
    c02c: e5d43034     	ldrb	r3, [r4, #0x34]
    c030: e35300ff     	cmp	r3, #255
    c034: 0a000323     	beq	0xccc8 <tick_processButtonDown+0xfcc> @ imm = #0xc8c
    c038: e5d4a06a     	ldrb	r10, [r4, #0x6a]
    c03c: e35a0002     	cmp	r10, #2
    c040: 1affff78     	bne	0xbe28 <tick_processButtonDown+0x12c> @ imm = #-0x220
    c044: e30aeaab     	movw	lr, #0xaaab
    c048: e34aeaaa     	movt	lr, #0xaaaa
    c04c: e3a0200a     	mov	r2, #10
    c050: e3a05006     	mov	r5, #6
    c054: e0898e93     	umull	r8, r9, r3, lr
    c058: e5d48032     	ldrb	r8, [r4, #0x32]
    c05c: e3a0c064     	mov	r12, #100
    c060: e59f0384     	ldr	r0, [pc, #0x384]        @ 0xc3ec <tick_processButtonDown+0x6f0>
    c064: e08f0000     	add	r0, pc, r0
    c068: e1a09129     	lsr	r9, r9, #2
    c06c: e6ef6079     	uxtb	r6, r9
    c070: e0633995     	mls	r3, r5, r9, r3
    c074: e0212296     	mla	r1, r6, r2, r2
    c078: e6efa073     	uxtb	r10, r3
    c07c: e28ae001     	add	lr, r10, #1
    c080: e0221c98     	mla	r2, r8, r12, r1
    c084: e3a08001     	mov	r8, #1
    c088: e08e5002     	add	r5, lr, r2
    c08c: e1a01005     	mov	r1, r5
    c090: ebffdeb8     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x8520
    c094: e1a02005     	mov	r2, r5
    c098: e3a01077     	mov	r1, #119
    c09c: e1a00004     	mov	r0, r4
    c0a0: ebffde90     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x85c0
    c0a4: e5c78de1     	strb	r8, [r7, #0xde1]
    c0a8: e5d46033     	ldrb	r6, [r4, #0x33]
    c0ac: e35600ff     	cmp	r6, #255
    c0b0: 0affff5c     	beq	0xbe28 <tick_processButtonDown+0x12c> @ imm = #-0x290
    c0b4: e5d4c06a     	ldrb	r12, [r4, #0x6a]
    c0b8: e35c0002     	cmp	r12, #2
    c0bc: 1affff59     	bne	0xbe28 <tick_processButtonDown+0x12c> @ imm = #-0x29c
    c0c0: e5d49032     	ldrb	r9, [r4, #0x32]
    c0c4: e3a03064     	mov	r3, #100
    c0c8: e1a00004     	mov	r0, r4
    c0cc: e3a0106c     	mov	r1, #108
    c0d0: e02a3399     	mla	r10, r9, r3, r3
    c0d4: e086600a     	add	r6, r6, r10
    c0d8: e1a02006     	mov	r2, r6
    c0dc: ebffde81     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x85fc
    c0e0: e59f0308     	ldr	r0, [pc, #0x308]        @ 0xc3f0 <tick_processButtonDown+0x6f4>
    c0e4: e3a01001     	mov	r1, #1
    c0e8: e1a03006     	mov	r3, r6
    c0ec: e5c71de1     	strb	r1, [r7, #0xde1]
    c0f0: e08f0000     	add	r0, pc, r0
    c0f4: e5d42033     	ldrb	r2, [r4, #0x33]
    c0f8: e5d4106a     	ldrb	r1, [r4, #0x6a]
    c0fc: ebffde9d     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x858c
    c100: eaffff48     	b	0xbe28 <tick_processButtonDown+0x12c> @ imm = #-0x2e0
    c104: e1a00006     	mov	r0, r6
    c108: ebffde10     	bl	0x3950 <.plt+0x254>     @ imm = #-0x87c0
    c10c: e3500002     	cmp	r0, #2
    c110: 0a000222     	beq	0xc9a0 <tick_processButtonDown+0xca4> @ imm = #0x888
    c114: e5d72d5c     	ldrb	r2, [r7, #0xd5c]
    c118: eaffff40     	b	0xbe20 <tick_processButtonDown+0x124> @ imm = #-0x300
    c11c: e1a00006     	mov	r0, r6
    c120: ebffde0a     	bl	0x3950 <.plt+0x254>     @ imm = #-0x87d8
    c124: e3500000     	cmp	r0, #0
    c128: 0a000010     	beq	0xc170 <tick_processButtonDown+0x474> @ imm = #0x40
    c12c: e59f22c0     	ldr	r2, [pc, #0x2c0]        @ 0xc3f4 <tick_processButtonDown+0x6f8>
    c130: e3a01000     	mov	r1, #0
    c134: ed9f0aa9     	vldr	s0, [pc, #676]          @ 0xc3e0 <tick_processButtonDown+0x6e4>
    c138: e08f0002     	add	r0, pc, r2
    c13c: e59090a8     	ldr	r9, [r0, #0xa8]
    c140: e58010ac     	str	r1, [r0, #0xac]
    c144: e289a001     	add	r10, r9, #1
    c148: e580a0a8     	str	r10, [r0, #0xa8]
    c14c: e35a0007     	cmp	r10, #7
    c150: c58010b0     	strgt	r1, [r0, #0xb0]
    c154: e1a00004     	mov	r0, r4
    c158: ebffdee3     	bl	0x3cec <.plt+0x5f0>     @ imm = #-0x8474
    c15c: e59fc294     	ldr	r12, [pc, #0x294]       @ 0xc3f8 <tick_processButtonDown+0x6fc>
    c160: e1a00004     	mov	r0, r4
    c164: e08f300c     	add	r3, pc, r12
    c168: e5d310b0     	ldrb	r1, [r3, #0xb0]
    c16c: ebffde87     	bl	0x3b90 <.plt+0x494>     @ imm = #-0x85e4
    c170: e1a00007     	mov	r0, r7
    c174: ebffddf5     	bl	0x3950 <.plt+0x254>     @ imm = #-0x882c
    c178: e3500000     	cmp	r0, #0
    c17c: 0a00000c     	beq	0xc1b4 <tick_processButtonDown+0x4b8> @ imm = #0x30
    c180: e59fe274     	ldr	lr, [pc, #0x274]        @ 0xc3fc <tick_processButtonDown+0x700>
    c184: e08f500e     	add	r5, pc, lr
    c188: e59520ac     	ldr	r2, [r5, #0xac]
    c18c: e2820001     	add	r0, r2, #1
    c190: e3500007     	cmp	r0, #7
    c194: d58500ac     	strle	r0, [r5, #0xac]
    c198: de070a90     	vmovle	s15, r0
    c19c: c3a00000     	movgt	r0, #0
    c1a0: cd9f0a8e     	vldrgt	s0, [pc, #568]          @ 0xc3e0 <tick_processButtonDown+0x6e4>
    c1a4: c58500ac     	strgt	r0, [r5, #0xac]
    c1a8: e1a00004     	mov	r0, r4
    c1ac: deb80ae7     	vcvtle.f32.s32	s0, s15
    c1b0: ebffdecd     	bl	0x3cec <.plt+0x5f0>     @ imm = #-0x84cc
    c1b4: e1a00008     	mov	r0, r8
    c1b8: ebffdde4     	bl	0x3950 <.plt+0x254>     @ imm = #-0x8870
    c1bc: e3500000     	cmp	r0, #0
    c1c0: 0a00000a     	beq	0xc1f0 <tick_processButtonDown+0x4f4> @ imm = #0x28
    c1c4: e59f1234     	ldr	r1, [pc, #0x234]        @ 0xc400 <tick_processButtonDown+0x704>
    c1c8: e1a00004     	mov	r0, r4
    c1cc: e08f9001     	add	r9, pc, r1
    c1d0: e599a0b0     	ldr	r10, [r9, #0xb0]
    c1d4: e28a1001     	add	r1, r10, #1
    c1d8: e3510007     	cmp	r1, #7
    c1dc: d58910b0     	strle	r1, [r9, #0xb0]
    c1e0: d6ef1071     	uxtble	r1, r1
    c1e4: c3a01000     	movgt	r1, #0
    c1e8: c58910b0     	strgt	r1, [r9, #0xb0]
    c1ec: ebffde67     	bl	0x3b90 <.plt+0x494>     @ imm = #-0x8664
    c1f0: e5d41030     	ldrb	r1, [r4, #0x30]
    c1f4: eaffff1a     	b	0xbe64 <tick_processButtonDown+0x168> @ imm = #-0x398
    c1f8: e2859014     	add	r9, r5, #20
    c1fc: e1a00009     	mov	r0, r9
    c200: ebffddd2     	bl	0x3950 <.plt+0x254>     @ imm = #-0x88b8
    c204: e3500002     	cmp	r0, #2
    c208: 8affff01     	bhi	0xbe14 <tick_processButtonDown+0x118> @ imm = #-0x3fc
    c20c: e284ad76     	add	r10, r4, #7552
    c210: e1a0000a     	mov	r0, r10
    c214: ebffddcd     	bl	0x3950 <.plt+0x254>     @ imm = #-0x88cc
    c218: e3500003     	cmp	r0, #3
    c21c: 8afffefc     	bhi	0xbe14 <tick_processButtonDown+0x118> @ imm = #-0x410
    c220: e59f51dc     	ldr	r5, [pc, #0x1dc]        @ 0xc404 <tick_processButtonDown+0x708>
    c224: e3a03000     	mov	r3, #0
    c228: e5c78d5c     	strb	r8, [r7, #0xd5c]
    c22c: e08fc005     	add	r12, pc, r5
    c230: eddc0a26     	vldr	s1, [r12, #152]
    c234: eef50a40     	vcmp.f32	s1, #0
    c238: eef1fa10     	vmrs	APSR_nzcv, fpscr
    c23c: 0a00000a     	beq	0xc26c <tick_processButtonDown+0x570> @ imm = #0x28
    c240: e5d4003d     	ldrb	r0, [r4, #0x3d]
    c244: e5976ddc     	ldr	r6, [r7, #0xddc]
    c248: e5d4103c     	ldrb	r1, [r4, #0x3c]
    c24c: e290e000     	adds	lr, r0, #0
    c250: e58c3098     	str	r3, [r12, #0x98]
    c254: 13a0e001     	movne	lr, #1
    c258: e5846040     	str	r6, [r4, #0x40]
    c25c: e5846048     	str	r6, [r4, #0x48]
    c260: e5c4e045     	strb	lr, [r4, #0x45]
    c264: e5c41044     	strb	r1, [r4, #0x44]
    c268: e5c4803c     	strb	r8, [r4, #0x3c]
    c26c: e1a00009     	mov	r0, r9
    c270: ebffdd98     	bl	0x38d8 <.plt+0x1dc>     @ imm = #-0x89a0
    c274: e3500000     	cmp	r0, #0
    c278: 0a000007     	beq	0xc29c <tick_processButtonDown+0x5a0> @ imm = #0x1c
    c27c: e5d48025     	ldrb	r8, [r4, #0x25]
    c280: e3580000     	cmp	r8, #0
    c284: 0a000004     	beq	0xc29c <tick_processButtonDown+0x5a0> @ imm = #0x10
    c288: e5d42028     	ldrb	r2, [r4, #0x28]
    c28c: e3520000     	cmp	r2, #0
    c290: 03a02001     	moveq	r2, #1
    c294: 05c42026     	strbeq	r2, [r4, #0x26]
    c298: 0a00000e     	beq	0xc2d8 <tick_processButtonDown+0x5dc> @ imm = #0x38
    c29c: e1a00009     	mov	r0, r9
    c2a0: ebffdd8c     	bl	0x38d8 <.plt+0x1dc>     @ imm = #-0x89d0
    c2a4: e3500000     	cmp	r0, #0
    c2a8: 0a0001d4     	beq	0xca00 <tick_processButtonDown+0xd04> @ imm = #0x750
    c2ac: e5d45025     	ldrb	r5, [r4, #0x25]
    c2b0: e3550000     	cmp	r5, #0
    c2b4: 1a0001d1     	bne	0xca00 <tick_processButtonDown+0xd04> @ imm = #0x744
    c2b8: e5d4c028     	ldrb	r12, [r4, #0x28]
    c2bc: e35c0000     	cmp	r12, #0
    c2c0: 1a0001ce     	bne	0xca00 <tick_processButtonDown+0xd04> @ imm = #0x738
    c2c4: e5970ddc     	ldr	r0, [r7, #0xddc]
    c2c8: e5978d78     	ldr	r8, [r7, #0xd78]
    c2cc: e0402008     	sub	r2, r0, r8
    c2d0: e3520001     	cmp	r2, #1
    c2d4: 9a00023d     	bls	0xcbd0 <tick_processButtonDown+0xed4> @ imm = #0x8f4
    c2d8: e1a0000a     	mov	r0, r10
    c2dc: ebffdd7d     	bl	0x38d8 <.plt+0x1dc>     @ imm = #-0x8a0c
    c2e0: e3500000     	cmp	r0, #0
    c2e4: 0afffecf     	beq	0xbe28 <tick_processButtonDown+0x12c> @ imm = #-0x4c4
    c2e8: e597addc     	ldr	r10, [r7, #0xddc]
    c2ec: e5975d88     	ldr	r5, [r7, #0xd88]
    c2f0: e04ac005     	sub	r12, r10, r5
    c2f4: e35c0001     	cmp	r12, #1
    c2f8: 8afffeca     	bhi	0xbe28 <tick_processButtonDown+0x12c> @ imm = #-0x4d8
    c2fc: e5d79df5     	ldrb	r9, [r7, #0xdf5]
    c300: e1a00004     	mov	r0, r4
    c304: e3590000     	cmp	r9, #0
    c308: 0a000002     	beq	0xc318 <tick_processButtonDown+0x61c> @ imm = #0x8
    c30c: e5d77d5c     	ldrb	r7, [r7, #0xd5c]
    c310: e3570000     	cmp	r7, #0
    c314: 0a0001ea     	beq	0xcac4 <tick_processButtonDown+0xdc8> @ imm = #0x7a8
    c318: ebffdd65     	bl	0x38b4 <.plt+0x1b8>     @ imm = #-0x8a6c
    c31c: ed9f0b2d     	vldr	d0, [pc, #180]          @ 0xc3d8 <tick_processButtonDown+0x6dc>
    c320: e59400dc     	ldr	r0, [r4, #0xdc]
    c324: ebffdd80     	bl	0x392c <.plt+0x230>     @ imm = #-0x8a00
    c328: e5d430c4     	ldrb	r3, [r4, #0xc4]
    c32c: e3530000     	cmp	r3, #0
    c330: 1a0001d9     	bne	0xca9c <tick_processButtonDown+0xda0> @ imm = #0x764
    c334: e59460b4     	ldr	r6, [r4, #0xb4]
    c338: e5d61002     	ldrb	r1, [r6, #0x2]
    c33c: e3510000     	cmp	r1, #0
    c340: 0afffeb8     	beq	0xbe28 <tick_processButtonDown+0x12c> @ imm = #-0x520
    c344: eeb70a00     	vmov.f32	s0, #1.000000e+00
    c348: ebffdde0     	bl	0x3ad0 <.plt+0x3d4>     @ imm = #-0x8880
    c34c: e59400d0     	ldr	r0, [r4, #0xd0]
    c350: eeb20b00     	vmov.f64	d0, #8.000000e+00
    c354: ebffdd74     	bl	0x392c <.plt+0x230>     @ imm = #-0x8a30
    c358: e5d4103c     	ldrb	r1, [r4, #0x3c]
    c35c: eafffeb2     	b	0xbe2c <tick_processButtonDown+0x130> @ imm = #-0x538
    c360: e2850014     	add	r0, r5, #20
    c364: ebffdd79     	bl	0x3950 <.plt+0x254>     @ imm = #-0x8a1c
    c368: e3500002     	cmp	r0, #2
    c36c: 1afffedf     	bne	0xbef0 <tick_processButtonDown+0x1f4> @ imm = #-0x484
    c370: e2840d76     	add	r0, r4, #7552
    c374: ebffdd75     	bl	0x3950 <.plt+0x254>     @ imm = #-0x8a2c
    c378: e3500003     	cmp	r0, #3
    c37c: 1afffedb     	bne	0xbef0 <tick_processButtonDown+0x1f4> @ imm = #-0x494
    c380: e59fe080     	ldr	lr, [pc, #0x80]         @ 0xc408 <tick_processButtonDown+0x70c>
    c384: e08f200e     	add	r2, pc, lr
    c388: edd26a26     	vldr	s13, [r2, #152]
    c38c: e3a08000     	mov	r8, #0
    c390: e3a0c000     	mov	r12, #0
    c394: e5c78d5c     	strb	r8, [r7, #0xd5c]
    c398: e5d4103c     	ldrb	r1, [r4, #0x3c]
    c39c: eef56a40     	vcmp.f32	s13, #0
    c3a0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    c3a4: 0afffea0     	beq	0xbe2c <tick_processButtonDown+0x130> @ imm = #-0x580
    c3a8: e5d4603d     	ldrb	r6, [r4, #0x3d]
    c3ac: e5977ddc     	ldr	r7, [r7, #0xddc]
    c3b0: e056a008     	subs	r10, r6, r8
    c3b4: e5c41044     	strb	r1, [r4, #0x44]
    c3b8: e5c4803c     	strb	r8, [r4, #0x3c]
    c3bc: e1a01008     	mov	r1, r8
    c3c0: 13a0a001     	movne	r10, #1
    c3c4: e5847040     	str	r7, [r4, #0x40]
    c3c8: e5847048     	str	r7, [r4, #0x48]
    c3cc: e582c098     	str	r12, [r2, #0x98]
    c3d0: e5c4a045     	strb	r10, [r4, #0x45]
    c3d4: eafffe94     	b	0xbe2c <tick_processButtonDown+0x130> @ imm = #-0x5b0
    c3d8: 00 00 00 00  	.word	0x00000000
    c3dc: 00 00 00 00  	.word	0x00000000
    c3e0: 00 00 00 00  	.word	0x00000000
    c3e4: 28 9a 00 00  	.word	0x00009a28
    c3e8: 20 9a 00 00  	.word	0x00009a20
    c3ec: e4 97 00 00  	.word	0x000097e4
    c3f0: 70 97 00 00  	.word	0x00009770
    c3f4: 78 b2 01 00  	.word	0x0001b278
    c3f8: 4c b2 01 00  	.word	0x0001b24c
    c3fc: 2c b2 01 00  	.word	0x0001b22c
    c400: e4 b1 01 00  	.word	0x0001b1e4
    c404: 84 b1 01 00  	.word	0x0001b184
    c408: 2c b0 01 00  	.word	0x0001b02c
    c40c: 04 af 01 00  	.word	0x0001af04
    c410: ec ae 01 00  	.word	0x0001aeec
    c414: 70 91 00 00  	.word	0x00009170
    c418: 68 91 00 00  	.word	0x00009168
    c41c: b4 85 00 00  	.word	0x000085b4
    c420: 40 85 00 00  	.word	0x00008540
    c424: 98 ad 01 00  	.word	0x0001ad98
    c428: 7c ac 01 00  	.word	0x0001ac7c
    c42c: 2c 87 00 00  	.word	0x0000872c
    c430: 24 84 00 00  	.word	0x00008424
    c434: 80 ac 01 00  	.word	0x0001ac80
    c438: b8 8e 00 00  	.word	0x00008eb8
    c43c: b0 8e 00 00  	.word	0x00008eb0
    c440: 78 8e 00 00  	.word	0x00008e78
    c444: 78 8e 00 00  	.word	0x00008e78
    c448: 74 8e 00 00  	.word	0x00008e74
    c44c: 0c 82 00 00  	.word	0x0000820c
    c450: 08 8e 00 00  	.word	0x00008e08
    c454: 54 aa 01 00  	.word	0x0001aa54
    c458: b8 a9 01 00  	.word	0x0001a9b8
    c45c: 60 8c 00 00  	.word	0x00008c60
    c460: 8c 8c 00 00  	.word	0x00008c8c
    c464: a8 8c 00 00  	.word	0x00008ca8
    c468: bc 8b 00 00  	.word	0x00008bbc
    c46c: 5c 8b 00 00  	.word	0x00008b5c
    c470: e4 8a 00 00  	.word	0x00008ae4
    c474: 00 8b 00 00  	.word	0x00008b00
    c478: e2850014     	add	r0, r5, #20
    c47c: ebffdd33     	bl	0x3950 <.plt+0x254>     @ imm = #-0x8b34
    c480: e250a000     	subs	r10, r0, #0
    c484: 1afffe92     	bne	0xbed4 <tick_processButtonDown+0x1d8> @ imm = #-0x5b8
    c488: e2840d76     	add	r0, r4, #7552
    c48c: ebffdd2f     	bl	0x3950 <.plt+0x254>     @ imm = #-0x8b44
    c490: e3500003     	cmp	r0, #3
    c494: 1afffe8e     	bne	0xbed4 <tick_processButtonDown+0x1d8> @ imm = #-0x5c8
    c498: e5973d88     	ldr	r3, [r7, #0xd88]
    c49c: e597ed8c     	ldr	lr, [r7, #0xd8c]
    c4a0: e043200e     	sub	r2, r3, lr
    c4a4: e35200c8     	cmp	r2, #200
    c4a8: d51f00a4     	ldrle	r0, [pc, #-0xa4]        @ 0xc40c <tick_processButtonDown+0x710>
    c4ac: d08f1000     	addle	r1, pc, r0
    c4b0: d59190a4     	ldrle	r9, [r1, #0xa4]
    c4b4: d289a001     	addle	r10, r9, #1
    c4b8: e51fc0b0     	ldr	r12, [pc, #-0xb0]       @ 0xc410 <tick_processButtonDown+0x714>
    c4bc: e1a0100a     	mov	r1, r10
    c4c0: e51f80b4     	ldr	r8, [pc, #-0xb4]        @ 0xc414 <tick_processButtonDown+0x718>
    c4c4: e08f900c     	add	r9, pc, r12
    c4c8: e08f0008     	add	r0, pc, r8
    c4cc: e589a0a4     	str	r10, [r9, #0xa4]
    c4d0: ebffdda8     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x8960
    c4d4: e59930a4     	ldr	r3, [r9, #0xa4]
    c4d8: e3530006     	cmp	r3, #6
    c4dc: dafffe7c     	ble	0xbed4 <tick_processButtonDown+0x1d8> @ imm = #-0x610
    c4e0: e51fe0d0     	ldr	lr, [pc, #-0xd0]        @ 0xc418 <tick_processButtonDown+0x71c>
    c4e4: e3a02002     	mov	r2, #2
    c4e8: e58d2008     	str	r2, [sp, #0x8]
    c4ec: e08f000e     	add	r0, pc, lr
    c4f0: ebffdc8c     	bl	0x3728 <.plt+0x2c>      @ imm = #-0x8dd0
    c4f4: e597adb4     	ldr	r10, [r7, #0xdb4]
    c4f8: e58d000c     	str	r0, [sp, #0xc]
    c4fc: e51f00e8     	ldr	r0, [pc, #-0xe8]        @ 0xc41c <tick_processButtonDown+0x720>
    c500: e08f0000     	add	r0, pc, r0
    c504: ebffdc87     	bl	0x3728 <.plt+0x2c>      @ imm = #-0x8de4
    c508: e28d3008     	add	r3, sp, #8
    c50c: e3a02001     	mov	r2, #1
    c510: e1a01000     	mov	r1, r0
    c514: e1a0000a     	mov	r0, r10
    c518: ebffddd5     	bl	0x3c74 <.plt+0x578>     @ imm = #-0x88ac
    c51c: ed990a26     	vldr	s0, [r9, #152]
    c520: e3a0c000     	mov	r12, #0
    c524: e3a01000     	mov	r1, #0
    c528: e5c7cd5c     	strb	r12, [r7, #0xd5c]
    c52c: eeb50a40     	vcmp.f32	s0, #0
    c530: eef1fa10     	vmrs	APSR_nzcv, fpscr
    c534: 0a00000a     	beq	0xc564 <tick_processButtonDown+0x868> @ imm = #0x28
    c538: e5d4303d     	ldrb	r3, [r4, #0x3d]
    c53c: e5d4803c     	ldrb	r8, [r4, #0x3c]
    c540: e5891098     	str	r1, [r9, #0x98]
    c544: e053e00c     	subs	lr, r3, r12
    c548: e5979ddc     	ldr	r9, [r7, #0xddc]
    c54c: 13a0e001     	movne	lr, #1
    c550: e5c48044     	strb	r8, [r4, #0x44]
    c554: e5c4e045     	strb	lr, [r4, #0x45]
    c558: e5849040     	str	r9, [r4, #0x40]
    c55c: e5849048     	str	r9, [r4, #0x48]
    c560: e5c4c03c     	strb	r12, [r4, #0x3c]
    c564: e51f214c     	ldr	r2, [pc, #-0x14c]       @ 0xc420 <tick_processButtonDown+0x724>
    c568: e3a0c001     	mov	r12, #1
    c56c: e5948070     	ldr	r8, [r4, #0x70]
    c570: e3a0a000     	mov	r10, #0
    c574: e08f0002     	add	r0, pc, r2
    c578: e344a2aa     	movt	r10, #0x42aa
    c57c: e58dc010     	str	r12, [sp, #0x10]
    c580: e3a09000     	mov	r9, #0
    c584: e58da014     	str	r10, [sp, #0x14]
    c588: e3449313     	movt	r9, #0x4313
    c58c: e58dc018     	str	r12, [sp, #0x18]
    c590: e3a01000     	mov	r1, #0
    c594: e58d901c     	str	r9, [sp, #0x1c]
    c598: e3441228     	movt	r1, #0x4228
    c59c: e58dc020     	str	r12, [sp, #0x20]
    c5a0: e3a03000     	mov	r3, #0
    c5a4: e58d1024     	str	r1, [sp, #0x24]
    c5a8: e3443312     	movt	r3, #0x4312
    c5ac: e58dc028     	str	r12, [sp, #0x28]
    c5b0: e58d302c     	str	r3, [sp, #0x2c]
    c5b4: ebffdc5b     	bl	0x3728 <.plt+0x2c>      @ imm = #-0x8e94
    c5b8: e28d3010     	add	r3, sp, #16
    c5bc: e3a02004     	mov	r2, #4
    c5c0: e1a01000     	mov	r1, r0
    c5c4: e1a00008     	mov	r0, r8
    c5c8: ebffdda9     	bl	0x3c74 <.plt+0x578>     @ imm = #-0x895c
    c5cc: eafffe40     	b	0xbed4 <tick_processButtonDown+0x1d8> @ imm = #-0x700
    c5d0: e2850014     	add	r0, r5, #20
    c5d4: ebffdcdd     	bl	0x3950 <.plt+0x254>     @ imm = #-0x8c8c
    c5d8: e3500002     	cmp	r0, #2
    c5dc: 1afffddf     	bne	0xbd60 <tick_processButtonDown+0x64> @ imm = #-0x884
    c5e0: e2840d76     	add	r0, r4, #7552
    c5e4: ebffdcd9     	bl	0x3950 <.plt+0x254>     @ imm = #-0x8c9c
    c5e8: e3500003     	cmp	r0, #3
    c5ec: 1afffddb     	bne	0xbd60 <tick_processButtonDown+0x64> @ imm = #-0x894
    c5f0: e3a0c004     	mov	r12, #4
    c5f4: e3a01000     	mov	r1, #0
    c5f8: e5c7cd5c     	strb	r12, [r7, #0xd5c]
    c5fc: e1a00004     	mov	r0, r4
    c600: ebffdcbd     	bl	0x38fc <.plt+0x200>     @ imm = #-0x8d0c
    c604: e5d76d5c     	ldrb	r6, [r7, #0xd5c]
    c608: e51f11ec     	ldr	r1, [pc, #-0x1ec]       @ 0xc424 <tick_processButtonDown+0x728>
    c60c: e3a03000     	mov	r3, #0
    c610: e5c73d62     	strb	r3, [r7, #0xd62]
    c614: ee056a10     	vmov	s10, r6
    c618: e08fa001     	add	r10, pc, r1
    c61c: e5c73d72     	strb	r3, [r7, #0xd72]
    c620: eef85a45     	vcvt.f32.u32	s11, s10
    c624: e5c73d82     	strb	r3, [r7, #0xd82]
    c628: ed9a6a26     	vldr	s12, [r10, #152]
    c62c: eef45a46     	vcmp.f32	s11, s12
    c630: eef1fa10     	vmrs	APSR_nzcv, fpscr
    c634: 0a00000a     	beq	0xc664 <tick_processButtonDown+0x968> @ imm = #0x28
    c638: e5d4503d     	ldrb	r5, [r4, #0x3d]
    c63c: edca5a26     	vstr	s11, [r10, #152]
    c640: e5979ddc     	ldr	r9, [r7, #0xddc]
    c644: e295e000     	adds	lr, r5, #0
    c648: e5d4003c     	ldrb	r0, [r4, #0x3c]
    c64c: 13a0e001     	movne	lr, #1
    c650: e5c4603c     	strb	r6, [r4, #0x3c]
    c654: e5849040     	str	r9, [r4, #0x40]
    c658: e5849048     	str	r9, [r4, #0x48]
    c65c: e5c40044     	strb	r0, [r4, #0x44]
    c660: e5c4e045     	strb	lr, [r4, #0x45]
    c664: e51f7244     	ldr	r7, [pc, #-0x244]       @ 0xc428 <tick_processButtonDown+0x72c>
    c668: e08f8007     	add	r8, pc, r7
    c66c: e5d82003     	ldrb	r2, [r8, #0x3]
    c670: e3520000     	cmp	r2, #0
    c674: 0a000016     	beq	0xc6d4 <tick_processButtonDown+0x9d8> @ imm = #0x58
    c678: e51fc254     	ldr	r12, [pc, #-0x254]      @ 0xc42c <tick_processButtonDown+0x730>
    c67c: e3a01000     	mov	r1, #0
    c680: e51f6258     	ldr	r6, [pc, #-0x258]       @ 0xc430 <tick_processButtonDown+0x734>
    c684: e3a0a001     	mov	r10, #1
    c688: e08f000c     	add	r0, pc, r12
    c68c: ebffdd39     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x8b1c
    c690: e08f0006     	add	r0, pc, r6
    c694: e3a01000     	mov	r1, #0
    c698: e5949070     	ldr	r9, [r4, #0x70]
    c69c: e58d1014     	str	r1, [sp, #0x14]
    c6a0: e3a03000     	mov	r3, #0
    c6a4: e58da010     	str	r10, [sp, #0x10]
    c6a8: e344331d     	movt	r3, #0x431d
    c6ac: e58da018     	str	r10, [sp, #0x18]
    c6b0: e58d301c     	str	r3, [sp, #0x1c]
    c6b4: ebffdc1b     	bl	0x3728 <.plt+0x2c>      @ imm = #-0x8f94
    c6b8: e28d3010     	add	r3, sp, #16
    c6bc: e3a02002     	mov	r2, #2
    c6c0: e1a01000     	mov	r1, r0
    c6c4: e1a00009     	mov	r0, r9
    c6c8: ebffdd69     	bl	0x3c74 <.plt+0x578>     @ imm = #-0x8a5c
    c6cc: e3a00000     	mov	r0, #0
    c6d0: e5c80003     	strb	r0, [r8, #0x3]
    c6d4: e3a010de     	mov	r1, #222
    c6d8: e3a02001     	mov	r2, #1
    c6dc: e1a00004     	mov	r0, r4
    c6e0: e3a05001     	mov	r5, #1
    c6e4: ebffdcff     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x8c04
    c6e8: e5d4103c     	ldrb	r1, [r4, #0x3c]
    c6ec: e5c45038     	strb	r5, [r4, #0x38]
    c6f0: eafffdcd     	b	0xbe2c <tick_processButtonDown+0x130> @ imm = #-0x8cc
    c6f4: e2850014     	add	r0, r5, #20
    c6f8: ebffdc94     	bl	0x3950 <.plt+0x254>     @ imm = #-0x8db0
    c6fc: e3500002     	cmp	r0, #2
    c700: 1afffd9d     	bne	0xbd7c <tick_processButtonDown+0x80> @ imm = #-0x98c
    c704: e2840d76     	add	r0, r4, #7552
    c708: ebffdc90     	bl	0x3950 <.plt+0x254>     @ imm = #-0x8dc0
    c70c: e3500003     	cmp	r0, #3
    c710: 0afffd99     	beq	0xbd7c <tick_processButtonDown+0x80> @ imm = #-0x99c
    c714: e5d4103c     	ldrb	r1, [r4, #0x3c]
    c718: e3510002     	cmp	r1, #2
    c71c: 0afffd96     	beq	0xbd7c <tick_processButtonDown+0x80> @ imm = #-0x9a8
    c720: eeb04a08     	vmov.f32	s8, #3.000000e+00
    c724: e51f02f8     	ldr	r0, [pc, #-0x2f8]       @ 0xc434 <tick_processButtonDown+0x738>
    c728: e3a0e003     	mov	lr, #3
    c72c: e5c7ed5c     	strb	lr, [r7, #0xd5c]
    c730: e08f5000     	add	r5, pc, r0
    c734: edd54a26     	vldr	s9, [r5, #152]
    c738: eef44a44     	vcmp.f32	s9, s8
    c73c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    c740: 0afffdb9     	beq	0xbe2c <tick_processButtonDown+0x130> @ imm = #-0x91c
    c744: e5d4203d     	ldrb	r2, [r4, #0x3d]
    c748: e5977ddc     	ldr	r7, [r7, #0xddc]
    c74c: e2928000     	adds	r8, r2, #0
    c750: e5c41044     	strb	r1, [r4, #0x44]
    c754: e5c4e03c     	strb	lr, [r4, #0x3c]
    c758: e1a0100e     	mov	r1, lr
    c75c: 13a08001     	movne	r8, #1
    c760: e5847040     	str	r7, [r4, #0x40]
    c764: e5847048     	str	r7, [r4, #0x48]
    c768: ed854a26     	vstr	s8, [r5, #152]
    c76c: e5c48045     	strb	r8, [r4, #0x45]
    c770: eafffdad     	b	0xbe2c <tick_processButtonDown+0x130> @ imm = #-0x94c
    c774: e2850014     	add	r0, r5, #20
    c778: ebffdc74     	bl	0x3950 <.plt+0x254>     @ imm = #-0x8e30
    c77c: e3500002     	cmp	r0, #2
    c780: 1afffd85     	bne	0xbd9c <tick_processButtonDown+0xa0> @ imm = #-0x9ec
    c784: e2840d76     	add	r0, r4, #7552
    c788: ebffdc70     	bl	0x3950 <.plt+0x254>     @ imm = #-0x8e40
    c78c: e3500003     	cmp	r0, #3
    c790: 0afffd81     	beq	0xbd9c <tick_processButtonDown+0xa0> @ imm = #-0x9fc
    c794: e5d4203c     	ldrb	r2, [r4, #0x3c]
    c798: e3520002     	cmp	r2, #2
    c79c: 1afffd7e     	bne	0xbd9c <tick_processButtonDown+0xa0> @ imm = #-0xa08
    c7a0: e51f3370     	ldr	r3, [pc, #-0x370]       @ 0xc438 <tick_processButtonDown+0x73c>
    c7a4: e3a0a000     	mov	r10, #0
    c7a8: e08f0003     	add	r0, pc, r3
    c7ac: ebffdcf1     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x8c3c
    c7b0: e3a01000     	mov	r1, #0
    c7b4: e1a00004     	mov	r0, r4
    c7b8: e584a02c     	str	r10, [r4, #0x2c]
    c7bc: ebffdc51     	bl	0x3908 <.plt+0x20c>     @ imm = #-0x8ebc
    c7c0: e3a01000     	mov	r1, #0
    c7c4: e1a00004     	mov	r0, r4
    c7c8: ebffdd7d     	bl	0x3dc4 <.plt+0x6c8>     @ imm = #-0x8a0c
    c7cc: e1a01009     	mov	r1, r9
    c7d0: e1a00004     	mov	r0, r4
    c7d4: e51f93a0     	ldr	r9, [pc, #-0x3a0]       @ 0xc43c <tick_processButtonDown+0x740>
    c7d8: ebffdc4a     	bl	0x3908 <.plt+0x20c>     @ imm = #-0x8ed8
    c7dc: ed943a0b     	vldr	s6, [r4, #44]
    c7e0: e08f0009     	add	r0, pc, r9
    c7e4: eefd3ac3     	vcvt.s32.f32	s7, s6
    c7e8: ee131a90     	vmov	r1, s7
    c7ec: ebffdce1     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x8c7c
    c7f0: e5d4103c     	ldrb	r1, [r4, #0x3c]
    c7f4: eafffd8c     	b	0xbe2c <tick_processButtonDown+0x130> @ imm = #-0x9d0
    c7f8: e2850014     	add	r0, r5, #20
    c7fc: ebffdc53     	bl	0x3950 <.plt+0x254>     @ imm = #-0x8eb4
    c800: e3500002     	cmp	r0, #2
    c804: 0afffd6c     	beq	0xbdbc <tick_processButtonDown+0xc0> @ imm = #-0xa50
    c808: e2840d76     	add	r0, r4, #7552
    c80c: ebffdc4f     	bl	0x3950 <.plt+0x254>     @ imm = #-0x8ec4
    c810: e3500002     	cmp	r0, #2
    c814: 1afffd68     	bne	0xbdbc <tick_processButtonDown+0xc0> @ imm = #-0xa60
    c818: e5d4303c     	ldrb	r3, [r4, #0x3c]
    c81c: e3530002     	cmp	r3, #2
    c820: 0afffd65     	beq	0xbdbc <tick_processButtonDown+0xc0> @ imm = #-0xa6c
    c824: e51f13ec     	ldr	r1, [pc, #-0x3ec]       @ 0xc440 <tick_processButtonDown+0x744>
    c828: e08f0001     	add	r0, pc, r1
    c82c: ebffdcd1     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x8cbc
    c830: e5d4a037     	ldrb	r10, [r4, #0x37]
    c834: e1a00004     	mov	r0, r4
    c838: e28a707c     	add	r7, r10, #124
    c83c: ee027a10     	vmov	s4, r7
    c840: eeb80ac2     	vcvt.f32.s32	s0, s4
    c844: ebffdbe7     	bl	0x37e8 <.plt+0xec>      @ imm = #-0x9064
    c848: e51f040c     	ldr	r0, [pc, #-0x40c]       @ 0xc444 <tick_processButtonDown+0x748>
    c84c: e5d41037     	ldrb	r1, [r4, #0x37]
    c850: e08f0000     	add	r0, pc, r0
    c854: eefd2ac0     	vcvt.s32.f32	s5, s0
    c858: ee125a90     	vmov	r5, s5
    c85c: ee122a90     	vmov	r2, s5
    c860: ebffdcc4     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x8cf0
    c864: e3550001     	cmp	r5, #1
    c868: 0a000082     	beq	0xca78 <tick_processButtonDown+0xd7c> @ imm = #0x208
    c86c: e3550002     	cmp	r5, #2
    c870: 1a000007     	bne	0xc894 <tick_processButtonDown+0xb98> @ imm = #0x1c
    c874: e51f2434     	ldr	r2, [pc, #-0x434]       @ 0xc448 <tick_processButtonDown+0x74c>
    c878: e1a0100a     	mov	r1, r10
    c87c: e08f0002     	add	r0, pc, r2
    c880: ebffdcbc     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x8d10
    c884: e1a02008     	mov	r2, r8
    c888: e1a01007     	mov	r1, r7
    c88c: e1a00004     	mov	r0, r4
    c890: ebffdc94     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x8db0
    c894: e5d48030     	ldrb	r8, [r4, #0x30]
    c898: e3580062     	cmp	r8, #98
    c89c: 8afffd61     	bhi	0xbe28 <tick_processButtonDown+0x12c> @ imm = #-0xa7c
    c8a0: e51fc45c     	ldr	r12, [pc, #-0x45c]      @ 0xc44c <tick_processButtonDown+0x750>
    c8a4: e594606c     	ldr	r6, [r4, #0x6c]
    c8a8: e08f000c     	add	r0, pc, r12
    c8ac: ebffdb9d     	bl	0x3728 <.plt+0x2c>      @ imm = #-0x918c
    c8b0: e28d3010     	add	r3, sp, #16
    c8b4: e3a02003     	mov	r2, #3
    c8b8: e1a01000     	mov	r1, r0
    c8bc: e1a00006     	mov	r0, r6
    c8c0: ebffdceb     	bl	0x3c74 <.plt+0x578>     @ imm = #-0x8c54
    c8c4: eafffd57     	b	0xbe28 <tick_processButtonDown+0x12c> @ imm = #-0xaa4
    c8c8: e2850014     	add	r0, r5, #20
    c8cc: ebffdc1f     	bl	0x3950 <.plt+0x254>     @ imm = #-0x8f84
    c8d0: e3500002     	cmp	r0, #2
    c8d4: 0afffd3f     	beq	0xbdd8 <tick_processButtonDown+0xdc> @ imm = #-0xb04
    c8d8: e2840d76     	add	r0, r4, #7552
    c8dc: ebffdc1b     	bl	0x3950 <.plt+0x254>     @ imm = #-0x8f94
    c8e0: e3500002     	cmp	r0, #2
    c8e4: 1afffd3b     	bne	0xbdd8 <tick_processButtonDown+0xdc> @ imm = #-0xb14
    c8e8: e5d4e03c     	ldrb	lr, [r4, #0x3c]
    c8ec: e35e0002     	cmp	lr, #2
    c8f0: 1afffd38     	bne	0xbdd8 <tick_processButtonDown+0xdc> @ imm = #-0xb20
    c8f4: e51f94ac     	ldr	r9, [pc, #-0x4ac]       @ 0xc450 <tick_processButtonDown+0x754>
    c8f8: e08f0009     	add	r0, pc, r9
    c8fc: ebffdc9d     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x8d8c
    c900: e1a00004     	mov	r0, r4
    c904: ebffdcc8     	bl	0x3c2c <.plt+0x530>     @ imm = #-0x8ce0
    c908: e5d4103c     	ldrb	r1, [r4, #0x3c]
    c90c: eafffd46     	b	0xbe2c <tick_processButtonDown+0x130> @ imm = #-0xae8
    c910: e2850014     	add	r0, r5, #20
    c914: ebffdc0d     	bl	0x3950 <.plt+0x254>     @ imm = #-0x8fcc
    c918: e3500002     	cmp	r0, #2
    c91c: 0afffd35     	beq	0xbdf8 <tick_processButtonDown+0xfc> @ imm = #-0xb2c
    c920: e2840d76     	add	r0, r4, #7552
    c924: ebffdc09     	bl	0x3950 <.plt+0x254>     @ imm = #-0x8fdc
    c928: e3500003     	cmp	r0, #3
    c92c: 0afffd31     	beq	0xbdf8 <tick_processButtonDown+0xfc> @ imm = #-0xb3c
    c930: e597ed68     	ldr	lr, [r7, #0xd68]
    c934: e5978d6c     	ldr	r8, [r7, #0xd6c]
    c938: e5c79d5c     	strb	r9, [r7, #0xd5c]
    c93c: e04e2008     	sub	r2, lr, r8
    c940: e5d4103c     	ldrb	r1, [r4, #0x3c]
    c944: e35200c7     	cmp	r2, #199
    c948: ca000001     	bgt	0xc954 <tick_processButtonDown+0xc58> @ imm = #0x4
    c94c: e3510000     	cmp	r1, #0
    c950: 0a00003d     	beq	0xca4c <tick_processButtonDown+0xd50> @ imm = #0xf4
    c954: eeb71a00     	vmov.f32	s2, #1.000000e+00
    c958: e51f550c     	ldr	r5, [pc, #-0x50c]       @ 0xc454 <tick_processButtonDown+0x758>
    c95c: e08fc005     	add	r12, pc, r5
    c960: ed9c7a26     	vldr	s14, [r12, #152]
    c964: eeb47a41     	vcmp.f32	s14, s2
    c968: eef1fa10     	vmrs	APSR_nzcv, fpscr
    c96c: 0afffd2e     	beq	0xbe2c <tick_processButtonDown+0x130> @ imm = #-0xb48
    c970: e5d4303d     	ldrb	r3, [r4, #0x3d]
    c974: e5977ddc     	ldr	r7, [r7, #0xddc]
    c978: e2936000     	adds	r6, r3, #0
    c97c: e5c41044     	strb	r1, [r4, #0x44]
    c980: e5c4903c     	strb	r9, [r4, #0x3c]
    c984: e1a01009     	mov	r1, r9
    c988: 13a06001     	movne	r6, #1
    c98c: e5847040     	str	r7, [r4, #0x40]
    c990: e5847048     	str	r7, [r4, #0x48]
    c994: ed8c1a26     	vstr	s2, [r12, #152]
    c998: e5c46045     	strb	r6, [r4, #0x45]
    c99c: eafffd22     	b	0xbe2c <tick_processButtonDown+0x130> @ imm = #-0xb78
    c9a0: e2850014     	add	r0, r5, #20
    c9a4: ebffdbe9     	bl	0x3950 <.plt+0x254>     @ imm = #-0x905c
    c9a8: e3500001     	cmp	r0, #1
    c9ac: 1afffdd8     	bne	0xc114 <tick_processButtonDown+0x418> @ imm = #-0x8a0
    c9b0: e2840d76     	add	r0, r4, #7552
    c9b4: ebffdbe5     	bl	0x3950 <.plt+0x254>     @ imm = #-0x906c
    c9b8: e3500000     	cmp	r0, #0
    c9bc: 1afffdd4     	bne	0xc114 <tick_processButtonDown+0x418> @ imm = #-0x8b0
    c9c0: e3e01000     	mvn	r1, #0
    c9c4: e1a00004     	mov	r0, r4
    c9c8: ebffdbce     	bl	0x3908 <.plt+0x20c>     @ imm = #-0x90c8
    c9cc: e5d4103c     	ldrb	r1, [r4, #0x3c]
    c9d0: eafffd15     	b	0xbe2c <tick_processButtonDown+0x130> @ imm = #-0xbac
    c9d4: e2850014     	add	r0, r5, #20
    c9d8: ebffdbdc     	bl	0x3950 <.plt+0x254>     @ imm = #-0x9090
    c9dc: e3500002     	cmp	r0, #2
    c9e0: 1afffd49     	bne	0xbf0c <tick_processButtonDown+0x210> @ imm = #-0xadc
    c9e4: e2840d76     	add	r0, r4, #7552
    c9e8: ebffdbd8     	bl	0x3950 <.plt+0x254>     @ imm = #-0x90a0
    c9ec: e3500003     	cmp	r0, #3
    c9f0: 1afffd45     	bne	0xbf0c <tick_processButtonDown+0x210> @ imm = #-0xaec
    c9f4: e51fe5a4     	ldr	lr, [pc, #-0x5a4]       @ 0xc458 <tick_processButtonDown+0x75c>
    c9f8: e08f200e     	add	r2, pc, lr
    c9fc: eafffe61     	b	0xc388 <tick_processButtonDown+0x68c> @ imm = #-0x67c
    ca00: e1a00009     	mov	r0, r9
    ca04: ebffdbb3     	bl	0x38d8 <.plt+0x1dc>     @ imm = #-0x9134
    ca08: e3500000     	cmp	r0, #0
    ca0c: 0afffe31     	beq	0xc2d8 <tick_processButtonDown+0x5dc> @ imm = #-0x73c
    ca10: e5d49028     	ldrb	r9, [r4, #0x28]
    ca14: e3590000     	cmp	r9, #0
    ca18: 0afffe2e     	beq	0xc2d8 <tick_processButtonDown+0x5dc> @ imm = #-0x748
    ca1c: e5973ddc     	ldr	r3, [r7, #0xddc]
    ca20: e5976d78     	ldr	r6, [r7, #0xd78]
    ca24: e0431006     	sub	r1, r3, r6
    ca28: e3510001     	cmp	r1, #1
    ca2c: 8afffe29     	bhi	0xc2d8 <tick_processButtonDown+0x5dc> @ imm = #-0x75c
    ca30: e5d4e027     	ldrb	lr, [r4, #0x27]
    ca34: e3a01001     	mov	r1, #1
    ca38: e1a00004     	mov	r0, r4
    ca3c: e35e0000     	cmp	lr, #0
    ca40: 0a000065     	beq	0xcbdc <tick_processButtonDown+0xee0> @ imm = #0x194
    ca44: ebffdbac     	bl	0x38fc <.plt+0x200>     @ imm = #-0x9150
    ca48: eafffe22     	b	0xc2d8 <tick_processButtonDown+0x5dc> @ imm = #-0x778
    ca4c: e1a02001     	mov	r2, r1
    ca50: e3a0a002     	mov	r10, #2
    ca54: e3a01037     	mov	r1, #55
    ca58: e5c7ad5c     	strb	r10, [r7, #0xd5c]
    ca5c: e1a00004     	mov	r0, r4
    ca60: ebffdc86     	bl	0x3c80 <.plt+0x584>     @ imm = #-0x8de8
    ca64: e5d79d5c     	ldrb	r9, [r7, #0xd5c]
    ca68: e5d4103c     	ldrb	r1, [r4, #0x3c]
    ca6c: ee019a90     	vmov	s3, r9
    ca70: eeb81a61     	vcvt.f32.u32	s2, s3
    ca74: eaffffb7     	b	0xc958 <tick_processButtonDown+0xc5c> @ imm = #-0x124
    ca78: e51fe624     	ldr	lr, [pc, #-0x624]       @ 0xc45c <tick_processButtonDown+0x760>
    ca7c: e1a0100a     	mov	r1, r10
    ca80: e08f000e     	add	r0, pc, lr
    ca84: ebffdc3b     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x8f14
    ca88: e1a01007     	mov	r1, r7
    ca8c: e3a02000     	mov	r2, #0
    ca90: e1a00004     	mov	r0, r4
    ca94: ebffdc13     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x8fb4
    ca98: eaffff7d     	b	0xc894 <tick_processButtonDown+0xb98> @ imm = #-0x20c
    ca9c: eeb70a00     	vmov.f32	s0, #1.000000e+00
    caa0: ebffdc0a     	bl	0x3ad0 <.plt+0x3d4>     @ imm = #-0x8fd8
    caa4: e59400d0     	ldr	r0, [r4, #0xd0]
    caa8: eeb20b00     	vmov.f64	d0, #8.000000e+00
    caac: ebffdb9e     	bl	0x392c <.plt+0x230>     @ imm = #-0x9188
    cab0: e51f0658     	ldr	r0, [pc, #-0x658]       @ 0xc460 <tick_processButtonDown+0x764>
    cab4: e08f0000     	add	r0, pc, r0
    cab8: ebffdc2e     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x8f48
    cabc: e5d4103c     	ldrb	r1, [r4, #0x3c]
    cac0: eafffcd9     	b	0xbe2c <tick_processButtonDown+0x130> @ imm = #-0xc9c
    cac4: ebffdc82     	bl	0x3cd4 <.plt+0x5d8>     @ imm = #-0x8df8
    cac8: eafffe16     	b	0xc328 <tick_processButtonDown+0x62c> @ imm = #-0x7a8
    cacc: e3550000     	cmp	r5, #0
    cad0: 0a000060     	beq	0xcc58 <tick_processButtonDown+0xf5c> @ imm = #0x180
    cad4: e5d4e033     	ldrb	lr, [r4, #0x33]
    cad8: e35e00ff     	cmp	lr, #255
    cadc: 0a000002     	beq	0xcaec <tick_processButtonDown+0xdf0> @ imm = #0x8
    cae0: e5d4206a     	ldrb	r2, [r4, #0x6a]
    cae4: e3520000     	cmp	r2, #0
    cae8: 0a00003d     	beq	0xcbe4 <tick_processButtonDown+0xee8> @ imm = #0xf4
    caec: e3a02000     	mov	r2, #0
    caf0: e3a01078     	mov	r1, #120
    caf4: e1a00004     	mov	r0, r4
    caf8: ebffdbfa     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x9018
    cafc: e3a02000     	mov	r2, #0
    cb00: e3a010e7     	mov	r1, #231
    cb04: e1a00004     	mov	r0, r4
    cb08: ebffdbf6     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x9028
    cb0c: e5d46034     	ldrb	r6, [r4, #0x34]
    cb10: e25630ff     	subs	r3, r6, #255
    cb14: 13a03001     	movne	r3, #1
    cb18: e35a00c7     	cmp	r10, #199
    cb1c: 83a03000     	movhi	r3, #0
    cb20: e3530000     	cmp	r3, #0
    cb24: 0afffd34     	beq	0xbffc <tick_processButtonDown+0x300> @ imm = #-0xb30
    cb28: e5d4806a     	ldrb	r8, [r4, #0x6a]
    cb2c: e3580001     	cmp	r8, #1
    cb30: 1afffd31     	bne	0xbffc <tick_processButtonDown+0x300> @ imm = #-0xb3c
    cb34: e30a1aab     	movw	r1, #0xaaab
    cb38: e34a1aaa     	movt	r1, #0xaaaa
    cb3c: e3a0c00a     	mov	r12, #10
    cb40: e3a05006     	mov	r5, #6
    cb44: e0832196     	umull	r2, r3, r6, r1
    cb48: e5d4e032     	ldrb	lr, [r4, #0x32]
    cb4c: e3a02064     	mov	r2, #100
    cb50: e51f96f4     	ldr	r9, [pc, #-0x6f4]       @ 0xc464 <tick_processButtonDown+0x768>
    cb54: e08f0009     	add	r0, pc, r9
    cb58: e1a03123     	lsr	r3, r3, #2
    cb5c: e6ef1073     	uxtb	r1, r3
    cb60: e0666395     	mls	r6, r5, r3, r6
    cb64: e02ccc91     	mla	r12, r1, r12, r12
    cb68: e6ef5076     	uxtb	r5, r6
    cb6c: e2859001     	add	r9, r5, #1
    cb70: e02ec29e     	mla	lr, lr, r2, r12
    cb74: e089600e     	add	r6, r9, lr
    cb78: e1a01006     	mov	r1, r6
    cb7c: ebffdbfd     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x900c
    cb80: e1a02006     	mov	r2, r6
    cb84: e3a01076     	mov	r1, #118
    cb88: e1a00004     	mov	r0, r4
    cb8c: ebffdbd5     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x90ac
    cb90: e5c78de1     	strb	r8, [r7, #0xde1]
    cb94: e5d46033     	ldrb	r6, [r4, #0x33]
    cb98: e35600ff     	cmp	r6, #255
    cb9c: 1afffd1f     	bne	0xc020 <tick_processButtonDown+0x324> @ imm = #-0xb84
    cba0: e5d43034     	ldrb	r3, [r4, #0x34]
    cba4: e35300ff     	cmp	r3, #255
    cba8: 0afffc9e     	beq	0xbe28 <tick_processButtonDown+0x12c> @ imm = #-0xd88
    cbac: e35a00c7     	cmp	r10, #199
    cbb0: 9afffd20     	bls	0xc038 <tick_processButtonDown+0x33c> @ imm = #-0xb80
    cbb4: eafffc9b     	b	0xbe28 <tick_processButtonDown+0x12c> @ imm = #-0xd94
    cbb8: e5d4e033     	ldrb	lr, [r4, #0x33]
    cbbc: e35e00ff     	cmp	lr, #255
    cbc0: 0affffc9     	beq	0xcaec <tick_processButtonDown+0xdf0> @ imm = #-0xdc
    cbc4: e35a00c7     	cmp	r10, #199
    cbc8: 8afffd03     	bhi	0xbfdc <tick_processButtonDown+0x2e0> @ imm = #-0xbf4
    cbcc: eaffffc3     	b	0xcae0 <tick_processButtonDown+0xde4> @ imm = #-0xf4
    cbd0: e1a00004     	mov	r0, r4
    cbd4: ebffdb51     	bl	0x3920 <.plt+0x224>     @ imm = #-0x92bc
    cbd8: eafffdbe     	b	0xc2d8 <tick_processButtonDown+0x5dc> @ imm = #-0x908
    cbdc: ebffdc78     	bl	0x3dc4 <.plt+0x6c8>     @ imm = #-0x8e20
    cbe0: eafffdbc     	b	0xc2d8 <tick_processButtonDown+0x5dc> @ imm = #-0x910
    cbe4: e5d48032     	ldrb	r8, [r4, #0x32]
    cbe8: e3a09064     	mov	r9, #100
    cbec: e3a01078     	mov	r1, #120
    cbf0: e1a00004     	mov	r0, r4
    cbf4: e02c9998     	mla	r12, r8, r9, r9
    cbf8: e08e500c     	add	r5, lr, r12
    cbfc: e1a02005     	mov	r2, r5
    cc00: ebffdbb8     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x9120
    cc04: e51f07a4     	ldr	r0, [pc, #-0x7a4]       @ 0xc468 <tick_processButtonDown+0x76c>
    cc08: e58d5000     	str	r5, [sp]
    cc0c: e08f0000     	add	r0, pc, r0
    cc10: e5d43033     	ldrb	r3, [r4, #0x33]
    cc14: e5d42032     	ldrb	r2, [r4, #0x32]
    cc18: e5d4106a     	ldrb	r1, [r4, #0x6a]
    cc1c: ebffdbd5     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x90ac
    cc20: e3a02001     	mov	r2, #1
    cc24: e3a010e7     	mov	r1, #231
    cc28: e1a00004     	mov	r0, r4
    cc2c: ebffdbad     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x914c
    cc30: e5d46034     	ldrb	r6, [r4, #0x34]
    cc34: e35600ff     	cmp	r6, #255
    cc38: 1affffba     	bne	0xcb28 <tick_processButtonDown+0xe2c> @ imm = #-0x118
    cc3c: eafffcee     	b	0xbffc <tick_processButtonDown+0x300> @ imm = #-0xc48
    cc40: e51f27dc     	ldr	r2, [pc, #-0x7dc]       @ 0xc46c <tick_processButtonDown+0x770>
    cc44: e3a08001     	mov	r8, #1
    cc48: e08f0002     	add	r0, pc, r2
    cc4c: ebffdbc9     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x90dc
    cc50: e5c48035     	strb	r8, [r4, #0x35]
    cc54: eafffce0     	b	0xbfdc <tick_processButtonDown+0x2e0> @ imm = #-0xc80
    cc58: e30a3aab     	movw	r3, #0xaaab
    cc5c: e34a3aaa     	movt	r3, #0xaaaa
    cc60: e5c45035     	strb	r5, [r4, #0x35]
    cc64: e3a0000a     	mov	r0, #10
    cc68: e0898392     	umull	r8, r9, r2, r3
    cc6c: e3a06006     	mov	r6, #6
    cc70: e5d4c032     	ldrb	r12, [r4, #0x32]
    cc74: e3a0e064     	mov	lr, #100
    cc78: e1a0100c     	mov	r1, r12
    cc7c: e1a09129     	lsr	r9, r9, #2
    cc80: e6ef5079     	uxtb	r5, r9
    cc84: e0632996     	mls	r3, r6, r9, r2
    cc88: e0280095     	mla	r8, r5, r0, r0
    cc8c: e51f0824     	ldr	r0, [pc, #-0x824]       @ 0xc470 <tick_processButtonDown+0x774>
    cc90: e08f0000     	add	r0, pc, r0
    cc94: e6ef6073     	uxtb	r6, r3
    cc98: e2869001     	add	r9, r6, #1
    cc9c: e02c8e9c     	mla	r12, r12, lr, r8
    cca0: e089500c     	add	r5, r9, r12
    cca4: e1a03005     	mov	r3, r5
    cca8: ebffdbb2     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x9138
    ccac: e3a0107a     	mov	r1, #122
    ccb0: e1a02005     	mov	r2, r5
    ccb4: e1a00004     	mov	r0, r4
    ccb8: ebffdb8a     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x91d8
    ccbc: e3a01001     	mov	r1, #1
    ccc0: e5c71de1     	strb	r1, [r7, #0xde1]
    ccc4: eaffff82     	b	0xcad4 <tick_processButtonDown+0xdd8> @ imm = #-0x1f8
    ccc8: e5d46033     	ldrb	r6, [r4, #0x33]
    cccc: e25610ff     	subs	r1, r6, #255
    ccd0: 13a01001     	movne	r1, #1
    ccd4: e35a00c7     	cmp	r10, #199
    ccd8: 83a01000     	movhi	r1, #0
    ccdc: e3510000     	cmp	r1, #0
    cce0: 1afffcf3     	bne	0xc0b4 <tick_processButtonDown+0x3b8> @ imm = #-0xc34
    cce4: eafffc4f     	b	0xbe28 <tick_processButtonDown+0x12c> @ imm = #-0xec4
    cce8: e5d42032     	ldrb	r2, [r4, #0x32]
    ccec: e3a0c064     	mov	r12, #100
    ccf0: e1a00004     	mov	r0, r4
    ccf4: e3a0106b     	mov	r1, #107
    ccf8: e025cc92     	mla	r5, r2, r12, r12
    ccfc: e0869005     	add	r9, r6, r5
    cd00: e1a02009     	mov	r2, r9
    cd04: ebffdb77     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x9224
    cd08: e51f089c     	ldr	r0, [pc, #-0x89c]       @ 0xc474 <tick_processButtonDown+0x778>
    cd0c: e5c78de1     	strb	r8, [r7, #0xde1]
    cd10: e1a03009     	mov	r3, r9
    cd14: e08f0000     	add	r0, pc, r0
    cd18: e5d42033     	ldrb	r2, [r4, #0x33]
    cd1c: e5d4106a     	ldrb	r1, [r4, #0x6a]
    cd20: ebffdb94     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x91b0
    cd24: eafffcc0     	b	0xc02c <tick_processButtonDown+0x330> @ imm = #-0xd00

