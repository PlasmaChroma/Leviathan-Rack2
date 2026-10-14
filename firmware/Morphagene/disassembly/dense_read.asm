; Recursive static traversal, not complete control flow.
; See analysis/indirect_transfers.json for unresolved transfers.
08029efe  059c      ldr          r4, [sp, #20]
08029f00  dff81494  ldr.w        r9, [pc, #1044]  ; [0x0802a318] = 0x20021e88 (f32=1.10215303e-19)
08029f04  4f1c      adds         r7, r1, #1
08029f06  07ea0408  and.w        r8, r7, r4
08029f0a  1b9c      ldr          r4, [sp, #108]
08029f0c  34f91170  ldrsh.w      r7, [r4, r1, lsl #1]
08029f10  34f91840  ldrsh.w      r4, [r4, r8, lsl #1]
08029f14  05ee904a  vmov         s11, r4
08029f18  1a9c      ldr          r4, [sp, #104]
08029f1a  02ee907a  vmov         s5, r7
08029f1e  34f91880  ldrsh.w      r8, [r4, r8, lsl #1]
08029f22  34f91140  ldrsh.w      r4, [r4, r1, lsl #1]
08029f26  009f      ldr          r7, [sp]
08029f28  b8eee27a  vcvt.f32.s32 s14, s5
08029f2c  02ee108a  vmov         s4, r8
08029f30  f8eee55a  vcvt.f32.s32 s11, s11
08029f34  02ee904a  vmov         s5, r4
08029f38  97ed001a  vldr         s2, [r7]
08029f3c  039f      ldr          r7, [sp, #12]
08029f3e  75eec75a  vsub.f32     s11, s11, s14
08029f42  f8eee22a  vcvt.f32.s32 s5, s5
08029f46  b8eec22a  vcvt.f32.s32 s4, s4
08029f4a  a5ee817a  vfma.f32     s14, s11, s2
08029f4e  09eb8208  add.w        r8, r9, r2, lsl #2
08029f52  32ee622a  vsub.f32     s4, s4, s5
08029f56  77eea75a  vadd.f32     s11, s15, s15
08029f5a  e2ee012a  vfma.f32     s5, s4, s2
08029f5e  98ed002a  vldr         s4, [r8]
08029f62  25ee877a  vmul.f32     s14, s11, s14
08029f66  36eec21a  vsub.f32     s2, s13, s4
08029f6a  65eea25a  vmul.f32     s11, s11, s5
08029f6e  61ee072a  vmul.f32     s5, s2, s14
08029f72  22ee077a  vmul.f32     s14, s4, s14
08029f76  e2ee252a  vfma.f32     s5, s4, s11
08029f7a  a1ee257a  vfma.f32     s14, s2, s11
08029f7e  74eea24a  vadd.f32     s9, s9, s5
08029f82  34ee074a  vadd.f32     s8, s8, s14
