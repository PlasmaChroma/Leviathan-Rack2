; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08026ea8  2de9f047  push.w	{r4, r5, r6, r7, r8, r9, r10, lr}
08026eac  1d46      mov	r5, r3
08026eae  90f88030  ldrb.w	r3, [r0, #128]
08026eb2  012b      cmp	r3, #1
08026eb4  00f0d880  beq.w	#432 ; -> 0x08027068 ; branch_target=0x08027068
08026eb8  0123      movs	r3, #1
08026eba  0446      mov	r4, r0
08026ebc  8a46      mov	r10, r1
08026ebe  9046      mov	r8, r2
08026ec0  80f88030  strb.w	r3, [r0, #128]
08026ec4  d0f80090  ldr.w	r9, [r0]
08026ec8  f9f768fa  bl	#-27440 ; -> 0x0802039c ; branch_target=0x0802039c
08026ecc  94f88130  ldrb.w	r3, [r4, #129]
08026ed0  0646      mov	r6, r0
08026ed2  012b      cmp	r3, #1
08026ed4  dfb2      uxtb	r7, r3
08026ed6  40f0c480  bne.w	#392 ; -> 0x08027062 ; branch_target=0x08027062
08026eda  baf1000f  cmp.w	r10, #0
08026ede  00f0e680  beq.w	#460 ; -> 0x080270ae ; branch_target=0x080270ae
08026ee2  b8fa88f3  clz	r3, r8
08026ee6  5b09      lsrs	r3, r3, #5
08026ee8  b8f1000f  cmp.w	r8, #0
08026eec  00f0df80  beq.w	#446 ; -> 0x080270ae ; branch_target=0x080270ae
08026ef0  0322      movs	r2, #3
08026ef2  6366      str	r3, [r4, #100]
08026ef4  a4f86830  strh.w	r3, [r4, #104]
08026ef8  84f88120  strb.w	r2, [r4, #129]
08026efc  c4f88430  str.w	r3, [r4, #132]
08026f00  a4f86280  strh.w	r8, [r4, #98]
08026f04  a4f86a30  strh.w	r3, [r4, #106]
08026f08  c4f85ca0  str.w	r10, [r4, #92]
08026f0c  a4f86080  strh.w	r8, [r4, #96]
08026f10  c4e91c33  strd	r3, r3, [r4, #112]
08026f14  a368      ldr	r3, [r4, #8]
08026f16  b3f5c02f  cmp.w	r3, #393216
08026f1a  04d1      bne	#8 ; -> 0x08026f26 ; branch_target=0x08026f26
08026f1c  2268      ldr	r2, [r4]
08026f1e  1368      ldr	r3, [r2]
08026f20  43f40063  orr	r3, r3, #2048
08026f24  1360      str	r3, [r2]
08026f26  2268      ldr	r2, [r4]
08026f28  824b      ldr	r3, [pc, #520] ; [0x08027134] = 0xffff0000
08026f2a  5168      ldr	r1, [r2, #4]
08026f2c  0b40      ands	r3, r1
08026f2e  43ea0803  orr.w	r3, r3, r8
08026f32  5360      str	r3, [r2, #4]
08026f34  2268      ldr	r2, [r4]
08026f36  1368      ldr	r3, [r2]
08026f38  43f00103  orr	r3, r3, #1
08026f3c  1360      str	r3, [r2]
08026f3e  6368      ldr	r3, [r4, #4]
08026f40  b3f5800f  cmp.w	r3, #4194304
08026f44  04d1      bne	#8 ; -> 0x08026f50 ; branch_target=0x08026f50
08026f46  2268      ldr	r2, [r4]
08026f48  1368      ldr	r3, [r2]
08026f4a  43f40073  orr	r3, r3, #512
08026f4e  1360      str	r3, [r2]
08026f50  e368      ldr	r3, [r4, #12]
08026f52  0f2b      cmp	r3, #15
08026f54  46d8      bhi	#140 ; -> 0x08026fe4 ; branch_target=0x08026fe4
08026f56  072b      cmp	r3, #7
08026f58  b4f86230  ldrh.w	r3, [r4, #98]
08026f5c  9bb2      uxth	r3, r3
08026f5e  40f28780  bls.w	#270 ; -> 0x08027070 ; branch_target=0x08027070
08026f62  dbb9      cbnz	r3, #54 ; -> 0x08026f9c ; branch_target=0x08026f9c
08026f64  58e0      b	#176 ; -> 0x08027018 ; branch_target=0x08027018
08026f66  b4f86230  ldrh.w	r3, [r4, #98]
08026f6a  e16d      ldr	r1, [r4, #92]
08026f6c  9bb2      uxth	r3, r3
08026f6e  012b      cmp	r3, #1
08026f70  40f2b780  bls.w	#366 ; -> 0x080270e2 ; branch_target=0x080270e2
08026f74  e36b      ldr	r3, [r4, #60]
08026f76  002b      cmp	r3, #0
08026f78  00f0b380  beq.w	#358 ; -> 0x080270e2 ; branch_target=0x080270e2
08026f7c  0b68      ldr	r3, [r1]
08026f7e  1362      str	r3, [r2, #32]
08026f80  e36d      ldr	r3, [r4, #92]
08026f82  0433      adds	r3, #4
08026f84  e365      str	r3, [r4, #92]
08026f86  b4f86230  ldrh.w	r3, [r4, #98]
08026f8a  023b      subs	r3, #2
08026f8c  9bb2      uxth	r3, r3
08026f8e  a4f86230  strh.w	r3, [r4, #98]
08026f92  b4f86230  ldrh.w	r3, [r4, #98]
08026f96  9bb2      uxth	r3, r3
08026f98  002b      cmp	r3, #0
08026f9a  3dd0      beq	#122 ; -> 0x08027018 ; branch_target=0x08027018
08026f9c  2268      ldr	r2, [r4]
08026f9e  5369      ldr	r3, [r2, #20]
08026fa0  13f00208  ands	r8, r3, #2
08026fa4  dfd1      bne	#-66 ; -> 0x08026f66 ; branch_target=0x08026f66
08026fa6  f9f7f9f9  bl	#-27662 ; -> 0x0802039c ; branch_target=0x0802039c
08026faa  801b      subs	r0, r0, r6
08026fac  a842      cmp	r0, r5
08026fae  f0d3      blo	#-32 ; -> 0x08026f92 ; branch_target=0x08026f92
08026fb0  6b1c      adds	r3, r5, #1
08026fb2  eed0      beq	#-36 ; -> 0x08026f92 ; branch_target=0x08026f92
08026fb4  2046      mov	r0, r4
08026fb6  fff765fe  bl	#-822 ; -> 0x08026c84 ; branch_target=0x08026c84
08026fba  d4f88430  ldr.w	r3, [r4, #132]
08026fbe  0122      movs	r2, #1
08026fc0  3846      mov	r0, r7
08026fc2  43f48073  orr	r3, r3, #256
08026fc6  84f88080  strb.w	r8, [r4, #128]
08026fca  c4f88430  str.w	r3, [r4, #132]
08026fce  84f88120  strb.w	r2, [r4, #129]
08026fd2  bde8f087  pop.w	{r4, r5, r6, r7, r8, r9, r10, pc}
08026fd6  f9f7e1f9  bl	#-27710 ; -> 0x0802039c ; branch_target=0x0802039c
08026fda  801b      subs	r0, r0, r6
08026fdc  a842      cmp	r0, r5
08026fde  01d3      blo	#2 ; -> 0x08026fe4 ; branch_target=0x08026fe4
08026fe0  6a1c      adds	r2, r5, #1
08026fe2  e7d1      bne	#-50 ; -> 0x08026fb4 ; branch_target=0x08026fb4
08026fe4  b4f86230  ldrh.w	r3, [r4, #98]
08026fe8  9bb2      uxth	r3, r3
08026fea  abb1      cbz	r3, #42 ; -> 0x08027018 ; branch_target=0x08027018
08026fec  2368      ldr	r3, [r4]
08026fee  5a69      ldr	r2, [r3, #20]
08026ff0  12f00208  ands	r8, r2, #2
08026ff4  efd0      beq	#-34 ; -> 0x08026fd6 ; branch_target=0x08026fd6
08026ff6  e26d      ldr	r2, [r4, #92]
08026ff8  1268      ldr	r2, [r2]
08026ffa  1a62      str	r2, [r3, #32]
08026ffc  b4f86220  ldrh.w	r2, [r4, #98]
08027000  e36d      ldr	r3, [r4, #92]
08027002  013a      subs	r2, #1
08027004  0433      adds	r3, #4
08027006  92b2      uxth	r2, r2
08027008  e365      str	r3, [r4, #92]
0802700a  a4f86220  strh.w	r2, [r4, #98]
0802700e  b4f86230  ldrh.w	r3, [r4, #98]
08027012  9bb2      uxth	r3, r3
08027014  002b      cmp	r3, #0
08027016  e9d1      bne	#-46 ; -> 0x08026fec ; branch_target=0x08026fec
08027018  002d      cmp	r5, #0
0802701a  00f08480  beq.w	#264 ; -> 0x08027126 ; branch_target=0x08027126
0802701e  2368      ldr	r3, [r4]
08027020  5b69      ldr	r3, [r3, #20]
08027022  1907      lsls	r1, r3, #28
08027024  0cd4      bmi	#24 ; -> 0x08027040 ; branch_target=0x08027040
08027026  f9f7b9f9  bl	#-27790 ; -> 0x0802039c ; branch_target=0x0802039c
0802702a  801b      subs	r0, r0, r6
0802702c  a842      cmp	r0, r5
0802702e  f6d3      blo	#-20 ; -> 0x0802701e ; branch_target=0x0802701e
08027030  6a1c      adds	r2, r5, #1
08027032  f4d0      beq	#-24 ; -> 0x0802701e ; branch_target=0x0802701e
08027034  d4f88430  ldr.w	r3, [r4, #132]
08027038  43f02003  orr	r3, r3, #32
0802703c  c4f88430  str.w	r3, [r4, #132]
08027040  2046      mov	r0, r4
08027042  fff71ffe  bl	#-962 ; -> 0x08026c84 ; branch_target=0x08026c84
08027046  0122      movs	r2, #1
08027048  0023      movs	r3, #0
0802704a  84f88120  strb.w	r2, [r4, #129]
0802704e  d4f88470  ldr.w	r7, [r4, #132]
08027052  84f88030  strb.w	r3, [r4, #128]
08027056  ff1a      subs	r7, r7, r3
08027058  18bf      it	ne
0802705a  0127      movne	r7, #1
0802705c  3846      mov	r0, r7
0802705e  bde8f087  pop.w	{r4, r5, r6, r7, r8, r9, r10, pc}
08027062  0023      movs	r3, #0
08027064  84f88030  strb.w	r3, [r4, #128]
08027068  0227      movs	r7, #2
0802706a  3846      mov	r0, r7
0802706c  bde8f087  pop.w	{r4, r5, r6, r7, r8, r9, r10, pc}
08027070  002b      cmp	r3, #0
08027072  d1d0      beq	#-94 ; -> 0x08027018 ; branch_target=0x08027018
08027074  2268      ldr	r2, [r4]
08027076  5369      ldr	r3, [r2, #20]
08027078  13f00208  ands	r8, r3, #2
0802707c  4bd0      beq	#150 ; -> 0x08027116 ; branch_target=0x08027116
0802707e  b4f86230  ldrh.w	r3, [r4, #98]
08027082  e16d      ldr	r1, [r4, #92]
08027084  9bb2      uxth	r3, r3
08027086  032b      cmp	r3, #3
08027088  17d9      bls	#46 ; -> 0x080270ba ; branch_target=0x080270ba
0802708a  e36b      ldr	r3, [r4, #60]
0802708c  402b      cmp	r3, #64
0802708e  14d9      bls	#40 ; -> 0x080270ba ; branch_target=0x080270ba
08027090  0b68      ldr	r3, [r1]
08027092  1362      str	r3, [r2, #32]
08027094  b4f86230  ldrh.w	r3, [r4, #98]
08027098  043b      subs	r3, #4
0802709a  9bb2      uxth	r3, r3
0802709c  a4f86230  strh.w	r3, [r4, #98]
080270a0  e36d      ldr	r3, [r4, #92]
080270a2  0433      adds	r3, #4
080270a4  e365      str	r3, [r4, #92]
080270a6  b4f86230  ldrh.w	r3, [r4, #98]
080270aa  9bb2      uxth	r3, r3
080270ac  e0e7      b	#-64 ; -> 0x08027070 ; branch_target=0x08027070
080270ae  0023      movs	r3, #0
080270b0  3846      mov	r0, r7
080270b2  84f88030  strb.w	r3, [r4, #128]
080270b6  bde8f087  pop.w	{r4, r5, r6, r7, r8, r9, r10, pc}
080270ba  b4f86230  ldrh.w	r3, [r4, #98]
080270be  9bb2      uxth	r3, r3
080270c0  012b      cmp	r3, #1
080270c2  1bd9      bls	#54 ; -> 0x080270fc ; branch_target=0x080270fc
080270c4  e36b      ldr	r3, [r4, #60]
080270c6  cbb1      cbz	r3, #50 ; -> 0x080270fc ; branch_target=0x080270fc
080270c8  0b88      ldrh	r3, [r1]
080270ca  a9f82030  strh.w	r3, [r9, #32]
080270ce  b4f86230  ldrh.w	r3, [r4, #98]
080270d2  023b      subs	r3, #2
080270d4  9bb2      uxth	r3, r3
080270d6  a4f86230  strh.w	r3, [r4, #98]
080270da  e36d      ldr	r3, [r4, #92]
080270dc  0233      adds	r3, #2
080270de  e365      str	r3, [r4, #92]
080270e0  e1e7      b	#-62 ; -> 0x080270a6 ; branch_target=0x080270a6
080270e2  0b88      ldrh	r3, [r1]
080270e4  a9f82030  strh.w	r3, [r9, #32]
080270e8  b4f86230  ldrh.w	r3, [r4, #98]
080270ec  013b      subs	r3, #1
080270ee  9bb2      uxth	r3, r3
080270f0  a4f86230  strh.w	r3, [r4, #98]
080270f4  e36d      ldr	r3, [r4, #92]
080270f6  0233      adds	r3, #2
080270f8  e365      str	r3, [r4, #92]
080270fa  4ae7      b	#-364 ; -> 0x08026f92 ; branch_target=0x08026f92
080270fc  0b78      ldrb	r3, [r1]
080270fe  82f82030  strb.w	r3, [r2, #32]
08027102  b4f86230  ldrh.w	r3, [r4, #98]
08027106  013b      subs	r3, #1
08027108  9bb2      uxth	r3, r3
0802710a  a4f86230  strh.w	r3, [r4, #98]
0802710e  e36d      ldr	r3, [r4, #92]
08027110  0133      adds	r3, #1
08027112  e365      str	r3, [r4, #92]
08027114  c7e7      b	#-114 ; -> 0x080270a6 ; branch_target=0x080270a6
08027116  f9f741f9  bl	#-28030 ; -> 0x0802039c ; branch_target=0x0802039c
0802711a  801b      subs	r0, r0, r6
0802711c  a842      cmp	r0, r5
0802711e  c2d3      blo	#-124 ; -> 0x080270a6 ; branch_target=0x080270a6
08027120  681c      adds	r0, r5, #1
08027122  c0d0      beq	#-128 ; -> 0x080270a6 ; branch_target=0x080270a6
08027124  46e7      b	#-372 ; -> 0x08026fb4 ; branch_target=0x08026fb4
08027126  2368      ldr	r3, [r4]
08027128  5b69      ldr	r3, [r3, #20]
0802712a  1b07      lsls	r3, r3, #28
0802712c  88d4      bmi	#-240 ; -> 0x08027040 ; branch_target=0x08027040
0802712e  f9f735f9  bl	#-28054 ; -> 0x0802039c ; branch_target=0x0802039c
08027132  7fe7      b	#-258 ; -> 0x08027034 ; branch_target=0x08027034
