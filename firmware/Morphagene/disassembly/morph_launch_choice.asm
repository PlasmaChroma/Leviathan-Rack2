; Recursive static traversal, not complete control flow.
; See analysis/indirect_transfers.json for unresolved transfers.
0802842a  0998      ldr          r0, [sp, #36]
0802842c  984c      ldr          r4, [pc, #608]  ; [0x08028690] = 0x0bb38435 (f32=6.9147215e-32)
0802842e  994f      ldr          r7, [pc, #612]  ; [0x08028694] = 0x3619636b (f32=2.28566455e-06)
08028430  04fb0078  mla          r8, r4, r0, r7
08028434  0aee108a  vmov         s20, r8
08028438  f8ee4a4a  vcvt.f32.u32 s9, s20
0802843c  9548      ldr          r0, [pc, #596]  ; [0x08028694] = 0x3619636b (f32=2.28566455e-06)
0802843e  64eea13a  vmul.f32     s7, s9, s3
08028442  04fb0807  mla          r7, r4, r8, r0
08028446  01ee107a  vmov         s2, r7
0802844a  f4eee39a  vcmpe.f32    s19, s7
0802844e  f1ee10fa  vmrs         APSR_nzcv, fpscr
08028452  0997      str          r7, [sp, #36]
08028454  f8ee410a  vcvt.f32.u32 s1, s2
08028458  c0f25784  blt.w        #2222  ; -> 0x08028d0a
0802845c  04fb0704  mla          r4, r4, r7, r0
08028460  dded2a6a  vldr         s13, [sp, #168]
08028464  0994      str          r4, [sp, #36]
08028466  05ee904a  vmov         s11, r4
0802846a  26eea07a  vmul.f32     s14, s13, s1
0802846e  f8ee650a  vcvt.f32.u32 s1, s11
08028472  6aeea07a  vmul.f32     s15, s21, s1
08028476  8848      ldr          r0, [pc, #544]  ; [0x08028698] = 0x20021e88 (f32=1.10215303e-19)
08028478  1c9c      ldr          r4, [sp, #112]
0802847a  bdeee7fa  vcvt.s32.f32 s30, s15
0802847e  0144      add          r1, r0
08028480  1fee107a  vmov         r7, s30
08028484  0398      ldr          r0, [sp, #12]
08028486  81ed007a  vstr         s14, [r1]
0802848a  04fb07f7  mul          r7, r4, r7
0802848e  0499      ldr          r1, [sp, #16]
08028490  4760      str          r7, [r0, #4]
