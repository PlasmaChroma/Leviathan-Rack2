; lubadh::Channel::FileLoader::inMenu() const
; VA 0x39220 size 24

   39220: e5903008     	ldr	r3, [r0, #0x8]
   39224: e5930000     	ldr	r0, [r3]
   39228: e2400003     	sub	r0, r0, #3
   3922c: e16f0f10     	clz	r0, r0
   39230: e1a002a0     	lsr	r0, r0, #5
   39234: e12fff1e     	bx	lr
