; lubadh::Channel::isPosLegal(int) const
; VA 0x37a10 size 52

   37a10: e59030e8     	ldr	r3, [r0, #0xe8]
   37a14: e5932014     	ldr	r2, [r3, #0x14]
   37a18: e593301c     	ldr	r3, [r3, #0x1c]
   37a1c: e1510002     	cmp	r1, r2
   37a20: d3a00000     	movle	r0, #0
   37a24: c3a00001     	movgt	r0, #1
   37a28: e1510003     	cmp	r1, r3
   37a2c: c3a01000     	movgt	r1, #0
   37a30: d3a01001     	movle	r1, #1
   37a34: e1520003     	cmp	r2, r3
   37a38: b0000001     	andlt	r0, r0, r1
   37a3c: a1810000     	orrge	r0, r1, r0
   37a40: e12fff1e     	bx	lr
