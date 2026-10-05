; lubadh::OnePole::OnePole()
; VA 0x4b09c size 20

   4b09c: f2c00010     	vmov.i32	d16, #0x0
   4b0a0: e3a02000     	mov	r2, #0
   4b0a4: e5802008     	str	r2, [r0, #0x8]
   4b0a8: f440078f     	vst1.32	{d16}, [r0]
   4b0ac: e12fff1e     	bx	lr
