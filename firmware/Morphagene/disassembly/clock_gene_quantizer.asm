; Recursive static traversal, not complete control flow.
; See analysis/indirect_transfers.json for unresolved transfers.
0802ab6c  b2ee003a  vmov.f32     s6, #8.000000e+00
0802ab70  b4eec37a  vcmpe.f32    s14, s6
0802ab74  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802ab78  7cf7f1ae  ble.w        #-12830  ; -> 0x0802795e
0802ab7c  dfed503a  vldr         s7, [pc, #320]  ; [0x0802acc0] = 0x3f2aaa9f (f32=0.666665971)
0802ab80  20ee236a  vmul.f32     s12, s0, s7
0802ab84  b4eec67a  vcmpe.f32    s14, s12
0802ab88  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802ab8c  58bf      it           pl
0802ab8e  b0ee406a  vmovpl.f32   s12, s0
0802ab92  40f19d80  bpl.w        #314  ; -> 0x0802acd0
0802ab96  b0ee634a  vmov.f32     s8, s7
0802ab9a  f6ee085a  vmov.f32     s11, #7.500000e-01
0802ab9e  f4ee653a  vcmp.f32     s7, s11
0802aba2  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802aba6  66ee044a  vmul.f32     s9, s12, s8
0802abaa  26ee255a  vmul.f32     s10, s12, s11
0802abae  0ad1      bne          #20  ; -> 0x0802abc6
0802abb0  b4eee47a  vcmpe.f32    s14, s9
0802abb4  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802abb8  40f18a80  bpl.w        #276  ; -> 0x0802acd0
0802abbc  b0ee646a  vmov.f32     s12, s9
0802abc0  f0ee443a  vmov.f32     s7, s8
0802abc4  ebe7      b            #-42  ; -> 0x0802ab9e
0802abc6  b4eec57a  vcmpe.f32    s14, s10
0802abca  f1ee10fa  vmrs         APSR_nzcv, fpscr
0802abce  7fd5      bpl          #254  ; -> 0x0802acd0
0802abd0  b0ee456a  vmov.f32     s12, s10
0802abd4  f6ee083a  vmov.f32     s7, #7.500000e-01
0802abd8  e1e7      b            #-62  ; -> 0x0802ab9e
