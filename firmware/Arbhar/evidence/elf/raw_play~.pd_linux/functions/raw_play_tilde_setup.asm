00003030 <raw_play_tilde_setup>:
    3030: e59f0130     	ldr	r0, [pc, #0x130]        @ 0x3168 <raw_play_tilde_setup+0x138>
    3034: e92d40f0     	push	{r4, r5, r6, r7, lr}
    3038: e08f0000     	add	r0, pc, r0
    303c: e24dd00c     	sub	sp, sp, #12
    3040: e59f6124     	ldr	r6, [pc, #0x124]        @ 0x316c <raw_play_tilde_setup+0x13c>
    3044: ebfffbd6     	bl	0x1fa4 <.plt+0x14>      @ imm = #-0x10a8
    3048: e59f2120     	ldr	r2, [pc, #0x120]        @ 0x3170 <raw_play_tilde_setup+0x140>
    304c: e59f1120     	ldr	r1, [pc, #0x120]        @ 0x3174 <raw_play_tilde_setup+0x144>
    3050: e08f6006     	add	r6, pc, r6
    3054: e3a05000     	mov	r5, #0
    3058: e30c3a38     	movw	r3, #0xca38
    305c: e7962002     	ldr	r2, [r6, r2]
    3060: e3403008     	movt	r3, #0x8
    3064: e7961001     	ldr	r1, [r6, r1]
    3068: e58d5004     	str	r5, [sp, #0x4]
    306c: e58d5000     	str	r5, [sp]
    3070: ebfffc34     	bl	0x2148 <.plt+0x1b8>     @ imm = #-0xf30
    3074: e59f40fc     	ldr	r4, [pc, #0xfc]         @ 0x3178 <raw_play_tilde_setup+0x148>
    3078: e59f30fc     	ldr	r3, [pc, #0xfc]         @ 0x317c <raw_play_tilde_setup+0x14c>
    307c: e08f4004     	add	r4, pc, r4
    3080: e1a07000     	mov	r7, r0
    3084: e08f0003     	add	r0, pc, r3
    3088: e5847000     	str	r7, [r4]
    308c: ebfffbc4     	bl	0x1fa4 <.plt+0x14>      @ imm = #-0x10f0
    3090: e59fc0e8     	ldr	r12, [pc, #0xe8]        @ 0x3180 <raw_play_tilde_setup+0x150>
    3094: e3a0300a     	mov	r3, #10
    3098: e58d5000     	str	r5, [sp]
    309c: e08f100c     	add	r1, pc, r12
    30a0: e1a02000     	mov	r2, r0
    30a4: e1a00007     	mov	r0, r7
    30a8: ebfffc2f     	bl	0x216c <.plt+0x1dc>     @ imm = #-0xf44
    30ac: e59f00d0     	ldr	r0, [pc, #0xd0]         @ 0x3184 <raw_play_tilde_setup+0x154>
    30b0: e5947000     	ldr	r7, [r4]
    30b4: e08f0000     	add	r0, pc, r0
    30b8: ebfffbb9     	bl	0x1fa4 <.plt+0x14>      @ imm = #-0x111c
    30bc: e59f20c4     	ldr	r2, [pc, #0xc4]         @ 0x3188 <raw_play_tilde_setup+0x158>
    30c0: e3a03001     	mov	r3, #1
    30c4: e58d5000     	str	r5, [sp]
    30c8: e08f1002     	add	r1, pc, r2
    30cc: e1a02000     	mov	r2, r0
    30d0: e1a00007     	mov	r0, r7
    30d4: ebfffc24     	bl	0x216c <.plt+0x1dc>     @ imm = #-0xf70
    30d8: e59f10ac     	ldr	r1, [pc, #0xac]         @ 0x318c <raw_play_tilde_setup+0x15c>
    30dc: e5947000     	ldr	r7, [r4]
    30e0: e08f0001     	add	r0, pc, r1
    30e4: ebfffbae     	bl	0x1fa4 <.plt+0x14>      @ imm = #-0x1148
    30e8: e59fc0a0     	ldr	r12, [pc, #0xa0]        @ 0x3190 <raw_play_tilde_setup+0x160>
    30ec: e58d5000     	str	r5, [sp]
    30f0: e3a03001     	mov	r3, #1
    30f4: e08f100c     	add	r1, pc, r12
    30f8: e1a02000     	mov	r2, r0
    30fc: e1a00007     	mov	r0, r7
    3100: ebfffc19     	bl	0x216c <.plt+0x1dc>     @ imm = #-0xf9c
    3104: e59f3088     	ldr	r3, [pc, #0x88]         @ 0x3194 <raw_play_tilde_setup+0x164>
    3108: e5947000     	ldr	r7, [r4]
    310c: e08f0003     	add	r0, pc, r3
    3110: ebfffba3     	bl	0x1fa4 <.plt+0x14>      @ imm = #-0x1174
    3114: e1a03005     	mov	r3, r5
    3118: e1a02000     	mov	r2, r0
    311c: e59f0074     	ldr	r0, [pc, #0x74]         @ 0x3198 <raw_play_tilde_setup+0x168>
    3120: e08f1000     	add	r1, pc, r0
    3124: e1a00007     	mov	r0, r7
    3128: ebfffc0f     	bl	0x216c <.plt+0x1dc>     @ imm = #-0xfc4
    312c: e59f2068     	ldr	r2, [pc, #0x68]         @ 0x319c <raw_play_tilde_setup+0x16c>
    3130: e5947000     	ldr	r7, [r4]
    3134: e08f0002     	add	r0, pc, r2
    3138: ebfffb99     	bl	0x1fa4 <.plt+0x14>      @ imm = #-0x119c
    313c: e59f105c     	ldr	r1, [pc, #0x5c]         @ 0x31a0 <raw_play_tilde_setup+0x170>
    3140: e1a03005     	mov	r3, r5
    3144: e7961001     	ldr	r1, [r6, r1]
    3148: e1a02000     	mov	r2, r0
    314c: e1a00007     	mov	r0, r7
    3150: ebfffc05     	bl	0x216c <.plt+0x1dc>     @ imm = #-0xfec
    3154: e5940000     	ldr	r0, [r4]
    3158: e3a01024     	mov	r1, #36
    315c: e28dd00c     	add	sp, sp, #12
    3160: e8bd40f0     	pop	{r4, r5, r6, r7, lr}
    3164: eafffbfa     	b	0x2154 <.plt+0x1c4>     @ imm = #-0x1018
    3168: 1c 46 00 00  	.word	0x0000461c
    316c: a8 4f 01 00  	.word	0x00014fa8
    3170: cc 00 00 00  	.word	0x000000cc
    3174: c0 00 00 00  	.word	0x000000c0
    3178: a0 50 01 00  	.word	0x000150a0
    317c: dc 45 00 00  	.word	0x000045dc
    3180: 30 f6 ff ff  	.word	0xfffff630
    3184: b4 45 00 00  	.word	0x000045b4
    3188: 00 f2 ff ff  	.word	0xfffff200
    318c: 90 45 00 00  	.word	0x00004590
    3190: d8 f6 ff ff  	.word	0xfffff6d8
    3194: 6c 45 00 00  	.word	0x0000456c
    3198: 90 f5 ff ff  	.word	0xfffff590
    319c: 50 45 00 00  	.word	0x00004550
    31a0: dc 00 00 00  	.word	0x000000dc

