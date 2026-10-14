; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
080358f4  70b5      push	{r4, r5, r6, lr}
080358f6  b6b0      sub	sp, #216
080358f8  0021      movs	r1, #0
080358fa  0446      mov	r4, r0
080358fc  bc22      movs	r2, #188
080358fe  07a8      add	r0, sp, #28
08035900  0691      str	r1, [sp, #24]
08035902  cde90211  strd	r1, r1, [sp, #8]
08035906  cde90411  strd	r1, r1, [sp, #16]
0803590a  00f0bafd  bl	#2932 ; -> 0x08036482 ; branch_target=0x08036482
0803590e  404b      ldr	r3, [pc, #256] ; [0x08035a10] = 0x40015000 / f32_bits_interpretation=2.020507812
08035910  2268      ldr	r2, [r4]
08035912  9a42      cmp	r2, r3
08035914  01d0      beq	#2 ; -> 0x0803591a ; branch_target=0x0803591a
08035916  36b0      add	sp, #216
08035918  70bd      pop	{r4, r5, r6, pc}
0803591a  4ff40053  mov.w	r3, #8192
0803591e  07a8      add	r0, sp, #28
08035920  0793      str	r3, [sp, #28]
08035922  eef7a1fc  bl	#-71358 ; -> 0x08024268 ; branch_target=0x08024268
08035926  0028      cmp	r0, #0
08035928  68d1      bne	#208 ; -> 0x080359fc ; branch_target=0x080359fc
0803592a  3a4b      ldr	r3, [pc, #232] ; [0x08035a14] = 0x58024400
0803592c  0520      movs	r0, #5
0803592e  02a9      add	r1, sp, #8
08035930  394e      ldr	r6, [pc, #228] ; [0x08035a18] = 0x20014b40
08035932  d3f8f020  ldr.w	r2, [r3, #240]
08035936  42f48012  orr	r2, r2, #1048576
0803593a  c3f8f020  str.w	r2, [r3, #240]
0803593e  d3f8f020  ldr.w	r2, [r3, #240]
08035942  02f48012  and	r2, r2, #1048576
08035946  0092      str	r2, [sp]
08035948  009a      ldr	r2, [sp]
0803594a  d3f8e020  ldr.w	r2, [r3, #224]
0803594e  42f02002  orr	r2, r2, #32
08035952  c3f8e020  str.w	r2, [r3, #224]
08035956  4ff47072  mov.w	r2, #960
0803595a  d3f8e030  ldr.w	r3, [r3, #224]
0803595e  0690      str	r0, [sp, #24]
08035960  03f02003  and	r3, r3, #32
08035964  2d48      ldr	r0, [pc, #180] ; [0x08035a1c] = 0x58021400
08035966  0193      str	r3, [sp, #4]
08035968  0223      movs	r3, #2
0803596a  019d      ldr	r5, [sp, #4]
0803596c  cde90223  strd	r2, r3, [sp, #8]
08035970  0022      movs	r2, #0
08035972  0223      movs	r3, #2
08035974  cde90423  strd	r2, r3, [sp, #16]
08035978  edf70efc  bl	#-75748 ; -> 0x08023198 ; branch_target=0x08023198
0803597c  2849      ldr	r1, [pc, #160] ; [0x08035a20] = 0x40020058 / f32_bits_interpretation=2.031270981
0803597e  5522      movs	r2, #85
08035980  0023      movs	r3, #0
08035982  3046      mov	r0, r6
08035984  c6e90012  strd	r1, r2, [r6]
08035988  0422      movs	r2, #4
0803598a  4ff48061  mov.w	r1, #1024
0803598e  7262      str	r2, [r6, #36]
08035990  0122      movs	r2, #1
08035992  3161      str	r1, [r6, #16]
08035994  b262      str	r2, [r6, #40]
08035996  c6e90233  strd	r3, r3, [r6, #8]
0803599a  c6e90533  strd	r3, r3, [r6, #20]
0803599e  c6e90733  strd	r3, r3, [r6, #28]
080359a2  c6e90b33  strd	r3, r3, [r6, #44]
080359a6  ebf731ff  bl	#-82334 ; -> 0x0802180c ; branch_target=0x0802180c
080359aa  68bb      cbnz	r0, #90 ; -> 0x08035a08 ; branch_target=0x08035a08
080359ac  1d4d      ldr	r5, [pc, #116] ; [0x08035a24] = 0x20014ac8
080359ae  5622      movs	r2, #86
080359b0  1d49      ldr	r1, [pc, #116] ; [0x08035a28] = 0x40020070 / f32_bits_interpretation=2.031276703
080359b2  0023      movs	r3, #0
080359b4  6a60      str	r2, [r5, #4]
080359b6  4ff48062  mov.w	r2, #1024
080359ba  2960      str	r1, [r5]
080359bc  4021      movs	r1, #64
080359be  2a61      str	r2, [r5, #16]
080359c0  0122      movs	r2, #1
080359c2  a960      str	r1, [r5, #8]
080359c4  0421      movs	r1, #4
080359c6  2846      mov	r0, r5
080359c8  e667      str	r6, [r4, #124]
080359ca  b463      str	r4, [r6, #56]
080359cc  eb60      str	r3, [r5, #12]
080359ce  6b61      str	r3, [r5, #20]
080359d0  ab61      str	r3, [r5, #24]
080359d2  c5e90733  strd	r3, r3, [r5, #28]
080359d6  c5e90912  strd	r1, r2, [r5, #36]
080359da  c5e90b33  strd	r3, r3, [r5, #44]
080359de  ebf715ff  bl	#-82390 ; -> 0x0802180c ; branch_target=0x0802180c
080359e2  70b9      cbnz	r0, #28 ; -> 0x08035a02 ; branch_target=0x08035a02
080359e4  0022      movs	r2, #0
080359e6  5520      movs	r0, #85
080359e8  a567      str	r5, [r4, #120]
080359ea  1146      mov	r1, r2
080359ec  ac63      str	r4, [r5, #56]
080359ee  ebf777fd  bl	#-83218 ; -> 0x080214e0 ; branch_target=0x080214e0
080359f2  5520      movs	r0, #85
080359f4  ebf7b0fd  bl	#-83104 ; -> 0x08021558 ; branch_target=0x08021558
080359f8  36b0      add	sp, #216
080359fa  70bd      pop	{r4, r5, r6, pc}
080359fc  fff780fb  bl	#-2304 ; -> 0x08035100 ; branch_target=0x08035100
08035a00  93e7      b	#-218 ; -> 0x0803592a ; branch_target=0x0803592a
08035a02  fff77dfb  bl	#-2310 ; -> 0x08035100 ; branch_target=0x08035100
08035a06  ede7      b	#-38 ; -> 0x080359e4 ; branch_target=0x080359e4
08035a08  fff77afb  bl	#-2316 ; -> 0x08035100 ; branch_target=0x08035100
08035a0c  cee7      b	#-100 ; -> 0x080359ac ; branch_target=0x080359ac
