00001ca4 <shell_setup>:
    1ca4: e59f0068     	ldr	r0, [pc, #0x68]         @ 0x1d14 <shell_setup+0x70>  // u32=0x164; f32?=4.98862253e-43
    1ca8: e92d4030     	push	{r4, r5, lr}
    1cac: e08f0000     	add	r0, pc, r0
    1cb0: e24dd00c     	sub	sp, sp, #12
    1cb4: e59f405c     	ldr	r4, [pc, #0x5c]         @ 0x1d18 <shell_setup+0x74>  // u32=0x10338; f32?=9.29901661e-41
    1cb8: ebfffb98     	bl	0xb20 <.plt+0x14>       @ imm = #-0x11a0  // CALL gensym
    1cbc: e59f2058     	ldr	r2, [pc, #0x58]         @ 0x1d1c <shell_setup+0x78>  // u32=0xec; f32?=3.30706438e-43
    1cc0: e08f4004     	add	r4, pc, r4
    1cc4: e59f1054     	ldr	r1, [pc, #0x54]         @ 0x1d20 <shell_setup+0x7c>  // u32=0xfffff824; f32?=nan
    1cc8: e3a0c000     	mov	r12, #0
    1ccc: e3a03050     	mov	r3, #80
    1cd0: e7942002     	ldr	r2, [r4, r2]
    1cd4: e08f1001     	add	r1, pc, r1
    1cd8: e58dc004     	str	r12, [sp, #0x4]
    1cdc: e58dc000     	str	r12, [sp]
    1ce0: ebfffbf4     	bl	0xcb8 <.plt+0x1ac>      @ imm = #-0x1030  // CALL class_new
    1ce4: e59f5038     	ldr	r5, [pc, #0x38]         @ 0x1d24 <shell_setup+0x80>  // u32=0x10404; f32?=9.3276031e-41
    1ce8: e59f3038     	ldr	r3, [pc, #0x38]         @ 0x1d28 <shell_setup+0x84>  // u32=0xd4; f32?=2.97075274e-43
    1cec: e08f5005     	add	r5, pc, r5
    1cf0: e5850000     	str	r0, [r5]
    1cf4: e7941003     	ldr	r1, [r4, r3]
    1cf8: ebfffba6     	bl	0xb98 <.plt+0x8c>       @ imm = #-0x1168  // CALL class_addbang
    1cfc: e59f4028     	ldr	r4, [pc, #0x28]         @ 0x1d2c <shell_setup+0x88>  // u32=0xfffffbf8; f32?=nan
    1d00: e5950000     	ldr	r0, [r5]
    1d04: e08f1004     	add	r1, pc, r4
    1d08: e28dd00c     	add	sp, sp, #12
    1d0c: e8bd4030     	pop	{r4, r5, lr}
    1d10: eafffbfa     	b	0xd00 <.plt+0x1f4>      @ imm = #-0x1018  // CALL class_addanything
    1d14: 64 01 00 00  	.word	0x00000164
    1d18: 38 03 01 00  	.word	0x00010338
    1d1c: ec 00 00 00  	.word	0x000000ec
    1d20: 24 f8 ff ff  	.word	0xfffff824
    1d24: 04 04 01 00  	.word	0x00010404
    1d28: d4 00 00 00  	.word	0x000000d4
    1d2c: f8 fb ff ff  	.word	0xfffffbf8

Disassembly of section .fini:

