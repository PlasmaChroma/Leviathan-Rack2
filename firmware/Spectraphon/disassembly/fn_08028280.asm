; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08028280  90f83c30  ldrb.w	r3, [r0, #60]
08028284  012b      cmp	r3, #1
08028286  44d0      beq	#136 ; -> 0x08028312 ; branch_target=0x08028312
08028288  0246      mov	r2, r0
0802828a  0123      movs	r3, #1
0802828c  30b4      push	{r4, r5}
0802828e  0068      ldr	r0, [r0]
08028290  82f83c30  strb.w	r3, [r2, #60]
08028294  cb68      ldr	r3, [r1, #12]
08028296  8d68      ldr	r5, [r1, #8]
08028298  23f44073  bic	r3, r3, #768
0802829c  1e4c      ldr	r4, [pc, #120] ; [0x08028318] = 0x40010000 / f32_bits_interpretation=2.015625
0802829e  2b43      orrs	r3, r5
080282a0  4d68      ldr	r5, [r1, #4]
080282a2  a042      cmp	r0, r4
080282a4  23f48063  bic	r3, r3, #1024
080282a8  43ea0503  orr.w	r3, r3, r5
080282ac  0d68      ldr	r5, [r1]
080282ae  23f40063  bic	r3, r3, #2048
080282b2  43ea0503  orr.w	r3, r3, r5
080282b6  0d69      ldr	r5, [r1, #16]
080282b8  23f48053  bic	r3, r3, #4096
080282bc  43ea0503  orr.w	r3, r3, r5
080282c0  4d69      ldr	r5, [r1, #20]
080282c2  23f40053  bic	r3, r3, #8192
080282c6  43ea0503  orr.w	r3, r3, r5
080282ca  8d6a      ldr	r5, [r1, #40]
080282cc  23f48043  bic	r3, r3, #16384
080282d0  43ea0503  orr.w	r3, r3, r5
080282d4  8d69      ldr	r5, [r1, #24]
080282d6  23f47023  bic	r3, r3, #983040
080282da  43ea0543  orr.w	r3, r3, r5, lsl #16
080282de  0ad0      beq	#20 ; -> 0x080282f6 ; branch_target=0x080282f6
080282e0  04f58064  add.w	r4, r4, #1024
080282e4  a042      cmp	r0, r4
080282e6  06d0      beq	#12 ; -> 0x080282f6 ; branch_target=0x080282f6
080282e8  0021      movs	r1, #0
080282ea  4364      str	r3, [r0, #68]
080282ec  0846      mov	r0, r1
080282ee  82f83c10  strb.w	r1, [r2, #60]
080282f2  30bc      pop	{r4, r5}
080282f4  7047      bx	lr
080282f6  4c6a      ldr	r4, [r1, #36]
080282f8  23f47003  bic	r3, r3, #15728640
080282fc  43ea0453  orr.w	r3, r3, r4, lsl #20
08028300  d1e90741  ldrd	r4, r1, [r1, #28]
08028304  23f08073  bic	r3, r3, #16777216
08028308  2343      orrs	r3, r4
0802830a  23f00073  bic	r3, r3, #33554432
0802830e  0b43      orrs	r3, r1
08028310  eae7      b	#-44 ; -> 0x080282e8 ; branch_target=0x080282e8
08028312  0220      movs	r0, #2
08028314  7047      bx	lr
