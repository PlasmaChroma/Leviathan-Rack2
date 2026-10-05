; lubadh::Channel::FilePreview::init()
; VA 0x3b024 size 764

   3b024: e92d4070     	push	{r4, r5, r6, lr}
   3b028: e3021728     	movw	r1, #0x2728
   3b02c: e3401007     	movt	r1, #0x7
   3b030: e24dd098     	sub	sp, sp, #152
   3b034: e1a04000     	mov	r4, r0
   3b038: e28d0080     	add	r0, sp, #128
   3b03c: ebffef81     	bl	0x36e48
   3b040: e3090fec     	movw	r0, #0x9fec
   3b044: e3400009     	movt	r0, #0x9
   3b048: e28d1080     	add	r1, sp, #128
   3b04c: e3a02000     	mov	r2, #0
   3b050: eb00d3b2     	bl	0x6ff20
   3b054: e59d0080     	ldr	r0, [sp, #0x80]
   3b058: e28d5088     	add	r5, sp, #136
   3b05c: e1500005     	cmp	r0, r5
   3b060: 0a000000     	beq	0x3b068
   3b064: ebff6b75     	bl	0x15e40    @ imm = #-0x2522c ; _ZdlPv
   3b068: e5941000     	ldr	r1, [r4]
   3b06c: e28d0008     	add	r0, sp, #8
   3b070: e3022744     	movw	r2, #0x2744
   3b074: e3402007     	movt	r2, #0x7
   3b078: e2811004     	add	r1, r1, #4
   3b07c: ebffced1     	bl	0x2ebc8
   3b080: e594c000     	ldr	r12, [r4]
   3b084: e3003d28     	movw	r3, #0xd28
   3b088: e3403007     	movt	r3, #0x7
   3b08c: e28cca2a     	add	r12, r12, #172032
   3b090: e3061218     	movw	r1, #0x6218
   3b094: e3401001     	movt	r1, #0x1
   3b098: e28d0020     	add	r0, sp, #32
   3b09c: e3a02010     	mov	r2, #16
   3b0a0: e59cc260     	ldr	r12, [r12, #0x260]
   3b0a4: e58dc000     	str	r12, [sp]
   3b0a8: ebffefbe     	bl	0x36fa8
   3b0ac: e28d2020     	add	r2, sp, #32
   3b0b0: e28d1008     	add	r1, sp, #8
   3b0b4: e28d0038     	add	r0, sp, #56
   3b0b8: ebffce0b     	bl	0x2e8ec
   3b0bc: e3021718     	movw	r1, #0x2718
   3b0c0: e3401007     	movt	r1, #0x7
   3b0c4: e28d0038     	add	r0, sp, #56
   3b0c8: ebff6cdf     	bl	0x1644c    @ imm = #-0x24c84 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6appendEPKc
   3b0cc: e1a01000     	mov	r1, r0
   3b0d0: e28d0050     	add	r0, sp, #80
   3b0d4: ebff6ac9     	bl	0x15c00    @ imm = #-0x254dc ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EOS4_
   3b0d8: e594c000     	ldr	r12, [r4]
   3b0dc: e3003d28     	movw	r3, #0xd28
   3b0e0: e3403007     	movt	r3, #0x7
   3b0e4: e28cca2a     	add	r12, r12, #172032
   3b0e8: e3061218     	movw	r1, #0x6218
   3b0ec: e3401001     	movt	r1, #0x1
   3b0f0: e28d0068     	add	r0, sp, #104
   3b0f4: e3a02010     	mov	r2, #16
   3b0f8: e59cc25c     	ldr	r12, [r12, #0x25c]
   3b0fc: e58dc000     	str	r12, [sp]
   3b100: ebffefa8     	bl	0x36fa8
   3b104: e28d2068     	add	r2, sp, #104
   3b108: e28d1050     	add	r1, sp, #80
   3b10c: e28d0080     	add	r0, sp, #128
   3b110: ebffcdf5     	bl	0x2e8ec
   3b114: e3090fec     	movw	r0, #0x9fec
   3b118: e3400009     	movt	r0, #0x9
   3b11c: e28d1080     	add	r1, sp, #128
   3b120: e3a02000     	mov	r2, #0
   3b124: eb00d37d     	bl	0x6ff20
   3b128: e59d0080     	ldr	r0, [sp, #0x80]
   3b12c: e1500005     	cmp	r0, r5
   3b130: 0a000000     	beq	0x3b138
   3b134: ebff6b41     	bl	0x15e40    @ imm = #-0x252fc ; _ZdlPv
   3b138: e59d0068     	ldr	r0, [sp, #0x68]
   3b13c: e28d3070     	add	r3, sp, #112
   3b140: e1500003     	cmp	r0, r3
   3b144: 0a000000     	beq	0x3b14c
   3b148: ebff6b3c     	bl	0x15e40    @ imm = #-0x25310 ; _ZdlPv
   3b14c: e59d0050     	ldr	r0, [sp, #0x50]
   3b150: e28d3058     	add	r3, sp, #88
   3b154: e1500003     	cmp	r0, r3
   3b158: 0a000000     	beq	0x3b160
   3b15c: ebff6b37     	bl	0x15e40    @ imm = #-0x25324 ; _ZdlPv
   3b160: e59d0038     	ldr	r0, [sp, #0x38]
   3b164: e28d3040     	add	r3, sp, #64
   3b168: e1500003     	cmp	r0, r3
   3b16c: 0a000000     	beq	0x3b174
   3b170: ebff6b32     	bl	0x15e40    @ imm = #-0x25338 ; _ZdlPv
   3b174: e59d0020     	ldr	r0, [sp, #0x20]
   3b178: e28d3028     	add	r3, sp, #40
   3b17c: e1500003     	cmp	r0, r3
   3b180: 0a000000     	beq	0x3b188
   3b184: ebff6b2d     	bl	0x15e40    @ imm = #-0x2534c ; _ZdlPv
   3b188: e59d0008     	ldr	r0, [sp, #0x8]
   3b18c: e28d3010     	add	r3, sp, #16
   3b190: e1500003     	cmp	r0, r3
   3b194: 0a000000     	beq	0x3b19c
   3b198: ebff6b28     	bl	0x15e40    @ imm = #-0x25360 ; _ZdlPv
   3b19c: e5943000     	ldr	r3, [r4]
   3b1a0: e3a02000     	mov	r2, #0
   3b1a4: e5842028     	str	r2, [r4, #0x28]
   3b1a8: e3a05001     	mov	r5, #1
   3b1ac: e2831a2a     	add	r1, r3, #172032
   3b1b0: e5940078     	ldr	r0, [r4, #0x78]
   3b1b4: e2812f97     	add	r2, r1, #604
   3b1b8: e594e054     	ldr	lr, [r4, #0x54]
   3b1bc: e5936000     	ldr	r6, [r3]
   3b1c0: e3a0c000     	mov	r12, #0
   3b1c4: e1c453b0     	strh	r5, [r4, #48]
   3b1c8: f462078f     	vld1.32	{d16}, [r2]
   3b1cc: e1a02005     	mov	r2, r5
   3b1d0: e584c02c     	str	r12, [r4, #0x2c]
   3b1d4: e594c09c     	ldr	r12, [r4, #0x9c]
   3b1d8: e5805000     	str	r5, [r0]
   3b1dc: e5930020     	ldr	r0, [r3, #0x20]
   3b1e0: e593301c     	ldr	r3, [r3, #0x1c]
   3b1e4: f44e078f     	vst1.32	{d16}, [lr]
   3b1e8: e280e915     	add	lr, r0, #344064
   3b1ec: e7cc5006     	strb	r5, [r12, r6]
   3b1f0: e5933000     	ldr	r3, [r3]
   3b1f4: e5deeabc     	ldrb	lr, [lr, #0xabc]
   3b1f8: e7cce003     	strb	lr, [r12, r3]
   3b1fc: e5d11496     	ldrb	r1, [r1, #0x496]
   3b200: eb00c5e3     	bl	0x6c994
   3b204: e5940000     	ldr	r0, [r4]
   3b208: e5903020     	ldr	r3, [r0, #0x20]
   3b20c: e2833915     	add	r3, r3, #344064
   3b210: e5d33abc     	ldrb	r3, [r3, #0xabc]
   3b214: e3530000     	cmp	r3, #0
   3b218: 1a00000a     	bne	0x3b248
   3b21c: ebfff4bd     	bl	0x38518
   3b220: e5942000     	ldr	r2, [r4]
   3b224: e5923020     	ldr	r3, [r2, #0x20]
   3b228: e2833915     	add	r3, r3, #344064
   3b22c: e5d33abc     	ldrb	r3, [r3, #0xabc]
   3b230: e3530000     	cmp	r3, #0
   3b234: 1a00000b     	bne	0x3b268
   3b238: e2840034     	add	r0, r4, #52
   3b23c: eb00d577     	bl	0x70820
   3b240: e28dd098     	add	sp, sp, #152
   3b244: e8bd8070     	pop	{r4, r5, r6, pc}
   3b248: e590301c     	ldr	r3, [r0, #0x1c]
   3b24c: e1a02005     	mov	r2, r5
   3b250: e2831a2a     	add	r1, r3, #172032
   3b254: e5930020     	ldr	r0, [r3, #0x20]
   3b258: e5d11496     	ldrb	r1, [r1, #0x496]
   3b25c: eb00c5cc     	bl	0x6c994
   3b260: e5940000     	ldr	r0, [r4]
   3b264: eaffffec     	b	0x3b21c
   3b268: e592001c     	ldr	r0, [r2, #0x1c]
   3b26c: ebfff4a9     	bl	0x38518
   3b270: e2840034     	add	r0, r4, #52
   3b274: eb00d569     	bl	0x70820
   3b278: e28dd098     	add	sp, sp, #152
   3b27c: e8bd8070     	pop	{r4, r5, r6, pc}
   3b280: e59d0080     	ldr	r0, [sp, #0x80]
   3b284: e28d3088     	add	r3, sp, #136
   3b288: e1500003     	cmp	r0, r3
   3b28c: 0a000000     	beq	0x3b294
   3b290: ebff6aea     	bl	0x15e40    @ imm = #-0x25458 ; _ZdlPv
   3b294: ebff6b31     	bl	0x15f60    @ imm = #-0x2533c ; __cxa_end_cleanup
   3b298: e59d0020     	ldr	r0, [sp, #0x20]
   3b29c: e28d3028     	add	r3, sp, #40
   3b2a0: e1500003     	cmp	r0, r3
   3b2a4: 0a000000     	beq	0x3b2ac
   3b2a8: ebff6ae4     	bl	0x15e40    @ imm = #-0x25470 ; _ZdlPv
   3b2ac: e59d0008     	ldr	r0, [sp, #0x8]
   3b2b0: e28d3010     	add	r3, sp, #16
   3b2b4: e1500003     	cmp	r0, r3
   3b2b8: 1afffff4     	bne	0x3b290
   3b2bc: eafffff4     	b	0x3b294
   3b2c0: eafffff9     	b	0x3b2ac
   3b2c4: e59d0080     	ldr	r0, [sp, #0x80]
   3b2c8: e1500005     	cmp	r0, r5
   3b2cc: 0a000000     	beq	0x3b2d4
   3b2d0: ebff6ada     	bl	0x15e40    @ imm = #-0x25498 ; _ZdlPv
   3b2d4: e59d0068     	ldr	r0, [sp, #0x68]
   3b2d8: e28d3070     	add	r3, sp, #112
   3b2dc: e1500003     	cmp	r0, r3
   3b2e0: 0a000000     	beq	0x3b2e8
   3b2e4: ebff6ad5     	bl	0x15e40    @ imm = #-0x254ac ; _ZdlPv
   3b2e8: e59d0050     	ldr	r0, [sp, #0x50]
   3b2ec: e28d3058     	add	r3, sp, #88
   3b2f0: e1500003     	cmp	r0, r3
   3b2f4: 0a000000     	beq	0x3b2fc
   3b2f8: ebff6ad0     	bl	0x15e40    @ imm = #-0x254c0 ; _ZdlPv
   3b2fc: e59d0038     	ldr	r0, [sp, #0x38]
   3b300: e28d3040     	add	r3, sp, #64
   3b304: e1500003     	cmp	r0, r3
   3b308: 0affffe2     	beq	0x3b298
   3b30c: ebff6acb     	bl	0x15e40    @ imm = #-0x254d4 ; _ZdlPv
   3b310: eaffffe0     	b	0x3b298
   3b314: eaffffee     	b	0x3b2d4
   3b318: eafffff2     	b	0x3b2e8
   3b31c: eafffff6     	b	0x3b2fc
