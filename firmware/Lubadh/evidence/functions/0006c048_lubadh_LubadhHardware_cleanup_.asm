; lubadh::LubadhHardware::cleanup()
; VA 0x6c048 size 28

   6c048: e92d4010     	push	{r4, lr}
   6c04c: e590003c     	ldr	r0, [r0, #0x3c]
   6c050: ebfea891     	bl	0x1629c    @ imm = #-0x55dbc ; close
   6c054: eb00058e     	bl	0x6d694
   6c058: eb000d08     	bl	0x6f480
   6c05c: e3a00000     	mov	r0, #0
   6c060: e8bd8010     	pop	{r4, pc}
