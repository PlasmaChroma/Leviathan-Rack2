; lubadh::TapManager::any_available() const
; VA 0x4dc80 size 120

   4dc80: e5901aa0     	ldr	r1, [r0, #0xaa0]
   4dc84: e2803eaa     	add	r3, r0, #2720
   4dc88: e2802eab     	add	r2, r0, #2736
   4dc8c: e2833004     	add	r3, r3, #4
   4dc90: e3510000     	cmp	r1, #0
   4dc94: 0a00000f     	beq	0x4dcd8
   4dc98: e5901aa4     	ldr	r1, [r0, #0xaa4]
   4dc9c: e2803eaa     	add	r3, r0, #2720
   4dca0: e2833008     	add	r3, r3, #8
   4dca4: e3510000     	cmp	r1, #0
   4dca8: 0a00000a     	beq	0x4dcd8
   4dcac: e5901aa8     	ldr	r1, [r0, #0xaa8]
   4dcb0: e2803eaa     	add	r3, r0, #2720
   4dcb4: e283300c     	add	r3, r3, #12
   4dcb8: e3510000     	cmp	r1, #0
   4dcbc: 0a000005     	beq	0x4dcd8
   4dcc0: e5901aac     	ldr	r1, [r0, #0xaac]
   4dcc4: e1a03002     	mov	r3, r2
   4dcc8: e3510000     	cmp	r1, #0
   4dccc: 0a000001     	beq	0x4dcd8
   4dcd0: e3a00000     	mov	r0, #0
   4dcd4: e12fff1e     	bx	lr
   4dcd8: e1520003     	cmp	r2, r3
   4dcdc: 0a000003     	beq	0x4dcf0
   4dce0: e2831004     	add	r1, r3, #4
   4dce4: e2833008     	add	r3, r3, #8
   4dce8: e1520001     	cmp	r2, r1
   4dcec: 1afffff9     	bne	0x4dcd8
   4dcf0: e3a00001     	mov	r0, #1
   4dcf4: e12fff1e     	bx	lr
