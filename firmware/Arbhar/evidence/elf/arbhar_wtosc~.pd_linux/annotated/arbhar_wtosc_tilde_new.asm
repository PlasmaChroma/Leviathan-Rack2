00003508 <arbhar_wtosc_tilde_new>:
    3508: e59f322c     	ldr	r3, [pc, #0x22c]        @ 0x373c <arbhar_wtosc_tilde_new+0x234>  // u32=0x15c54; f32?=1.24956587e-40
    350c: e92d41f0     	push	{r4, r5, r6, r7, r8, lr}
    3510: e08f0003     	add	r0, pc, r3
    3514: ed2d8b02     	vpush	{d8}
    3518: e1a07002     	mov	r7, r2
    351c: e590000c     	ldr	r0, [r0, #0xc]
    3520: e3a05000     	mov	r5, #0
    3524: ebfffb91     	bl	0x2370 <.plt+0x8c>      @ imm = #-0x11bc  // CALL pd_new
    3528: e3a08902     	mov	r8, #32768
    352c: ed9f8a73     	vldr	s16, [pc, #460]         @ 0x3700 <arbhar_wtosc_tilde_new+0x1f8>  // f32=624000
    3530: e344873b     	movt	r8, #0x473b
    3534: e1a04000     	mov	r4, r0
    3538: e1a00007     	mov	r0, r7
    353c: ebfffc15     	bl	0x2598 <.plt+0x2b4>     @ imm = #-0xfac  // CALL atom_getsymbol
    3540: e1a06000     	mov	r6, r0
    3544: e2870008     	add	r0, r7, #8
    3548: ebfffbb2     	bl	0x2418 <.plt+0x134>     @ imm = #-0x1138  // CALL atom_getfloat
    354c: e1a01005     	mov	r1, r5
    3550: e58450c8     	str	r5, [r4, #0xc8]
    3554: e1a00004     	mov	r0, r4
    3558: eddf0a69     	vldr	s1, [pc, #420]          @ 0x3704 <arbhar_wtosc_tilde_new+0x1fc>  // f32=250
    355c: e3a07001     	mov	r7, #1
    3560: eef08a40     	vmov.f32	s17, s0
    3564: ed9f0a67     	vldr	s0, [pc, #412]          @ 0x3708 <arbhar_wtosc_tilde_new+0x200>  // f32=61019
    3568: ebfffbd1     	bl	0x24b4 <.plt+0x1d0>     @ imm = #-0x10bc  // CALL _memAllocate
    356c: e1a00004     	mov	r0, r4
    3570: e3a01001     	mov	r1, #1
    3574: eef00a48     	vmov.f32	s1, s16
    3578: ed9f0a63     	vldr	s0, [pc, #396]          @ 0x370c <arbhar_wtosc_tilde_new+0x204>  // f32=90521
    357c: ebfffbcc     	bl	0x24b4 <.plt+0x1d0>     @ imm = #-0x10d0  // CALL _memAllocate
    3580: e1a00004     	mov	r0, r4
    3584: e3a01002     	mov	r1, #2
    3588: eef00a48     	vmov.f32	s1, s16
    358c: ed9f0a5f     	vldr	s0, [pc, #380]          @ 0x3710 <arbhar_wtosc_tilde_new+0x208>  // f32=90522
    3590: ebfffbc7     	bl	0x24b4 <.plt+0x1d0>     @ imm = #-0x10e4  // CALL _memAllocate
    3594: e1a00004     	mov	r0, r4
    3598: e3a01003     	mov	r1, #3
    359c: eef00a48     	vmov.f32	s1, s16
    35a0: ed9f0a5b     	vldr	s0, [pc, #364]          @ 0x3714 <arbhar_wtosc_tilde_new+0x20c>  // f32=90523
    35a4: ebfffbc2     	bl	0x24b4 <.plt+0x1d0>     @ imm = #-0x10f8  // CALL _memAllocate
    35a8: e1a00004     	mov	r0, r4
    35ac: e3a01004     	mov	r1, #4
    35b0: eef00a48     	vmov.f32	s1, s16
    35b4: ed9f0a57     	vldr	s0, [pc, #348]          @ 0x3718 <arbhar_wtosc_tilde_new+0x210>  // f32=90524
    35b8: ebfffbbd     	bl	0x24b4 <.plt+0x1d0>     @ imm = #-0x110c  // CALL _memAllocate
    35bc: e1a00004     	mov	r0, r4
    35c0: e3a01005     	mov	r1, #5
    35c4: eef00a48     	vmov.f32	s1, s16
    35c8: ed9f0a53     	vldr	s0, [pc, #332]          @ 0x371c <arbhar_wtosc_tilde_new+0x214>  // f32=90525
    35cc: ebfffbb8     	bl	0x24b4 <.plt+0x1d0>     @ imm = #-0x1120  // CALL _memAllocate
    35d0: e1a00004     	mov	r0, r4
    35d4: e3a01006     	mov	r1, #6
    35d8: eef00a48     	vmov.f32	s1, s16
    35dc: ed9f0a4f     	vldr	s0, [pc, #316]          @ 0x3720 <arbhar_wtosc_tilde_new+0x218>  // f32=90526
    35e0: ebfffbb3     	bl	0x24b4 <.plt+0x1d0>     @ imm = #-0x1134  // CALL _memAllocate
    35e4: e1a00004     	mov	r0, r4
    35e8: e3a01007     	mov	r1, #7
    35ec: eef00a48     	vmov.f32	s1, s16
    35f0: ed9f0a4b     	vldr	s0, [pc, #300]          @ 0x3724 <arbhar_wtosc_tilde_new+0x21c>  // f32=90527
    35f4: ebfffbae     	bl	0x24b4 <.plt+0x1d0>     @ imm = #-0x1148  // CALL _memAllocate
    35f8: e1a00004     	mov	r0, r4
    35fc: e3a01008     	mov	r1, #8
    3600: eef00a48     	vmov.f32	s1, s16
    3604: ed9f0a47     	vldr	s0, [pc, #284]          @ 0x3728 <arbhar_wtosc_tilde_new+0x220>  // f32=90528
    3608: ebfffba9     	bl	0x24b4 <.plt+0x1d0>     @ imm = #-0x115c  // CALL _memAllocate
    360c: e1a00004     	mov	r0, r4
    3610: e3a01009     	mov	r1, #9
    3614: eef00a48     	vmov.f32	s1, s16
    3618: ed9f0a43     	vldr	s0, [pc, #268]          @ 0x372c <arbhar_wtosc_tilde_new+0x224>  // f32=90529
    361c: ebfffba4     	bl	0x24b4 <.plt+0x1d0>     @ imm = #-0x1170  // CALL _memAllocate
    3620: e1a00004     	mov	r0, r4
    3624: e3a0100a     	mov	r1, #10
    3628: eef00a48     	vmov.f32	s1, s16
    362c: ed9f0a3f     	vldr	s0, [pc, #252]          @ 0x3730 <arbhar_wtosc_tilde_new+0x228>  // f32=90530
    3630: ebfffb9f     	bl	0x24b4 <.plt+0x1d0>     @ imm = #-0x1184  // CALL _memAllocate
    3634: e1a00004     	mov	r0, r4
    3638: e3a0100b     	mov	r1, #11
    363c: eef00a48     	vmov.f32	s1, s16
    3640: ed9f0a3b     	vldr	s0, [pc, #236]          @ 0x3734 <arbhar_wtosc_tilde_new+0x22c>  // f32=90531
    3644: ebfffb9a     	bl	0x24b4 <.plt+0x1d0>     @ imm = #-0x1198  // CALL _memAllocate
    3648: e1a00004     	mov	r0, r4
    364c: e3a0100c     	mov	r1, #12
    3650: eef00a48     	vmov.f32	s1, s16
    3654: ed9f0a37     	vldr	s0, [pc, #220]          @ 0x3738 <arbhar_wtosc_tilde_new+0x230>  // f32=90532
    3658: ebfffb95     	bl	0x24b4 <.plt+0x1d0>     @ imm = #-0x11ac  // CALL _memAllocate
    365c: e59f10dc     	ldr	r1, [pc, #0xdc]         @ 0x3740 <arbhar_wtosc_tilde_new+0x238>  // u32=0x54c; f32?=1.90016072e-42
    3660: e3a0243b     	mov	r2, #989855744
    3664: e3a0c311     	mov	r12, #1140850688
    3668: e08f1001     	add	r1, pc, r1
    366c: e584207c     	str	r2, [r4, #0x7c]
    3670: e5846084     	str	r6, [r4, #0x84]
    3674: e1a00004     	mov	r0, r4
    3678: e5845080     	str	r5, [r4, #0x80]
    367c: e584c078     	str	r12, [r4, #0x78]
    3680: e584801c     	str	r8, [r4, #0x1c]
    3684: ebfffb75     	bl	0x2460 <.plt+0x17c>     @ imm = #-0x122c  // CALL clock_new
    3688: e3e03102     	mvn	r3, #-2147483648
    368c: e5846030     	str	r6, [r4, #0x30]
    3690: e303c958     	movw	r12, #0x3958
    3694: e5845038     	str	r5, [r4, #0x38]
    3698: e343cbb4     	movt	r12, #0x3bb4
    369c: e5843020     	str	r3, [r4, #0x20]
    36a0: e3a06001     	mov	r6, #1
    36a4: edc48a19     	vstr	s17, [r4, #100]
    36a8: e3a055fe     	mov	r5, #1065353216
    36ac: e1c465f0     	strd	r6, r7, [r4, #80]
    36b0: e3a06000     	mov	r6, #0
    36b4: e584c058     	str	r12, [r4, #0x58]
    36b8: e30c7ccd     	movw	r7, #0xcccd
    36bc: e5845060     	str	r5, [r4, #0x60]
    36c0: e3437d4c     	movt	r7, #0x3d4c
    36c4: e5846034     	str	r6, [r4, #0x34]
    36c8: e584705c     	str	r7, [r4, #0x5c]
    36cc: e584606c     	str	r6, [r4, #0x6c]
    36d0: e5846068     	str	r6, [r4, #0x68]
    36d4: e5846048     	str	r6, [r4, #0x48]
    36d8: e5840074     	str	r0, [r4, #0x74]
    36dc: e59f0060     	ldr	r0, [pc, #0x60]         @ 0x3744 <arbhar_wtosc_tilde_new+0x23c>  // u32=0x4f44; f32?=2.84351484e-41
    36e0: e08f0000     	add	r0, pc, r0
    36e4: ebfffb03     	bl	0x22f8 <.plt+0x14>      @ imm = #-0x13f4  // CALL gensym
    36e8: e1a01000     	mov	r1, r0
    36ec: e1a00004     	mov	r0, r4
    36f0: ebfffb96     	bl	0x2550 <.plt+0x26c>     @ imm = #-0x11a8  // CALL outlet_new
    36f4: ecbd8b02     	vpop	{d8}
    36f8: e1a00004     	mov	r0, r4
    36fc: e8bd81f0     	pop	{r4, r5, r6, r7, r8, pc}
    3700: 00 58 18 49  	.word	0x49185800
    3704: 00 00 7a 43  	.word	0x437a0000
    3708: 00 5b 6e 47  	.word	0x476e5b00
    370c: 80 cc b0 47  	.word	0x47b0cc80
    3710: 00 cd b0 47  	.word	0x47b0cd00
    3714: 80 cd b0 47  	.word	0x47b0cd80
    3718: 00 ce b0 47  	.word	0x47b0ce00
    371c: 80 ce b0 47  	.word	0x47b0ce80
    3720: 00 cf b0 47  	.word	0x47b0cf00
    3724: 80 cf b0 47  	.word	0x47b0cf80
    3728: 00 d0 b0 47  	.word	0x47b0d000
    372c: 80 d0 b0 47  	.word	0x47b0d080
    3730: 00 d1 b0 47  	.word	0x47b0d100
    3734: 80 d1 b0 47  	.word	0x47b0d180
    3738: 00 d2 b0 47  	.word	0x47b0d200
    373c: 54 5c 01 00  	.word	0x00015c54
    3740: 4c 05 00 00  	.word	0x0000054c
    3744: 44 4f 00 00  	.word	0x00004f44

