00007684 <writeToSharedMem>:
    7684: e2800a01     	add	r0, r0, #4096
    7688: ee072a90     	vmov	s15, r2
    768c: e5903dc0     	ldr	r3, [r0, #0xdc0]
    7690: e3530000     	cmp	r3, #0
    7694: 0a000003     	beq	0x76a8 <writeToSharedMem+0x24> @ imm = #0xc
    7698: eeb80a67     	vcvt.f32.u32	s0, s15
    769c: e0831101     	add	r1, r3, r1, lsl #2
    76a0: ed810a00     	vstr	s0, [r1]
    76a4: e12fff1e     	bx	lr
    76a8: e59f2004     	ldr	r2, [pc, #0x4]          @ 0x76b4 <writeToSharedMem+0x30>  // u32=0xd76c; f32?=7.72788077e-41
    76ac: e08f0002     	add	r0, pc, r2
    76b0: eafff0c1     	b	0x39bc <.plt+0x2c0>     @ imm = #-0x3cfc  // CALL error
    76b4: 6c d7 00 00  	.word	0x0000d76c

