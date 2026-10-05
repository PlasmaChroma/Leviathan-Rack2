; lubadh::Channel::~Channel()
; VA 0x313e0 size 1576

   313e0: e92d47f0     	push	{r4, r5, r6, r7, r8, r9, r10, lr}
   313e4: e2804a2a     	add	r4, r0, #172032
   313e8: e1a05000     	mov	r5, r0
   313ec: e59404e8     	ldr	r0, [r4, #0x4e8]
   313f0: e3500000     	cmp	r0, #0
   313f4: 0a000000     	beq	0x313fc
   313f8: ebff9290     	bl	0x15e40    @ imm = #-0x1b5c0 ; _ZdlPv
   313fc: e2856ba9     	add	r6, r5, #173056
   31400: e59f95bc     	ldr	r9, [pc, #0x5bc]        @ 0x319c4
   31404: e59f75bc     	ldr	r7, [pc, #0x5bc]        @ 0x319c8
   31408: e286005c     	add	r0, r6, #92
   3140c: e584945c     	str	r9, [r4, #0x45c]
   31410: ebfff769     	bl	0x2f1bc
   31414: e2860038     	add	r0, r6, #56
   31418: e5847438     	str	r7, [r4, #0x438]
   3141c: e2846e39     	add	r6, r4, #912
   31420: ebfff765     	bl	0x2f1bc
   31424: e2840e41     	add	r0, r4, #1040
   31428: e280000c     	add	r0, r0, #12
   3142c: e59f8598     	ldr	r8, [pc, #0x598]        @ 0x319cc
   31430: eb00fc16     	bl	0x70490
   31434: e1a00004     	mov	r0, r4
   31438: e5a093f8     	str	r9, [r0, #0x3f8]!
   3143c: ebfff75e     	bl	0x2f1bc
   31440: e1a00004     	mov	r0, r4
   31444: e5a073d4     	str	r7, [r0, #0x3d4]!
   31448: ebfff75b     	bl	0x2f1bc
   3144c: e2840fee     	add	r0, r4, #952
   31450: eb00fc0e     	bl	0x70490
   31454: e1a00006     	mov	r0, r6
   31458: e5848390     	str	r8, [r4, #0x390]
   3145c: ebfff839     	bl	0x2f548
   31460: e1a00006     	mov	r0, r6
   31464: ebfff890     	bl	0x2f6ac
   31468: e5940398     	ldr	r0, [r4, #0x398]
   3146c: e2843e3a     	add	r3, r4, #928
   31470: e1500003     	cmp	r0, r3
   31474: 0a000000     	beq	0x3147c
   31478: ebff9270     	bl	0x15e40    @ imm = #-0x1b640 ; _ZdlPv
   3147c: e1a00004     	mov	r0, r4
   31480: e59f3548     	ldr	r3, [pc, #0x548]        @ 0x319d0
   31484: e2846fd2     	add	r6, r4, #840
   31488: e59f9544     	ldr	r9, [pc, #0x544]        @ 0x319d4
   3148c: e5a0336c     	str	r3, [r0, #0x36c]!
   31490: ebfff749     	bl	0x2f1bc
   31494: e1a00006     	mov	r0, r6
   31498: e5849348     	str	r9, [r4, #0x348]
   3149c: ebfff902     	bl	0x2f8ac
   314a0: e1a00006     	mov	r0, r6
   314a4: ebfff959     	bl	0x2fa10
   314a8: e5940350     	ldr	r0, [r4, #0x350]
   314ac: e2843fd6     	add	r3, r4, #856
   314b0: e1500003     	cmp	r0, r3
   314b4: 0a000000     	beq	0x314bc
   314b8: ebff9260     	bl	0x15e40    @ imm = #-0x1b680 ; _ZdlPv
   314bc: e2840fcb     	add	r0, r4, #812
   314c0: e2846fbf     	add	r6, r4, #764
   314c4: eb00fbf1     	bl	0x70490
   314c8: e59f3508     	ldr	r3, [pc, #0x508]        @ 0x319d8
   314cc: e58432fc     	str	r3, [r4, #0x2fc]
   314d0: e1a00006     	mov	r0, r6
   314d4: ebfff9cd     	bl	0x2fc10
   314d8: e1a00006     	mov	r0, r6
   314dc: ebfffa25     	bl	0x2fd78
   314e0: e5940304     	ldr	r0, [r4, #0x304]
   314e4: e2843fc3     	add	r3, r4, #780
   314e8: e1500003     	cmp	r0, r3
   314ec: 0a000000     	beq	0x314f4
   314f0: ebff9252     	bl	0x15e40    @ imm = #-0x1b6b8 ; _ZdlPv
   314f4: e1a00004     	mov	r0, r4
   314f8: e2846fa2     	add	r6, r4, #648
   314fc: e5a072c8     	str	r7, [r0, #0x2c8]!
   31500: ebfff72d     	bl	0x2f1bc
   31504: e2840fab     	add	r0, r4, #684
   31508: eb00fbe0     	bl	0x70490
   3150c: e1a00006     	mov	r0, r6
   31510: e5848288     	str	r8, [r4, #0x288]
   31514: ebfff80b     	bl	0x2f548
   31518: e1a00006     	mov	r0, r6
   3151c: ebfff862     	bl	0x2f6ac
   31520: e5940290     	ldr	r0, [r4, #0x290]
   31524: e2843fa6     	add	r3, r4, #664
   31528: e1500003     	cmp	r0, r3
   3152c: 0a000000     	beq	0x31534
   31530: ebff9242     	bl	0x15e40    @ imm = #-0x1b6f8 ; _ZdlPv
   31534: e2846f99     	add	r6, r4, #612
   31538: e5849264     	str	r9, [r4, #0x264]
   3153c: e1a00006     	mov	r0, r6
   31540: ebfff8d9     	bl	0x2f8ac
   31544: e1a00006     	mov	r0, r6
   31548: ebfff930     	bl	0x2fa10
   3154c: e594026c     	ldr	r0, [r4, #0x26c]
   31550: e2843f9d     	add	r3, r4, #628
   31554: e1500003     	cmp	r0, r3
   31558: 0a000000     	beq	0x31560
   3155c: ebff9237     	bl	0x15e40    @ imm = #-0x1b724 ; _ZdlPv
   31560: e59f3474     	ldr	r3, [pc, #0x474]        @ 0x319dc
   31564: e2846f8e     	add	r6, r4, #568
   31568: e5843238     	str	r3, [r4, #0x238]
   3156c: e1a00006     	mov	r0, r6
   31570: ebfffa80     	bl	0x2ff78
   31574: e1a00006     	mov	r0, r6
   31578: ebfffad7     	bl	0x300dc
   3157c: e5940240     	ldr	r0, [r4, #0x240]
   31580: e2843f92     	add	r3, r4, #584
   31584: e1500003     	cmp	r0, r3
   31588: 0a000000     	beq	0x31590
   3158c: ebff922b     	bl	0x15e40    @ imm = #-0x1b754 ; _ZdlPv
   31590: e2840f87     	add	r0, r4, #540
   31594: e2846e16     	add	r6, r4, #352
   31598: eb00fbbc     	bl	0x70490
   3159c: e1a00004     	mov	r0, r4
   315a0: e59f3438     	ldr	r3, [pc, #0x438]        @ 0x319e0
   315a4: e59f9438     	ldr	r9, [pc, #0x438]        @ 0x319e4
   315a8: e5a031f4     	str	r3, [r0, #0x1f4]!
   315ac: ebfff702     	bl	0x2f1bc
   315b0: e1a00004     	mov	r0, r4
   315b4: e59f342c     	ldr	r3, [pc, #0x42c]        @ 0x319e8
   315b8: e5a03188     	str	r3, [r0, #0x188]!
   315bc: ebfff6fe     	bl	0x2f1bc
   315c0: e1a00006     	mov	r0, r6
   315c4: e5849160     	str	r9, [r4, #0x160]
   315c8: ebfffb46     	bl	0x302e8
   315cc: e1a00006     	mov	r0, r6
   315d0: ebfffb9d     	bl	0x3044c
   315d4: e5940168     	ldr	r0, [r4, #0x168]
   315d8: e2843e17     	add	r3, r4, #368
   315dc: e1500003     	cmp	r0, r3
   315e0: 0a000000     	beq	0x315e8
   315e4: ebff9215     	bl	0x15e40    @ imm = #-0x1b7ac ; _ZdlPv
   315e8: e2840f51     	add	r0, r4, #324
   315ec: e28460dc     	add	r6, r4, #220
   315f0: eb00fba6     	bl	0x70490
   315f4: e2840f4a     	add	r0, r4, #296
   315f8: eb00fba4     	bl	0x70490
   315fc: e1a00004     	mov	r0, r4
   31600: e5a07100     	str	r7, [r0, #0x100]!
   31604: ebfff6ec     	bl	0x2f1bc
   31608: e1a00006     	mov	r0, r6
   3160c: e58480dc     	str	r8, [r4, #0xdc]
   31610: ebfff7cc     	bl	0x2f548
   31614: e1a00006     	mov	r0, r6
   31618: ebfff823     	bl	0x2f6ac
   3161c: e59400e4     	ldr	r0, [r4, #0xe4]
   31620: e28430ec     	add	r3, r4, #236
   31624: e1500003     	cmp	r0, r3
   31628: 0a000000     	beq	0x31630
   3162c: ebff9203     	bl	0x15e40    @ imm = #-0x1b7f4 ; _ZdlPv
   31630: e59f33b4     	ldr	r3, [pc, #0x3b4]        @ 0x319ec
   31634: e28460b8     	add	r6, r4, #184
   31638: e58430b8     	str	r3, [r4, #0xb8]
   3163c: e1a00006     	mov	r0, r6
   31640: ebfffc01     	bl	0x3064c
   31644: e1a00006     	mov	r0, r6
   31648: ebfffc58     	bl	0x307b0
   3164c: e59400c0     	ldr	r0, [r4, #0xc0]
   31650: e28430c8     	add	r3, r4, #200
   31654: e1500003     	cmp	r0, r3
   31658: 0a000000     	beq	0x31660
   3165c: ebff91f7     	bl	0x15e40    @ imm = #-0x1b824 ; _ZdlPv
   31660: e1a00004     	mov	r0, r4
   31664: e2846070     	add	r6, r4, #112
   31668: e5a07094     	str	r7, [r0, #0x94]!
   3166c: ebfff6d2     	bl	0x2f1bc
   31670: e59f3378     	ldr	r3, [pc, #0x378]        @ 0x319f0
   31674: e5843070     	str	r3, [r4, #0x70]
   31678: e1a00006     	mov	r0, r6
   3167c: ebfffccb     	bl	0x309b0
   31680: e1a00006     	mov	r0, r6
   31684: ebfffd22     	bl	0x30b14
   31688: e5940078     	ldr	r0, [r4, #0x78]
   3168c: e2843080     	add	r3, r4, #128
   31690: e1500003     	cmp	r0, r3
   31694: 0a000000     	beq	0x3169c
   31698: ebff91e8     	bl	0x15e40    @ imm = #-0x1b860 ; _ZdlPv
   3169c: e1a00004     	mov	r0, r4
   316a0: e59f334c     	ldr	r3, [pc, #0x34c]        @ 0x319f4
   316a4: e5a0304c     	str	r3, [r0, #0x4c]!
   316a8: ebfff6c3     	bl	0x2f1bc
   316ac: e594003c     	ldr	r0, [r4, #0x3c]
   316b0: e3500000     	cmp	r0, #0
   316b4: 0a000000     	beq	0x316bc
   316b8: ebff91e0     	bl	0x15e40    @ imm = #-0x1b880 ; _ZdlPv
   316bc: e5940030     	ldr	r0, [r4, #0x30]
   316c0: e3500000     	cmp	r0, #0
   316c4: 0a000000     	beq	0x316cc
   316c8: ebff91dc     	bl	0x15e40    @ imm = #-0x1b890 ; _ZdlPv
   316cc: e5940024     	ldr	r0, [r4, #0x24]
   316d0: e3500000     	cmp	r0, #0
   316d4: 0a000000     	beq	0x316dc
   316d8: ebff91d8     	bl	0x15e40    @ imm = #-0x1b8a0 ; _ZdlPv
   316dc: e5940018     	ldr	r0, [r4, #0x18]
   316e0: e3500000     	cmp	r0, #0
   316e4: 0a000000     	beq	0x316ec
   316e8: ebff91d4     	bl	0x15e40    @ imm = #-0x1b8b0 ; _ZdlPv
   316ec: e594000c     	ldr	r0, [r4, #0xc]
   316f0: e3500000     	cmp	r0, #0
   316f4: 0a000000     	beq	0x316fc
   316f8: ebff91d0     	bl	0x15e40    @ imm = #-0x1b8c0 ; _ZdlPv
   316fc: e2856a29     	add	r6, r5, #167936
   31700: e5960ffc     	ldr	r0, [r6, #0xffc]
   31704: e3500000     	cmp	r0, #0
   31708: 0a000000     	beq	0x31710
   3170c: ebff91cb     	bl	0x15e40    @ imm = #-0x1b8d4 ; _ZdlPv
   31710: e5960fe8     	ldr	r0, [r6, #0xfe8]
   31714: e3500000     	cmp	r0, #0
   31718: 0a000000     	beq	0x31720
   3171c: ebff91c7     	bl	0x15e40    @ imm = #-0x1b8e4 ; _ZdlPv
   31720: e2854ba7     	add	r4, r5, #171008
   31724: e59f32cc     	ldr	r3, [pc, #0x2cc]        @ 0x319f8
   31728: e2848d0f     	add	r8, r4, #960
   3172c: e5863fc0     	str	r3, [r6, #0xfc0]
   31730: e1a00008     	mov	r0, r8
   31734: ebfffd76     	bl	0x30d14
   31738: e1a00008     	mov	r0, r8
   3173c: ebfffdce     	bl	0x30e7c
   31740: e5960fc8     	ldr	r0, [r6, #0xfc8]
   31744: e2843e3d     	add	r3, r4, #976
   31748: e1500003     	cmp	r0, r3
   3174c: 0a000000     	beq	0x31754
   31750: ebff91ba     	bl	0x15e40    @ imm = #-0x1b918 ; _ZdlPv
   31754: e2848d0e     	add	r8, r4, #896
   31758: e2840fe9     	add	r0, r4, #932
   3175c: eb00fb4b     	bl	0x70490
   31760: e1a00008     	mov	r0, r8
   31764: e5869f80     	str	r9, [r6, #0xf80]
   31768: ebfffade     	bl	0x302e8
   3176c: e1a00008     	mov	r0, r8
   31770: ebfffb35     	bl	0x3044c
   31774: e5960f88     	ldr	r0, [r6, #0xf88]
   31778: e2843e39     	add	r3, r4, #912
   3177c: e1500003     	cmp	r0, r3
   31780: 0a000000     	beq	0x31788
   31784: ebff91ad     	bl	0x15e40    @ imm = #-0x1b94c ; _ZdlPv
   31788: e59f326c     	ldr	r3, [pc, #0x26c]        @ 0x319fc
   3178c: e2848fd6     	add	r8, r4, #856
   31790: e1a00008     	mov	r0, r8
   31794: e5863f58     	str	r3, [r6, #0xf58]
   31798: ebfffe37     	bl	0x3107c
   3179c: e1a00008     	mov	r0, r8
   317a0: ebfffe8e     	bl	0x311e0
   317a4: e5960f60     	ldr	r0, [r6, #0xf60]
   317a8: e2844fda     	add	r4, r4, #872
   317ac: e1500004     	cmp	r0, r4
   317b0: 0a000000     	beq	0x317b8
   317b4: ebff91a1     	bl	0x15e40    @ imm = #-0x1b97c ; _ZdlPv
   317b8: e2853a06     	add	r3, r5, #24576
   317bc: e593021c     	ldr	r0, [r3, #0x21c]
   317c0: e3500000     	cmp	r0, #0
   317c4: 0a000000     	beq	0x317cc
   317c8: ebff919c     	bl	0x15e40    @ imm = #-0x1b990 ; _ZdlPv
   317cc: e2854901     	add	r4, r5, #16384
   317d0: e2850c4e     	add	r0, r5, #19968
   317d4: e280008c     	add	r0, r0, #140
   317d8: ebff918f     	bl	0x15e1c    @ imm = #-0x1b9c4 ; _ZNSt13random_device7_M_finiEv
   317dc: e5940a7c     	ldr	r0, [r4, #0xa7c]
   317e0: e3500000     	cmp	r0, #0
   317e4: 0a000000     	beq	0x317ec
   317e8: ebff9194     	bl	0x15e40    @ imm = #-0x1b9b0 ; _ZdlPv
   317ec: e5940a70     	ldr	r0, [r4, #0xa70]
   317f0: e3500000     	cmp	r0, #0
   317f4: 0a000000     	beq	0x317fc
   317f8: ebff9190     	bl	0x15e40    @ imm = #-0x1b9c0 ; _ZdlPv
   317fc: e5940a64     	ldr	r0, [r4, #0xa64]
   31800: e3500000     	cmp	r0, #0
   31804: 0a000000     	beq	0x3180c
   31808: ebff918c     	bl	0x15e40    @ imm = #-0x1b9d0 ; _ZdlPv
   3180c: e5940a50     	ldr	r0, [r4, #0xa50]
   31810: e3500000     	cmp	r0, #0
   31814: 0a000000     	beq	0x3181c
   31818: ebff9188     	bl	0x15e40    @ imm = #-0x1b9e0 ; _ZdlPv
   3181c: e5940a44     	ldr	r0, [r4, #0xa44]
   31820: e3500000     	cmp	r0, #0
   31824: 0a000000     	beq	0x3182c
   31828: ebff9184     	bl	0x15e40    @ imm = #-0x1b9f0 ; _ZdlPv
   3182c: e5940a38     	ldr	r0, [r4, #0xa38]
   31830: e3500000     	cmp	r0, #0
   31834: 0a000000     	beq	0x3183c
   31838: ebff9180     	bl	0x15e40    @ imm = #-0x1ba00 ; _ZdlPv
   3183c: e2854a03     	add	r4, r5, #12288
   31840: e594025c     	ldr	r0, [r4, #0x25c]
   31844: e3500000     	cmp	r0, #0
   31848: 0a000000     	beq	0x31850
   3184c: ebff917b     	bl	0x15e40    @ imm = #-0x1ba14 ; _ZdlPv
   31850: e5940250     	ldr	r0, [r4, #0x250]
   31854: e3500000     	cmp	r0, #0
   31858: 0a000000     	beq	0x31860
   3185c: ebff9177     	bl	0x15e40    @ imm = #-0x1ba24 ; _ZdlPv
   31860: e5940244     	ldr	r0, [r4, #0x244]
   31864: e3500000     	cmp	r0, #0
   31868: 0a000000     	beq	0x31870
   3186c: ebff9173     	bl	0x15e40    @ imm = #-0x1ba34 ; _ZdlPv
   31870: e2854a01     	add	r4, r5, #4096
   31874: e5940a50     	ldr	r0, [r4, #0xa50]
   31878: e3500000     	cmp	r0, #0
   3187c: 0a000000     	beq	0x31884
   31880: ebff916e     	bl	0x15e40    @ imm = #-0x1ba48 ; _ZdlPv
   31884: e5940a44     	ldr	r0, [r4, #0xa44]
   31888: e3500000     	cmp	r0, #0
   3188c: 0a000000     	beq	0x31894
   31890: ebff916a     	bl	0x15e40    @ imm = #-0x1ba58 ; _ZdlPv
   31894: e5940a38     	ldr	r0, [r4, #0xa38]
   31898: e3500000     	cmp	r0, #0
   3189c: 0a000000     	beq	0x318a4
   318a0: ebff9166     	bl	0x15e40    @ imm = #-0x1ba68 ; _ZdlPv
   318a4: e59409dc     	ldr	r0, [r4, #0x9dc]
   318a8: e3500000     	cmp	r0, #0
   318ac: 0a000000     	beq	0x318b4
   318b0: ebff9162     	bl	0x15e40    @ imm = #-0x1ba78 ; _ZdlPv
   318b4: e59409d0     	ldr	r0, [r4, #0x9d0]
   318b8: e3500000     	cmp	r0, #0
   318bc: 0a000000     	beq	0x318c4
   318c0: ebff915e     	bl	0x15e40    @ imm = #-0x1ba88 ; _ZdlPv
   318c4: e59409c4     	ldr	r0, [r4, #0x9c4]
   318c8: e3500000     	cmp	r0, #0
   318cc: 0a000000     	beq	0x318d4
   318d0: ebff915a     	bl	0x15e40    @ imm = #-0x1ba98 ; _ZdlPv
   318d4: e59502cc     	ldr	r0, [r5, #0x2cc]
   318d8: e3500000     	cmp	r0, #0
   318dc: 0a000000     	beq	0x318e4
   318e0: ebff9156     	bl	0x15e40    @ imm = #-0x1baa8 ; _ZdlPv
   318e4: e59502c0     	ldr	r0, [r5, #0x2c0]
   318e8: e3500000     	cmp	r0, #0
   318ec: 0a000000     	beq	0x318f4
   318f0: ebff9152     	bl	0x15e40    @ imm = #-0x1bab8 ; _ZdlPv
   318f4: e595020c     	ldr	r0, [r5, #0x20c]
   318f8: e3500000     	cmp	r0, #0
   318fc: 0a00000a     	beq	0x3192c
   31900: e5956230     	ldr	r6, [r5, #0x230]
   31904: e5954220     	ldr	r4, [r5, #0x220]
   31908: e2866004     	add	r6, r6, #4
   3190c: e1540006     	cmp	r4, r6
   31910: 2a000004     	bhs	0x31928
   31914: e4940004     	ldr	r0, [r4], #4
   31918: ebff9148     	bl	0x15e40    @ imm = #-0x1bae0 ; _ZdlPv
   3191c: e1560004     	cmp	r6, r4
   31920: 8afffffb     	bhi	0x31914
   31924: e595020c     	ldr	r0, [r5, #0x20c]
   31928: ebff9144     	bl	0x15e40    @ imm = #-0x1baf0 ; _ZdlPv
   3192c: e59501e4     	ldr	r0, [r5, #0x1e4]
   31930: e3500000     	cmp	r0, #0
   31934: 0a00000a     	beq	0x31964
   31938: e5956208     	ldr	r6, [r5, #0x208]
   3193c: e59541f8     	ldr	r4, [r5, #0x1f8]
   31940: e2866004     	add	r6, r6, #4
   31944: e1540006     	cmp	r4, r6
   31948: 2a000004     	bhs	0x31960
   3194c: e4940004     	ldr	r0, [r4], #4
   31950: ebff913a     	bl	0x15e40    @ imm = #-0x1bb18 ; _ZdlPv
   31954: e1560004     	cmp	r6, r4
   31958: 8afffffb     	bhi	0x3194c
   3195c: e59501e4     	ldr	r0, [r5, #0x1e4]
   31960: ebff9136     	bl	0x15e40    @ imm = #-0x1bb28 ; _ZdlPv
   31964: e1a00005     	mov	r0, r5
   31968: e59f2090     	ldr	r2, [pc, #0x90]         @ 0x31a00
   3196c: e59f3090     	ldr	r3, [pc, #0x90]         @ 0x31a04
   31970: e5852118     	str	r2, [r5, #0x118]
   31974: e5a03128     	str	r3, [r0, #0x128]!
   31978: eb00e8c6     	bl	0x6bc98
   3197c: e59500d8     	ldr	r0, [r5, #0xd8]
   31980: e3500000     	cmp	r0, #0
   31984: 0a000000     	beq	0x3198c
   31988: ebff912c     	bl	0x15e40    @ imm = #-0x1bb50 ; _ZdlPv
   3198c: e5950094     	ldr	r0, [r5, #0x94]
   31990: e3500000     	cmp	r0, #0
   31994: 0a000000     	beq	0x3199c
   31998: ebff9128     	bl	0x15e40    @ imm = #-0x1bb60 ; _ZdlPv
   3199c: e1a00005     	mov	r0, r5
   319a0: e5a07054     	str	r7, [r0, #0x54]!
   319a4: ebfff604     	bl	0x2f1bc
   319a8: e5950004     	ldr	r0, [r5, #0x4]
   319ac: e285300c     	add	r3, r5, #12
   319b0: e1500003     	cmp	r0, r3
   319b4: 0a000000     	beq	0x319bc
   319b8: ebff9120     	bl	0x15e40    @ imm = #-0x1bb80 ; _ZdlPv
   319bc: e1a00005     	mov	r0, r5
   319c0: e8bd87f0     	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
   319c4: c0 1f 07 00  	.word	0x00071fc0
   319c8: e0 1e 07 00  	.word	0x00071ee0
   319cc: 50 1f 07 00  	.word	0x00071f50
   319d0: b0 1f 07 00  	.word	0x00071fb0
   319d4: 90 1f 07 00  	.word	0x00071f90
   319d8: a0 1f 07 00  	.word	0x00071fa0
   319dc: 80 1f 07 00  	.word	0x00071f80
   319e0: 70 1f 07 00  	.word	0x00071f70
   319e4: 00 1f 07 00  	.word	0x00071f00
   319e8: 60 1f 07 00  	.word	0x00071f60
   319ec: 40 1f 07 00  	.word	0x00071f40
   319f0: 30 1f 07 00  	.word	0x00071f30
   319f4: 20 1f 07 00  	.word	0x00071f20
   319f8: 10 1f 07 00  	.word	0x00071f10
   319fc: f0 1e 07 00  	.word	0x00071ef0
   31a00: 14 23 07 00  	.word	0x00072314
   31a04: ec ee 08 00  	.word	0x0008eeec
