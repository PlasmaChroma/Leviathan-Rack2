; lubadh::Application::saveCalibration(std::array<lubadh::VpOctCalibration, 2u>&)
; VA 0x2a11c size 964

   2a11c: e92d43f0     	push	{r4, r5, r6, r7, r8, r9, lr}
   2a120: e30f5810     	movw	r5, #0xf810
   2a124: e3405008     	movt	r5, #0x8
   2a128: e24ddf57     	sub	sp, sp, #348
   2a12c: e3006f94     	movw	r6, #0xf94
   2a130: e3406007     	movt	r6, #0x7
   2a134: e1a0000d     	mov	r0, sp
   2a138: e3007f9c     	movw	r7, #0xf9c
   2a13c: e3407007     	movt	r7, #0x7
   2a140: e3008fa4     	movw	r8, #0xfa4
   2a144: e3408007     	movt	r8, #0x7
   2a148: e1a04001     	mov	r4, r1
   2a14c: e2819020     	add	r9, r1, #32
   2a150: eb00f598     	bl	0x677b8
   2a154: e28d0014     	add	r0, sp, #20
   2a158: eb00f596     	bl	0x677b8
   2a15c: e5941000     	ldr	r1, [r4]
   2a160: e28d0028     	add	r0, sp, #40
   2a164: eb00d7ec     	bl	0x6011c
   2a168: e1a02006     	mov	r2, r6
   2a16c: e28d003c     	add	r0, sp, #60
   2a170: e28d1014     	add	r1, sp, #20
   2a174: eb00f6d3     	bl	0x67cc8
   2a178: e28d1028     	add	r1, sp, #40
   2a17c: e28d003c     	add	r0, sp, #60
   2a180: eb00efbc     	bl	0x66078
   2a184: e28d003c     	add	r0, sp, #60
   2a188: eb00f05f     	bl	0x6630c
   2a18c: e28d0028     	add	r0, sp, #40
   2a190: eb00dc1f     	bl	0x61214
   2a194: e5941004     	ldr	r1, [r4, #0x4]
   2a198: e28d0028     	add	r0, sp, #40
   2a19c: eb00d7de     	bl	0x6011c
   2a1a0: e1a02007     	mov	r2, r7
   2a1a4: e28d1014     	add	r1, sp, #20
   2a1a8: e28d003c     	add	r0, sp, #60
   2a1ac: eb00f6c5     	bl	0x67cc8
   2a1b0: e28d1028     	add	r1, sp, #40
   2a1b4: e28d003c     	add	r0, sp, #60
   2a1b8: eb00efae     	bl	0x66078
   2a1bc: e28d003c     	add	r0, sp, #60
   2a1c0: eb00f051     	bl	0x6630c
   2a1c4: e28d0028     	add	r0, sp, #40
   2a1c8: eb00dc11     	bl	0x61214
   2a1cc: e5941008     	ldr	r1, [r4, #0x8]
   2a1d0: e28d0028     	add	r0, sp, #40
   2a1d4: eb00d7d0     	bl	0x6011c
   2a1d8: e1a02008     	mov	r2, r8
   2a1dc: e28d1014     	add	r1, sp, #20
   2a1e0: e28d003c     	add	r0, sp, #60
   2a1e4: eb00f6b7     	bl	0x67cc8
   2a1e8: e28d1028     	add	r1, sp, #40
   2a1ec: e28d003c     	add	r0, sp, #60
   2a1f0: eb00efa0     	bl	0x66078
   2a1f4: e28d003c     	add	r0, sp, #60
   2a1f8: eb00f043     	bl	0x6630c
   2a1fc: e28d0028     	add	r0, sp, #40
   2a200: eb00dc03     	bl	0x61214
   2a204: e594100c     	ldr	r1, [r4, #0xc]
   2a208: e28d0028     	add	r0, sp, #40
   2a20c: eb00d7c2     	bl	0x6011c
   2a210: e3002fac     	movw	r2, #0xfac
   2a214: e3402007     	movt	r2, #0x7
   2a218: e28d1014     	add	r1, sp, #20
   2a21c: e28d003c     	add	r0, sp, #60
   2a220: eb00f6a8     	bl	0x67cc8
   2a224: e28d1028     	add	r1, sp, #40
   2a228: e28d003c     	add	r0, sp, #60
   2a22c: eb00ef91     	bl	0x66078
   2a230: e28d003c     	add	r0, sp, #60
   2a234: eb00f034     	bl	0x6630c
   2a238: e28d0028     	add	r0, sp, #40
   2a23c: eb00dbf4     	bl	0x61214
   2a240: e1a02005     	mov	r2, r5
   2a244: e1a0100d     	mov	r1, sp
   2a248: e28d003c     	add	r0, sp, #60
   2a24c: eb00f65b     	bl	0x67bc0
   2a250: e28d1014     	add	r1, sp, #20
   2a254: e28d003c     	add	r0, sp, #60
   2a258: eb00f209     	bl	0x66a84
   2a25c: e28d003c     	add	r0, sp, #60
   2a260: e2844010     	add	r4, r4, #16
   2a264: eb00f028     	bl	0x6630c
   2a268: e28d0014     	add	r0, sp, #20
   2a26c: eb00dbe8     	bl	0x61214
   2a270: e2855018     	add	r5, r5, #24
   2a274: e1540009     	cmp	r4, r9
   2a278: 1affffb5     	bne	0x2a154
   2a27c: f2c00050     	vmov.i32	q8, #0x0
   2a280: e28d003c     	add	r0, sp, #60
   2a284: e30513e4     	movw	r1, #0x53e4
   2a288: e3401007     	movt	r1, #0x7
   2a28c: edcd0b1b     	vstr	d16, [sp, #108]
   2a290: f4400a0f     	vst1.8	{d16, d17}, [r0]
   2a294: edcd0b13     	vstr	d16, [sp, #76]
   2a298: edcd0b15     	vstr	d16, [sp, #84]
   2a29c: edcd0b17     	vstr	d16, [sp, #92]
   2a2a0: edcd0b19     	vstr	d16, [sp, #100]
   2a2a4: edcd0b1c     	vstr	d16, [sp, #112]
   2a2a8: ebfff63e     	bl	0x27ba8
   2a2ac: e3001fb4     	movw	r1, #0xfb4
   2a2b0: e3401007     	movt	r1, #0x7
   2a2b4: e28d0058     	add	r0, sp, #88
   2a2b8: e3a03001     	mov	r3, #1
   2a2bc: e1cd35b4     	strh	r3, [sp, #84]
   2a2c0: e3a03000     	mov	r3, #0
   2a2c4: e5cd3056     	strb	r3, [sp, #0x56]
   2a2c8: ebfff636     	bl	0x27ba8
   2a2cc: e59f1208     	ldr	r1, [pc, #0x208]        @ 0x2a4dc&)+0x3c0>
   2a2d0: e28d203c     	add	r2, sp, #60
   2a2d4: e1a0000d     	mov	r0, sp
   2a2d8: e3a03401     	mov	r3, #16777216
   2a2dc: e58d3070     	str	r3, [sp, #0x70]
   2a2e0: e3a03c01     	mov	r3, #256
   2a2e4: e1cd37b4     	strh	r3, [sp, #116]
   2a2e8: eb00a9e1     	bl	0x54a74
   2a2ec: e59d0058     	ldr	r0, [sp, #0x58]
   2a2f0: e28d3060     	add	r3, sp, #96
   2a2f4: e1500003     	cmp	r0, r3
   2a2f8: 0a000000     	beq	0x2a300
   2a2fc: ebffaecf     	bl	0x15e40    @ imm = #-0x144c4 ; _ZdlPv
   2a300: e59d003c     	ldr	r0, [sp, #0x3c]
   2a304: e28d3044     	add	r3, sp, #68
   2a308: e1500003     	cmp	r0, r3
   2a30c: 0a000000     	beq	0x2a314
   2a310: ebffaeca     	bl	0x15e40    @ imm = #-0x144d8 ; _ZdlPv
   2a314: e1a0000d     	mov	r0, sp
   2a318: eb00dbbd     	bl	0x61214
   2a31c: e28ddf57     	add	sp, sp, #348
   2a320: e8bd83f0     	pop	{r4, r5, r6, r7, r8, r9, pc}
   2a324: e1a0000d     	mov	r0, sp
   2a328: eb00dbb9     	bl	0x61214
   2a32c: ebffaf0b     	bl	0x15f60    @ imm = #-0x143d4 ; __cxa_end_cleanup
   2a330: e1a04000     	mov	r4, r0
   2a334: e28d003c     	add	r0, sp, #60
   2a338: e1a05001     	mov	r5, r1
   2a33c: eb00114e     	bl	0x2e87c
   2a340: e1a00004     	mov	r0, r4
   2a344: e1a03005     	mov	r3, r5
   2a348: e3530001     	cmp	r3, #1
   2a34c: 1afffff4     	bne	0x2a324
   2a350: ebffadeb     	bl	0x15b04    @ imm = #-0x14854 ; __cxa_begin_catch
   2a354: e59f1180     	ldr	r1, [pc, #0x180]        @ 0x2a4dc&)+0x3c0>
   2a358: e3a02002     	mov	r2, #2
   2a35c: e28d003c     	add	r0, sp, #60
   2a360: eb00119b     	bl	0x2e9d4
   2a364: e28d103c     	add	r1, sp, #60
   2a368: e28d0028     	add	r0, sp, #40
   2a36c: ebffaddb     	bl	0x15ae0    @ imm = #-0x14894 ; _ZNSt10filesystem6statusERKNS_7__cxx114pathE
   2a370: e5dd3028     	ldrb	r3, [sp, #0x28]
   2a374: e2833001     	add	r3, r3, #1
   2a378: e6ef3073     	uxtb	r3, r3
   2a37c: e3530001     	cmp	r3, #1
   2a380: 9a000017     	bls	0x2a3e4
   2a384: e28d003c     	add	r0, sp, #60
   2a388: eb001149     	bl	0x2e8b4
   2a38c: ea00001c     	b	0x2a404
   2a390: e59d303c     	ldr	r3, [sp, #0x3c]
   2a394: e28d2044     	add	r2, sp, #68
   2a398: e1a04000     	mov	r4, r0
   2a39c: e1a05001     	mov	r5, r1
   2a3a0: e1530002     	cmp	r3, r2
   2a3a4: 0affffe5     	beq	0x2a340
   2a3a8: e1a00003     	mov	r0, r3
   2a3ac: ebffaea3     	bl	0x15e40    @ imm = #-0x14574 ; _ZdlPv
   2a3b0: eaffffe2     	b	0x2a340
   2a3b4: e28d003c     	add	r0, sp, #60
   2a3b8: eb00efd3     	bl	0x6630c
   2a3bc: e28d0014     	add	r0, sp, #20
   2a3c0: eb00db93     	bl	0x61214
   2a3c4: eaffffd6     	b	0x2a324
   2a3c8: e1a03001     	mov	r3, r1
   2a3cc: eaffffdd     	b	0x2a348
   2a3d0: e28d003c     	add	r0, sp, #60
   2a3d4: eb00efcc     	bl	0x6630c
   2a3d8: e28d0028     	add	r0, sp, #40
   2a3dc: eb00db8c     	bl	0x61214
   2a3e0: eafffff5     	b	0x2a3bc
   2a3e4: e28d003c     	add	r0, sp, #60
   2a3e8: eb001131     	bl	0x2e8b4
   2a3ec: e59f10e8     	ldr	r1, [pc, #0xe8]         @ 0x2a4dc&)+0x3c0>
   2a3f0: e3a02010     	mov	r2, #16
   2a3f4: e28d003c     	add	r0, sp, #60
   2a3f8: ebffafaa     	bl	0x162a8    @ imm = #-0x14158 ; _ZNSt13basic_fstreamIcSt11char_traitsIcEEC1ERKNSt7__cxx1112basic_stringIcS1_SaIcEEESt13_Ios_Openmode
   2a3fc: e28d003c     	add	r0, sp, #60
   2a400: ebffad68     	bl	0x159a8     @ imm = #-0x14a60 ; _ZNSt13basic_fstreamIcSt11char_traitsIcEED1Ev
   2a404: f2c00050     	vmov.i32	q8, #0x0
   2a408: e28d003c     	add	r0, sp, #60
   2a40c: e30513e4     	movw	r1, #0x53e4
   2a410: e3401007     	movt	r1, #0x7
   2a414: edcd0b1b     	vstr	d16, [sp, #108]
   2a418: f4400a0f     	vst1.8	{d16, d17}, [r0]
   2a41c: edcd0b13     	vstr	d16, [sp, #76]
   2a420: edcd0b15     	vstr	d16, [sp, #84]
   2a424: edcd0b17     	vstr	d16, [sp, #92]
   2a428: edcd0b19     	vstr	d16, [sp, #100]
   2a42c: edcd0b1c     	vstr	d16, [sp, #112]
   2a430: ebfff5dc     	bl	0x27ba8
   2a434: e3001fb4     	movw	r1, #0xfb4
   2a438: e3401007     	movt	r1, #0x7
   2a43c: e28d0058     	add	r0, sp, #88
   2a440: e3a03001     	mov	r3, #1
   2a444: e1cd35b4     	strh	r3, [sp, #84]
   2a448: e3a03000     	mov	r3, #0
   2a44c: e5cd3056     	strb	r3, [sp, #0x56]
   2a450: ebfff5d4     	bl	0x27ba8
   2a454: e59f1080     	ldr	r1, [pc, #0x80]         @ 0x2a4dc&)+0x3c0>
   2a458: e28d203c     	add	r2, sp, #60
   2a45c: e1a0000d     	mov	r0, sp
   2a460: e3a03401     	mov	r3, #16777216
   2a464: e58d3070     	str	r3, [sp, #0x70]
   2a468: e3a03c01     	mov	r3, #256
   2a46c: e1cd37b4     	strh	r3, [sp, #116]
   2a470: eb00a97f     	bl	0x54a74
   2a474: e28d003c     	add	r0, sp, #60
   2a478: eb0010ff     	bl	0x2e87c
   2a47c: ebffaf7a     	bl	0x1626c    @ imm = #-0x14218 ; __cxa_end_catch
   2a480: eaffffa3     	b	0x2a314
   2a484: e28d003c     	add	r0, sp, #60
   2a488: eb0010fb     	bl	0x2e87c
   2a48c: ebffaf76     	bl	0x1626c    @ imm = #-0x14228 ; __cxa_end_catch
   2a490: eaffffa3     	b	0x2a324
   2a494: e59d003c     	ldr	r0, [sp, #0x3c]
   2a498: e28d3044     	add	r3, sp, #68
   2a49c: e1500003     	cmp	r0, r3
   2a4a0: 0afffff9     	beq	0x2a48c
   2a4a4: ebffae65     	bl	0x15e40    @ imm = #-0x1466c ; _ZdlPv
   2a4a8: eafffff7     	b	0x2a48c
   2a4ac: e28d003c     	add	r0, sp, #60
   2a4b0: eb0010ff     	bl	0x2e8b4
   2a4b4: eafffff4     	b	0x2a48c
   2a4b8: eafffff3     	b	0x2a48c
   2a4bc: eaffffc5     	b	0x2a3d8
   2a4c0: eaffffc2     	b	0x2a3d0
   2a4c4: eaffffc3     	b	0x2a3d8
   2a4c8: eaffffc0     	b	0x2a3d0
   2a4cc: eaffffc1     	b	0x2a3d8
   2a4d0: eaffffbe     	b	0x2a3d0
   2a4d4: eaffffbf     	b	0x2a3d8
   2a4d8: eaffffb7     	b	0x2a3bc
   2a4dc: dc fc 08 00  	.word	0x0008fcdc
