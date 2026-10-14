0000b398 <_setRecordingStateWithReset>:
    b398: e3510000     	cmp	r1, #0
    b39c: e5c01024     	strb	r1, [r0, #0x24]
    b3a0: 13a03000     	movne	r3, #0
    b3a4: 1580302c     	strne	r3, [r0, #0x2c]
    b3a8: eaffe0e4     	b	0x3740 <.plt+0x44>      @ imm = #-0x7c70

