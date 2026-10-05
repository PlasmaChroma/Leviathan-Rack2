; lubadh::Channel::AudioData::at(int) const
; VA 0x38728 size 32

   38728: e3023a2f     	movw	r3, #0x2a2f
   3872c: e34031c2     	movt	r3, #0x1c2
   38730: e1510003     	cmp	r1, r3
   38734: e5900048     	ldr	r0, [r0, #0x48]
   38738: a1a01003     	movge	r1, r3
   3873c: e1c11fc1     	bic	r1, r1, r1, asr #31
   38740: e0800101     	add	r0, r0, r1, lsl #2
   38744: e12fff1e     	bx	lr
