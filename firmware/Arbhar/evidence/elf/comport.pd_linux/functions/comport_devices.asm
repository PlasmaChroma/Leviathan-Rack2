00000f34 <comport_devices>:
     f34: e92d43f0     	push	{r4, r5, r6, r7, r8, r9, lr}
     f38: e1a04000     	mov	r4, r0
     f3c: e59f0104     	ldr	r0, [pc, #0x104]        @ 0x1048 <comport_devices+0x114>
     f40: e24dd064     	sub	sp, sp, #100
     f44: e08f0000     	add	r0, pc, r0
     f48: ebfffefb     	bl	0xb3c <.plt+0x134>      @ imm = #-0x414
     f4c: e28450a0     	add	r5, r4, #160
     f50: e3a02000     	mov	r2, #0
     f54: e1a00005     	mov	r0, r5
     f58: e1a01002     	mov	r1, r2
     f5c: e1a0300d     	mov	r3, sp
     f60: ebfffefe     	bl	0xb60 <.plt+0x158>      @ imm = #-0x408
     f64: e3500002     	cmp	r0, #2
     f68: 0a000030     	beq	0x1030 <comport_devices+0xfc> @ imm = #0xc0
     f6c: e3500003     	cmp	r0, #3
     f70: 0a000028     	beq	0x1018 <comport_devices+0xe4> @ imm = #0xa0
     f74: e3500001     	cmp	r0, #1
     f78: 0a000020     	beq	0x1000 <comport_devices+0xcc> @ imm = #0x80
     f7c: e59d2000     	ldr	r2, [sp]
     f80: e3520000     	cmp	r2, #0
     f84: 0a00001b     	beq	0xff8 <comport_devices+0xc4> @ imm = #0x6c
     f88: e59f90bc     	ldr	r9, [pc, #0xbc]         @ 0x104c <comport_devices+0x118>
     f8c: e59f80bc     	ldr	r8, [pc, #0xbc]         @ 0x1050 <comport_devices+0x11c>
     f90: e08f9009     	add	r9, pc, r9
     f94: e3a04000     	mov	r4, #0
     f98: e28d7024     	add	r7, sp, #36
     f9c: e59dc004     	ldr	r12, [sp, #0x4]
     fa0: e1a01008     	mov	r1, r8
     fa4: e1a06104     	lsl	r6, r4, #2
     fa8: e79c0104     	ldr	r0, [r12, r4, lsl #2]
     fac: ebfffecd     	bl	0xae8 <.plt+0xe0>       @ imm = #-0x4cc
     fb0: e1a01007     	mov	r1, r7
     fb4: e3700001     	cmn	r0, #1
     fb8: e1a05000     	mov	r5, r0
     fbc: 0a000009     	beq	0xfe8 <comport_devices+0xb4> @ imm = #0x24
     fc0: ebffff0d     	bl	0xbfc <.plt+0x1f4>      @ imm = #-0x3cc
     fc4: e1a01004     	mov	r1, r4
     fc8: e3700001     	cmn	r0, #1
     fcc: e1a00009     	mov	r0, r9
     fd0: 0a000002     	beq	0xfe0 <comport_devices+0xac> @ imm = #0x8
     fd4: e59de004     	ldr	lr, [sp, #0x4]
     fd8: e79e2006     	ldr	r2, [lr, r6]
     fdc: ebfffed6     	bl	0xb3c <.plt+0x134>      @ imm = #-0x4a8
     fe0: e1a00005     	mov	r0, r5
     fe4: ebfffefb     	bl	0xbd8 <.plt+0x1d0>      @ imm = #-0x414
     fe8: e59d0000     	ldr	r0, [sp]
     fec: e2844001     	add	r4, r4, #1
     ff0: e1540000     	cmp	r4, r0
     ff4: 3affffe8     	blo	0xf9c <comport_devices+0x68> @ imm = #-0x60
     ff8: e28dd064     	add	sp, sp, #100
     ffc: e8bd83f0     	pop	{r4, r5, r6, r7, r8, r9, pc}
    1000: e59f104c     	ldr	r1, [pc, #0x4c]         @ 0x1054 <comport_devices+0x120>
    1004: e1a02005     	mov	r2, r5
    1008: e1a00004     	mov	r0, r4
    100c: e08f1001     	add	r1, pc, r1
    1010: ebfffef3     	bl	0xbe4 <.plt+0x1dc>      @ imm = #-0x434
    1014: eaffffd8     	b	0xf7c <comport_devices+0x48> @ imm = #-0xa0
    1018: e59f3038     	ldr	r3, [pc, #0x38]         @ 0x1058 <comport_devices+0x124>
    101c: e1a02005     	mov	r2, r5
    1020: e1a00004     	mov	r0, r4
    1024: e08f1003     	add	r1, pc, r3
    1028: ebfffeed     	bl	0xbe4 <.plt+0x1dc>      @ imm = #-0x44c
    102c: eaffffd2     	b	0xf7c <comport_devices+0x48> @ imm = #-0xb8
    1030: e59f6024     	ldr	r6, [pc, #0x24]         @ 0x105c <comport_devices+0x128>
    1034: e1a02005     	mov	r2, r5
    1038: e1a00004     	mov	r0, r4
    103c: e08f1006     	add	r1, pc, r6
    1040: ebfffee7     	bl	0xbe4 <.plt+0x1dc>      @ imm = #-0x464
    1044: eaffffcc     	b	0xf7c <comport_devices+0x48> @ imm = #-0xd0
    1048: d4 2e 00 00  	.word	0x00002ed4
    104c: ac 2e 00 00  	.word	0x00002eac
    1050: 02 09 00 00  	.word	0x00000902
    1054: 9c 2d 00 00  	.word	0x00002d9c
    1058: c0 2d 00 00  	.word	0x00002dc0
    105c: 90 2d 00 00  	.word	0x00002d90

