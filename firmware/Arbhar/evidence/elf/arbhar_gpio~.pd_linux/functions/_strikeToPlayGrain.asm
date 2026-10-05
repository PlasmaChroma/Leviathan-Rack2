0000bce0 <_strikeToPlayGrain>:
    bce0: e5d03054     	ldrb	r3, [r0, #0x54]
    bce4: e3a02004     	mov	r2, #4
    bce8: e5c02104     	strb	r2, [r0, #0x104]
    bcec: e3530000     	cmp	r3, #0
    bcf0: 13a03000     	movne	r3, #0
    bcf4: 15803058     	strne	r3, [r0, #0x58]
    bcf8: e12fff1e     	bx	lr

