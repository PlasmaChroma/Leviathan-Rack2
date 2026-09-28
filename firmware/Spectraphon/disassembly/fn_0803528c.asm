; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
0803528c  f0b5      push	{r4, r5, r6, r7, lr}
0803528e  0b4a      ldr	r2, [pc, #44] ; [0x080352bc] = 0x40015804 / f32_bits_interpretation=2.020997047
08035290  8fb0      sub	sp, #60
08035292  0368      ldr	r3, [r0]
08035294  0446      mov	r4, r0
08035296  9342      cmp	r3, r2
08035298  1ad0      beq	#52 ; -> 0x080352d0 ; branch_target=0x080352d0
0803529a  094a      ldr	r2, [pc, #36] ; [0x080352c0] = 0x40015824 / f32_bits_interpretation=2.021004677
0803529c  9342      cmp	r3, r2
0803529e  65d0      beq	#202 ; -> 0x0803536c ; branch_target=0x0803536c
080352a0  084a      ldr	r2, [pc, #32] ; [0x080352c4] = 0x40015c04 / f32_bits_interpretation=2.021241188
080352a2  9342      cmp	r3, r2
080352a4  00f09f80  beq.w	#318 ; -> 0x080353e6 ; branch_target=0x080353e6
080352a8  074a      ldr	r2, [pc, #28] ; [0x080352c8] = 0x40015c24 / f32_bits_interpretation=2.021248817
080352aa  9342      cmp	r3, r2
080352ac  00f0ea80  beq.w	#468 ; -> 0x08035484 ; branch_target=0x08035484
080352b0  064a      ldr	r2, [pc, #24] ; [0x080352cc] = 0x58005404
080352b2  9342      cmp	r3, r2
080352b4  00f03381  beq.w	#614 ; -> 0x0803551e ; branch_target=0x0803551e
080352b8  0fb0      add	sp, #60
080352ba  f0bd      pop	{r4, r5, r6, r7, pc}
080352d0  b54b      ldr	r3, [pc, #724] ; [0x080355a8] = 0x200144f4
080352d2  1a68      ldr	r2, [r3]
080352d4  002a      cmp	r2, #0
080352d6  00f0bd81  beq.w	#890 ; -> 0x08035654 ; branch_target=0x08035654
080352da  0132      adds	r2, #1
080352dc  0026      movs	r6, #0
080352de  0221      movs	r1, #2
080352e0  7020      movs	r0, #112
080352e2  1a60      str	r2, [r3]
080352e4  0623      movs	r3, #6
080352e6  0990      str	r0, [sp, #36]
080352e8  0c91      str	r1, [sp, #48]
080352ea  b048      ldr	r0, [pc, #704] ; [0x080355ac] = 0x58021000
080352ec  b04d      ldr	r5, [pc, #704] ; [0x080355b0] = 0x200146d8
080352ee  0d93      str	r3, [sp, #52]
080352f0  cde90a16  strd	r1, r6, [sp, #40]
080352f4  09a9      add	r1, sp, #36
080352f6  edf74fff  bl	#-74082 ; -> 0x08023198 ; branch_target=0x08023198
080352fa  ae4a      ldr	r2, [pc, #696] ; [0x080355b4] = 0x40020028 / f32_bits_interpretation=2.031259537
080352fc  5723      movs	r3, #87
080352fe  2846      mov	r0, r5
08035300  2a60      str	r2, [r5]
08035302  4022      movs	r2, #64
08035304  6b60      str	r3, [r5, #4]
08035306  4ff48063  mov.w	r3, #1024
0803530a  aa60      str	r2, [r5, #8]
0803530c  4ff48052  mov.w	r2, #4096
08035310  2b61      str	r3, [r5, #16]
08035312  4ff48043  mov.w	r3, #16384
08035316  ee60      str	r6, [r5, #12]
08035318  ae62      str	r6, [r5, #40]
0803531a  ee62      str	r6, [r5, #44]
0803531c  2e63      str	r6, [r5, #48]
0803531e  c5e90523  strd	r2, r3, [r5, #20]
08035322  4ff44033  mov.w	r3, #196608
08035326  4ff48072  mov.w	r2, #256
0803532a  c5e90723  strd	r2, r3, [r5, #28]
0803532e  0423      movs	r3, #4
08035330  6b62      str	r3, [r5, #36]
08035332  ecf76bfa  bl	#-80682 ; -> 0x0802180c ; branch_target=0x0802180c
08035336  0028      cmp	r0, #0
08035338  40f0a381  bne.w	#838 ; -> 0x08035682 ; branch_target=0x08035682
0803533c  0621      movs	r1, #6
0803533e  0023      movs	r3, #0
08035340  4ff48072  mov.w	r2, #256
08035344  9a48      ldr	r0, [pc, #616] ; [0x080355b0] = 0x200146d8
08035346  cde90513  strd	r1, r3, [sp, #20]
0803534a  0123      movs	r3, #1
0803534c  05a9      add	r1, sp, #20
0803534e  adf81c20  strh.w	r2, [sp, #28]
08035352  0893      str	r3, [sp, #32]
08035354  edf736fc  bl	#-75668 ; -> 0x08022bc4 ; branch_target=0x08022bc4
08035358  0028      cmp	r0, #0
0803535a  40f08f81  bne.w	#798 ; -> 0x0803567c ; branch_target=0x0803567c
0803535e  964a      ldr	r2, [pc, #600] ; [0x080355b8] = 0x40015824 / f32_bits_interpretation=2.021004677
08035360  ac63      str	r4, [r5, #56]
08035362  2368      ldr	r3, [r4]
08035364  9342      cmp	r3, r2
08035366  c4e92055  strd	r5, r5, [r4, #128]
0803536a  99d1      bne	#-206 ; -> 0x080352a0 ; branch_target=0x080352a0
0803536c  8e4b      ldr	r3, [pc, #568] ; [0x080355a8] = 0x200144f4
0803536e  1a68      ldr	r2, [r3]
08035370  002a      cmp	r2, #0
08035372  00f00b81  beq.w	#534 ; -> 0x0803558c ; branch_target=0x0803558c
08035376  0132      adds	r2, #1
08035378  0026      movs	r6, #0
0803537a  0221      movs	r1, #2
0803537c  0820      movs	r0, #8
0803537e  1a60      str	r2, [r3]
08035380  0623      movs	r3, #6
08035382  0990      str	r0, [sp, #36]
08035384  0c91      str	r1, [sp, #48]
08035386  8948      ldr	r0, [pc, #548] ; [0x080355ac] = 0x58021000
08035388  8c4d      ldr	r5, [pc, #560] ; [0x080355bc] = 0x20014660
0803538a  0d93      str	r3, [sp, #52]
0803538c  cde90a16  strd	r1, r6, [sp, #40]
08035390  09a9      add	r1, sp, #36
08035392  edf701ff  bl	#-74238 ; -> 0x08023198 ; branch_target=0x08023198
08035396  8a4a      ldr	r2, [pc, #552] ; [0x080355c0] = 0x40020040 / f32_bits_interpretation=2.031265259
08035398  5823      movs	r3, #88
0803539a  2846      mov	r0, r5
0803539c  2a60      str	r2, [r5]
0803539e  4ff48062  mov.w	r2, #1024
080353a2  6b60      str	r3, [r5, #4]
080353a4  4ff48053  mov.w	r3, #4096
080353a8  2a61      str	r2, [r5, #16]
080353aa  4ff48042  mov.w	r2, #16384
080353ae  6b61      str	r3, [r5, #20]
080353b0  4ff48073  mov.w	r3, #256
080353b4  ae60      str	r6, [r5, #8]
080353b6  ee60      str	r6, [r5, #12]
080353b8  ae62      str	r6, [r5, #40]
080353ba  ee62      str	r6, [r5, #44]
080353bc  2e63      str	r6, [r5, #48]
080353be  c5e90623  strd	r2, r3, [r5, #24]
080353c2  4ff44032  mov.w	r2, #196608
080353c6  0423      movs	r3, #4
080353c8  c5e90823  strd	r2, r3, [r5, #32]
080353cc  ecf71efa  bl	#-80836 ; -> 0x0802180c ; branch_target=0x0802180c
080353d0  0028      cmp	r0, #0
080353d2  40f05081  bne.w	#672 ; -> 0x08035676 ; branch_target=0x08035676
080353d6  7b4a      ldr	r2, [pc, #492] ; [0x080355c4] = 0x40015c04 / f32_bits_interpretation=2.021241188
080353d8  ac63      str	r4, [r5, #56]
080353da  2368      ldr	r3, [r4]
080353dc  9342      cmp	r3, r2
080353de  c4e92055  strd	r5, r5, [r4, #128]
080353e2  7ff461af  bne.w	#-318 ; -> 0x080352a8 ; branch_target=0x080352a8
080353e6  784b      ldr	r3, [pc, #480] ; [0x080355c8] = 0x200144f0
080353e8  1a68      ldr	r2, [r3]
080353ea  002a      cmp	r2, #0
080353ec  00f00881  beq.w	#528 ; -> 0x08035600 ; branch_target=0x08035600
080353f0  4ff40061  mov.w	r1, #2048
080353f4  0132      adds	r2, #1
080353f6  0026      movs	r6, #0
080353f8  7448      ldr	r0, [pc, #464] ; [0x080355cc] = 0x58020c00
080353fa  0991      str	r1, [sp, #36]
080353fc  0221      movs	r1, #2
080353fe  1a60      str	r2, [r3]
08035400  0122      movs	r2, #1
08035402  0a23      movs	r3, #10
08035404  0a91      str	r1, [sp, #40]
08035406  09a9      add	r1, sp, #36
08035408  714d      ldr	r5, [pc, #452] ; [0x080355d0] = 0x200145e8
0803540a  0c92      str	r2, [sp, #48]
0803540c  0d93      str	r3, [sp, #52]
0803540e  0b96      str	r6, [sp, #44]
08035410  edf7c2fe  bl	#-74364 ; -> 0x08023198 ; branch_target=0x08023198
08035414  6f4a      ldr	r2, [pc, #444] ; [0x080355d4] = 0x40020088 / f32_bits_interpretation=2.031282425
08035416  5923      movs	r3, #89
08035418  2846      mov	r0, r5
0803541a  ee60      str	r6, [r5, #12]
0803541c  2a60      str	r2, [r5]
0803541e  4022      movs	r2, #64
08035420  6b60      str	r3, [r5, #4]
08035422  4ff48063  mov.w	r3, #1024
08035426  aa60      str	r2, [r5, #8]
08035428  4ff48052  mov.w	r2, #4096
0803542c  2b61      str	r3, [r5, #16]
0803542e  4ff48043  mov.w	r3, #16384
08035432  ae62      str	r6, [r5, #40]
08035434  ee62      str	r6, [r5, #44]
08035436  2e63      str	r6, [r5, #48]
08035438  c5e90523  strd	r2, r3, [r5, #20]
0803543c  4ff40033  mov.w	r3, #131072
08035440  4ff48072  mov.w	r2, #256
08035444  c5e90723  strd	r2, r3, [r5, #28]
08035448  0423      movs	r3, #4
0803544a  6b62      str	r3, [r5, #36]
0803544c  ecf7def9  bl	#-80964 ; -> 0x0802180c ; branch_target=0x0802180c
08035450  0028      cmp	r0, #0
08035452  40f01c81  bne.w	#568 ; -> 0x0803568e ; branch_target=0x0803568e
08035456  0123      movs	r3, #1
08035458  4ff40032  mov.w	r2, #131072
0803545c  05a9      add	r1, sp, #20
0803545e  5c48      ldr	r0, [pc, #368] ; [0x080355d0] = 0x200145e8
08035460  adf81c30  strh.w	r3, [sp, #28]
08035464  0893      str	r3, [sp, #32]
08035466  cde90532  strd	r3, r2, [sp, #20]
0803546a  edf7abfb  bl	#-75946 ; -> 0x08022bc4 ; branch_target=0x08022bc4
0803546e  0028      cmp	r0, #0
08035470  40f00a81  bne.w	#532 ; -> 0x08035688 ; branch_target=0x08035688
08035474  584a      ldr	r2, [pc, #352] ; [0x080355d8] = 0x40015c24 / f32_bits_interpretation=2.021248817
08035476  ac63      str	r4, [r5, #56]
08035478  2368      ldr	r3, [r4]
0803547a  9342      cmp	r3, r2
0803547c  c4e92055  strd	r5, r5, [r4, #128]
08035480  7ff416af  bne.w	#-468 ; -> 0x080352b0 ; branch_target=0x080352b0
08035484  504b      ldr	r3, [pc, #320] ; [0x080355c8] = 0x200144f0
08035486  1a68      ldr	r2, [r3]
08035488  002a      cmp	r2, #0
0803548a  00f0c780  beq.w	#398 ; -> 0x0803561c ; branch_target=0x0803561c
0803548e  0132      adds	r2, #1
08035490  0026      movs	r6, #0
08035492  4ff48067  mov.w	r7, #1024
08035496  5148      ldr	r0, [pc, #324] ; [0x080355dc] = 0x58021800
08035498  1a60      str	r2, [r3]
0803549a  0123      movs	r3, #1
0803549c  0222      movs	r2, #2
0803549e  09a9      add	r1, sp, #36
080354a0  0c93      str	r3, [sp, #48]
080354a2  0a23      movs	r3, #10
080354a4  4e4d      ldr	r5, [pc, #312] ; [0x080355e0] = 0x20014570
080354a6  0a92      str	r2, [sp, #40]
080354a8  0d93      str	r3, [sp, #52]
080354aa  0b96      str	r6, [sp, #44]
080354ac  0997      str	r7, [sp, #36]
080354ae  edf773fe  bl	#-74522 ; -> 0x08023198 ; branch_target=0x08023198
080354b2  4c4a      ldr	r2, [pc, #304] ; [0x080355e4] = 0x400200a0 / f32_bits_interpretation=2.031288147
080354b4  4ff48053  mov.w	r3, #4096
080354b8  2846      mov	r0, r5
080354ba  ae62      str	r6, [r5, #40]
080354bc  2a60      str	r2, [r5]
080354be  5a22      movs	r2, #90
080354c0  6b61      str	r3, [r5, #20]
080354c2  4ff48043  mov.w	r3, #16384
080354c6  6a60      str	r2, [r5, #4]
080354c8  4022      movs	r2, #64
080354ca  ab61      str	r3, [r5, #24]
080354cc  4ff48073  mov.w	r3, #256
080354d0  aa60      str	r2, [r5, #8]
080354d2  4ff40032  mov.w	r2, #131072
080354d6  eb61      str	r3, [r5, #28]
080354d8  0423      movs	r3, #4
080354da  ee62      str	r6, [r5, #44]
080354dc  2e63      str	r6, [r5, #48]
080354de  c5e90367  strd	r6, r7, [r5, #12]
080354e2  c5e90823  strd	r2, r3, [r5, #32]
080354e6  ecf791f9  bl	#-81118 ; -> 0x0802180c ; branch_target=0x0802180c
080354ea  0028      cmp	r0, #0
080354ec  40f0d580  bne.w	#426 ; -> 0x0803569a ; branch_target=0x0803569a
080354f0  0123      movs	r3, #1
080354f2  4ff40032  mov.w	r2, #131072
080354f6  05a9      add	r1, sp, #20
080354f8  3948      ldr	r0, [pc, #228] ; [0x080355e0] = 0x20014570
080354fa  adf81c30  strh.w	r3, [sp, #28]
080354fe  0893      str	r3, [sp, #32]
08035500  cde90532  strd	r3, r2, [sp, #20]
08035504  edf75efb  bl	#-76100 ; -> 0x08022bc4 ; branch_target=0x08022bc4
08035508  0028      cmp	r0, #0
0803550a  40f0c380  bne.w	#390 ; -> 0x08035694 ; branch_target=0x08035694
0803550e  364a      ldr	r2, [pc, #216] ; [0x080355e8] = 0x58005404
08035510  ac63      str	r4, [r5, #56]
08035512  2368      ldr	r3, [r4]
08035514  9342      cmp	r3, r2
08035516  c4e92055  strd	r5, r5, [r4, #128]
0803551a  7ff4cdae  bne.w	#-614 ; -> 0x080352b8 ; branch_target=0x080352b8
0803551e  3349      ldr	r1, [pc, #204] ; [0x080355ec] = 0x200144ec
08035520  0b68      ldr	r3, [r1]
08035522  002b      cmp	r3, #0
08035524  00f08880  beq.w	#272 ; -> 0x08035638 ; branch_target=0x08035638
08035528  0133      adds	r3, #1
0803552a  0222      movs	r2, #2
0803552c  0026      movs	r6, #0
0803552e  3048      ldr	r0, [pc, #192] ; [0x080355f0] = 0x58020800
08035530  0b60      str	r3, [r1]
08035532  0823      movs	r3, #8
08035534  09a9      add	r1, sp, #36
08035536  2f4d      ldr	r5, [pc, #188] ; [0x080355f4] = 0x200144f8
08035538  0d93      str	r3, [sp, #52]
0803553a  cde90922  strd	r2, r2, [sp, #36]
0803553e  0122      movs	r2, #1
08035540  cde90b62  strd	r6, r2, [sp, #44]
08035544  edf728fe  bl	#-74672 ; -> 0x08023198 ; branch_target=0x08023198
08035548  2b49      ldr	r1, [pc, #172] ; [0x080355f8] = 0x58025408
0803554a  0f22      movs	r2, #15
0803554c  4ff48063  mov.w	r3, #1024
08035550  2846      mov	r0, r5
08035552  ee60      str	r6, [r5, #12]
08035554  2b61      str	r3, [r5, #16]
08035556  4ff48053  mov.w	r3, #4096
0803555a  c5e90012  strd	r1, r2, [r5]
0803555e  4022      movs	r2, #64
08035560  4ff48041  mov.w	r1, #16384
08035564  6b61      str	r3, [r5, #20]
08035566  4ff40033  mov.w	r3, #131072
0803556a  aa60      str	r2, [r5, #8]
0803556c  4ff48072  mov.w	r2, #256
08035570  2b62      str	r3, [r5, #32]
08035572  c5e90612  strd	r1, r2, [r5, #24]
08035576  ecf749f9  bl	#-81262 ; -> 0x0802180c ; branch_target=0x0802180c
0803557a  0028      cmp	r0, #0
0803557c  78d1      bne	#240 ; -> 0x08035670 ; branch_target=0x08035670
0803557e  c4f88450  str.w	r5, [r4, #132]
08035582  ac63      str	r4, [r5, #56]
08035584  c4f88050  str.w	r5, [r4, #128]
08035588  0fb0      add	sp, #60
0803558a  f0bd      pop	{r4, r5, r6, r7, pc}
0803558c  1b49      ldr	r1, [pc, #108] ; [0x080355fc] = 0x58024400
0803558e  d1f8f000  ldr.w	r0, [r1, #240]
08035592  40f48000  orr	r0, r0, #4194304
08035596  c1f8f000  str.w	r0, [r1, #240]
0803559a  d1f8f010  ldr.w	r1, [r1, #240]
0803559e  01f48001  and	r1, r1, #4194304
080355a2  0191      str	r1, [sp, #4]
080355a4  0199      ldr	r1, [sp, #4]
080355a6  e6e6      b	#-564 ; -> 0x08035376 ; branch_target=0x08035376
08035600  2749      ldr	r1, [pc, #156] ; [0x080356a0] = 0x58024400
08035602  d1f8f000  ldr.w	r0, [r1, #240]
08035606  40f40000  orr	r0, r0, #8388608
0803560a  c1f8f000  str.w	r0, [r1, #240]
0803560e  d1f8f010  ldr.w	r1, [r1, #240]
08035612  01f40001  and	r1, r1, #8388608
08035616  0291      str	r1, [sp, #8]
08035618  0299      ldr	r1, [sp, #8]
0803561a  e9e6      b	#-558 ; -> 0x080353f0 ; branch_target=0x080353f0
0803561c  2049      ldr	r1, [pc, #128] ; [0x080356a0] = 0x58024400
0803561e  d1f8f000  ldr.w	r0, [r1, #240]
08035622  40f40000  orr	r0, r0, #8388608
08035626  c1f8f000  str.w	r0, [r1, #240]
0803562a  d1f8f010  ldr.w	r1, [r1, #240]
0803562e  01f40001  and	r1, r1, #8388608
08035632  0391      str	r1, [sp, #12]
08035634  0399      ldr	r1, [sp, #12]
08035636  2ae7      b	#-428 ; -> 0x0803548e ; branch_target=0x0803548e
08035638  194a      ldr	r2, [pc, #100] ; [0x080356a0] = 0x58024400
0803563a  d2f8f400  ldr.w	r0, [r2, #244]
0803563e  40f40010  orr	r0, r0, #2097152
08035642  c2f8f400  str.w	r0, [r2, #244]
08035646  d2f8f420  ldr.w	r2, [r2, #244]
0803564a  02f40012  and	r2, r2, #2097152
0803564e  0492      str	r2, [sp, #16]
08035650  049a      ldr	r2, [sp, #16]
08035652  69e7      b	#-302 ; -> 0x08035528 ; branch_target=0x08035528
08035654  1249      ldr	r1, [pc, #72] ; [0x080356a0] = 0x58024400
08035656  d1f8f000  ldr.w	r0, [r1, #240]
0803565a  40f48000  orr	r0, r0, #4194304
0803565e  c1f8f000  str.w	r0, [r1, #240]
08035662  d1f8f010  ldr.w	r1, [r1, #240]
08035666  01f48001  and	r1, r1, #4194304
0803566a  0091      str	r1, [sp]
0803566c  0099      ldr	r1, [sp]
0803566e  34e6      b	#-920 ; -> 0x080352da ; branch_target=0x080352da
08035670  fff746fd  bl	#-1396 ; -> 0x08035100 ; branch_target=0x08035100
08035674  83e7      b	#-250 ; -> 0x0803557e ; branch_target=0x0803557e
08035676  fff743fd  bl	#-1402 ; -> 0x08035100 ; branch_target=0x08035100
0803567a  ace6      b	#-680 ; -> 0x080353d6 ; branch_target=0x080353d6
0803567c  fff740fd  bl	#-1408 ; -> 0x08035100 ; branch_target=0x08035100
08035680  6de6      b	#-806 ; -> 0x0803535e ; branch_target=0x0803535e
08035682  fff73dfd  bl	#-1414 ; -> 0x08035100 ; branch_target=0x08035100
08035686  59e6      b	#-846 ; -> 0x0803533c ; branch_target=0x0803533c
08035688  fff73afd  bl	#-1420 ; -> 0x08035100 ; branch_target=0x08035100
0803568c  f2e6      b	#-540 ; -> 0x08035474 ; branch_target=0x08035474
0803568e  fff737fd  bl	#-1426 ; -> 0x08035100 ; branch_target=0x08035100
08035692  e0e6      b	#-576 ; -> 0x08035456 ; branch_target=0x08035456
08035694  fff734fd  bl	#-1432 ; -> 0x08035100 ; branch_target=0x08035100
08035698  39e7      b	#-398 ; -> 0x0803550e ; branch_target=0x0803550e
0803569a  fff731fd  bl	#-1438 ; -> 0x08035100 ; branch_target=0x08035100
0803569e  27e7      b	#-434 ; -> 0x080354f0 ; branch_target=0x080354f0
