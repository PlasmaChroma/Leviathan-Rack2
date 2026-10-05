00000564 <midi_reader_setup>:
     564: e59f004c     	ldr	r0, [pc, #0x4c]         @ 0x5b8 <midi_reader_setup+0x54>  // u32=0x74; f32?=1.62550622e-43
     568: e52de004     	str	lr, [sp, #-0x4]!
     56c: e08f0000     	add	r0, pc, r0
     570: e24dd00c     	sub	sp, sp, #12
     574: ebffff92     	bl	0x3c4 <.plt+0x14>       @ imm = #-0x1b8  // CALL gensym
     578: e59f103c     	ldr	r1, [pc, #0x3c]         @ 0x5bc <midi_reader_setup+0x58>  // u32=0x10a70; f32?=9.55797657e-41
     57c: e59fc03c     	ldr	r12, [pc, #0x3c]        @ 0x5c0 <midi_reader_setup+0x5c>  // u32=0x30; f32?=6.72623263e-44
     580: e3a0e000     	mov	lr, #0
     584: e59f2038     	ldr	r2, [pc, #0x38]         @ 0x5c4 <midi_reader_setup+0x60>  // u32=0x2c; f32?=6.16571324e-44
     588: e08f1001     	add	r1, pc, r1
     58c: e3a03024     	mov	r3, #36
     590: e7912002     	ldr	r2, [r1, r2]
     594: e791100c     	ldr	r1, [r1, r12]
     598: e58de004     	str	lr, [sp, #0x4]
     59c: e58de000     	str	lr, [sp]
     5a0: ebffff96     	bl	0x400 <.plt+0x50>       @ imm = #-0x1a8  // CALL class_new
     5a4: e59f301c     	ldr	r3, [pc, #0x1c]         @ 0x5c8 <midi_reader_setup+0x64>  // u32=0x10a94; f32?=9.56302124e-41
     5a8: e08f1003     	add	r1, pc, r3
     5ac: e5810000     	str	r0, [r1]
     5b0: e28dd00c     	add	sp, sp, #12
     5b4: e49df004     	ldr	pc, [sp], #4
     5b8: 74 00 00 00  	.word	0x00000074
     5bc: 70 0a 01 00  	.word	0x00010a70
     5c0: 30 00 00 00  	.word	0x00000030
     5c4: 2c 00 00 00  	.word	0x0000002c
     5c8: 94 0a 01 00  	.word	0x00010a94

Disassembly of section .fini:

