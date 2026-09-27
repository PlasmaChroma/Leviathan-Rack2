; Recursive static traversal, not complete control flow.
; See analysis/indirect_transfers.json for unresolved transfers.
08028e00  95ed00da  vldr         s26, [r5]
08028e04  d4ed00ca  vldr         s25, [r4]
08028e08  5fed1dda  vldr         s27, [pc, #-116]  ; [0x08028d98] = 0x3f7fbe77 (f32=0.999000013)
08028e0c  129a      ldr          r2, [sp, #72]
08028e0e  109e      ldr          r6, [sp, #64]
08028e10  92ed001a  vldr         s2, [r2]
08028e14  2c9b      ldr          r3, [sp, #176]
08028e16  b349      ldr          r1, [pc, #716]  ; [0x080290e4] = 0x20021de4 (f32=1.10213183e-19)
08028e18  706a      ldr          r0, [r6, #36]
08028e1a  1b68      ldr          r3, [r3]
08028e1c  0c68      ldr          r4, [r1]
08028e1e  219f      ldr          r7, [sp, #132]
08028e20  b5ee40da  vcmp.f32     s26, #0
08028e24  f1ee10fa  vmrs         APSR_nzcv, fpscr
08028e28  6ceeadaa  vmul.f32     s21, s25, s27
08028e2c  1cbf      itt          ne
08028e2e  9fedaeda  vldrne       s26, [pc, #696]  ; [0x080290e8] = 0x3a83126f (f32=0.00100000005)
08028e32  7aee8daa  vaddne.f32   s21, s21, s26
08028e36  bfee000a  vmov.f32     s0, #-1.000000e+00
08028e3a  22eeaa8a  vmul.f32     s16, s5, s21
08028e3e  63ee2a9a  vmul.f32     s19, s6, s21
08028e42  b4eeeb8a  vcmpe.f32    s16, s23
08028e46  f1ee10fa  vmrs         APSR_nzcv, fpscr
08028e4a  f4eeeb9a  vcmpe.f32    s19, s23
08028e4e  88bf      it           hi
08028e50  b0ee6b8a  vmovhi.f32   s16, s23
08028e54  f1ee10fa  vmrs         APSR_nzcv, fpscr
08028e58  88bf      it           hi
08028e5a  f0ee6b9a  vmovhi.f32   s19, s23
08028e5e  b4eec08a  vcmpe.f32    s16, s0
08028e62  f1ee10fa  vmrs         APSR_nzcv, fpscr
08028e66  f4eec09a  vcmpe.f32    s19, s0
08028e6a  b8bf      it           lt
08028e6c  b0ee408a  vmovlt.f32   s16, s0
08028e70  f1ee10fa  vmrs         APSR_nzcv, fpscr
08028e74  b8bf      it           lt
08028e76  f0ee409a  vmovlt.f32   s19, s0
08028e7a  38ee424a  vsub.f32     s8, s16, s4
08028e7e  79eee13a  vsub.f32     s7, s19, s3
08028e82  f0ee426a  vmov.f32     s13, s4
08028e86  b0ee617a  vmov.f32     s14, s3
08028e8a  a1ee237a  vfma.f32     s14, s2, s7
08028e8e  0028      cmp          r0, #0
08028e90  e4ee016a  vfma.f32     s13, s8, s2
08028e94  08bf      it           eq
08028e96  f0ee471a  vmoveq.f32   s3, s14
08028e9a  08bf      it           eq
08028e9c  b0ee662a  vmoveq.f32   s4, s13
08028ea0  a342      cmp          r3, r4
08028ea2  c7ed00aa  vstr         s21, [r7]
08028ea6  b0ee47fa  vmov.f32     s30, s14
08028eaa  23d1      bne          #70  ; -> 0x08028ef4
08028eac  8f4d      ldr          r5, [pc, #572]  ; [0x080290ec] = 0x20024134 (f32=1.10330022e-19)
08028eae  904e      ldr          r6, [pc, #576]  ; [0x080290f0] = 0x2002412c (f32=1.10329919e-19)
08028eb0  9048      ldr          r0, [pc, #576]  ; [0x080290f4] = 0x20024138 (f32=1.10330074e-19)
08028eb2  914a      ldr          r2, [pc, #580]  ; [0x080290f8] = 0x20024130 (f32=1.1032997e-19)
08028eb4  d5ed000a  vldr         s1, [r5]
08028eb8  d6ed005a  vldr         s11, [r6]
08028ebc  dfed8f4a  vldr         s9, [pc, #572]  ; [0x080290fc] = 0x3f7f3b64 (f32=0.996999979)
08028ec0  90ed005a  vldr         s10, [r0]
08028ec4  92ed006a  vldr         s12, [r2]
08028ec8  9fed8dca  vldr         s24, [pc, #564]  ; [0x08029100] = 0x3f333333 (f32=0.699999988)
08028ecc  d0eea45a  vfnms.f32    s11, s1, s9
08028ed0  95ee246a  vfnms.f32    s12, s10, s9
08028ed4  22ee0cba  vmul.f32     s22, s4, s24
08028ed8  61ee8c7a  vmul.f32     s15, s3, s24
08028edc  35ee8b2a  vadd.f32     s4, s11, s22
08028ee0  76ee271a  vadd.f32     s3, s12, s15
08028ee4  86ed00ba  vstr         s22, [r6]
08028ee8  85ed002a  vstr         s4, [r5]
08028eec  c0ed001a  vstr         s3, [r0]
08028ef0  c2ed007a  vstr         s15, [r2]
