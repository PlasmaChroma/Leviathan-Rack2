; lubadh::Channel::AudioData::at(int)
; VA 0x38708 size 32

   38708: e3023a2f     	movw	r3, #0x2a2f
   3870c: e34031c2     	movt	r3, #0x1c2
   38710: e1510003     	cmp	r1, r3
   38714: e5900048     	ldr	r0, [r0, #0x48]
   38718: a1a01003     	movge	r1, r3
   3871c: e1c11fc1     	bic	r1, r1, r1, asr #31
   38720: e0800101     	add	r0, r0, r1, lsl #2
   38724: e12fff1e     	bx	lr
