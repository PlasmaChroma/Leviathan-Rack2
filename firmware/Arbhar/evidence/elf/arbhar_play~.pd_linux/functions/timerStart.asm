00005fc8 <timerStart>:
    5fc8: e59f3010     	ldr	r3, [pc, #0x10]         @ 0x5fe0 <timerStart+0x18>
    5fcc: e3a01000     	mov	r1, #0
    5fd0: e59f200c     	ldr	r2, [pc, #0xc]          @ 0x5fe4 <timerStart+0x1c>
    5fd4: e08f3003     	add	r3, pc, r3
    5fd8: e7930002     	ldr	r0, [r3, r2]
    5fdc: eafff14f     	b	0x2520 <.plt+0x140>     @ imm = #-0x3ac4
    5fe0: 24 80 01 00  	.word	0x00018024
    5fe4: 2c 01 00 00  	.word	0x0000012c

