0000252c <tanh_approx_tilde_new>:
    252c: e59f3054     	ldr	r3, [pc, #0x54]         @ 0x2588 <tanh_approx_tilde_new+0x5c>  // u32=0x15bdc; f32?=1.24788431e-40
    2530: e92d4070     	push	{r4, r5, r6, lr}
    2534: e08f0003     	add	r0, pc, r3
    2538: ed2d8b02     	vpush	{d8}
    253c: e5900000     	ldr	r0, [r0]
    2540: e59f5044     	ldr	r5, [pc, #0x44]         @ 0x258c <tanh_approx_tilde_new+0x60>  // u32=0x15aac; f32?=1.24362436e-40
    2544: eeb08a40     	vmov.f32	s16, s0
    2548: ebfffea1     	bl	0x1fd4 <.plt+0x8c>      @ imm = #-0x57c  // CALL pd_new
    254c: e08f5005     	add	r5, pc, r5
    2550: ee182a10     	vmov	r2, s16
    2554: e1a01000     	mov	r1, r0
    2558: e1a04000     	mov	r4, r0
    255c: e5a1201c     	str	r2, [r1, #0x1c]!
    2560: ebfffee9     	bl	0x210c <.plt+0x1c4>     @ imm = #-0x45c  // CALL floatinlet_new
    2564: e59f1024     	ldr	r1, [pc, #0x24]         @ 0x2590 <tanh_approx_tilde_new+0x64>  // u32=0xc0; f32?=2.69049305e-43
    2568: e5840028     	str	r0, [r4, #0x28]
    256c: e1a00004     	mov	r0, r4
    2570: e7951001     	ldr	r1, [r5, r1]
    2574: ebfffecf     	bl	0x20b8 <.plt+0x170>     @ imm = #-0x4c4  // CALL outlet_new
    2578: ecbd8b02     	vpop	{d8}
    257c: e584002c     	str	r0, [r4, #0x2c]
    2580: e1a00004     	mov	r0, r4
    2584: e8bd8070     	pop	{r4, r5, r6, pc}
    2588: dc 5b 01 00  	.word	0x00015bdc
    258c: ac 5a 01 00  	.word	0x00015aac
    2590: c0 00 00 00  	.word	0x000000c0

