00002f88 <stereo_in_tilde_setup>:
    2f88: e59f01b4     	ldr	r0, [pc, #0x1b4]        @ 0x3144 <stereo_in_tilde_setup+0x1bc>
    2f8c: e92d40f0     	push	{r4, r5, r6, r7, lr}
    2f90: e08f0000     	add	r0, pc, r0
    2f94: e24dd00c     	sub	sp, sp, #12
    2f98: e3a05000     	mov	r5, #0
    2f9c: ebfffc35     	bl	0x2078 <.plt+0x14>      @ imm = #-0xf2c
    2fa0: e59f21a0     	ldr	r2, [pc, #0x1a0]        @ 0x3148 <stereo_in_tilde_setup+0x1c0>
    2fa4: e59f11a0     	ldr	r1, [pc, #0x1a0]        @ 0x314c <stereo_in_tilde_setup+0x1c4>
    2fa8: e3a03058     	mov	r3, #88
    2fac: e08f2002     	add	r2, pc, r2
    2fb0: e58d5004     	str	r5, [sp, #0x4]
    2fb4: e58d5000     	str	r5, [sp]
    2fb8: e08f1001     	add	r1, pc, r1
    2fbc: ebfffc96     	bl	0x221c <.plt+0x1b8>     @ imm = #-0xda8
    2fc0: e59f4188     	ldr	r4, [pc, #0x188]        @ 0x3150 <stereo_in_tilde_setup+0x1c8>
    2fc4: e3a0101c     	mov	r1, #28
    2fc8: e59f6184     	ldr	r6, [pc, #0x184]        @ 0x3154 <stereo_in_tilde_setup+0x1cc>
    2fcc: e08f4004     	add	r4, pc, r4
    2fd0: e08f6006     	add	r6, pc, r6
    2fd4: e5840000     	str	r0, [r4]
    2fd8: ebfffc92     	bl	0x2228 <.plt+0x1c4>     @ imm = #-0xdb8
    2fdc: e59f3174     	ldr	r3, [pc, #0x174]        @ 0x3158 <stereo_in_tilde_setup+0x1d0>
    2fe0: e5947000     	ldr	r7, [r4]
    2fe4: e08f0003     	add	r0, pc, r3
    2fe8: ebfffc22     	bl	0x2078 <.plt+0x14>      @ imm = #-0xf78
    2fec: e59fc168     	ldr	r12, [pc, #0x168]       @ 0x315c <stereo_in_tilde_setup+0x1d4>
    2ff0: e3a03006     	mov	r3, #6
    2ff4: e796100c     	ldr	r1, [r6, r12]
    2ff8: e58d5000     	str	r5, [sp]
    2ffc: e1a02000     	mov	r2, r0
    3000: e1a00007     	mov	r0, r7
    3004: ebfffc8d     	bl	0x2240 <.plt+0x1dc>     @ imm = #-0xdcc
    3008: e59f0150     	ldr	r0, [pc, #0x150]        @ 0x3160 <stereo_in_tilde_setup+0x1d8>
    300c: e5947000     	ldr	r7, [r4]
    3010: e08f0000     	add	r0, pc, r0
    3014: ebfffc17     	bl	0x2078 <.plt+0x14>      @ imm = #-0xfa4
    3018: e59f2144     	ldr	r2, [pc, #0x144]        @ 0x3164 <stereo_in_tilde_setup+0x1dc>
    301c: e3a03006     	mov	r3, #6
    3020: e7961002     	ldr	r1, [r6, r2]
    3024: e58d5000     	str	r5, [sp]
    3028: e1a02000     	mov	r2, r0
    302c: e1a00007     	mov	r0, r7
    3030: ebfffc82     	bl	0x2240 <.plt+0x1dc>     @ imm = #-0xdf8
    3034: e59f112c     	ldr	r1, [pc, #0x12c]        @ 0x3168 <stereo_in_tilde_setup+0x1e0>
    3038: e5947000     	ldr	r7, [r4]
    303c: e08f0001     	add	r0, pc, r1
    3040: ebfffc0c     	bl	0x2078 <.plt+0x14>      @ imm = #-0xfd0
    3044: e59fc120     	ldr	r12, [pc, #0x120]       @ 0x316c <stereo_in_tilde_setup+0x1e4>
    3048: e3a03006     	mov	r3, #6
    304c: e796100c     	ldr	r1, [r6, r12]
    3050: e58d5000     	str	r5, [sp]
    3054: e1a02000     	mov	r2, r0
    3058: e1a00007     	mov	r0, r7
    305c: ebfffc77     	bl	0x2240 <.plt+0x1dc>     @ imm = #-0xe24
    3060: e59f3108     	ldr	r3, [pc, #0x108]        @ 0x3170 <stereo_in_tilde_setup+0x1e8>
    3064: e5947000     	ldr	r7, [r4]
    3068: e08f0003     	add	r0, pc, r3
    306c: ebfffc01     	bl	0x2078 <.plt+0x14>      @ imm = #-0xffc
    3070: e59f20fc     	ldr	r2, [pc, #0xfc]         @ 0x3174 <stereo_in_tilde_setup+0x1ec>
    3074: e3a03006     	mov	r3, #6
    3078: e7961002     	ldr	r1, [r6, r2]
    307c: e58d5000     	str	r5, [sp]
    3080: e1a02000     	mov	r2, r0
    3084: e1a00007     	mov	r0, r7
    3088: ebfffc6c     	bl	0x2240 <.plt+0x1dc>     @ imm = #-0xe50
    308c: e59f00e4     	ldr	r0, [pc, #0xe4]         @ 0x3178 <stereo_in_tilde_setup+0x1f0>
    3090: e5947000     	ldr	r7, [r4]
    3094: e08f0000     	add	r0, pc, r0
    3098: ebfffbf6     	bl	0x2078 <.plt+0x14>      @ imm = #-0x1028
    309c: e59f10d8     	ldr	r1, [pc, #0xd8]         @ 0x317c <stereo_in_tilde_setup+0x1f4>
    30a0: e3a03006     	mov	r3, #6
    30a4: e7961001     	ldr	r1, [r6, r1]
    30a8: e58d5000     	str	r5, [sp]
    30ac: e1a02000     	mov	r2, r0
    30b0: e1a00007     	mov	r0, r7
    30b4: ebfffc61     	bl	0x2240 <.plt+0x1dc>     @ imm = #-0xe7c
    30b8: e59fc0c0     	ldr	r12, [pc, #0xc0]        @ 0x3180 <stereo_in_tilde_setup+0x1f8>
    30bc: e5947000     	ldr	r7, [r4]
    30c0: e08f000c     	add	r0, pc, r12
    30c4: ebfffbeb     	bl	0x2078 <.plt+0x14>      @ imm = #-0x1054
    30c8: e59f20b4     	ldr	r2, [pc, #0xb4]         @ 0x3184 <stereo_in_tilde_setup+0x1fc>
    30cc: e3a03006     	mov	r3, #6
    30d0: e7961002     	ldr	r1, [r6, r2]
    30d4: e58d5000     	str	r5, [sp]
    30d8: e1a02000     	mov	r2, r0
    30dc: e1a00007     	mov	r0, r7
    30e0: ebfffc56     	bl	0x2240 <.plt+0x1dc>     @ imm = #-0xea8
    30e4: e59f309c     	ldr	r3, [pc, #0x9c]         @ 0x3188 <stereo_in_tilde_setup+0x200>
    30e8: e5947000     	ldr	r7, [r4]
    30ec: e08f0003     	add	r0, pc, r3
    30f0: ebfffbe0     	bl	0x2078 <.plt+0x14>      @ imm = #-0x1080
    30f4: e59f1090     	ldr	r1, [pc, #0x90]         @ 0x318c <stereo_in_tilde_setup+0x204>
    30f8: e3a03006     	mov	r3, #6
    30fc: e7961001     	ldr	r1, [r6, r1]
    3100: e59f6088     	ldr	r6, [pc, #0x88]         @ 0x3190 <stereo_in_tilde_setup+0x208>
    3104: e58d5000     	str	r5, [sp]
    3108: e1a02000     	mov	r2, r0
    310c: e1a00007     	mov	r0, r7
    3110: ebfffc4a     	bl	0x2240 <.plt+0x1dc>     @ imm = #-0xed8
    3114: e08f0006     	add	r0, pc, r6
    3118: e5944000     	ldr	r4, [r4]
    311c: ebfffbd5     	bl	0x2078 <.plt+0x14>      @ imm = #-0x10ac
    3120: e58d5000     	str	r5, [sp]
    3124: e3a0300b     	mov	r3, #11
    3128: e59f5064     	ldr	r5, [pc, #0x64]         @ 0x3194 <stereo_in_tilde_setup+0x20c>
    312c: e08f1005     	add	r1, pc, r5
    3130: e1a02000     	mov	r2, r0
    3134: e1a00004     	mov	r0, r4
    3138: ebfffc40     	bl	0x2240 <.plt+0x1dc>     @ imm = #-0xf00
    313c: e28dd00c     	add	sp, sp, #12
    3140: e8bd80f0     	pop	{r4, r5, r6, r7, pc}
    3144: 44 46 00 00  	.word	0x00004644
    3148: 80 f4 ff ff  	.word	0xfffff480
    314c: 98 f4 ff ff  	.word	0xfffff498
    3150: 58 51 01 00  	.word	0x00015158
    3154: 28 50 01 00  	.word	0x00015028
    3158: fc 45 00 00  	.word	0x000045fc
    315c: e4 00 00 00  	.word	0x000000e4
    3160: dc 45 00 00  	.word	0x000045dc
    3164: b4 00 00 00  	.word	0x000000b4
    3168: bc 45 00 00  	.word	0x000045bc
    316c: dc 00 00 00  	.word	0x000000dc
    3170: 9c 45 00 00  	.word	0x0000459c
    3174: c4 00 00 00  	.word	0x000000c4
    3178: 7c 45 00 00  	.word	0x0000457c
    317c: c8 00 00 00  	.word	0x000000c8
    3180: 58 45 00 00  	.word	0x00004558
    3184: d0 00 00 00  	.word	0x000000d0
    3188: 38 45 00 00  	.word	0x00004538
    318c: c0 00 00 00  	.word	0x000000c0
    3190: 20 45 00 00  	.word	0x00004520
    3194: a0 f2 ff ff  	.word	0xfffff2a0

