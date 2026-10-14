; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08036068  10b5      push	{r4, lr}
0803606a  4b49      ldr	r1, [pc, #300] ; [0x08036198] = 0x40010000 / f32_bits_interpretation=2.015625
0803606c  8cb0      sub	sp, #48
0803606e  0268      ldr	r2, [r0]
08036070  0023      movs	r3, #0
08036072  8a42      cmp	r2, r1
08036074  0a93      str	r3, [sp, #40]
08036076  cde90633  strd	r3, r3, [sp, #24]
0803607a  cde90833  strd	r3, r3, [sp, #32]
0803607e  0dd0      beq	#26 ; -> 0x0803609c ; branch_target=0x0803609c
08036080  b2f1804f  cmp.w	r2, #1073741824
08036084  30d0      beq	#96 ; -> 0x080360e8 ; branch_target=0x080360e8
08036086  454b      ldr	r3, [pc, #276] ; [0x0803619c] = 0x40000400 / f32_bits_interpretation=2.000244141
08036088  9a42      cmp	r2, r3
0803608a  16d0      beq	#44 ; -> 0x080360ba ; branch_target=0x080360ba
0803608c  444b      ldr	r3, [pc, #272] ; [0x080361a0] = 0x40000800 / f32_bits_interpretation=2.000488281
0803608e  9a42      cmp	r2, r3
08036090  43d0      beq	#134 ; -> 0x0803611a ; branch_target=0x0803611a
08036092  444b      ldr	r3, [pc, #272] ; [0x080361a4] = 0x40001800 / f32_bits_interpretation=2.001464844
08036094  9a42      cmp	r2, r3
08036096  56d0      beq	#172 ; -> 0x08036146 ; branch_target=0x08036146
08036098  0cb0      add	sp, #48
0803609a  10bd      pop	{r4, pc}
0803609c  424b      ldr	r3, [pc, #264] ; [0x080361a8] = 0x58024400
0803609e  d3f8e020  ldr.w	r2, [r3, #224]
080360a2  42f00102  orr	r2, r2, #1
080360a6  c3f8e020  str.w	r2, [r3, #224]
080360aa  d3f8e030  ldr.w	r3, [r3, #224]
080360ae  03f00103  and	r3, r3, #1
080360b2  0193      str	r3, [sp, #4]
080360b4  019b      ldr	r3, [sp, #4]
080360b6  0cb0      add	sp, #48
080360b8  10bd      pop	{r4, pc}
080360ba  3b4b      ldr	r3, [pc, #236] ; [0x080361a8] = 0x58024400
080360bc  0224      movs	r4, #2
080360be  06a9      add	r1, sp, #24
080360c0  3a48      ldr	r0, [pc, #232] ; [0x080361ac] = 0x58020400
080360c2  d3f8e020  ldr.w	r2, [r3, #224]
080360c6  2243      orrs	r2, r4
080360c8  c3f8e020  str.w	r2, [r3, #224]
080360cc  d3f8e030  ldr.w	r3, [r3, #224]
080360d0  0a94      str	r4, [sp, #40]
080360d2  2340      ands	r3, r4
080360d4  9fed287b  vldr	d7, [pc, #160] ; [0x08036178] = 0x00000003 / f64_bits_interpretation=4.2439915834127416e-314
080360d8  0393      str	r3, [sp, #12]
080360da  039b      ldr	r3, [sp, #12]
080360dc  8ded067b  vstr	d7, [sp, #24]
080360e0  edf75af8  bl	#-77644 ; -> 0x08023198 ; branch_target=0x08023198
080360e4  0cb0      add	sp, #48
080360e6  10bd      pop	{r4, pc}
080360e8  2f4b      ldr	r3, [pc, #188] ; [0x080361a8] = 0x58024400
080360ea  0124      movs	r4, #1
080360ec  06a9      add	r1, sp, #24
080360ee  2f48      ldr	r0, [pc, #188] ; [0x080361ac] = 0x58020400
080360f0  d3f8e020  ldr.w	r2, [r3, #224]
080360f4  42f00202  orr	r2, r2, #2
080360f8  c3f8e020  str.w	r2, [r3, #224]
080360fc  d3f8e030  ldr.w	r3, [r3, #224]
08036100  0a94      str	r4, [sp, #40]
08036102  03f00203  and	r3, r3, #2
08036106  9fed1e7b  vldr	d7, [pc, #120] ; [0x08036180] = 0x00000c00 / f64_bits_interpretation=4.2439930997002087e-314
0803610a  0293      str	r3, [sp, #8]
0803610c  029b      ldr	r3, [sp, #8]
0803610e  8ded067b  vstr	d7, [sp, #24]
08036112  edf741f8  bl	#-77694 ; -> 0x08023198 ; branch_target=0x08023198
08036116  0cb0      add	sp, #48
08036118  10bd      pop	{r4, pc}
0803611a  234b      ldr	r3, [pc, #140] ; [0x080361a8] = 0x58024400
0803611c  0224      movs	r4, #2
0803611e  06a9      add	r1, sp, #24
08036120  2248      ldr	r0, [pc, #136] ; [0x080361ac] = 0x58020400
08036122  d3f8e020  ldr.w	r2, [r3, #224]
08036126  2243      orrs	r2, r4
08036128  c3f8e020  str.w	r2, [r3, #224]
0803612c  d3f8e030  ldr.w	r3, [r3, #224]
08036130  0a94      str	r4, [sp, #40]
08036132  2340      ands	r3, r4
08036134  9fed147b  vldr	d7, [pc, #80] ; [0x08036188] = 0x00000100 / f64_bits_interpretation=4.24399170841135e-314
08036138  0493      str	r3, [sp, #16]
0803613a  049b      ldr	r3, [sp, #16]
0803613c  8ded067b  vstr	d7, [sp, #24]
08036140  edf72af8  bl	#-77740 ; -> 0x08023198 ; branch_target=0x08023198
08036144  a8e7      b	#-176 ; -> 0x08036098 ; branch_target=0x08036098
08036146  184b      ldr	r3, [pc, #96] ; [0x080361a8] = 0x58024400
08036148  0224      movs	r4, #2
0803614a  06a9      add	r1, sp, #24
0803614c  1748      ldr	r0, [pc, #92] ; [0x080361ac] = 0x58020400
0803614e  d3f8e020  ldr.w	r2, [r3, #224]
08036152  2243      orrs	r2, r4
08036154  c3f8e020  str.w	r2, [r3, #224]
08036158  d3f8e030  ldr.w	r3, [r3, #224]
0803615c  0a94      str	r4, [sp, #40]
0803615e  2340      ands	r3, r4
08036160  9fed0b7b  vldr	d7, [pc, #44] ; [0x08036190] = 0x00004000 / f64_bits_interpretation=4.2439996767020861e-314
08036164  0593      str	r3, [sp, #20]
08036166  059b      ldr	r3, [sp, #20]
08036168  8ded067b  vstr	d7, [sp, #24]
0803616c  edf714f8  bl	#-77784 ; -> 0x08023198 ; branch_target=0x08023198
08036170  92e7      b	#-220 ; -> 0x08036098 ; branch_target=0x08036098
