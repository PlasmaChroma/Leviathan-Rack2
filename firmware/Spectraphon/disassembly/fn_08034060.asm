; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic,startup_call
08034060  2de9f04f  push.w	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
08034064  a9b0      sub	sp, #164
08034066  fff7d1ff  bl	#-94 ; -> 0x0803400c ; branch_target=0x0803400c
0803406a  f14a      ldr	r2, [pc, #964] ; [0x08034430] = 0xe000ed00
0803406c  5369      ldr	r3, [r2, #20]
0803406e  13f40033  ands	r3, r3, #131072
08034072  11d1      bne	#34 ; -> 0x08034098 ; branch_target=0x08034098
08034074  bff34f8f  dsb	sy
08034078  bff36f8f  isb	sy
0803407c  c2f85032  str.w	r3, [r2, #592]
08034080  bff34f8f  dsb	sy
08034084  bff36f8f  isb	sy
08034088  5369      ldr	r3, [r2, #20]
0803408a  43f40033  orr	r3, r3, #131072
0803408e  5361      str	r3, [r2, #20]
08034090  bff34f8f  dsb	sy
08034094  bff36f8f  isb	sy
08034098  e548      ldr	r0, [pc, #916] ; [0x08034430] = 0xe000ed00
0803409a  4369      ldr	r3, [r0, #20]
0803409c  13f48033  ands	r3, r3, #65536
080340a0  24d1      bne	#72 ; -> 0x080340ec ; branch_target=0x080340ec
080340a2  c0f88430  str.w	r3, [r0, #132]
080340a6  bff34f8f  dsb	sy
080340aa  d0f88050  ldr.w	r5, [r0, #128]
080340ae  43f6e076  movw	r6, #16352
080340b2  c5f34e34  ubfx	r4, r5, #13, #15
080340b6  c5f3c905  ubfx	r5, r5, #3, #10
080340ba  6401      lsls	r4, r4, #5
080340bc  04ea0601  and.w	r1, r4, r6
080340c0  2b46      mov	r3, r5
080340c2  41ea8372  orr.w	r2, r1, r3, lsl #30
080340c6  013b      subs	r3, #1
080340c8  5f1c      adds	r7, r3, #1
080340ca  c0f86022  str.w	r2, [r0, #608]
080340ce  f8d1      bne	#-16 ; -> 0x080340c2 ; branch_target=0x080340c2
080340d0  203c      subs	r4, #32
080340d2  14f1200f  cmn.w	r4, #32
080340d6  f1d1      bne	#-30 ; -> 0x080340bc ; branch_target=0x080340bc
080340d8  bff34f8f  dsb	sy
080340dc  4369      ldr	r3, [r0, #20]
080340de  43f48033  orr	r3, r3, #65536
080340e2  4361      str	r3, [r0, #20]
080340e4  bff34f8f  dsb	sy
080340e8  bff36f8f  isb	sy
080340ec  ecf71af9  bl	#-81356 ; -> 0x08020324 ; branch_target=0x08020324
080340f0  ecf76cf9  bl	#-81192 ; -> 0x080203cc ; branch_target=0x080203cc
080340f4  cf4b      ldr	r3, [pc, #828] ; [0x08034434] = 0x200144cc
080340f6  1860      str	r0, [r3]
080340f8  fff78efe  bl	#-740 ; -> 0x08033e18 ; branch_target=0x08033e18
080340fc  fff7f8fe  bl	#-528 ; -> 0x08033ef0 ; branch_target=0x08033ef0
08034100  fef710fe  bl	#-5088 ; -> 0x08032d24 ; branch_target=0x08032d24
08034104  fef7e0fc  bl	#-5696 ; -> 0x08032ac8 ; branch_target=0x08032ac8
08034108  fef756fd  bl	#-5460 ; -> 0x08032bb8 ; branch_target=0x08032bb8
0803410c  f7f7ccff  bl	#-32872 ; -> 0x0802c0a8 ; branch_target=0x0802c0a8
08034110  01f048fb  bl	#5776 ; -> 0x080357a4 ; branch_target=0x080357a4
08034114  fef734fd  bl	#-5528 ; -> 0x08032b80 ; branch_target=0x08032b80
08034118  02f0c2f8  bl	#8580 ; -> 0x080362a0 ; branch_target=0x080362a0
0803411c  02f028f9  bl	#8784 ; -> 0x08036370 ; branch_target=0x08036370
08034120  00f0f0ff  bl	#4064 ; -> 0x08035104 ; branch_target=0x08035104
08034124  01f0b8fb  bl	#6000 ; -> 0x08035898 ; branch_target=0x08035898
08034128  02f042f8  bl	#8324 ; -> 0x080361b0 ; branch_target=0x080361b0
0803412c  01f04cfd  bl	#6808 ; -> 0x08035bc8 ; branch_target=0x08035bc8
08034130  01f030fe  bl	#7264 ; -> 0x08035d94 ; branch_target=0x08035d94
08034134  01f05cfe  bl	#7352 ; -> 0x08035df0 ; branch_target=0x08035df0
08034138  f8f752f8  bl	#-32604 ; -> 0x0802c1e0 ; branch_target=0x0802c1e0
0803413c  fef74cff  bl	#-4456 ; -> 0x08032fd8 ; branch_target=0x08032fd8
08034140  01f01ef8  bl	#4156 ; -> 0x08035180 ; branch_target=0x08035180
08034144  fef7b8fa  bl	#-6800 ; -> 0x080326b8 ; branch_target=0x080326b8
08034148  01f084f8  bl	#4360 ; -> 0x08035254 ; branch_target=0x08035254
0803414c  01f04ef8  bl	#4252 ; -> 0x080351ec ; branch_target=0x080351ec
08034150  0022      movs	r2, #0
08034152  b948      ldr	r0, [pc, #740] ; [0x08034438] = 0x20003430
08034154  1146      mov	r1, r2
08034156  eff7dff9  bl	#-68674 ; -> 0x08023518 ; branch_target=0x08023518
0803415a  f1ee103a  vmrs	r3, fpscr
0803415e  43f08073  orr	r3, r3, #16777216
08034162  e1ee103a  vmsr	fpscr, r3
08034166  0122      movs	r2, #1
08034168  4ff48061  mov.w	r1, #1024
0803416c  b348      ldr	r0, [pc, #716] ; [0x0803443c] = 0x58021400
0803416e  eff74df9  bl	#-68966 ; -> 0x0802340c ; branch_target=0x0802340c
08034172  0022      movs	r2, #0
08034174  4ff40041  mov.w	r1, #32768
08034178  b148      ldr	r0, [pc, #708] ; [0x08034440] = 0x58020800
0803417a  eff747f9  bl	#-68978 ; -> 0x0802340c ; branch_target=0x0802340c
0803417e  0a20      movs	r0, #10
08034180  dff8ccb2  ldr.w	r11, [pc, #716] ; [0x08034450] = 0x20000920
08034184  ecf710f9  bl	#-81376 ; -> 0x080203a8 ; branch_target=0x080203a8
08034188  fff7e2fe  bl	#-572 ; -> 0x08033f50 ; branch_target=0x08033f50
0803418c  4ff48013  mov.w	r3, #1048576
08034190  dff8c082  ldr.w	r8, [pc, #704] ; [0x08034454] = 0x20000a20
08034194  0824      movs	r4, #8
08034196  cbf80030  str.w	r3, [r11]
0803419a  4ff48033  mov.w	r3, #65536
0803419e  0125      movs	r5, #1
080341a0  0026      movs	r6, #0
080341a2  c8f81030  str.w	r3, [r8, #16]
080341a6  4ff48813  mov.w	r3, #1114112
080341aa  cbf80440  str.w	r4, [r11, #4]
080341ae  2a46      mov	r2, r5
080341b0  cbf81030  str.w	r3, [r11, #16]
080341b4  4ff40033  mov.w	r3, #131072
080341b8  c8f80440  str.w	r4, [r8, #4]
080341bc  4ff48061  mov.w	r1, #1024
080341c0  c8f82030  str.w	r3, [r8, #32]
080341c4  4ff49013  mov.w	r3, #1179648
080341c8  cbf80840  str.w	r4, [r11, #8]
080341cc  4ff08049  mov.w	r9, #1073741824
080341d0  cbf82030  str.w	r3, [r11, #32]
080341d4  4ff44033  mov.w	r3, #196608
080341d8  c8f80840  str.w	r4, [r8, #8]
080341dc  c8f83030  str.w	r3, [r8, #48]
080341e0  4ff49813  mov.w	r3, #1245184
080341e4  cbf80c50  str.w	r5, [r11, #12]
080341e8  cbf83030  str.w	r3, [r11, #48]
080341ec  4ff48023  mov.w	r3, #262144
080341f0  c8f80c50  str.w	r5, [r8, #12]
080341f4  cbf81440  str.w	r4, [r11, #20]
080341f8  c8f81440  str.w	r4, [r8, #20]
080341fc  cbf81840  str.w	r4, [r11, #24]
08034200  c8f81840  str.w	r4, [r8, #24]
08034204  cbf81c50  str.w	r5, [r11, #28]
08034208  c8f81c50  str.w	r5, [r8, #28]
0803420c  cbf82440  str.w	r4, [r11, #36]
08034210  c8f82440  str.w	r4, [r8, #36]
08034214  cbf82840  str.w	r4, [r11, #40]
08034218  c8f82840  str.w	r4, [r8, #40]
0803421c  cbf82c50  str.w	r5, [r11, #44]
08034220  c8f82c50  str.w	r5, [r8, #44]
08034224  cbf83440  str.w	r4, [r11, #52]
08034228  c8f83440  str.w	r4, [r8, #52]
0803422c  cbf83840  str.w	r4, [r11, #56]
08034230  c8f83840  str.w	r4, [r8, #56]
08034234  cbf83c50  str.w	r5, [r11, #60]
08034238  c8f83c50  str.w	r5, [r8, #60]
0803423c  c8f80060  str.w	r6, [r8]
08034240  cbf84440  str.w	r4, [r11, #68]
08034244  c8f84030  str.w	r3, [r8, #64]
08034248  4ff4a013  mov.w	r3, #1310720
0803424c  c8f84440  str.w	r4, [r8, #68]
08034250  cbf84030  str.w	r3, [r11, #64]
08034254  4ff4a023  mov.w	r3, #327680
08034258  cbf84840  str.w	r4, [r11, #72]
0803425c  c8f85030  str.w	r3, [r8, #80]
08034260  4ff4a813  mov.w	r3, #1376256
08034264  c8f84840  str.w	r4, [r8, #72]
08034268  cbf85030  str.w	r3, [r11, #80]
0803426c  4ff4c023  mov.w	r3, #393216
08034270  cbf84c50  str.w	r5, [r11, #76]
08034274  c8f86030  str.w	r3, [r8, #96]
08034278  4ff4b013  mov.w	r3, #1441792
0803427c  c8f84c50  str.w	r5, [r8, #76]
08034280  cbf86030  str.w	r3, [r11, #96]
08034284  4ff4e023  mov.w	r3, #458752
08034288  cbf85440  str.w	r4, [r11, #84]
0803428c  c8f87030  str.w	r3, [r8, #112]
08034290  4ff4b813  mov.w	r3, #1507328
08034294  c8f85440  str.w	r4, [r8, #84]
08034298  cbf87030  str.w	r3, [r11, #112]
0803429c  4ff40023  mov.w	r3, #524288
080342a0  cbf85840  str.w	r4, [r11, #88]
080342a4  c8f85840  str.w	r4, [r8, #88]
080342a8  cbf85c50  str.w	r5, [r11, #92]
080342ac  c8f85c50  str.w	r5, [r8, #92]
080342b0  cbf86440  str.w	r4, [r11, #100]
080342b4  c8f86440  str.w	r4, [r8, #100]
080342b8  cbf86840  str.w	r4, [r11, #104]
080342bc  c8f86840  str.w	r4, [r8, #104]
080342c0  cbf86c50  str.w	r5, [r11, #108]
080342c4  c8f86c50  str.w	r5, [r8, #108]
080342c8  cbf87440  str.w	r4, [r11, #116]
080342cc  c8f87440  str.w	r4, [r8, #116]
080342d0  cbf87840  str.w	r4, [r11, #120]
080342d4  c8f87840  str.w	r4, [r8, #120]
080342d8  cbf87c50  str.w	r5, [r11, #124]
080342dc  c8f87c50  str.w	r5, [r8, #124]
080342e0  cbf88440  str.w	r4, [r11, #132]
080342e4  c8f88440  str.w	r4, [r8, #132]
080342e8  c8f88030  str.w	r3, [r8, #128]
080342ec  4ff4c013  mov.w	r3, #1572864
080342f0  cbf88840  str.w	r4, [r11, #136]
080342f4  cbf88030  str.w	r3, [r11, #128]
080342f8  4ff41023  mov.w	r3, #589824
080342fc  c8f88840  str.w	r4, [r8, #136]
08034300  c8f89030  str.w	r3, [r8, #144]
08034304  4ff4c813  mov.w	r3, #1638400
08034308  cbf88c50  str.w	r5, [r11, #140]
0803430c  cbf89030  str.w	r3, [r11, #144]
08034310  4ff42023  mov.w	r3, #655360
08034314  c8f88c50  str.w	r5, [r8, #140]
08034318  c8f8a030  str.w	r3, [r8, #160]
0803431c  4ff4d013  mov.w	r3, #1703936
08034320  cbf89440  str.w	r4, [r11, #148]
08034324  cbf8a030  str.w	r3, [r11, #160]
08034328  4ff43023  mov.w	r3, #720896
0803432c  c8f89440  str.w	r4, [r8, #148]
08034330  c8f8b030  str.w	r3, [r8, #176]
08034334  4ff4d813  mov.w	r3, #1769472
08034338  cbf89840  str.w	r4, [r11, #152]
0803433c  cbf8b030  str.w	r3, [r11, #176]
08034340  4ff44023  mov.w	r3, #786432
08034344  c8f89840  str.w	r4, [r8, #152]
08034348  cbf89c50  str.w	r5, [r11, #156]
0803434c  c8f89c50  str.w	r5, [r8, #156]
08034350  cbf8a440  str.w	r4, [r11, #164]
08034354  c8f8a440  str.w	r4, [r8, #164]
08034358  cbf8a840  str.w	r4, [r11, #168]
0803435c  c8f8a840  str.w	r4, [r8, #168]
08034360  cbf8ac50  str.w	r5, [r11, #172]
08034364  c8f8ac50  str.w	r5, [r8, #172]
08034368  cbf8b440  str.w	r4, [r11, #180]
0803436c  c8f8b440  str.w	r4, [r8, #180]
08034370  cbf8b840  str.w	r4, [r11, #184]
08034374  c8f8b840  str.w	r4, [r8, #184]
08034378  cbf8bc50  str.w	r5, [r11, #188]
0803437c  c8f8bc50  str.w	r5, [r8, #188]
08034380  cbf8c440  str.w	r4, [r11, #196]
08034384  c8f8c440  str.w	r4, [r8, #196]
08034388  cbf8c840  str.w	r4, [r11, #200]
0803438c  c8f8c030  str.w	r3, [r8, #192]
08034390  4ff4e013  mov.w	r3, #1835008
08034394  2b4f      ldr	r7, [pc, #172] ; [0x08034444] = 0x200144d4
08034396  cbf8c030  str.w	r3, [r11, #192]
0803439a  4ff45023  mov.w	r3, #851968
0803439e  2a48      ldr	r0, [pc, #168] ; [0x08034448] = 0x58020000
080343a0  c8f8d030  str.w	r3, [r8, #208]
080343a4  4ff4e813  mov.w	r3, #1900544
080343a8  c8f8c840  str.w	r4, [r8, #200]
080343ac  cbf8d030  str.w	r3, [r11, #208]
080343b0  4ff46023  mov.w	r3, #917504
080343b4  cbf8cc50  str.w	r5, [r11, #204]
080343b8  c8f8e030  str.w	r3, [r8, #224]
080343bc  4ff4f013  mov.w	r3, #1966080
080343c0  c8f8cc50  str.w	r5, [r8, #204]
080343c4  cbf8e030  str.w	r3, [r11, #224]
080343c8  4ff47023  mov.w	r3, #983040
080343cc  cbf8d440  str.w	r4, [r11, #212]
080343d0  c8f8f030  str.w	r3, [r8, #240]
080343d4  4ff4f813  mov.w	r3, #2031616
080343d8  c8f8d440  str.w	r4, [r8, #212]
080343dc  cbf8f030  str.w	r3, [r11, #240]
080343e0  1a4b      ldr	r3, [pc, #104] ; [0x0803444c] = 0x200144d0
080343e2  cbf8d840  str.w	r4, [r11, #216]
080343e6  1e60      str	r6, [r3]
080343e8  c8f8d840  str.w	r4, [r8, #216]
080343ec  cbf8dc50  str.w	r5, [r11, #220]
080343f0  c8f8dc50  str.w	r5, [r8, #220]
080343f4  cbf8e440  str.w	r4, [r11, #228]
080343f8  c8f8e440  str.w	r4, [r8, #228]
080343fc  cbf8e840  str.w	r4, [r11, #232]
08034400  c8f8e840  str.w	r4, [r8, #232]
08034404  cbf8ec50  str.w	r5, [r11, #236]
08034408  c8f8ec50  str.w	r5, [r8, #236]
0803440c  cbf8f440  str.w	r4, [r11, #244]
08034410  c8f8f440  str.w	r4, [r8, #244]
08034414  cbf8f840  str.w	r4, [r11, #248]
08034418  c8f8f840  str.w	r4, [r8, #248]
0803441c  cbf8fc50  str.w	r5, [r11, #252]
08034420  c8f8fc50  str.w	r5, [r8, #252]
08034424  3e60      str	r6, [r7]
08034426  eef7f1ff  bl	#-69662 ; -> 0x0802340c ; branch_target=0x0802340c
0803442a  2a46      mov	r2, r5
0803442c  14e0      b	#40 ; -> 0x08034458 ; branch_target=0x08034458
08034458  6e48      ldr	r0, [pc, #440] ; [0x08034614] = 0x58020000
0803445a  4ff48071  mov.w	r1, #256
0803445e  dff8dca1  ldr.w	r10, [pc, #476] ; [0x0803463c] = 0x40000800 / f32_bits_interpretation=2.000488281
08034462  eef7d3ff  bl	#-69722 ; -> 0x0802340c ; branch_target=0x0802340c
08034466  2a46      mov	r2, r5
08034468  2946      mov	r1, r5
0803446a  6b48      ldr	r0, [pc, #428] ; [0x08034618] = 0x58020400
0803446c  eef7ceff  bl	#-69732 ; -> 0x0802340c ; branch_target=0x0802340c
08034470  2a46      mov	r2, r5
08034472  0221      movs	r1, #2
08034474  6848      ldr	r0, [pc, #416] ; [0x08034618] = 0x58020400
08034476  eef7c9ff  bl	#-69742 ; -> 0x0802340c ; branch_target=0x0802340c
0803447a  2a46      mov	r2, r5
0803447c  2021      movs	r1, #32
0803447e  6648      ldr	r0, [pc, #408] ; [0x08034618] = 0x58020400
08034480  eef7c4ff  bl	#-69752 ; -> 0x0802340c ; branch_target=0x0802340c
08034484  2a46      mov	r2, r5
08034486  4ff48071  mov.w	r1, #256
0803448a  6348      ldr	r0, [pc, #396] ; [0x08034618] = 0x58020400
0803448c  eef7beff  bl	#-69764 ; -> 0x0802340c ; branch_target=0x0802340c
08034490  2a46      mov	r2, r5
08034492  4ff48061  mov.w	r1, #1024
08034496  6048      ldr	r0, [pc, #384] ; [0x08034618] = 0x58020400
08034498  eef7b8ff  bl	#-69776 ; -> 0x0802340c ; branch_target=0x0802340c
0803449c  2a46      mov	r2, r5
0803449e  4ff40061  mov.w	r1, #2048
080344a2  5d48      ldr	r0, [pc, #372] ; [0x08034618] = 0x58020400
080344a4  eef7b2ff  bl	#-69788 ; -> 0x0802340c ; branch_target=0x0802340c
080344a8  2a46      mov	r2, r5
080344aa  4ff48041  mov.w	r1, #16384
080344ae  5a48      ldr	r0, [pc, #360] ; [0x08034618] = 0x58020400
080344b0  eef7acff  bl	#-69800 ; -> 0x0802340c ; branch_target=0x0802340c
080344b4  2a46      mov	r2, r5
080344b6  4ff40041  mov.w	r1, #32768
080344ba  5748      ldr	r0, [pc, #348] ; [0x08034618] = 0x58020400
080344bc  eef7a6ff  bl	#-69812 ; -> 0x0802340c ; branch_target=0x0802340c
080344c0  2a46      mov	r2, r5
080344c2  4021      movs	r1, #64
080344c4  5548      ldr	r0, [pc, #340] ; [0x0803461c] = 0x58020800
080344c6  eef7a1ff  bl	#-69822 ; -> 0x0802340c ; branch_target=0x0802340c
080344ca  2a46      mov	r2, r5
080344cc  8021      movs	r1, #128
080344ce  5348      ldr	r0, [pc, #332] ; [0x0803461c] = 0x58020800
080344d0  eef79cff  bl	#-69832 ; -> 0x0802340c ; branch_target=0x0802340c
080344d4  2a46      mov	r2, r5
080344d6  4ff48051  mov.w	r1, #4096
080344da  5148      ldr	r0, [pc, #324] ; [0x08034620] = 0x58020c00
080344dc  eef796ff  bl	#-69844 ; -> 0x0802340c ; branch_target=0x0802340c
080344e0  2a46      mov	r2, r5
080344e2  4f48      ldr	r0, [pc, #316] ; [0x08034620] = 0x58020c00
080344e4  4ff40051  mov.w	r1, #8192
080344e8  eef790ff  bl	#-69856 ; -> 0x0802340c ; branch_target=0x0802340c
080344ec  2146      mov	r1, r4
080344ee  4d48      ldr	r0, [pc, #308] ; [0x08034624] = 0x20014d74
080344f0  f3f76afb  bl	#-51500 ; -> 0x08027bc8 ; branch_target=0x08027bc8
080344f4  0c21      movs	r1, #12
080344f6  4b48      ldr	r0, [pc, #300] ; [0x08034624] = 0x20014d74
080344f8  f3f766fb  bl	#-51508 ; -> 0x08027bc8 ; branch_target=0x08027bc8
080344fc  3146      mov	r1, r6
080344fe  4a48      ldr	r0, [pc, #296] ; [0x08034628] = 0x20014c40
08034500  f3f762fb  bl	#-51516 ; -> 0x08027bc8 ; branch_target=0x08027bc8
08034504  2146      mov	r1, r4
08034506  4948      ldr	r0, [pc, #292] ; [0x0803462c] = 0x20014d28
08034508  f3f75efb  bl	#-51524 ; -> 0x08027bc8 ; branch_target=0x08027bc8
0803450c  0c21      movs	r1, #12
0803450e  4748      ldr	r0, [pc, #284] ; [0x0803462c] = 0x20014d28
08034510  f3f75afb  bl	#-51532 ; -> 0x08027bc8 ; branch_target=0x08027bc8
08034514  2146      mov	r1, r4
08034516  4648      ldr	r0, [pc, #280] ; [0x08034630] = 0x20014cdc
08034518  7f24      movs	r4, #127
0803451a  f3f755fb  bl	#-51542 ; -> 0x08027bc8 ; branch_target=0x08027bc8
0803451e  454d      ldr	r5, [pc, #276] ; [0x08034634] = 0x200144c8
08034520  454a      ldr	r2, [pc, #276] ; [0x08034638] = 0x40001800 / f32_bits_interpretation=2.001464844
08034522  4323      movs	r3, #67
08034524  4ff49660  mov.w	r0, #1200
08034528  2b60      str	r3, [r5]
0803452a  5463      str	r4, [r2, #52]
0803452c  c9f83c40  str.w	r4, [r9, #60]
08034530  c9f84040  str.w	r4, [r9, #64]
08034534  caf83c40  str.w	r4, [r10, #60]
08034538  caf83c60  str.w	r6, [r10, #60]
0803453c  ebf734ff  bl	#-82328 ; -> 0x080203a8 ; branch_target=0x080203a8
08034540  3d4a      ldr	r2, [pc, #244] ; [0x08034638] = 0x40001800 / f32_bits_interpretation=2.001464844
08034542  5463      str	r4, [r2, #52]
08034544  c9f83c40  str.w	r4, [r9, #60]
08034548  c9f84040  str.w	r4, [r9, #64]
0803454c  caf83c40  str.w	r4, [r10, #60]
08034550  2b68      ldr	r3, [r5]
08034552  1907      lsls	r1, r3, #28
08034554  00d5      bpl	#0 ; -> 0x08034558 ; branch_target=0x08034558
08034556  5663      str	r6, [r2, #52]
08034558  5c07      lsls	r4, r3, #29
0803455a  02d5      bpl	#4 ; -> 0x08034562 ; branch_target=0x08034562
0803455c  374a      ldr	r2, [pc, #220] ; [0x0803463c] = 0x40000800 / f32_bits_interpretation=2.000488281
0803455e  0021      movs	r1, #0
08034560  d163      str	r1, [r2, #60]
08034562  9807      lsls	r0, r3, #30
08034564  03d5      bpl	#6 ; -> 0x0803456e ; branch_target=0x0803456e
08034566  4ff08042  mov.w	r2, #1073741824
0803456a  0021      movs	r1, #0
0803456c  1164      str	r1, [r2, #64]
0803456e  d907      lsls	r1, r3, #31
08034570  03d5      bpl	#6 ; -> 0x0803457a ; branch_target=0x0803457a
08034572  4ff08043  mov.w	r3, #1073741824
08034576  0022      movs	r2, #0
08034578  da63      str	r2, [r3, #60]
0803457a  4ff49660  mov.w	r0, #1200
0803457e  ebf713ff  bl	#-82394 ; -> 0x080203a8 ; branch_target=0x080203a8
08034582  2f4b      ldr	r3, [pc, #188] ; [0x08034640] = 0x20002ee8
08034584  1b68      ldr	r3, [r3]
08034586  012b      cmp	r3, #1
08034588  25dd      ble	#74 ; -> 0x080345d6 ; branch_target=0x080345d6
0803458a  0023      movs	r3, #0
0803458c  4ff08044  mov.w	r4, #1073741824
08034590  294f      ldr	r7, [pc, #164] ; [0x08034638] = 0x40001800 / f32_bits_interpretation=2.001464844
08034592  4ff49670  mov.w	r0, #300
08034596  294e      ldr	r6, [pc, #164] ; [0x0803463c] = 0x40000800 / f32_bits_interpretation=2.000488281
08034598  7b63      str	r3, [r7, #52]
0803459a  e363      str	r3, [r4, #60]
0803459c  2364      str	r3, [r4, #64]
0803459e  f363      str	r3, [r6, #60]
080345a0  ebf702ff  bl	#-82428 ; -> 0x080203a8 ; branch_target=0x080203a8
080345a4  7f23      movs	r3, #127
080345a6  4ff49670  mov.w	r0, #300
080345aa  7b63      str	r3, [r7, #52]
080345ac  e363      str	r3, [r4, #60]
080345ae  2364      str	r3, [r4, #64]
080345b0  f363      str	r3, [r6, #60]
080345b2  ebf7f9fe  bl	#-82446 ; -> 0x080203a8 ; branch_target=0x080203a8
080345b6  4ff40072  mov.w	r2, #512
080345ba  2249      ldr	r1, [pc, #136] ; [0x08034644] = 0x30000040 / f32_bits_interpretation=4.6566484e-10
080345bc  2248      ldr	r0, [pc, #136] ; [0x08034648] = 0x200021f8
080345be  ecf72dfb  bl	#-80294 ; -> 0x08020c1c ; branch_target=0x08020c1c
080345c2  00b1      cbz	r0, #0 ; -> 0x080345c6 ; branch_target=0x080345c6
080345c4  fee7      b	#-4 ; -> 0x080345c4 ; branch_target=0x080345c4
080345c6  0622      movs	r2, #6
080345c8  2049      ldr	r1, [pc, #128] ; [0x0803464c] = 0x30000020 / f32_bits_interpretation=4.656630637e-10
080345ca  2148      ldr	r0, [pc, #132] ; [0x08034650] = 0x20002194
080345cc  ecf726fb  bl	#-80308 ; -> 0x08020c1c ; branch_target=0x08020c1c
080345d0  0028      cmp	r0, #0
080345d2  43d0      beq	#134 ; -> 0x0803465c ; branch_target=0x0803465c
080345d4  fee7      b	#-4 ; -> 0x080345d4 ; branch_target=0x080345d4
080345d6  1f4b      ldr	r3, [pc, #124] ; [0x08034654] = 0x20002ee0
080345d8  1b68      ldr	r3, [r3]
080345da  012b      cmp	r3, #1
080345dc  d5dc      bgt	#-86 ; -> 0x0803458a ; branch_target=0x0803458a
080345de  8021      movs	r1, #128
080345e0  1d48      ldr	r0, [pc, #116] ; [0x08034658] = 0x58021800
080345e2  eef70dff  bl	#-70118 ; -> 0x08023400 ; branch_target=0x08023400
080345e6  30b1      cbz	r0, #12 ; -> 0x080345f6 ; branch_target=0x080345f6
080345e8  4021      movs	r1, #64
080345ea  1b48      ldr	r0, [pc, #108] ; [0x08034658] = 0x58021800
080345ec  eef708ff  bl	#-70128 ; -> 0x08023400 ; branch_target=0x08023400
080345f0  0028      cmp	r0, #0
080345f2  40f0fb80  bne.w	#502 ; -> 0x080347ec ; branch_target=0x080347ec
080345f6  8021      movs	r1, #128
080345f8  1748      ldr	r0, [pc, #92] ; [0x08034658] = 0x58021800
080345fa  eef701ff  bl	#-70142 ; -> 0x08023400 ; branch_target=0x08023400
080345fe  4021      movs	r1, #64
08034600  3860      str	r0, [r7]
08034602  1548      ldr	r0, [pc, #84] ; [0x08034658] = 0x58021800
08034604  eef7fcfe  bl	#-70152 ; -> 0x08023400 ; branch_target=0x08023400
08034608  0028      cmp	r0, #0
0803460a  00f0d384  beq.w	#2470 ; -> 0x08034fb4 ; branch_target=0x08034fb4
0803460e  0323      movs	r3, #3
08034610  3b60      str	r3, [r7]
08034612  bae7      b	#-140 ; -> 0x0803458a ; branch_target=0x0803458a
0803465c  794c      ldr	r4, [pc, #484] ; [0x08034844] = 0x2001348c
0803465e  fef741fe  bl	#-4990 ; -> 0x080332e4 ; branch_target=0x080332e4
08034662  2388      ldrh	r3, [r4]
08034664  012b      cmp	r3, #1
08034666  01d9      bls	#2 ; -> 0x0803466c ; branch_target=0x0803466c
08034668  0123      movs	r3, #1
0803466a  2380      strh	r3, [r4]
0803466c  6388      ldrh	r3, [r4, #2]
0803466e  012b      cmp	r3, #1
08034670  01d9      bls	#2 ; -> 0x08034676 ; branch_target=0x08034676
08034672  0123      movs	r3, #1
08034674  6380      strh	r3, [r4, #2]
08034676  a368      ldr	r3, [r4, #8]
08034678  042b      cmp	r3, #4
0803467a  01d9      bls	#2 ; -> 0x08034680 ; branch_target=0x08034680
0803467c  0023      movs	r3, #0
0803467e  a360      str	r3, [r4, #8]
08034680  e368      ldr	r3, [r4, #12]
08034682  042b      cmp	r3, #4
08034684  01d9      bls	#2 ; -> 0x0803468a ; branch_target=0x0803468a
08034686  0023      movs	r3, #0
08034688  e360      str	r3, [r4, #12]
0803468a  2369      ldr	r3, [r4, #16]
0803468c  022b      cmp	r3, #2
0803468e  01d9      bls	#2 ; -> 0x08034694 ; branch_target=0x08034694
08034690  0023      movs	r3, #0
08034692  2361      str	r3, [r4, #16]
08034694  6369      ldr	r3, [r4, #20]
08034696  022b      cmp	r3, #2
08034698  01d9      bls	#2 ; -> 0x0803469e ; branch_target=0x0803469e
0803469a  0023      movs	r3, #0
0803469c  6361      str	r3, [r4, #20]
0803469e  236a      ldr	r3, [r4, #32]
080346a0  012b      cmp	r3, #1
080346a2  01d9      bls	#2 ; -> 0x080346a8 ; branch_target=0x080346a8
080346a4  0023      movs	r3, #0
080346a6  2362      str	r3, [r4, #32]
080346a8  636a      ldr	r3, [r4, #36]
080346aa  012b      cmp	r3, #1
080346ac  01d9      bls	#2 ; -> 0x080346b2 ; branch_target=0x080346b2
080346ae  0023      movs	r3, #0
080346b0  6362      str	r3, [r4, #36]
080346b2  a36a      ldr	r3, [r4, #40]
080346b4  0f2b      cmp	r3, #15
080346b6  01d9      bls	#2 ; -> 0x080346bc ; branch_target=0x080346bc
080346b8  0023      movs	r3, #0
080346ba  a362      str	r3, [r4, #40]
080346bc  e36a      ldr	r3, [r4, #44]
080346be  0f2b      cmp	r3, #15
080346c0  01d9      bls	#2 ; -> 0x080346c6 ; branch_target=0x080346c6
080346c2  0023      movs	r3, #0
080346c4  e362      str	r3, [r4, #44]
080346c6  236b      ldr	r3, [r4, #48]
080346c8  022b      cmp	r3, #2
080346ca  01d9      bls	#2 ; -> 0x080346d0 ; branch_target=0x080346d0
080346cc  0023      movs	r3, #0
080346ce  2363      str	r3, [r4, #48]
080346d0  5d4f      ldr	r7, [pc, #372] ; [0x08034848] = 0x081a0000
080346d2  2b68      ldr	r3, [r5]
080346d4  3a68      ldr	r2, [r7]
080346d6  9a42      cmp	r2, r3
080346d8  64d1      bne	#200 ; -> 0x080347a4 ; branch_target=0x080347a4
080346da  1aab      add	r3, sp, #104
080346dc  0793      str	r3, [sp, #28]
080346de  1bab      add	r3, sp, #108
080346e0  0893      str	r3, [sp, #32]
080346e2  20ab      add	r3, sp, #128
080346e4  0993      str	r3, [sp, #36]
080346e6  0a20      movs	r0, #10
080346e8  584c      ldr	r4, [pc, #352] ; [0x0803484c] = 0x20000a20
080346ea  ebf75dfe  bl	#-82758 ; -> 0x080203a8 ; branch_target=0x080203a8
080346ee  584b      ldr	r3, [pc, #352] ; [0x08034850] = 0x20002f74
080346f0  584a      ldr	r2, [pc, #352] ; [0x08034854] = 0x60c01000
080346f2  0021      movs	r1, #0
080346f4  5848      ldr	r0, [pc, #352] ; [0x08034858] = 0x60001000
080346f6  04f5807a  add.w	r10, r4, #256
080346fa  1a60      str	r2, [r3]
080346fc  0825      movs	r5, #8
080346fe  574b      ldr	r3, [pc, #348] ; [0x0803485c] = 0x20002f70
08034700  554a      ldr	r2, [pc, #340] ; [0x08034858] = 0x60001000
08034702  dff88891  ldr.w	r9, [pc, #392] ; [0x0803488c] = 0x20000920
08034706  1a60      str	r2, [r3]
08034708  4ff44002  mov.w	r2, #12582912
0803470c  01f0b9fe  bl	#7538 ; -> 0x08036482 ; branch_target=0x08036482
08034710  4ff44002  mov.w	r2, #12582912
08034714  0021      movs	r1, #0
08034716  4f48      ldr	r0, [pc, #316] ; [0x08034854] = 0x60c01000
08034718  01f0b3fe  bl	#7526 ; -> 0x08036482 ; branch_target=0x08036482
0803471c  0020      movs	r0, #0
0803471e  f5f7d9f8  bl	#-44622 ; -> 0x080298d4 ; branch_target=0x080298d4
08034722  0020      movs	r0, #0
08034724  f5f7ccf8  bl	#-44648 ; -> 0x080298c0 ; branch_target=0x080298c0
08034728  0122      movs	r2, #1
0803472a  4d49      ldr	r1, [pc, #308] ; [0x08034860] = 0x200033ec
0803472c  4d48      ldr	r0, [pc, #308] ; [0x08034864] = 0x200031bc
0803472e  4e4f      ldr	r7, [pc, #312] ; [0x08034868] = 0x08045a98
08034730  f6f7d4fb  bl	#-39000 ; -> 0x0802aedc ; branch_target=0x0802aedc
08034734  0126      movs	r6, #1
08034736  474b      ldr	r3, [pc, #284] ; [0x08034854] = 0x60c01000
08034738  4ff48042  mov.w	r2, #16384
0803473c  3946      mov	r1, r7
0803473e  e660      str	r6, [r4, #12]
08034740  c4e90155  strd	r5, r5, [r4, #4]
08034744  54f8100b  ldr	r0, [r4], #16
08034748  03eb8000  add.w	r0, r3, r0, lsl #2
0803474c  01f0c8fe  bl	#7568 ; -> 0x080364e0 ; branch_target=0x080364e0
08034750  414b      ldr	r3, [pc, #260] ; [0x08034858] = 0x60001000
08034752  c9f80c60  str.w	r6, [r9, #12]
08034756  4ff48042  mov.w	r2, #16384
0803475a  3946      mov	r1, r7
0803475c  c9e90155  strd	r5, r5, [r9, #4]
08034760  59f8100b  ldr	r0, [r9], #16
08034764  03eb8000  add.w	r0, r3, r0, lsl #2
08034768  01f0bafe  bl	#7540 ; -> 0x080364e0 ; branch_target=0x080364e0
0803476c  5445      cmp	r4, r10
0803476e  e1d1      bne	#-62 ; -> 0x08034734 ; branch_target=0x08034734
08034770  0022      movs	r2, #0
08034772  3e4b      ldr	r3, [pc, #248] ; [0x0803486c] = 0x20000b20
08034774  1021      movs	r1, #16
08034776  3e48      ldr	r0, [pc, #248] ; [0x08034870] = 0x58020400
08034778  1a60      str	r2, [r3]
0803477a  3e4b      ldr	r3, [pc, #248] ; [0x08034874] = 0x20002430
0803477c  1a60      str	r2, [r3]
0803477e  eef745fe  bl	#-70518 ; -> 0x0802340c ; branch_target=0x0802340c
08034782  fdf7e9ff  bl	#-8238 ; -> 0x08032758 ; branch_target=0x08032758
08034786  0028      cmp	r0, #0
08034788  40f08280  bne.w	#260 ; -> 0x08034890 ; branch_target=0x08034890
0803478c  0023      movs	r3, #0
0803478e  3a49      ldr	r1, [pc, #232] ; [0x08034878] = 0x20002edc
08034790  3a4a      ldr	r2, [pc, #232] ; [0x0803487c] = 0x20002ee4
08034792  3b48      ldr	r0, [pc, #236] ; [0x08034880] = 0x20014bb8
08034794  0b60      str	r3, [r1]
08034796  1360      str	r3, [r2]
08034798  f7f7a2fe  bl	#-33468 ; -> 0x0802c4e0 ; branch_target=0x0802c4e0
0803479c  0a20      movs	r0, #10
0803479e  ebf703fe  bl	#-82938 ; -> 0x080203a8 ; branch_target=0x080203a8
080347a2  fee7      b	#-4 ; -> 0x080347a2 ; branch_target=0x080347a2
080347a4  eef732fb  bl	#-72092 ; -> 0x08022e0c ; branch_target=0x08022e0c
080347a8  2b68      ldr	r3, [r5]
080347aa  0522      movs	r2, #5
080347ac  1aa9      add	r1, sp, #104
080347ae  2093      str	r3, [sp, #128]
080347b0  4ff00113  mov.w	r3, #65537
080347b4  0025      movs	r5, #0
080347b6  1ba8      add	r0, sp, #108
080347b8  6360      str	r3, [r4, #4]
080347ba  0223      movs	r3, #2
080347bc  0126      movs	r6, #1
080347be  0791      str	r1, [sp, #28]
080347c0  1b95      str	r5, [sp, #108]
080347c2  0890      str	r0, [sp, #32]
080347c4  1e96      str	r6, [sp, #120]
080347c6  cde91c32  strd	r3, r2, [sp, #112]
080347ca  2023      movs	r3, #32
080347cc  1f93      str	r3, [sp, #124]
080347ce  20ab      add	r3, sp, #128
080347d0  1c46      mov	r4, r3
080347d2  0993      str	r3, [sp, #36]
080347d4  eef712fc  bl	#-71644 ; -> 0x08022ffc ; branch_target=0x08022ffc
080347d8  3946      mov	r1, r7
080347da  2246      mov	r2, r4
080347dc  3046      mov	r0, r6
080347de  eef7b1fa  bl	#-72350 ; -> 0x08022d44 ; branch_target=0x08022d44
080347e2  eef739fb  bl	#-72078 ; -> 0x08022e58 ; branch_target=0x08022e58
080347e6  274b      ldr	r3, [pc, #156] ; [0x08034884] = 0x20002eb8
080347e8  1d60      str	r5, [r3]
080347ea  7ce7      b	#-264 ; -> 0x080346e6 ; branch_target=0x080346e6
080347ec  0a26      movs	r6, #10
080347ee  264c      ldr	r4, [pc, #152] ; [0x08034888] = 0x58020800
080347f0  f8f72afd  bl	#-30124 ; -> 0x0802d248 ; branch_target=0x0802d248
080347f4  0122      movs	r2, #1
080347f6  4021      movs	r1, #64
080347f8  2046      mov	r0, r4
080347fa  eef707fe  bl	#-70642 ; -> 0x0802340c ; branch_target=0x0802340c
080347fe  0022      movs	r2, #0
08034800  8021      movs	r1, #128
08034802  2046      mov	r0, r4
08034804  eef702fe  bl	#-70652 ; -> 0x0802340c ; branch_target=0x0802340c
08034808  6420      movs	r0, #100
0803480a  ebf7cdfd  bl	#-83046 ; -> 0x080203a8 ; branch_target=0x080203a8
0803480e  0022      movs	r2, #0
08034810  4021      movs	r1, #64
08034812  2046      mov	r0, r4
08034814  eef7fafd  bl	#-70668 ; -> 0x0802340c ; branch_target=0x0802340c
08034818  2046      mov	r0, r4
0803481a  0122      movs	r2, #1
0803481c  8021      movs	r1, #128
0803481e  eef7f5fd  bl	#-70678 ; -> 0x0802340c ; branch_target=0x0802340c
08034822  6420      movs	r0, #100
08034824  ebf7c0fd  bl	#-83072 ; -> 0x080203a8 ; branch_target=0x080203a8
08034828  013e      subs	r6, #1
0803482a  e3d1      bne	#-58 ; -> 0x080347f4 ; branch_target=0x080347f4
0803482c  0122      movs	r2, #1
0803482e  4021      movs	r1, #64
08034830  1548      ldr	r0, [pc, #84] ; [0x08034888] = 0x58020800
08034832  eef7ebfd  bl	#-70698 ; -> 0x0802340c ; branch_target=0x0802340c
08034836  0122      movs	r2, #1
08034838  8021      movs	r1, #128
0803483a  1348      ldr	r0, [pc, #76] ; [0x08034888] = 0x58020800
0803483c  eef7e6fd  bl	#-70708 ; -> 0x0802340c ; branch_target=0x0802340c
08034840  a3e6      b	#-698 ; -> 0x0803458a ; branch_target=0x0803458a
08034890  3246      mov	r2, r6
08034892  1021      movs	r1, #16
08034894  5a48      ldr	r0, [pc, #360] ; [0x08034a00] = 0x58020400
08034896  eef7b9fd  bl	#-70798 ; -> 0x0802340c ; branch_target=0x0802340c
0803489a  4ff49670  mov.w	r0, #300
0803489e  ebf783fd  bl	#-83194 ; -> 0x080203a8 ; branch_target=0x080203a8
080348a2  584b      ldr	r3, [pc, #352] ; [0x08034a04] = 0x08045a74
080348a4  584a      ldr	r2, [pc, #352] ; [0x08034a08] = 0x200033ec
080348a6  5949      ldr	r1, [pc, #356] ; [0x08034a0c] = 0x24000040
080348a8  5948      ldr	r0, [pc, #356] ; [0x08034a10] = 0x24000000
080348aa  f7f725fb  bl	#-35254 ; -> 0x0802bef8 ; branch_target=0x0802bef8
080348ae  dff85ca1  ldr.w	r10, [pc, #348] ; [0x08034a0c] = 0x24000040
080348b2  0646      mov	r6, r0
080348b4  4ff00a09  mov.w	r9, #10
080348b8  0027      movs	r7, #0
080348ba  0496      str	r6, [sp, #16]
080348bc  9af80e50  ldrb.w	r5, [r10, #14]
080348c0  4ff08044  mov.w	r4, #1073741824
080348c4  9af80fc0  ldrb.w	r12, [r10, #15]
080348c8  a5f13003  sub.w	r3, r5, #48
080348cc  9af810e0  ldrb.w	lr, [r10, #16]
080348d0  9af81120  ldrb.w	r2, [r10, #17]
080348d4  1946      mov	r1, r3
080348d6  0e93      str	r3, [sp, #56]
080348d8  acf13003  sub.w	r3, r12, #48
080348dc  0592      str	r2, [sp, #20]
080348de  9af81400  ldrb.w	r0, [r10, #20]
080348e2  09fb0133  mla	r3, r9, r1, r3
080348e6  9af80960  ldrb.w	r6, [r10, #9]
080348ea  0b90      str	r0, [sp, #44]
080348ec  1946      mov	r1, r3
080348ee  0f93      str	r3, [sp, #60]
080348f0  aef13003  sub.w	r3, lr, #48
080348f4  09fb0131  mla	r1, r9, r1, r3
080348f8  a2f13003  sub.w	r3, r2, #48
080348fc  09fb0132  mla	r2, r9, r1, r3
08034900  9af81230  ldrb.w	r3, [r10, #18]
08034904  1091      str	r1, [sp, #64]
08034906  0693      str	r3, [sp, #24]
08034908  303b      subs	r3, #48
0803490a  9af81310  ldrb.w	r1, [r10, #19]
0803490e  1192      str	r2, [sp, #68]
08034910  09fb0232  mla	r2, r9, r2, r3
08034914  a1f13003  sub.w	r3, r1, #48
08034918  0a91      str	r1, [sp, #40]
0803491a  1292      str	r2, [sp, #72]
0803491c  09fb0232  mla	r2, r9, r2, r3
08034920  a0f13003  sub.w	r3, r0, #48
08034924  1392      str	r2, [sp, #76]
08034926  09fb0231  mla	r1, r9, r2, r3
0803492a  9af81520  ldrb.w	r2, [r10, #21]
0803492e  394b      ldr	r3, [pc, #228] ; [0x08034a14] = 0x20000b20
08034930  0c92      str	r2, [sp, #48]
08034932  303a      subs	r2, #48
08034934  1491      str	r1, [sp, #80]
08034936  09fb0122  mla	r2, r9, r1, r2
0803493a  1b68      ldr	r3, [r3]
0803493c  7f21      movs	r1, #127
0803493e  0d92      str	r2, [sp, #52]
08034940  354a      ldr	r2, [pc, #212] ; [0x08034a18] = 0x20002430
08034942  1268      ldr	r2, [r2]
08034944  002e      cmp	r6, #0
08034946  6fd0      beq	#222 ; -> 0x08034a28 ; branch_target=0x08034a28
08034948  2e2d      cmp	r5, #46
0803494a  3ad0      beq	#116 ; -> 0x080349c2 ; branch_target=0x080349c2
0803494c  bcf12e0f  cmp.w	r12, #46
08034950  4bd0      beq	#150 ; -> 0x080349ea ; branch_target=0x080349ea
08034952  bef12e0f  cmp.w	lr, #46
08034956  4ad0      beq	#148 ; -> 0x080349ee ; branch_target=0x080349ee
08034958  059b      ldr	r3, [sp, #20]
0803495a  2e2b      cmp	r3, #46
0803495c  49d0      beq	#146 ; -> 0x080349f2 ; branch_target=0x080349f2
0803495e  069b      ldr	r3, [sp, #24]
08034960  2e2b      cmp	r3, #46
08034962  48d0      beq	#144 ; -> 0x080349f6 ; branch_target=0x080349f6
08034964  0a9b      ldr	r3, [sp, #40]
08034966  2e2b      cmp	r3, #46
08034968  47d0      beq	#142 ; -> 0x080349fa ; branch_target=0x080349fa
0803496a  0b9b      ldr	r3, [sp, #44]
0803496c  2e2b      cmp	r3, #46
0803496e  00f01583  beq.w	#1578 ; -> 0x08034f9c ; branch_target=0x08034f9c
08034972  0c9b      ldr	r3, [sp, #48]
08034974  2e2b      cmp	r3, #46
08034976  00f01b83  beq.w	#1590 ; -> 0x08034fb0 ; branch_target=0x08034fb0
0803497a  9af81620  ldrb.w	r2, [r10, #22]
0803497e  2e2a      cmp	r2, #46
08034980  00f01483  beq.w	#1576 ; -> 0x08034fac ; branch_target=0x08034fac
08034984  2548      ldr	r0, [pc, #148] ; [0x08034a1c] = 0x24000056
08034986  0d9b      ldr	r3, [sp, #52]
08034988  303a      subs	r2, #48
0803498a  09fb0323  mla	r3, r9, r3, r2
0803498e  10f8012f  ldrb	r2, [r0, #1]!
08034992  2e2a      cmp	r2, #46
08034994  f8d1      bne	#-16 ; -> 0x08034988 ; branch_target=0x08034988
08034996  0f2b      cmp	r3, #15
08034998  214a      ldr	r2, [pc, #132] ; [0x08034a20] = 0x40001800 / f32_bits_interpretation=2.001464844
0803499a  a8bf      it	ge
0803499c  0f23      movge	r3, #15
0803499e  5163      str	r1, [r2, #52]
080349a0  a2f58052  sub.w	r2, r2, #4096
080349a4  2164      str	r1, [r4, #64]
080349a6  e163      str	r1, [r4, #60]
080349a8  d163      str	r1, [r2, #60]
080349aa  13f00302  ands	r2, r3, #3
080349ae  12d0      beq	#36 ; -> 0x080349d6 ; branch_target=0x080349d6
080349b0  012a      cmp	r2, #1
080349b2  13d1      bne	#38 ; -> 0x080349dc ; branch_target=0x080349dc
080349b4  2764      str	r7, [r4, #64]
080349b6  049a      ldr	r2, [sp, #16]
080349b8  002a      cmp	r2, #0
080349ba  00f04f82  beq.w	#1182 ; -> 0x08034e5c ; branch_target=0x08034e5c
080349be  1a46      mov	r2, r3
080349c0  c0e7      b	#-128 ; -> 0x08034944 ; branch_target=0x08034944
080349c2  7f23      movs	r3, #127
080349c4  164a      ldr	r2, [pc, #88] ; [0x08034a20] = 0x40001800 / f32_bits_interpretation=2.001464844
080349c6  5363      str	r3, [r2, #52]
080349c8  4ff08042  mov.w	r2, #1073741824
080349cc  1364      str	r3, [r2, #64]
080349ce  d363      str	r3, [r2, #60]
080349d0  c2f83c38  str.w	r3, [r2, #2108]
080349d4  0023      movs	r3, #0
080349d6  124a      ldr	r2, [pc, #72] ; [0x08034a20] = 0x40001800 / f32_bits_interpretation=2.001464844
080349d8  5763      str	r7, [r2, #52]
080349da  ece7      b	#-40 ; -> 0x080349b6 ; branch_target=0x080349b6
080349dc  022a      cmp	r2, #2
080349de  01d1      bne	#2 ; -> 0x080349e4 ; branch_target=0x080349e4
080349e0  e763      str	r7, [r4, #60]
080349e2  e8e7      b	#-48 ; -> 0x080349b6 ; branch_target=0x080349b6
080349e4  0f4a      ldr	r2, [pc, #60] ; [0x08034a24] = 0x40000800 / f32_bits_interpretation=2.000488281
080349e6  d763      str	r7, [r2, #60]
080349e8  e5e7      b	#-54 ; -> 0x080349b6 ; branch_target=0x080349b6
080349ea  0e9b      ldr	r3, [sp, #56]
080349ec  d3e7      b	#-90 ; -> 0x08034996 ; branch_target=0x08034996
080349ee  0f9b      ldr	r3, [sp, #60]
080349f0  d1e7      b	#-94 ; -> 0x08034996 ; branch_target=0x08034996
080349f2  109b      ldr	r3, [sp, #64]
080349f4  cfe7      b	#-98 ; -> 0x08034996 ; branch_target=0x08034996
080349f6  119b      ldr	r3, [sp, #68]
080349f8  cde7      b	#-102 ; -> 0x08034996 ; branch_target=0x08034996
080349fa  129b      ldr	r3, [sp, #72]
080349fc  cbe7      b	#-106 ; -> 0x08034996 ; branch_target=0x08034996
08034a28  1846      mov	r0, r3
08034a2a  774b      ldr	r3, [pc, #476] ; [0x08034c08] = 0x20002430
08034a2c  7749      ldr	r1, [pc, #476] ; [0x08034c0c] = 0x24000040
08034a2e  1a60      str	r2, [r3]
08034a30  774b      ldr	r3, [pc, #476] ; [0x08034c10] = 0x20000b20
08034a32  784a      ldr	r2, [pc, #480] ; [0x08034c14] = 0x200033ec
08034a34  1860      str	r0, [r3]
08034a36  784b      ldr	r3, [pc, #480] ; [0x08034c18] = 0x08045a80
08034a38  7848      ldr	r0, [pc, #480] ; [0x08034c1c] = 0x24000000
08034a3a  f7f75dfa  bl	#-35654 ; -> 0x0802bef8 ; branch_target=0x0802bef8
08034a3e  dff8f091  ldr.w	r9, [pc, #496] ; [0x08034c30] = 0x40000800 / f32_bits_interpretation=2.000488281
08034a42  0646      mov	r6, r0
08034a44  0a27      movs	r7, #10
08034a46  0496      str	r6, [sp, #16]
08034a48  9af80e50  ldrb.w	r5, [r10, #14]
08034a4c  9af80f60  ldrb.w	r6, [r10, #15]
08034a50  a5f13003  sub.w	r3, r5, #48
08034a54  9af810c0  ldrb.w	r12, [r10, #16]
08034a58  9af81120  ldrb.w	r2, [r10, #17]
08034a5c  1946      mov	r1, r3
08034a5e  1193      str	r3, [sp, #68]
08034a60  a6f13003  sub.w	r3, r6, #48
08034a64  9af812e0  ldrb.w	lr, [r10, #18]
08034a68  0692      str	r2, [sp, #24]
08034a6a  07fb0133  mla	r3, r7, r1, r3
08034a6e  9af81400  ldrb.w	r0, [r10, #20]
08034a72  1946      mov	r1, r3
08034a74  1293      str	r3, [sp, #72]
08034a76  acf13003  sub.w	r3, r12, #48
08034a7a  0a90      str	r0, [sp, #40]
08034a7c  07fb0131  mla	r1, r7, r1, r3
08034a80  a2f13003  sub.w	r3, r2, #48
08034a84  1391      str	r1, [sp, #76]
08034a86  07fb0132  mla	r2, r7, r1, r3
08034a8a  9af81310  ldrb.w	r1, [r10, #19]
08034a8e  aef13003  sub.w	r3, lr, #48
08034a92  1492      str	r2, [sp, #80]
08034a94  07fb0232  mla	r2, r7, r2, r3
08034a98  a1f13003  sub.w	r3, r1, #48
08034a9c  0b91      str	r1, [sp, #44]
08034a9e  1592      str	r2, [sp, #84]
08034aa0  07fb0232  mla	r2, r7, r2, r3
08034aa4  a0f13003  sub.w	r3, r0, #48
08034aa8  07fb0231  mla	r1, r7, r2, r3
08034aac  9af81530  ldrb.w	r3, [r10, #21]
08034ab0  0f92      str	r2, [sp, #60]
08034ab2  1a46      mov	r2, r3
08034ab4  0c93      str	r3, [sp, #48]
08034ab6  9af80930  ldrb.w	r3, [r10, #9]
08034aba  303a      subs	r2, #48
08034abc  1091      str	r1, [sp, #64]
08034abe  07fb0122  mla	r2, r7, r1, r2
08034ac2  0d92      str	r2, [sp, #52]
08034ac4  002b      cmp	r3, #0
08034ac6  00f0b580  beq.w	#362 ; -> 0x08034c34 ; branch_target=0x08034c34
08034aca  7f23      movs	r3, #127
08034acc  4ff08041  mov.w	r1, #1073741824
08034ad0  0024      movs	r4, #0
08034ad2  2e2d      cmp	r5, #46
08034ad4  7cd0      beq	#248 ; -> 0x08034bd0 ; branch_target=0x08034bd0
08034ad6  2e2e      cmp	r6, #46
08034ad8  00f08b80  beq.w	#278 ; -> 0x08034bf2 ; branch_target=0x08034bf2
08034adc  bcf12e0f  cmp.w	r12, #46
08034ae0  00f08980  beq.w	#274 ; -> 0x08034bf6 ; branch_target=0x08034bf6
08034ae4  069a      ldr	r2, [sp, #24]
08034ae6  2e2a      cmp	r2, #46
08034ae8  00f08780  beq.w	#270 ; -> 0x08034bfa ; branch_target=0x08034bfa
08034aec  bef12e0f  cmp.w	lr, #46
08034af0  00f08580  beq.w	#266 ; -> 0x08034bfe ; branch_target=0x08034bfe
08034af4  0b9a      ldr	r2, [sp, #44]
08034af6  2e2a      cmp	r2, #46
08034af8  00f08380  beq.w	#262 ; -> 0x08034c02 ; branch_target=0x08034c02
08034afc  0a9a      ldr	r2, [sp, #40]
08034afe  2e2a      cmp	r2, #46
08034b00  00f04e82  beq.w	#1180 ; -> 0x08034fa0 ; branch_target=0x08034fa0
08034b04  0c9a      ldr	r2, [sp, #48]
08034b06  2e2a      cmp	r2, #46
08034b08  00f04c82  beq.w	#1176 ; -> 0x08034fa4 ; branch_target=0x08034fa4
08034b0c  9af81600  ldrb.w	r0, [r10, #22]
08034b10  2e28      cmp	r0, #46
08034b12  00f04982  beq.w	#1170 ; -> 0x08034fa8 ; branch_target=0x08034fa8
08034b16  424a      ldr	r2, [pc, #264] ; [0x08034c20] = 0x24000056
08034b18  0e95      str	r5, [sp, #56]
08034b1a  0592      str	r2, [sp, #20]
08034b1c  0d9a      ldr	r2, [sp, #52]
08034b1e  3038      subs	r0, #48
08034b20  059d      ldr	r5, [sp, #20]
08034b22  07fb0202  mla	r2, r7, r2, r0
08034b26  15f8010f  ldrb	r0, [r5, #1]!
08034b2a  2e28      cmp	r0, #46
08034b2c  0595      str	r5, [sp, #20]
08034b2e  f6d1      bne	#-20 ; -> 0x08034b1e ; branch_target=0x08034b1e
08034b30  0e9d      ldr	r5, [sp, #56]
08034b32  3c48      ldr	r0, [pc, #240] ; [0x08034c24] = 0x40001800 / f32_bits_interpretation=2.001464844
08034b34  4363      str	r3, [r0, #52]
08034b36  12f00300  ands	r0, r2, #3
08034b3a  0b64      str	r3, [r1, #64]
08034b3c  cb63      str	r3, [r1, #60]
08034b3e  c9f83c30  str.w	r3, [r9, #60]
08034b42  4cd0      beq	#152 ; -> 0x08034bde ; branch_target=0x08034bde
08034b44  0128      cmp	r0, #1
08034b46  4dd1      bne	#154 ; -> 0x08034be4 ; branch_target=0x08034be4
08034b48  0c64      str	r4, [r1, #64]
08034b4a  0498      ldr	r0, [sp, #16]
08034b4c  0028      cmp	r0, #0
08034b4e  c0d1      bne	#-128 ; -> 0x08034ad2 ; branch_target=0x08034ad2
08034b50  2d4b      ldr	r3, [pc, #180] ; [0x08034c08] = 0x20002430
08034b52  3549      ldr	r1, [pc, #212] ; [0x08034c28] = 0x24000049
08034b54  1a60      str	r2, [r3]
08034b56  1122      movs	r2, #17
08034b58  3448      ldr	r0, [pc, #208] ; [0x08034c2c] = 0x20002f8c
08034b5a  f6f70bfa  bl	#-39914 ; -> 0x0802af74 ; branch_target=0x0802af74
08034b5e  0646      mov	r6, r0
08034b60  0028      cmp	r0, #0
08034b62  7ff471af  bne.w	#-286 ; -> 0x08034a48 ; branch_target=0x08034a48
08034b66  099b      ldr	r3, [sp, #36]
08034b68  17aa      add	r2, sp, #92
08034b6a  16a9      add	r1, sp, #88
08034b6c  2f48      ldr	r0, [pc, #188] ; [0x08034c2c] = 0x20002f8c
08034b6e  0393      str	r3, [sp, #12]
08034b70  089b      ldr	r3, [sp, #32]
08034b72  0293      str	r3, [sp, #8]
08034b74  079b      ldr	r3, [sp, #28]
08034b76  0193      str	r3, [sp, #4]
08034b78  19ab      add	r3, sp, #100
08034b7a  0093      str	r3, [sp]
08034b7c  18ab      add	r3, sp, #96
08034b7e  fef797ff  bl	#-4306 ; -> 0x08033ab0 ; branch_target=0x08033ab0
08034b82  0446      mov	r4, r0
08034b84  0028      cmp	r0, #0
08034b86  7ff45eaf  bne.w	#-324 ; -> 0x08034a46 ; branch_target=0x08034a46
08034b8a  dde91763  ldrd	r6, r3, [sp, #92]
08034b8e  1a9d      ldr	r5, [sp, #104]
08034b90  06fb03f3  mul	r3, r6, r3
08034b94  1b9f      ldr	r7, [sp, #108]
08034b96  2548      ldr	r0, [pc, #148] ; [0x08034c2c] = 0x20002f8c
08034b98  002b      cmp	r3, #0
08034b9a  27f00301  bic	r1, r7, #3
08034b9e  b8bf      it	lt
08034ba0  0733      addlt	r3, #7
08034ba2  db10      asrs	r3, r3, #3
08034ba4  95fbf3f5  sdiv	r5, r5, r3
08034ba8  f6f7ccfe  bl	#-37480 ; -> 0x0802b944 ; branch_target=0x0802b944
08034bac  402d      cmp	r5, #64
08034bae  00f31282  bgt.w	#1060 ; -> 0x08034fd6 ; branch_target=0x08034fd6
08034bb2  1e48      ldr	r0, [pc, #120] ; [0x08034c2c] = 0x20002f8c
08034bb4  f6f744fe  bl	#-37752 ; -> 0x0802b840 ; branch_target=0x0802b840
08034bb8  134a      ldr	r2, [pc, #76] ; [0x08034c08] = 0x20002430
08034bba  1449      ldr	r1, [pc, #80] ; [0x08034c0c] = 0x24000040
08034bbc  1368      ldr	r3, [r2]
08034bbe  1748      ldr	r0, [pc, #92] ; [0x08034c1c] = 0x24000000
08034bc0  0133      adds	r3, #1
08034bc2  03f00f03  and	r3, r3, #15
08034bc6  1360      str	r3, [r2]
08034bc8  f7f7ccf8  bl	#-36456 ; -> 0x0802bd64 ; branch_target=0x0802bd64
08034bcc  0646      mov	r6, r0
08034bce  39e7      b	#-398 ; -> 0x08034a44 ; branch_target=0x08034a44
08034bd0  144a      ldr	r2, [pc, #80] ; [0x08034c24] = 0x40001800 / f32_bits_interpretation=2.001464844
08034bd2  5363      str	r3, [r2, #52]
08034bd4  0022      movs	r2, #0
08034bd6  0b64      str	r3, [r1, #64]
08034bd8  cb63      str	r3, [r1, #60]
08034bda  c9f83c30  str.w	r3, [r9, #60]
08034bde  1148      ldr	r0, [pc, #68] ; [0x08034c24] = 0x40001800 / f32_bits_interpretation=2.001464844
08034be0  4463      str	r4, [r0, #52]
08034be2  b2e7      b	#-156 ; -> 0x08034b4a ; branch_target=0x08034b4a
08034be4  0228      cmp	r0, #2
08034be6  01d1      bne	#2 ; -> 0x08034bec ; branch_target=0x08034bec
08034be8  cc63      str	r4, [r1, #60]
08034bea  aee7      b	#-164 ; -> 0x08034b4a ; branch_target=0x08034b4a
08034bec  c9f83c40  str.w	r4, [r9, #60]
08034bf0  abe7      b	#-170 ; -> 0x08034b4a ; branch_target=0x08034b4a
08034bf2  119a      ldr	r2, [sp, #68]
08034bf4  9de7      b	#-198 ; -> 0x08034b32 ; branch_target=0x08034b32
08034bf6  129a      ldr	r2, [sp, #72]
08034bf8  9be7      b	#-202 ; -> 0x08034b32 ; branch_target=0x08034b32
08034bfa  139a      ldr	r2, [sp, #76]
08034bfc  99e7      b	#-206 ; -> 0x08034b32 ; branch_target=0x08034b32
08034bfe  149a      ldr	r2, [sp, #80]
08034c00  97e7      b	#-210 ; -> 0x08034b32 ; branch_target=0x08034b32
08034c02  159a      ldr	r2, [sp, #84]
08034c04  95e7      b	#-214 ; -> 0x08034b32 ; branch_target=0x08034b32
08034c34  7b4b      ldr	r3, [pc, #492] ; [0x08034e24] = 0x08045a8c
08034c36  7c4a      ldr	r2, [pc, #496] ; [0x08034e28] = 0x200033ec
08034c38  7c49      ldr	r1, [pc, #496] ; [0x08034e2c] = 0x24000040
08034c3a  7d48      ldr	r0, [pc, #500] ; [0x08034e30] = 0x24000000
08034c3c  f7f75cf9  bl	#-36168 ; -> 0x0802bef8 ; branch_target=0x0802bef8
08034c40  0646      mov	r6, r0
08034c42  4ff00a09  mov.w	r9, #10
08034c46  3746      mov	r7, r6
08034c48  9af80e50  ldrb.w	r5, [r10, #14]
08034c4c  9af80f60  ldrb.w	r6, [r10, #15]
08034c50  a5f13003  sub.w	r3, r5, #48
08034c54  9af810c0  ldrb.w	r12, [r10, #16]
08034c58  9af811e0  ldrb.w	lr, [r10, #17]
08034c5c  1a46      mov	r2, r3
08034c5e  0f93      str	r3, [sp, #60]
08034c60  a6f13003  sub.w	r3, r6, #48
08034c64  9af81280  ldrb.w	r8, [r10, #18]
08034c68  9af81310  ldrb.w	r1, [r10, #19]
08034c6c  09fb0233  mla	r3, r9, r2, r3
08034c70  9af81400  ldrb.w	r0, [r10, #20]
08034c74  0591      str	r1, [sp, #20]
08034c76  1a46      mov	r2, r3
08034c78  1093      str	r3, [sp, #64]
08034c7a  acf13003  sub.w	r3, r12, #48
08034c7e  0b90      str	r0, [sp, #44]
08034c80  09fb0233  mla	r3, r9, r2, r3
08034c84  1a46      mov	r2, r3
08034c86  0d93      str	r3, [sp, #52]
08034c88  aef13003  sub.w	r3, lr, #48
08034c8c  09fb0232  mla	r2, r9, r2, r3
08034c90  a8f13003  sub.w	r3, r8, #48
08034c94  0e92      str	r2, [sp, #56]
08034c96  09fb0232  mla	r2, r9, r2, r3
08034c9a  a1f13003  sub.w	r3, r1, #48
08034c9e  1192      str	r2, [sp, #68]
08034ca0  09fb0232  mla	r2, r9, r2, r3
08034ca4  a0f13003  sub.w	r3, r0, #48
08034ca8  09fb0231  mla	r1, r9, r2, r3
08034cac  9af81530  ldrb.w	r3, [r10, #21]
08034cb0  1292      str	r2, [sp, #72]
08034cb2  1a46      mov	r2, r3
08034cb4  0a93      str	r3, [sp, #40]
08034cb6  9af80930  ldrb.w	r3, [r10, #9]
08034cba  303a      subs	r2, #48
08034cbc  1391      str	r1, [sp, #76]
08034cbe  09fb0122  mla	r2, r9, r1, r2
08034cc2  0692      str	r2, [sp, #24]
08034cc4  002b      cmp	r3, #0
08034cc6  00f0a080  beq.w	#320 ; -> 0x08034e0a ; branch_target=0x08034e0a
08034cca  7f23      movs	r3, #127
08034ccc  4ff08041  mov.w	r1, #1073741824
08034cd0  0024      movs	r4, #0
08034cd2  2e2d      cmp	r5, #46
08034cd4  7dd0      beq	#250 ; -> 0x08034dd2 ; branch_target=0x08034dd2
08034cd6  2e2e      cmp	r6, #46
08034cd8  00f08d80  beq.w	#282 ; -> 0x08034df6 ; branch_target=0x08034df6
08034cdc  bcf12e0f  cmp.w	r12, #46
08034ce0  00f08b80  beq.w	#278 ; -> 0x08034dfa ; branch_target=0x08034dfa
08034ce4  bef12e0f  cmp.w	lr, #46
08034ce8  00f08980  beq.w	#274 ; -> 0x08034dfe ; branch_target=0x08034dfe
08034cec  b8f12e0f  cmp.w	r8, #46
08034cf0  00f08780  beq.w	#270 ; -> 0x08034e02 ; branch_target=0x08034e02
08034cf4  059a      ldr	r2, [sp, #20]
08034cf6  2e2a      cmp	r2, #46
08034cf8  00f08580  beq.w	#266 ; -> 0x08034e06 ; branch_target=0x08034e06
08034cfc  0b9a      ldr	r2, [sp, #44]
08034cfe  2e2a      cmp	r2, #46
08034d00  00f08d80  beq.w	#282 ; -> 0x08034e1e ; branch_target=0x08034e1e
08034d04  0a9a      ldr	r2, [sp, #40]
08034d06  2e2a      cmp	r2, #46
08034d08  00f06181  beq.w	#706 ; -> 0x08034fce ; branch_target=0x08034fce
08034d0c  9af81600  ldrb.w	r0, [r10, #22]
08034d10  2e28      cmp	r0, #46
08034d12  00f05e81  beq.w	#700 ; -> 0x08034fd2 ; branch_target=0x08034fd2
08034d16  474a      ldr	r2, [pc, #284] ; [0x08034e34] = 0x24000056
08034d18  0c95      str	r5, [sp, #48]
08034d1a  0492      str	r2, [sp, #16]
08034d1c  069a      ldr	r2, [sp, #24]
08034d1e  3038      subs	r0, #48
08034d20  049d      ldr	r5, [sp, #16]
08034d22  09fb0202  mla	r2, r9, r2, r0
08034d26  15f8010f  ldrb	r0, [r5, #1]!
08034d2a  2e28      cmp	r0, #46
08034d2c  0495      str	r5, [sp, #16]
08034d2e  f6d1      bne	#-20 ; -> 0x08034d1e ; branch_target=0x08034d1e
08034d30  0c9d      ldr	r5, [sp, #48]
08034d32  4148      ldr	r0, [pc, #260] ; [0x08034e38] = 0x40001800 / f32_bits_interpretation=2.001464844
08034d34  4363      str	r3, [r0, #52]
08034d36  a0f58050  sub.w	r0, r0, #4096
08034d3a  0b64      str	r3, [r1, #64]
08034d3c  cb63      str	r3, [r1, #60]
08034d3e  c363      str	r3, [r0, #60]
08034d40  12f00300  ands	r0, r2, #3
08034d44  4dd0      beq	#154 ; -> 0x08034de2 ; branch_target=0x08034de2
08034d46  0128      cmp	r0, #1
08034d48  4ed1      bne	#156 ; -> 0x08034de8 ; branch_target=0x08034de8
08034d4a  0c64      str	r4, [r1, #64]
08034d4c  002f      cmp	r7, #0
08034d4e  c0d1      bne	#-128 ; -> 0x08034cd2 ; branch_target=0x08034cd2
08034d50  3a4b      ldr	r3, [pc, #232] ; [0x08034e3c] = 0x20000b20
08034d52  3b49      ldr	r1, [pc, #236] ; [0x08034e40] = 0x24000049
08034d54  1a60      str	r2, [r3]
08034d56  1122      movs	r2, #17
08034d58  3a48      ldr	r0, [pc, #232] ; [0x08034e44] = 0x20002f8c
08034d5a  f6f70bf9  bl	#-40426 ; -> 0x0802af74 ; branch_target=0x0802af74
08034d5e  0646      mov	r6, r0
08034d60  0028      cmp	r0, #0
08034d62  7ff471af  bne.w	#-286 ; -> 0x08034c48 ; branch_target=0x08034c48
08034d66  099b      ldr	r3, [sp, #36]
08034d68  17aa      add	r2, sp, #92
08034d6a  16a9      add	r1, sp, #88
08034d6c  3548      ldr	r0, [pc, #212] ; [0x08034e44] = 0x20002f8c
08034d6e  0393      str	r3, [sp, #12]
08034d70  089b      ldr	r3, [sp, #32]
08034d72  0293      str	r3, [sp, #8]
08034d74  079b      ldr	r3, [sp, #28]
08034d76  0193      str	r3, [sp, #4]
08034d78  19ab      add	r3, sp, #100
08034d7a  0093      str	r3, [sp]
08034d7c  18ab      add	r3, sp, #96
08034d7e  fef797fe  bl	#-4818 ; -> 0x08033ab0 ; branch_target=0x08033ab0
08034d82  0446      mov	r4, r0
08034d84  0028      cmp	r0, #0
08034d86  7ff45eaf  bne.w	#-324 ; -> 0x08034c46 ; branch_target=0x08034c46
08034d8a  dde91773  ldrd	r7, r3, [sp, #92]
08034d8e  1a9d      ldr	r5, [sp, #104]
08034d90  07fb03f3  mul	r3, r7, r3
08034d94  ddf86c80  ldr.w	r8, [sp, #108]
08034d98  2a48      ldr	r0, [pc, #168] ; [0x08034e44] = 0x20002f8c
08034d9a  002b      cmp	r3, #0
08034d9c  28f00301  bic	r1, r8, #3
08034da0  b8bf      it	lt
08034da2  0733      addlt	r3, #7
08034da4  db10      asrs	r3, r3, #3
08034da6  95fbf3f5  sdiv	r5, r5, r3
08034daa  f6f7cbfd  bl	#-37994 ; -> 0x0802b944 ; branch_target=0x0802b944
08034dae  402d      cmp	r5, #64
08034db0  00f34c81  bgt.w	#664 ; -> 0x0803504c ; branch_target=0x0803504c
08034db4  2348      ldr	r0, [pc, #140] ; [0x08034e44] = 0x20002f8c
08034db6  f6f743fd  bl	#-38266 ; -> 0x0802b840 ; branch_target=0x0802b840
08034dba  204a      ldr	r2, [pc, #128] ; [0x08034e3c] = 0x20000b20
08034dbc  1b49      ldr	r1, [pc, #108] ; [0x08034e2c] = 0x24000040
08034dbe  1368      ldr	r3, [r2]
08034dc0  1b48      ldr	r0, [pc, #108] ; [0x08034e30] = 0x24000000
08034dc2  0133      adds	r3, #1
08034dc4  03f00f03  and	r3, r3, #15
08034dc8  1360      str	r3, [r2]
08034dca  f6f7cbff  bl	#-36970 ; -> 0x0802bd64 ; branch_target=0x0802bd64
08034dce  0646      mov	r6, r0
08034dd0  37e7      b	#-402 ; -> 0x08034c42 ; branch_target=0x08034c42
08034dd2  194a      ldr	r2, [pc, #100] ; [0x08034e38] = 0x40001800 / f32_bits_interpretation=2.001464844
08034dd4  5363      str	r3, [r2, #52]
08034dd6  a2f58052  sub.w	r2, r2, #4096
08034dda  0b64      str	r3, [r1, #64]
08034ddc  cb63      str	r3, [r1, #60]
08034dde  d363      str	r3, [r2, #60]
08034de0  0022      movs	r2, #0
08034de2  1548      ldr	r0, [pc, #84] ; [0x08034e38] = 0x40001800 / f32_bits_interpretation=2.001464844
08034de4  4463      str	r4, [r0, #52]
08034de6  b1e7      b	#-158 ; -> 0x08034d4c ; branch_target=0x08034d4c
08034de8  0228      cmp	r0, #2
08034dea  01d1      bne	#2 ; -> 0x08034df0 ; branch_target=0x08034df0
08034dec  cc63      str	r4, [r1, #60]
08034dee  ade7      b	#-166 ; -> 0x08034d4c ; branch_target=0x08034d4c
08034df0  1548      ldr	r0, [pc, #84] ; [0x08034e48] = 0x40000800 / f32_bits_interpretation=2.000488281
08034df2  c463      str	r4, [r0, #60]
08034df4  aae7      b	#-172 ; -> 0x08034d4c ; branch_target=0x08034d4c
08034df6  0f9a      ldr	r2, [sp, #60]
08034df8  9be7      b	#-202 ; -> 0x08034d32 ; branch_target=0x08034d32
08034dfa  109a      ldr	r2, [sp, #64]
08034dfc  99e7      b	#-206 ; -> 0x08034d32 ; branch_target=0x08034d32
08034dfe  0d9a      ldr	r2, [sp, #52]
08034e00  97e7      b	#-210 ; -> 0x08034d32 ; branch_target=0x08034d32
08034e02  0e9a      ldr	r2, [sp, #56]
08034e04  95e7      b	#-214 ; -> 0x08034d32 ; branch_target=0x08034d32
08034e06  119a      ldr	r2, [sp, #68]
08034e08  93e7      b	#-218 ; -> 0x08034d32 ; branch_target=0x08034d32
08034e0a  104b      ldr	r3, [pc, #64] ; [0x08034e4c] = 0x20002f74
08034e0c  104a      ldr	r2, [pc, #64] ; [0x08034e50] = 0x60c01000
08034e0e  0848      ldr	r0, [pc, #32] ; [0x08034e30] = 0x24000000
08034e10  1a60      str	r2, [r3]
08034e12  104b      ldr	r3, [pc, #64] ; [0x08034e54] = 0x20002f70
08034e14  104a      ldr	r2, [pc, #64] ; [0x08034e58] = 0x60001000
08034e16  1a60      str	r2, [r3]
08034e18  f6f76eff  bl	#-37156 ; -> 0x0802bcf8 ; branch_target=0x0802bcf8
08034e1c  b6e4      b	#-1684 ; -> 0x0803478c ; branch_target=0x0803478c
08034e1e  129a      ldr	r2, [sp, #72]
08034e20  87e7      b	#-242 ; -> 0x08034d32 ; branch_target=0x08034d32
08034e5c  9b4a      ldr	r2, [pc, #620] ; [0x080350cc] = 0x20002430
08034e5e  9c49      ldr	r1, [pc, #624] ; [0x080350d0] = 0x24000049
08034e60  1360      str	r3, [r2]
08034e62  9c4a      ldr	r2, [pc, #624] ; [0x080350d4] = 0x20000b20
08034e64  9c48      ldr	r0, [pc, #624] ; [0x080350d8] = 0x20002f8c
08034e66  1360      str	r3, [r2]
08034e68  1122      movs	r2, #17
08034e6a  f6f783f8  bl	#-40698 ; -> 0x0802af74 ; branch_target=0x0802af74
08034e6e  0646      mov	r6, r0
08034e70  0028      cmp	r0, #0
08034e72  7ff423ad  bne.w	#-1466 ; -> 0x080348bc ; branch_target=0x080348bc
08034e76  099b      ldr	r3, [sp, #36]
08034e78  17aa      add	r2, sp, #92
08034e7a  16a9      add	r1, sp, #88
08034e7c  9648      ldr	r0, [pc, #600] ; [0x080350d8] = 0x20002f8c
08034e7e  0393      str	r3, [sp, #12]
08034e80  089b      ldr	r3, [sp, #32]
08034e82  0293      str	r3, [sp, #8]
08034e84  079b      ldr	r3, [sp, #28]
08034e86  0193      str	r3, [sp, #4]
08034e88  19ab      add	r3, sp, #100
08034e8a  0093      str	r3, [sp]
08034e8c  18ab      add	r3, sp, #96
08034e8e  fef70ffe  bl	#-5090 ; -> 0x08033ab0 ; branch_target=0x08033ab0
08034e92  0028      cmp	r0, #0
08034e94  7ff410ad  bne.w	#-1504 ; -> 0x080348b8 ; branch_target=0x080348b8
08034e98  dde91773  ldrd	r7, r3, [sp, #92]
08034e9c  1a9d      ldr	r5, [sp, #104]
08034e9e  07fb03f3  mul	r3, r7, r3
08034ea2  ddf86c90  ldr.w	r9, [sp, #108]
08034ea6  0490      str	r0, [sp, #16]
08034ea8  002b      cmp	r3, #0
08034eaa  8b48      ldr	r0, [pc, #556] ; [0x080350d8] = 0x20002f8c
08034eac  29f00301  bic	r1, r9, #3
08034eb0  b8bf      it	lt
08034eb2  0733      addlt	r3, #7
08034eb4  db10      asrs	r3, r3, #3
08034eb6  95fbf3f5  sdiv	r5, r5, r3
08034eba  f6f743fd  bl	#-38266 ; -> 0x0802b944 ; branch_target=0x0802b944
08034ebe  402d      cmp	r5, #64
08034ec0  0646      mov	r6, r0
08034ec2  7ff7f7ac  ble.w	#-1554 ; -> 0x080348b4 ; branch_target=0x080348b4
08034ec6  814b      ldr	r3, [pc, #516] ; [0x080350cc] = 0x20002430
08034ec8  b5f5805f  cmp.w	r5, #4096
08034ecc  1a68      ldr	r2, [r3]
08034ece  814b      ldr	r3, [pc, #516] ; [0x080350d4] = 0x20000b20
08034ed0  1b68      ldr	r3, [r3]
08034ed2  00f0f880  beq.w	#496 ; -> 0x080350c6 ; branch_target=0x080350c6
08034ed6  a911      asrs	r1, r5, #6
08034ed8  0126      movs	r6, #1
08034eda  1001      lsls	r0, r2, #4
08034edc  7f4c      ldr	r4, [pc, #508] ; [0x080350dc] = 0x200134c4
08034ede  08eb0212  add.w	r2, r8, r2, lsl #4
08034ee2  58f80000  ldr.w	r0, [r8, r0]
08034ee6  2060      str	r0, [r4]
08034ee8  1801      lsls	r0, r3, #4
08034eea  0beb0313  add.w	r3, r11, r3, lsl #4
08034eee  049c      ldr	r4, [sp, #16]
08034ef0  c2e90116  strd	r1, r6, [r2, #4]
08034ef4  c3e90116  strd	r1, r6, [r3, #4]
08034ef8  5bf80010  ldr.w	r1, [r11, r0]
08034efc  7848      ldr	r0, [pc, #480] ; [0x080350e0] = 0x200134c0
08034efe  0160      str	r1, [r0]
08034f00  0121      movs	r1, #1
08034f02  d160      str	r1, [r2, #12]
08034f04  d960      str	r1, [r3, #12]
08034f06  04fb07f2  mul	r2, r4, r7
08034f0a  7348      ldr	r0, [pc, #460] ; [0x080350d8] = 0x20002f8c
08034f0c  002a      cmp	r2, #0
08034f0e  b8bf      it	lt
08034f10  0732      addlt	r2, #7
08034f12  09ebe202  add.w	r2, r9, r2, asr #3
08034f16  22f00301  bic	r1, r2, #3
08034f1a  f6f713fd  bl	#-38362 ; -> 0x0802b944 ; branch_target=0x0802b944
08034f1e  714b      ldr	r3, [pc, #452] ; [0x080350e4] = 0x200134c8
08034f20  199a      ldr	r2, [sp, #100]
08034f22  4946      mov	r1, r9
08034f24  0093      str	r3, [sp]
08034f26  4023      movs	r3, #64
08034f28  6b48      ldr	r0, [pc, #428] ; [0x080350d8] = 0x20002f8c
08034f2a  0193      str	r3, [sp, #4]
08034f2c  3b46      mov	r3, r7
08034f2e  fef781fa  bl	#-6910 ; -> 0x08033434 ; branch_target=0x08033434
08034f32  6b4b      ldr	r3, [pc, #428] ; [0x080350e0] = 0x200134c0
08034f34  0444      add	r4, r0
08034f36  6949      ldr	r1, [pc, #420] ; [0x080350dc] = 0x200134c4
08034f38  1b68      ldr	r3, [r3]
08034f3a  4ff48072  mov.w	r2, #256
08034f3e  6a48      ldr	r0, [pc, #424] ; [0x080350e8] = 0x60001000
08034f40  0e68      ldr	r6, [r1]
08034f42  00eb8300  add.w	r0, r0, r3, lsl #2
08034f46  6749      ldr	r1, [pc, #412] ; [0x080350e4] = 0x200134c8
08034f48  0493      str	r3, [sp, #16]
08034f4a  01f0c9fa  bl	#5522 ; -> 0x080364e0 ; branch_target=0x080364e0
08034f4e  6748      ldr	r0, [pc, #412] ; [0x080350ec] = 0x60c01000
08034f50  4ff48072  mov.w	r2, #256
08034f54  6349      ldr	r1, [pc, #396] ; [0x080350e4] = 0x200134c8
08034f56  00eb8600  add.w	r0, r0, r6, lsl #2
08034f5a  4036      adds	r6, #64
08034f5c  01f0c0fa  bl	#5504 ; -> 0x080364e0 ; branch_target=0x080364e0
08034f60  049b      ldr	r3, [sp, #16]
08034f62  5e49      ldr	r1, [pc, #376] ; [0x080350dc] = 0x200134c4
08034f64  a542      cmp	r5, r4
08034f66  03f14003  add.w	r3, r3, #64
08034f6a  5d4a      ldr	r2, [pc, #372] ; [0x080350e0] = 0x200134c0
08034f6c  0e60      str	r6, [r1]
08034f6e  1360      str	r3, [r2]
08034f70  c9dc      bgt	#-110 ; -> 0x08034f06 ; branch_target=0x08034f06
08034f72  5948      ldr	r0, [pc, #356] ; [0x080350d8] = 0x20002f8c
08034f74  f6f764fc  bl	#-38712 ; -> 0x0802b840 ; branch_target=0x0802b840
08034f78  564a      ldr	r2, [pc, #344] ; [0x080350d4] = 0x20000b20
08034f7a  5d49      ldr	r1, [pc, #372] ; [0x080350f0] = 0x24000040
08034f7c  1368      ldr	r3, [r2]
08034f7e  5d48      ldr	r0, [pc, #372] ; [0x080350f4] = 0x24000000
08034f80  0133      adds	r3, #1
08034f82  03f00f03  and	r3, r3, #15
08034f86  1360      str	r3, [r2]
08034f88  504a      ldr	r2, [pc, #320] ; [0x080350cc] = 0x20002430
08034f8a  1368      ldr	r3, [r2]
08034f8c  0133      adds	r3, #1
08034f8e  03f00f03  and	r3, r3, #15
08034f92  1360      str	r3, [r2]
08034f94  f6f7e6fe  bl	#-37428 ; -> 0x0802bd64 ; branch_target=0x0802bd64
08034f98  0646      mov	r6, r0
08034f9a  8be4      b	#-1770 ; -> 0x080348b4 ; branch_target=0x080348b4
08034f9c  139b      ldr	r3, [sp, #76]
08034f9e  fae4      b	#-1548 ; -> 0x08034996 ; branch_target=0x08034996
08034fa0  0f9a      ldr	r2, [sp, #60]
08034fa2  c6e5      b	#-1140 ; -> 0x08034b32 ; branch_target=0x08034b32
08034fa4  109a      ldr	r2, [sp, #64]
08034fa6  c4e5      b	#-1144 ; -> 0x08034b32 ; branch_target=0x08034b32
08034fa8  0d9a      ldr	r2, [sp, #52]
08034faa  c2e5      b	#-1148 ; -> 0x08034b32 ; branch_target=0x08034b32
08034fac  0d9b      ldr	r3, [sp, #52]
08034fae  f2e4      b	#-1564 ; -> 0x08034996 ; branch_target=0x08034996
08034fb0  149b      ldr	r3, [sp, #80]
08034fb2  f0e4      b	#-1568 ; -> 0x08034996 ; branch_target=0x08034996
08034fb4  0821      movs	r1, #8
08034fb6  5048      ldr	r0, [pc, #320] ; [0x080350f8] = 0x58020c00
08034fb8  eef722fa  bl	#-72636 ; -> 0x08023400 ; branch_target=0x08023400
08034fbc  0028      cmp	r0, #0
08034fbe  3ff4e4aa  beq.w	#-2616 ; -> 0x0803458a ; branch_target=0x0803458a
08034fc2  0123      movs	r3, #1
08034fc4  4d4a      ldr	r2, [pc, #308] ; [0x080350fc] = 0x200144d0
08034fc6  3b60      str	r3, [r7]
08034fc8  1360      str	r3, [r2]
08034fca  fff7deba  b.w	#-2628 ; -> 0x0803458a ; branch_target=0x0803458a
08034fce  139a      ldr	r2, [sp, #76]
08034fd0  afe6      b	#-674 ; -> 0x08034d32 ; branch_target=0x08034d32
08034fd2  069a      ldr	r2, [sp, #24]
08034fd4  ade6      b	#-678 ; -> 0x08034d32 ; branch_target=0x08034d32
08034fd6  3d4b      ldr	r3, [pc, #244] ; [0x080350cc] = 0x20002430
08034fd8  b5f5805f  cmp.w	r5, #4096
08034fdc  1b68      ldr	r3, [r3]
08034fde  6cd0      beq	#216 ; -> 0x080350ba ; branch_target=0x080350ba
08034fe0  aa11      asrs	r2, r5, #6
08034fe2  0121      movs	r1, #1
08034fe4  1801      lsls	r0, r3, #4
08034fe6  08eb0313  add.w	r3, r8, r3, lsl #4
08034fea  c3e90121  strd	r2, r1, [r3, #4]
08034fee  58f80020  ldr.w	r2, [r8, r0]
08034ff2  3e49      ldr	r1, [pc, #248] ; [0x080350ec] = 0x60c01000
08034ff4  01eb8202  add.w	r2, r1, r2, lsl #2
08034ff8  0492      str	r2, [sp, #16]
08034ffa  0122      movs	r2, #1
08034ffc  da60      str	r2, [r3, #12]
08034ffe  2b46      mov	r3, r5
08035000  3546      mov	r5, r6
08035002  1e46      mov	r6, r3
08035004  04fb05f2  mul	r2, r4, r5
08035008  3348      ldr	r0, [pc, #204] ; [0x080350d8] = 0x20002f8c
0803500a  002a      cmp	r2, #0
0803500c  b8bf      it	lt
0803500e  0732      addlt	r2, #7
08035010  07ebe202  add.w	r2, r7, r2, asr #3
08035014  22f00301  bic	r1, r2, #3
08035018  f6f794fc  bl	#-38616 ; -> 0x0802b944 ; branch_target=0x0802b944
0803501c  4023      movs	r3, #64
0803501e  199a      ldr	r2, [sp, #100]
08035020  3946      mov	r1, r7
08035022  0193      str	r3, [sp, #4]
08035024  2f4b      ldr	r3, [pc, #188] ; [0x080350e4] = 0x200134c8
08035026  2c48      ldr	r0, [pc, #176] ; [0x080350d8] = 0x20002f8c
08035028  0093      str	r3, [sp]
0803502a  2b46      mov	r3, r5
0803502c  fef702fa  bl	#-7164 ; -> 0x08033434 ; branch_target=0x08033434
08035030  049b      ldr	r3, [sp, #16]
08035032  0444      add	r4, r0
08035034  4ff48072  mov.w	r2, #256
08035038  1846      mov	r0, r3
0803503a  03f58073  add.w	r3, r3, #256
0803503e  2949      ldr	r1, [pc, #164] ; [0x080350e4] = 0x200134c8
08035040  0493      str	r3, [sp, #16]
08035042  01f04dfa  bl	#5274 ; -> 0x080364e0 ; branch_target=0x080364e0
08035046  a642      cmp	r6, r4
08035048  dcdc      bgt	#-72 ; -> 0x08035004 ; branch_target=0x08035004
0803504a  b2e5      b	#-1180 ; -> 0x08034bb2 ; branch_target=0x08034bb2
0803504c  214b      ldr	r3, [pc, #132] ; [0x080350d4] = 0x20000b20
0803504e  b5f5805f  cmp.w	r5, #4096
08035052  1b68      ldr	r3, [r3]
08035054  34d0      beq	#104 ; -> 0x080350c0 ; branch_target=0x080350c0
08035056  aa11      asrs	r2, r5, #6
08035058  0121      movs	r1, #1
0803505a  1801      lsls	r0, r3, #4
0803505c  dff87890  ldr.w	r9, [pc, #120] ; [0x080350d8] = 0x20002f8c
08035060  0beb0313  add.w	r3, r11, r3, lsl #4
08035064  5bf80060  ldr.w	r6, [r11, r0]
08035068  c3e90121  strd	r2, r1, [r3, #4]
0803506c  1e4a      ldr	r2, [pc, #120] ; [0x080350e8] = 0x60001000
0803506e  02eb8606  add.w	r6, r2, r6, lsl #2
08035072  0122      movs	r2, #1
08035074  da60      str	r2, [r3, #12]
08035076  04fb07f2  mul	r2, r4, r7
0803507a  4846      mov	r0, r9
0803507c  002a      cmp	r2, #0
0803507e  b8bf      it	lt
08035080  0732      addlt	r2, #7
08035082  08ebe202  add.w	r2, r8, r2, asr #3
08035086  22f00301  bic	r1, r2, #3
0803508a  f6f75bfc  bl	#-38730 ; -> 0x0802b944 ; branch_target=0x0802b944
0803508e  4023      movs	r3, #64
08035090  199a      ldr	r2, [sp, #100]
08035092  4146      mov	r1, r8
08035094  0193      str	r3, [sp, #4]
08035096  4846      mov	r0, r9
08035098  124b      ldr	r3, [pc, #72] ; [0x080350e4] = 0x200134c8
0803509a  0093      str	r3, [sp]
0803509c  3b46      mov	r3, r7
0803509e  fef7c9f9  bl	#-7278 ; -> 0x08033434 ; branch_target=0x08033434
080350a2  0444      add	r4, r0
080350a4  4ff48072  mov.w	r2, #256
080350a8  3046      mov	r0, r6
080350aa  0e49      ldr	r1, [pc, #56] ; [0x080350e4] = 0x200134c8
080350ac  06f58076  add.w	r6, r6, #256
080350b0  01f016fa  bl	#5164 ; -> 0x080364e0 ; branch_target=0x080364e0
080350b4  a542      cmp	r5, r4
080350b6  dedc      bgt	#-68 ; -> 0x08035076 ; branch_target=0x08035076
080350b8  7ce6      b	#-776 ; -> 0x08034db4 ; branch_target=0x08034db4
080350ba  0822      movs	r2, #8
080350bc  1146      mov	r1, r2
080350be  91e7      b	#-222 ; -> 0x08034fe4 ; branch_target=0x08034fe4
080350c0  0822      movs	r2, #8
080350c2  1146      mov	r1, r2
080350c4  c9e7      b	#-110 ; -> 0x0803505a ; branch_target=0x0803505a
080350c6  0821      movs	r1, #8
080350c8  0e46      mov	r6, r1
080350ca  06e7      b	#-500 ; -> 0x08034eda ; branch_target=0x08034eda
