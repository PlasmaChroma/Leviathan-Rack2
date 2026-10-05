00005928 <_setTriggerOut>:
    5928: eefc7ac0     	vcvt.u32.f32	s15, s0
    592c: e3a00022     	mov	r0, #34
    5930: ee171a90     	vmov	r1, s15
    5934: e2011001     	and	r1, r1, #1
    5938: eafff33d     	b	0x2634 <.plt+0x254>     @ imm = #-0x330c

