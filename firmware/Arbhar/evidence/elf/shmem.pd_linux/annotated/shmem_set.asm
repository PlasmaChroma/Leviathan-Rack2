00000ea0 <shmem_set>:
     ea0: e5901024     	ldr	r1, [r0, #0x24]
     ea4: e3510000     	cmp	r1, #0
     ea8: 0a0000d9     	beq	0x1214 <shmem_set+0x374> @ imm = #0x364
     eac: e3520000     	cmp	r2, #0
     eb0: da0000d4     	ble	0x1208 <shmem_set+0x368> @ imm = #0x350
     eb4: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
     eb8: e1a04003     	mov	r4, r3
     ebc: e2123003     	ands	r3, r2, #3
     ec0: e24dd014     	sub	sp, sp, #20
     ec4: e1a05000     	mov	r5, r0
     ec8: e1a07002     	mov	r7, r2
     ecc: e3a06000     	mov	r6, #0
     ed0: 0a000032     	beq	0xfa0 <shmem_set+0x100> @ imm = #0xc8
     ed4: e3530001     	cmp	r3, #1
     ed8: 0a00001f     	beq	0xf5c <shmem_set+0xbc>  @ imm = #0x7c
     edc: e3530002     	cmp	r3, #2
     ee0: 0a00000e     	beq	0xf20 <shmem_set+0x80>  @ imm = #0x38
     ee4: e5940000     	ldr	r0, [r4]
     ee8: e3500002     	cmp	r0, #2
     eec: 1a00005f     	bne	0x1070 <shmem_set+0x1d0> @ imm = #0x17c
     ef0: e1a02004     	mov	r2, r4
     ef4: e1a01007     	mov	r1, r7
     ef8: e1a00006     	mov	r0, r6
     efc: ebfffe59     	bl	0x868 <.plt+0xf8>       @ imm = #-0x69c  // CALL atom_getsymbolarg
     f00: e5958028     	ldr	r8, [r5, #0x28]
     f04: e1a03006     	mov	r3, r6
     f08: e1a02006     	mov	r2, r6
     f0c: e3a06001     	mov	r6, #1
     f10: e58d8000     	str	r8, [sp]
     f14: e1a01000     	mov	r1, r0
     f18: e1a00005     	mov	r0, r5
     f1c: ebfffe48     	bl	0x844 <.plt+0xd4>       @ imm = #-0x6e0  // CALL shmem_set_tab
     f20: e7942186     	ldr	r2, [r4, r6, lsl #3]
     f24: e3520002     	cmp	r2, #2
     f28: 1a000050     	bne	0x1070 <shmem_set+0x1d0> @ imm = #0x140
     f2c: e1a02004     	mov	r2, r4
     f30: e1a01007     	mov	r1, r7
     f34: e1a00006     	mov	r0, r6
     f38: e2866001     	add	r6, r6, #1
     f3c: ebfffe49     	bl	0x868 <.plt+0xf8>       @ imm = #-0x6dc  // CALL atom_getsymbolarg
     f40: e5959028     	ldr	r9, [r5, #0x28]
     f44: e3a03000     	mov	r3, #0
     f48: e1a02003     	mov	r2, r3
     f4c: e58d9000     	str	r9, [sp]
     f50: e1a01000     	mov	r1, r0
     f54: e1a00005     	mov	r0, r5
     f58: ebfffe39     	bl	0x844 <.plt+0xd4>       @ imm = #-0x71c  // CALL shmem_set_tab
     f5c: e794a186     	ldr	r10, [r4, r6, lsl #3]
     f60: e35a0002     	cmp	r10, #2
     f64: 1a000041     	bne	0x1070 <shmem_set+0x1d0> @ imm = #0x104
     f68: e1a00006     	mov	r0, r6
     f6c: e2866001     	add	r6, r6, #1
     f70: e1a02004     	mov	r2, r4
     f74: e1a01007     	mov	r1, r7
     f78: ebfffe3a     	bl	0x868 <.plt+0xf8>       @ imm = #-0x718  // CALL atom_getsymbolarg
     f7c: e595b028     	ldr	r11, [r5, #0x28]
     f80: e3a03000     	mov	r3, #0
     f84: e1a02003     	mov	r2, r3
     f88: e58db000     	str	r11, [sp]
     f8c: e1a01000     	mov	r1, r0
     f90: e1a00005     	mov	r0, r5
     f94: ebfffe2a     	bl	0x844 <.plt+0xd4>       @ imm = #-0x758  // CALL shmem_set_tab
     f98: e1570006     	cmp	r7, r6
     f9c: 0a000033     	beq	0x1070 <shmem_set+0x1d0> @ imm = #0xcc
     fa0: e794c186     	ldr	r12, [r4, r6, lsl #3]
     fa4: e1a02004     	mov	r2, r4
     fa8: e1a01007     	mov	r1, r7
     fac: e1a00006     	mov	r0, r6
     fb0: e35c0002     	cmp	r12, #2
     fb4: e2868001     	add	r8, r6, #1
     fb8: 1a00002c     	bne	0x1070 <shmem_set+0x1d0> @ imm = #0xb0
     fbc: ebfffe29     	bl	0x868 <.plt+0xf8>       @ imm = #-0x75c  // CALL atom_getsymbolarg
     fc0: e5951028     	ldr	r1, [r5, #0x28]
     fc4: e3a03000     	mov	r3, #0
     fc8: e1a02003     	mov	r2, r3
     fcc: e58d1000     	str	r1, [sp]
     fd0: e1a01000     	mov	r1, r0
     fd4: e1a00005     	mov	r0, r5
     fd8: ebfffe19     	bl	0x844 <.plt+0xd4>       @ imm = #-0x79c  // CALL shmem_set_tab
     fdc: e7943188     	ldr	r3, [r4, r8, lsl #3]
     fe0: e1a02004     	mov	r2, r4
     fe4: e1a01007     	mov	r1, r7
     fe8: e3530002     	cmp	r3, #2
     fec: e1a00008     	mov	r0, r8
     ff0: 1a00001e     	bne	0x1070 <shmem_set+0x1d0> @ imm = #0x78
     ff4: ebfffe1b     	bl	0x868 <.plt+0xf8>       @ imm = #-0x794  // CALL atom_getsymbolarg
     ff8: e5959028     	ldr	r9, [r5, #0x28]
     ffc: e3a03000     	mov	r3, #0
    1000: e288a001     	add	r10, r8, #1
    1004: e1a02003     	mov	r2, r3
    1008: e58d9000     	str	r9, [sp]
    100c: e1a01000     	mov	r1, r0
    1010: e1a00005     	mov	r0, r5
    1014: ebfffe0a     	bl	0x844 <.plt+0xd4>       @ imm = #-0x7d8  // CALL shmem_set_tab
    1018: e794b18a     	ldr	r11, [r4, r10, lsl #3]
    101c: e1a02004     	mov	r2, r4
    1020: e1a01007     	mov	r1, r7
    1024: e35b0002     	cmp	r11, #2
    1028: e1a0000a     	mov	r0, r10
    102c: 1a00000f     	bne	0x1070 <shmem_set+0x1d0> @ imm = #0x3c
    1030: ebfffe0c     	bl	0x868 <.plt+0xf8>       @ imm = #-0x7d0  // CALL atom_getsymbolarg
    1034: e5958028     	ldr	r8, [r5, #0x28]
    1038: e3a03000     	mov	r3, #0
    103c: e1a02003     	mov	r2, r3
    1040: e58d8000     	str	r8, [sp]
    1044: e1a01000     	mov	r1, r0
    1048: e1a00005     	mov	r0, r5
    104c: ebfffdfc     	bl	0x844 <.plt+0xd4>       @ imm = #-0x810  // CALL shmem_set_tab
    1050: e286c003     	add	r12, r6, #3
    1054: e1a02004     	mov	r2, r4
    1058: e1a01007     	mov	r1, r7
    105c: e794318c     	ldr	r3, [r4, r12, lsl #3]
    1060: e1a0000c     	mov	r0, r12
    1064: e2866004     	add	r6, r6, #4
    1068: e3530002     	cmp	r3, #2
    106c: 0affffc1     	beq	0xf78 <shmem_set+0xd8>  @ imm = #-0xfc
    1070: e3570001     	cmp	r7, #1
    1074: 0a000002     	beq	0x1084 <shmem_set+0x1e4> @ imm = #0x8
    1078: e5940000     	ldr	r0, [r4]
    107c: e3500001     	cmp	r0, #1
    1080: 0a000001     	beq	0x108c <shmem_set+0x1ec> @ imm = #0x4
    1084: e28dd014     	add	sp, sp, #20
    1088: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
    108c: e1a02004     	mov	r2, r4
    1090: e3a00000     	mov	r0, #0
    1094: e1a01007     	mov	r1, r7
    1098: ebfffdf8     	bl	0x880 <.plt+0x110>      @ imm = #-0x820  // CALL atom_getfloatarg
    109c: e5940008     	ldr	r0, [r4, #0x8]
    10a0: e3500002     	cmp	r0, #2
    10a4: eebd0ac0     	vcvt.s32.f32	s0, s0
    10a8: ee102a10     	vmov	r2, s0
    10ac: e1c2afc2     	bic	r10, r2, r2, asr #31
    10b0: 0a00005a     	beq	0x1220 <shmem_set+0x380> @ imm = #0x168
    10b4: e3500001     	cmp	r0, #1
    10b8: 1afffff1     	bne	0x1084 <shmem_set+0x1e4> @ imm = #-0x3c
    10bc: e595e028     	ldr	lr, [r5, #0x28]
    10c0: e04e900a     	sub	r9, lr, r10
    10c4: e1570009     	cmp	r7, r9
    10c8: d2479001     	suble	r9, r7, #1
    10cc: e3590000     	cmp	r9, #0
    10d0: daffffeb     	ble	0x1084 <shmem_set+0x1e4> @ imm = #-0x54
    10d4: e2191003     	ands	r1, r9, #3
    10d8: e1a0610a     	lsl	r6, r10, #2
    10dc: e3a08000     	mov	r8, #0
    10e0: 0a000020     	beq	0x1168 <shmem_set+0x2c8> @ imm = #0x80
    10e4: e3510001     	cmp	r1, #1
    10e8: 0a000013     	beq	0x113c <shmem_set+0x29c> @ imm = #0x4c
    10ec: e3510002     	cmp	r1, #2
    10f0: 0a000008     	beq	0x1118 <shmem_set+0x278> @ imm = #0x20
    10f4: e3a08001     	mov	r8, #1
    10f8: e1a02004     	mov	r2, r4
    10fc: e1a00008     	mov	r0, r8
    1100: e1a01007     	mov	r1, r7
    1104: e595b024     	ldr	r11, [r5, #0x24]
    1108: ebfffddc     	bl	0x880 <.plt+0x110>      @ imm = #-0x890  // CALL atom_getfloatarg
    110c: e08ba006     	add	r10, r11, r6
    1110: e2866004     	add	r6, r6, #4
    1114: ed8a0a00     	vstr	s0, [r10]
    1118: e2888001     	add	r8, r8, #1
    111c: e595c024     	ldr	r12, [r5, #0x24]
    1120: e1a02004     	mov	r2, r4
    1124: e1a01007     	mov	r1, r7
    1128: e1a00008     	mov	r0, r8
    112c: e08cb006     	add	r11, r12, r6
    1130: ebfffdd2     	bl	0x880 <.plt+0x110>      @ imm = #-0x8b8  // CALL atom_getfloatarg
    1134: e2866004     	add	r6, r6, #4
    1138: ed8b0a00     	vstr	s0, [r11]
    113c: e2888001     	add	r8, r8, #1
    1140: e5953024     	ldr	r3, [r5, #0x24]
    1144: e1a02004     	mov	r2, r4
    1148: e1a01007     	mov	r1, r7
    114c: e1a00008     	mov	r0, r8
    1150: e083a006     	add	r10, r3, r6
    1154: ebfffdc9     	bl	0x880 <.plt+0x110>      @ imm = #-0x8dc  // CALL atom_getfloatarg
    1158: e1580009     	cmp	r8, r9
    115c: e2866004     	add	r6, r6, #4
    1160: ed8a0a00     	vstr	s0, [r10]
    1164: 0affffc6     	beq	0x1084 <shmem_set+0x1e4> @ imm = #-0xe8
    1168: e595e024     	ldr	lr, [r5, #0x24]
    116c: e2880001     	add	r0, r8, #1
    1170: e1a02004     	mov	r2, r4
    1174: e1a01007     	mov	r1, r7
    1178: e08ec006     	add	r12, lr, r6
    117c: e58dc00c     	str	r12, [sp, #0xc]
    1180: ebfffdbe     	bl	0x880 <.plt+0x110>      @ imm = #-0x908  // CALL atom_getfloatarg
    1184: e59dc00c     	ldr	r12, [sp, #0xc]
    1188: e595b024     	ldr	r11, [r5, #0x24]
    118c: e2863004     	add	r3, r6, #4
    1190: e2880002     	add	r0, r8, #2
    1194: e1a02004     	mov	r2, r4
    1198: e1a01007     	mov	r1, r7
    119c: e08ba003     	add	r10, r11, r3
    11a0: ed8c0a00     	vstr	s0, [r12]
    11a4: ebfffdb5     	bl	0x880 <.plt+0x110>      @ imm = #-0x92c  // CALL atom_getfloatarg
    11a8: e595b024     	ldr	r11, [r5, #0x24]
    11ac: e2863008     	add	r3, r6, #8
    11b0: e2880003     	add	r0, r8, #3
    11b4: e1a02004     	mov	r2, r4
    11b8: e1a01007     	mov	r1, r7
    11bc: e08bc003     	add	r12, r11, r3
    11c0: e58dc00c     	str	r12, [sp, #0xc]
    11c4: e2888004     	add	r8, r8, #4
    11c8: e286b00c     	add	r11, r6, #12
    11cc: e2866010     	add	r6, r6, #16
    11d0: ed8a0a00     	vstr	s0, [r10]
    11d4: ebfffda9     	bl	0x880 <.plt+0x110>      @ imm = #-0x95c  // CALL atom_getfloatarg
    11d8: e59d300c     	ldr	r3, [sp, #0xc]
    11dc: e595a024     	ldr	r10, [r5, #0x24]
    11e0: e1a00008     	mov	r0, r8
    11e4: e1a02004     	mov	r2, r4
    11e8: e1a01007     	mov	r1, r7
    11ec: e08aa00b     	add	r10, r10, r11
    11f0: ed830a00     	vstr	s0, [r3]
    11f4: ebfffda1     	bl	0x880 <.plt+0x110>      @ imm = #-0x97c  // CALL atom_getfloatarg
    11f8: e1580009     	cmp	r8, r9
    11fc: ed8a0a00     	vstr	s0, [r10]
    1200: 1affffd8     	bne	0x1168 <shmem_set+0x2c8> @ imm = #-0xa0
    1204: eaffff9e     	b	0x1084 <shmem_set+0x1e4> @ imm = #-0x188
    1208: e59fc0a0     	ldr	r12, [pc, #0xa0]        @ 0x12b0 <shmem_set+0x410>  // u32=0x6c0; f32?=2.42144375e-42
    120c: e08f100c     	add	r1, pc, r12
    1210: eafffd9d     	b	0x88c <.plt+0x11c>      @ imm = #-0x98c  // CALL pd_error
    1214: e59f0098     	ldr	r0, [pc, #0x98]         @ 0x12b4 <shmem_set+0x414>  // u32=0x5d4; f32?=2.09073731e-42
    1218: e08f0000     	add	r0, pc, r0
    121c: eafffd6d     	b	0x7d8 <.plt+0x68>       @ imm = #-0xa4c  // CALL error
    1220: e3570002     	cmp	r7, #2
    1224: e5959028     	ldr	r9, [r5, #0x28]
    1228: 0a000002     	beq	0x1238 <shmem_set+0x398> @ imm = #0x8
    122c: e5942010     	ldr	r2, [r4, #0x10]
    1230: e3520001     	cmp	r2, #1
    1234: 0a00000b     	beq	0x1268 <shmem_set+0x3c8> @ imm = #0x2c
    1238: e3a08000     	mov	r8, #0
    123c: e1a02004     	mov	r2, r4
    1240: e1a01007     	mov	r1, r7
    1244: e3a00001     	mov	r0, #1
    1248: ebfffd86     	bl	0x868 <.plt+0xf8>       @ imm = #-0x9e8  // CALL atom_getsymbolarg
    124c: e58d9000     	str	r9, [sp]
    1250: e1a0300a     	mov	r3, r10
    1254: e1a02008     	mov	r2, r8
    1258: e1a01000     	mov	r1, r0
    125c: e1a00005     	mov	r0, r5
    1260: ebfffd77     	bl	0x844 <.plt+0xd4>       @ imm = #-0xa24  // CALL shmem_set_tab
    1264: eaffff86     	b	0x1084 <shmem_set+0x1e4> @ imm = #-0x1e8
    1268: e1a01007     	mov	r1, r7
    126c: e1a02004     	mov	r2, r4
    1270: ebfffd82     	bl	0x880 <.plt+0x110>      @ imm = #-0x9f8  // CALL atom_getfloatarg
    1274: e3570003     	cmp	r7, #3
    1278: eefd0ac0     	vcvt.s32.f32	s1, s0
    127c: ee101a90     	vmov	r1, s1
    1280: e1c18fc1     	bic	r8, r1, r1, asr #31
    1284: 0affffec     	beq	0x123c <shmem_set+0x39c> @ imm = #-0x50
    1288: e5946010     	ldr	r6, [r4, #0x10]
    128c: e3560001     	cmp	r6, #1
    1290: 1affffe9     	bne	0x123c <shmem_set+0x39c> @ imm = #-0x5c
    1294: e1a02004     	mov	r2, r4
    1298: e1a01007     	mov	r1, r7
    129c: e3a00003     	mov	r0, #3
    12a0: ebfffd76     	bl	0x880 <.plt+0x110>      @ imm = #-0xa28  // CALL atom_getfloatarg
    12a4: eefd7ac0     	vcvt.s32.f32	s15, s0
    12a8: ee179a90     	vmov	r9, s15
    12ac: eaffffe2     	b	0x123c <shmem_set+0x39c> @ imm = #-0x78
    12b0: c0 06 00 00  	.word	0x000006c0
    12b4: d4 05 00 00  	.word	0x000005d4

