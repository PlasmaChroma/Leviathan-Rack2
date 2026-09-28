; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
0802180c  38b5      push	{r3, r4, r5, lr}
0802180e  0446      mov	r4, r0
08021810  fef7c4fd  bl	#-5240 ; -> 0x0802039c ; branch_target=0x0802039c
08021814  002c      cmp	r4, #0
08021816  00f0bc81  beq.w	#888 ; -> 0x08021b92 ; branch_target=0x08021b92
0802181a  2368      ldr	r3, [r4]
0802181c  0546      mov	r5, r0
0802181e  544a      ldr	r2, [pc, #336] ; [0x08021970] = 0x40020010 / f32_bits_interpretation=2.031253815
08021820  9342      cmp	r3, r2
08021822  00f09980  beq.w	#306 ; -> 0x08021958 ; branch_target=0x08021958
08021826  1832      adds	r2, #24
08021828  9a1a      subs	r2, r3, r2
0802182a  b2fa82f2  clz	r2, r2
0802182e  5209      lsrs	r2, r2, #5
08021830  002a      cmp	r2, #0
08021832  40f09180  bne.w	#290 ; -> 0x08021958 ; branch_target=0x08021958
08021836  4f49      ldr	r1, [pc, #316] ; [0x08021974] = 0x40020040 / f32_bits_interpretation=2.031265259
08021838  8b42      cmp	r3, r1
0802183a  00f09f81  beq.w	#830 ; -> 0x08021b7c ; branch_target=0x08021b7c
0802183e  1831      adds	r1, #24
08021840  8b42      cmp	r3, r1
08021842  00f09b81  beq.w	#822 ; -> 0x08021b7c ; branch_target=0x08021b7c
08021846  1831      adds	r1, #24
08021848  8b42      cmp	r3, r1
0802184a  00f09781  beq.w	#814 ; -> 0x08021b7c ; branch_target=0x08021b7c
0802184e  1831      adds	r1, #24
08021850  8b42      cmp	r3, r1
08021852  00f09381  beq.w	#806 ; -> 0x08021b7c ; branch_target=0x08021b7c
08021856  1831      adds	r1, #24
08021858  8b42      cmp	r3, r1
0802185a  00f08f81  beq.w	#798 ; -> 0x08021b7c ; branch_target=0x08021b7c
0802185e  1831      adds	r1, #24
08021860  8b42      cmp	r3, r1
08021862  00f08b81  beq.w	#790 ; -> 0x08021b7c ; branch_target=0x08021b7c
08021866  444a      ldr	r2, [pc, #272] ; [0x08021978] = 0x40020410 / f32_bits_interpretation=2.031497955
08021868  9342      cmp	r3, r2
0802186a  75d0      beq	#234 ; -> 0x08021958 ; branch_target=0x08021958
0802186c  1832      adds	r2, #24
0802186e  9342      cmp	r3, r2
08021870  72d0      beq	#228 ; -> 0x08021958 ; branch_target=0x08021958
08021872  1832      adds	r2, #24
08021874  9342      cmp	r3, r2
08021876  6fd0      beq	#222 ; -> 0x08021958 ; branch_target=0x08021958
08021878  1832      adds	r2, #24
0802187a  9342      cmp	r3, r2
0802187c  6cd0      beq	#216 ; -> 0x08021958 ; branch_target=0x08021958
0802187e  1832      adds	r2, #24
08021880  9342      cmp	r3, r2
08021882  69d0      beq	#210 ; -> 0x08021958 ; branch_target=0x08021958
08021884  1832      adds	r2, #24
08021886  9342      cmp	r3, r2
08021888  66d0      beq	#204 ; -> 0x08021958 ; branch_target=0x08021958
0802188a  1832      adds	r2, #24
0802188c  9342      cmp	r3, r2
0802188e  63d0      beq	#198 ; -> 0x08021958 ; branch_target=0x08021958
08021890  1832      adds	r2, #24
08021892  9342      cmp	r3, r2
08021894  60d0      beq	#192 ; -> 0x08021958 ; branch_target=0x08021958
08021896  394a      ldr	r2, [pc, #228] ; [0x0802197c] = 0x58025408
08021898  3948      ldr	r0, [pc, #228] ; [0x08021980] = 0x5802541c
0802189a  3a49      ldr	r1, [pc, #232] ; [0x08021984] = 0x58025430
0802189c  8342      cmp	r3, r0
0802189e  18bf      it	ne
080218a0  9342      cmpne	r3, r2
080218a2  00f12800  add.w	r0, r0, #40
080218a6  0cbf      ite	eq
080218a8  0122      moveq	r2, #1
080218aa  0022      movne	r2, #0
080218ac  8b42      cmp	r3, r1
080218ae  08bf      it	eq
080218b0  42f00102  orreq	r2, r2, #1
080218b4  2831      adds	r1, #40
080218b6  8342      cmp	r3, r0
080218b8  08bf      it	eq
080218ba  42f00102  orreq	r2, r2, #1
080218be  2830      adds	r0, #40
080218c0  8b42      cmp	r3, r1
080218c2  08bf      it	eq
080218c4  42f00102  orreq	r2, r2, #1
080218c8  2831      adds	r1, #40
080218ca  8342      cmp	r3, r0
080218cc  08bf      it	eq
080218ce  42f00102  orreq	r2, r2, #1
080218d2  8b42      cmp	r3, r1
080218d4  08bf      it	eq
080218d6  42f00102  orreq	r2, r2, #1
080218da  1ab9      cbnz	r2, #6 ; -> 0x080218e4 ; branch_target=0x080218e4
080218dc  2a4a      ldr	r2, [pc, #168] ; [0x08021988] = 0x58025494
080218de  9342      cmp	r3, r2
080218e0  40f0e081  bne.w	#960 ; -> 0x08021ca4 ; branch_target=0x08021ca4
080218e4  0222      movs	r2, #2
080218e6  0020      movs	r0, #0
080218e8  2849      ldr	r1, [pc, #160] ; [0x0802198c] = 0xfffe000f
080218ea  84f83400  strb.w	r0, [r4, #52]
080218ee  84f83520  strb.w	r2, [r4, #53]
080218f2  1a68      ldr	r2, [r3]
080218f4  1140      ands	r1, r2
080218f6  a268      ldr	r2, [r4, #8]
080218f8  402a      cmp	r2, #64
080218fa  00f0b581  beq.w	#874 ; -> 0x08021c68 ; branch_target=0x08021c68
080218fe  a2f18002  sub.w	r2, r2, #128
08021902  b2fa82f2  clz	r2, r2
08021906  5209      lsrs	r2, r2, #5
08021908  9003      lsls	r0, r2, #14
0802190a  d4e90352  ldrd	r5, r2, [r4, #12]
0802190e  d208      lsrs	r2, r2, #3
08021910  42ead502  orr.w	r2, r2, r5, lsr #3
08021914  6569      ldr	r5, [r4, #20]
08021916  42ead502  orr.w	r2, r2, r5, lsr #3
0802191a  a569      ldr	r5, [r4, #24]
0802191c  42ead502  orr.w	r2, r2, r5, lsr #3
08021920  e569      ldr	r5, [r4, #28]
08021922  42ead502  orr.w	r2, r2, r5, lsr #3
08021926  256a      ldr	r5, [r4, #32]
08021928  42ea1512  orr.w	r2, r2, r5, lsr #4
0802192c  0a43      orrs	r2, r1
0802192e  1849      ldr	r1, [pc, #96] ; [0x08021990] = 0xa7fdabf8
08021930  1043      orrs	r0, r2
08021932  1860      str	r0, [r3]
08021934  2046      mov	r0, r4
08021936  2268      ldr	r2, [r4]
08021938  164b      ldr	r3, [pc, #88] ; [0x08021994] = 0xcccccccd / f32_bits_interpretation=-107374184
0802193a  1144      add	r1, r2
0802193c  a3fb0123  umull	r2, r3, r3, r1
08021940  1b09      lsrs	r3, r3, #4
08021942  9b00      lsls	r3, r3, #2
08021944  e365      str	r3, [r4, #92]
08021946  fff779fe  bl	#-782 ; -> 0x0802163c ; branch_target=0x0802163c
0802194a  e16d      ldr	r1, [r4, #92]
0802194c  0122      movs	r2, #1
0802194e  01f01f01  and	r1, r1, #31
08021952  8a40      lsls	r2, r1
08021954  4260      str	r2, [r0, #4]
08021956  7de0      b	#250 ; -> 0x08021a54 ; branch_target=0x08021a54
08021958  0021      movs	r1, #0
0802195a  0222      movs	r2, #2
0802195c  84f83410  strb.w	r1, [r4, #52]
08021960  84f83520  strb.w	r2, [r4, #53]
08021964  1a68      ldr	r2, [r3]
08021966  22f00102  bic	r2, r2, #1
0802196a  1a60      str	r2, [r3]
0802196c  1ae0      b	#52 ; -> 0x080219a4 ; branch_target=0x080219a4
08021998  fef700fd  bl	#-5632 ; -> 0x0802039c ; branch_target=0x0802039c
0802199c  431b      subs	r3, r0, r5
0802199e  052b      cmp	r3, #5
080219a0  00f2f280  bhi.w	#484 ; -> 0x08021b88 ; branch_target=0x08021b88
080219a4  2368      ldr	r3, [r4]
080219a6  1a68      ldr	r2, [r3]
080219a8  d007      lsls	r0, r2, #31
080219aa  f5d4      bmi	#-22 ; -> 0x08021998 ; branch_target=0x08021998
080219ac  d4e90220  ldrd	r2, r0, [r4, #8]
080219b0  2169      ldr	r1, [r4, #16]
080219b2  0243      orrs	r2, r0
080219b4  1d68      ldr	r5, [r3]
080219b6  606a      ldr	r0, [r4, #36]
080219b8  0a43      orrs	r2, r1
080219ba  6169      ldr	r1, [r4, #20]
080219bc  0428      cmp	r0, #4
080219be  42ea0102  orr.w	r2, r2, r1
080219c2  a169      ldr	r1, [r4, #24]
080219c4  42ea0102  orr.w	r2, r2, r1
080219c8  e169      ldr	r1, [r4, #28]
080219ca  42ea0102  orr.w	r2, r2, r1
080219ce  216a      ldr	r1, [r4, #32]
080219d0  42ea0102  orr.w	r2, r2, r1
080219d4  a549      ldr	r1, [pc, #660] ; [0x08021c6c] = 0xfe10803f
080219d6  01ea0501  and.w	r1, r1, r5
080219da  42ea0102  orr.w	r2, r2, r1
080219de  03d1      bne	#6 ; -> 0x080219e8 ; branch_target=0x080219e8
080219e0  d4e90b10  ldrd	r1, r0, [r4, #44]
080219e4  0143      orrs	r1, r0
080219e6  0a43      orrs	r2, r1
080219e8  a148      ldr	r0, [pc, #644] ; [0x08021c70] = 0x5c001000
080219ea  a249      ldr	r1, [pc, #648] ; [0x08021c74] = 0xffff0000
080219ec  0068      ldr	r0, [r0]
080219ee  0140      ands	r1, r0
080219f0  b1f1005f  cmp.w	r1, #536870912
080219f4  0bd3      blo	#22 ; -> 0x08021a0e ; branch_target=0x08021a0e
080219f6  6168      ldr	r1, [r4, #4]
080219f8  a1f12900  sub.w	r0, r1, #41
080219fc  1f28      cmp	r0, #31
080219fe  00f2ca80  bhi.w	#404 ; -> 0x08021b96 ; branch_target=0x08021b96
08021a02  9d49      ldr	r1, [pc, #628] ; [0x08021c78] = 0xc3c0003f / f32_bits_interpretation=-384.0019226
08021a04  c140      lsrs	r1, r0
08021a06  c907      lsls	r1, r1, #31
08021a08  01d5      bpl	#2 ; -> 0x08021a0e ; branch_target=0x08021a0e
08021a0a  42f48012  orr	r2, r2, #1048576
08021a0e  1a60      str	r2, [r3]
08021a10  2168      ldr	r1, [r4]
08021a12  626a      ldr	r2, [r4, #36]
08021a14  4b69      ldr	r3, [r1, #20]
08021a16  042a      cmp	r2, #4
08021a18  23f00703  bic	r3, r3, #7
08021a1c  43ea0203  orr.w	r3, r3, r2
08021a20  0ed1      bne	#28 ; -> 0x08021a40 ; branch_target=0x08021a40
08021a22  d4e90a20  ldrd	r2, r0, [r4, #40]
08021a26  1343      orrs	r3, r2
08021a28  50b1      cbz	r0, #20 ; -> 0x08021a40 ; branch_target=0x08021a40
08021a2a  a569      ldr	r5, [r4, #24]
08021a2c  002d      cmp	r5, #0
08021a2e  40f0f180  bne.w	#482 ; -> 0x08021c14 ; branch_target=0x08021c14
08021a32  012a      cmp	r2, #1
08021a34  00f00e81  beq.w	#540 ; -> 0x08021c54 ; branch_target=0x08021c54
08021a38  32f00202  bics	r2, r2, #2
08021a3c  00f0f280  beq.w	#484 ; -> 0x08021c24 ; branch_target=0x08021c24
08021a40  4b61      str	r3, [r1, #20]
08021a42  2046      mov	r0, r4
08021a44  fff7fafd  bl	#-1036 ; -> 0x0802163c ; branch_target=0x0802163c
08021a48  e26d      ldr	r2, [r4, #92]
08021a4a  3f23      movs	r3, #63
08021a4c  02f01f02  and	r2, r2, #31
08021a50  9340      lsls	r3, r2
08021a52  8360      str	r3, [r0, #8]
08021a54  2268      ldr	r2, [r4]
08021a56  894b      ldr	r3, [pc, #548] ; [0x08021c7c] = 0x40020010 / f32_bits_interpretation=2.031253815
08021a58  8948      ldr	r0, [pc, #548] ; [0x08021c80] = 0x40020028 / f32_bits_interpretation=2.031259537
08021a5a  8a49      ldr	r1, [pc, #552] ; [0x08021c84] = 0x40020040 / f32_bits_interpretation=2.031265259
08021a5c  8242      cmp	r2, r0
08021a5e  18bf      it	ne
08021a60  9a42      cmpne	r2, r3
08021a62  00f13000  add.w	r0, r0, #48
08021a66  0cbf      ite	eq
08021a68  0123      moveq	r3, #1
08021a6a  0023      movne	r3, #0
08021a6c  8a42      cmp	r2, r1
08021a6e  08bf      it	eq
08021a70  43f00103  orreq	r3, r3, #1
08021a74  3031      adds	r1, #48
08021a76  8242      cmp	r2, r0
08021a78  08bf      it	eq
08021a7a  43f00103  orreq	r3, r3, #1
08021a7e  3030      adds	r0, #48
08021a80  8a42      cmp	r2, r1
08021a82  08bf      it	eq
08021a84  43f00103  orreq	r3, r3, #1
08021a88  3031      adds	r1, #48
08021a8a  8242      cmp	r2, r0
08021a8c  08bf      it	eq
08021a8e  43f00103  orreq	r3, r3, #1
08021a92  3030      adds	r0, #48
08021a94  8a42      cmp	r2, r1
08021a96  08bf      it	eq
08021a98  43f00103  orreq	r3, r3, #1
08021a9c  01f55c71  add.w	r1, r1, #880
08021aa0  8242      cmp	r2, r0
08021aa2  08bf      it	eq
08021aa4  43f00103  orreq	r3, r3, #1
08021aa8  00f55c70  add.w	r0, r0, #880
08021aac  8a42      cmp	r2, r1
08021aae  08bf      it	eq
08021ab0  43f00103  orreq	r3, r3, #1
08021ab4  3031      adds	r1, #48
08021ab6  8242      cmp	r2, r0
08021ab8  08bf      it	eq
08021aba  43f00103  orreq	r3, r3, #1
08021abe  3030      adds	r0, #48
08021ac0  8a42      cmp	r2, r1
08021ac2  08bf      it	eq
08021ac4  43f00103  orreq	r3, r3, #1
08021ac8  3031      adds	r1, #48
08021aca  8242      cmp	r2, r0
08021acc  08bf      it	eq
08021ace  43f00103  orreq	r3, r3, #1
08021ad2  3030      adds	r0, #48
08021ad4  8a42      cmp	r2, r1
08021ad6  08bf      it	eq
08021ad8  43f00103  orreq	r3, r3, #1
08021adc  3031      adds	r1, #48
08021ade  8242      cmp	r2, r0
08021ae0  08bf      it	eq
08021ae2  43f00103  orreq	r3, r3, #1
08021ae6  3030      adds	r0, #48
08021ae8  8a42      cmp	r2, r1
08021aea  08bf      it	eq
08021aec  43f00103  orreq	r3, r3, #1
08021af0  6549      ldr	r1, [pc, #404] ; [0x08021c88] = 0x58025408
08021af2  8242      cmp	r2, r0
08021af4  08bf      it	eq
08021af6  43f00103  orreq	r3, r3, #1
08021afa  6448      ldr	r0, [pc, #400] ; [0x08021c8c] = 0x5802541c
08021afc  8a42      cmp	r2, r1
08021afe  08bf      it	eq
08021b00  43f00103  orreq	r3, r3, #1
08021b04  2831      adds	r1, #40
08021b06  8242      cmp	r2, r0
08021b08  08bf      it	eq
08021b0a  43f00103  orreq	r3, r3, #1
08021b0e  2830      adds	r0, #40
08021b10  8a42      cmp	r2, r1
08021b12  08bf      it	eq
08021b14  43f00103  orreq	r3, r3, #1
08021b18  2831      adds	r1, #40
08021b1a  8242      cmp	r2, r0
08021b1c  08bf      it	eq
08021b1e  43f00103  orreq	r3, r3, #1
08021b22  2830      adds	r0, #40
08021b24  8a42      cmp	r2, r1
08021b26  08bf      it	eq
08021b28  43f00103  orreq	r3, r3, #1
08021b2c  2831      adds	r1, #40
08021b2e  8242      cmp	r2, r0
08021b30  08bf      it	eq
08021b32  43f00103  orreq	r3, r3, #1
08021b36  8a42      cmp	r2, r1
08021b38  08bf      it	eq
08021b3a  43f00103  orreq	r3, r3, #1
08021b3e  13b9      cbnz	r3, #4 ; -> 0x08021b46 ; branch_target=0x08021b46
08021b40  534b      ldr	r3, [pc, #332] ; [0x08021c90] = 0x58025494
08021b42  9a42      cmp	r2, r3
08021b44  13d1      bne	#38 ; -> 0x08021b6e ; branch_target=0x08021b6e
08021b46  2046      mov	r0, r4
08021b48  fff7f8fd  bl	#-1040 ; -> 0x0802173c ; branch_target=0x0802173c
08021b4c  a368      ldr	r3, [r4, #8]
08021b4e  802b      cmp	r3, #128
08021b50  26d0      beq	#76 ; -> 0x08021ba0 ; branch_target=0x08021ba0
08021b52  2279      ldrb	r2, [r4, #4]
08021b54  236e      ldr	r3, [r4, #96]
08021b56  1a60      str	r2, [r3]
08021b58  d4e91932  ldrd	r3, r2, [r4, #100]
08021b5c  5a60      str	r2, [r3, #4]
08021b5e  6068      ldr	r0, [r4, #4]
08021b60  411e      subs	r1, r0, #1
08021b62  0729      cmp	r1, #7
08021b64  20d9      bls	#64 ; -> 0x08021ba8 ; branch_target=0x08021ba8
08021b66  0023      movs	r3, #0
08021b68  c4e91b33  strd	r3, r3, [r4, #108]
08021b6c  6367      str	r3, [r4, #116]
08021b6e  0023      movs	r3, #0
08021b70  0122      movs	r2, #1
08021b72  6365      str	r3, [r4, #84]
08021b74  1846      mov	r0, r3
08021b76  84f83520  strb.w	r2, [r4, #53]
08021b7a  38bd      pop	{r3, r4, r5, pc}
08021b7c  0221      movs	r1, #2
08021b7e  84f83420  strb.w	r2, [r4, #52]
08021b82  84f83510  strb.w	r1, [r4, #53]
08021b86  ede6      b	#-550 ; -> 0x08021964 ; branch_target=0x08021964
08021b88  2022      movs	r2, #32
08021b8a  0323      movs	r3, #3
08021b8c  6265      str	r2, [r4, #84]
08021b8e  84f83530  strb.w	r3, [r4, #53]
08021b92  0120      movs	r0, #1
08021b94  38bd      pop	{r3, r4, r5, pc}
08021b96  4f39      subs	r1, #79
08021b98  0329      cmp	r1, #3
08021b9a  7ff636af  bls.w	#-404 ; -> 0x08021a0a ; branch_target=0x08021a0a
08021b9e  36e7      b	#-404 ; -> 0x08021a0e ; branch_target=0x08021a0e
08021ba0  0023      movs	r3, #0
08021ba2  1a46      mov	r2, r3
08021ba4  6360      str	r3, [r4, #4]
08021ba6  d5e7      b	#-86 ; -> 0x08021b54 ; branch_target=0x08021b54
08021ba8  2268      ldr	r2, [r4]
08021baa  374b      ldr	r3, [pc, #220] ; [0x08021c88] = 0x58025408
08021bac  374d      ldr	r5, [pc, #220] ; [0x08021c8c] = 0x5802541c
08021bae  aa42      cmp	r2, r5
08021bb0  18bf      it	ne
08021bb2  9a42      cmpne	r2, r3
08021bb4  05f11405  add.w	r5, r5, #20
08021bb8  0cbf      ite	eq
08021bba  0123      moveq	r3, #1
08021bbc  0023      movne	r3, #0
08021bbe  aa42      cmp	r2, r5
08021bc0  08bf      it	eq
08021bc2  43f00103  orreq	r3, r3, #1
08021bc6  1435      adds	r5, #20
08021bc8  aa42      cmp	r2, r5
08021bca  08bf      it	eq
08021bcc  43f00103  orreq	r3, r3, #1
08021bd0  1435      adds	r5, #20
08021bd2  aa42      cmp	r2, r5
08021bd4  08bf      it	eq
08021bd6  43f00103  orreq	r3, r3, #1
08021bda  1435      adds	r5, #20
08021bdc  aa42      cmp	r2, r5
08021bde  08bf      it	eq
08021be0  43f00103  orreq	r3, r3, #1
08021be4  1435      adds	r5, #20
08021be6  aa42      cmp	r2, r5
08021be8  08bf      it	eq
08021bea  43f00103  orreq	r3, r3, #1
08021bee  13b9      cbnz	r3, #4 ; -> 0x08021bf6 ; branch_target=0x08021bf6
08021bf0  274b      ldr	r3, [pc, #156] ; [0x08021c90] = 0x58025494
08021bf2  9a42      cmp	r2, r3
08021bf4  33d1      bne	#102 ; -> 0x08021c5e ; branch_target=0x08021c5e
08021bf6  274b      ldr	r3, [pc, #156] ; [0x08021c94] = 0x1600963f
08021bf8  274d      ldr	r5, [pc, #156] ; [0x08021c98] = 0x58025940
08021bfa  0344      add	r3, r0
08021bfc  9b00      lsls	r3, r3, #2
08021bfe  0122      movs	r2, #1
08021c00  8a40      lsls	r2, r1
08021c02  0021      movs	r1, #0
08021c04  c4e91b35  strd	r3, r5, [r4, #108]
08021c08  6267      str	r2, [r4, #116]
08021c0a  1960      str	r1, [r3]
08021c0c  d4e91c32  ldrd	r3, r2, [r4, #112]
08021c10  5a60      str	r2, [r3, #4]
08021c12  ace7      b	#-168 ; -> 0x08021b6e ; branch_target=0x08021b6e
08021c14  b5f5005f  cmp.w	r5, #8192
08021c18  0dd0      beq	#26 ; -> 0x08021c36 ; branch_target=0x08021c36
08021c1a  022a      cmp	r2, #2
08021c1c  05d9      bls	#10 ; -> 0x08021c2a ; branch_target=0x08021c2a
08021c1e  032a      cmp	r2, #3
08021c20  7ff40eaf  bne.w	#-484 ; -> 0x08021a40 ; branch_target=0x08021a40
08021c24  c201      lsls	r2, r0, #7
08021c26  7ff50baf  bpl.w	#-490 ; -> 0x08021a40 ; branch_target=0x08021a40
08021c2a  4022      movs	r2, #64
08021c2c  0123      movs	r3, #1
08021c2e  6265      str	r2, [r4, #84]
08021c30  84f83530  strb.w	r3, [r4, #53]
08021c34  ade7      b	#-166 ; -> 0x08021b92 ; branch_target=0x08021b92
08021c36  032a      cmp	r2, #3
08021c38  3ff602af  bhi.w	#-508 ; -> 0x08021a40 ; branch_target=0x08021a40
08021c3c  01a5      adr	r5, #4
08021c3e  55f822f0  ldr.w	pc, [r5, r2, lsl #2]
08021c42  00bf      nop
08021c44  2b1c      adds	r3, r5, #0
08021c46  0208      lsrs	r2, r0, #32
08021c48  251c      adds	r5, r4, #0
08021c4a  0208      lsrs	r2, r0, #32
08021c4c  2b1c      adds	r3, r5, #0
08021c4e  0208      lsrs	r2, r0, #32
08021c50  551c      adds	r5, r2, #1
08021c52  0208      lsrs	r2, r0, #32
08021c54  b0f1c07f  cmp.w	r0, #25165824
08021c58  7ff4f2ae  bne.w	#-540 ; -> 0x08021a40 ; branch_target=0x08021a40
08021c5c  e5e7      b	#-54 ; -> 0x08021c2a ; branch_target=0x08021c2a
08021c5e  0f4b      ldr	r3, [pc, #60] ; [0x08021c9c] = 0x1000823f
08021c60  0f4d      ldr	r5, [pc, #60] ; [0x08021ca0] = 0x40020940 / f32_bits_interpretation=2.031814575
08021c62  0344      add	r3, r0
08021c64  9b00      lsls	r3, r3, #2
08021c66  cae7      b	#-108 ; -> 0x08021bfe ; branch_target=0x08021bfe
08021c68  1020      movs	r0, #16
08021c6a  4ee6      b	#-868 ; -> 0x0802190a ; branch_target=0x0802190a
08021ca4  4022      movs	r2, #64
08021ca6  0323      movs	r3, #3
08021ca8  6265      str	r2, [r4, #84]
08021caa  84f83530  strb.w	r3, [r4, #53]
08021cae  70e7      b	#-288 ; -> 0x08021b92 ; branch_target=0x08021b92
