0001078c <deregister_tm_clones>:
   1078c: e59f0018     	ldr	r0, [pc, #0x18]         @ 0x107ac <deregister_tm_clones+0x20>
   10790: e59f3018     	ldr	r3, [pc, #0x18]         @ 0x107b0 <deregister_tm_clones+0x24>
   10794: e1530000     	cmp	r3, r0
   10798: 012fff1e     	bxeq	lr
   1079c: e59f3010     	ldr	r3, [pc, #0x10]         @ 0x107b4 <deregister_tm_clones+0x28>
   107a0: e3530000     	cmp	r3, #0
   107a4: 012fff1e     	bxeq	lr
   107a8: e12fff13     	bx	r3
   107ac: 74 20 02 00  	.word	0x00022074
   107b0: 74 20 02 00  	.word	0x00022074
   107b4: 00 00 00 00  	.word	0x00000000

