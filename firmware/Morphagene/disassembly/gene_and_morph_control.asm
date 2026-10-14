; Recursive static traversal, not complete control flow.
; See analysis/indirect_transfers.json for unresolved transfers.
080278cc  4049      ldr          r1, [pc, #256]  ; [0x080279d0] = 0x200220a4 (f32=1.10222282e-19)
080278ce  4f4c      ldr          r4, [pc, #316]  ; [0x08027a0c] = 0x2002215c (f32=1.1022466e-19)
080278d0  4f69      ldr          r7, [r1, #20]
080278d2  54f82260  ldr.w        r6, [r4, r2, lsl #2]
080278d6  02f1ff3b  add.w        r11, r2, #4294967295
080278da  c51b      subs         r5, r0, r7
080278dc  48bf      it           mi
080278de  3d1a      submi        r5, r7, r0
080278e0  54f82b20  ldr.w        r2, [r4, r11, lsl #2]
080278e4  4a4f      ldr          r7, [pc, #296]  ; [0x08027a10] = 0x200220c4 (f32=1.10222696e-19)
080278e6  202d      cmp          r5, #32
080278e8  a6eb0201  sub.w        r1, r6, r2
080278ec  03dc      bgt          #6  ; -> 0x080278f6
080278ee  3d68      ldr          r5, [r7]
080278f0  8d42      cmp          r5, r1
080278f2  03f01281  beq.w        #12836  ; -> 0x0802ab1a
080278f6  07ee901a  vmov         s15, r1
080278fa  dfed466a  vldr         s13, [pc, #280]  ; [0x08027a14] = 0x490ca000 (f32=576000)
080278fe  464c      ldr          r4, [pc, #280]  ; [0x08027a18] = 0x20021f90 (f32=1.10218715e-19)
08027900  334e      ldr          r6, [pc, #204]  ; [0x080279d0] = 0x200220a4 (f32=1.10222282e-19)
08027902  3960      str          r1, [r7]
08027904  b8eee70a  vcvt.f32.s32 s0, s15
08027908  40f2314b  movw         r11, #1073
0802790c  b4eee60a  vcmpe.f32    s0, s13
08027910  f1ee10fa  vmrs         APSR_nzcv, fpscr
08027914  1494      str          r4, [sp, #80]
08027916  7061      str          r0, [r6, #20]
08027918  84ed000a  vstr         s0, [r4]
0802791c  abeba007  sub.w        r7, r11, r0, asr #2
08027920  b0ee401a  vmov.f32     s2, s0
08027924  08dd      ble          #16  ; -> 0x08027938
08027926  f6ee000a  vmov.f32     s1, #5.000000e-01
0802792a  21ee201a  vmul.f32     s2, s2, s1
0802792e  b4eee61a  vcmpe.f32    s2, s13
08027932  f1ee10fa  vmrs         APSR_nzcv, fpscr
08027936  f8dc      bgt          #-16  ; -> 0x0802792a
08027938  3849      ldr          r1, [pc, #224]  ; [0x08027a1c] = 0x08044d10 (f32=3.98128916e-34)
0802793a  1d4a      ldr          r2, [pc, #116]  ; [0x080279b0] = 0x20021cb0 (f32=1.10209202e-19)
0802793c  149c      ldr          r4, [sp, #80]
0802793e  1568      ldr          r5, [r2]
08027940  01eb8700  add.w        r0, r1, r7, lsl #2
08027944  d0ed001a  vldr         s3, [r0]
08027948  21eea12a  vmul.f32     s4, s3, s3
0802794c  012d      cmp          r5, #1
0802794e  62ee212a  vmul.f32     s5, s4, s3
08027952  22ee817a  vmul.f32     s14, s5, s2
08027956  84ed007a  vstr         s14, [r4]
0802795a  03f00781  beq.w        #12814  ; -> 0x0802ab6c
0802795e  012b      cmp          r3, #1
08027960  03d1      bne          #6  ; -> 0x0802796a
08027962  2f49      ldr          r1, [pc, #188]  ; [0x08027a20] = 0x200220e8 (f32=1.10223161e-19)
08027964  4ff07e54  mov.w        r4, #1065353216
08027968  0c60      str          r4, [r1]
0802796a  124e      ldr          r6, [pc, #72]  ; [0x080279b4] = 0x20021c68 (f32=1.10208272e-19)
0802796c  dfed2d0a  vldr         s1, [pc, #180]  ; [0x08027a24] = 0x4207f5c3 (f32=33.9900017)
08027970  7768      ldr          r7, [r6, #4]
08027972  3269      ldr          r2, [r6, #16]
08027974  9fed101a  vldr         s2, [pc, #64]  ; [0x080279b8] = 0x398a26fe (f32=0.000263504626)
08027978  2b48      ldr          r0, [pc, #172]  ; [0x08027a28] = 0x200003e0 (f32=1.08433039e-19)
0802797a  2c49      ldr          r1, [pc, #176]  ; [0x08027a2c] = 0x20000358 (f32=1.08431281e-19)
0802797c  2c4d      ldr          r5, [pc, #176]  ; [0x08027a30] = 0x200004f0 (f32=1.08436554e-19)
0802797e  dfed2d1a  vldr         s3, [pc, #180]  ; [0x08027a34] = 0x3d480c74 (f32=0.0488400012)
08027982  0097      str          r7, [sp]
08027984  0fee907a  vmov         s31, r7
08027988  faeecafa  vcvt.f32.s32 s31, s31, #12
0802798c  02ee102a  vmov         s4, r2
08027990  2feea03a  vmul.f32     s6, s31, s1
08027994  f8eec22a  vcvt.f32.s32 s5, s4
08027998  4ee0      b            #156  ; -> 0x08027a38
08027a38  fdeec33a  vcvt.s32.f32 s7, s6
08027a3c  62ee817a  vmul.f32     s15, s5, s2
08027a40  13ee906a  vmov         r6, s7
08027a44  152e      cmp          r6, #21
08027a46  a8bf      it           ge
08027a48  1526      movge        r6, #21
08027a4a  b400      lsls         r4, r6, #2
08027a4c  2044      add          r0, r4
08027a4e  2144      add          r1, r4
08027a50  90ed005a  vldr         s10, [r0]
08027a54  55f82670  ldr.w        r7, [r5, r6, lsl #2]
08027a58  d1ed004a  vldr         s9, [r1]
08027a5c  c748      ldr          r0, [pc, #796]  ; [0x08027d7c] = 0x20021f48 (f32=1.10217784e-19)
08027a5e  c84d      ldr          r5, [pc, #800]  ; [0x08027d80] = 0x2002209c (f32=1.10222179e-19)
08027a60  0192      str          r2, [sp, #4]
08027a62  f4eee17a  vcmpe.f32    s15, s3
08027a66  f1ee10fa  vmrs         APSR_nzcv, fpscr
08027a6a  3297      str          r7, [sp, #200]
08027a6c  80ed005a  vstr         s10, [r0]
08027a70  c5ed004a  vstr         s9, [r5]
08027a74  00f1fd86  bmi.w        #3578  ; -> 0x08028872
08027a78  9fedc26a  vldr         s12, [pc, #776]  ; [0x08027d84] = 0x3f864064 (f32=1.04884005)
08027a7c  f4eec67a  vcmpe.f32    s15, s12
08027a80  f1ee10fa  vmrs         APSR_nzcv, fpscr
08027a84  b8bf      it           lt
08027a86  77eee17a  vsublt.f32   s15, s15, s3
08027a8a  01db      blt          #2  ; -> 0x08027a90
08027a8c  dfedbe7a  vldr         s15, [pc, #760]  ; [0x08027d88] = 0x3f7fffef (f32=0.999998987)
08027a90  be4a      ldr          r2, [pc, #760]  ; [0x08027d8c] = 0x2002208c (f32=1.10221972e-19)
08027a92  129e      ldr          r6, [sp, #72]
08027a94  1268      ldr          r2, [r2]
08027a96  002a      cmp          r2, #0
08027a98  40f04e86  bne.w        #3228  ; -> 0x08028738
08027a9c  c6ed047a  vstr         s15, [r6, #16]
08027aa0  f0ee678a  vmov.f32     s17, s15
08027aa4  ba4f      ldr          r7, [pc, #744]  ; [0x08027d90] = 0x20022140 (f32=1.10224298e-19)
08027aa6  3868      ldr          r0, [r7]
08027aa8  0028      cmp          r0, #0
08027aaa  43f34d80  ble.w        #12442  ; -> 0x0802ab48
08027aae  451e      subs         r5, r0, #1
08027ab0  3d60      str          r5, [r7]
08027ab2  002d      cmp          r5, #0
08027ab4  43f03281  bne.w        #12900  ; -> 0x0802ad1c
08027ab8  129e      ldr          r6, [sp, #72]
08027aba  c6ed047a  vstr         s15, [r6, #16]
08027abe  b549      ldr          r1, [pc, #724]  ; [0x08027d94] = 0x20021e14 (f32=1.10213803e-19)
08027ac0  0c68      ldr          r4, [r1]
08027ac2  012c      cmp          r4, #1
08027ac4  00f06286  beq.w        #3268  ; -> 0x0802878c
08027ac8  b348      ldr          r0, [pc, #716]  ; [0x08027d98] = 0x2002214c (f32=1.10224453e-19)
08027aca  b24e      ldr          r6, [pc, #712]  ; [0x08027d94] = 0x20021e14 (f32=1.10213803e-19)
08027acc  0025      movs         r5, #0
08027ace  0560      str          r5, [r0]
08027ad0  3560      str          r5, [r6]
08027ad2  b249      ldr          r1, [pc, #712]  ; [0x08027d9c] = 0x20021e64 (f32=1.10214837e-19)
08027ad4  b24f      ldr          r7, [pc, #712]  ; [0x08027da0] = 0x20000468 (f32=1.08434796e-19)
08027ad6  91ed049a  vldr         s18, [r1, #16]
08027ada  91ed05ba  vldr         s22, [r1, #20]
08027ade  b14c      ldr          r4, [pc, #708]  ; [0x08027da4] = 0x20022094 (f32=1.10222075e-19)
08027ae0  79ee0bba  vadd.f32     s23, s18, s22
08027ae4  b7ee00ca  vmov.f32     s24, #1.000000e+00
08027ae8  7bee886a  vadd.f32     s13, s23, s16
08027aec  b1ee088a  vmov.f32     s16, #6.000000e+00
08027af0  76eeccca  vsub.f32     s25, s13, s24
08027af4  94ed00da  vldr         s26, [r4]
08027af8  6cee88da  vmul.f32     s27, s25, s16
08027afc  bdeeedea  vcvt.s32.f32 s28, s27
08027b00  1eee100a  vmov         r0, s28
08027b04  0330      adds         r0, #3
08027b06  2128      cmp          r0, #33
08027b08  a8bf      it           ge
08027b0a  2120      movge        r0, #33
08027b0c  20eae075  bic.w        r5, r0, r0, asr #31
08027b10  07eb850b  add.w        r11, r7, r5, lsl #2
08027b14  012b      cmp          r3, #1
08027b16  9bed001a  vldr         s2, [r11]
08027b1a  00f0ef86  beq.w        #3550  ; -> 0x080288fc
