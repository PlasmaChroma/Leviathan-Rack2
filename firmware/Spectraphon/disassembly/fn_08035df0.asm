; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08035df0  10b5      push	{r4, lr}
08035df2  5f4c      ldr	r4, [pc, #380] ; [0x08035f70] = 0x20014c40
08035df4  0023      movs	r3, #0
08035df6  94b0      sub	sp, #80
08035df8  8022      movs	r2, #128
08035dfa  5e49      ldr	r1, [pc, #376] ; [0x08035f74] = 0x40001800 / f32_bits_interpretation=2.001464844
08035dfc  2046      mov	r0, r4
08035dfe  0c93      str	r3, [sp, #48]
08035e00  2361      str	r3, [r4, #16]
08035e02  a261      str	r2, [r4, #24]
08035e04  c4e90013  strd	r1, r3, [r4]
08035e08  c4e90232  strd	r3, r2, [r4, #8]
08035e0c  cde90d33  strd	r3, r3, [sp, #52]
08035e10  cde90f33  strd	r3, r3, [sp, #60]
08035e14  cde91133  strd	r3, r3, [sp, #68]
08035e18  f1f73afe  bl	#-58252 ; -> 0x08027a90 ; branch_target=0x08027a90
08035e1c  30bb      cbnz	r0, #76 ; -> 0x08035e6c ; branch_target=0x08035e6c
08035e1e  0022      movs	r2, #0
08035e20  0ca9      add	r1, sp, #48
08035e22  5348      ldr	r0, [pc, #332] ; [0x08035f70] = 0x20014c40
08035e24  0e92      str	r2, [sp, #56]
08035e26  1092      str	r2, [sp, #64]
08035e28  9fed477b  vldr	d7, [pc, #284] ; [0x08035f48] = 0x00000060 / f64_bits_interpretation=4.7430302000759668e-322
08035e2c  8ded0c7b  vstr	d7, [sp, #48]
08035e30  f2f796f8  bl	#-57044 ; -> 0x08027f60 ; branch_target=0x08027f60
08035e34  b8b9      cbnz	r0, #46 ; -> 0x08035e66 ; branch_target=0x08035e66
08035e36  2268      ldr	r2, [r4]
08035e38  0023      movs	r3, #0
08035e3a  4f49      ldr	r1, [pc, #316] ; [0x08035f78] = 0x40010000 / f32_bits_interpretation=2.015625
08035e3c  0a93      str	r3, [sp, #40]
08035e3e  8a42      cmp	r2, r1
08035e40  cde90633  strd	r3, r3, [sp, #24]
08035e44  cde90833  strd	r3, r3, [sp, #32]
08035e48  13d0      beq	#38 ; -> 0x08035e72 ; branch_target=0x08035e72
08035e4a  b2f1804f  cmp.w	r2, #1073741824
08035e4e  36d0      beq	#108 ; -> 0x08035ebe ; branch_target=0x08035ebe
08035e50  4a4b      ldr	r3, [pc, #296] ; [0x08035f7c] = 0x40000400 / f32_bits_interpretation=2.000244141
08035e52  9a42      cmp	r2, r3
08035e54  1cd0      beq	#56 ; -> 0x08035e90 ; branch_target=0x08035e90
08035e56  4a4b      ldr	r3, [pc, #296] ; [0x08035f80] = 0x40000800 / f32_bits_interpretation=2.000488281
08035e58  9a42      cmp	r2, r3
08035e5a  49d0      beq	#146 ; -> 0x08035ef0 ; branch_target=0x08035ef0
08035e5c  454b      ldr	r3, [pc, #276] ; [0x08035f74] = 0x40001800 / f32_bits_interpretation=2.001464844
08035e5e  9a42      cmp	r2, r3
08035e60  5cd0      beq	#184 ; -> 0x08035f1c ; branch_target=0x08035f1c
08035e62  14b0      add	sp, #80
08035e64  10bd      pop	{r4, pc}
08035e66  fff74bf9  bl	#-3434 ; -> 0x08035100 ; branch_target=0x08035100
08035e6a  e4e7      b	#-56 ; -> 0x08035e36 ; branch_target=0x08035e36
08035e6c  fff748f9  bl	#-3440 ; -> 0x08035100 ; branch_target=0x08035100
08035e70  d5e7      b	#-86 ; -> 0x08035e1e ; branch_target=0x08035e1e
08035e72  444b      ldr	r3, [pc, #272] ; [0x08035f84] = 0x58024400
08035e74  d3f8e020  ldr.w	r2, [r3, #224]
08035e78  42f00102  orr	r2, r2, #1
08035e7c  c3f8e020  str.w	r2, [r3, #224]
08035e80  d3f8e030  ldr.w	r3, [r3, #224]
08035e84  03f00103  and	r3, r3, #1
08035e88  0193      str	r3, [sp, #4]
08035e8a  019b      ldr	r3, [sp, #4]
08035e8c  14b0      add	sp, #80
08035e8e  10bd      pop	{r4, pc}
08035e90  3c4b      ldr	r3, [pc, #240] ; [0x08035f84] = 0x58024400
08035e92  0224      movs	r4, #2
08035e94  06a9      add	r1, sp, #24
08035e96  3c48      ldr	r0, [pc, #240] ; [0x08035f88] = 0x58020400
08035e98  d3f8e020  ldr.w	r2, [r3, #224]
08035e9c  2243      orrs	r2, r4
08035e9e  c3f8e020  str.w	r2, [r3, #224]
08035ea2  d3f8e030  ldr.w	r3, [r3, #224]
08035ea6  0a94      str	r4, [sp, #40]
08035ea8  2340      ands	r3, r4
08035eaa  9fed297b  vldr	d7, [pc, #164] ; [0x08035f50] = 0x00000003 / f64_bits_interpretation=4.2439915834127416e-314
08035eae  0393      str	r3, [sp, #12]
08035eb0  039b      ldr	r3, [sp, #12]
08035eb2  8ded067b  vstr	d7, [sp, #24]
08035eb6  edf76ff9  bl	#-77090 ; -> 0x08023198 ; branch_target=0x08023198
08035eba  14b0      add	sp, #80
08035ebc  10bd      pop	{r4, pc}
08035ebe  314b      ldr	r3, [pc, #196] ; [0x08035f84] = 0x58024400
08035ec0  0124      movs	r4, #1
08035ec2  06a9      add	r1, sp, #24
08035ec4  3048      ldr	r0, [pc, #192] ; [0x08035f88] = 0x58020400
08035ec6  d3f8e020  ldr.w	r2, [r3, #224]
08035eca  42f00202  orr	r2, r2, #2
08035ece  c3f8e020  str.w	r2, [r3, #224]
08035ed2  d3f8e030  ldr.w	r3, [r3, #224]
08035ed6  0a94      str	r4, [sp, #40]
08035ed8  03f00203  and	r3, r3, #2
08035edc  9fed1e7b  vldr	d7, [pc, #120] ; [0x08035f58] = 0x00000c00 / f64_bits_interpretation=4.2439930997002087e-314
08035ee0  0293      str	r3, [sp, #8]
08035ee2  029b      ldr	r3, [sp, #8]
08035ee4  8ded067b  vstr	d7, [sp, #24]
08035ee8  edf756f9  bl	#-77140 ; -> 0x08023198 ; branch_target=0x08023198
08035eec  14b0      add	sp, #80
08035eee  10bd      pop	{r4, pc}
08035ef0  244b      ldr	r3, [pc, #144] ; [0x08035f84] = 0x58024400
08035ef2  0224      movs	r4, #2
08035ef4  06a9      add	r1, sp, #24
08035ef6  2448      ldr	r0, [pc, #144] ; [0x08035f88] = 0x58020400
08035ef8  d3f8e020  ldr.w	r2, [r3, #224]
08035efc  2243      orrs	r2, r4
08035efe  c3f8e020  str.w	r2, [r3, #224]
08035f02  d3f8e030  ldr.w	r3, [r3, #224]
08035f06  0a94      str	r4, [sp, #40]
08035f08  2340      ands	r3, r4
08035f0a  9fed157b  vldr	d7, [pc, #84] ; [0x08035f60] = 0x00000100 / f64_bits_interpretation=4.24399170841135e-314
08035f0e  0493      str	r3, [sp, #16]
08035f10  049b      ldr	r3, [sp, #16]
08035f12  8ded067b  vstr	d7, [sp, #24]
08035f16  edf73ff9  bl	#-77186 ; -> 0x08023198 ; branch_target=0x08023198
08035f1a  a2e7      b	#-188 ; -> 0x08035e62 ; branch_target=0x08035e62
08035f1c  194b      ldr	r3, [pc, #100] ; [0x08035f84] = 0x58024400
08035f1e  0224      movs	r4, #2
08035f20  06a9      add	r1, sp, #24
08035f22  1948      ldr	r0, [pc, #100] ; [0x08035f88] = 0x58020400
08035f24  d3f8e020  ldr.w	r2, [r3, #224]
08035f28  2243      orrs	r2, r4
08035f2a  c3f8e020  str.w	r2, [r3, #224]
08035f2e  d3f8e030  ldr.w	r3, [r3, #224]
08035f32  0a94      str	r4, [sp, #40]
08035f34  2340      ands	r3, r4
08035f36  9fed0c7b  vldr	d7, [pc, #48] ; [0x08035f68] = 0x00004000 / f64_bits_interpretation=4.2439996767020861e-314
08035f3a  0593      str	r3, [sp, #20]
08035f3c  059b      ldr	r3, [sp, #20]
08035f3e  8ded067b  vstr	d7, [sp, #24]
08035f42  edf729f9  bl	#-77230 ; -> 0x08023198 ; branch_target=0x08023198
08035f46  8ce7      b	#-232 ; -> 0x08035e62 ; branch_target=0x08035e62
