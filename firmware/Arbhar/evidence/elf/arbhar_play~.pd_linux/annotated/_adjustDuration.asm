00002e20 <_adjustDuration>:
    2e20: eefd7ac0     	vcvt.s32.f32	s15, s0
    2e24: ee173a90     	vmov	r3, s15
    2e28: e3530000     	cmp	r3, #0
    2e2c: 0a000009     	beq	0x2e58 <_adjustDuration+0x38> @ imm = #0x24
    2e30: e3530001     	cmp	r3, #1
    2e34: 0a000002     	beq	0x2e44 <_adjustDuration+0x24> @ imm = #0x8
    2e38: e59f002c     	ldr	r0, [pc, #0x2c]         @ 0x2e6c <_adjustDuration+0x4c>  // u32=0x9c84; f32?=5.61472269e-41
    2e3c: e08f0000     	add	r0, pc, r0
    2e40: eafffdf5     	b	0x261c <.plt+0x23c>     @ imm = #-0x82c  // CALL post
    2e44: e2802a02     	add	r2, r0, #8192
    2e48: e59f1020     	ldr	r1, [pc, #0x20]         @ 0x2e70 <_adjustDuration+0x50>  // u32=0x9c58; f32?=5.60855697e-41
    2e4c: e58236bc     	str	r3, [r2, #0x6bc]
    2e50: e08f0001     	add	r0, pc, r1
    2e54: eafffdf0     	b	0x261c <.plt+0x23c>     @ imm = #-0x840  // CALL post
    2e58: e280ca02     	add	r12, r0, #8192
    2e5c: e59f0010     	ldr	r0, [pc, #0x10]         @ 0x2e74 <_adjustDuration+0x54>  // u32=0x9c2c; f32?=5.60239126e-41
    2e60: e58c36bc     	str	r3, [r12, #0x6bc]
    2e64: e08f0000     	add	r0, pc, r0
    2e68: eafffdeb     	b	0x261c <.plt+0x23c>     @ imm = #-0x854  // CALL post
    2e6c: 84 9c 00 00  	.word	0x00009c84
    2e70: 58 9c 00 00  	.word	0x00009c58
    2e74: 2c 9c 00 00  	.word	0x00009c2c

