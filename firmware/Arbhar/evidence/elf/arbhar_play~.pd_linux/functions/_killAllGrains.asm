0000294c <_killAllGrains>:
    294c: e92d4070     	push	{r4, r5, r6, lr}
    2950: e2806c25     	add	r6, r0, #9472
    2954: e2866028     	add	r6, r6, #40
    2958: e2803074     	add	r3, r0, #116
    295c: e3a02000     	mov	r2, #0
    2960: e580212c     	str	r2, [r0, #0x12c]
    2964: e2831074     	add	r1, r3, #116
    2968: e28350e8     	add	r5, r3, #232
    296c: e2834f57     	add	r4, r3, #348
    2970: e283ee1d     	add	lr, r3, #464
    2974: e583212c     	str	r2, [r3, #0x12c]
    2978: e58321a0     	str	r2, [r3, #0x1a0]
    297c: e5832214     	str	r2, [r3, #0x214]
    2980: e5832288     	str	r2, [r3, #0x288]
    2984: e58322fc     	str	r2, [r3, #0x2fc]
    2988: e5832370     	str	r2, [r3, #0x370]
    298c: e58323e4     	str	r2, [r3, #0x3e4]
    2990: e5832458     	str	r2, [r3, #0x458]
    2994: e58324cc     	str	r2, [r3, #0x4cc]
    2998: e2813e3a     	add	r3, r1, #928
    299c: e1530006     	cmp	r3, r6
    29a0: 1affffef     	bne	0x2964 <_killAllGrains+0x18> @ imm = #-0x44
    29a4: e8bd8070     	pop	{r4, r5, r6, pc}

