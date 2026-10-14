000009bc <shmem_clear>:
     9bc: e5902028     	ldr	r2, [r0, #0x28]
     9c0: e3520000     	cmp	r2, #0
     9c4: d12fff1e     	bxle	lr
     9c8: e1a02102     	lsl	r2, r2, #2
     9cc: e5900024     	ldr	r0, [r0, #0x24]
     9d0: e3a01000     	mov	r1, #0
     9d4: eaffff8e     	b	0x814 <.plt+0xa4>       @ imm = #-0x1c8  // CALL memset

