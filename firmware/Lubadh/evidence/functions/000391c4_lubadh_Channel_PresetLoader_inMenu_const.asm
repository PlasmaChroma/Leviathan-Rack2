; lubadh::Channel::PresetLoader::inMenu() const
; VA 0x391c4 size 24

   391c4: e5903144     	ldr	r3, [r0, #0x144]
   391c8: e5930000     	ldr	r0, [r3]
   391cc: e2400003     	sub	r0, r0, #3
   391d0: e16f0f10     	clz	r0, r0
   391d4: e1a002a0     	lsr	r0, r0, #5
   391d8: e12fff1e     	bx	lr
