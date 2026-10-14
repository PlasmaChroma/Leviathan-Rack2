; Recursive static traversal, not complete control flow.
; See analysis/indirect_transfers.json for unresolved transfers.
08028b5a  049f      ldr          r7, [sp, #16]
08028b5c  089c      ldr          r4, [sp, #32]
08028b5e  481e      subs         r0, r1, #1
08028b60  01f10208  add.w        r8, r1, #2
08028b64  00ea0709  and.w        r9, r0, r7
08028b68  08ea0708  and.w        r8, r8, r7
08028b6c  2746      mov          r7, r4
08028b6e  34f91940  ldrsh.w      r4, [r4, r9, lsl #1]
08028b72  37f91870  ldrsh.w      r7, [r7, r8, lsl #1]
08028b76  05ee107a  vmov         s10, r7
08028b7a  0a9f      ldr          r7, [sp, #40]
08028b7c  0aee104a  vmov         s20, r4
08028b80  3c46      mov          r4, r7
08028b82  37f91970  ldrsh.w      r7, [r7, r9, lsl #1]
08028b86  34f91840  ldrsh.w      r4, [r4, r8, lsl #1]
08028b8a  06ee104a  vmov         s12, r4
08028b8e  089c      ldr          r4, [sp, #32]
08028b90  0fee107a  vmov         s30, r7
08028b94  019f      ldr          r7, [sp, #4]
08028b96  34f91100  ldrsh.w      r0, [r4, r1, lsl #1]
08028b9a  97ed001a  vldr         s2, [r7]
08028b9e  049f      ldr          r7, [sp, #16]
08028ba0  00ee900a  vmov         s1, r0
08028ba4  481c      adds         r0, r1, #1
08028ba6  00ea0709  and.w        r9, r0, r7
08028baa  f8eeca5a  vcvt.f32.s32 s11, s20
08028bae  34f91980  ldrsh.w      r8, [r4, r9, lsl #1]
08028bb2  0a9c      ldr          r4, [sp, #40]
08028bb4  0598      ldr          r0, [sp, #20]
08028bb6  34f91170  ldrsh.w      r7, [r4, r1, lsl #1]
08028bba  34f91940  ldrsh.w      r4, [r4, r9, lsl #1]
08028bbe  dff8cc91  ldr.w        r9, [pc, #460]  ; [0x08028d8c] = 0x20021e88 (f32=1.10215303e-19)
08028bc2  f8eec54a  vcvt.f32.s32 s9, s10
08028bc6  b8eee07a  vcvt.f32.s32 s14, s1
08028bca  34eea5aa  vadd.f32     s20, s9, s11
08028bce  05ee108a  vmov         s10, r8
08028bd2  f8eecf3a  vcvt.f32.s32 s7, s30
08028bd6  b8eec55a  vcvt.f32.s32 s10, s10
08028bda  7aee474a  vsub.f32     s9, s20, s14
08028bde  b8eec6fa  vcvt.f32.s32 s30, s12
08028be2  06ee907a  vmov         s13, r7
08028be6  35ee85aa  vadd.f32     s20, s11, s10
08028bea  3fee236a  vadd.f32     s12, s30, s7
08028bee  74eec54a  vsub.f32     s9, s9, s10
08028bf2  f8eee66a  vcvt.f32.s32 s13, s13
08028bf6  75ee655a  vsub.f32     s11, s10, s11
08028bfa  05ee104a  vmov         s10, r4
08028bfe  f6ee000a  vmov.f32     s1, #5.000000e-01
08028c02  21ee20fa  vmul.f32     s30, s2, s1
08028c06  aaee207a  vfma.f32     s14, s20, s1
08028c0a  09eb8208  add.w        r8, r9, r2, lsl #2
08028c0e  b8eec5aa  vcvt.f32.s32 s20, s10
08028c12  36ee666a  vsub.f32     s12, s12, s13
08028c16  e4ee8f5a  vfma.f32     s11, s9, s30
08028c1a  36ee4a6a  vsub.f32     s12, s12, s20
08028c1e  7aee634a  vsub.f32     s9, s20, s7
08028c22  73ee8a3a  vadd.f32     s7, s7, s20
08028c26  e6ee0f4a  vfma.f32     s9, s12, s30
08028c2a  e3eea06a  vfma.f32     s13, s7, s1
08028c2e  a5ee817a  vfma.f32     s14, s11, s2
08028c32  e4ee816a  vfma.f32     s13, s9, s2
08028c36  98ed001a  vldr         s2, [r8]
08028c3a  27ee277a  vmul.f32     s14, s14, s15
08028c3e  7beec10a  vsub.f32     s1, s23, s2
08028c42  66eea76a  vmul.f32     s13, s13, s15
08028c46  20ee876a  vmul.f32     s12, s1, s14
08028c4a  61ee075a  vmul.f32     s11, s2, s14
08028c4e  a1ee266a  vfma.f32     s12, s2, s13
08028c52  e0eea65a  vfma.f32     s11, s1, s13
08028c56  33ee063a  vadd.f32     s6, s6, s12
08028c5a  72eea52a  vadd.f32     s5, s5, s11
