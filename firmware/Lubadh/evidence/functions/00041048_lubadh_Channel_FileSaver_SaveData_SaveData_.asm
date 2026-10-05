; lubadh::Channel::FileSaver::SaveData::~SaveData()
; VA 0x41048 size 60

   41048: e59f302c     	ldr	r3, [pc, #0x2c]         @ 0x4107c
   4104c: e92d4010     	push	{r4, lr}
   41050: e1a04000     	mov	r4, r0
   41054: e5a03040     	str	r3, [r0, #0x40]!
   41058: ebffb857     	bl	0x2f1bc
   4105c: e1a00004     	mov	r0, r4
   41060: e59f3018     	ldr	r3, [pc, #0x18]         @ 0x41080
   41064: e5a0301c     	str	r3, [r0, #0x1c]!
   41068: ebffb853     	bl	0x2f1bc
   4106c: e1a00004     	mov	r0, r4
   41070: eb00bd06     	bl	0x70490
   41074: e1a00004     	mov	r0, r4
   41078: e8bd8010     	pop	{r4, pc}
   4107c: c0 1f 07 00  	.word	0x00071fc0
   41080: e0 1e 07 00  	.word	0x00071ee0
