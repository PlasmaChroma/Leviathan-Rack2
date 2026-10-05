; lubadh::LubadhHardware::~LubadhHardware()
; VA 0x6c028 size 32

   6c028: e92d4010     	push	{r4, lr}
   6c02c: e1a04000     	mov	r4, r0
   6c030: e590003c     	ldr	r0, [r0, #0x3c]
   6c034: ebfea898     	bl	0x1629c    @ imm = #-0x55da0 ; close
   6c038: eb000595     	bl	0x6d694
   6c03c: eb000d0f     	bl	0x6f480
   6c040: e1a00004     	mov	r0, r4
   6c044: e8bd8010     	pop	{r4, pc}
