00004008 <writeToSharedMem>:
    4008: e281107a     	add	r1, r1, #122
    400c: e7903101     	ldr	r3, [r0, r1, lsl #2]
    4010: e0832102     	add	r2, r3, r2, lsl #2
    4014: ed820a00     	vstr	s0, [r2]
    4018: e12fff1e     	bx	lr

