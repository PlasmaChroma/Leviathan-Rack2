; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
0802bef8  70b5      push	{r4, r5, r6, lr}
0802befa  82b0      sub	sp, #8
0802befc  1546      mov	r5, r2
0802befe  0446      mov	r4, r0
0802bf00  0e46      mov	r6, r1
0802bf02  0363      str	r3, [r0, #48]
0802bf04  0022      movs	r2, #0
0802bf06  01a9      add	r1, sp, #4
0802bf08  6846      mov	r0, sp
0802bf0a  0095      str	r5, [sp]
0802bf0c  fdf794ff  bl	#-8408 ; -> 0x08029e38 ; branch_target=0x08029e38
0802bf10  18b1      cbz	r0, #6 ; -> 0x0802bf1a ; branch_target=0x0802bf1a
0802bf12  0023      movs	r3, #0
0802bf14  2360      str	r3, [r4]
0802bf16  02b0      add	sp, #8
0802bf18  70bd      pop	{r4, r5, r6, pc}
0802bf1a  dde90015  ldrd	r1, r5, [sp]
0802bf1e  2046      mov	r0, r4
0802bf20  2560      str	r5, [r4]
0802bf22  fef7cbfb  bl	#-6250 ; -> 0x0802a6bc ; branch_target=0x0802a6bc
0802bf26  f8b9      cbnz	r0, #62 ; -> 0x0802bf68 ; branch_target=0x0802bf68
0802bf28  94f92f30  ldrsb.w	r3, [r4, #47]
0802bf2c  002b      cmp	r3, #0
0802bf2e  0bdb      blt	#22 ; -> 0x0802bf48 ; branch_target=0x0802bf48
0802bf30  a379      ldrb	r3, [r4, #6]
0802bf32  db06      lsls	r3, r3, #27
0802bf34  1ad5      bpl	#52 ; -> 0x0802bf6c ; branch_target=0x0802bf6c
0802bf36  2978      ldrb	r1, [r5]
0802bf38  226a      ldr	r2, [r4, #32]
0802bf3a  0329      cmp	r1, #3
0802bf3c  538b      ldrh	r3, [r2, #26]
0802bf3e  02d1      bne	#4 ; -> 0x0802bf46 ; branch_target=0x0802bf46
0802bf40  928a      ldrh	r2, [r2, #20]
0802bf42  43ea0243  orr.w	r3, r3, r2, lsl #16
0802bf46  a360      str	r3, [r4, #8]
0802bf48  eb88      ldrh	r3, [r5, #6]
0802bf4a  2046      mov	r0, r4
0802bf4c  a380      strh	r3, [r4, #4]
0802bf4e  fef72dfb  bl	#-6566 ; -> 0x0802a5ac ; branch_target=0x0802a5ac
0802bf52  48b9      cbnz	r0, #18 ; -> 0x0802bf68 ; branch_target=0x0802bf68
0802bf54  a368      ldr	r3, [r4, #8]
0802bf56  73b9      cbnz	r3, #28 ; -> 0x0802bf76 ; branch_target=0x0802bf76
0802bf58  2361      str	r3, [r4, #16]
0802bf5a  3146      mov	r1, r6
0802bf5c  2046      mov	r0, r4
0802bf5e  02b0      add	sp, #8
0802bf60  bde87040  pop.w	{r4, r5, r6, lr}
0802bf64  fff7febe  b.w	#-516 ; -> 0x0802bd64 ; branch_target=0x0802bd64
0802bf68  0428      cmp	r0, #4
0802bf6a  d2d1      bne	#-92 ; -> 0x0802bf12 ; branch_target=0x0802bf12
0802bf6c  0520      movs	r0, #5
0802bf6e  0023      movs	r3, #0
0802bf70  2360      str	r3, [r4]
0802bf72  02b0      add	sp, #8
0802bf74  70bd      pop	{r4, r5, r6, pc}
0802bf76  0146      mov	r1, r0
0802bf78  2046      mov	r0, r4
0802bf7a  fdf7e5fc  bl	#-9782 ; -> 0x08029948 ; branch_target=0x08029948
0802bf7e  2061      str	r0, [r4, #16]
0802bf80  0028      cmp	r0, #0
0802bf82  ead1      bne	#-44 ; -> 0x0802bf5a ; branch_target=0x0802bf5a
0802bf84  1220      movs	r0, #18
0802bf86  c4e7      b	#-120 ; -> 0x0802bf12 ; branch_target=0x0802bf12
