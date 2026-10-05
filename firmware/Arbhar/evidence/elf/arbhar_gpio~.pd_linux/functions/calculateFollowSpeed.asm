00003f10 <calculateFollowSpeed>:
    3f10: e3500002     	cmp	r0, #2
    3f14: 0a000003     	beq	0x3f28 <calculateFollowSpeed+0x18> @ imm = #0xc
    3f18: eddf7a55     	vldr	s15, [pc, #340]         @ 0x4074 <calculateFollowSpeed+0x164>
    3f1c: e3500001     	cmp	r0, #1
    3f20: ee370ac0     	vsub.f32	s0, s15, s0
    3f24: 0e300a00     	vaddeq.f32	s0, s0, s0
    3f28: eddf0a51     	vldr	s1, [pc, #324]          @ 0x4074 <calculateFollowSpeed+0x164>
    3f2c: eeb40ae0     	vcmpe.f32	s0, s1
    3f30: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3f34: 5a000024     	bpl	0x3fcc <calculateFollowSpeed+0xbc> @ imm = #0x90
    3f38: eddf1a4e     	vldr	s3, [pc, #312]          @ 0x4078 <calculateFollowSpeed+0x168>
    3f3c: eeb40ae1     	vcmpe.f32	s0, s3
    3f40: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3f44: 5a00000d     	bpl	0x3f80 <calculateFollowSpeed+0x70> @ imm = #0x34
    3f48: ed9f4a4b     	vldr	s8, [pc, #300]          @ 0x407c <calculateFollowSpeed+0x16c>
    3f4c: eddf4a4b     	vldr	s9, [pc, #300]          @ 0x4080 <calculateFollowSpeed+0x170>
    3f50: ee603a04     	vmul.f32	s7, s0, s8
    3f54: eef43ae4     	vcmpe.f32	s7, s9
    3f58: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3f5c: aa000015     	bge	0x3fb8 <calculateFollowSpeed+0xa8> @ imm = #0x54
    3f60: eeb70a00     	vmov.f32	s0, #1.000000e+00
    3f64: ed9f5a46     	vldr	s10, [pc, #280]         @ 0x4084 <calculateFollowSpeed+0x174>
    3f68: eef07a40     	vmov.f32	s15, s0
    3f6c: ee437ac5     	vmls.f32	s15, s7, s10
    3f70: eeb36a03     	vmov.f32	s12, #1.900000e+01
    3f74: ee676aa7     	vmul.f32	s13, s15, s15
    3f78: ee060a86     	vmla.f32	s0, s13, s12
    3f7c: e12fff1e     	bx	lr
    3f80: ed9f7a40     	vldr	s14, [pc, #256]         @ 0x4088 <calculateFollowSpeed+0x178>
    3f84: eeb40ac7     	vcmpe.f32	s0, s14
    3f88: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3f8c: ba000036     	blt	0x406c <calculateFollowSpeed+0x15c> @ imm = #0xd8
    3f90: ee302a47     	vsub.f32	s4, s0, s14
    3f94: eddf2a3c     	vldr	s5, [pc, #240]          @ 0x408c <calculateFollowSpeed+0x17c>
    3f98: ed9f0a38     	vldr	s0, [pc, #224]          @ 0x4080 <calculateFollowSpeed+0x170>
    3f9c: eddf4a37     	vldr	s9, [pc, #220]          @ 0x4080 <calculateFollowSpeed+0x170>
    3fa0: ee223a22     	vmul.f32	s6, s4, s5
    3fa4: ee030a00     	vmla.f32	s0, s6, s0
    3fa8: eef03a40     	vmov.f32	s7, s0
    3fac: eef43ae4     	vcmpe.f32	s7, s9
    3fb0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3fb4: baffffe9     	blt	0x3f60 <calculateFollowSpeed+0x50> @ imm = #-0x5c
    3fb8: ee740ae3     	vsub.f32	s1, s9, s7
    3fbc: ed9f1a30     	vldr	s2, [pc, #192]          @ 0x4084 <calculateFollowSpeed+0x174>
    3fc0: eeb70a00     	vmov.f32	s0, #1.000000e+00
    3fc4: ee000a81     	vmla.f32	s0, s1, s2
    3fc8: e12fff1e     	bx	lr
    3fcc: ed9f1a2f     	vldr	s2, [pc, #188]          @ 0x4090 <calculateFollowSpeed+0x180>
    3fd0: eeb40ac1     	vcmpe.f32	s0, s2
    3fd4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3fd8: da00000f     	ble	0x401c <calculateFollowSpeed+0x10c> @ imm = #0x3c
    3fdc: ed9f3a2c     	vldr	s6, [pc, #176]          @ 0x4094 <calculateFollowSpeed+0x184>
    3fe0: eddf3a25     	vldr	s7, [pc, #148]          @ 0x407c <calculateFollowSpeed+0x16c>
    3fe4: ee334a40     	vsub.f32	s8, s6, s0
    3fe8: eddf4a24     	vldr	s9, [pc, #144]          @ 0x4080 <calculateFollowSpeed+0x170>
    3fec: ee242a23     	vmul.f32	s4, s8, s7
    3ff0: eeb42ae4     	vcmpe.f32	s4, s9
    3ff4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3ff8: aa000014     	bge	0x4050 <calculateFollowSpeed+0x140> @ imm = #0x50
    3ffc: eeb70a00     	vmov.f32	s0, #1.000000e+00
    4000: ed9f5a1f     	vldr	s10, [pc, #124]         @ 0x4084 <calculateFollowSpeed+0x174>
    4004: eef07a40     	vmov.f32	s15, s0
    4008: ee427a45     	vmls.f32	s15, s4, s10
    400c: eeb36a03     	vmov.f32	s12, #1.900000e+01
    4010: ee676aa7     	vmul.f32	s13, s15, s15
    4014: ee160a66     	vnmla.f32	s0, s12, s13
    4018: e12fff1e     	bx	lr
    401c: eddf1a1d     	vldr	s3, [pc, #116]          @ 0x4098 <calculateFollowSpeed+0x188>
    4020: eeb40ae1     	vcmpe.f32	s0, s3
    4024: eef1fa10     	vmrs	APSR_nzcv, fpscr
    4028: 8a00000d     	bhi	0x4064 <calculateFollowSpeed+0x154> @ imm = #0x34
    402c: ee310ac0     	vsub.f32	s0, s3, s0
    4030: ed9f7a15     	vldr	s14, [pc, #84]          @ 0x408c <calculateFollowSpeed+0x17c>
    4034: ed9f2a11     	vldr	s4, [pc, #68]           @ 0x4080 <calculateFollowSpeed+0x170>
    4038: eddf4a10     	vldr	s9, [pc, #64]           @ 0x4080 <calculateFollowSpeed+0x170>
    403c: ee602a07     	vmul.f32	s5, s0, s14
    4040: ee022a82     	vmla.f32	s4, s5, s4
    4044: eeb42ae4     	vcmpe.f32	s4, s9
    4048: eef1fa10     	vmrs	APSR_nzcv, fpscr
    404c: baffffea     	blt	0x3ffc <calculateFollowSpeed+0xec> @ imm = #-0x58
    4050: ee720a64     	vsub.f32	s1, s4, s9
    4054: ed9f1a0a     	vldr	s2, [pc, #40]           @ 0x4084 <calculateFollowSpeed+0x174>
    4058: eeb70a00     	vmov.f32	s0, #1.000000e+00
    405c: ee100a81     	vnmls.f32	s0, s1, s2
    4060: e12fff1e     	bx	lr
    4064: eebf0a00     	vmov.f32	s0, #-1.000000e+00
    4068: e12fff1e     	bx	lr
    406c: eeb70a00     	vmov.f32	s0, #1.000000e+00
    4070: e12fff1e     	bx	lr
    4074: 00 f0 7f 45  	.word	0x457ff000
    4078: 00 80 89 44  	.word	0x44898000
    407c: 0f 50 ee 3f  	.word	0x3fee500f
    4080: 00 00 00 45  	.word	0x45000000
    4084: 00 00 00 3a  	.word	0x3a000000
    4088: 00 80 a2 44  	.word	0x44a28000
    408c: a7 94 bb 39  	.word	0x39bb94a7
    4090: 00 90 dd 45  	.word	0x45dd9000
    4094: 00 f0 ff 45  	.word	0x45fff000
    4098: 00 50 d7 45  	.word	0x45d75000

