0000619c <_checkAnyButtonPressed>:
    619c: e2801034     	add	r1, r0, #52
    61a0: e891000e     	ldm	r1, {r1, r2, r3}
    61a4: e5d10001     	ldrb	r0, [r1, #0x1]
    61a8: e5d22001     	ldrb	r2, [r2, #0x1]
    61ac: e5d33001     	ldrb	r3, [r3, #0x1]
    61b0: e080c002     	add	r12, r0, r2
    61b4: e6e3107c     	uxtab	r1, r3, r12
    61b8: e6ef0071     	uxtb	r0, r1
    61bc: e2900000     	adds	r0, r0, #0
    61c0: 13a00001     	movne	r0, #1
    61c4: e12fff1e     	bx	lr

