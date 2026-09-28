; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08027a90  0028      cmp	r0, #0
08027a92  00f08b80  beq.w	#278 ; -> 0x08027bac ; branch_target=0x08027bac
08027a96  70b5      push	{r4, r5, r6, lr}
08027a98  90f83d30  ldrb.w	r3, [r0, #61]
08027a9c  0446      mov	r4, r0
08027a9e  03f0ff02  and	r2, r3, #255
08027aa2  002b      cmp	r3, #0
08027aa4  70d0      beq	#224 ; -> 0x08027b88 ; branch_target=0x08027b88
08027aa6  2268      ldr	r2, [r4]
08027aa8  0223      movs	r3, #2
08027aaa  4148      ldr	r0, [pc, #260] ; [0x08027bb0] = 0x40010000 / f32_bits_interpretation=2.015625
08027aac  414d      ldr	r5, [pc, #260] ; [0x08027bb4] = 0x40010400 / f32_bits_interpretation=2.015869141
08027aae  b2f1804f  cmp.w	r2, #1073741824
08027ab2  a2eb0000  sub.w	r0, r2, r0
08027ab6  84f83d30  strb.w	r3, [r4, #61]
08027aba  a2eb0505  sub.w	r5, r2, r5
08027abe  1368      ldr	r3, [r2]
08027ac0  b0fa80f0  clz	r0, r0
08027ac4  a169      ldr	r1, [r4, #24]
08027ac6  b5fa85f5  clz	r5, r5
08027aca  4fea5010  lsr.w	r0, r0, #5
08027ace  4fea5515  lsr.w	r5, r5, #5
08027ad2  1fd0      beq	#62 ; -> 0x08027b14 ; branch_target=0x08027b14
08027ad4  f0b9      cbnz	r0, #60 ; -> 0x08027b14 ; branch_target=0x08027b14
08027ad6  384e      ldr	r6, [pc, #224] ; [0x08027bb8] = 0x40000400 / f32_bits_interpretation=2.000244141
08027ad8  b242      cmp	r2, r6
08027ada  1bd0      beq	#54 ; -> 0x08027b14 ; branch_target=0x08027b14
08027adc  06f58066  add.w	r6, r6, #1024
08027ae0  b242      cmp	r2, r6
08027ae2  17d0      beq	#46 ; -> 0x08027b14 ; branch_target=0x08027b14
08027ae4  06f58066  add.w	r6, r6, #1024
08027ae8  b242      cmp	r2, r6
08027aea  13d0      beq	#38 ; -> 0x08027b14 ; branch_target=0x08027b14
08027aec  95b9      cbnz	r5, #36 ; -> 0x08027b14 ; branch_target=0x08027b14
08027aee  334d      ldr	r5, [pc, #204] ; [0x08027bbc] = 0x40014000 / f32_bits_interpretation=2.01953125
08027af0  3348      ldr	r0, [pc, #204] ; [0x08027bc0] = 0x40014400 / f32_bits_interpretation=2.019775391
08027af2  8242      cmp	r2, r0
08027af4  18bf      it	ne
08027af6  aa42      cmpne	r2, r5
08027af8  4bd0      beq	#150 ; -> 0x08027b92 ; branch_target=0x08027b92
08027afa  00f58060  add.w	r0, r0, #1024
08027afe  8242      cmp	r2, r0
08027b00  47d0      beq	#142 ; -> 0x08027b92 ; branch_target=0x08027b92
08027b02  23f08003  bic	r3, r3, #128
08027b06  0b43      orrs	r3, r1
08027b08  1360      str	r3, [r2]
08027b0a  e368      ldr	r3, [r4, #12]
08027b0c  d362      str	r3, [r2, #44]
08027b0e  6368      ldr	r3, [r4, #4]
08027b10  9362      str	r3, [r2, #40]
08027b12  1de0      b	#58 ; -> 0x08027b50 ; branch_target=0x08027b50
08027b14  a668      ldr	r6, [r4, #8]
08027b16  23f07003  bic	r3, r3, #112
08027b1a  3343      orrs	r3, r6
08027b1c  2669      ldr	r6, [r4, #16]
08027b1e  23f44073  bic	r3, r3, #768
08027b22  3343      orrs	r3, r6
08027b24  23f08003  bic	r3, r3, #128
08027b28  0b43      orrs	r3, r1
08027b2a  1360      str	r3, [r2]
08027b2c  e368      ldr	r3, [r4, #12]
08027b2e  d362      str	r3, [r2, #44]
08027b30  6368      ldr	r3, [r4, #4]
08027b32  9362      str	r3, [r2, #40]
08027b34  50b9      cbnz	r0, #20 ; -> 0x08027b4c ; branch_target=0x08027b4c
08027b36  4db9      cbnz	r5, #18 ; -> 0x08027b4c ; branch_target=0x08027b4c
08027b38  2249      ldr	r1, [pc, #136] ; [0x08027bc4] = 0x40014800 / f32_bits_interpretation=2.020019531
08027b3a  204b      ldr	r3, [pc, #128] ; [0x08027bbc] = 0x40014000 / f32_bits_interpretation=2.01953125
08027b3c  9a42      cmp	r2, r3
08027b3e  18bf      it	ne
08027b40  8a42      cmpne	r2, r1
08027b42  03d0      beq	#6 ; -> 0x08027b4c ; branch_target=0x08027b4c
08027b44  03f58063  add.w	r3, r3, #1024
08027b48  9a42      cmp	r2, r3
08027b4a  01d1      bne	#2 ; -> 0x08027b50 ; branch_target=0x08027b50
08027b4c  6369      ldr	r3, [r4, #20]
08027b4e  1363      str	r3, [r2, #48]
08027b50  0123      movs	r3, #1
08027b52  0020      movs	r0, #0
08027b54  5361      str	r3, [r2, #20]
08027b56  84f84830  strb.w	r3, [r4, #72]
08027b5a  84f83e30  strb.w	r3, [r4, #62]
08027b5e  84f83f30  strb.w	r3, [r4, #63]
08027b62  84f84030  strb.w	r3, [r4, #64]
08027b66  84f84130  strb.w	r3, [r4, #65]
08027b6a  84f84230  strb.w	r3, [r4, #66]
08027b6e  84f84330  strb.w	r3, [r4, #67]
08027b72  84f84430  strb.w	r3, [r4, #68]
08027b76  84f84530  strb.w	r3, [r4, #69]
08027b7a  84f84630  strb.w	r3, [r4, #70]
08027b7e  84f84730  strb.w	r3, [r4, #71]
08027b82  84f83d30  strb.w	r3, [r4, #61]
08027b86  70bd      pop	{r4, r5, r6, pc}
08027b88  80f83c20  strb.w	r2, [r0, #60]
08027b8c  0ef0fef9  bl	#58364 ; -> 0x08035f8c ; branch_target=0x08035f8c
08027b90  89e7      b	#-238 ; -> 0x08027aa6 ; branch_target=0x08027aa6
08027b92  2069      ldr	r0, [r4, #16]
08027b94  23f44073  bic	r3, r3, #768
08027b98  0343      orrs	r3, r0
08027b9a  23f08003  bic	r3, r3, #128
08027b9e  0b43      orrs	r3, r1
08027ba0  1360      str	r3, [r2]
08027ba2  e368      ldr	r3, [r4, #12]
08027ba4  d362      str	r3, [r2, #44]
08027ba6  6368      ldr	r3, [r4, #4]
08027ba8  9362      str	r3, [r2, #40]
08027baa  c5e7      b	#-118 ; -> 0x08027b38 ; branch_target=0x08027b38
08027bac  0120      movs	r0, #1
08027bae  7047      bx	lr
