00009fcc <writeChunkToSharedMem>:
    9fcc: e280ca01     	add	r12, r0, #4096
    9fd0: e59ccdc0     	ldr	r12, [r12, #0xdc0]
    9fd4: e35c0000     	cmp	r12, #0
    9fd8: 0a0000a2     	beq	0xa268 <writeChunkToSharedMem+0x29c> @ imm = #0x288
    9fdc: e3530000     	cmp	r3, #0
    9fe0: 012fff1e     	bxeq	lr
    9fe4: e92d4030     	push	{r4, r5, lr}
    9fe8: e3a0ef71     	mov	lr, #452
    9fec: e1d240b0     	ldrh	r4, [r2]
    9ff0: e3530001     	cmp	r3, #1
    9ff4: ee074a90     	vmov	s15, r4
    9ff8: e024019e     	mla	r4, lr, r1, r0
    9ffc: eeb80a67     	vcvt.f32.u32	s0, s15
    a000: e5d4411c     	ldrb	r4, [r4, #0x11c]
    a004: e08c4104     	add	r4, r12, r4, lsl #2
    a008: ed840a00     	vstr	s0, [r4]
    a00c: 08bd8030     	popeq	{r4, r5, pc}
    a010: e1d250b2     	ldrh	r5, [r2, #2]
    a014: e3530002     	cmp	r3, #2
    a018: e024ee91     	mla	r4, r1, lr, lr
    a01c: ee005a90     	vmov	s1, r5
    a020: e0804004     	add	r4, r0, r4
    a024: eeb81a60     	vcvt.f32.u32	s2, s1
    a028: e5d4411c     	ldrb	r4, [r4, #0x11c]
    a02c: e08c4104     	add	r4, r12, r4, lsl #2
    a030: ed841a00     	vstr	s2, [r4]
    a034: 08bd8030     	popeq	{r4, r5, pc}
    a038: e2814002     	add	r4, r1, #2
    a03c: e1d250b4     	ldrh	r5, [r2, #4]
    a040: e3530003     	cmp	r3, #3
    a044: e024049e     	mla	r4, lr, r4, r0
    a048: ee015a90     	vmov	s3, r5
    a04c: eeb82a61     	vcvt.f32.u32	s4, s3
    a050: e5d4411c     	ldrb	r4, [r4, #0x11c]
    a054: e08c4104     	add	r4, r12, r4, lsl #2
    a058: ed842a00     	vstr	s4, [r4]
    a05c: 08bd8030     	popeq	{r4, r5, pc}
    a060: e2814003     	add	r4, r1, #3
    a064: e1d250b6     	ldrh	r5, [r2, #6]
    a068: e3530004     	cmp	r3, #4
    a06c: e024049e     	mla	r4, lr, r4, r0
    a070: ee025a90     	vmov	s5, r5
    a074: eeb83a62     	vcvt.f32.u32	s6, s5
    a078: e5d4411c     	ldrb	r4, [r4, #0x11c]
    a07c: e08c4104     	add	r4, r12, r4, lsl #2
    a080: ed843a00     	vstr	s6, [r4]
    a084: 08bd8030     	popeq	{r4, r5, pc}
    a088: e2814004     	add	r4, r1, #4
    a08c: e1d250b8     	ldrh	r5, [r2, #8]
    a090: e3530005     	cmp	r3, #5
    a094: e024049e     	mla	r4, lr, r4, r0
    a098: ee035a90     	vmov	s7, r5
    a09c: eeb84a63     	vcvt.f32.u32	s8, s7
    a0a0: e5d4411c     	ldrb	r4, [r4, #0x11c]
    a0a4: e08c4104     	add	r4, r12, r4, lsl #2
    a0a8: ed844a00     	vstr	s8, [r4]
    a0ac: 08bd8030     	popeq	{r4, r5, pc}
    a0b0: e2814005     	add	r4, r1, #5
    a0b4: e1d250ba     	ldrh	r5, [r2, #10]
    a0b8: e3530006     	cmp	r3, #6
    a0bc: e024049e     	mla	r4, lr, r4, r0
    a0c0: ee045a90     	vmov	s9, r5
    a0c4: eeb85a64     	vcvt.f32.u32	s10, s9
    a0c8: e5d4411c     	ldrb	r4, [r4, #0x11c]
    a0cc: e08c4104     	add	r4, r12, r4, lsl #2
    a0d0: ed845a00     	vstr	s10, [r4]
    a0d4: 08bd8030     	popeq	{r4, r5, pc}
    a0d8: e2814006     	add	r4, r1, #6
    a0dc: e1d250bc     	ldrh	r5, [r2, #12]
    a0e0: e3530007     	cmp	r3, #7
    a0e4: e024049e     	mla	r4, lr, r4, r0
    a0e8: ee055a90     	vmov	s11, r5
    a0ec: eeb86a65     	vcvt.f32.u32	s12, s11
    a0f0: e5d4411c     	ldrb	r4, [r4, #0x11c]
    a0f4: e08c4104     	add	r4, r12, r4, lsl #2
    a0f8: ed846a00     	vstr	s12, [r4]
    a0fc: 08bd8030     	popeq	{r4, r5, pc}
    a100: e2814007     	add	r4, r1, #7
    a104: e1d250be     	ldrh	r5, [r2, #14]
    a108: e3530008     	cmp	r3, #8
    a10c: e02e049e     	mla	lr, lr, r4, r0
    a110: ee065a90     	vmov	s13, r5
    a114: eeb87a66     	vcvt.f32.u32	s14, s13
    a118: e5dee11c     	ldrb	lr, [lr, #0x11c]
    a11c: e08ce10e     	add	lr, r12, lr, lsl #2
    a120: ed8e7a00     	vstr	s14, [lr]
    a124: 08bd8030     	popeq	{r4, r5, pc}
    a128: e2814008     	add	r4, r1, #8
    a12c: e1d251b0     	ldrh	r5, [r2, #16]
    a130: e3a0ef71     	mov	lr, #452
    a134: e3530009     	cmp	r3, #9
    a138: e024049e     	mla	r4, lr, r4, r0
    a13c: ee075a90     	vmov	s15, r5
    a140: eeb80a67     	vcvt.f32.u32	s0, s15
    a144: e5d4411c     	ldrb	r4, [r4, #0x11c]
    a148: e08c4104     	add	r4, r12, r4, lsl #2
    a14c: ed840a00     	vstr	s0, [r4]
    a150: 08bd8030     	popeq	{r4, r5, pc}
    a154: e2814009     	add	r4, r1, #9
    a158: e1d251b2     	ldrh	r5, [r2, #18]
    a15c: e353000a     	cmp	r3, #10
    a160: e024049e     	mla	r4, lr, r4, r0
    a164: ee005a90     	vmov	s1, r5
    a168: eeb81a60     	vcvt.f32.u32	s2, s1
    a16c: e5d4411c     	ldrb	r4, [r4, #0x11c]
    a170: e08c4104     	add	r4, r12, r4, lsl #2
    a174: ed841a00     	vstr	s2, [r4]
    a178: 08bd8030     	popeq	{r4, r5, pc}
    a17c: e281400a     	add	r4, r1, #10
    a180: e1d251b4     	ldrh	r5, [r2, #20]
    a184: e353000b     	cmp	r3, #11
    a188: e024049e     	mla	r4, lr, r4, r0
    a18c: ee015a90     	vmov	s3, r5
    a190: eeb82a61     	vcvt.f32.u32	s4, s3
    a194: e5d4411c     	ldrb	r4, [r4, #0x11c]
    a198: e08c4104     	add	r4, r12, r4, lsl #2
    a19c: ed842a00     	vstr	s4, [r4]
    a1a0: 08bd8030     	popeq	{r4, r5, pc}
    a1a4: e281400b     	add	r4, r1, #11
    a1a8: e1d251b6     	ldrh	r5, [r2, #22]
    a1ac: e353000c     	cmp	r3, #12
    a1b0: e024049e     	mla	r4, lr, r4, r0
    a1b4: ee025a90     	vmov	s5, r5
    a1b8: eeb83a62     	vcvt.f32.u32	s6, s5
    a1bc: e5d4411c     	ldrb	r4, [r4, #0x11c]
    a1c0: e08c4104     	add	r4, r12, r4, lsl #2
    a1c4: ed843a00     	vstr	s6, [r4]
    a1c8: 08bd8030     	popeq	{r4, r5, pc}
    a1cc: e281400c     	add	r4, r1, #12
    a1d0: e1d251b8     	ldrh	r5, [r2, #24]
    a1d4: e353000d     	cmp	r3, #13
    a1d8: e024049e     	mla	r4, lr, r4, r0
    a1dc: ee035a90     	vmov	s7, r5
    a1e0: eeb84a63     	vcvt.f32.u32	s8, s7
    a1e4: e5d4411c     	ldrb	r4, [r4, #0x11c]
    a1e8: e08c4104     	add	r4, r12, r4, lsl #2
    a1ec: ed844a00     	vstr	s8, [r4]
    a1f0: 08bd8030     	popeq	{r4, r5, pc}
    a1f4: e281400d     	add	r4, r1, #13
    a1f8: e1d251ba     	ldrh	r5, [r2, #26]
    a1fc: e353000e     	cmp	r3, #14
    a200: e024049e     	mla	r4, lr, r4, r0
    a204: ee045a90     	vmov	s9, r5
    a208: eeb85a64     	vcvt.f32.u32	s10, s9
    a20c: e5d4411c     	ldrb	r4, [r4, #0x11c]
    a210: e08c4104     	add	r4, r12, r4, lsl #2
    a214: ed845a00     	vstr	s10, [r4]
    a218: 08bd8030     	popeq	{r4, r5, pc}
    a21c: e281400e     	add	r4, r1, #14
    a220: e1d251bc     	ldrh	r5, [r2, #28]
    a224: e353000f     	cmp	r3, #15
    a228: e024049e     	mla	r4, lr, r4, r0
    a22c: ee055a90     	vmov	s11, r5
    a230: eeb86a65     	vcvt.f32.u32	s12, s11
    a234: e5d4311c     	ldrb	r3, [r4, #0x11c]
    a238: e08c3103     	add	r3, r12, r3, lsl #2
    a23c: ed836a00     	vstr	s12, [r3]
    a240: 08bd8030     	popeq	{r4, r5, pc}
    a244: e281100f     	add	r1, r1, #15
    a248: e1d221be     	ldrh	r2, [r2, #30]
    a24c: e020019e     	mla	r0, lr, r1, r0
    a250: ee062a90     	vmov	s13, r2
    a254: eeb87a66     	vcvt.f32.u32	s14, s13
    a258: e5d0311c     	ldrb	r3, [r0, #0x11c]
    a25c: e08cc103     	add	r12, r12, r3, lsl #2
    a260: ed8c7a00     	vstr	s14, [r12]
    a264: e8bd8030     	pop	{r4, r5, pc}
    a268: e59f1004     	ldr	r1, [pc, #0x4]          @ 0xa274 <writeChunkToSharedMem+0x2a8>
    a26c: e08f0001     	add	r0, pc, r1
    a270: eaffe5d1     	b	0x39bc <.plt+0x2c0>     @ imm = #-0x68bc
    a274: ac ab 00 00  	.word	0x0000abac

