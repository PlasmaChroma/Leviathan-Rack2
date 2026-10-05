00000cc0 <shell_read>:
     cc0: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
     cc4: e24dde81     	sub	sp, sp, #2064
     cc8: e24dd00c     	sub	sp, sp, #12
     ccc: e1a05001     	mov	r5, r1
     cd0: e1a0b000     	mov	r11, r0
     cd4: ebffff4d     	bl	0xa10 <.plt+0xa4>       @ imm = #-0x2cc  // CALL binbuf_new
     cd8: e28d4018     	add	r4, sp, #24
     cdc: e30027ff     	movw	r2, #0x7ff
     ce0: e1a01004     	mov	r1, r4
     ce4: e1a08000     	mov	r8, r0
     ce8: e1a00005     	mov	r0, r5
     cec: ebffff2f     	bl	0x9b0 <.plt+0x44>       @ imm = #-0x344  // CALL read
     cf0: e28d3e81     	add	r3, sp, #2064
     cf4: e2831008     	add	r1, r3, #8
     cf8: e3a02000     	mov	r2, #0
     cfc: e0816000     	add	r6, r1, r0
     d00: e3500000     	cmp	r0, #0
     d04: e5462800     	strb	r2, [r6, #-0x800]
     d08: da000156     	ble	0x1268 <shell_read+0x5a8> @ imm = #0x558
     d0c: e210a007     	ands	r10, r0, #7
     d10: e084e000     	add	lr, r4, r0
     d14: e1a03004     	mov	r3, r4
     d18: e3a0503b     	mov	r5, #59
     d1c: 0a00001f     	beq	0xda0 <shell_read+0xe0> @ imm = #0x7c
     d20: e35a0001     	cmp	r10, #1
     d24: 0a000018     	beq	0xd8c <shell_read+0xcc> @ imm = #0x60
     d28: e35a0002     	cmp	r10, #2
     d2c: 0a000013     	beq	0xd80 <shell_read+0xc0> @ imm = #0x4c
     d30: e35a0003     	cmp	r10, #3
     d34: 0a00000e     	beq	0xd74 <shell_read+0xb4> @ imm = #0x38
     d38: e35a0004     	cmp	r10, #4
     d3c: 0a000009     	beq	0xd68 <shell_read+0xa8> @ imm = #0x24
     d40: e35a0005     	cmp	r10, #5
     d44: 0a000004     	beq	0xd5c <shell_read+0x9c> @ imm = #0x10
     d48: e35a0006     	cmp	r10, #6
     d4c: 1a000159     	bne	0x12b8 <shell_read+0x5f8> @ imm = #0x564
     d50: e4d31001     	ldrb	r1, [r3], #1
     d54: e351000a     	cmp	r1, #10
     d58: 05435001     	strbeq	r5, [r3, #-0x1]
     d5c: e4d36001     	ldrb	r6, [r3], #1
     d60: e356000a     	cmp	r6, #10
     d64: 05435001     	strbeq	r5, [r3, #-0x1]
     d68: e4d32001     	ldrb	r2, [r3], #1
     d6c: e352000a     	cmp	r2, #10
     d70: 05435001     	strbeq	r5, [r3, #-0x1]
     d74: e4d30001     	ldrb	r0, [r3], #1
     d78: e350000a     	cmp	r0, #10
     d7c: 05435001     	strbeq	r5, [r3, #-0x1]
     d80: e4d37001     	ldrb	r7, [r3], #1
     d84: e357000a     	cmp	r7, #10
     d88: 05435001     	strbeq	r5, [r3, #-0x1]
     d8c: e4d39001     	ldrb	r9, [r3], #1
     d90: e359000a     	cmp	r9, #10
     d94: 0a00001b     	beq	0xe08 <shell_read+0x148> @ imm = #0x6c
     d98: e15e0003     	cmp	lr, r3
     d9c: 0a00001c     	beq	0xe14 <shell_read+0x154> @ imm = #0x70
     da0: e4d39001     	ldrb	r9, [r3], #1
     da4: e359000a     	cmp	r9, #10
     da8: e1a02003     	mov	r2, r3
     dac: 05435001     	strbeq	r5, [r3, #-0x1]
     db0: e4d2a001     	ldrb	r10, [r2], #1
     db4: e35a000a     	cmp	r10, #10
     db8: 05425001     	strbeq	r5, [r2, #-0x1]
     dbc: e5d3c001     	ldrb	r12, [r3, #0x1]
     dc0: e2833007     	add	r3, r3, #7
     dc4: e5530005     	ldrb	r0, [r3, #-0x5]
     dc8: e5536004     	ldrb	r6, [r3, #-0x4]
     dcc: e35c000a     	cmp	r12, #10
     dd0: e5531003     	ldrb	r1, [r3, #-0x3]
     dd4: e5537002     	ldrb	r7, [r3, #-0x2]
     dd8: e5539001     	ldrb	r9, [r3, #-0x1]
     ddc: 05435006     	strbeq	r5, [r3, #-0x6]
     de0: e350000a     	cmp	r0, #10
     de4: 05435005     	strbeq	r5, [r3, #-0x5]
     de8: e356000a     	cmp	r6, #10
     dec: 05435004     	strbeq	r5, [r3, #-0x4]
     df0: e351000a     	cmp	r1, #10
     df4: 05435003     	strbeq	r5, [r3, #-0x3]
     df8: e357000a     	cmp	r7, #10
     dfc: 05435002     	strbeq	r5, [r3, #-0x2]
     e00: e359000a     	cmp	r9, #10
     e04: 1affffe3     	bne	0xd98 <shell_read+0xd8> @ imm = #-0x74
     e08: e15e0003     	cmp	lr, r3
     e0c: e5435001     	strb	r5, [r3, #-0x1]
     e10: 1affffe2     	bne	0xda0 <shell_read+0xe0> @ imm = #-0x78
     e14: e1a00004     	mov	r0, r4
     e18: ebffff23     	bl	0xaac <.plt+0x140>      @ imm = #-0x374  // CALL strlen
     e1c: e1a01004     	mov	r1, r4
     e20: e1a02000     	mov	r2, r0
     e24: e1a00008     	mov	r0, r8
     e28: ebffff3d     	bl	0xb24 <.plt+0x1b8>      @ imm = #-0x30c  // CALL binbuf_text
     e2c: e1a00008     	mov	r0, r8
     e30: ebffff4a     	bl	0xb60 <.plt+0x1f4>      @ imm = #-0x2d8  // CALL binbuf_getnatom
     e34: e1a00008     	mov	r0, r8
     e38: ebfffed9     	bl	0x9a4 <.plt+0x38>       @ imm = #-0x49c  // CALL binbuf_getvec
     e3c: e1a00008     	mov	r0, r8
     e40: ebffff46     	bl	0xb60 <.plt+0x1f4>      @ imm = #-0x2e8  // CALL binbuf_getnatom
     e44: e1a09000     	mov	r9, r0
     e48: e1a00008     	mov	r0, r8
     e4c: ebfffed4     	bl	0x9a4 <.plt+0x38>       @ imm = #-0x4b0  // CALL binbuf_getvec
     e50: e3590000     	cmp	r9, #0
     e54: e1a05000     	mov	r5, r0
     e58: da000047     	ble	0xf7c <shell_read+0x2bc> @ imm = #0x11c
     e5c: e59f4484     	ldr	r4, [pc, #0x484]        @ 0x12e8 <shell_read+0x628>  // u32=0xbfc; f32?=4.29918369e-42
     e60: e3a0a000     	mov	r10, #0
     e64: e58d8014     	str	r8, [sp, #0x14]
     e68: e08f3004     	add	r3, pc, r4
     e6c: e58db004     	str	r11, [sp, #0x4]
     e70: e58d3010     	str	r3, [sp, #0x10]
     e74: e1e0b00a     	mvn	r11, r10
     e78: e08b8009     	add	r8, r11, r9
     e7c: e218e007     	ands	lr, r8, #7
     e80: e1a0200a     	mov	r2, r10
     e84: 0a00006e     	beq	0x1044 <shell_read+0x384> @ imm = #0x1b8
     e88: e795c18a     	ldr	r12, [r5, r10, lsl #3]
     e8c: e28a4001     	add	r4, r10, #1
     e90: e24c1004     	sub	r1, r12, #4
     e94: e3510001     	cmp	r1, #1
     e98: 9a000031     	bls	0xf64 <shell_read+0x2a4> @ imm = #0xc4
     e9c: e1590004     	cmp	r9, r4
     ea0: da00006e     	ble	0x1060 <shell_read+0x3a0> @ imm = #0x1b8
     ea4: e35e0001     	cmp	lr, #1
     ea8: e1a02004     	mov	r2, r4
     eac: 0a000064     	beq	0x1044 <shell_read+0x384> @ imm = #0x190
     eb0: e35e0002     	cmp	lr, #2
     eb4: 0a000025     	beq	0xf50 <shell_read+0x290> @ imm = #0x94
     eb8: e35e0003     	cmp	lr, #3
     ebc: 0a00001d     	beq	0xf38 <shell_read+0x278> @ imm = #0x74
     ec0: e35e0004     	cmp	lr, #4
     ec4: 0a000015     	beq	0xf20 <shell_read+0x260> @ imm = #0x54
     ec8: e35e0005     	cmp	lr, #5
     ecc: 0a00000d     	beq	0xf08 <shell_read+0x248> @ imm = #0x34
     ed0: e35e0006     	cmp	lr, #6
     ed4: 0a000005     	beq	0xef0 <shell_read+0x230> @ imm = #0x14
     ed8: e7956184     	ldr	r6, [r5, r4, lsl #3]
     edc: e2844001     	add	r4, r4, #1
     ee0: e2460004     	sub	r0, r6, #4
     ee4: e3500001     	cmp	r0, #1
     ee8: 9a00001d     	bls	0xf64 <shell_read+0x2a4> @ imm = #0x74
     eec: e1a02004     	mov	r2, r4
     ef0: e7957182     	ldr	r7, [r5, r2, lsl #3]
     ef4: e2844001     	add	r4, r4, #1
     ef8: e2473004     	sub	r3, r7, #4
     efc: e3530001     	cmp	r3, #1
     f00: 9a000017     	bls	0xf64 <shell_read+0x2a4> @ imm = #0x5c
     f04: e1a02004     	mov	r2, r4
     f08: e795b182     	ldr	r11, [r5, r2, lsl #3]
     f0c: e2844001     	add	r4, r4, #1
     f10: e24b8004     	sub	r8, r11, #4
     f14: e3580001     	cmp	r8, #1
     f18: 9a000011     	bls	0xf64 <shell_read+0x2a4> @ imm = #0x44
     f1c: e1a02004     	mov	r2, r4
     f20: e795e182     	ldr	lr, [r5, r2, lsl #3]
     f24: e2844001     	add	r4, r4, #1
     f28: e24ec004     	sub	r12, lr, #4
     f2c: e35c0001     	cmp	r12, #1
     f30: 9a00000b     	bls	0xf64 <shell_read+0x2a4> @ imm = #0x2c
     f34: e1a02004     	mov	r2, r4
     f38: e7951182     	ldr	r1, [r5, r2, lsl #3]
     f3c: e2844001     	add	r4, r4, #1
     f40: e2416004     	sub	r6, r1, #4
     f44: e3560001     	cmp	r6, #1
     f48: 9a000005     	bls	0xf64 <shell_read+0x2a4> @ imm = #0x14
     f4c: e1a02004     	mov	r2, r4
     f50: e7950182     	ldr	r0, [r5, r2, lsl #3]
     f54: e2844001     	add	r4, r4, #1
     f58: e2407004     	sub	r7, r0, #4
     f5c: e3570001     	cmp	r7, #1
     f60: 8a000036     	bhi	0x1040 <shell_read+0x380> @ imm = #0xd8
     f64: e152000a     	cmp	r2, r10
     f68: ca000041     	bgt	0x1074 <shell_read+0x3b4> @ imm = #0x104
     f6c: e1590004     	cmp	r9, r4
     f70: e1a0a004     	mov	r10, r4
     f74: caffffbe     	bgt	0xe74 <shell_read+0x1b4> @ imm = #-0x108
     f78: e59d8014     	ldr	r8, [sp, #0x14]
     f7c: e1a00008     	mov	r0, r8
     f80: ebfffec6     	bl	0xaa0 <.plt+0x134>      @ imm = #-0x4e8  // CALL binbuf_free
     f84: e28dde81     	add	sp, sp, #2064
     f88: e28dd00c     	add	sp, sp, #12
     f8c: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
     f90: e1590004     	cmp	r9, r4
     f94: da000031     	ble	0x1060 <shell_read+0x3a0> @ imm = #0xc4
     f98: e1a02004     	mov	r2, r4
     f9c: e2844001     	add	r4, r4, #1
     fa0: e795c182     	ldr	r12, [r5, r2, lsl #3]
     fa4: e24c1004     	sub	r1, r12, #4
     fa8: e3510001     	cmp	r1, #1
     fac: 9affffec     	bls	0xf64 <shell_read+0x2a4> @ imm = #-0x50
     fb0: e1a02004     	mov	r2, r4
     fb4: e2844001     	add	r4, r4, #1
     fb8: e7956182     	ldr	r6, [r5, r2, lsl #3]
     fbc: e2460004     	sub	r0, r6, #4
     fc0: e3500001     	cmp	r0, #1
     fc4: 9affffe6     	bls	0xf64 <shell_read+0x2a4> @ imm = #-0x68
     fc8: e2882002     	add	r2, r8, #2
     fcc: e2884003     	add	r4, r8, #3
     fd0: e7957182     	ldr	r7, [r5, r2, lsl #3]
     fd4: e2473004     	sub	r3, r7, #4
     fd8: e3530001     	cmp	r3, #1
     fdc: 9affffe0     	bls	0xf64 <shell_read+0x2a4> @ imm = #-0x80
     fe0: e1a02004     	mov	r2, r4
     fe4: e2884004     	add	r4, r8, #4
     fe8: e795b182     	ldr	r11, [r5, r2, lsl #3]
     fec: e24be004     	sub	lr, r11, #4
     ff0: e35e0001     	cmp	lr, #1
     ff4: 9affffda     	bls	0xf64 <shell_read+0x2a4> @ imm = #-0x98
     ff8: e1a02004     	mov	r2, r4
     ffc: e2884005     	add	r4, r8, #5
    1000: e795c182     	ldr	r12, [r5, r2, lsl #3]
    1004: e24c1004     	sub	r1, r12, #4
    1008: e3510001     	cmp	r1, #1
    100c: 9affffd4     	bls	0xf64 <shell_read+0x2a4> @ imm = #-0xb0
    1010: e1a02004     	mov	r2, r4
    1014: e2884006     	add	r4, r8, #6
    1018: e7956182     	ldr	r6, [r5, r2, lsl #3]
    101c: e2460004     	sub	r0, r6, #4
    1020: e3500001     	cmp	r0, #1
    1024: 9affffce     	bls	0xf64 <shell_read+0x2a4> @ imm = #-0xc8
    1028: e1a02004     	mov	r2, r4
    102c: e2884007     	add	r4, r8, #7
    1030: e7958182     	ldr	r8, [r5, r2, lsl #3]
    1034: e2487004     	sub	r7, r8, #4
    1038: e3570001     	cmp	r7, #1
    103c: 9affffc8     	bls	0xf64 <shell_read+0x2a4> @ imm = #-0xe0
    1040: e1a02004     	mov	r2, r4
    1044: e7953182     	ldr	r3, [r5, r2, lsl #3]
    1048: e2824001     	add	r4, r2, #1
    104c: e243b004     	sub	r11, r3, #4
    1050: e1a08004     	mov	r8, r4
    1054: e35b0001     	cmp	r11, #1
    1058: 8affffcc     	bhi	0xf90 <shell_read+0x2d0> @ imm = #-0xd0
    105c: eaffffc0     	b	0xf64 <shell_read+0x2a4> @ imm = #-0x100
    1060: e282e002     	add	lr, r2, #2
    1064: e1a02004     	mov	r2, r4
    1068: e152000a     	cmp	r2, r10
    106c: e1a0400e     	mov	r4, lr
    1070: daffffbd     	ble	0xf6c <shell_read+0x2ac> @ imm = #-0x10c
    1074: e795618a     	ldr	r6, [r5, r10, lsl #3]
    1078: e1a0b18a     	lsl	r11, r10, #3
    107c: e085300b     	add	r3, r5, r11
    1080: e58d300c     	str	r3, [sp, #0xc]
    1084: e246e008     	sub	lr, r6, #8
    1088: e35e0001     	cmp	lr, #1
    108c: 9a000059     	bls	0x11f8 <shell_read+0x538> @ imm = #0x164
    1090: e04ac002     	sub	r12, r10, r2
    1094: e1a0300a     	mov	r3, r10
    1098: e1e0100c     	mvn	r1, r12
    109c: e2110007     	ands	r0, r1, #7
    10a0: 0a000029     	beq	0x114c <shell_read+0x48c> @ imm = #0xa4
    10a4: e28a3001     	add	r3, r10, #1
    10a8: e7958183     	ldr	r8, [r5, r3, lsl #3]
    10ac: e2487008     	sub	r7, r8, #8
    10b0: e3570001     	cmp	r7, #1
    10b4: 9a00004f     	bls	0x11f8 <shell_read+0x538> @ imm = #0x13c
    10b8: e3500001     	cmp	r0, #1
    10bc: 0a000022     	beq	0x114c <shell_read+0x48c> @ imm = #0x88
    10c0: e3500002     	cmp	r0, #2
    10c4: 0a00001b     	beq	0x1138 <shell_read+0x478> @ imm = #0x6c
    10c8: e3500003     	cmp	r0, #3
    10cc: 0a000014     	beq	0x1124 <shell_read+0x464> @ imm = #0x50
    10d0: e3500004     	cmp	r0, #4
    10d4: 0a00000d     	beq	0x1110 <shell_read+0x450> @ imm = #0x34
    10d8: e3500005     	cmp	r0, #5
    10dc: 0a000006     	beq	0x10fc <shell_read+0x43c> @ imm = #0x18
    10e0: e3500006     	cmp	r0, #6
    10e4: 1a00006d     	bne	0x12a0 <shell_read+0x5e0> @ imm = #0x1b4
    10e8: e2833001     	add	r3, r3, #1
    10ec: e7951183     	ldr	r1, [r5, r3, lsl #3]
    10f0: e2410008     	sub	r0, r1, #8
    10f4: e3500001     	cmp	r0, #1
    10f8: 9a00003e     	bls	0x11f8 <shell_read+0x538> @ imm = #0xf8
    10fc: e2833001     	add	r3, r3, #1
    1100: e7958183     	ldr	r8, [r5, r3, lsl #3]
    1104: e2487008     	sub	r7, r8, #8
    1108: e3570001     	cmp	r7, #1
    110c: 9a000039     	bls	0x11f8 <shell_read+0x538> @ imm = #0xe4
    1110: e2833001     	add	r3, r3, #1
    1114: e795e183     	ldr	lr, [r5, r3, lsl #3]
    1118: e24ec008     	sub	r12, lr, #8
    111c: e35c0001     	cmp	r12, #1
    1120: 9a000034     	bls	0x11f8 <shell_read+0x538> @ imm = #0xd0
    1124: e2833001     	add	r3, r3, #1
    1128: e7951183     	ldr	r1, [r5, r3, lsl #3]
    112c: e2410008     	sub	r0, r1, #8
    1130: e3500001     	cmp	r0, #1
    1134: 9a00002f     	bls	0x11f8 <shell_read+0x538> @ imm = #0xbc
    1138: e2833001     	add	r3, r3, #1
    113c: e7958183     	ldr	r8, [r5, r3, lsl #3]
    1140: e2487008     	sub	r7, r8, #8
    1144: e3570001     	cmp	r7, #1
    1148: 9a00002a     	bls	0x11f8 <shell_read+0x538> @ imm = #0xa8
    114c: e58d6008     	str	r6, [sp, #0x8]
    1150: e2831001     	add	r1, r3, #1
    1154: e283e003     	add	lr, r3, #3
    1158: e1520001     	cmp	r2, r1
    115c: e2836004     	add	r6, r3, #4
    1160: e2837005     	add	r7, r3, #5
    1164: e2838006     	add	r8, r3, #6
    1168: e2830007     	add	r0, r3, #7
    116c: e281c001     	add	r12, r1, #1
    1170: 0a000024     	beq	0x1208 <shell_read+0x548> @ imm = #0x90
    1174: e7951181     	ldr	r1, [r5, r1, lsl #3]
    1178: e2833008     	add	r3, r3, #8
    117c: e2411008     	sub	r1, r1, #8
    1180: e3510001     	cmp	r1, #1
    1184: 9a00001b     	bls	0x11f8 <shell_read+0x538> @ imm = #0x6c
    1188: e795c18c     	ldr	r12, [r5, r12, lsl #3]
    118c: e24c1008     	sub	r1, r12, #8
    1190: e3510001     	cmp	r1, #1
    1194: 9a000017     	bls	0x11f8 <shell_read+0x538> @ imm = #0x5c
    1198: e795e18e     	ldr	lr, [r5, lr, lsl #3]
    119c: e24ec008     	sub	r12, lr, #8
    11a0: e35c0001     	cmp	r12, #1
    11a4: 9a000013     	bls	0x11f8 <shell_read+0x538> @ imm = #0x4c
    11a8: e7956186     	ldr	r6, [r5, r6, lsl #3]
    11ac: e2461008     	sub	r1, r6, #8
    11b0: e3510001     	cmp	r1, #1
    11b4: 9a00000f     	bls	0x11f8 <shell_read+0x538> @ imm = #0x3c
    11b8: e7957187     	ldr	r7, [r5, r7, lsl #3]
    11bc: e247e008     	sub	lr, r7, #8
    11c0: e35e0001     	cmp	lr, #1
    11c4: 9a00000b     	bls	0x11f8 <shell_read+0x538> @ imm = #0x2c
    11c8: e7958188     	ldr	r8, [r5, r8, lsl #3]
    11cc: e248c008     	sub	r12, r8, #8
    11d0: e35c0001     	cmp	r12, #1
    11d4: 9a000007     	bls	0x11f8 <shell_read+0x538> @ imm = #0x1c
    11d8: e7950180     	ldr	r0, [r5, r0, lsl #3]
    11dc: e2406008     	sub	r6, r0, #8
    11e0: e3560001     	cmp	r6, #1
    11e4: 9a000003     	bls	0x11f8 <shell_read+0x538> @ imm = #0xc
    11e8: e7951183     	ldr	r1, [r5, r3, lsl #3]
    11ec: e2417008     	sub	r7, r1, #8
    11f0: e3570001     	cmp	r7, #1
    11f4: 8affffd5     	bhi	0x1150 <shell_read+0x490> @ imm = #-0xac
    11f8: e59d1010     	ldr	r1, [sp, #0x10]
    11fc: e59d0004     	ldr	r0, [sp, #0x4]
    1200: ebfffe53     	bl	0xb54 <.plt+0x1e8>      @ imm = #-0x6b4  // CALL pd_error
    1204: eaffff58     	b	0xf6c <shell_read+0x2ac> @ imm = #-0x2a0
    1208: e59de008     	ldr	lr, [sp, #0x8]
    120c: e35e0001     	cmp	lr, #1
    1210: 0a00000b     	beq	0x1244 <shell_read+0x584> @ imm = #0x2c
    1214: e35e0002     	cmp	lr, #2
    1218: 1affff53     	bne	0xf6c <shell_read+0x2ac> @ imm = #-0x2b4
    121c: e59d600c     	ldr	r6, [sp, #0xc]
    1220: e28bb008     	add	r11, r11, #8
    1224: e59d7004     	ldr	r7, [sp, #0x4]
    1228: e042a00a     	sub	r10, r2, r10
    122c: e085300b     	add	r3, r5, r11
    1230: e24a2001     	sub	r2, r10, #1
    1234: e5961004     	ldr	r1, [r6, #0x4]
    1238: e597000c     	ldr	r0, [r7, #0xc]
    123c: ebfffe3b     	bl	0xb30 <.plt+0x1c4>      @ imm = #-0x714  // CALL outlet_anything
    1240: eaffff49     	b	0xf6c <shell_read+0x2ac> @ imm = #-0x2dc
    1244: e153000a     	cmp	r3, r10
    1248: e59d3004     	ldr	r3, [sp, #0x4]
    124c: e593000c     	ldr	r0, [r3, #0xc]
    1250: e59d300c     	ldr	r3, [sp, #0xc]
    1254: da00001b     	ble	0x12c8 <shell_read+0x608> @ imm = #0x6c
    1258: e042200a     	sub	r2, r2, r10
    125c: e3a01000     	mov	r1, #0
    1260: ebfffe2c     	bl	0xb18 <.plt+0x1ac>      @ imm = #-0x750  // CALL outlet_list
    1264: eaffff40     	b	0xf6c <shell_read+0x2ac> @ imm = #-0x300
    1268: 1a000019     	bne	0x12d4 <shell_read+0x614> @ imm = #0x64
    126c: e59f0078     	ldr	r0, [pc, #0x78]         @ 0x12ec <shell_read+0x62c>  // u32=0x7dc; f32?=2.81941251e-42
    1270: e1a01005     	mov	r1, r5
    1274: e08f0000     	add	r0, pc, r0
    1278: ebfffe11     	bl	0xac4 <.plt+0x158>      @ imm = #-0x7bc  // CALL post
    127c: e1a00005     	mov	r0, r5
    1280: e3e09000     	mvn	r9, #0
    1284: ebfffdf0     	bl	0xa4c <.plt+0xe0>       @ imm = #-0x840  // CALL sys_rmpollfn
    1288: e1a00005     	mov	r0, r5
    128c: e58b9030     	str	r9, [r11, #0x30]
    1290: ebfffe2c     	bl	0xb48 <.plt+0x1dc>      @ imm = #-0x750  // CALL close
    1294: e28dde81     	add	sp, sp, #2064
    1298: e28dd00c     	add	sp, sp, #12
    129c: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
    12a0: e28a3002     	add	r3, r10, #2
    12a4: e795e183     	ldr	lr, [r5, r3, lsl #3]
    12a8: e24ec008     	sub	r12, lr, #8
    12ac: e35c0001     	cmp	r12, #1
    12b0: 8affff8c     	bhi	0x10e8 <shell_read+0x428> @ imm = #-0x1d0
    12b4: eaffffcf     	b	0x11f8 <shell_read+0x538> @ imm = #-0xc4
    12b8: e4d3c001     	ldrb	r12, [r3], #1
    12bc: e35c000a     	cmp	r12, #10
    12c0: 05435001     	strbeq	r5, [r3, #-0x1]
    12c4: eafffea1     	b	0xd50 <shell_read+0x90> @ imm = #-0x57c
    12c8: ed930a01     	vldr	s0, [r3, #4]
    12cc: ebfffe0e     	bl	0xb0c <.plt+0x1a0>      @ imm = #-0x7c8  // CALL outlet_float
    12d0: eaffff25     	b	0xf6c <shell_read+0x2ac> @ imm = #-0x36c
    12d4: e59f7014     	ldr	r7, [pc, #0x14]         @ 0x12f0 <shell_read+0x630>  // u32=0x75c; f32?=2.64004631e-42
    12d8: e1a0000b     	mov	r0, r11
    12dc: e08f1007     	add	r1, pc, r7
    12e0: ebfffe1b     	bl	0xb54 <.plt+0x1e8>      @ imm = #-0x794  // CALL pd_error
    12e4: eaffffe4     	b	0x127c <shell_read+0x5bc> @ imm = #-0x70
    12e8: fc 0b 00 00  	.word	0x00000bfc
    12ec: dc 07 00 00  	.word	0x000007dc
    12f0: 5c 07 00 00  	.word	0x0000075c

