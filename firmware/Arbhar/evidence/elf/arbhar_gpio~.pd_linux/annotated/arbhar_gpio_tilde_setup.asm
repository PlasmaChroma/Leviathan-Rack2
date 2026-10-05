0000ff3c <arbhar_gpio_tilde_setup>:
    ff3c: e59f0494     	ldr	r0, [pc, #0x494]        @ 0x103d8 <arbhar_gpio_tilde_setup+0x49c>  // u32=0x5bc4; f32?=3.29193035e-41
    ff40: e92d41f0     	push	{r4, r5, r6, r7, r8, lr}
    ff44: e08f0000     	add	r0, pc, r0
    ff48: e24dd010     	sub	sp, sp, #16
    ff4c: e59f6488     	ldr	r6, [pc, #0x488]        @ 0x103dc <arbhar_gpio_tilde_setup+0x4a0>  // u32=0x1709c; f32?=1.32232128e-40
    ff50: ebffcdf4     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xc830  // CALL gensym
    ff54: e59f2484     	ldr	r2, [pc, #0x484]        @ 0x103e0 <arbhar_gpio_tilde_setup+0x4a4>  // u32=0x298; f32?=9.3046218e-43
    ff58: e59f1484     	ldr	r1, [pc, #0x484]        @ 0x103e4 <arbhar_gpio_tilde_setup+0x4a8>  // u32=0x280; f32?=8.96831017e-43
    ff5c: e08f6006     	add	r6, pc, r6
    ff60: e3a05000     	mov	r5, #0
    ff64: e3a0c006     	mov	r12, #6
    ff68: e7962002     	ldr	r2, [r6, r2]
    ff6c: e3013fb0     	movw	r3, #0x1fb0
    ff70: e7961001     	ldr	r1, [r6, r1]
    ff74: e3a07001     	mov	r7, #1
    ff78: e58dc008     	str	r12, [sp, #0x8]
    ff7c: e58dc004     	str	r12, [sp, #0x4]
    ff80: e58d500c     	str	r5, [sp, #0xc]
    ff84: e58d5000     	str	r5, [sp]
    ff88: ebffcf2d     	bl	0x3c44 <.plt+0x548>     @ imm = #-0xc34c  // CALL class_new
    ff8c: e59f4454     	ldr	r4, [pc, #0x454]        @ 0x103e8 <arbhar_gpio_tilde_setup+0x4ac>  // u32=0x1741c; f32?=1.33487692e-40
    ff90: e59f3454     	ldr	r3, [pc, #0x454]        @ 0x103ec <arbhar_gpio_tilde_setup+0x4b0>  // u32=0x5b7c; f32?=3.281841e-41
    ff94: e08f4004     	add	r4, pc, r4
    ff98: e1a08000     	mov	r8, r0
    ff9c: e08f0003     	add	r0, pc, r3
    ffa0: e58481c0     	str	r8, [r4, #0x1c0]
    ffa4: ebffcddf     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xc884  // CALL gensym
    ffa8: e59f2440     	ldr	r2, [pc, #0x440]        @ 0x103f0 <arbhar_gpio_tilde_setup+0x4b4>  // u32=0x2e0; f32?=1.03135567e-42
    ffac: e1a03005     	mov	r3, r5
    ffb0: e7961002     	ldr	r1, [r6, r2]
    ffb4: e1a02000     	mov	r2, r0
    ffb8: e1a00008     	mov	r0, r8
    ffbc: ebffcf62     	bl	0x3d4c <.plt+0x650>     @ imm = #-0xc278  // CALL class_addmethod
    ffc0: e59fc42c     	ldr	r12, [pc, #0x42c]       @ 0x103f4 <arbhar_gpio_tilde_setup+0x4b8>  // u32=0xffffce48; f32?=nan
    ffc4: e59401c0     	ldr	r0, [r4, #0x1c0]
    ffc8: e08f100c     	add	r1, pc, r12
    ffcc: ebffce2c     	bl	0x3884 <.plt+0x188>     @ imm = #-0xc750  // CALL class_addbang
    ffd0: e59f0420     	ldr	r0, [pc, #0x420]        @ 0x103f8 <arbhar_gpio_tilde_setup+0x4bc>  // u32=0x5b44; f32?=3.27399373e-41
    ffd4: e59481c0     	ldr	r8, [r4, #0x1c0]
    ffd8: e08f0000     	add	r0, pc, r0
    ffdc: ebffcdd1     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xc8bc  // CALL gensym
    ffe0: e59f1414     	ldr	r1, [pc, #0x414]        @ 0x103fc <arbhar_gpio_tilde_setup+0x4c0>  // u32=0xffff6398; f32?=nan
    ffe4: e58d5000     	str	r5, [sp]
    ffe8: e1a03007     	mov	r3, r7
    ffec: e08f1001     	add	r1, pc, r1
    fff0: e1a02000     	mov	r2, r0
    fff4: e1a00008     	mov	r0, r8
    fff8: ebffcf53     	bl	0x3d4c <.plt+0x650>     @ imm = #-0xc2b4  // CALL class_addmethod
    fffc: e59f33fc     	ldr	r3, [pc, #0x3fc]        @ 0x10400 <arbhar_gpio_tilde_setup+0x4c4>  // u32=0x5b28; f32?=3.2700701e-41
   10000: e59481c0     	ldr	r8, [r4, #0x1c0]
   10004: e08f0003     	add	r0, pc, r3
   10008: ebffcdc6     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xc8e8  // CALL gensym
   1000c: e59f23f0     	ldr	r2, [pc, #0x3f0]        @ 0x10404 <arbhar_gpio_tilde_setup+0x4c8>  // u32=0x2a8; f32?=9.52882956e-43
   10010: e1a03007     	mov	r3, r7
   10014: e7961002     	ldr	r1, [r6, r2]
   10018: e58d5000     	str	r5, [sp]
   1001c: e1a02000     	mov	r2, r0
   10020: e1a00008     	mov	r0, r8
   10024: ebffcf48     	bl	0x3d4c <.plt+0x650>     @ imm = #-0xc2e0  // CALL class_addmethod
   10028: e59fc3d8     	ldr	r12, [pc, #0x3d8]       @ 0x10408 <arbhar_gpio_tilde_setup+0x4cc>  // u32=0x5b08; f32?=3.26558594e-41
   1002c: e59481c0     	ldr	r8, [r4, #0x1c0]
   10030: e08f000c     	add	r0, pc, r12
   10034: ebffcdbb     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xc914  // CALL gensym
   10038: e59f13cc     	ldr	r1, [pc, #0x3cc]        @ 0x1040c <arbhar_gpio_tilde_setup+0x4d0>  // u32=0x26c; f32?=8.68805048e-43
   1003c: e1a03007     	mov	r3, r7
   10040: e7961001     	ldr	r1, [r6, r1]
   10044: e58d5000     	str	r5, [sp]
   10048: e1a02000     	mov	r2, r0
   1004c: e1a00008     	mov	r0, r8
   10050: ebffcf3d     	bl	0x3d4c <.plt+0x650>     @ imm = #-0xc30c  // CALL class_addmethod
   10054: e59f03b4     	ldr	r0, [pc, #0x3b4]        @ 0x10410 <arbhar_gpio_tilde_setup+0x4d4>  // u32=0x5ae8; f32?=3.26110179e-41
   10058: e59481c0     	ldr	r8, [r4, #0x1c0]
   1005c: e08f0000     	add	r0, pc, r0
   10060: ebffcdb0     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xc940  // CALL gensym
   10064: e59f23a8     	ldr	r2, [pc, #0x3a8]        @ 0x10414 <arbhar_gpio_tilde_setup+0x4d8>  // u32=0x27c; f32?=8.91225823e-43
   10068: e1a03007     	mov	r3, r7
   1006c: e7961002     	ldr	r1, [r6, r2]
   10070: e58d5000     	str	r5, [sp]
   10074: e1a02000     	mov	r2, r0
   10078: e1a00008     	mov	r0, r8
   1007c: ebffcf32     	bl	0x3d4c <.plt+0x650>     @ imm = #-0xc338  // CALL class_addmethod
   10080: e59f3390     	ldr	r3, [pc, #0x390]        @ 0x10418 <arbhar_gpio_tilde_setup+0x4dc>  // u32=0x5ac8; f32?=3.25661763e-41
   10084: e59481c0     	ldr	r8, [r4, #0x1c0]
   10088: e08f0003     	add	r0, pc, r3
   1008c: ebffcda5     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xc96c  // CALL gensym
   10090: e59fc384     	ldr	r12, [pc, #0x384]       @ 0x1041c <arbhar_gpio_tilde_setup+0x4e0>  // u32=0x294; f32?=9.24856986e-43
   10094: e1a03007     	mov	r3, r7
   10098: e796100c     	ldr	r1, [r6, r12]
   1009c: e58d5000     	str	r5, [sp]
   100a0: e1a02000     	mov	r2, r0
   100a4: e1a00008     	mov	r0, r8
   100a8: ebffcf27     	bl	0x3d4c <.plt+0x650>     @ imm = #-0xc364  // CALL class_addmethod
   100ac: e59f136c     	ldr	r1, [pc, #0x36c]        @ 0x10420 <arbhar_gpio_tilde_setup+0x4e4>  // u32=0x5aa8; f32?=3.25213348e-41
   100b0: e59481c0     	ldr	r8, [r4, #0x1c0]
   100b4: e08f0001     	add	r0, pc, r1
   100b8: ebffcd9a     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xc998  // CALL gensym
   100bc: e59f2360     	ldr	r2, [pc, #0x360]        @ 0x10424 <arbhar_gpio_tilde_setup+0x4e8>  // u32=0x288; f32?=9.08041405e-43
   100c0: e1a03007     	mov	r3, r7
   100c4: e7961002     	ldr	r1, [r6, r2]
   100c8: e58d5000     	str	r5, [sp]
   100cc: e1a02000     	mov	r2, r0
   100d0: e1a00008     	mov	r0, r8
   100d4: ebffcf1c     	bl	0x3d4c <.plt+0x650>     @ imm = #-0xc390  // CALL class_addmethod
   100d8: e59f0348     	ldr	r0, [pc, #0x348]        @ 0x10428 <arbhar_gpio_tilde_setup+0x4ec>  // u32=0x5a88; f32?=3.24764932e-41
   100dc: e59481c0     	ldr	r8, [r4, #0x1c0]
   100e0: e08f0000     	add	r0, pc, r0
   100e4: ebffcd8f     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xc9c4  // CALL gensym
   100e8: e59fc33c     	ldr	r12, [pc, #0x33c]       @ 0x1042c <arbhar_gpio_tilde_setup+0x4f0>  // u32=0xffff4264; f32?=nan
   100ec: e58d5000     	str	r5, [sp]
   100f0: e1a03007     	mov	r3, r7
   100f4: e08f100c     	add	r1, pc, r12
   100f8: e1a02000     	mov	r2, r0
   100fc: e1a00008     	mov	r0, r8
   10100: ebffcf11     	bl	0x3d4c <.plt+0x650>     @ imm = #-0xc3bc  // CALL class_addmethod
   10104: e59f3324     	ldr	r3, [pc, #0x324]        @ 0x10430 <arbhar_gpio_tilde_setup+0x4f4>  // u32=0x5a6c; f32?=3.24372569e-41
   10108: e59481c0     	ldr	r8, [r4, #0x1c0]
   1010c: e08f0003     	add	r0, pc, r3
   10110: ebffcd84     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xc9f0  // CALL gensym
   10114: e59f1318     	ldr	r1, [pc, #0x318]        @ 0x10434 <arbhar_gpio_tilde_setup+0x4f8>  // u32=0xffff4218; f32?=nan
   10118: e1a03007     	mov	r3, r7
   1011c: e58d5000     	str	r5, [sp]
   10120: e08f1001     	add	r1, pc, r1
   10124: e1a02000     	mov	r2, r0
   10128: e1a00008     	mov	r0, r8
   1012c: ebffcf06     	bl	0x3d4c <.plt+0x650>     @ imm = #-0xc3e8  // CALL class_addmethod
   10130: e59f2300     	ldr	r2, [pc, #0x300]        @ 0x10438 <arbhar_gpio_tilde_setup+0x4fc>  // u32=0x5a54; f32?=3.24036257e-41
   10134: e59481c0     	ldr	r8, [r4, #0x1c0]
   10138: e08f0002     	add	r0, pc, r2
   1013c: ebffcd79     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xca1c  // CALL gensym
   10140: e59fc2f4     	ldr	r12, [pc, #0x2f4]       @ 0x1043c <arbhar_gpio_tilde_setup+0x500>  // u32=0xffff6c80; f32?=nan
   10144: e1a03007     	mov	r3, r7
   10148: e58d5000     	str	r5, [sp]
   1014c: e08f100c     	add	r1, pc, r12
   10150: e1a02000     	mov	r2, r0
   10154: e1a00008     	mov	r0, r8
   10158: ebffcefb     	bl	0x3d4c <.plt+0x650>     @ imm = #-0xc414  // CALL class_addmethod
   1015c: e59f02dc     	ldr	r0, [pc, #0x2dc]        @ 0x10440 <arbhar_gpio_tilde_setup+0x504>  // u32=0x5a34; f32?=3.23587841e-41
   10160: e59481c0     	ldr	r8, [r4, #0x1c0]
   10164: e08f0000     	add	r0, pc, r0
   10168: ebffcd6e     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xca48  // CALL gensym
   1016c: e59f12d0     	ldr	r1, [pc, #0x2d0]        @ 0x10444 <arbhar_gpio_tilde_setup+0x508>  // u32=0xffff4174; f32?=nan
   10170: e58d5000     	str	r5, [sp]
   10174: e1a03007     	mov	r3, r7
   10178: e08f1001     	add	r1, pc, r1
   1017c: e1a02000     	mov	r2, r0
   10180: e1a00008     	mov	r0, r8
   10184: ebffcef0     	bl	0x3d4c <.plt+0x650>     @ imm = #-0xc440  // CALL class_addmethod
   10188: e59f32b8     	ldr	r3, [pc, #0x2b8]        @ 0x10448 <arbhar_gpio_tilde_setup+0x50c>  // u32=0x5a1c; f32?=3.2325153e-41
   1018c: e59481c0     	ldr	r8, [r4, #0x1c0]
   10190: e08f0003     	add	r0, pc, r3
   10194: ebffcd63     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xca74  // CALL gensym
   10198: e59f22ac     	ldr	r2, [pc, #0x2ac]        @ 0x1044c <arbhar_gpio_tilde_setup+0x510>  // u32=0x278; f32?=8.85620629e-43
   1019c: e1a03005     	mov	r3, r5
   101a0: e7961002     	ldr	r1, [r6, r2]
   101a4: e1a02000     	mov	r2, r0
   101a8: e1a00008     	mov	r0, r8
   101ac: ebffcee6     	bl	0x3d4c <.plt+0x650>     @ imm = #-0xc468  // CALL class_addmethod
   101b0: e59fc298     	ldr	r12, [pc, #0x298]       @ 0x10450 <arbhar_gpio_tilde_setup+0x514>  // u32=0x5a00; f32?=3.22859166e-41
   101b4: e59481c0     	ldr	r8, [r4, #0x1c0]
   101b8: e08f000c     	add	r0, pc, r12
   101bc: ebffcd59     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xca9c  // CALL gensym
   101c0: e59f128c     	ldr	r1, [pc, #0x28c]        @ 0x10454 <arbhar_gpio_tilde_setup+0x518>  // u32=0xffff422c; f32?=nan
   101c4: e58d5000     	str	r5, [sp]
   101c8: e3a0300a     	mov	r3, #10
   101cc: e08f1001     	add	r1, pc, r1
   101d0: e1a02000     	mov	r2, r0
   101d4: e1a00008     	mov	r0, r8
   101d8: ebffcedb     	bl	0x3d4c <.plt+0x650>     @ imm = #-0xc494  // CALL class_addmethod
   101dc: e59f0274     	ldr	r0, [pc, #0x274]        @ 0x10458 <arbhar_gpio_tilde_setup+0x51c>  // u32=0x59dc; f32?=3.22354699e-41
   101e0: e59481c0     	ldr	r8, [r4, #0x1c0]
   101e4: e08f0000     	add	r0, pc, r0
   101e8: ebffcd4e     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xcac8  // CALL gensym
   101ec: e59f2268     	ldr	r2, [pc, #0x268]        @ 0x1045c <arbhar_gpio_tilde_setup+0x520>  // u32=0x2b8; f32?=9.75303731e-43
   101f0: e1a03005     	mov	r3, r5
   101f4: e7961002     	ldr	r1, [r6, r2]
   101f8: e1a02000     	mov	r2, r0
   101fc: e1a00008     	mov	r0, r8
   10200: ebffced1     	bl	0x3d4c <.plt+0x650>     @ imm = #-0xc4bc  // CALL class_addmethod
   10204: e59f3254     	ldr	r3, [pc, #0x254]        @ 0x10460 <arbhar_gpio_tilde_setup+0x524>  // u32=0x59c0; f32?=3.21962335e-41
   10208: e59481c0     	ldr	r8, [r4, #0x1c0]
   1020c: e08f0003     	add	r0, pc, r3
   10210: ebffcd44     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xcaf0  // CALL gensym
   10214: e59fc248     	ldr	r12, [pc, #0x248]       @ 0x10464 <arbhar_gpio_tilde_setup+0x528>  // u32=0xffff3e74; f32?=nan
   10218: e1a03007     	mov	r3, r7
   1021c: e58d5004     	str	r5, [sp, #0x4]
   10220: e08f100c     	add	r1, pc, r12
   10224: e58d7000     	str	r7, [sp]
   10228: e1a02000     	mov	r2, r0
   1022c: e1a00008     	mov	r0, r8
   10230: ebffcec5     	bl	0x3d4c <.plt+0x650>     @ imm = #-0xc4ec  // CALL class_addmethod
   10234: e59f122c     	ldr	r1, [pc, #0x22c]        @ 0x10468 <arbhar_gpio_tilde_setup+0x52c>  // u32=0x59a0; f32?=3.2151392e-41
   10238: e59481c0     	ldr	r8, [r4, #0x1c0]
   1023c: e08f0001     	add	r0, pc, r1
   10240: ebffcd38     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xcb20  // CALL gensym
   10244: e59f2220     	ldr	r2, [pc, #0x220]        @ 0x1046c <arbhar_gpio_tilde_setup+0x530>  // u32=0xffff3e74; f32?=nan
   10248: e1a03007     	mov	r3, r7
   1024c: e58d5004     	str	r5, [sp, #0x4]
   10250: e08f1002     	add	r1, pc, r2
   10254: e58d7000     	str	r7, [sp]
   10258: e1a02000     	mov	r2, r0
   1025c: e1a00008     	mov	r0, r8
   10260: ebffceb9     	bl	0x3d4c <.plt+0x650>     @ imm = #-0xc51c  // CALL class_addmethod
   10264: e59f0204     	ldr	r0, [pc, #0x204]        @ 0x10470 <arbhar_gpio_tilde_setup+0x534>  // u32=0x5980; f32?=3.21065504e-41
   10268: e59481c0     	ldr	r8, [r4, #0x1c0]
   1026c: e08f0000     	add	r0, pc, r0
   10270: ebffcd2c     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xcb50  // CALL gensym
   10274: e59fc1f8     	ldr	r12, [pc, #0x1f8]       @ 0x10474 <arbhar_gpio_tilde_setup+0x538>  // u32=0x2d8; f32?=1.02014528e-42
   10278: e1a03007     	mov	r3, r7
   1027c: e796100c     	ldr	r1, [r6, r12]
   10280: e58d5004     	str	r5, [sp, #0x4]
   10284: e58d7000     	str	r7, [sp]
   10288: e1a02000     	mov	r2, r0
   1028c: e1a00008     	mov	r0, r8
   10290: ebffcead     	bl	0x3d4c <.plt+0x650>     @ imm = #-0xc54c  // CALL class_addmethod
   10294: e59f31dc     	ldr	r3, [pc, #0x1dc]        @ 0x10478 <arbhar_gpio_tilde_setup+0x53c>  // u32=0x5960; f32?=3.20617089e-41
   10298: e59481c0     	ldr	r8, [r4, #0x1c0]
   1029c: e08f0003     	add	r0, pc, r3
   102a0: ebffcd20     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xcb80  // CALL gensym
   102a4: e59f11d0     	ldr	r1, [pc, #0x1d0]        @ 0x1047c <arbhar_gpio_tilde_setup+0x540>  // u32=0xffff4408; f32?=nan
   102a8: e1a03007     	mov	r3, r7
   102ac: e58d5000     	str	r5, [sp]
   102b0: e08f1001     	add	r1, pc, r1
   102b4: e1a02000     	mov	r2, r0
   102b8: e1a00008     	mov	r0, r8
   102bc: ebffcea2     	bl	0x3d4c <.plt+0x650>     @ imm = #-0xc578  // CALL class_addmethod
   102c0: e59f21b8     	ldr	r2, [pc, #0x1b8]        @ 0x10480 <arbhar_gpio_tilde_setup+0x544>  // u32=0x594c; f32?=3.20336829e-41
   102c4: e59481c0     	ldr	r8, [r4, #0x1c0]
   102c8: e08f0002     	add	r0, pc, r2
   102cc: ebffcd15     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xcbac  // CALL gensym
   102d0: e59fc1ac     	ldr	r12, [pc, #0x1ac]       @ 0x10484 <arbhar_gpio_tilde_setup+0x548>  // u32=0x2c0; f32?=9.86514119e-43
   102d4: e1a03007     	mov	r3, r7
   102d8: e796100c     	ldr	r1, [r6, r12]
   102dc: e58d5000     	str	r5, [sp]
   102e0: e1a02000     	mov	r2, r0
   102e4: e1a00008     	mov	r0, r8
   102e8: ebffce97     	bl	0x3d4c <.plt+0x650>     @ imm = #-0xc5a4  // CALL class_addmethod
   102ec: e59f0194     	ldr	r0, [pc, #0x194]        @ 0x10488 <arbhar_gpio_tilde_setup+0x54c>  // u32=0x5934; f32?=3.20000517e-41
   102f0: e59481c0     	ldr	r8, [r4, #0x1c0]
   102f4: e08f0000     	add	r0, pc, r0
   102f8: ebffcd0a     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xcbd8  // CALL gensym
   102fc: e59f1188     	ldr	r1, [pc, #0x188]        @ 0x1048c <arbhar_gpio_tilde_setup+0x550>  // u32=0x2dc; f32?=1.02575048e-42
   10300: e1a03005     	mov	r3, r5
   10304: e7961001     	ldr	r1, [r6, r1]
   10308: e1a02000     	mov	r2, r0
   1030c: e1a00008     	mov	r0, r8
   10310: ebffce8d     	bl	0x3d4c <.plt+0x650>     @ imm = #-0xc5cc  // CALL class_addmethod
   10314: e59f3174     	ldr	r3, [pc, #0x174]        @ 0x10490 <arbhar_gpio_tilde_setup+0x554>  // u32=0x5920; f32?=3.19720258e-41
   10318: e59481c0     	ldr	r8, [r4, #0x1c0]
   1031c: e08f0003     	add	r0, pc, r3
   10320: ebffcd00     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xcc00  // CALL gensym
   10324: e59f2168     	ldr	r2, [pc, #0x168]        @ 0x10494 <arbhar_gpio_tilde_setup+0x558>  // u32=0x270; f32?=8.74410242e-43
   10328: e3a0300a     	mov	r3, #10
   1032c: e7961002     	ldr	r1, [r6, r2]
   10330: e58d5000     	str	r5, [sp]
   10334: e1a02000     	mov	r2, r0
   10338: e1a00008     	mov	r0, r8
   1033c: ebffce82     	bl	0x3d4c <.plt+0x650>     @ imm = #-0xc5f8  // CALL class_addmethod
   10340: e59fc150     	ldr	r12, [pc, #0x150]       @ 0x10498 <arbhar_gpio_tilde_setup+0x55c>  // u32=0x5904; f32?=3.19327894e-41
   10344: e59481c0     	ldr	r8, [r4, #0x1c0]
   10348: e08f000c     	add	r0, pc, r12
   1034c: ebffccf5     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xcc2c  // CALL gensym
   10350: e59f1144     	ldr	r1, [pc, #0x144]        @ 0x1049c <arbhar_gpio_tilde_setup+0x560>  // u32=0x29c; f32?=9.36067374e-43
   10354: e1a03007     	mov	r3, r7
   10358: e7961001     	ldr	r1, [r6, r1]
   1035c: e58d5000     	str	r5, [sp]
   10360: e1a02000     	mov	r2, r0
   10364: e1a00008     	mov	r0, r8
   10368: ebffce77     	bl	0x3d4c <.plt+0x650>     @ imm = #-0xc624  // CALL class_addmethod
   1036c: e59f012c     	ldr	r0, [pc, #0x12c]        @ 0x104a0 <arbhar_gpio_tilde_setup+0x564>  // u32=0x58e8; f32?=3.1893553e-41
   10370: e59481c0     	ldr	r8, [r4, #0x1c0]
   10374: e08f0000     	add	r0, pc, r0
   10378: ebffccea     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xcc58  // CALL gensym
   1037c: e59f2120     	ldr	r2, [pc, #0x120]        @ 0x104a4 <arbhar_gpio_tilde_setup+0x568>  // u32=0x274; f32?=8.80015436e-43
   10380: e3a0300a     	mov	r3, #10
   10384: e7961002     	ldr	r1, [r6, r2]
   10388: e59f6118     	ldr	r6, [pc, #0x118]        @ 0x104a8 <arbhar_gpio_tilde_setup+0x56c>  // u32=0x58d0; f32?=3.18599219e-41
   1038c: e58d5000     	str	r5, [sp]
   10390: e1a02000     	mov	r2, r0
   10394: e1a00008     	mov	r0, r8
   10398: ebffce6b     	bl	0x3d4c <.plt+0x650>     @ imm = #-0xc654  // CALL class_addmethod
   1039c: e08f0006     	add	r0, pc, r6
   103a0: e59481c0     	ldr	r8, [r4, #0x1c0]
   103a4: ebffccdf     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xcc84  // CALL gensym
   103a8: e58d5000     	str	r5, [sp]
   103ac: e1a03007     	mov	r3, r7
   103b0: e59f50f4     	ldr	r5, [pc, #0xf4]         @ 0x104ac <arbhar_gpio_tilde_setup+0x570>  // u32=0xffff8274; f32?=nan
   103b4: e08f1005     	add	r1, pc, r5
   103b8: e1a02000     	mov	r2, r0
   103bc: e1a00008     	mov	r0, r8
   103c0: ebffce61     	bl	0x3d4c <.plt+0x650>     @ imm = #-0xc67c  // CALL class_addmethod
   103c4: e59401c0     	ldr	r0, [r4, #0x1c0]
   103c8: e3a0101c     	mov	r1, #28
   103cc: e28dd010     	add	sp, sp, #16
   103d0: e8bd41f0     	pop	{r4, r5, r6, r7, r8, lr}
   103d4: eaffce35     	b	0x3cb0 <.plt+0x5b4>     @ imm = #-0xc72c  // CALL class_domainsignalin
   103d8: c4 5b 00 00  	.word	0x00005bc4
   103dc: 9c 70 01 00  	.word	0x0001709c
   103e0: 98 02 00 00  	.word	0x00000298
   103e4: 80 02 00 00  	.word	0x00000280
   103e8: 1c 74 01 00  	.word	0x0001741c
   103ec: 7c 5b 00 00  	.word	0x00005b7c
   103f0: e0 02 00 00  	.word	0x000002e0
   103f4: 48 ce ff ff  	.word	0xffffce48
   103f8: 44 5b 00 00  	.word	0x00005b44
   103fc: 98 63 ff ff  	.word	0xffff6398
   10400: 28 5b 00 00  	.word	0x00005b28
   10404: a8 02 00 00  	.word	0x000002a8
   10408: 08 5b 00 00  	.word	0x00005b08
   1040c: 6c 02 00 00  	.word	0x0000026c
   10410: e8 5a 00 00  	.word	0x00005ae8
   10414: 7c 02 00 00  	.word	0x0000027c
   10418: c8 5a 00 00  	.word	0x00005ac8
   1041c: 94 02 00 00  	.word	0x00000294
   10420: a8 5a 00 00  	.word	0x00005aa8
   10424: 88 02 00 00  	.word	0x00000288
   10428: 88 5a 00 00  	.word	0x00005a88
   1042c: 64 42 ff ff  	.word	0xffff4264
   10430: 6c 5a 00 00  	.word	0x00005a6c
   10434: 18 42 ff ff  	.word	0xffff4218
   10438: 54 5a 00 00  	.word	0x00005a54
   1043c: 80 6c ff ff  	.word	0xffff6c80
   10440: 34 5a 00 00  	.word	0x00005a34
   10444: 74 41 ff ff  	.word	0xffff4174
   10448: 1c 5a 00 00  	.word	0x00005a1c
   1044c: 78 02 00 00  	.word	0x00000278
   10450: 00 5a 00 00  	.word	0x00005a00
   10454: 2c 42 ff ff  	.word	0xffff422c
   10458: dc 59 00 00  	.word	0x000059dc
   1045c: b8 02 00 00  	.word	0x000002b8
   10460: c0 59 00 00  	.word	0x000059c0
   10464: 74 3e ff ff  	.word	0xffff3e74
   10468: a0 59 00 00  	.word	0x000059a0
   1046c: 74 3e ff ff  	.word	0xffff3e74
   10470: 80 59 00 00  	.word	0x00005980
   10474: d8 02 00 00  	.word	0x000002d8
   10478: 60 59 00 00  	.word	0x00005960
   1047c: 08 44 ff ff  	.word	0xffff4408
   10480: 4c 59 00 00  	.word	0x0000594c
   10484: c0 02 00 00  	.word	0x000002c0
   10488: 34 59 00 00  	.word	0x00005934
   1048c: dc 02 00 00  	.word	0x000002dc
   10490: 20 59 00 00  	.word	0x00005920
   10494: 70 02 00 00  	.word	0x00000270
   10498: 04 59 00 00  	.word	0x00005904
   1049c: 9c 02 00 00  	.word	0x0000029c
   104a0: e8 58 00 00  	.word	0x000058e8
   104a4: 74 02 00 00  	.word	0x00000274
   104a8: d0 58 00 00  	.word	0x000058d0
   104ac: 74 82 ff ff  	.word	0xffff8274

