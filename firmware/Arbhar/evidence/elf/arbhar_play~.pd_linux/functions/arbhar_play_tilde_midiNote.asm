00003328 <arbhar_play_tilde_midiNote>:
    3328: eef50ac0     	vcmpe.f32	s1, #0
    332c: e92d4010     	push	{r4, lr}
    3330: e2804a02     	add	r4, r0, #8192
    3334: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3338: da000029     	ble	0x33e4 <arbhar_play_tilde_midiNote+0xbc> @ imm = #0xa4
    333c: e2843e6f     	add	r3, r4, #1776
    3340: edd37a00     	vldr	s15, [r3]
    3344: eef57a40     	vcmp.f32	s15, #0
    3348: eef1fa10     	vmrs	APSR_nzcv, fpscr
    334c: 0a000090     	beq	0x3594 <arbhar_play_tilde_midiNote+0x26c> @ imm = #0x240
    3350: edd31a03     	vldr	s3, [r3, #12]
    3354: eef51a40     	vcmp.f32	s3, #0
    3358: eef1fa10     	vmrs	APSR_nzcv, fpscr
    335c: 0a00008e     	beq	0x359c <arbhar_play_tilde_midiNote+0x274> @ imm = #0x238
    3360: e2841c07     	add	r1, r4, #1792
    3364: ed912a02     	vldr	s4, [r1, #8]
    3368: eeb52a40     	vcmp.f32	s4, #0
    336c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3370: 0a00008b     	beq	0x35a4 <arbhar_play_tilde_midiNote+0x27c> @ imm = #0x22c
    3374: e2842e71     	add	r2, r4, #1808
    3378: edd22a01     	vldr	s5, [r2, #4]
    337c: eef52a40     	vcmp.f32	s5, #0
    3380: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3384: 0a000088     	beq	0x35ac <arbhar_play_tilde_midiNote+0x284> @ imm = #0x220
    3388: e284ce72     	add	r12, r4, #1824
    338c: ed9c3a00     	vldr	s6, [r12]
    3390: eeb53a40     	vcmp.f32	s6, #0
    3394: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3398: 0a000079     	beq	0x3584 <arbhar_play_tilde_midiNote+0x25c> @ imm = #0x1e4
    339c: eddc3a03     	vldr	s7, [r12, #12]
    33a0: eef53a40     	vcmp.f32	s7, #0
    33a4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    33a8: 0a000077     	beq	0x358c <arbhar_play_tilde_midiNote+0x264> @ imm = #0x1dc
    33ac: e284ee73     	add	lr, r4, #1840
    33b0: ed9e4a02     	vldr	s8, [lr, #8]
    33b4: eeb54a40     	vcmp.f32	s8, #0
    33b8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    33bc: 0a000065     	beq	0x3558 <arbhar_play_tilde_midiNote+0x230> @ imm = #0x194
    33c0: e2843d1d     	add	r3, r4, #1856
    33c4: edd34a01     	vldr	s9, [r3, #4]
    33c8: eef54a40     	vcmp.f32	s9, #0
    33cc: eef1fa10     	vmrs	APSR_nzcv, fpscr
    33d0: 03a03007     	moveq	r3, #7
    33d4: 0a000060     	beq	0x355c <arbhar_play_tilde_midiNote+0x234> @ imm = #0x180
    33d8: ebfffed8     	bl	0x2f40 <_getChord>      @ imm = #-0x4a0
    33dc: e58406ec     	str	r0, [r4, #0x6ec]
    33e0: e8bd8010     	pop	{r4, pc}
    33e4: e284ee6f     	add	lr, r4, #1776
    33e8: edde0a00     	vldr	s1, [lr]
    33ec: eeb40a60     	vcmp.f32	s0, s1
    33f0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    33f4: 1a000004     	bne	0x340c <arbhar_play_tilde_midiNote+0xe4> @ imm = #0x10
    33f8: e3a0c000     	mov	r12, #0
    33fc: e3a02000     	mov	r2, #0
    3400: e58ec000     	str	r12, [lr]
    3404: e58ec004     	str	r12, [lr, #0x4]
    3408: e58426f8     	str	r2, [r4, #0x6f8]
    340c: e2841e6f     	add	r1, r4, #1776
    3410: e281e00c     	add	lr, r1, #12
    3414: ed915a03     	vldr	s10, [r1, #12]
    3418: eeb40a45     	vcmp.f32	s0, s10
    341c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3420: 1a000005     	bne	0x343c <arbhar_play_tilde_midiNote+0x114> @ imm = #0x14
    3424: e2803c27     	add	r3, r0, #9984
    3428: e3a01000     	mov	r1, #0
    342c: e3a0c000     	mov	r12, #0
    3430: e58e1000     	str	r1, [lr]
    3434: e5831000     	str	r1, [r3]
    3438: e584c704     	str	r12, [r4, #0x704]
    343c: e284ec07     	add	lr, r4, #1792
    3440: e28e3008     	add	r3, lr, #8
    3444: edde5a02     	vldr	s11, [lr, #8]
    3448: eeb40a65     	vcmp.f32	s0, s11
    344c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3450: 1a000005     	bne	0x346c <arbhar_play_tilde_midiNote+0x144> @ imm = #0x14
    3454: e2842c07     	add	r2, r4, #1792
    3458: e3a01000     	mov	r1, #0
    345c: e3a0c000     	mov	r12, #0
    3460: e5831000     	str	r1, [r3]
    3464: e582100c     	str	r1, [r2, #0xc]
    3468: e584c710     	str	r12, [r4, #0x710]
    346c: e284ee71     	add	lr, r4, #1808
    3470: e28e3004     	add	r3, lr, #4
    3474: ed9e6a01     	vldr	s12, [lr, #4]
    3478: eeb40a46     	vcmp.f32	s0, s12
    347c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3480: 1a000005     	bne	0x349c <arbhar_play_tilde_midiNote+0x174> @ imm = #0x14
    3484: e2842e71     	add	r2, r4, #1808
    3488: e3a01000     	mov	r1, #0
    348c: e3a0c000     	mov	r12, #0
    3490: e5831000     	str	r1, [r3]
    3494: e5821008     	str	r1, [r2, #0x8]
    3498: e584c71c     	str	r12, [r4, #0x71c]
    349c: e284ee72     	add	lr, r4, #1824
    34a0: edde6a00     	vldr	s13, [lr]
    34a4: eeb40a66     	vcmp.f32	s0, s13
    34a8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    34ac: 1a000004     	bne	0x34c4 <arbhar_play_tilde_midiNote+0x19c> @ imm = #0x10
    34b0: e3a03000     	mov	r3, #0
    34b4: e3a02000     	mov	r2, #0
    34b8: e58e3000     	str	r3, [lr]
    34bc: e58e3004     	str	r3, [lr, #0x4]
    34c0: e5842728     	str	r2, [r4, #0x728]
    34c4: e284ce72     	add	r12, r4, #1824
    34c8: e28ce00c     	add	lr, r12, #12
    34cc: ed9c7a03     	vldr	s14, [r12, #12]
    34d0: eeb40a47     	vcmp.f32	s0, s14
    34d4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    34d8: 1a000005     	bne	0x34f4 <arbhar_play_tilde_midiNote+0x1cc> @ imm = #0x14
    34dc: e2843e73     	add	r3, r4, #1840
    34e0: e3a01000     	mov	r1, #0
    34e4: e3a02000     	mov	r2, #0
    34e8: e58e1000     	str	r1, [lr]
    34ec: e5831000     	str	r1, [r3]
    34f0: e5842734     	str	r2, [r4, #0x734]
    34f4: e284ee73     	add	lr, r4, #1840
    34f8: e28e3008     	add	r3, lr, #8
    34fc: edde7a02     	vldr	s15, [lr, #8]
    3500: eeb40a67     	vcmp.f32	s0, s15
    3504: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3508: 1a000005     	bne	0x3524 <arbhar_play_tilde_midiNote+0x1fc> @ imm = #0x14
    350c: e2842e73     	add	r2, r4, #1840
    3510: e3a01000     	mov	r1, #0
    3514: e3a0c000     	mov	r12, #0
    3518: e5831000     	str	r1, [r3]
    351c: e582100c     	str	r1, [r2, #0xc]
    3520: e584c740     	str	r12, [r4, #0x740]
    3524: e284ed1d     	add	lr, r4, #1856
    3528: e28e3004     	add	r3, lr, #4
    352c: edde1a01     	vldr	s3, [lr, #4]
    3530: eef41a40     	vcmp.f32	s3, s0
    3534: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3538: 1affffa6     	bne	0x33d8 <arbhar_play_tilde_midiNote+0xb0> @ imm = #-0x168
    353c: e2842d1d     	add	r2, r4, #1856
    3540: e3a01000     	mov	r1, #0
    3544: e3a0c000     	mov	r12, #0
    3548: e5831000     	str	r1, [r3]
    354c: e5821008     	str	r1, [r2, #0x8]
    3550: e584c74c     	str	r12, [r4, #0x74c]
    3554: eaffff9f     	b	0x33d8 <arbhar_play_tilde_midiNote+0xb0> @ imm = #-0x184
    3558: e3a03006     	mov	r3, #6
    355c: eebd1ac1     	vcvt.s32.f32	s2, s2
    3560: e3a0100c     	mov	r1, #12
    3564: e02e0391     	mla	lr, r1, r3, r0
    3568: e302c6f8     	movw	r12, #0x26f8
    356c: e28e3d9b     	add	r3, lr, #9920
    3570: ee111a10     	vmov	r1, s2
    3574: ed830a0c     	vstr	s0, [r3, #48]
    3578: edc30a0d     	vstr	s1, [r3, #52]
    357c: e78e100c     	str	r1, [lr, r12]
    3580: eaffff94     	b	0x33d8 <arbhar_play_tilde_midiNote+0xb0> @ imm = #-0x1b0
    3584: e3a03004     	mov	r3, #4
    3588: eafffff3     	b	0x355c <arbhar_play_tilde_midiNote+0x234> @ imm = #-0x34
    358c: e3a03005     	mov	r3, #5
    3590: eafffff1     	b	0x355c <arbhar_play_tilde_midiNote+0x234> @ imm = #-0x3c
    3594: e3a03000     	mov	r3, #0
    3598: eaffffef     	b	0x355c <arbhar_play_tilde_midiNote+0x234> @ imm = #-0x44
    359c: e3a03001     	mov	r3, #1
    35a0: eaffffed     	b	0x355c <arbhar_play_tilde_midiNote+0x234> @ imm = #-0x4c
    35a4: e3a03002     	mov	r3, #2
    35a8: eaffffeb     	b	0x355c <arbhar_play_tilde_midiNote+0x234> @ imm = #-0x54
    35ac: e3a03003     	mov	r3, #3
    35b0: eaffffe9     	b	0x355c <arbhar_play_tilde_midiNote+0x234> @ imm = #-0x5c

