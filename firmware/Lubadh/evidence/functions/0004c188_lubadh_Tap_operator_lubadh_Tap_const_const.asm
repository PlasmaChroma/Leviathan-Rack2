; lubadh::Tap::operator==(lubadh::Tap const&) const
; VA 0x4c188 size 40

   4c188: e590207c     	ldr	r2, [r0, #0x7c]
   4c18c: e591307c     	ldr	r3, [r1, #0x7c]
   4c190: e1520003     	cmp	r2, r3
   4c194: 05900080     	ldreq	r0, [r0, #0x80]
   4c198: 05913080     	ldreq	r3, [r1, #0x80]
   4c19c: 00400003     	subeq	r0, r0, r3
   4c1a0: 016f0f10     	clzeq	r0, r0
   4c1a4: 01a002a0     	lsreq	r0, r0, #5
   4c1a8: 13a00000     	movne	r0, #0
   4c1ac: e12fff1e     	bx	lr
