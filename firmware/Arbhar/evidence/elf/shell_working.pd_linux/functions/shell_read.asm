00000ed4 <shell_read>:
     ed4: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
     ed8: e24dde41     	sub	sp, sp, #1040
     edc: e24dd004     	sub	sp, sp, #4
     ee0: e1a05001     	mov	r5, r1
     ee4: e28d4010     	add	r4, sp, #16
     ee8: e58d0000     	str	r0, [sp]
     eec: ebffff2c     	bl	0xba4 <.plt+0x98>       @ imm = #-0x350
     ef0: e1a01004     	mov	r1, r4
     ef4: e30023ff     	movw	r2, #0x3ff
     ef8: e1a0b000     	mov	r11, r0
     efc: e1a00005     	mov	r0, r5
     f00: ebffff12     	bl	0xb50 <.plt+0x44>       @ imm = #-0x3b8
     f04: e28d3e41     	add	r3, sp, #1040
     f08: e3a02000     	mov	r2, #0
     f0c: e0831000     	add	r1, r3, r0
     f10: e3500000     	cmp	r0, #0
     f14: e5412400     	strb	r2, [r1, #-0x400]
     f18: da000162     	ble	0x14a8 <shell_read+0x5d4> @ imm = #0x588
     f1c: e5d49000     	ldrb	r9, [r4]
     f20: e240a001     	sub	r10, r0, #1
     f24: e20ae007     	and	lr, r10, #7
     f28: e084c000     	add	r12, r4, r0
     f2c: e359000a     	cmp	r9, #10
     f30: e3a0503b     	mov	r5, #59
     f34: 0a000148     	beq	0x145c <shell_read+0x588> @ imm = #0x520
     f38: e2842001     	add	r2, r4, #1
     f3c: e15c0002     	cmp	r12, r2
     f40: 0a00002b     	beq	0xff4 <shell_read+0x120> @ imm = #0xac
     f44: e35e0000     	cmp	lr, #0
     f48: 0a00012d     	beq	0x1404 <shell_read+0x530> @ imm = #0x4b4
     f4c: e35e0001     	cmp	lr, #1
     f50: 0a000021     	beq	0xfdc <shell_read+0x108> @ imm = #0x84
     f54: e35e0002     	cmp	lr, #2
     f58: 0a00001b     	beq	0xfcc <shell_read+0xf8> @ imm = #0x6c
     f5c: e35e0003     	cmp	lr, #3
     f60: 0a000015     	beq	0xfbc <shell_read+0xe8> @ imm = #0x54
     f64: e35e0004     	cmp	lr, #4
     f68: 0a00000f     	beq	0xfac <shell_read+0xd8> @ imm = #0x3c
     f6c: e35e0005     	cmp	lr, #5
     f70: 0a000009     	beq	0xf9c <shell_read+0xc8> @ imm = #0x24
     f74: e35e0006     	cmp	lr, #6
     f78: 0a000003     	beq	0xf8c <shell_read+0xb8> @ imm = #0xc
     f7c: e5d23000     	ldrb	r3, [r2]
     f80: e2822001     	add	r2, r2, #1
     f84: e353000a     	cmp	r3, #10
     f88: 05425001     	strbeq	r5, [r2, #-0x1]
     f8c: e5d21000     	ldrb	r1, [r2]
     f90: e2822001     	add	r2, r2, #1
     f94: e351000a     	cmp	r1, #10
     f98: 05425001     	strbeq	r5, [r2, #-0x1]
     f9c: e5d20000     	ldrb	r0, [r2]
     fa0: e2822001     	add	r2, r2, #1
     fa4: e350000a     	cmp	r0, #10
     fa8: 05425001     	strbeq	r5, [r2, #-0x1]
     fac: e5d26000     	ldrb	r6, [r2]
     fb0: e2822001     	add	r2, r2, #1
     fb4: e356000a     	cmp	r6, #10
     fb8: 05425001     	strbeq	r5, [r2, #-0x1]
     fbc: e5d27000     	ldrb	r7, [r2]
     fc0: e2822001     	add	r2, r2, #1
     fc4: e357000a     	cmp	r7, #10
     fc8: 05425001     	strbeq	r5, [r2, #-0x1]
     fcc: e5d28000     	ldrb	r8, [r2]
     fd0: e2822001     	add	r2, r2, #1
     fd4: e358000a     	cmp	r8, #10
     fd8: 05425001     	strbeq	r5, [r2, #-0x1]
     fdc: e5d29000     	ldrb	r9, [r2]
     fe0: e2822001     	add	r2, r2, #1
     fe4: e359000a     	cmp	r9, #10
     fe8: 05425001     	strbeq	r5, [r2, #-0x1]
     fec: e15c0002     	cmp	r12, r2
     ff0: 1a000103     	bne	0x1404 <shell_read+0x530> @ imm = #0x40c
     ff4: e1a00004     	mov	r0, r4
     ff8: ebffff19     	bl	0xc64 <.plt+0x158>      @ imm = #-0x39c
     ffc: e1a01004     	mov	r1, r4
    1000: e1a02000     	mov	r2, r0
    1004: e1a0000b     	mov	r0, r11
    1008: ebffff33     	bl	0xcdc <.plt+0x1d0>      @ imm = #-0x334
    100c: e1a0000b     	mov	r0, r11
    1010: ebffff43     	bl	0xd24 <.plt+0x218>      @ imm = #-0x2f4
    1014: e1a0000b     	mov	r0, r11
    1018: ebfffec9     	bl	0xb44 <.plt+0x38>       @ imm = #-0x4dc
    101c: e1a0000b     	mov	r0, r11
    1020: ebffff3f     	bl	0xd24 <.plt+0x218>      @ imm = #-0x304
    1024: e1a09000     	mov	r9, r0
    1028: e1a0000b     	mov	r0, r11
    102c: ebfffec4     	bl	0xb44 <.plt+0x38>       @ imm = #-0x4f0
    1030: e3590000     	cmp	r9, #0
    1034: e1a05000     	mov	r5, r0
    1038: da0000d6     	ble	0x1398 <shell_read+0x4c4> @ imm = #0x358
    103c: e59f44b0     	ldr	r4, [pc, #0x4b0]        @ 0x14f4 <shell_read+0x620>
    1040: e3a0a000     	mov	r10, #0
    1044: e58db00c     	str	r11, [sp, #0xc]
    1048: e08fc004     	add	r12, pc, r4
    104c: e58dc008     	str	r12, [sp, #0x8]
    1050: e15a0009     	cmp	r10, r9
    1054: aa00010c     	bge	0x148c <shell_read+0x5b8> @ imm = #0x430
    1058: e795118a     	ldr	r1, [r5, r10, lsl #3]
    105c: e1a0c18a     	lsl	r12, r10, #3
    1060: e085b00c     	add	r11, r5, r12
    1064: e241e004     	sub	lr, r1, #4
    1068: e35e0001     	cmp	lr, #1
    106c: 9a000106     	bls	0x148c <shell_read+0x5b8> @ imm = #0x418
    1070: e04a3009     	sub	r3, r10, r9
    1074: e1a0400a     	mov	r4, r10
    1078: e1e00003     	mvn	r0, r3
    107c: e2106007     	ands	r6, r0, #7
    1080: 0a00002e     	beq	0x1140 <shell_read+0x26c> @ imm = #0xb8
    1084: e28a4001     	add	r4, r10, #1
    1088: e7957184     	ldr	r7, [r5, r4, lsl #3]
    108c: e2478004     	sub	r8, r7, #4
    1090: e3580001     	cmp	r8, #1
    1094: 9a000054     	bls	0x11ec <shell_read+0x318> @ imm = #0x150
    1098: e3560001     	cmp	r6, #1
    109c: 0a000027     	beq	0x1140 <shell_read+0x26c> @ imm = #0x9c
    10a0: e3560002     	cmp	r6, #2
    10a4: 0a000020     	beq	0x112c <shell_read+0x258> @ imm = #0x80
    10a8: e3560003     	cmp	r6, #3
    10ac: 0a000019     	beq	0x1118 <shell_read+0x244> @ imm = #0x64
    10b0: e3560004     	cmp	r6, #4
    10b4: 0a000012     	beq	0x1104 <shell_read+0x230> @ imm = #0x48
    10b8: e3560005     	cmp	r6, #5
    10bc: 0a00000b     	beq	0x10f0 <shell_read+0x21c> @ imm = #0x2c
    10c0: e3560006     	cmp	r6, #6
    10c4: 0a000004     	beq	0x10dc <shell_read+0x208> @ imm = #0x10
    10c8: e2844001     	add	r4, r4, #1
    10cc: e7952184     	ldr	r2, [r5, r4, lsl #3]
    10d0: e242e004     	sub	lr, r2, #4
    10d4: e35e0001     	cmp	lr, #1
    10d8: 9a000043     	bls	0x11ec <shell_read+0x318> @ imm = #0x10c
    10dc: e2844001     	add	r4, r4, #1
    10e0: e7953184     	ldr	r3, [r5, r4, lsl #3]
    10e4: e2430004     	sub	r0, r3, #4
    10e8: e3500001     	cmp	r0, #1
    10ec: 9a00003e     	bls	0x11ec <shell_read+0x318> @ imm = #0xf8
    10f0: e2844001     	add	r4, r4, #1
    10f4: e7956184     	ldr	r6, [r5, r4, lsl #3]
    10f8: e2467004     	sub	r7, r6, #4
    10fc: e3570001     	cmp	r7, #1
    1100: 9a000039     	bls	0x11ec <shell_read+0x318> @ imm = #0xe4
    1104: e2844001     	add	r4, r4, #1
    1108: e7958184     	ldr	r8, [r5, r4, lsl #3]
    110c: e2482004     	sub	r2, r8, #4
    1110: e3520001     	cmp	r2, #1
    1114: 9a000034     	bls	0x11ec <shell_read+0x318> @ imm = #0xd0
    1118: e2844001     	add	r4, r4, #1
    111c: e795e184     	ldr	lr, [r5, r4, lsl #3]
    1120: e24e3004     	sub	r3, lr, #4
    1124: e3530001     	cmp	r3, #1
    1128: 9a00002f     	bls	0x11ec <shell_read+0x318> @ imm = #0xbc
    112c: e2844001     	add	r4, r4, #1
    1130: e7950184     	ldr	r0, [r5, r4, lsl #3]
    1134: e2406004     	sub	r6, r0, #4
    1138: e3560001     	cmp	r6, #1
    113c: 9a00002a     	bls	0x11ec <shell_read+0x318> @ imm = #0xa8
    1140: e2844001     	add	r4, r4, #1
    1144: e1590004     	cmp	r9, r4
    1148: e1a07004     	mov	r7, r4
    114c: 0a000026     	beq	0x11ec <shell_read+0x318> @ imm = #0x98
    1150: e7958184     	ldr	r8, [r5, r4, lsl #3]
    1154: e2482004     	sub	r2, r8, #4
    1158: e3520001     	cmp	r2, #1
    115c: 9a000022     	bls	0x11ec <shell_read+0x318> @ imm = #0x88
    1160: e2844001     	add	r4, r4, #1
    1164: e795e184     	ldr	lr, [r5, r4, lsl #3]
    1168: e24e3004     	sub	r3, lr, #4
    116c: e3530001     	cmp	r3, #1
    1170: 9a00001d     	bls	0x11ec <shell_read+0x318> @ imm = #0x74
    1174: e2874002     	add	r4, r7, #2
    1178: e7950184     	ldr	r0, [r5, r4, lsl #3]
    117c: e2406004     	sub	r6, r0, #4
    1180: e3560001     	cmp	r6, #1
    1184: 9a000018     	bls	0x11ec <shell_read+0x318> @ imm = #0x60
    1188: e2874003     	add	r4, r7, #3
    118c: e7958184     	ldr	r8, [r5, r4, lsl #3]
    1190: e2482004     	sub	r2, r8, #4
    1194: e3520001     	cmp	r2, #1
    1198: 9a000013     	bls	0x11ec <shell_read+0x318> @ imm = #0x4c
    119c: e2874004     	add	r4, r7, #4
    11a0: e795e184     	ldr	lr, [r5, r4, lsl #3]
    11a4: e24e3004     	sub	r3, lr, #4
    11a8: e3530001     	cmp	r3, #1
    11ac: 9a00000e     	bls	0x11ec <shell_read+0x318> @ imm = #0x38
    11b0: e2874005     	add	r4, r7, #5
    11b4: e7950184     	ldr	r0, [r5, r4, lsl #3]
    11b8: e2406004     	sub	r6, r0, #4
    11bc: e3560001     	cmp	r6, #1
    11c0: 9a000009     	bls	0x11ec <shell_read+0x318> @ imm = #0x24
    11c4: e2874006     	add	r4, r7, #6
    11c8: e7958184     	ldr	r8, [r5, r4, lsl #3]
    11cc: e2482004     	sub	r2, r8, #4
    11d0: e3520001     	cmp	r2, #1
    11d4: 9a000004     	bls	0x11ec <shell_read+0x318> @ imm = #0x10
    11d8: e2874007     	add	r4, r7, #7
    11dc: e7957184     	ldr	r7, [r5, r4, lsl #3]
    11e0: e247e004     	sub	lr, r7, #4
    11e4: e35e0001     	cmp	lr, #1
    11e8: 8affffd4     	bhi	0x1140 <shell_read+0x26c> @ imm = #-0xb0
    11ec: e15a0004     	cmp	r10, r4
    11f0: aa000064     	bge	0x1388 <shell_read+0x4b4> @ imm = #0x190
    11f4: e2413008     	sub	r3, r1, #8
    11f8: e3530001     	cmp	r3, #1
    11fc: 9a00005e     	bls	0x137c <shell_read+0x4a8> @ imm = #0x178
    1200: e04a0004     	sub	r0, r10, r4
    1204: e1a0300a     	mov	r3, r10
    1208: e1e06000     	mvn	r6, r0
    120c: e2168007     	ands	r8, r6, #7
    1210: 0a00002e     	beq	0x12d0 <shell_read+0x3fc> @ imm = #0xb8
    1214: e28a3001     	add	r3, r10, #1
    1218: e7952183     	ldr	r2, [r5, r3, lsl #3]
    121c: e2427008     	sub	r7, r2, #8
    1220: e3570001     	cmp	r7, #1
    1224: 9a000054     	bls	0x137c <shell_read+0x4a8> @ imm = #0x150
    1228: e3580001     	cmp	r8, #1
    122c: 0a000027     	beq	0x12d0 <shell_read+0x3fc> @ imm = #0x9c
    1230: e3580002     	cmp	r8, #2
    1234: 0a000020     	beq	0x12bc <shell_read+0x3e8> @ imm = #0x80
    1238: e3580003     	cmp	r8, #3
    123c: 0a000019     	beq	0x12a8 <shell_read+0x3d4> @ imm = #0x64
    1240: e3580004     	cmp	r8, #4
    1244: 0a000012     	beq	0x1294 <shell_read+0x3c0> @ imm = #0x48
    1248: e3580005     	cmp	r8, #5
    124c: 0a00000b     	beq	0x1280 <shell_read+0x3ac> @ imm = #0x2c
    1250: e3580006     	cmp	r8, #6
    1254: 0a000004     	beq	0x126c <shell_read+0x398> @ imm = #0x10
    1258: e2833001     	add	r3, r3, #1
    125c: e795e183     	ldr	lr, [r5, r3, lsl #3]
    1260: e24e0008     	sub	r0, lr, #8
    1264: e3500001     	cmp	r0, #1
    1268: 9a000043     	bls	0x137c <shell_read+0x4a8> @ imm = #0x10c
    126c: e2833001     	add	r3, r3, #1
    1270: e7956183     	ldr	r6, [r5, r3, lsl #3]
    1274: e2468008     	sub	r8, r6, #8
    1278: e3580001     	cmp	r8, #1
    127c: 9a00003e     	bls	0x137c <shell_read+0x4a8> @ imm = #0xf8
    1280: e2833001     	add	r3, r3, #1
    1284: e7952183     	ldr	r2, [r5, r3, lsl #3]
    1288: e2427008     	sub	r7, r2, #8
    128c: e3570001     	cmp	r7, #1
    1290: 9a000039     	bls	0x137c <shell_read+0x4a8> @ imm = #0xe4
    1294: e2833001     	add	r3, r3, #1
    1298: e795e183     	ldr	lr, [r5, r3, lsl #3]
    129c: e24e0008     	sub	r0, lr, #8
    12a0: e3500001     	cmp	r0, #1
    12a4: 9a000034     	bls	0x137c <shell_read+0x4a8> @ imm = #0xd0
    12a8: e2833001     	add	r3, r3, #1
    12ac: e7956183     	ldr	r6, [r5, r3, lsl #3]
    12b0: e2468008     	sub	r8, r6, #8
    12b4: e3580001     	cmp	r8, #1
    12b8: 9a00002f     	bls	0x137c <shell_read+0x4a8> @ imm = #0xbc
    12bc: e2833001     	add	r3, r3, #1
    12c0: e7952183     	ldr	r2, [r5, r3, lsl #3]
    12c4: e2427008     	sub	r7, r2, #8
    12c8: e3570001     	cmp	r7, #1
    12cc: 9a00002a     	bls	0x137c <shell_read+0x4a8> @ imm = #0xa8
    12d0: e58da004     	str	r10, [sp, #0x4]
    12d4: e2832001     	add	r2, r3, #1
    12d8: e283e003     	add	lr, r3, #3
    12dc: e1540002     	cmp	r4, r2
    12e0: e2836004     	add	r6, r3, #4
    12e4: e2838005     	add	r8, r3, #5
    12e8: e2837006     	add	r7, r3, #6
    12ec: e283a007     	add	r10, r3, #7
    12f0: e2820001     	add	r0, r2, #1
    12f4: e2833008     	add	r3, r3, #8
    12f8: 0a000049     	beq	0x1424 <shell_read+0x550> @ imm = #0x124
    12fc: e7952182     	ldr	r2, [r5, r2, lsl #3]
    1300: e2422008     	sub	r2, r2, #8
    1304: e3520001     	cmp	r2, #1
    1308: 9a00001b     	bls	0x137c <shell_read+0x4a8> @ imm = #0x6c
    130c: e7950180     	ldr	r0, [r5, r0, lsl #3]
    1310: e2402008     	sub	r2, r0, #8
    1314: e3520001     	cmp	r2, #1
    1318: 9a000017     	bls	0x137c <shell_read+0x4a8> @ imm = #0x5c
    131c: e795e18e     	ldr	lr, [r5, lr, lsl #3]
    1320: e24e0008     	sub	r0, lr, #8
    1324: e3500001     	cmp	r0, #1
    1328: 9a000013     	bls	0x137c <shell_read+0x4a8> @ imm = #0x4c
    132c: e7956186     	ldr	r6, [r5, r6, lsl #3]
    1330: e2462008     	sub	r2, r6, #8
    1334: e3520001     	cmp	r2, #1
    1338: 9a00000f     	bls	0x137c <shell_read+0x4a8> @ imm = #0x3c
    133c: e7958188     	ldr	r8, [r5, r8, lsl #3]
    1340: e248e008     	sub	lr, r8, #8
    1344: e35e0001     	cmp	lr, #1
    1348: 9a00000b     	bls	0x137c <shell_read+0x4a8> @ imm = #0x2c
    134c: e7957187     	ldr	r7, [r5, r7, lsl #3]
    1350: e2470008     	sub	r0, r7, #8
    1354: e3500001     	cmp	r0, #1
    1358: 9a000007     	bls	0x137c <shell_read+0x4a8> @ imm = #0x1c
    135c: e795a18a     	ldr	r10, [r5, r10, lsl #3]
    1360: e24a6008     	sub	r6, r10, #8
    1364: e3560001     	cmp	r6, #1
    1368: 9a000003     	bls	0x137c <shell_read+0x4a8> @ imm = #0xc
    136c: e7952183     	ldr	r2, [r5, r3, lsl #3]
    1370: e2428008     	sub	r8, r2, #8
    1374: e3580001     	cmp	r8, #1
    1378: 8affffd5     	bhi	0x12d4 <shell_read+0x400> @ imm = #-0xac
    137c: e59d1008     	ldr	r1, [sp, #0x8]
    1380: e59d0000     	ldr	r0, [sp]
    1384: ebfffe63     	bl	0xd18 <.plt+0x20c>      @ imm = #-0x674
    1388: e284a001     	add	r10, r4, #1
    138c: e159000a     	cmp	r9, r10
    1390: caffff2e     	bgt	0x1050 <shell_read+0x17c> @ imm = #-0x348
    1394: e59db00c     	ldr	r11, [sp, #0xc]
    1398: e1a0000b     	mov	r0, r11
    139c: ebfffe2d     	bl	0xc58 <.plt+0x14c>      @ imm = #-0x74c
    13a0: e28dde41     	add	sp, sp, #1040
    13a4: e28dd004     	add	sp, sp, #4
    13a8: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
    13ac: e5c25001     	strb	r5, [r2, #0x1]
    13b0: e5d32001     	ldrb	r2, [r3, #0x1]
    13b4: e5d31002     	ldrb	r1, [r3, #0x2]
    13b8: e5d30003     	ldrb	r0, [r3, #0x3]
    13bc: e352000a     	cmp	r2, #10
    13c0: e5d36004     	ldrb	r6, [r3, #0x4]
    13c4: e2832007     	add	r2, r3, #7
    13c8: e5d37005     	ldrb	r7, [r3, #0x5]
    13cc: e5d38006     	ldrb	r8, [r3, #0x6]
    13d0: 05c35001     	strbeq	r5, [r3, #0x1]
    13d4: e351000a     	cmp	r1, #10
    13d8: 05c35002     	strbeq	r5, [r3, #0x2]
    13dc: e350000a     	cmp	r0, #10
    13e0: 05c35003     	strbeq	r5, [r3, #0x3]
    13e4: e356000a     	cmp	r6, #10
    13e8: 05c35004     	strbeq	r5, [r3, #0x4]
    13ec: e357000a     	cmp	r7, #10
    13f0: 05c35005     	strbeq	r5, [r3, #0x5]
    13f4: e358000a     	cmp	r8, #10
    13f8: 05c35006     	strbeq	r5, [r3, #0x6]
    13fc: e15c0002     	cmp	r12, r2
    1400: 0afffefb     	beq	0xff4 <shell_read+0x120> @ imm = #-0x414
    1404: e5d2a000     	ldrb	r10, [r2]
    1408: e2823001     	add	r3, r2, #1
    140c: e5d2e001     	ldrb	lr, [r2, #0x1]
    1410: e35a000a     	cmp	r10, #10
    1414: 05c25000     	strbeq	r5, [r2]
    1418: e35e000a     	cmp	lr, #10
    141c: 1affffe3     	bne	0x13b0 <shell_read+0x4dc> @ imm = #-0x74
    1420: eaffffe1     	b	0x13ac <shell_read+0x4d8> @ imm = #-0x7c
    1424: e3510001     	cmp	r1, #1
    1428: e59de004     	ldr	lr, [sp, #0x4]
    142c: 0a00000c     	beq	0x1464 <shell_read+0x590> @ imm = #0x30
    1430: e3510002     	cmp	r1, #2
    1434: 1affffd3     	bne	0x1388 <shell_read+0x4b4> @ imm = #-0xb4
    1438: e28c1008     	add	r1, r12, #8
    143c: e044c00e     	sub	r12, r4, lr
    1440: e0853001     	add	r3, r5, r1
    1444: e59b1004     	ldr	r1, [r11, #0x4]
    1448: e59db000     	ldr	r11, [sp]
    144c: e24c2001     	sub	r2, r12, #1
    1450: e59b000c     	ldr	r0, [r11, #0xc]
    1454: ebfffe26     	bl	0xcf4 <.plt+0x1e8>      @ imm = #-0x768
    1458: eaffffca     	b	0x1388 <shell_read+0x4b4> @ imm = #-0xd8
    145c: e5c45000     	strb	r5, [r4]
    1460: eafffeb4     	b	0xf38 <shell_read+0x64> @ imm = #-0x530
    1464: e28e6001     	add	r6, lr, #1
    1468: e1540006     	cmp	r4, r6
    146c: da000008     	ble	0x1494 <shell_read+0x5c0> @ imm = #0x20
    1470: e59d7000     	ldr	r7, [sp]
    1474: e1a0300b     	mov	r3, r11
    1478: e044200e     	sub	r2, r4, lr
    147c: e3a01000     	mov	r1, #0
    1480: e597000c     	ldr	r0, [r7, #0xc]
    1484: ebfffe11     	bl	0xcd0 <.plt+0x1c4>      @ imm = #-0x7bc
    1488: eaffffbe     	b	0x1388 <shell_read+0x4b4> @ imm = #-0x108
    148c: e1a0400a     	mov	r4, r10
    1490: eaffffbc     	b	0x1388 <shell_read+0x4b4> @ imm = #-0x110
    1494: e59d8000     	ldr	r8, [sp]
    1498: ed9b0a01     	vldr	s0, [r11, #4]
    149c: e598000c     	ldr	r0, [r8, #0xc]
    14a0: ebfffe07     	bl	0xcc4 <.plt+0x1b8>      @ imm = #-0x7e4
    14a4: eaffffb7     	b	0x1388 <shell_read+0x4b4> @ imm = #-0x124
    14a8: 1a00000d     	bne	0x14e4 <shell_read+0x610> @ imm = #0x34
    14ac: e59f0044     	ldr	r0, [pc, #0x44]         @ 0x14f8 <shell_read+0x624>
    14b0: e1a01005     	mov	r1, r5
    14b4: e08f0000     	add	r0, pc, r0
    14b8: ebfffdef     	bl	0xc7c <.plt+0x170>      @ imm = #-0x844
    14bc: e1a00005     	mov	r0, r5
    14c0: e3e07000     	mvn	r7, #0
    14c4: ebfffdc8     	bl	0xbec <.plt+0xe0>       @ imm = #-0x8e0
    14c8: e59d8000     	ldr	r8, [sp]
    14cc: e1a00005     	mov	r0, r5
    14d0: e5887030     	str	r7, [r8, #0x30]
    14d4: ebfffe0c     	bl	0xd0c <.plt+0x200>      @ imm = #-0x7d0
    14d8: e28dde41     	add	sp, sp, #1040
    14dc: e28dd004     	add	sp, sp, #4
    14e0: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
    14e4: e59f6010     	ldr	r6, [pc, #0x10]         @ 0x14fc <shell_read+0x628>
    14e8: e08f0006     	add	r0, pc, r6
    14ec: ebfffdc1     	bl	0xbf8 <.plt+0xec>       @ imm = #-0x8fc
    14f0: eafffff1     	b	0x14bc <shell_read+0x5e8> @ imm = #-0x3c
    14f4: 1c 0d 00 00  	.word	0x00000d1c
    14f8: 9c 08 00 00  	.word	0x0000089c
    14fc: 50 08 00 00  	.word	0x00000850

