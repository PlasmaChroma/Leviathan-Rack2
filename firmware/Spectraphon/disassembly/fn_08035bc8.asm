; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08035bc8  10b5      push	{r4, lr}
08035bca  6b4c      ldr	r4, [pc, #428] ; [0x08035d78] = 0x20014d74
08035bcc  0023      movs	r3, #0
08035bce  96b0      sub	sp, #88
08035bd0  8022      movs	r2, #128
08035bd2  4ff08041  mov.w	r1, #1073741824
08035bd6  2046      mov	r0, r4
08035bd8  0593      str	r3, [sp, #20]
08035bda  0e93      str	r3, [sp, #56]
08035bdc  2361      str	r3, [r4, #16]
08035bde  a261      str	r2, [r4, #24]
08035be0  c4e90013  strd	r1, r3, [r4]
08035be4  c4e90232  strd	r3, r2, [r4, #8]
08035be8  cde90633  strd	r3, r3, [sp, #24]
08035bec  cde90f33  strd	r3, r3, [sp, #60]
08035bf0  cde91133  strd	r3, r3, [sp, #68]
08035bf4  cde91333  strd	r3, r3, [sp, #76]
08035bf8  f1f74aff  bl	#-57708 ; -> 0x08027a90 ; branch_target=0x08027a90
08035bfc  0028      cmp	r0, #0
08035bfe  3bd1      bne	#118 ; -> 0x08035c78 ; branch_target=0x08035c78
08035c00  0023      movs	r3, #0
08035c02  05a9      add	r1, sp, #20
08035c04  5c48      ldr	r0, [pc, #368] ; [0x08035d78] = 0x20014d74
08035c06  0593      str	r3, [sp, #20]
08035c08  0793      str	r3, [sp, #28]
08035c0a  f2f7dffa  bl	#-55874 ; -> 0x080281cc ; branch_target=0x080281cc
08035c0e  80bb      cbnz	r0, #96 ; -> 0x08035c72 ; branch_target=0x08035c72
08035c10  6020      movs	r0, #96
08035c12  0021      movs	r1, #0
08035c14  0023      movs	r3, #0
08035c16  0822      movs	r2, #8
08035c18  cde90e01  strd	r0, r1, [sp, #56]
08035c1c  0ea9      add	r1, sp, #56
08035c1e  5648      ldr	r0, [pc, #344] ; [0x08035d78] = 0x20014d74
08035c20  1093      str	r3, [sp, #64]
08035c22  1293      str	r3, [sp, #72]
08035c24  f2f79cf9  bl	#-56520 ; -> 0x08027f60 ; branch_target=0x08027f60
08035c28  00bb      cbnz	r0, #64 ; -> 0x08035c6c ; branch_target=0x08035c6c
08035c2a  0c22      movs	r2, #12
08035c2c  0ea9      add	r1, sp, #56
08035c2e  5248      ldr	r0, [pc, #328] ; [0x08035d78] = 0x20014d74
08035c30  f2f796f9  bl	#-56532 ; -> 0x08027f60 ; branch_target=0x08027f60
08035c34  b8b9      cbnz	r0, #46 ; -> 0x08035c66 ; branch_target=0x08035c66
08035c36  2268      ldr	r2, [r4]
08035c38  0023      movs	r3, #0
08035c3a  5049      ldr	r1, [pc, #320] ; [0x08035d7c] = 0x40010000 / f32_bits_interpretation=2.015625
08035c3c  0c93      str	r3, [sp, #48]
08035c3e  8a42      cmp	r2, r1
08035c40  cde90833  strd	r3, r3, [sp, #32]
08035c44  cde90a33  strd	r3, r3, [sp, #40]
08035c48  19d0      beq	#50 ; -> 0x08035c7e ; branch_target=0x08035c7e
08035c4a  b2f1804f  cmp.w	r2, #1073741824
08035c4e  3cd0      beq	#120 ; -> 0x08035cca ; branch_target=0x08035cca
08035c50  4b4b      ldr	r3, [pc, #300] ; [0x08035d80] = 0x40000400 / f32_bits_interpretation=2.000244141
08035c52  9a42      cmp	r2, r3
08035c54  22d0      beq	#68 ; -> 0x08035c9c ; branch_target=0x08035c9c
08035c56  4b4b      ldr	r3, [pc, #300] ; [0x08035d84] = 0x40000800 / f32_bits_interpretation=2.000488281
08035c58  9a42      cmp	r2, r3
08035c5a  4fd0      beq	#158 ; -> 0x08035cfc ; branch_target=0x08035cfc
08035c5c  4a4b      ldr	r3, [pc, #296] ; [0x08035d88] = 0x40001800 / f32_bits_interpretation=2.001464844
08035c5e  9a42      cmp	r2, r3
08035c60  62d0      beq	#196 ; -> 0x08035d28 ; branch_target=0x08035d28
08035c62  16b0      add	sp, #88
08035c64  10bd      pop	{r4, pc}
08035c66  fff74bfa  bl	#-2922 ; -> 0x08035100 ; branch_target=0x08035100
08035c6a  e4e7      b	#-56 ; -> 0x08035c36 ; branch_target=0x08035c36
08035c6c  fff748fa  bl	#-2928 ; -> 0x08035100 ; branch_target=0x08035100
08035c70  dbe7      b	#-74 ; -> 0x08035c2a ; branch_target=0x08035c2a
08035c72  fff745fa  bl	#-2934 ; -> 0x08035100 ; branch_target=0x08035100
08035c76  cbe7      b	#-106 ; -> 0x08035c10 ; branch_target=0x08035c10
08035c78  fff742fa  bl	#-2940 ; -> 0x08035100 ; branch_target=0x08035100
08035c7c  c0e7      b	#-128 ; -> 0x08035c00 ; branch_target=0x08035c00
08035c7e  434b      ldr	r3, [pc, #268] ; [0x08035d8c] = 0x58024400
08035c80  d3f8e020  ldr.w	r2, [r3, #224]
08035c84  42f00102  orr	r2, r2, #1
08035c88  c3f8e020  str.w	r2, [r3, #224]
08035c8c  d3f8e030  ldr.w	r3, [r3, #224]
08035c90  03f00103  and	r3, r3, #1
08035c94  0093      str	r3, [sp]
08035c96  009b      ldr	r3, [sp]
08035c98  16b0      add	sp, #88
08035c9a  10bd      pop	{r4, pc}
08035c9c  3b4b      ldr	r3, [pc, #236] ; [0x08035d8c] = 0x58024400
08035c9e  0224      movs	r4, #2
08035ca0  08a9      add	r1, sp, #32
08035ca2  3b48      ldr	r0, [pc, #236] ; [0x08035d90] = 0x58020400
08035ca4  d3f8e020  ldr.w	r2, [r3, #224]
08035ca8  2243      orrs	r2, r4
08035caa  c3f8e020  str.w	r2, [r3, #224]
08035cae  d3f8e030  ldr.w	r3, [r3, #224]
08035cb2  0c94      str	r4, [sp, #48]
08035cb4  2340      ands	r3, r4
08035cb6  9fed287b  vldr	d7, [pc, #160] ; [0x08035d58] = 0x00000003 / f64_bits_interpretation=4.2439915834127416e-314
08035cba  0293      str	r3, [sp, #8]
08035cbc  029b      ldr	r3, [sp, #8]
08035cbe  8ded087b  vstr	d7, [sp, #32]
08035cc2  edf769fa  bl	#-76590 ; -> 0x08023198 ; branch_target=0x08023198
08035cc6  16b0      add	sp, #88
08035cc8  10bd      pop	{r4, pc}
08035cca  304b      ldr	r3, [pc, #192] ; [0x08035d8c] = 0x58024400
08035ccc  0124      movs	r4, #1
08035cce  08a9      add	r1, sp, #32
08035cd0  2f48      ldr	r0, [pc, #188] ; [0x08035d90] = 0x58020400
08035cd2  d3f8e020  ldr.w	r2, [r3, #224]
08035cd6  42f00202  orr	r2, r2, #2
08035cda  c3f8e020  str.w	r2, [r3, #224]
08035cde  d3f8e030  ldr.w	r3, [r3, #224]
08035ce2  0c94      str	r4, [sp, #48]
08035ce4  03f00203  and	r3, r3, #2
08035ce8  9fed1d7b  vldr	d7, [pc, #116] ; [0x08035d60] = 0x00000c00 / f64_bits_interpretation=4.2439930997002087e-314
08035cec  0193      str	r3, [sp, #4]
08035cee  019b      ldr	r3, [sp, #4]
08035cf0  8ded087b  vstr	d7, [sp, #32]
08035cf4  edf750fa  bl	#-76640 ; -> 0x08023198 ; branch_target=0x08023198
08035cf8  16b0      add	sp, #88
08035cfa  10bd      pop	{r4, pc}
08035cfc  234b      ldr	r3, [pc, #140] ; [0x08035d8c] = 0x58024400
08035cfe  0224      movs	r4, #2
08035d00  08a9      add	r1, sp, #32
08035d02  2348      ldr	r0, [pc, #140] ; [0x08035d90] = 0x58020400
08035d04  d3f8e020  ldr.w	r2, [r3, #224]
08035d08  2243      orrs	r2, r4
08035d0a  c3f8e020  str.w	r2, [r3, #224]
08035d0e  d3f8e030  ldr.w	r3, [r3, #224]
08035d12  0c94      str	r4, [sp, #48]
08035d14  2340      ands	r3, r4
08035d16  9fed147b  vldr	d7, [pc, #80] ; [0x08035d68] = 0x00000100 / f64_bits_interpretation=4.24399170841135e-314
08035d1a  0393      str	r3, [sp, #12]
08035d1c  039b      ldr	r3, [sp, #12]
08035d1e  8ded087b  vstr	d7, [sp, #32]
08035d22  edf739fa  bl	#-76686 ; -> 0x08023198 ; branch_target=0x08023198
08035d26  9ce7      b	#-200 ; -> 0x08035c62 ; branch_target=0x08035c62
08035d28  184b      ldr	r3, [pc, #96] ; [0x08035d8c] = 0x58024400
08035d2a  0224      movs	r4, #2
08035d2c  08a9      add	r1, sp, #32
08035d2e  1848      ldr	r0, [pc, #96] ; [0x08035d90] = 0x58020400
08035d30  d3f8e020  ldr.w	r2, [r3, #224]
08035d34  2243      orrs	r2, r4
08035d36  c3f8e020  str.w	r2, [r3, #224]
08035d3a  d3f8e030  ldr.w	r3, [r3, #224]
08035d3e  0c94      str	r4, [sp, #48]
08035d40  2340      ands	r3, r4
08035d42  9fed0b7b  vldr	d7, [pc, #44] ; [0x08035d70] = 0x00004000 / f64_bits_interpretation=4.2439996767020861e-314
08035d46  0493      str	r3, [sp, #16]
08035d48  049b      ldr	r3, [sp, #16]
08035d4a  8ded087b  vstr	d7, [sp, #32]
08035d4e  edf723fa  bl	#-76730 ; -> 0x08023198 ; branch_target=0x08023198
08035d52  86e7      b	#-244 ; -> 0x08035c62 ; branch_target=0x08035c62
