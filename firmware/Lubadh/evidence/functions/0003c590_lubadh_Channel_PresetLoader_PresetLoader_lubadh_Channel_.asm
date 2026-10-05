; lubadh::Channel::PresetLoader::PresetLoader(lubadh::Channel&)
; VA 0x3c590 size 1456

   3c590: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   3c594: e1a0b000     	mov	r11, r0
   3c598: e1a03001     	mov	r3, r1
   3c59c: e24dd034     	sub	sp, sp, #52
   3c5a0: e2816004     	add	r6, r1, #4
   3c5a4: e48b3004     	str	r3, [r11], #4
   3c5a8: e1a04000     	mov	r4, r0
   3c5ac: e59f153c     	ldr	r1, [pc, #0x53c]        @ 0x3caf0
   3c5b0: e28d0018     	add	r0, sp, #24
   3c5b4: e1a02006     	mov	r2, r6
   3c5b8: ebffc96f     	bl	0x2eb7c
   3c5bc: e28d1018     	add	r1, sp, #24
   3c5c0: e1a0000b     	mov	r0, r11
   3c5c4: eb001610     	bl	0x41e0c
   3c5c8: e59d0018     	ldr	r0, [sp, #0x18]
   3c5cc: e28d5020     	add	r5, sp, #32
   3c5d0: e59f351c     	ldr	r3, [pc, #0x51c]        @ 0x3caf4
   3c5d4: e1500005     	cmp	r0, r5
   3c5d8: e5843004     	str	r3, [r4, #0x4]
   3c5dc: 0a000000     	beq	0x3c5e4
   3c5e0: ebff6616     	bl	0x15e40    @ imm = #-0x267a8 ; _ZdlPv
   3c5e4: e59f150c     	ldr	r1, [pc, #0x50c]        @ 0x3caf8
   3c5e8: e1a02006     	mov	r2, r6
   3c5ec: e28d0018     	add	r0, sp, #24
   3c5f0: ebffc961     	bl	0x2eb7c
   3c5f4: e59d1018     	ldr	r1, [sp, #0x18]
   3c5f8: e3a03000     	mov	r3, #0
   3c5fc: e59d201c     	ldr	r2, [sp, #0x1c]
   3c600: e2848038     	add	r8, r4, #56
   3c604: e59f04f0     	ldr	r0, [pc, #0x4f0]        @ 0x3cafc
   3c608: e2847028     	add	r7, r4, #40
   3c60c: e5840028     	str	r0, [r4, #0x28]
   3c610: e0812002     	add	r2, r1, r2
   3c614: e2840030     	add	r0, r4, #48
   3c618: e584302c     	str	r3, [r4, #0x2c]
   3c61c: e5848030     	str	r8, [r4, #0x30]
   3c620: ebffea34     	bl	0x36ef8
   3c624: e3e03000     	mvn	r3, #0
   3c628: e1a00007     	mov	r0, r7
   3c62c: e5843048     	str	r3, [r4, #0x48]
   3c630: eb001b6d     	bl	0x433ec
   3c634: e1a00007     	mov	r0, r7
   3c638: eb001cb3     	bl	0x4390c
   3c63c: e1a00007     	mov	r0, r7
   3c640: eb001d84     	bl	0x43c58
   3c644: e59d0018     	ldr	r0, [sp, #0x18]
   3c648: e59f34b0     	ldr	r3, [pc, #0x4b0]        @ 0x3cb00
   3c64c: e1500005     	cmp	r0, r5
   3c650: e5843028     	str	r3, [r4, #0x28]
   3c654: 0a000000     	beq	0x3c65c
   3c658: ebff65f8     	bl	0x15e40    @ imm = #-0x26820 ; _ZdlPv
   3c65c: e59f14a0     	ldr	r1, [pc, #0x4a0]        @ 0x3cb04
   3c660: e1a02006     	mov	r2, r6
   3c664: e28d0018     	add	r0, sp, #24
   3c668: ebffc943     	bl	0x2eb7c
   3c66c: e284304c     	add	r3, r4, #76
   3c670: e28d1018     	add	r1, sp, #24
   3c674: e1a00003     	mov	r0, r3
   3c678: e58d300c     	str	r3, [sp, #0xc]
   3c67c: eb0015e2     	bl	0x41e0c
   3c680: e59d0018     	ldr	r0, [sp, #0x18]
   3c684: e59f347c     	ldr	r3, [pc, #0x47c]        @ 0x3cb08
   3c688: e1500005     	cmp	r0, r5
   3c68c: e584304c     	str	r3, [r4, #0x4c]
   3c690: 0a000000     	beq	0x3c698
   3c694: ebff65e9     	bl	0x15e40    @ imm = #-0x2685c ; _ZdlPv
   3c698: e59f146c     	ldr	r1, [pc, #0x46c]        @ 0x3cb0c
   3c69c: e1a02006     	mov	r2, r6
   3c6a0: e28d0018     	add	r0, sp, #24
   3c6a4: ebffc934     	bl	0x2eb7c
   3c6a8: e59d1018     	ldr	r1, [sp, #0x18]
   3c6ac: e3a03000     	mov	r3, #0
   3c6b0: e59d201c     	ldr	r2, [sp, #0x1c]
   3c6b4: e2849080     	add	r9, r4, #128
   3c6b8: e59f0450     	ldr	r0, [pc, #0x450]        @ 0x3cb10
   3c6bc: e2848070     	add	r8, r4, #112
   3c6c0: e5840070     	str	r0, [r4, #0x70]
   3c6c4: e0812002     	add	r2, r1, r2
   3c6c8: e2840078     	add	r0, r4, #120
   3c6cc: e5843074     	str	r3, [r4, #0x74]
   3c6d0: e5849078     	str	r9, [r4, #0x78]
   3c6d4: ebffea07     	bl	0x36ef8
   3c6d8: e3e03000     	mvn	r3, #0
   3c6dc: e1a00008     	mov	r0, r8
   3c6e0: e5843090     	str	r3, [r4, #0x90]
   3c6e4: eb001deb     	bl	0x43e98
   3c6e8: e1a00008     	mov	r0, r8
   3c6ec: eb001f31     	bl	0x443b8
   3c6f0: e1a00008     	mov	r0, r8
   3c6f4: eb002002     	bl	0x44704
   3c6f8: e59d0018     	ldr	r0, [sp, #0x18]
   3c6fc: e59f3410     	ldr	r3, [pc, #0x410]        @ 0x3cb14
   3c700: e1500005     	cmp	r0, r5
   3c704: e5843070     	str	r3, [r4, #0x70]
   3c708: 0a000000     	beq	0x3c710
   3c70c: ebff65cb     	bl	0x15e40    @ imm = #-0x268d4 ; _ZdlPv
   3c710: e3043dc0     	movw	r3, #0x4dc0
   3c714: e3403009     	movt	r3, #0x9
   3c718: e59f23f8     	ldr	r2, [pc, #0x3f8]        @ 0x3cb18
   3c71c: e3a0c000     	mov	r12, #0
   3c720: e5842094     	str	r2, [r4, #0x94]
   3c724: e284a0a4     	add	r10, r4, #164
   3c728: e5931210     	ldr	r1, [r3, #0x210]
   3c72c: e284009c     	add	r0, r4, #156
   3c730: e5932214     	ldr	r2, [r3, #0x214]
   3c734: e2849094     	add	r9, r4, #148
   3c738: e584c098     	str	r12, [r4, #0x98]
   3c73c: e0812002     	add	r2, r1, r2
   3c740: e584a09c     	str	r10, [r4, #0x9c]
   3c744: ebffe9eb     	bl	0x36ef8
   3c748: e3e03000     	mvn	r3, #0
   3c74c: e1a00009     	mov	r0, r9
   3c750: e58430b4     	str	r3, [r4, #0xb4]
   3c754: eb00207a     	bl	0x44944
   3c758: e1a00009     	mov	r0, r9
   3c75c: eb0021c0     	bl	0x44e64
   3c760: e1a00009     	mov	r0, r9
   3c764: eb002291     	bl	0x451b0
   3c768: e59f33ac     	ldr	r3, [pc, #0x3ac]        @ 0x3cb1c
   3c76c: e1a02006     	mov	r2, r6
   3c770: e59f13a8     	ldr	r1, [pc, #0x3a8]        @ 0x3cb20
   3c774: e28d0018     	add	r0, sp, #24
   3c778: e5843094     	str	r3, [r4, #0x94]
   3c77c: ebffc8fe     	bl	0x2eb7c
   3c780: e28430b8     	add	r3, r4, #184
   3c784: e28d1018     	add	r1, sp, #24
   3c788: e1a00003     	mov	r0, r3
   3c78c: e58d3010     	str	r3, [sp, #0x10]
   3c790: eb00159d     	bl	0x41e0c
   3c794: e59d0018     	ldr	r0, [sp, #0x18]
   3c798: e59f3368     	ldr	r3, [pc, #0x368]        @ 0x3cb08
   3c79c: e1500005     	cmp	r0, r5
   3c7a0: e58430b8     	str	r3, [r4, #0xb8]
   3c7a4: 0a000000     	beq	0x3c7ac
   3c7a8: ebff65a4     	bl	0x15e40    @ imm = #-0x26970 ; _ZdlPv
   3c7ac: e28430e0     	add	r3, r4, #224
   3c7b0: e59f136c     	ldr	r1, [pc, #0x36c]        @ 0x3cb24
   3c7b4: e1a02006     	mov	r2, r6
   3c7b8: e28d0018     	add	r0, sp, #24
   3c7bc: e1a0a003     	mov	r10, r3
   3c7c0: e58d3004     	str	r3, [sp, #0x4]
   3c7c4: ebffc8ec     	bl	0x2eb7c
   3c7c8: e3a02000     	mov	r2, #0
   3c7cc: e28d1018     	add	r1, sp, #24
   3c7d0: e1a0000a     	mov	r0, r10
   3c7d4: eb00ce03     	bl	0x6ffe8
   3c7d8: e59d0018     	ldr	r0, [sp, #0x18]
   3c7dc: e1500005     	cmp	r0, r5
   3c7e0: 0a000000     	beq	0x3c7e8
   3c7e4: ebff6595     	bl	0x15e40    @ imm = #-0x269ac ; _ZdlPv
   3c7e8: e28430fc     	add	r3, r4, #252
   3c7ec: e59f1334     	ldr	r1, [pc, #0x334]        @ 0x3cb28
   3c7f0: e1a02006     	mov	r2, r6
   3c7f4: e28d0018     	add	r0, sp, #24
   3c7f8: e1a0a003     	mov	r10, r3
   3c7fc: e58d3008     	str	r3, [sp, #0x8]
   3c800: ebffc8dd     	bl	0x2eb7c
   3c804: e3a02000     	mov	r2, #0
   3c808: e28d1018     	add	r1, sp, #24
   3c80c: e1a0000a     	mov	r0, r10
   3c810: eb00cdf4     	bl	0x6ffe8
   3c814: e59d0018     	ldr	r0, [sp, #0x18]
   3c818: e1500005     	cmp	r0, r5
   3c81c: 0a000000     	beq	0x3c824
   3c820: ebff6586     	bl	0x15e40    @ imm = #-0x269e8 ; _ZdlPv
   3c824: e59f1300     	ldr	r1, [pc, #0x300]        @ 0x3cb2c
   3c828: e1a02006     	mov	r2, r6
   3c82c: e28d0018     	add	r0, sp, #24
   3c830: ebffc8d1     	bl	0x2eb7c
   3c834: e2840f4a     	add	r0, r4, #296
   3c838: e59d1018     	ldr	r1, [sp, #0x18]
   3c83c: e59d201c     	ldr	r2, [sp, #0x1c]
   3c840: e1a0c000     	mov	r12, r0
   3c844: e58d0014     	str	r0, [sp, #0x14]
   3c848: e3a03000     	mov	r3, #0
   3c84c: e59f02dc     	ldr	r0, [pc, #0x2dc]        @ 0x3cb30
   3c850: e0812002     	add	r2, r1, r2
   3c854: e5840118     	str	r0, [r4, #0x118]
   3c858: e2840e12     	add	r0, r4, #288
   3c85c: e584311c     	str	r3, [r4, #0x11c]
   3c860: e284af46     	add	r10, r4, #280
   3c864: e584c120     	str	r12, [r4, #0x120]
   3c868: ebffe9a2     	bl	0x36ef8
   3c86c: e3e03000     	mvn	r3, #0
   3c870: e1a0000a     	mov	r0, r10
   3c874: e5843138     	str	r3, [r4, #0x138]
   3c878: eb00157e     	bl	0x41e78
   3c87c: e1a0000a     	mov	r0, r10
   3c880: eb0016c4     	bl	0x42398
   3c884: e1a0000a     	mov	r0, r10
   3c888: eb001795     	bl	0x426e4
   3c88c: e59d0018     	ldr	r0, [sp, #0x18]
   3c890: e59f329c     	ldr	r3, [pc, #0x29c]        @ 0x3cb34
   3c894: e1500005     	cmp	r0, r5
   3c898: e5843118     	str	r3, [r4, #0x118]
   3c89c: 0a000000     	beq	0x3c8a4
   3c8a0: ebff6566     	bl	0x15e40    @ imm = #-0x26a68 ; _ZdlPv
   3c8a4: e3a03000     	mov	r3, #0
   3c8a8: e59f1288     	ldr	r1, [pc, #0x288]        @ 0x3cb38
   3c8ac: e1a02006     	mov	r2, r6
   3c8b0: e28d0018     	add	r0, sp, #24
   3c8b4: e5c4313c     	strb	r3, [r4, #0x13c]
   3c8b8: ebffc8af     	bl	0x2eb7c
   3c8bc: e28d1018     	add	r1, sp, #24
   3c8c0: e2840d05     	add	r0, r4, #320
   3c8c4: eb001550     	bl	0x41e0c
   3c8c8: e59d0018     	ldr	r0, [sp, #0x18]
   3c8cc: e59f3268     	ldr	r3, [pc, #0x268]        @ 0x3cb3c
   3c8d0: e1500005     	cmp	r0, r5
   3c8d4: e5843140     	str	r3, [r4, #0x140]
   3c8d8: 0a000000     	beq	0x3c8e0
   3c8dc: ebff6557     	bl	0x15e40    @ imm = #-0x26aa4 ; _ZdlPv
   3c8e0: f2c00010     	vmov.i32	d16, #0x0
   3c8e4: e2843f5a     	add	r3, r4, #360
   3c8e8: e594602c     	ldr	r6, [r4, #0x2c]
   3c8ec: e3a05000     	mov	r5, #0
   3c8f0: e304229c     	movw	r2, #0x429c
   3c8f4: e30512a8     	movw	r1, #0x52a8
   3c8f8: e3401009     	movt	r1, #0x9
   3c8fc: e1a00006     	mov	r0, r6
   3c900: f443078f     	vst1.32	{d16}, [r3]
   3c904: e5c45170     	strb	r5, [r4, #0x170]
   3c908: ebff65cd     	bl	0x16044    @ imm = #-0x268cc ; memcpy
   3c90c: e2860c42     	add	r0, r6, #16896
   3c910: e304229c     	movw	r2, #0x429c
   3c914: e280009c     	add	r0, r0, #156
   3c918: e30512a8     	movw	r1, #0x52a8
   3c91c: e3401009     	movt	r1, #0x9
   3c920: ebff65c7     	bl	0x16044    @ imm = #-0x268e4 ; memcpy
   3c924: e5943098     	ldr	r3, [r4, #0x98]
   3c928: e1a00004     	mov	r0, r4
   3c92c: e5c35000     	strb	r5, [r3]
   3c930: e5c35001     	strb	r5, [r3, #0x1]
   3c934: e28dd034     	add	sp, sp, #52
   3c938: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
   3c93c: e59d0018     	ldr	r0, [sp, #0x18]
   3c940: e28d3020     	add	r3, sp, #32
   3c944: e1500003     	cmp	r0, r3
   3c948: 0a000000     	beq	0x3c950
   3c94c: ebff653b     	bl	0x15e40    @ imm = #-0x26b14 ; _ZdlPv
   3c950: ebff6582     	bl	0x15f60    @ imm = #-0x269f8 ; __cxa_end_cleanup
   3c954: e59d0018     	ldr	r0, [sp, #0x18]
   3c958: e1500005     	cmp	r0, r5
   3c95c: 1a000020     	bne	0x3c9e4
   3c960: e59f31cc     	ldr	r3, [pc, #0x1cc]        @ 0x3cb34
   3c964: e1a0000a     	mov	r0, r10
   3c968: e5843118     	str	r3, [r4, #0x118]
   3c96c: ebffdea1     	bl	0x343f8
   3c970: e59d0008     	ldr	r0, [sp, #0x8]
   3c974: eb00cec5     	bl	0x70490
   3c978: e59d0004     	ldr	r0, [sp, #0x4]
   3c97c: eb00cec3     	bl	0x70490
   3c980: e59f3180     	ldr	r3, [pc, #0x180]        @ 0x3cb08
   3c984: e59d0010     	ldr	r0, [sp, #0x10]
   3c988: e58430b8     	str	r3, [r4, #0xb8]
   3c98c: ebffca0a     	bl	0x2f1bc
   3c990: e59f3184     	ldr	r3, [pc, #0x184]        @ 0x3cb1c
   3c994: e1a00009     	mov	r0, r9
   3c998: e5843094     	str	r3, [r4, #0x94]
   3c99c: ebffdefe     	bl	0x3459c
   3c9a0: e59f316c     	ldr	r3, [pc, #0x16c]        @ 0x3cb14
   3c9a4: e1a00008     	mov	r0, r8
   3c9a8: e5843070     	str	r3, [r4, #0x70]
   3c9ac: ebffddbf     	bl	0x340b0
   3c9b0: e59f3150     	ldr	r3, [pc, #0x150]        @ 0x3cb08
   3c9b4: e59d000c     	ldr	r0, [sp, #0xc]
   3c9b8: e584304c     	str	r3, [r4, #0x4c]
   3c9bc: ebffc9fe     	bl	0x2f1bc
   3c9c0: e59f3138     	ldr	r3, [pc, #0x138]        @ 0x3cb00
   3c9c4: e1a00007     	mov	r0, r7
   3c9c8: e5843028     	str	r3, [r4, #0x28]
   3c9cc: ebffde20     	bl	0x34254
   3c9d0: e59f311c     	ldr	r3, [pc, #0x11c]        @ 0x3caf4
   3c9d4: e1a0000b     	mov	r0, r11
   3c9d8: e5843004     	str	r3, [r4, #0x4]
   3c9dc: ebffc9f6     	bl	0x2f1bc
   3c9e0: eaffffda     	b	0x3c950
   3c9e4: ebff6515     	bl	0x15e40    @ imm = #-0x26bac ; _ZdlPv
   3c9e8: eaffffdc     	b	0x3c960
   3c9ec: eaffffdb     	b	0x3c960
   3c9f0: e5940120     	ldr	r0, [r4, #0x120]
   3c9f4: e59d3014     	ldr	r3, [sp, #0x14]
   3c9f8: e1530000     	cmp	r3, r0
   3c9fc: 0a000000     	beq	0x3ca04
   3ca00: ebff650e     	bl	0x15e40    @ imm = #-0x26bc8 ; _ZdlPv
   3ca04: e59d0018     	ldr	r0, [sp, #0x18]
   3ca08: e1500005     	cmp	r0, r5
   3ca0c: 0affffd7     	beq	0x3c970
   3ca10: ebff650a     	bl	0x15e40    @ imm = #-0x26bd8 ; _ZdlPv
   3ca14: eaffffd5     	b	0x3c970
   3ca18: eafffff9     	b	0x3ca04
   3ca1c: eaffffd3     	b	0x3c970
   3ca20: e59d0018     	ldr	r0, [sp, #0x18]
   3ca24: e1500005     	cmp	r0, r5
   3ca28: 0affffd2     	beq	0x3c978
   3ca2c: ebff6503     	bl	0x15e40    @ imm = #-0x26bf4 ; _ZdlPv
   3ca30: eaffffd0     	b	0x3c978
   3ca34: eaffffcf     	b	0x3c978
   3ca38: e59d0018     	ldr	r0, [sp, #0x18]
   3ca3c: e1500005     	cmp	r0, r5
   3ca40: 0affffce     	beq	0x3c980
   3ca44: ebff64fd     	bl	0x15e40    @ imm = #-0x26c0c ; _ZdlPv
   3ca48: eaffffcc     	b	0x3c980
   3ca4c: eaffffcb     	b	0x3c980
   3ca50: e59d0018     	ldr	r0, [sp, #0x18]
   3ca54: e1500005     	cmp	r0, r5
   3ca58: 0affffcc     	beq	0x3c990
   3ca5c: ebff64f7     	bl	0x15e40    @ imm = #-0x26c24 ; _ZdlPv
   3ca60: eaffffca     	b	0x3c990
   3ca64: eaffffc9     	b	0x3c990
   3ca68: e594009c     	ldr	r0, [r4, #0x9c]
   3ca6c: e15a0000     	cmp	r10, r0
   3ca70: 0affffca     	beq	0x3c9a0
   3ca74: ebff64f1     	bl	0x15e40    @ imm = #-0x26c3c ; _ZdlPv
   3ca78: eaffffc8     	b	0x3c9a0
   3ca7c: eaffffc7     	b	0x3c9a0
   3ca80: e5940078     	ldr	r0, [r4, #0x78]
   3ca84: e1590000     	cmp	r9, r0
   3ca88: 0a000000     	beq	0x3ca90
   3ca8c: ebff64eb     	bl	0x15e40    @ imm = #-0x26c54 ; _ZdlPv
   3ca90: e59d0018     	ldr	r0, [sp, #0x18]
   3ca94: e1500005     	cmp	r0, r5
   3ca98: 0affffc4     	beq	0x3c9b0
   3ca9c: ebff64e7     	bl	0x15e40    @ imm = #-0x26c64 ; _ZdlPv
   3caa0: eaffffc2     	b	0x3c9b0
   3caa4: eafffff9     	b	0x3ca90
   3caa8: eaffffc0     	b	0x3c9b0
   3caac: e59d0018     	ldr	r0, [sp, #0x18]
   3cab0: e1500005     	cmp	r0, r5
   3cab4: 0affffc1     	beq	0x3c9c0
   3cab8: ebff64e0     	bl	0x15e40    @ imm = #-0x26c80 ; _ZdlPv
   3cabc: eaffffbf     	b	0x3c9c0
   3cac0: eaffffbe     	b	0x3c9c0
   3cac4: e59d0018     	ldr	r0, [sp, #0x18]
   3cac8: e1500005     	cmp	r0, r5
   3cacc: 0affffbf     	beq	0x3c9d0
   3cad0: ebff64da     	bl	0x15e40    @ imm = #-0x26c98 ; _ZdlPv
   3cad4: eaffffbd     	b	0x3c9d0
   3cad8: eaffffbc     	b	0x3c9d0
   3cadc: e5940030     	ldr	r0, [r4, #0x30]
   3cae0: e1580000     	cmp	r8, r0
   3cae4: 0afffff6     	beq	0x3cac4
   3cae8: ebff64d4     	bl	0x15e40    @ imm = #-0x26cb0 ; _ZdlPv
   3caec: eafffff4     	b	0x3cac4
   3caf0: 70 4f 09 00  	.word	0x00094f70
   3caf4: 20 1f 07 00  	.word	0x00071f20
   3caf8: 88 4f 09 00  	.word	0x00094f88
   3cafc: 30 1f 07 00  	.word	0x00071f30
   3cb00: c4 2e 07 00  	.word	0x00072ec4
   3cb04: a0 4f 09 00  	.word	0x00094fa0
   3cb08: e0 1e 07 00  	.word	0x00071ee0
   3cb0c: b8 4f 09 00  	.word	0x00094fb8
   3cb10: 40 1f 07 00  	.word	0x00071f40
   3cb14: d4 2e 07 00  	.word	0x00072ed4
   3cb18: 50 1f 07 00  	.word	0x00071f50
   3cb1c: e4 2e 07 00  	.word	0x00072ee4
   3cb20: e8 4f 09 00  	.word	0x00094fe8
   3cb24: 00 50 09 00  	.word	0x00095000
   3cb28: 18 50 09 00  	.word	0x00095018
   3cb2c: 30 50 09 00  	.word	0x00095030
   3cb30: 00 1f 07 00  	.word	0x00071f00
   3cb34: a4 2e 07 00  	.word	0x00072ea4
   3cb38: 48 50 09 00  	.word	0x00095048
   3cb3c: 60 1f 07 00  	.word	0x00071f60
