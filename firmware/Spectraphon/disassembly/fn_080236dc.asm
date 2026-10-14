; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
080236dc  0028      cmp	r0, #0
080236de  00f0d982  beq.w	#1458 ; -> 0x08023c94 ; branch_target=0x08023c94
080236e2  f8b5      push	{r3, r4, r5, r6, r7, lr}
080236e4  0368      ldr	r3, [r0]
080236e6  0446      mov	r4, r0
080236e8  d907      lsls	r1, r3, #31
080236ea  2ed5      bpl	#92 ; -> 0x0802374a ; branch_target=0x0802374a
080236ec  9f49      ldr	r1, [pc, #636] ; [0x0802396c] = 0x58024400
080236ee  0a69      ldr	r2, [r1, #16]
080236f0  896a      ldr	r1, [r1, #40]
080236f2  02f03802  and	r2, r2, #56
080236f6  102a      cmp	r2, #16
080236f8  00f02581  beq.w	#586 ; -> 0x08023946 ; branch_target=0x08023946
080236fc  182a      cmp	r2, #24
080236fe  00f01d81  beq.w	#570 ; -> 0x0802393c ; branch_target=0x0802393c
08023702  6368      ldr	r3, [r4, #4]
08023704  b3f5803f  cmp.w	r3, #65536
08023708  00f06081  beq.w	#704 ; -> 0x080239cc ; branch_target=0x080239cc
0802370c  002b      cmp	r3, #0
0802370e  40f0a981  bne.w	#850 ; -> 0x08023a64 ; branch_target=0x08023a64
08023712  964b      ldr	r3, [pc, #600] ; [0x0802396c] = 0x58024400
08023714  1a68      ldr	r2, [r3]
08023716  22f48032  bic	r2, r2, #65536
0802371a  1a60      str	r2, [r3]
0802371c  1a68      ldr	r2, [r3]
0802371e  22f48022  bic	r2, r2, #262144
08023722  1a60      str	r2, [r3]
08023724  6368      ldr	r3, [r4, #4]
08023726  002b      cmp	r3, #0
08023728  00f06b81  beq.w	#726 ; -> 0x08023a02 ; branch_target=0x08023a02
0802372c  fcf736fe  bl	#-13204 ; -> 0x0802039c ; branch_target=0x0802039c
08023730  8e4e      ldr	r6, [pc, #568] ; [0x0802396c] = 0x58024400
08023732  0546      mov	r5, r0
08023734  05e0      b	#10 ; -> 0x08023742 ; branch_target=0x08023742
08023736  fcf731fe  bl	#-13214 ; -> 0x0802039c ; branch_target=0x0802039c
0802373a  401b      subs	r0, r0, r5
0802373c  6428      cmp	r0, #100
0802373e  00f25e81  bhi.w	#700 ; -> 0x080239fe ; branch_target=0x080239fe
08023742  3368      ldr	r3, [r6]
08023744  9f03      lsls	r7, r3, #14
08023746  f6d5      bpl	#-20 ; -> 0x08023736 ; branch_target=0x08023736
08023748  2368      ldr	r3, [r4]
0802374a  9907      lsls	r1, r3, #30
0802374c  21d5      bpl	#66 ; -> 0x08023792 ; branch_target=0x08023792
0802374e  874a      ldr	r2, [pc, #540] ; [0x0802396c] = 0x58024400
08023750  1369      ldr	r3, [r2, #16]
08023752  926a      ldr	r2, [r2, #40]
08023754  13f03803  ands	r3, r3, #56
08023758  40f0a480  bne.w	#328 ; -> 0x080238a4 ; branch_target=0x080238a4
0802375c  834b      ldr	r3, [pc, #524] ; [0x0802396c] = 0x58024400
0802375e  1b68      ldr	r3, [r3]
08023760  5b07      lsls	r3, r3, #29
08023762  03d5      bpl	#6 ; -> 0x0802376c ; branch_target=0x0802376c
08023764  e368      ldr	r3, [r4, #12]
08023766  002b      cmp	r3, #0
08023768  00f0e680  beq.w	#460 ; -> 0x08023938 ; branch_target=0x08023938
0802376c  fcf72efe  bl	#-13220 ; -> 0x080203cc ; branch_target=0x080203cc
08023770  41f20303  movw	r3, #4099
08023774  9842      cmp	r0, r3
08023776  00f28381  bhi.w	#774 ; -> 0x08023a80 ; branch_target=0x08023a80
0802377a  2269      ldr	r2, [r4, #16]
0802377c  402a      cmp	r2, #64
0802377e  00f03b82  beq.w	#1142 ; -> 0x08023bf8 ; branch_target=0x08023bf8
08023782  7a49      ldr	r1, [pc, #488] ; [0x0802396c] = 0x58024400
08023784  4b68      ldr	r3, [r1, #4]
08023786  23f47c33  bic	r3, r3, #258048
0802378a  43ea0233  orr.w	r3, r3, r2, lsl #12
0802378e  4b60      str	r3, [r1, #4]
08023790  2368      ldr	r3, [r4]
08023792  d906      lsls	r1, r3, #27
08023794  53d4      bmi	#166 ; -> 0x0802383e ; branch_target=0x0802383e
08023796  1d07      lsls	r5, r3, #28
08023798  16d5      bpl	#44 ; -> 0x080237c8 ; branch_target=0x080237c8
0802379a  6369      ldr	r3, [r4, #20]
0802379c  734d      ldr	r5, [pc, #460] ; [0x0802396c] = 0x58024400
0802379e  002b      cmp	r3, #0
080237a0  00f0ae80  beq.w	#348 ; -> 0x08023900 ; branch_target=0x08023900
080237a4  6b6f      ldr	r3, [r5, #116]
080237a6  43f00103  orr	r3, r3, #1
080237aa  6b67      str	r3, [r5, #116]
080237ac  fcf7f6fd  bl	#-13332 ; -> 0x0802039c ; branch_target=0x0802039c
080237b0  0646      mov	r6, r0
080237b2  05e0      b	#10 ; -> 0x080237c0 ; branch_target=0x080237c0
080237b4  fcf7f2fd  bl	#-13340 ; -> 0x0802039c ; branch_target=0x0802039c
080237b8  801b      subs	r0, r0, r6
080237ba  0228      cmp	r0, #2
080237bc  00f21f81  bhi.w	#574 ; -> 0x080239fe ; branch_target=0x080239fe
080237c0  6b6f      ldr	r3, [r5, #116]
080237c2  9807      lsls	r0, r3, #30
080237c4  f6d5      bpl	#-20 ; -> 0x080237b4 ; branch_target=0x080237b4
080237c6  2368      ldr	r3, [r4]
080237c8  9a06      lsls	r2, r3, #26
080237ca  16d5      bpl	#44 ; -> 0x080237fa ; branch_target=0x080237fa
080237cc  a369      ldr	r3, [r4, #24]
080237ce  674d      ldr	r5, [pc, #412] ; [0x0802396c] = 0x58024400
080237d0  002b      cmp	r3, #0
080237d2  00f02481  beq.w	#584 ; -> 0x08023a1e ; branch_target=0x08023a1e
080237d6  2b68      ldr	r3, [r5]
080237d8  43f48053  orr	r3, r3, #4096
080237dc  2b60      str	r3, [r5]
080237de  fcf7ddfd  bl	#-13382 ; -> 0x0802039c ; branch_target=0x0802039c
080237e2  0646      mov	r6, r0
080237e4  05e0      b	#10 ; -> 0x080237f2 ; branch_target=0x080237f2
080237e6  fcf7d9fd  bl	#-13390 ; -> 0x0802039c ; branch_target=0x0802039c
080237ea  801b      subs	r0, r0, r6
080237ec  0228      cmp	r0, #2
080237ee  00f20681  bhi.w	#524 ; -> 0x080239fe ; branch_target=0x080239fe
080237f2  2b68      ldr	r3, [r5]
080237f4  9f04      lsls	r7, r3, #18
080237f6  f6d5      bpl	#-20 ; -> 0x080237e6 ; branch_target=0x080237e6
080237f8  2368      ldr	r3, [r4]
080237fa  5907      lsls	r1, r3, #29
080237fc  00f1ad80  bmi.w	#346 ; -> 0x0802395a ; branch_target=0x0802395a
08023800  626a      ldr	r2, [r4, #36]
08023802  d2b1      cbz	r2, #52 ; -> 0x0802383a ; branch_target=0x0802383a
08023804  594d      ldr	r5, [pc, #356] ; [0x0802396c] = 0x58024400
08023806  2b69      ldr	r3, [r5, #16]
08023808  03f03803  and	r3, r3, #56
0802380c  182b      cmp	r3, #24
0802380e  00f0c581  beq.w	#906 ; -> 0x08023b9c ; branch_target=0x08023b9c
08023812  2b68      ldr	r3, [r5]
08023814  022a      cmp	r2, #2
08023816  23f08073  bic	r3, r3, #16777216
0802381a  2b60      str	r3, [r5]
0802381c  00f05981  beq.w	#690 ; -> 0x08023ad2 ; branch_target=0x08023ad2
08023820  fcf7bcfd  bl	#-13448 ; -> 0x0802039c ; branch_target=0x0802039c
08023824  0446      mov	r4, r0
08023826  05e0      b	#10 ; -> 0x08023834 ; branch_target=0x08023834
08023828  fcf7b8fd  bl	#-13456 ; -> 0x0802039c ; branch_target=0x0802039c
0802382c  001b      subs	r0, r0, r4
0802382e  0228      cmp	r0, #2
08023830  00f2e580  bhi.w	#458 ; -> 0x080239fe ; branch_target=0x080239fe
08023834  2b68      ldr	r3, [r5]
08023836  9b01      lsls	r3, r3, #6
08023838  f6d4      bmi	#-20 ; -> 0x08023828 ; branch_target=0x08023828
0802383a  0020      movs	r0, #0
0802383c  f8bd      pop	{r3, r4, r5, r6, r7, pc}
0802383e  4b4a      ldr	r2, [pc, #300] ; [0x0802396c] = 0x58024400
08023840  1369      ldr	r3, [r2, #16]
08023842  926a      ldr	r2, [r2, #40]
08023844  03f03803  and	r3, r3, #56
08023848  082b      cmp	r3, #8
0802384a  6ed0      beq	#220 ; -> 0x0802392a ; branch_target=0x0802392a
0802384c  182b      cmp	r3, #24
0802384e  68d0      beq	#208 ; -> 0x08023922 ; branch_target=0x08023922
08023850  e369      ldr	r3, [r4, #28]
08023852  464d      ldr	r5, [pc, #280] ; [0x0802396c] = 0x58024400
08023854  002b      cmp	r3, #0
08023856  00f0f380  beq.w	#486 ; -> 0x08023a40 ; branch_target=0x08023a40
0802385a  2b68      ldr	r3, [r5]
0802385c  43f08003  orr	r3, r3, #128
08023860  2b60      str	r3, [r5]
08023862  fcf79bfd  bl	#-13514 ; -> 0x0802039c ; branch_target=0x0802039c
08023866  0646      mov	r6, r0
08023868  05e0      b	#10 ; -> 0x08023876 ; branch_target=0x08023876
0802386a  fcf797fd  bl	#-13522 ; -> 0x0802039c ; branch_target=0x0802039c
0802386e  801b      subs	r0, r0, r6
08023870  0228      cmp	r0, #2
08023872  00f2c480  bhi.w	#392 ; -> 0x080239fe ; branch_target=0x080239fe
08023876  2b68      ldr	r3, [r5]
08023878  db05      lsls	r3, r3, #23
0802387a  f6d5      bpl	#-20 ; -> 0x0802386a ; branch_target=0x0802386a
0802387c  fcf7a6fd  bl	#-13492 ; -> 0x080203cc ; branch_target=0x080203cc
08023880  41f20303  movw	r3, #4099
08023884  9842      cmp	r0, r3
08023886  00f2d781  bhi.w	#942 ; -> 0x08023c38 ; branch_target=0x08023c38
0802388a  226a      ldr	r2, [r4, #32]
0802388c  6b68      ldr	r3, [r5, #4]
0802388e  202a      cmp	r2, #32
08023890  23f0f843  bic	r3, r3, #2080374784
08023894  0cbf      ite	eq
08023896  43f08043  orreq	r3, r3, #1073741824
0802389a  43ea8263  orrne.w	r3, r3, r2, lsl #26
0802389e  6b60      str	r3, [r5, #4]
080238a0  2368      ldr	r3, [r4]
080238a2  78e7      b	#-272 ; -> 0x08023796 ; branch_target=0x08023796
080238a4  182b      cmp	r3, #24
080238a6  00f00681  beq.w	#524 ; -> 0x08023ab6 ; branch_target=0x08023ab6
080238aa  304d      ldr	r5, [pc, #192] ; [0x0802396c] = 0x58024400
080238ac  e268      ldr	r2, [r4, #12]
080238ae  2b68      ldr	r3, [r5]
080238b0  002a      cmp	r2, #0
080238b2  00f0ef80  beq.w	#478 ; -> 0x08023a94 ; branch_target=0x08023a94
080238b6  23f01903  bic	r3, r3, #25
080238ba  1343      orrs	r3, r2
080238bc  2b60      str	r3, [r5]
080238be  fcf76dfd  bl	#-13606 ; -> 0x0802039c ; branch_target=0x0802039c
080238c2  0646      mov	r6, r0
080238c4  05e0      b	#10 ; -> 0x080238d2 ; branch_target=0x080238d2
080238c6  fcf769fd  bl	#-13614 ; -> 0x0802039c ; branch_target=0x0802039c
080238ca  801b      subs	r0, r0, r6
080238cc  0228      cmp	r0, #2
080238ce  00f29680  bhi.w	#300 ; -> 0x080239fe ; branch_target=0x080239fe
080238d2  2b68      ldr	r3, [r5]
080238d4  5f07      lsls	r7, r3, #29
080238d6  f6d5      bpl	#-20 ; -> 0x080238c6 ; branch_target=0x080238c6
080238d8  fcf778fd  bl	#-13584 ; -> 0x080203cc ; branch_target=0x080203cc
080238dc  41f20303  movw	r3, #4099
080238e0  9842      cmp	r0, r3
080238e2  00f2b281  bhi.w	#868 ; -> 0x08023c4a ; branch_target=0x08023c4a
080238e6  2269      ldr	r2, [r4, #16]
080238e8  6b68      ldr	r3, [r5, #4]
080238ea  402a      cmp	r2, #64
080238ec  23f47c33  bic	r3, r3, #258048
080238f0  0cbf      ite	eq
080238f2  43f40033  orreq	r3, r3, #131072
080238f6  43ea0233  orrne.w	r3, r3, r2, lsl #12
080238fa  6b60      str	r3, [r5, #4]
080238fc  2368      ldr	r3, [r4]
080238fe  48e7      b	#-368 ; -> 0x08023792 ; branch_target=0x08023792
08023900  6b6f      ldr	r3, [r5, #116]
08023902  23f00103  bic	r3, r3, #1
08023906  6b67      str	r3, [r5, #116]
08023908  fcf748fd  bl	#-13680 ; -> 0x0802039c ; branch_target=0x0802039c
0802390c  0646      mov	r6, r0
0802390e  04e0      b	#8 ; -> 0x0802391a ; branch_target=0x0802391a
08023910  fcf744fd  bl	#-13688 ; -> 0x0802039c ; branch_target=0x0802039c
08023914  801b      subs	r0, r0, r6
08023916  0228      cmp	r0, #2
08023918  71d8      bhi	#226 ; -> 0x080239fe ; branch_target=0x080239fe
0802391a  6b6f      ldr	r3, [r5, #116]
0802391c  9907      lsls	r1, r3, #30
0802391e  f7d4      bmi	#-18 ; -> 0x08023910 ; branch_target=0x08023910
08023920  51e7      b	#-350 ; -> 0x080237c6 ; branch_target=0x080237c6
08023922  02f00302  and	r2, r2, #3
08023926  012a      cmp	r2, #1
08023928  92d1      bne	#-220 ; -> 0x08023850 ; branch_target=0x08023850
0802392a  104b      ldr	r3, [pc, #64] ; [0x0802396c] = 0x58024400
0802392c  1b68      ldr	r3, [r3]
0802392e  da05      lsls	r2, r3, #23
08023930  52d5      bpl	#164 ; -> 0x080239d8 ; branch_target=0x080239d8
08023932  e369      ldr	r3, [r4, #28]
08023934  802b      cmp	r3, #128
08023936  4fd0      beq	#158 ; -> 0x080239d8 ; branch_target=0x080239d8
08023938  0120      movs	r0, #1
0802393a  f8bd      pop	{r3, r4, r5, r6, r7, pc}
0802393c  01f00301  and	r1, r1, #3
08023940  0229      cmp	r1, #2
08023942  7ff4deae  bne.w	#-580 ; -> 0x08023702 ; branch_target=0x08023702
08023946  094a      ldr	r2, [pc, #36] ; [0x0802396c] = 0x58024400
08023948  1268      ldr	r2, [r2]
0802394a  9203      lsls	r2, r2, #14
0802394c  7ff5fdae  bpl.w	#-518 ; -> 0x0802374a ; branch_target=0x0802374a
08023950  6268      ldr	r2, [r4, #4]
08023952  002a      cmp	r2, #0
08023954  7ff4f9ae  bne.w	#-526 ; -> 0x0802374a ; branch_target=0x0802374a
08023958  eee7      b	#-36 ; -> 0x08023938 ; branch_target=0x08023938
0802395a  054d      ldr	r5, [pc, #20] ; [0x08023970] = 0x58024800
0802395c  2b68      ldr	r3, [r5]
0802395e  43f48073  orr	r3, r3, #256
08023962  2b60      str	r3, [r5]
08023964  fcf71afd  bl	#-13772 ; -> 0x0802039c ; branch_target=0x0802039c
08023968  0646      mov	r6, r0
0802396a  08e0      b	#16 ; -> 0x0802397e ; branch_target=0x0802397e
08023974  fcf712fd  bl	#-13788 ; -> 0x0802039c ; branch_target=0x0802039c
08023978  801b      subs	r0, r0, r6
0802397a  6428      cmp	r0, #100
0802397c  3fd8      bhi	#126 ; -> 0x080239fe ; branch_target=0x080239fe
0802397e  2b68      ldr	r3, [r5]
08023980  da05      lsls	r2, r3, #23
08023982  f7d5      bpl	#-18 ; -> 0x08023974 ; branch_target=0x08023974
08023984  a368      ldr	r3, [r4, #8]
08023986  012b      cmp	r3, #1
08023988  00f06881  beq.w	#720 ; -> 0x08023c5c ; branch_target=0x08023c5c
0802398c  002b      cmp	r3, #0
0802398e  40f06b81  bne.w	#726 ; -> 0x08023c68 ; branch_target=0x08023c68
08023992  a64b      ldr	r3, [pc, #664] ; [0x08023c2c] = 0x58024400
08023994  1a6f      ldr	r2, [r3, #112]
08023996  22f00102  bic	r2, r2, #1
0802399a  1a67      str	r2, [r3, #112]
0802399c  1a6f      ldr	r2, [r3, #112]
0802399e  22f00402  bic	r2, r2, #4
080239a2  1a67      str	r2, [r3, #112]
080239a4  a368      ldr	r3, [r4, #8]
080239a6  002b      cmp	r3, #0
080239a8  00f02f81  beq.w	#606 ; -> 0x08023c0a ; branch_target=0x08023c0a
080239ac  fcf7f6fc  bl	#-13844 ; -> 0x0802039c ; branch_target=0x0802039c
080239b0  9e4e      ldr	r6, [pc, #632] ; [0x08023c2c] = 0x58024400
080239b2  41f28837  movw	r7, #5000
080239b6  0546      mov	r5, r0
080239b8  04e0      b	#8 ; -> 0x080239c4 ; branch_target=0x080239c4
080239ba  fcf7effc  bl	#-13858 ; -> 0x0802039c ; branch_target=0x0802039c
080239be  401b      subs	r0, r0, r5
080239c0  b842      cmp	r0, r7
080239c2  1cd8      bhi	#56 ; -> 0x080239fe ; branch_target=0x080239fe
080239c4  336f      ldr	r3, [r6, #112]
080239c6  9b07      lsls	r3, r3, #30
080239c8  f7d5      bpl	#-18 ; -> 0x080239ba ; branch_target=0x080239ba
080239ca  19e7      b	#-462 ; -> 0x08023800 ; branch_target=0x08023800
080239cc  974a      ldr	r2, [pc, #604] ; [0x08023c2c] = 0x58024400
080239ce  1368      ldr	r3, [r2]
080239d0  43f48033  orr	r3, r3, #65536
080239d4  1360      str	r3, [r2]
080239d6  a5e6      b	#-694 ; -> 0x08023724 ; branch_target=0x08023724
080239d8  fcf7f8fc  bl	#-13840 ; -> 0x080203cc ; branch_target=0x080203cc
080239dc  41f20303  movw	r3, #4099
080239e0  9842      cmp	r0, r3
080239e2  6cd8      bhi	#216 ; -> 0x08023abe ; branch_target=0x08023abe
080239e4  226a      ldr	r2, [r4, #32]
080239e6  202a      cmp	r2, #32
080239e8  00f04b81  beq.w	#662 ; -> 0x08023c82 ; branch_target=0x08023c82
080239ec  8f49      ldr	r1, [pc, #572] ; [0x08023c2c] = 0x58024400
080239ee  4b68      ldr	r3, [r1, #4]
080239f0  23f0f843  bic	r3, r3, #2080374784
080239f4  43ea8263  orr.w	r3, r3, r2, lsl #26
080239f8  4b60      str	r3, [r1, #4]
080239fa  2368      ldr	r3, [r4]
080239fc  cbe6      b	#-618 ; -> 0x08023796 ; branch_target=0x08023796
080239fe  0320      movs	r0, #3
08023a00  f8bd      pop	{r3, r4, r5, r6, r7, pc}
08023a02  fcf7cbfc  bl	#-13930 ; -> 0x0802039c ; branch_target=0x0802039c
08023a06  894e      ldr	r6, [pc, #548] ; [0x08023c2c] = 0x58024400
08023a08  0546      mov	r5, r0
08023a0a  04e0      b	#8 ; -> 0x08023a16 ; branch_target=0x08023a16
08023a0c  fcf7c6fc  bl	#-13940 ; -> 0x0802039c ; branch_target=0x0802039c
08023a10  401b      subs	r0, r0, r5
08023a12  6428      cmp	r0, #100
08023a14  f3d8      bhi	#-26 ; -> 0x080239fe ; branch_target=0x080239fe
08023a16  3368      ldr	r3, [r6]
08023a18  9803      lsls	r0, r3, #14
08023a1a  f7d4      bmi	#-18 ; -> 0x08023a0c ; branch_target=0x08023a0c
08023a1c  94e6      b	#-728 ; -> 0x08023748 ; branch_target=0x08023748
08023a1e  2b68      ldr	r3, [r5]
08023a20  23f48053  bic	r3, r3, #4096
08023a24  2b60      str	r3, [r5]
08023a26  fcf7b9fc  bl	#-13966 ; -> 0x0802039c ; branch_target=0x0802039c
08023a2a  0646      mov	r6, r0
08023a2c  04e0      b	#8 ; -> 0x08023a38 ; branch_target=0x08023a38
08023a2e  fcf7b5fc  bl	#-13974 ; -> 0x0802039c ; branch_target=0x0802039c
08023a32  801b      subs	r0, r0, r6
08023a34  0228      cmp	r0, #2
08023a36  e2d8      bhi	#-60 ; -> 0x080239fe ; branch_target=0x080239fe
08023a38  2b68      ldr	r3, [r5]
08023a3a  9804      lsls	r0, r3, #18
08023a3c  f7d4      bmi	#-18 ; -> 0x08023a2e ; branch_target=0x08023a2e
08023a3e  dbe6      b	#-586 ; -> 0x080237f8 ; branch_target=0x080237f8
08023a40  2b68      ldr	r3, [r5]
08023a42  23f08003  bic	r3, r3, #128
08023a46  2b60      str	r3, [r5]
08023a48  fcf7a8fc  bl	#-14000 ; -> 0x0802039c ; branch_target=0x0802039c
08023a4c  0646      mov	r6, r0
08023a4e  04e0      b	#8 ; -> 0x08023a5a ; branch_target=0x08023a5a
08023a50  fcf7a4fc  bl	#-14008 ; -> 0x0802039c ; branch_target=0x0802039c
08023a54  801b      subs	r0, r0, r6
08023a56  0228      cmp	r0, #2
08023a58  d1d8      bhi	#-94 ; -> 0x080239fe ; branch_target=0x080239fe
08023a5a  2b68      ldr	r3, [r5]
08023a5c  df05      lsls	r7, r3, #23
08023a5e  f7d4      bmi	#-18 ; -> 0x08023a50 ; branch_target=0x08023a50
08023a60  2368      ldr	r3, [r4]
08023a62  98e6      b	#-720 ; -> 0x08023796 ; branch_target=0x08023796
08023a64  b3f5a02f  cmp.w	r3, #327680
08023a68  704b      ldr	r3, [pc, #448] ; [0x08023c2c] = 0x58024400
08023a6a  1a68      ldr	r2, [r3]
08023a6c  7ff453ae  bne.w	#-858 ; -> 0x08023716 ; branch_target=0x08023716
08023a70  42f48022  orr	r2, r2, #262144
08023a74  1a60      str	r2, [r3]
08023a76  1a68      ldr	r2, [r3]
08023a78  42f48032  orr	r2, r2, #65536
08023a7c  1a60      str	r2, [r3]
08023a7e  51e6      b	#-862 ; -> 0x08023724 ; branch_target=0x08023724
08023a80  6a4a      ldr	r2, [pc, #424] ; [0x08023c2c] = 0x58024400
08023a82  2169      ldr	r1, [r4, #16]
08023a84  5368      ldr	r3, [r2, #4]
08023a86  23f0fe43  bic	r3, r3, #2130706432
08023a8a  43ea0163  orr.w	r3, r3, r1, lsl #24
08023a8e  5360      str	r3, [r2, #4]
08023a90  2368      ldr	r3, [r4]
08023a92  7ee6      b	#-772 ; -> 0x08023792 ; branch_target=0x08023792
08023a94  23f00103  bic	r3, r3, #1
08023a98  2b60      str	r3, [r5]
08023a9a  fcf77ffc  bl	#-14082 ; -> 0x0802039c ; branch_target=0x0802039c
08023a9e  0646      mov	r6, r0
08023aa0  04e0      b	#8 ; -> 0x08023aac ; branch_target=0x08023aac
08023aa2  fcf77bfc  bl	#-14090 ; -> 0x0802039c ; branch_target=0x0802039c
08023aa6  801b      subs	r0, r0, r6
08023aa8  0228      cmp	r0, #2
08023aaa  a8d8      bhi	#-176 ; -> 0x080239fe ; branch_target=0x080239fe
08023aac  2b68      ldr	r3, [r5]
08023aae  5807      lsls	r0, r3, #29
08023ab0  f7d4      bmi	#-18 ; -> 0x08023aa2 ; branch_target=0x08023aa2
08023ab2  2368      ldr	r3, [r4]
08023ab4  6de6      b	#-806 ; -> 0x08023792 ; branch_target=0x08023792
08023ab6  9207      lsls	r2, r2, #30
08023ab8  7ff4f7ae  bne.w	#-530 ; -> 0x080238aa ; branch_target=0x080238aa
08023abc  4ee6      b	#-868 ; -> 0x0802375c ; branch_target=0x0802375c
08023abe  5b4a      ldr	r2, [pc, #364] ; [0x08023c2c] = 0x58024400
08023ac0  216a      ldr	r1, [r4, #32]
08023ac2  d368      ldr	r3, [r2, #12]
08023ac4  23f07c53  bic	r3, r3, #1056964608
08023ac8  43ea0163  orr.w	r3, r3, r1, lsl #24
08023acc  d360      str	r3, [r2, #12]
08023ace  2368      ldr	r3, [r4]
08023ad0  61e6      b	#-830 ; -> 0x08023796 ; branch_target=0x08023796
08023ad2  fcf763fc  bl	#-14138 ; -> 0x0802039c ; branch_target=0x0802039c
08023ad6  0646      mov	r6, r0
08023ad8  04e0      b	#8 ; -> 0x08023ae4 ; branch_target=0x08023ae4
08023ada  fcf75ffc  bl	#-14146 ; -> 0x0802039c ; branch_target=0x0802039c
08023ade  801b      subs	r0, r0, r6
08023ae0  0228      cmp	r0, #2
08023ae2  8cd8      bhi	#-232 ; -> 0x080239fe ; branch_target=0x080239fe
08023ae4  2b68      ldr	r3, [r5]
08023ae6  9901      lsls	r1, r3, #6
08023ae8  f7d4      bmi	#-18 ; -> 0x08023ada ; branch_target=0x08023ada
08023aea  a96a      ldr	r1, [r5, #40]
08023aec  504b      ldr	r3, [pc, #320] ; [0x08023c30] = 0xfffffc0c
08023aee  a26a      ldr	r2, [r4, #40]
08023af0  0b40      ands	r3, r1
08023af2  5049      ldr	r1, [pc, #320] ; [0x08023c34] = 0xffff0007
08023af4  4d4e      ldr	r6, [pc, #308] ; [0x08023c2c] = 0x58024400
08023af6  1343      orrs	r3, r2
08023af8  e26a      ldr	r2, [r4, #44]
08023afa  43ea0213  orr.w	r3, r3, r2, lsl #4
08023afe  ab62      str	r3, [r5, #40]
08023b00  d4e90d32  ldrd	r3, r2, [r4, #52]
08023b04  013b      subs	r3, #1
08023b06  013a      subs	r2, #1
08023b08  5b02      lsls	r3, r3, #9
08023b0a  1204      lsls	r2, r2, #16
08023b0c  9bb2      uxth	r3, r3
08023b0e  02f4fe02  and	r2, r2, #8323072
08023b12  1343      orrs	r3, r2
08023b14  226b      ldr	r2, [r4, #48]
08023b16  013a      subs	r2, #1
08023b18  c2f30802  ubfx	r2, r2, #0, #9
08023b1c  1343      orrs	r3, r2
08023b1e  e26b      ldr	r2, [r4, #60]
08023b20  013a      subs	r2, #1
08023b22  1206      lsls	r2, r2, #24
08023b24  02f0fe42  and	r2, r2, #2130706432
08023b28  1343      orrs	r3, r2
08023b2a  2b63      str	r3, [r5, #48]
08023b2c  eb6a      ldr	r3, [r5, #44]
08023b2e  23f00103  bic	r3, r3, #1
08023b32  eb62      str	r3, [r5, #44]
08023b34  6a6b      ldr	r2, [r5, #52]
08023b36  a36c      ldr	r3, [r4, #72]
08023b38  1140      ands	r1, r2
08023b3a  41eac301  orr.w	r1, r1, r3, lsl #3
08023b3e  6963      str	r1, [r5, #52]
08023b40  eb6a      ldr	r3, [r5, #44]
08023b42  226c      ldr	r2, [r4, #64]
08023b44  23f00c03  bic	r3, r3, #12
08023b48  1343      orrs	r3, r2
08023b4a  eb62      str	r3, [r5, #44]
08023b4c  eb6a      ldr	r3, [r5, #44]
08023b4e  626c      ldr	r2, [r4, #68]
08023b50  23f00203  bic	r3, r3, #2
08023b54  1343      orrs	r3, r2
08023b56  eb62      str	r3, [r5, #44]
08023b58  eb6a      ldr	r3, [r5, #44]
08023b5a  43f48033  orr	r3, r3, #65536
08023b5e  eb62      str	r3, [r5, #44]
08023b60  eb6a      ldr	r3, [r5, #44]
08023b62  43f40033  orr	r3, r3, #131072
08023b66  eb62      str	r3, [r5, #44]
08023b68  eb6a      ldr	r3, [r5, #44]
08023b6a  43f48023  orr	r3, r3, #262144
08023b6e  eb62      str	r3, [r5, #44]
08023b70  eb6a      ldr	r3, [r5, #44]
08023b72  43f00103  orr	r3, r3, #1
08023b76  eb62      str	r3, [r5, #44]
08023b78  2b68      ldr	r3, [r5]
08023b7a  43f08073  orr	r3, r3, #16777216
08023b7e  2b60      str	r3, [r5]
08023b80  fcf70cfc  bl	#-14312 ; -> 0x0802039c ; branch_target=0x0802039c
08023b84  0446      mov	r4, r0
08023b86  05e0      b	#10 ; -> 0x08023b94 ; branch_target=0x08023b94
08023b88  fcf708fc  bl	#-14320 ; -> 0x0802039c ; branch_target=0x0802039c
08023b8c  001b      subs	r0, r0, r4
08023b8e  0228      cmp	r0, #2
08023b90  3ff635af  bhi.w	#-406 ; -> 0x080239fe ; branch_target=0x080239fe
08023b94  3368      ldr	r3, [r6]
08023b96  9a01      lsls	r2, r3, #6
08023b98  f6d5      bpl	#-20 ; -> 0x08023b88 ; branch_target=0x08023b88
08023b9a  4ee6      b	#-868 ; -> 0x0802383a ; branch_target=0x0802383a
08023b9c  012a      cmp	r2, #1
08023b9e  a96a      ldr	r1, [r5, #40]
08023ba0  2b6b      ldr	r3, [r5, #48]
08023ba2  3ff4c9ae  beq.w	#-622 ; -> 0x08023938 ; branch_target=0x08023938
08023ba6  01f00302  and	r2, r1, #3
08023baa  a06a      ldr	r0, [r4, #40]
08023bac  8242      cmp	r2, r0
08023bae  7ff4c3ae  bne.w	#-634 ; -> 0x08023938 ; branch_target=0x08023938
08023bb2  c1f30511  ubfx	r1, r1, #4, #6
08023bb6  e26a      ldr	r2, [r4, #44]
08023bb8  9142      cmp	r1, r2
08023bba  7ff4bdae  bne.w	#-646 ; -> 0x08023938 ; branch_target=0x08023938
08023bbe  226b      ldr	r2, [r4, #48]
08023bc0  c3f30801  ubfx	r1, r3, #0, #9
08023bc4  013a      subs	r2, #1
08023bc6  9142      cmp	r1, r2
08023bc8  7ff4b6ae  bne.w	#-660 ; -> 0x08023938 ; branch_target=0x08023938
08023bcc  626b      ldr	r2, [r4, #52]
08023bce  c3f34621  ubfx	r1, r3, #9, #7
08023bd2  013a      subs	r2, #1
08023bd4  9142      cmp	r1, r2
08023bd6  7ff4afae  bne.w	#-674 ; -> 0x08023938 ; branch_target=0x08023938
08023bda  a26b      ldr	r2, [r4, #56]
08023bdc  c3f30641  ubfx	r1, r3, #16, #7
08023be0  013a      subs	r2, #1
08023be2  9142      cmp	r1, r2
08023be4  7ff4a8ae  bne.w	#-688 ; -> 0x08023938 ; branch_target=0x08023938
08023be8  e26b      ldr	r2, [r4, #60]
08023bea  c3f30663  ubfx	r3, r3, #24, #7
08023bee  013a      subs	r2, #1
08023bf0  981a      subs	r0, r3, r2
08023bf2  18bf      it	ne
08023bf4  0120      movne	r0, #1
08023bf6  f8bd      pop	{r3, r4, r5, r6, r7, pc}
08023bf8  0c4a      ldr	r2, [pc, #48] ; [0x08023c2c] = 0x58024400
08023bfa  5368      ldr	r3, [r2, #4]
08023bfc  23f47c33  bic	r3, r3, #258048
08023c00  43f40033  orr	r3, r3, #131072
08023c04  5360      str	r3, [r2, #4]
08023c06  2368      ldr	r3, [r4]
08023c08  c3e5      b	#-1146 ; -> 0x08023792 ; branch_target=0x08023792
08023c0a  fcf7c7fb  bl	#-14450 ; -> 0x0802039c ; branch_target=0x0802039c
08023c0e  074e      ldr	r6, [pc, #28] ; [0x08023c2c] = 0x58024400
08023c10  41f28837  movw	r7, #5000
08023c14  0546      mov	r5, r0
08023c16  05e0      b	#10 ; -> 0x08023c24 ; branch_target=0x08023c24
08023c18  fcf7c0fb  bl	#-14464 ; -> 0x0802039c ; branch_target=0x0802039c
08023c1c  401b      subs	r0, r0, r5
08023c1e  b842      cmp	r0, r7
08023c20  3ff6edae  bhi.w	#-550 ; -> 0x080239fe ; branch_target=0x080239fe
08023c24  336f      ldr	r3, [r6, #112]
08023c26  9807      lsls	r0, r3, #30
08023c28  f6d4      bmi	#-20 ; -> 0x08023c18 ; branch_target=0x08023c18
08023c2a  e9e5      b	#-1070 ; -> 0x08023800 ; branch_target=0x08023800
08023c38  eb68      ldr	r3, [r5, #12]
08023c3a  226a      ldr	r2, [r4, #32]
08023c3c  23f07c53  bic	r3, r3, #1056964608
08023c40  43ea0263  orr.w	r3, r3, r2, lsl #24
08023c44  eb60      str	r3, [r5, #12]
08023c46  2368      ldr	r3, [r4]
08023c48  a5e5      b	#-1206 ; -> 0x08023796 ; branch_target=0x08023796
08023c4a  6b68      ldr	r3, [r5, #4]
08023c4c  2269      ldr	r2, [r4, #16]
08023c4e  23f0fe43  bic	r3, r3, #2130706432
08023c52  43ea0263  orr.w	r3, r3, r2, lsl #24
08023c56  6b60      str	r3, [r5, #4]
08023c58  2368      ldr	r3, [r4]
08023c5a  9ae5      b	#-1228 ; -> 0x08023792 ; branch_target=0x08023792
08023c5c  0e4a      ldr	r2, [pc, #56] ; [0x08023c98] = 0x58024400
08023c5e  136f      ldr	r3, [r2, #112]
08023c60  43f00103  orr	r3, r3, #1
08023c64  1367      str	r3, [r2, #112]
08023c66  9de6      b	#-710 ; -> 0x080239a4 ; branch_target=0x080239a4
08023c68  052b      cmp	r3, #5
08023c6a  0b4b      ldr	r3, [pc, #44] ; [0x08023c98] = 0x58024400
08023c6c  1a6f      ldr	r2, [r3, #112]
08023c6e  7ff492ae  bne.w	#-732 ; -> 0x08023996 ; branch_target=0x08023996
08023c72  42f00402  orr	r2, r2, #4
08023c76  1a67      str	r2, [r3, #112]
08023c78  1a6f      ldr	r2, [r3, #112]
08023c7a  42f00102  orr	r2, r2, #1
08023c7e  1a67      str	r2, [r3, #112]
08023c80  90e6      b	#-736 ; -> 0x080239a4 ; branch_target=0x080239a4
08023c82  054a      ldr	r2, [pc, #20] ; [0x08023c98] = 0x58024400
08023c84  5368      ldr	r3, [r2, #4]
08023c86  23f0f843  bic	r3, r3, #2080374784
08023c8a  43f08043  orr	r3, r3, #1073741824
08023c8e  5360      str	r3, [r2, #4]
08023c90  2368      ldr	r3, [r4]
08023c92  80e5      b	#-1280 ; -> 0x08023796 ; branch_target=0x08023796
08023c94  0120      movs	r0, #1
08023c96  7047      bx	lr
