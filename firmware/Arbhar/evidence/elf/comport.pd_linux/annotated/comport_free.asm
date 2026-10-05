00001e74 <comport_free>:
    1e74: e92d4070     	push	{r4, r5, r6, lr}
    1e78: e2804a01     	add	r4, r0, #4096
    1e7c: e1a05000     	mov	r5, r0
    1e80: e59f0074     	ldr	r0, [pc, #0x74]         @ 0x1efc <comport_free+0x88>  // u32=0x27b4; f32?=1.42427976e-41
    1e84: e08f0000     	add	r0, pc, r0
    1e88: ebfffb2b     	bl	0xb3c <.plt+0x134>      @ imm = #-0x1354  // CALL post
    1e8c: e59400c4     	ldr	r0, [r4, #0xc4]
    1e90: ebfffb17     	bl	0xaf4 <.plt+0xec>       @ imm = #-0x13a4  // CALL clock_unset
    1e94: e59400c4     	ldr	r0, [r4, #0xc4]
    1e98: ebfffb4b     	bl	0xbcc <.plt+0x1c4>      @ imm = #-0x12d4  // CALL clock_free
    1e9c: e5956020     	ldr	r6, [r5, #0x20]
    1ea0: e3760001     	cmn	r6, #1
    1ea4: 0a00000b     	beq	0x1ed8 <comport_free+0x64> @ imm = #0x2c
    1ea8: e2852060     	add	r2, r5, #96
    1eac: e3a01000     	mov	r1, #0
    1eb0: e1a00006     	mov	r0, r6
    1eb4: ebfffafc     	bl	0xaac <.plt+0xa4>       @ imm = #-0x1410  // CALL tcsetattr
    1eb8: e1a00006     	mov	r0, r6
    1ebc: ebfffb45     	bl	0xbd8 <.plt+0x1d0>      @ imm = #-0x12ec  // CALL close
    1ec0: e595309c     	ldr	r3, [r5, #0x9c]
    1ec4: e59f2034     	ldr	r2, [pc, #0x34]         @ 0x1f00 <comport_free+0x8c>  // u32=0x2788; f32?=1.41811405e-41
    1ec8: e1d41af0     	ldrsh	r1, [r4, #160]
    1ecc: e08f0002     	add	r0, pc, r2
    1ed0: e5932000     	ldr	r2, [r3]
    1ed4: ebfffb18     	bl	0xb3c <.plt+0x134>      @ imm = #-0x13a0  // CALL post
    1ed8: e3e01000     	mvn	r1, #0
    1edc: e5851020     	str	r1, [r5, #0x20]
    1ee0: e59400ec     	ldr	r0, [r4, #0xec]
    1ee4: e59410f4     	ldr	r1, [r4, #0xf4]
    1ee8: ebfffb31     	bl	0xbb4 <.plt+0x1ac>      @ imm = #-0x133c  // CALL freebytes
    1eec: e59410f8     	ldr	r1, [r4, #0xf8]
    1ef0: e59400f0     	ldr	r0, [r4, #0xf0]
    1ef4: e8bd4070     	pop	{r4, r5, r6, lr}
    1ef8: eafffb2d     	b	0xbb4 <.plt+0x1ac>      @ imm = #-0x134c  // CALL freebytes
    1efc: b4 27 00 00  	.word	0x000027b4
    1f00: 88 27 00 00  	.word	0x00002788

