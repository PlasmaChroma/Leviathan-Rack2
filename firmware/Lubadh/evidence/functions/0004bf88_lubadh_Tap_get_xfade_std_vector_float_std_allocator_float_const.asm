; lubadh::Tap::get_xfade(std::vector<float, std::allocator<float> >&) const
; VA 0x4bf88 size 24

   4bf88: e5912004     	ldr	r2, [r1, #0x4]
   4bf8c: e5913000     	ldr	r3, [r1]
   4bf90: e0422003     	sub	r2, r2, r3
   4bf94: e1b02142     	asrs	r2, r2, #2
   4bf98: 012fff1e     	bxeq	lr
   4bf9c: eafffdd7     	b	0x4b700
