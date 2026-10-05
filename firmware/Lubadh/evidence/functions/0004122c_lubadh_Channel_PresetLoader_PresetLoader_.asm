; lubadh::Channel::PresetLoader::~PresetLoader()
; VA 0x4122c size 308

   4122c: e92d4070     	push	{r4, r5, r6, lr}
   41230: e1a04000     	mov	r4, r0
   41234: e59f3108     	ldr	r3, [pc, #0x108]        @ 0x41344
   41238: e2805f46     	add	r5, r0, #280
   4123c: e5a03140     	str	r3, [r0, #0x140]!
   41240: ebffb7dd     	bl	0x2f1bc
   41244: e59f30fc     	ldr	r3, [pc, #0xfc]         @ 0x41348
   41248: e5843118     	str	r3, [r4, #0x118]
   4124c: e1a00005     	mov	r0, r5
   41250: ebffbc24     	bl	0x302e8
   41254: e1a00005     	mov	r0, r5
   41258: ebffbc7b     	bl	0x3044c
   4125c: e5940120     	ldr	r0, [r4, #0x120]
   41260: e2843f4a     	add	r3, r4, #296
   41264: e1500003     	cmp	r0, r3
   41268: 0a000000     	beq	0x41270
   4126c: ebff52f3     	bl	0x15e40    @ imm = #-0x2b434 ; _ZdlPv
   41270: e28400fc     	add	r0, r4, #252
   41274: e59f60d0     	ldr	r6, [pc, #0xd0]         @ 0x4134c
   41278: eb00bc84     	bl	0x70490
   4127c: e28400e0     	add	r0, r4, #224
   41280: eb00bc82     	bl	0x70490
   41284: e1a00004     	mov	r0, r4
   41288: e2845094     	add	r5, r4, #148
   4128c: e5a060b8     	str	r6, [r0, #0xb8]!
   41290: ebffb7c9     	bl	0x2f1bc
   41294: e59f30b4     	ldr	r3, [pc, #0xb4]         @ 0x41350
   41298: e5843094     	str	r3, [r4, #0x94]
   4129c: e1a00005     	mov	r0, r5
   412a0: ebffb8a8     	bl	0x2f548
   412a4: e1a00005     	mov	r0, r5
   412a8: ebffb8ff     	bl	0x2f6ac
   412ac: e594009c     	ldr	r0, [r4, #0x9c]
   412b0: e28430a4     	add	r3, r4, #164
   412b4: e1500003     	cmp	r0, r3
   412b8: 0a000000     	beq	0x412c0
   412bc: ebff52df     	bl	0x15e40    @ imm = #-0x2b484 ; _ZdlPv
   412c0: e59f308c     	ldr	r3, [pc, #0x8c]         @ 0x41354
   412c4: e2845070     	add	r5, r4, #112
   412c8: e5843070     	str	r3, [r4, #0x70]
   412cc: e1a00005     	mov	r0, r5
   412d0: ebffbcdd     	bl	0x3064c
   412d4: e1a00005     	mov	r0, r5
   412d8: ebffbd34     	bl	0x307b0
   412dc: e5940078     	ldr	r0, [r4, #0x78]
   412e0: e2843080     	add	r3, r4, #128
   412e4: e1500003     	cmp	r0, r3
   412e8: 0a000000     	beq	0x412f0
   412ec: ebff52d3     	bl	0x15e40    @ imm = #-0x2b4b4 ; _ZdlPv
   412f0: e1a00004     	mov	r0, r4
   412f4: e2845028     	add	r5, r4, #40
   412f8: e5a0604c     	str	r6, [r0, #0x4c]!
   412fc: ebffb7ae     	bl	0x2f1bc
   41300: e59f3050     	ldr	r3, [pc, #0x50]         @ 0x41358
   41304: e5843028     	str	r3, [r4, #0x28]
   41308: e1a00005     	mov	r0, r5
   4130c: ebffbda7     	bl	0x309b0
   41310: e1a00005     	mov	r0, r5
   41314: ebffbdfe     	bl	0x30b14
   41318: e5940030     	ldr	r0, [r4, #0x30]
   4131c: e2843038     	add	r3, r4, #56
   41320: e1500003     	cmp	r0, r3
   41324: 0a000000     	beq	0x4132c
   41328: ebff52c4     	bl	0x15e40    @ imm = #-0x2b4f0 ; _ZdlPv
   4132c: e1a00004     	mov	r0, r4
   41330: e59f3024     	ldr	r3, [pc, #0x24]         @ 0x4135c
   41334: e5a03004     	str	r3, [r0, #0x4]!
   41338: ebffb79f     	bl	0x2f1bc
   4133c: e1a00004     	mov	r0, r4
   41340: e8bd8070     	pop	{r4, r5, r6, pc}
   41344: 60 1f 07 00  	.word	0x00071f60
   41348: 00 1f 07 00  	.word	0x00071f00
   4134c: e0 1e 07 00  	.word	0x00071ee0
   41350: 50 1f 07 00  	.word	0x00071f50
   41354: 40 1f 07 00  	.word	0x00071f40
   41358: 30 1f 07 00  	.word	0x00071f30
   4135c: 20 1f 07 00  	.word	0x00071f20
