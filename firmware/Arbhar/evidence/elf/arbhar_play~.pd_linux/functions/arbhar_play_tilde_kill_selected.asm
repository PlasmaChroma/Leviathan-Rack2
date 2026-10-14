000029a8 <arbhar_play_tilde_kill_selected>:
    29a8: eefd7ac0     	vcvt.s32.f32	s15, s0
    29ac: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
    29b0: eefd0ae0     	vcvt.s32.f32	s1, s1
    29b4: ee172a90     	vmov	r2, s15
    29b8: ee103a90     	vmov	r3, s1
    29bc: e083e002     	add	lr, r3, r2
    29c0: e152000e     	cmp	r2, lr
    29c4: a8bd8ff0     	popge	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
    29c8: e3520051     	cmp	r2, #81
    29cc: ca000073     	bgt	0x2ba0 <arbhar_play_tilde_kill_selected+0x1f8> @ imm = #0x1cc
    29d0: e3a01074     	mov	r1, #116
    29d4: e24ec001     	sub	r12, lr, #1
    29d8: e0230291     	mla	r3, r1, r2, r0
    29dc: e35c0051     	cmp	r12, #81
    29e0: e3a04001     	mov	r4, #1
    29e4: e3a055fe     	mov	r5, #1065353216
    29e8: a3a0c051     	movge	r12, #81
    29ec: e04c6002     	sub	r6, r12, r2
    29f0: e2061007     	and	r1, r6, #7
    29f4: e2822001     	add	r2, r2, #1
    29f8: e2833074     	add	r3, r3, #116
    29fc: e59360b8     	ldr	r6, [r3, #0xb8]
    2a00: e3560000     	cmp	r6, #0
    2a04: c58350e0     	strgt	r5, [r3, #0xe0]
    2a08: c58340e4     	strgt	r4, [r3, #0xe4]
    2a0c: e152000c     	cmp	r2, r12
    2a10: ca000060     	bgt	0x2b98 <arbhar_play_tilde_kill_selected+0x1f0> @ imm = #0x180
    2a14: e3510000     	cmp	r1, #0
    2a18: 0a000037     	beq	0x2afc <arbhar_play_tilde_kill_selected+0x154> @ imm = #0xdc
    2a1c: e3510001     	cmp	r1, #1
    2a20: 0a00002d     	beq	0x2adc <arbhar_play_tilde_kill_selected+0x134> @ imm = #0xb4
    2a24: e3510002     	cmp	r1, #2
    2a28: 0a000025     	beq	0x2ac4 <arbhar_play_tilde_kill_selected+0x11c> @ imm = #0x94
    2a2c: e3510003     	cmp	r1, #3
    2a30: 0a00001d     	beq	0x2aac <arbhar_play_tilde_kill_selected+0x104> @ imm = #0x74
    2a34: e3510004     	cmp	r1, #4
    2a38: 0a000015     	beq	0x2a94 <arbhar_play_tilde_kill_selected+0xec> @ imm = #0x54
    2a3c: e3510005     	cmp	r1, #5
    2a40: 0a00000d     	beq	0x2a7c <arbhar_play_tilde_kill_selected+0xd4> @ imm = #0x34
    2a44: e3510006     	cmp	r1, #6
    2a48: 0a000005     	beq	0x2a64 <arbhar_play_tilde_kill_selected+0xbc> @ imm = #0x14
    2a4c: e593112c     	ldr	r1, [r3, #0x12c]
    2a50: e2822001     	add	r2, r2, #1
    2a54: e2833074     	add	r3, r3, #116
    2a58: e3510000     	cmp	r1, #0
    2a5c: c58350e0     	strgt	r5, [r3, #0xe0]
    2a60: c58340e4     	strgt	r4, [r3, #0xe4]
    2a64: e593112c     	ldr	r1, [r3, #0x12c]
    2a68: e2822001     	add	r2, r2, #1
    2a6c: e2833074     	add	r3, r3, #116
    2a70: e3510000     	cmp	r1, #0
    2a74: c58350e0     	strgt	r5, [r3, #0xe0]
    2a78: c58340e4     	strgt	r4, [r3, #0xe4]
    2a7c: e593112c     	ldr	r1, [r3, #0x12c]
    2a80: e2822001     	add	r2, r2, #1
    2a84: e2833074     	add	r3, r3, #116
    2a88: e3510000     	cmp	r1, #0
    2a8c: c58350e0     	strgt	r5, [r3, #0xe0]
    2a90: c58340e4     	strgt	r4, [r3, #0xe4]
    2a94: e593112c     	ldr	r1, [r3, #0x12c]
    2a98: e2822001     	add	r2, r2, #1
    2a9c: e2833074     	add	r3, r3, #116
    2aa0: e3510000     	cmp	r1, #0
    2aa4: c58350e0     	strgt	r5, [r3, #0xe0]
    2aa8: c58340e4     	strgt	r4, [r3, #0xe4]
    2aac: e593112c     	ldr	r1, [r3, #0x12c]
    2ab0: e2822001     	add	r2, r2, #1
    2ab4: e2833074     	add	r3, r3, #116
    2ab8: e3510000     	cmp	r1, #0
    2abc: c58350e0     	strgt	r5, [r3, #0xe0]
    2ac0: c58340e4     	strgt	r4, [r3, #0xe4]
    2ac4: e593112c     	ldr	r1, [r3, #0x12c]
    2ac8: e2822001     	add	r2, r2, #1
    2acc: e2833074     	add	r3, r3, #116
    2ad0: e3510000     	cmp	r1, #0
    2ad4: c58350e0     	strgt	r5, [r3, #0xe0]
    2ad8: c58340e4     	strgt	r4, [r3, #0xe4]
    2adc: e593112c     	ldr	r1, [r3, #0x12c]
    2ae0: e2822001     	add	r2, r2, #1
    2ae4: e2833074     	add	r3, r3, #116
    2ae8: e3510000     	cmp	r1, #0
    2aec: c58350e0     	strgt	r5, [r3, #0xe0]
    2af0: c58340e4     	strgt	r4, [r3, #0xe4]
    2af4: e152000c     	cmp	r2, r12
    2af8: ca000026     	bgt	0x2b98 <arbhar_play_tilde_kill_selected+0x1f0> @ imm = #0x98
    2afc: e593612c     	ldr	r6, [r3, #0x12c]
    2b00: e2833074     	add	r3, r3, #116
    2b04: e2822001     	add	r2, r2, #1
    2b08: e3560000     	cmp	r6, #0
    2b0c: c58350e0     	strgt	r5, [r3, #0xe0]
    2b10: c58340e4     	strgt	r4, [r3, #0xe4]
    2b14: e2822007     	add	r2, r2, #7
    2b18: e593112c     	ldr	r1, [r3, #0x12c]
    2b1c: e59361a0     	ldr	r6, [r3, #0x1a0]
    2b20: e3510000     	cmp	r1, #0
    2b24: e5931214     	ldr	r1, [r3, #0x214]
    2b28: c5835154     	strgt	r5, [r3, #0x154]
    2b2c: c5834158     	strgt	r4, [r3, #0x158]
    2b30: e3560000     	cmp	r6, #0
    2b34: e5936288     	ldr	r6, [r3, #0x288]
    2b38: c58351c8     	strgt	r5, [r3, #0x1c8]
    2b3c: c58341cc     	strgt	r4, [r3, #0x1cc]
    2b40: e3510000     	cmp	r1, #0
    2b44: e59312fc     	ldr	r1, [r3, #0x2fc]
    2b48: c583523c     	strgt	r5, [r3, #0x23c]
    2b4c: c5834240     	strgt	r4, [r3, #0x240]
    2b50: e3560000     	cmp	r6, #0
    2b54: e5936370     	ldr	r6, [r3, #0x370]
    2b58: c58352b0     	strgt	r5, [r3, #0x2b0]
    2b5c: c58342b4     	strgt	r4, [r3, #0x2b4]
    2b60: e3510000     	cmp	r1, #0
    2b64: e2831fae     	add	r1, r3, #696
    2b68: c5835324     	strgt	r5, [r3, #0x324]
    2b6c: c5834328     	strgt	r4, [r3, #0x328]
    2b70: e3560000     	cmp	r6, #0
    2b74: e59363e4     	ldr	r6, [r3, #0x3e4]
    2b78: c5835398     	strgt	r5, [r3, #0x398]
    2b7c: c583439c     	strgt	r4, [r3, #0x39c]
    2b80: e3560000     	cmp	r6, #0
    2b84: c5815154     	strgt	r5, [r1, #0x154]
    2b88: c5834410     	strgt	r4, [r3, #0x410]
    2b8c: e152000c     	cmp	r2, r12
    2b90: e2833fcb     	add	r3, r3, #812
    2b94: daffffd8     	ble	0x2afc <arbhar_play_tilde_kill_selected+0x154> @ imm = #-0xa0
    2b98: e152000e     	cmp	r2, lr
    2b9c: a8bd8ff0     	popge	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
    2ba0: e300c63f     	movw	r12, #0x63f
    2ba4: e346c3e7     	movt	r12, #0x63e7
    2ba8: e1a03fc2     	asr	r3, r2, #31
    2bac: e3a04052     	mov	r4, #82
    2bb0: e0c8129c     	smull	r1, r8, r12, r2
    2bb4: e3a05074     	mov	r5, #116
    2bb8: e1e07002     	mvn	r7, r2
    2bbc: e087600e     	add	r6, r7, lr
    2bc0: e2061003     	and	r1, r6, #3
    2bc4: e3a075fe     	mov	r7, #1065353216
    2bc8: e3a06001     	mov	r6, #1
    2bcc: e06332c8     	rsb	r3, r3, r8, asr #5
    2bd0: e0682394     	mls	r8, r4, r3, r2
    2bd4: e2822001     	add	r2, r2, #1
    2bd8: e0230895     	mla	r3, r5, r8, r0
    2bdc: e593812c     	ldr	r8, [r3, #0x12c]
    2be0: e3580000     	cmp	r8, #0
    2be4: c5837154     	strgt	r7, [r3, #0x154]
    2be8: c5836158     	strgt	r6, [r3, #0x158]
    2bec: e15e0002     	cmp	lr, r2
    2bf0: d8bd8ff0     	pople	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
    2bf4: e3510000     	cmp	r1, #0
    2bf8: 0a000023     	beq	0x2c8c <arbhar_play_tilde_kill_selected+0x2e4> @ imm = #0x8c
    2bfc: e3510001     	cmp	r1, #1
    2c00: 0a000015     	beq	0x2c5c <arbhar_play_tilde_kill_selected+0x2b4> @ imm = #0x54
    2c04: e3510002     	cmp	r1, #2
    2c08: 0a000009     	beq	0x2c34 <arbhar_play_tilde_kill_selected+0x28c> @ imm = #0x24
    2c0c: e0c3129c     	smull	r1, r3, r12, r2
    2c10: e1a01fc2     	asr	r1, r2, #31
    2c14: e06132c3     	rsb	r3, r1, r3, asr #5
    2c18: e0612394     	mls	r1, r4, r3, r2
    2c1c: e2822001     	add	r2, r2, #1
    2c20: e0230195     	mla	r3, r5, r1, r0
    2c24: e593112c     	ldr	r1, [r3, #0x12c]
    2c28: e3510000     	cmp	r1, #0
    2c2c: c5837154     	strgt	r7, [r3, #0x154]
    2c30: c5836158     	strgt	r6, [r3, #0x158]
    2c34: e0c3129c     	smull	r1, r3, r12, r2
    2c38: e1a01fc2     	asr	r1, r2, #31
    2c3c: e06132c3     	rsb	r3, r1, r3, asr #5
    2c40: e0612394     	mls	r1, r4, r3, r2
    2c44: e2822001     	add	r2, r2, #1
    2c48: e0230195     	mla	r3, r5, r1, r0
    2c4c: e593112c     	ldr	r1, [r3, #0x12c]
    2c50: e3510000     	cmp	r1, #0
    2c54: c5837154     	strgt	r7, [r3, #0x154]
    2c58: c5836158     	strgt	r6, [r3, #0x158]
    2c5c: e0c3129c     	smull	r1, r3, r12, r2
    2c60: e1a01fc2     	asr	r1, r2, #31
    2c64: e06132c3     	rsb	r3, r1, r3, asr #5
    2c68: e0612394     	mls	r1, r4, r3, r2
    2c6c: e2822001     	add	r2, r2, #1
    2c70: e0230195     	mla	r3, r5, r1, r0
    2c74: e593112c     	ldr	r1, [r3, #0x12c]
    2c78: e3510000     	cmp	r1, #0
    2c7c: c5837154     	strgt	r7, [r3, #0x154]
    2c80: c5836158     	strgt	r6, [r3, #0x158]
    2c84: e15e0002     	cmp	lr, r2
    2c88: d8bd8ff0     	pople	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
    2c8c: e2821001     	add	r1, r2, #1
    2c90: e1a08fc2     	asr	r8, r2, #31
    2c94: e0ca329c     	smull	r3, r10, r12, r2
    2c98: e1a03fc1     	asr	r3, r1, #31
    2c9c: e0c9b19c     	smull	r11, r9, r12, r1
    2ca0: e068b2ca     	rsb	r11, r8, r10, asr #5
    2ca4: e0622b94     	mls	r2, r4, r11, r2
    2ca8: e06382c9     	rsb	r8, r3, r9, asr #5
    2cac: e06a1894     	mls	r10, r4, r8, r1
    2cb0: e2818002     	add	r8, r1, #2
    2cb4: e0290295     	mla	r9, r5, r2, r0
    2cb8: e0230a95     	mla	r3, r5, r10, r0
    2cbc: e281a001     	add	r10, r1, #1
    2cc0: e599b12c     	ldr	r11, [r9, #0x12c]
    2cc4: e35b0000     	cmp	r11, #0
    2cc8: c5896158     	strgt	r6, [r9, #0x158]
    2ccc: e593212c     	ldr	r2, [r3, #0x12c]
    2cd0: c5897154     	strgt	r7, [r9, #0x154]
    2cd4: e3520000     	cmp	r2, #0
    2cd8: c5836158     	strgt	r6, [r3, #0x158]
    2cdc: c5837154     	strgt	r7, [r3, #0x154]
    2ce0: e2812003     	add	r2, r1, #3
    2ce4: e0cb3a9c     	smull	r3, r11, r12, r10
    2ce8: e1a01fca     	asr	r1, r10, #31
    2cec: e0c9389c     	smull	r3, r9, r12, r8
    2cf0: e1a03fc8     	asr	r3, r8, #31
    2cf4: e06112cb     	rsb	r1, r1, r11, asr #5
    2cf8: e061a194     	mls	r1, r4, r1, r10
    2cfc: e06332c9     	rsb	r3, r3, r9, asr #5
    2d00: e0688394     	mls	r8, r4, r3, r8
    2d04: e0210195     	mla	r1, r5, r1, r0
    2d08: e0230895     	mla	r3, r5, r8, r0
    2d0c: e591812c     	ldr	r8, [r1, #0x12c]
    2d10: e3580000     	cmp	r8, #0
    2d14: c5817154     	strgt	r7, [r1, #0x154]
    2d18: c5816158     	strgt	r6, [r1, #0x158]
    2d1c: e593112c     	ldr	r1, [r3, #0x12c]
    2d20: e3510000     	cmp	r1, #0
    2d24: c5837154     	strgt	r7, [r3, #0x154]
    2d28: c5836158     	strgt	r6, [r3, #0x158]
    2d2c: e15e0002     	cmp	lr, r2
    2d30: caffffd5     	bgt	0x2c8c <arbhar_play_tilde_kill_selected+0x2e4> @ imm = #-0xac
    2d34: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}

