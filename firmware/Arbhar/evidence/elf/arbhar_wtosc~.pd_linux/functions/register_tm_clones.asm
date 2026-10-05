00002640 <register_tm_clones>:
    2640: e59f0038     	ldr	r0, [pc, #0x38]         @ 0x2680 <register_tm_clones+0x40>
    2644: e59f3038     	ldr	r3, [pc, #0x38]         @ 0x2684 <register_tm_clones+0x44>
    2648: e08f0000     	add	r0, pc, r0
    264c: e08f3003     	add	r3, pc, r3
    2650: e0431000     	sub	r1, r3, r0
    2654: e59f302c     	ldr	r3, [pc, #0x2c]         @ 0x2688 <register_tm_clones+0x48>
    2658: e1a01141     	asr	r1, r1, #2
    265c: e08f3003     	add	r3, pc, r3
    2660: e0811fa1     	add	r1, r1, r1, lsr #31
    2664: e1b010c1     	asrs	r1, r1, #1
    2668: 012fff1e     	bxeq	lr
    266c: e59f2018     	ldr	r2, [pc, #0x18]         @ 0x268c <register_tm_clones+0x4c>
    2670: e7933002     	ldr	r3, [r3, r2]
    2674: e3530000     	cmp	r3, #0
    2678: 012fff1e     	bxeq	lr
    267c: e12fff13     	bx	r3
    2680: 18 6b 01 00  	.word	0x00016b18
    2684: 14 6b 01 00  	.word	0x00016b14
    2688: 9c 69 01 00  	.word	0x0001699c
    268c: 24 01 00 00  	.word	0x00000124

