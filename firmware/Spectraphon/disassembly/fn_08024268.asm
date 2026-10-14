; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08024268  2de9f843  push.w	{r3, r4, r5, r6, r7, r8, r9, lr}
0802426c  0368      ldr	r3, [r0]
0802426e  0446      mov	r4, r0
08024270  13f00066  ands	r6, r3, #134217728
08024274  27d0      beq	#78 ; -> 0x080242c6 ; branch_target=0x080242c6
08024276  426e      ldr	r2, [r0, #100]
08024278  b2f5001f  cmp.w	r2, #2097152
0802427c  00f0bf85  beq.w	#2942 ; -> 0x08024dfe ; branch_target=0x08024dfe
08024280  15d8      bhi	#42 ; -> 0x080242ae ; branch_target=0x080242ae
08024282  002a      cmp	r2, #0
08024284  00f04186  beq.w	#3202 ; -> 0x08024f0a ; branch_target=0x08024f0a
08024288  b2f5801f  cmp.w	r2, #1048576
0802428c  40f0df83  bne.w	#1982 ; -> 0x08024a4e ; branch_target=0x08024a4e
08024290  a34a      ldr	r2, [pc, #652] ; [0x08024520] = 0x58024400
08024292  926a      ldr	r2, [r2, #40]
08024294  02f00302  and	r2, r2, #3
08024298  032a      cmp	r2, #3
0802429a  00f0d883  beq.w	#1968 ; -> 0x08024a4e ; branch_target=0x08024a4e
0802429e  0221      movs	r1, #2
080242a0  0430      adds	r0, #4
080242a2  fff7fdfe  bl	#-518 ; -> 0x080240a0 ; branch_target=0x080240a0
080242a6  0646      mov	r6, r0
080242a8  66b9      cbnz	r6, #24 ; -> 0x080242c4 ; branch_target=0x080242c4
080242aa  626e      ldr	r2, [r4, #100]
080242ac  03e0      b	#6 ; -> 0x080242b6 ; branch_target=0x080242b6
080242ae  b2f5401f  cmp.w	r2, #3145728
080242b2  40f0cc83  bne.w	#1944 ; -> 0x08024a4e ; branch_target=0x08024a4e
080242b6  9a49      ldr	r1, [pc, #616] ; [0x08024520] = 0x58024400
080242b8  0026      movs	r6, #0
080242ba  0b6d      ldr	r3, [r1, #80]
080242bc  23f44013  bic	r3, r3, #3145728
080242c0  1343      orrs	r3, r2
080242c2  0b65      str	r3, [r1, #80]
080242c4  2368      ldr	r3, [r4]
080242c6  dd05      lsls	r5, r3, #23
080242c8  0ad5      bpl	#20 ; -> 0x080242e0 ; branch_target=0x080242e0
080242ca  626d      ldr	r2, [r4, #84]
080242cc  042a      cmp	r2, #4
080242ce  06d8      bhi	#12 ; -> 0x080242de ; branch_target=0x080242de
080242d0  dfe812f0  tbh	[pc, r2, lsl #1]
080242de  0126      movs	r6, #1
080242e0  3546      mov	r5, r6
080242e2  9805      lsls	r0, r3, #22
080242e4  22d5      bpl	#68 ; -> 0x0802432c ; branch_target=0x0802432c
080242e6  a26d      ldr	r2, [r4, #88]
080242e8  802a      cmp	r2, #128
080242ea  00f0c585  beq.w	#2954 ; -> 0x08024e78 ; branch_target=0x08024e78
080242ee  00f21981  bhi.w	#562 ; -> 0x08024524 ; branch_target=0x08024524
080242f2  002a      cmp	r2, #0
080242f4  00f01a84  beq.w	#2100 ; -> 0x08024b2c ; branch_target=0x08024b2c
080242f8  402a      cmp	r2, #64
080242fa  40f01a81  bne.w	#564 ; -> 0x08024532 ; branch_target=0x08024532
080242fe  884a      ldr	r2, [pc, #544] ; [0x08024520] = 0x58024400
08024300  926a      ldr	r2, [r2, #40]
08024302  02f00302  and	r2, r2, #3
08024306  032a      cmp	r2, #3
08024308  00f01381  beq.w	#550 ; -> 0x08024532 ; branch_target=0x08024532
0802430c  0021      movs	r1, #0
0802430e  201d      adds	r0, r4, #4
08024310  fff7c6fe  bl	#-628 ; -> 0x080240a0 ; branch_target=0x080240a0
08024314  0546      mov	r5, r0
08024316  002d      cmp	r5, #0
08024318  40f08884  bne.w	#2320 ; -> 0x08024c2c ; branch_target=0x08024c2c
0802431c  804a      ldr	r2, [pc, #512] ; [0x08024520] = 0x58024400
0802431e  a16d      ldr	r1, [r4, #88]
08024320  136d      ldr	r3, [r2, #80]
08024322  23f4e073  bic	r3, r3, #448
08024326  0b43      orrs	r3, r1
08024328  1365      str	r3, [r2, #80]
0802432a  2368      ldr	r3, [r4]
0802432c  5905      lsls	r1, r3, #21
0802432e  26d5      bpl	#76 ; -> 0x0802437e ; branch_target=0x0802437e
08024330  d4f8a420  ldr.w	r2, [r4, #164]
08024334  b2f5800f  cmp.w	r2, #4194304
08024338  00f06f85  beq.w	#2782 ; -> 0x08024e1a ; branch_target=0x08024e1a
0802433c  00f2fc80  bhi.w	#504 ; -> 0x08024538 ; branch_target=0x08024538
08024340  002a      cmp	r2, #0
08024342  00f0d883  beq.w	#1968 ; -> 0x08024af6 ; branch_target=0x08024af6
08024346  b2f5001f  cmp.w	r2, #2097152
0802434a  40f0fd80  bne.w	#506 ; -> 0x08024548 ; branch_target=0x08024548
0802434e  744a      ldr	r2, [pc, #464] ; [0x08024520] = 0x58024400
08024350  926a      ldr	r2, [r2, #40]
08024352  02f00302  and	r2, r2, #3
08024356  032a      cmp	r2, #3
08024358  00f0f680  beq.w	#492 ; -> 0x08024548 ; branch_target=0x08024548
0802435c  0021      movs	r1, #0
0802435e  201d      adds	r0, r4, #4
08024360  fff79efe  bl	#-708 ; -> 0x080240a0 ; branch_target=0x080240a0
08024364  0546      mov	r5, r0
08024366  002d      cmp	r5, #0
08024368  40f06c84  bne.w	#2264 ; -> 0x08024c44 ; branch_target=0x08024c44
0802436c  6c4a      ldr	r2, [pc, #432] ; [0x08024520] = 0x58024400
0802436e  d4f8a410  ldr.w	r1, [r4, #164]
08024372  936d      ldr	r3, [r2, #88]
08024374  23f46003  bic	r3, r3, #14680064
08024378  0b43      orrs	r3, r1
0802437a  9365      str	r3, [r2, #88]
0802437c  2368      ldr	r3, [r4]
0802437e  1a05      lsls	r2, r3, #20
08024380  26d5      bpl	#76 ; -> 0x080243d0 ; branch_target=0x080243d0
08024382  d4f8a820  ldr.w	r2, [r4, #168]
08024386  b2f1007f  cmp.w	r2, #33554432
0802438a  00f06485  beq.w	#2760 ; -> 0x08024e56 ; branch_target=0x08024e56
0802438e  00f2de80  bhi.w	#444 ; -> 0x0802454e ; branch_target=0x0802454e
08024392  002a      cmp	r2, #0
08024394  00f0b883  beq.w	#1904 ; -> 0x08024b08 ; branch_target=0x08024b08
08024398  b2f1807f  cmp.w	r2, #16777216
0802439c  40f0df80  bne.w	#446 ; -> 0x0802455e ; branch_target=0x0802455e
080243a0  5f4a      ldr	r2, [pc, #380] ; [0x08024520] = 0x58024400
080243a2  926a      ldr	r2, [r2, #40]
080243a4  02f00302  and	r2, r2, #3
080243a8  032a      cmp	r2, #3
080243aa  00f0d880  beq.w	#432 ; -> 0x0802455e ; branch_target=0x0802455e
080243ae  0021      movs	r1, #0
080243b0  201d      adds	r0, r4, #4
080243b2  fff775fe  bl	#-790 ; -> 0x080240a0 ; branch_target=0x080240a0
080243b6  0546      mov	r5, r0
080243b8  002d      cmp	r5, #0
080243ba  40f03384  bne.w	#2150 ; -> 0x08024c24 ; branch_target=0x08024c24
080243be  584a      ldr	r2, [pc, #352] ; [0x08024520] = 0x58024400
080243c0  d4f8a810  ldr.w	r1, [r4, #168]
080243c4  936d      ldr	r3, [r2, #88]
080243c6  23f0e063  bic	r3, r3, #117440512
080243ca  0b43      orrs	r3, r1
080243cc  9365      str	r3, [r2, #88]
080243ce  2368      ldr	r3, [r4]
080243d0  9f01      lsls	r7, r3, #6
080243d2  19d5      bpl	#50 ; -> 0x08024408 ; branch_target=0x08024408
080243d4  a26c      ldr	r2, [r4, #72]
080243d6  202a      cmp	r2, #32
080243d8  00f08284  beq.w	#2308 ; -> 0x08024ce0 ; branch_target=0x08024ce0
080243dc  00f2c280  bhi.w	#388 ; -> 0x08024564 ; branch_target=0x08024564
080243e0  3ab1      cbz	r2, #14 ; -> 0x080243f2 ; branch_target=0x080243f2
080243e2  102a      cmp	r2, #16
080243e4  40f0c180  bne.w	#386 ; -> 0x0802456a ; branch_target=0x0802456a
080243e8  4d4a      ldr	r2, [pc, #308] ; [0x08024520] = 0x58024400
080243ea  d36a      ldr	r3, [r2, #44]
080243ec  43f40033  orr	r3, r3, #131072
080243f0  d362      str	r3, [r2, #44]
080243f2  002d      cmp	r5, #0
080243f4  40f00984  bne.w	#2066 ; -> 0x08024c0a ; branch_target=0x08024c0a
080243f8  494a      ldr	r2, [pc, #292] ; [0x08024520] = 0x58024400
080243fa  a16c      ldr	r1, [r4, #72]
080243fc  d36c      ldr	r3, [r2, #76]
080243fe  23f03003  bic	r3, r3, #48
08024402  0b43      orrs	r3, r1
08024404  d364      str	r3, [r2, #76]
08024406  2368      ldr	r3, [r4]
08024408  d804      lsls	r0, r3, #19
0802440a  24d5      bpl	#72 ; -> 0x08024456 ; branch_target=0x08024456
0802440c  e26d      ldr	r2, [r4, #92]
0802440e  b2f5005f  cmp.w	r2, #8192
08024412  00f0e384  beq.w	#2502 ; -> 0x08024ddc ; branch_target=0x08024ddc
08024416  00f2ab80  bhi.w	#342 ; -> 0x08024570 ; branch_target=0x08024570
0802441a  002a      cmp	r2, #0
0802441c  00f07d83  beq.w	#1786 ; -> 0x08024b1a ; branch_target=0x08024b1a
08024420  b2f5805f  cmp.w	r2, #4096
08024424  40f0ac80  bne.w	#344 ; -> 0x08024580 ; branch_target=0x08024580
08024428  3d4a      ldr	r2, [pc, #244] ; [0x08024520] = 0x58024400
0802442a  926a      ldr	r2, [r2, #40]
0802442c  02f00302  and	r2, r2, #3
08024430  032a      cmp	r2, #3
08024432  00f0a580  beq.w	#330 ; -> 0x08024580 ; branch_target=0x08024580
08024436  0021      movs	r1, #0
08024438  201d      adds	r0, r4, #4
0802443a  fff731fe  bl	#-926 ; -> 0x080240a0 ; branch_target=0x080240a0
0802443e  0546      mov	r5, r0
08024440  002d      cmp	r5, #0
08024442  40f0f783  bne.w	#2030 ; -> 0x08024c34 ; branch_target=0x08024c34
08024446  364a      ldr	r2, [pc, #216] ; [0x08024520] = 0x58024400
08024448  e16d      ldr	r1, [r4, #92]
0802444a  136d      ldr	r3, [r2, #80]
0802444c  23f4e043  bic	r3, r3, #28672
08024450  0b43      orrs	r3, r1
08024452  1365      str	r3, [r2, #80]
08024454  2368      ldr	r3, [r4]
08024456  9904      lsls	r1, r3, #18
08024458  22d5      bpl	#68 ; -> 0x080244a0 ; branch_target=0x080244a0
0802445a  226e      ldr	r2, [r4, #96]
0802445c  b2f5003f  cmp.w	r2, #131072
08024460  00f02d84  beq.w	#2138 ; -> 0x08024cbe ; branch_target=0x08024cbe
08024464  00f28f80  bhi.w	#286 ; -> 0x08024586 ; branch_target=0x08024586
08024468  7ab1      cbz	r2, #30 ; -> 0x0802448a ; branch_target=0x0802448a
0802446a  b2f5803f  cmp.w	r2, #65536
0802446e  40f09480  bne.w	#296 ; -> 0x0802459a ; branch_target=0x0802459a
08024472  2b4a      ldr	r2, [pc, #172] ; [0x08024520] = 0x58024400
08024474  926a      ldr	r2, [r2, #40]
08024476  02f00302  and	r2, r2, #3
0802447a  032a      cmp	r2, #3
0802447c  00f08d80  beq.w	#282 ; -> 0x0802459a ; branch_target=0x0802459a
08024480  0121      movs	r1, #1
08024482  201d      adds	r0, r4, #4
08024484  fff70cfe  bl	#-1000 ; -> 0x080240a0 ; branch_target=0x080240a0
08024488  0546      mov	r5, r0
0802448a  002d      cmp	r5, #0
0802448c  40f0e783  bne.w	#1998 ; -> 0x08024c5e ; branch_target=0x08024c5e
08024490  234a      ldr	r2, [pc, #140] ; [0x08024520] = 0x58024400
08024492  216e      ldr	r1, [r4, #96]
08024494  136d      ldr	r3, [r2, #80]
08024496  23f4e023  bic	r3, r3, #458752
0802449a  0b43      orrs	r3, r1
0802449c  1365      str	r3, [r2, #80]
0802449e  2368      ldr	r3, [r4]
080244a0  5a04      lsls	r2, r3, #17
080244a2  21d5      bpl	#66 ; -> 0x080244e8 ; branch_target=0x080244e8
080244a4  d4f8ac20  ldr.w	r2, [r4, #172]
080244a8  b2f1005f  cmp.w	r2, #536870912
080244ac  00f06b84  beq.w	#2262 ; -> 0x08024d86 ; branch_target=0x08024d86
080244b0  76d8      bhi	#236 ; -> 0x080245a0 ; branch_target=0x080245a0
080244b2  6ab1      cbz	r2, #26 ; -> 0x080244d0 ; branch_target=0x080244d0
080244b4  b2f1805f  cmp.w	r2, #268435456
080244b8  7ad1      bne	#244 ; -> 0x080245b0 ; branch_target=0x080245b0
080244ba  194a      ldr	r2, [pc, #100] ; [0x08024520] = 0x58024400
080244bc  926a      ldr	r2, [r2, #40]
080244be  02f00302  and	r2, r2, #3
080244c2  032a      cmp	r2, #3
080244c4  74d0      beq	#232 ; -> 0x080245b0 ; branch_target=0x080245b0
080244c6  0121      movs	r1, #1
080244c8  201d      adds	r0, r4, #4
080244ca  fff7e9fd  bl	#-1070 ; -> 0x080240a0 ; branch_target=0x080240a0
080244ce  0546      mov	r5, r0
080244d0  002d      cmp	r5, #0
080244d2  40f0dd83  bne.w	#1978 ; -> 0x08024c90 ; branch_target=0x08024c90
080244d6  124a      ldr	r2, [pc, #72] ; [0x08024520] = 0x58024400
080244d8  d4f8ac10  ldr.w	r1, [r4, #172]
080244dc  936d      ldr	r3, [r2, #88]
080244de  23f0e043  bic	r3, r3, #1879048192
080244e2  0b43      orrs	r3, r1
080244e4  9365      str	r3, [r2, #88]
080244e6  2368      ldr	r3, [r4]
080244e8  1f04      lsls	r7, r3, #16
080244ea  0dd5      bpl	#26 ; -> 0x08024508 ; branch_target=0x08024508
080244ec  e26e      ldr	r2, [r4, #108]
080244ee  b2f1805f  cmp.w	r2, #268435456
080244f2  00f05782  beq.w	#1198 ; -> 0x080249a4 ; branch_target=0x080249a4
080244f6  b2f1005f  cmp.w	r2, #536870912
080244fa  00f06183  beq.w	#1730 ; -> 0x08024bc0 ; branch_target=0x08024bc0
080244fe  002a      cmp	r2, #0
08024500  00f05582  beq.w	#1194 ; -> 0x080249ae ; branch_target=0x080249ae
08024504  0126      movs	r6, #1
08024506  3546      mov	r5, r6
08024508  d801      lsls	r0, r3, #7
0802450a  5ed5      bpl	#188 ; -> 0x080245ca ; branch_target=0x080245ca
0802450c  626c      ldr	r2, [r4, #68]
0802450e  032a      cmp	r2, #3
08024510  00f20b85  bhi.w	#2582 ; -> 0x08024f2a ; branch_target=0x08024f2a
08024514  dfe812f0  tbh	[pc, r2, lsl #1]
08024524  c02a      cmp	r2, #192
08024526  3ff4f6ae  beq.w	#-532 ; -> 0x08024316 ; branch_target=0x08024316
0802452a  b2f5807f  cmp.w	r2, #256
0802452e  3ff4f2ae  beq.w	#-540 ; -> 0x08024316 ; branch_target=0x08024316
08024532  0126      movs	r6, #1
08024534  3546      mov	r5, r6
08024536  f9e6      b	#-526 ; -> 0x0802432c ; branch_target=0x0802432c
08024538  b2f5c00f  cmp.w	r2, #6291456
0802453c  3ff413af  beq.w	#-474 ; -> 0x08024366 ; branch_target=0x08024366
08024540  b2f5000f  cmp.w	r2, #8388608
08024544  3ff40faf  beq.w	#-482 ; -> 0x08024366 ; branch_target=0x08024366
08024548  0126      movs	r6, #1
0802454a  3546      mov	r5, r6
0802454c  17e7      b	#-466 ; -> 0x0802437e ; branch_target=0x0802437e
0802454e  b2f1407f  cmp.w	r2, #50331648
08024552  3ff431af  beq.w	#-414 ; -> 0x080243b8 ; branch_target=0x080243b8
08024556  b2f1806f  cmp.w	r2, #67108864
0802455a  3ff42daf  beq.w	#-422 ; -> 0x080243b8 ; branch_target=0x080243b8
0802455e  0126      movs	r6, #1
08024560  3546      mov	r5, r6
08024562  35e7      b	#-406 ; -> 0x080243d0 ; branch_target=0x080243d0
08024564  302a      cmp	r2, #48
08024566  3ff444af  beq.w	#-376 ; -> 0x080243f2 ; branch_target=0x080243f2
0802456a  0126      movs	r6, #1
0802456c  3546      mov	r5, r6
0802456e  4be7      b	#-362 ; -> 0x08024408 ; branch_target=0x08024408
08024570  b2f5405f  cmp.w	r2, #12288
08024574  3ff464af  beq.w	#-312 ; -> 0x08024440 ; branch_target=0x08024440
08024578  b2f5804f  cmp.w	r2, #16384
0802457c  3ff460af  beq.w	#-320 ; -> 0x08024440 ; branch_target=0x08024440
08024580  0126      movs	r6, #1
08024582  3546      mov	r5, r6
08024584  67e7      b	#-306 ; -> 0x08024456 ; branch_target=0x08024456
08024586  22f48031  bic	r1, r2, #65536
0802458a  b1f5802f  cmp.w	r1, #262144
0802458e  3ff47caf  beq.w	#-264 ; -> 0x0802448a ; branch_target=0x0802448a
08024592  b2f5403f  cmp.w	r2, #196608
08024596  3ff478af  beq.w	#-272 ; -> 0x0802448a ; branch_target=0x0802448a
0802459a  0126      movs	r6, #1
0802459c  3546      mov	r5, r6
0802459e  7fe7      b	#-258 ; -> 0x080244a0 ; branch_target=0x080244a0
080245a0  22f08051  bic	r1, r2, #268435456
080245a4  b1f1804f  cmp.w	r1, #1073741824
080245a8  92d0      beq	#-220 ; -> 0x080244d0 ; branch_target=0x080244d0
080245aa  b2f1405f  cmp.w	r2, #805306368
080245ae  8fd0      beq	#-226 ; -> 0x080244d0 ; branch_target=0x080244d0
080245b0  0126      movs	r6, #1
080245b2  3546      mov	r5, r6
080245b4  98e7      b	#-208 ; -> 0x080244e8 ; branch_target=0x080244e8
080245b6  364a      ldr	r2, [pc, #216] ; [0x08024690] = 0x58024400
080245b8  d36a      ldr	r3, [r2, #44]
080245ba  43f40033  orr	r3, r3, #131072
080245be  d362      str	r3, [r2, #44]
080245c0  002d      cmp	r5, #0
080245c2  00f02683  beq.w	#1612 ; -> 0x08024c12 ; branch_target=0x08024c12
080245c6  2368      ldr	r3, [r4]
080245c8  2e46      mov	r6, r5
080245ca  5902      lsls	r1, r3, #9
080245cc  00f14182  bmi.w	#1154 ; -> 0x08024a52 ; branch_target=0x08024a52
080245d0  df07      lsls	r7, r3, #31
080245d2  2fd5      bpl	#94 ; -> 0x08024634 ; branch_target=0x08024634
080245d4  a26f      ldr	r2, [r4, #120]
080245d6  282a      cmp	r2, #40
080245d8  2ad8      bhi	#84 ; -> 0x08024630 ; branch_target=0x08024630
080245da  dfe812f0  tbh	[pc, r2, lsl #1]
08024630  0126      movs	r6, #1
08024632  3546      mov	r5, r6
08024634  9807      lsls	r0, r3, #30
08024636  1cd5      bpl	#56 ; -> 0x08024672 ; branch_target=0x08024672
08024638  626f      ldr	r2, [r4, #116]
0802463a  052a      cmp	r2, #5
0802463c  00f27184  bhi.w	#2274 ; -> 0x08024f22 ; branch_target=0x08024f22
08024640  dfe812f0  tbh	[pc, r2, lsl #1]
08024650  0f4a      ldr	r2, [pc, #60] ; [0x08024690] = 0x58024400
08024652  926a      ldr	r2, [r2, #40]
08024654  02f00302  and	r2, r2, #3
08024658  032a      cmp	r2, #3
0802465a  00f06284  beq.w	#2244 ; -> 0x08024f22 ; branch_target=0x08024f22
0802465e  0121      movs	r1, #1
08024660  201d      adds	r0, r4, #4
08024662  fff71dfd  bl	#-1478 ; -> 0x080240a0 ; branch_target=0x080240a0
08024666  0546      mov	r5, r0
08024668  002d      cmp	r5, #0
0802466a  00f0fb82  beq.w	#1526 ; -> 0x08024c64 ; branch_target=0x08024c64
0802466e  2368      ldr	r3, [r4]
08024670  2e46      mov	r6, r5
08024672  5907      lsls	r1, r3, #29
08024674  1fd5      bpl	#62 ; -> 0x080246b6 ; branch_target=0x080246b6
08024676  d4f89020  ldr.w	r2, [r4, #144]
0802467a  052a      cmp	r2, #5
0802467c  00f24d84  bhi.w	#2202 ; -> 0x08024f1a ; branch_target=0x08024f1a
08024680  dfe812f0  tbh	[pc, r2, lsl #1]
08024694  aa4a      ldr	r2, [pc, #680] ; [0x08024940] = 0x58024400
08024696  926a      ldr	r2, [r2, #40]
08024698  02f00302  and	r2, r2, #3
0802469c  032a      cmp	r2, #3
0802469e  00f03c84  beq.w	#2168 ; -> 0x08024f1a ; branch_target=0x08024f1a
080246a2  0121      movs	r1, #1
080246a4  201d      adds	r0, r4, #4
080246a6  fff7fbfc  bl	#-1546 ; -> 0x080240a0 ; branch_target=0x080240a0
080246aa  0546      mov	r5, r0
080246ac  002d      cmp	r5, #0
080246ae  00f0e282  beq.w	#1476 ; -> 0x08024c76 ; branch_target=0x08024c76
080246b2  2368      ldr	r3, [r4]
080246b4  2e46      mov	r6, r5
080246b6  9a06      lsls	r2, r3, #26
080246b8  24d5      bpl	#72 ; -> 0x08024704 ; branch_target=0x08024704
080246ba  d4f88c20  ldr.w	r2, [r4, #140]
080246be  b2f1005f  cmp.w	r2, #536870912
080246c2  00f01d83  beq.w	#1594 ; -> 0x08024d00 ; branch_target=0x08024d00
080246c6  00f22081  bhi.w	#576 ; -> 0x0802490a ; branch_target=0x0802490a
080246ca  7ab1      cbz	r2, #30 ; -> 0x080246ec ; branch_target=0x080246ec
080246cc  b2f1805f  cmp.w	r2, #268435456
080246d0  40f02581  bne.w	#586 ; -> 0x0802491e ; branch_target=0x0802491e
080246d4  9a4a      ldr	r2, [pc, #616] ; [0x08024940] = 0x58024400
080246d6  926a      ldr	r2, [r2, #40]
080246d8  02f00302  and	r2, r2, #3
080246dc  032a      cmp	r2, #3
080246de  00f01e81  beq.w	#572 ; -> 0x0802491e ; branch_target=0x0802491e
080246e2  0021      movs	r1, #0
080246e4  201d      adds	r0, r4, #4
080246e6  fff7dbfc  bl	#-1610 ; -> 0x080240a0 ; branch_target=0x080240a0
080246ea  0546      mov	r5, r0
080246ec  002d      cmp	r5, #0
080246ee  40f0ad82  bne.w	#1370 ; -> 0x08024c4c ; branch_target=0x08024c4c
080246f2  934a      ldr	r2, [pc, #588] ; [0x08024940] = 0x58024400
080246f4  d4f88c10  ldr.w	r1, [r4, #140]
080246f8  536d      ldr	r3, [r2, #84]
080246fa  23f0e043  bic	r3, r3, #1879048192
080246fe  0b43      orrs	r3, r1
08024700  5365      str	r3, [r2, #84]
08024702  2368      ldr	r3, [r4]
08024704  5f06      lsls	r7, r3, #25
08024706  24d5      bpl	#72 ; -> 0x08024752 ; branch_target=0x08024752
08024708  d4f89820  ldr.w	r2, [r4, #152]
0802470c  b2f5006f  cmp.w	r2, #2048
08024710  00f00783  beq.w	#1550 ; -> 0x08024d22 ; branch_target=0x08024d22
08024714  00f20681  bhi.w	#524 ; -> 0x08024924 ; branch_target=0x08024924
08024718  7ab1      cbz	r2, #30 ; -> 0x0802473a ; branch_target=0x0802473a
0802471a  b2f5806f  cmp.w	r2, #1024
0802471e  40f00b81  bne.w	#534 ; -> 0x08024938 ; branch_target=0x08024938
08024722  874a      ldr	r2, [pc, #540] ; [0x08024940] = 0x58024400
08024724  926a      ldr	r2, [r2, #40]
08024726  02f00302  and	r2, r2, #3
0802472a  032a      cmp	r2, #3
0802472c  00f00481  beq.w	#520 ; -> 0x08024938 ; branch_target=0x08024938
08024730  0021      movs	r1, #0
08024732  201d      adds	r0, r4, #4
08024734  fff7b4fc  bl	#-1688 ; -> 0x080240a0 ; branch_target=0x080240a0
08024738  0546      mov	r5, r0
0802473a  002d      cmp	r5, #0
0802473c  40f08c82  bne.w	#1304 ; -> 0x08024c58 ; branch_target=0x08024c58
08024740  7f4a      ldr	r2, [pc, #508] ; [0x08024940] = 0x58024400
08024742  d4f89810  ldr.w	r1, [r4, #152]
08024746  936d      ldr	r3, [r2, #88]
08024748  23f4e053  bic	r3, r3, #7168
0802474c  0b43      orrs	r3, r1
0802474e  9365      str	r3, [r2, #88]
08024750  2368      ldr	r3, [r4]
08024752  1806      lsls	r0, r3, #24
08024754  24d5      bpl	#72 ; -> 0x080247a0 ; branch_target=0x080247a0
08024756  d4f89c20  ldr.w	r2, [r4, #156]
0802475a  b2f5804f  cmp.w	r2, #16384
0802475e  00f0f182  beq.w	#1506 ; -> 0x08024d44 ; branch_target=0x08024d44
08024762  00f2ef80  bhi.w	#478 ; -> 0x08024944 ; branch_target=0x08024944
08024766  7ab1      cbz	r2, #30 ; -> 0x08024788 ; branch_target=0x08024788
08024768  b2f5005f  cmp.w	r2, #8192
0802476c  40f0f480  bne.w	#488 ; -> 0x08024958 ; branch_target=0x08024958
08024770  734a      ldr	r2, [pc, #460] ; [0x08024940] = 0x58024400
08024772  926a      ldr	r2, [r2, #40]
08024774  02f00302  and	r2, r2, #3
08024778  032a      cmp	r2, #3
0802477a  00f0ed80  beq.w	#474 ; -> 0x08024958 ; branch_target=0x08024958
0802477e  0021      movs	r1, #0
08024780  201d      adds	r0, r4, #4
08024782  fff78dfc  bl	#-1766 ; -> 0x080240a0 ; branch_target=0x080240a0
08024786  0546      mov	r5, r0
08024788  002d      cmp	r5, #0
0802478a  40f06282  bne.w	#1220 ; -> 0x08024c52 ; branch_target=0x08024c52
0802478e  6c4a      ldr	r2, [pc, #432] ; [0x08024940] = 0x58024400
08024790  d4f89c10  ldr.w	r1, [r4, #156]
08024794  936d      ldr	r3, [r2, #88]
08024796  23f46043  bic	r3, r3, #57344
0802479a  0b43      orrs	r3, r1
0802479c  9365      str	r3, [r2, #88]
0802479e  2368      ldr	r3, [r4]
080247a0  1907      lsls	r1, r3, #28
080247a2  0cd5      bpl	#24 ; -> 0x080247be ; branch_target=0x080247be
080247a4  d4f88020  ldr.w	r2, [r4, #128]
080247a8  b2f5805f  cmp.w	r2, #4096
080247ac  00f0c781  beq.w	#910 ; -> 0x08024b3e ; branch_target=0x08024b3e
080247b0  6349      ldr	r1, [pc, #396] ; [0x08024940] = 0x58024400
080247b2  4b6d      ldr	r3, [r1, #84]
080247b4  23f44053  bic	r3, r3, #12288
080247b8  1343      orrs	r3, r2
080247ba  4b65      str	r3, [r1, #84]
080247bc  2368      ldr	r3, [r4]
080247be  da06      lsls	r2, r3, #27
080247c0  0cd5      bpl	#24 ; -> 0x080247dc ; branch_target=0x080247dc
080247c2  d4f89420  ldr.w	r2, [r4, #148]
080247c6  b2f5807f  cmp.w	r2, #256
080247ca  00f0ca81  beq.w	#916 ; -> 0x08024b62 ; branch_target=0x08024b62
080247ce  5c49      ldr	r1, [pc, #368] ; [0x08024940] = 0x58024400
080247d0  8b6d      ldr	r3, [r1, #88]
080247d2  23f44073  bic	r3, r3, #768
080247d6  1343      orrs	r3, r2
080247d8  8b65      str	r3, [r1, #88]
080247da  2368      ldr	r3, [r4]
080247dc  1f03      lsls	r7, r3, #12
080247de  0ed5      bpl	#28 ; -> 0x080247fe ; branch_target=0x080247fe
080247e0  d4f8a010  ldr.w	r1, [r4, #160]
080247e4  b1f5803f  cmp.w	r1, #65536
080247e8  00f0ff80  beq.w	#510 ; -> 0x080249ea ; branch_target=0x080249ea
080247ec  b1f5003f  cmp.w	r1, #131072
080247f0  00f00881  beq.w	#528 ; -> 0x08024a04 ; branch_target=0x08024a04
080247f4  0029      cmp	r1, #0
080247f6  00f0f381  beq.w	#998 ; -> 0x08024be0 ; branch_target=0x08024be0
080247fa  0126      movs	r6, #1
080247fc  3546      mov	r5, r6
080247fe  5803      lsls	r0, r3, #13
08024800  0fd5      bpl	#30 ; -> 0x08024822 ; branch_target=0x08024822
08024802  d4f88420  ldr.w	r2, [r4, #132]
08024806  b2f5001f  cmp.w	r2, #2097152
0802480a  00f04782  beq.w	#1166 ; -> 0x08024c9c ; branch_target=0x08024c9c
0802480e  b2f5401f  cmp.w	r2, #3145728
08024812  00f0dd80  beq.w	#442 ; -> 0x080249d0 ; branch_target=0x080249d0
08024816  b2f5801f  cmp.w	r2, #1048576
0802481a  00f0d480  beq.w	#424 ; -> 0x080249c6 ; branch_target=0x080249c6
0802481e  0126      movs	r6, #1
08024820  3546      mov	r5, r6
08024822  d903      lsls	r1, r3, #15
08024824  18d5      bpl	#48 ; -> 0x08024858 ; branch_target=0x08024858
08024826  e26c      ldr	r2, [r4, #76]
08024828  002a      cmp	r2, #0
0802482a  00f0b881  beq.w	#880 ; -> 0x08024b9e ; branch_target=0x08024b9e
0802482e  b2f5803f  cmp.w	r2, #65536
08024832  40f0b280  bne.w	#356 ; -> 0x0802499a ; branch_target=0x0802499a
08024836  424a      ldr	r2, [pc, #264] ; [0x08024940] = 0x58024400
08024838  926a      ldr	r2, [r2, #40]
0802483a  02f00302  and	r2, r2, #3
0802483e  032a      cmp	r2, #3
08024840  00f0ab80  beq.w	#342 ; -> 0x0802499a ; branch_target=0x0802499a
08024844  0221      movs	r1, #2
08024846  201d      adds	r0, r4, #4
08024848  fff72afc  bl	#-1964 ; -> 0x080240a0 ; branch_target=0x080240a0
0802484c  0546      mov	r5, r0
0802484e  002d      cmp	r5, #0
08024850  00f0ad81  beq.w	#858 ; -> 0x08024bae ; branch_target=0x08024bae
08024854  2368      ldr	r3, [r4]
08024856  2e46      mov	r6, r5
08024858  9a00      lsls	r2, r3, #2
0802485a  0fd5      bpl	#30 ; -> 0x0802487c ; branch_target=0x0802487c
0802485c  384a      ldr	r2, [pc, #224] ; [0x08024940] = 0x58024400
0802485e  926a      ldr	r2, [r2, #40]
08024860  02f00302  and	r2, r2, #3
08024864  032a      cmp	r2, #3
08024866  00f09b80  beq.w	#310 ; -> 0x080249a0 ; branch_target=0x080249a0
0802486a  0221      movs	r1, #2
0802486c  04f12400  add.w	r0, r4, #36
08024870  fff788fc  bl	#-1776 ; -> 0x08024184 ; branch_target=0x08024184
08024874  2368      ldr	r3, [r4]
08024876  0028      cmp	r0, #0
08024878  40f09280  bne.w	#292 ; -> 0x080249a0 ; branch_target=0x080249a0
0802487c  9f03      lsls	r7, r3, #14
0802487e  6ed4      bmi	#220 ; -> 0x0802495e ; branch_target=0x0802495e
08024880  301e      subs	r0, r6, #0
08024882  18bf      it	ne
08024884  0120      movne	r0, #1
08024886  de02      lsls	r6, r3, #11
08024888  07d5      bpl	#14 ; -> 0x0802489a ; branch_target=0x0802489a
0802488a  2d4a      ldr	r2, [pc, #180] ; [0x08024940] = 0x58024400
0802488c  216f      ldr	r1, [r4, #112]
0802488e  136d      ldr	r3, [r2, #80]
08024890  23f00043  bic	r3, r3, #2147483648
08024894  0b43      orrs	r3, r1
08024896  1365      str	r3, [r2, #80]
08024898  2368      ldr	r3, [r4]
0802489a  dd00      lsls	r5, r3, #3
0802489c  08d5      bpl	#16 ; -> 0x080248b0 ; branch_target=0x080248b0
0802489e  284a      ldr	r2, [pc, #160] ; [0x08024940] = 0x58024400
080248a0  d4f8b410  ldr.w	r1, [r4, #180]
080248a4  1369      ldr	r3, [r2, #16]
080248a6  23f48043  bic	r3, r3, #16384
080248aa  0b43      orrs	r3, r1
080248ac  1361      str	r3, [r2, #16]
080248ae  2368      ldr	r3, [r4]
080248b0  9902      lsls	r1, r3, #10
080248b2  07d5      bpl	#14 ; -> 0x080248c4 ; branch_target=0x080248c4
080248b4  224a      ldr	r2, [pc, #136] ; [0x08024940] = 0x58024400
080248b6  a16e      ldr	r1, [r4, #104]
080248b8  136d      ldr	r3, [r2, #80]
080248ba  23f08073  bic	r3, r3, #16777216
080248be  0b43      orrs	r3, r1
080248c0  1365      str	r3, [r2, #80]
080248c2  2368      ldr	r3, [r4]
080248c4  5a00      lsls	r2, r3, #1
080248c6  0ad5      bpl	#20 ; -> 0x080248de ; branch_target=0x080248de
080248c8  1d4b      ldr	r3, [pc, #116] ; [0x08024940] = 0x58024400
080248ca  1a69      ldr	r2, [r3, #16]
080248cc  22f40042  bic	r2, r2, #32768
080248d0  1a61      str	r2, [r3, #16]
080248d2  1a69      ldr	r2, [r3, #16]
080248d4  d4f8b810  ldr.w	r1, [r4, #184]
080248d8  0a43      orrs	r2, r1
080248da  1a61      str	r2, [r3, #16]
080248dc  2368      ldr	r3, [r4]
080248de  002b      cmp	r3, #0
080248e0  07da      bge	#14 ; -> 0x080248f2 ; branch_target=0x080248f2
080248e2  174a      ldr	r2, [pc, #92] ; [0x08024940] = 0x58024400
080248e4  216d      ldr	r1, [r4, #80]
080248e6  d36c      ldr	r3, [r2, #76]
080248e8  23f04053  bic	r3, r3, #805306368
080248ec  0b43      orrs	r3, r1
080248ee  d364      str	r3, [r2, #76]
080248f0  2368      ldr	r3, [r4]
080248f2  1b02      lsls	r3, r3, #8
080248f4  07d5      bpl	#14 ; -> 0x08024906 ; branch_target=0x08024906
080248f6  124a      ldr	r2, [pc, #72] ; [0x08024940] = 0x58024400
080248f8  d4f88810  ldr.w	r1, [r4, #136]
080248fc  536d      ldr	r3, [r2, #84]
080248fe  23f44003  bic	r3, r3, #12582912
08024902  0b43      orrs	r3, r1
08024904  5365      str	r3, [r2, #84]
08024906  bde8f883  pop.w	{r3, r4, r5, r6, r7, r8, r9, pc}
0802490a  22f08051  bic	r1, r2, #268435456
0802490e  b1f1804f  cmp.w	r1, #1073741824
08024912  3ff4ebae  beq.w	#-554 ; -> 0x080246ec ; branch_target=0x080246ec
08024916  b2f1405f  cmp.w	r2, #805306368
0802491a  3ff4e7ae  beq.w	#-562 ; -> 0x080246ec ; branch_target=0x080246ec
0802491e  0126      movs	r6, #1
08024920  3546      mov	r5, r6
08024922  efe6      b	#-546 ; -> 0x08024704 ; branch_target=0x08024704
08024924  22f48061  bic	r1, r2, #1024
08024928  b1f5805f  cmp.w	r1, #4096
0802492c  3ff405af  beq.w	#-502 ; -> 0x0802473a ; branch_target=0x0802473a
08024930  b2f5406f  cmp.w	r2, #3072
08024934  3ff401af  beq.w	#-510 ; -> 0x0802473a ; branch_target=0x0802473a
08024938  0126      movs	r6, #1
0802493a  3546      mov	r5, r6
0802493c  09e7      b	#-494 ; -> 0x08024752 ; branch_target=0x08024752
08024944  22f40051  bic	r1, r2, #8192
08024948  b1f5004f  cmp.w	r1, #32768
0802494c  3ff41caf  beq.w	#-456 ; -> 0x08024788 ; branch_target=0x08024788
08024950  b2f5c04f  cmp.w	r2, #24576
08024954  3ff418af  beq.w	#-464 ; -> 0x08024788 ; branch_target=0x08024788
08024958  0126      movs	r6, #1
0802495a  3546      mov	r5, r6
0802495c  20e7      b	#-448 ; -> 0x080247a0 ; branch_target=0x080247a0
0802495e  e26f      ldr	r2, [r4, #124]
08024960  b2f5807f  cmp.w	r2, #256
08024964  00f01281  beq.w	#548 ; -> 0x08024b8c ; branch_target=0x08024b8c
08024968  06d9      bls	#12 ; -> 0x08024978 ; branch_target=0x08024978
0802496a  22f48072  bic	r2, r2, #256
0802496e  b2f5007f  cmp.w	r2, #512
08024972  03d0      beq	#6 ; -> 0x0802497c ; branch_target=0x0802497c
08024974  0120      movs	r0, #1
08024976  86e7      b	#-244 ; -> 0x08024886 ; branch_target=0x08024886
08024978  002a      cmp	r2, #0
0802497a  fbd1      bne	#-10 ; -> 0x08024974 ; branch_target=0x08024974
0802497c  002d      cmp	r5, #0
0802497e  40f00281  bne.w	#516 ; -> 0x08024b86 ; branch_target=0x08024b86
08024982  ae4a      ldr	r2, [pc, #696] ; [0x08024c3c] = 0x58024400
08024984  301e      subs	r0, r6, #0
08024986  e16f      ldr	r1, [r4, #124]
08024988  536d      ldr	r3, [r2, #84]
0802498a  18bf      it	ne
0802498c  0120      movne	r0, #1
0802498e  23f44073  bic	r3, r3, #768
08024992  0b43      orrs	r3, r1
08024994  5365      str	r3, [r2, #84]
08024996  2368      ldr	r3, [r4]
08024998  75e7      b	#-278 ; -> 0x08024886 ; branch_target=0x08024886
0802499a  0126      movs	r6, #1
0802499c  3546      mov	r5, r6
0802499e  5be7      b	#-330 ; -> 0x08024858 ; branch_target=0x08024858
080249a0  0126      movs	r6, #1
080249a2  6be7      b	#-298 ; -> 0x0802487c ; branch_target=0x0802487c
080249a4  a54a      ldr	r2, [pc, #660] ; [0x08024c3c] = 0x58024400
080249a6  d36a      ldr	r3, [r2, #44]
080249a8  43f40033  orr	r3, r3, #131072
080249ac  d362      str	r3, [r2, #44]
080249ae  002d      cmp	r5, #0
080249b0  40f02881  bne.w	#592 ; -> 0x08024c04 ; branch_target=0x08024c04
080249b4  a14a      ldr	r2, [pc, #644] ; [0x08024c3c] = 0x58024400
080249b6  e16e      ldr	r1, [r4, #108]
080249b8  136d      ldr	r3, [r2, #80]
080249ba  23f04053  bic	r3, r3, #805306368
080249be  0b43      orrs	r3, r1
080249c0  1365      str	r3, [r2, #80]
080249c2  2368      ldr	r3, [r4]
080249c4  a0e5      b	#-1216 ; -> 0x08024508 ; branch_target=0x08024508
080249c6  9d4a      ldr	r2, [pc, #628] ; [0x08024c3c] = 0x58024400
080249c8  d36a      ldr	r3, [r2, #44]
080249ca  43f40033  orr	r3, r3, #131072
080249ce  d362      str	r3, [r2, #44]
080249d0  002d      cmp	r5, #0
080249d2  40f01481  bne.w	#552 ; -> 0x08024bfe ; branch_target=0x08024bfe
080249d6  994a      ldr	r2, [pc, #612] ; [0x08024c3c] = 0x58024400
080249d8  d4f88410  ldr.w	r1, [r4, #132]
080249dc  536d      ldr	r3, [r2, #84]
080249de  23f44013  bic	r3, r3, #3145728
080249e2  0b43      orrs	r3, r1
080249e4  5365      str	r3, [r2, #84]
080249e6  2368      ldr	r3, [r4]
080249e8  1be7      b	#-458 ; -> 0x08024822 ; branch_target=0x08024822
080249ea  944a      ldr	r2, [pc, #592] ; [0x08024c3c] = 0x58024400
080249ec  926a      ldr	r2, [r2, #40]
080249ee  02f00302  and	r2, r2, #3
080249f2  032a      cmp	r2, #3
080249f4  3ff401af  beq.w	#-510 ; -> 0x080247fa ; branch_target=0x080247fa
080249f8  0221      movs	r1, #2
080249fa  04f12400  add.w	r0, r4, #36
080249fe  fff7c1fb  bl	#-2174 ; -> 0x08024184 ; branch_target=0x08024184
08024a02  0546      mov	r5, r0
08024a04  002d      cmp	r5, #0
08024a06  40f04081  bne.w	#640 ; -> 0x08024c8a ; branch_target=0x08024c8a
08024a0a  8c4a      ldr	r2, [pc, #560] ; [0x08024c3c] = 0x58024400
08024a0c  d4f8a010  ldr.w	r1, [r4, #160]
08024a10  936d      ldr	r3, [r2, #88]
08024a12  23f44033  bic	r3, r3, #196608
08024a16  0b43      orrs	r3, r1
08024a18  9365      str	r3, [r2, #88]
08024a1a  2368      ldr	r3, [r4]
08024a1c  efe6      b	#-546 ; -> 0x080247fe ; branch_target=0x080247fe
08024a1e  874a      ldr	r2, [pc, #540] ; [0x08024c3c] = 0x58024400
08024a20  926a      ldr	r2, [r2, #40]
08024a22  02f00302  and	r2, r2, #3
08024a26  032a      cmp	r2, #3
08024a28  3ff402ae  beq.w	#-1020 ; -> 0x08024630 ; branch_target=0x08024630
08024a2c  0121      movs	r1, #1
08024a2e  201d      adds	r0, r4, #4
08024a30  fff736fb  bl	#-2452 ; -> 0x080240a0 ; branch_target=0x080240a0
08024a34  0546      mov	r5, r0
08024a36  002d      cmp	r5, #0
08024a38  40f02d81  bne.w	#602 ; -> 0x08024c96 ; branch_target=0x08024c96
08024a3c  7f4a      ldr	r2, [pc, #508] ; [0x08024c3c] = 0x58024400
08024a3e  a16f      ldr	r1, [r4, #120]
08024a40  536d      ldr	r3, [r2, #84]
08024a42  23f03803  bic	r3, r3, #56
08024a46  0b43      orrs	r3, r1
08024a48  5365      str	r3, [r2, #84]
08024a4a  2368      ldr	r3, [r4]
08024a4c  f2e5      b	#-1052 ; -> 0x08024634 ; branch_target=0x08024634
08024a4e  0126      movs	r6, #1
08024a50  39e4      b	#-1934 ; -> 0x080242c6 ; branch_target=0x080242c6
08024a52  7b4f      ldr	r7, [pc, #492] ; [0x08024c40] = 0x58024800
08024a54  3b68      ldr	r3, [r7]
08024a56  43f48073  orr	r3, r3, #256
08024a5a  3b60      str	r3, [r7]
08024a5c  fbf79efc  bl	#-18116 ; -> 0x0802039c ; branch_target=0x0802039c
08024a60  8046      mov	r8, r0
08024a62  06e0      b	#12 ; -> 0x08024a72 ; branch_target=0x08024a72
08024a64  fbf79afc  bl	#-18124 ; -> 0x0802039c ; branch_target=0x0802039c
08024a68  a0eb0800  sub.w	r0, r0, r8
08024a6c  6428      cmp	r0, #100
08024a6e  00f24782  bhi.w	#1166 ; -> 0x08024f00 ; branch_target=0x08024f00
08024a72  3b68      ldr	r3, [r7]
08024a74  da05      lsls	r2, r3, #23
08024a76  f5d5      bpl	#-22 ; -> 0x08024a64 ; branch_target=0x08024a64
08024a78  002d      cmp	r5, #0
08024a7a  40f04282  bne.w	#1156 ; -> 0x08024f02 ; branch_target=0x08024f02
08024a7e  6f4b      ldr	r3, [pc, #444] ; [0x08024c3c] = 0x58024400
08024a80  d4f8b020  ldr.w	r2, [r4, #176]
08024a84  196f      ldr	r1, [r3, #112]
08024a86  5140      eors	r1, r2
08024a88  11f4407f  tst.w	r1, #768
08024a8c  0dd0      beq	#26 ; -> 0x08024aaa ; branch_target=0x08024aaa
08024a8e  1a6f      ldr	r2, [r3, #112]
08024a90  196f      ldr	r1, [r3, #112]
08024a92  22f44072  bic	r2, r2, #768
08024a96  41f48031  orr	r1, r1, #65536
08024a9a  1967      str	r1, [r3, #112]
08024a9c  196f      ldr	r1, [r3, #112]
08024a9e  21f48031  bic	r1, r1, #65536
08024aa2  1967      str	r1, [r3, #112]
08024aa4  1a67      str	r2, [r3, #112]
08024aa6  d4f8b020  ldr.w	r2, [r4, #176]
08024aaa  b2f5807f  cmp.w	r2, #256
08024aae  00f04082  beq.w	#1152 ; -> 0x08024f32 ; branch_target=0x08024f32
08024ab2  02f44073  and	r3, r2, #768
08024ab6  b3f5407f  cmp.w	r3, #768
08024aba  00f05182  beq.w	#1186 ; -> 0x08024f60 ; branch_target=0x08024f60
08024abe  5f4a      ldr	r2, [pc, #380] ; [0x08024c3c] = 0x58024400
08024ac0  1369      ldr	r3, [r2, #16]
08024ac2  23f47c53  bic	r3, r3, #16128
08024ac6  1361      str	r3, [r2, #16]
08024ac8  5c4a      ldr	r2, [pc, #368] ; [0x08024c3c] = 0x58024400
08024aca  d4f8b030  ldr.w	r3, [r4, #176]
08024ace  116f      ldr	r1, [r2, #112]
08024ad0  c3f30b03  ubfx	r3, r3, #0, #12
08024ad4  0b43      orrs	r3, r1
08024ad6  1367      str	r3, [r2, #112]
08024ad8  2368      ldr	r3, [r4]
08024ada  79e5      b	#-1294 ; -> 0x080245d0 ; branch_target=0x080245d0
08024adc  574a      ldr	r2, [pc, #348] ; [0x08024c3c] = 0x58024400
08024ade  d36a      ldr	r3, [r2, #44]
08024ae0  43f40033  orr	r3, r3, #131072
08024ae4  d362      str	r3, [r2, #44]
08024ae6  3546      mov	r5, r6
08024ae8  002d      cmp	r5, #0
08024aea  00f06d81  beq.w	#730 ; -> 0x08024dc8 ; branch_target=0x08024dc8
08024aee  2368      ldr	r3, [r4]
08024af0  2e46      mov	r6, r5
08024af2  fff7f6bb  b.w	#-2068 ; -> 0x080242e2 ; branch_target=0x080242e2
08024af6  514a      ldr	r2, [pc, #324] ; [0x08024c3c] = 0x58024400
08024af8  d36a      ldr	r3, [r2, #44]
08024afa  43f40033  orr	r3, r3, #131072
08024afe  d362      str	r3, [r2, #44]
08024b00  002d      cmp	r5, #0
08024b02  3ff433ac  beq.w	#-1946 ; -> 0x0802436c ; branch_target=0x0802436c
08024b06  9de0      b	#314 ; -> 0x08024c44 ; branch_target=0x08024c44
08024b08  4c4a      ldr	r2, [pc, #304] ; [0x08024c3c] = 0x58024400
08024b0a  d36a      ldr	r3, [r2, #44]
08024b0c  43f40033  orr	r3, r3, #131072
08024b10  d362      str	r3, [r2, #44]
08024b12  002d      cmp	r5, #0
08024b14  3ff453ac  beq.w	#-1882 ; -> 0x080243be ; branch_target=0x080243be
08024b18  84e0      b	#264 ; -> 0x08024c24 ; branch_target=0x08024c24
08024b1a  484a      ldr	r2, [pc, #288] ; [0x08024c3c] = 0x58024400
08024b1c  d36a      ldr	r3, [r2, #44]
08024b1e  43f40033  orr	r3, r3, #131072
08024b22  d362      str	r3, [r2, #44]
08024b24  002d      cmp	r5, #0
08024b26  3ff48eac  beq.w	#-1764 ; -> 0x08024446 ; branch_target=0x08024446
08024b2a  83e0      b	#262 ; -> 0x08024c34 ; branch_target=0x08024c34
08024b2c  434a      ldr	r2, [pc, #268] ; [0x08024c3c] = 0x58024400
08024b2e  d36a      ldr	r3, [r2, #44]
08024b30  43f40033  orr	r3, r3, #131072
08024b34  d362      str	r3, [r2, #44]
08024b36  002d      cmp	r5, #0
08024b38  3ff4f0ab  beq.w	#-2080 ; -> 0x0802431c ; branch_target=0x0802431c
08024b3c  76e0      b	#236 ; -> 0x08024c2c ; branch_target=0x08024c2c
08024b3e  3f4b      ldr	r3, [pc, #252] ; [0x08024c3c] = 0x58024400
08024b40  9b6a      ldr	r3, [r3, #40]
08024b42  03f00303  and	r3, r3, #3
08024b46  032b      cmp	r3, #3
08024b48  09d0      beq	#18 ; -> 0x08024b5e ; branch_target=0x08024b5e
08024b4a  0221      movs	r1, #2
08024b4c  04f12400  add.w	r0, r4, #36
08024b50  fff718fb  bl	#-2512 ; -> 0x08024184 ; branch_target=0x08024184
08024b54  d4f88020  ldr.w	r2, [r4, #128]
08024b58  0028      cmp	r0, #0
08024b5a  3ff429ae  beq.w	#-942 ; -> 0x080247b0 ; branch_target=0x080247b0
08024b5e  0126      movs	r6, #1
08024b60  26e6      b	#-948 ; -> 0x080247b0 ; branch_target=0x080247b0
08024b62  364b      ldr	r3, [pc, #216] ; [0x08024c3c] = 0x58024400
08024b64  9b6a      ldr	r3, [r3, #40]
08024b66  03f00303  and	r3, r3, #3
08024b6a  032b      cmp	r3, #3
08024b6c  09d0      beq	#18 ; -> 0x08024b82 ; branch_target=0x08024b82
08024b6e  0221      movs	r1, #2
08024b70  04f12400  add.w	r0, r4, #36
08024b74  fff706fb  bl	#-2548 ; -> 0x08024184 ; branch_target=0x08024184
08024b78  d4f89420  ldr.w	r2, [r4, #148]
08024b7c  0028      cmp	r0, #0
08024b7e  3ff426ae  beq.w	#-948 ; -> 0x080247ce ; branch_target=0x080247ce
08024b82  0126      movs	r6, #1
08024b84  23e6      b	#-954 ; -> 0x080247ce ; branch_target=0x080247ce
08024b86  2368      ldr	r3, [r4]
08024b88  0120      movs	r0, #1
08024b8a  7ce6      b	#-776 ; -> 0x08024886 ; branch_target=0x08024886
08024b8c  2b4a      ldr	r2, [pc, #172] ; [0x08024c3c] = 0x58024400
08024b8e  d36a      ldr	r3, [r2, #44]
08024b90  43f40033  orr	r3, r3, #131072
08024b94  d362      str	r3, [r2, #44]
08024b96  002d      cmp	r5, #0
08024b98  3ff4f3ae  beq.w	#-538 ; -> 0x08024982 ; branch_target=0x08024982
08024b9c  f3e7      b	#-26 ; -> 0x08024b86 ; branch_target=0x08024b86
08024b9e  274a      ldr	r2, [pc, #156] ; [0x08024c3c] = 0x58024400
08024ba0  d36a      ldr	r3, [r2, #44]
08024ba2  43f40033  orr	r3, r3, #131072
08024ba6  d362      str	r3, [r2, #44]
08024ba8  002d      cmp	r5, #0
08024baa  7ff453ae  bne.w	#-858 ; -> 0x08024854 ; branch_target=0x08024854
08024bae  234a      ldr	r2, [pc, #140] ; [0x08024c3c] = 0x58024400
08024bb0  e16c      ldr	r1, [r4, #76]
08024bb2  d36c      ldr	r3, [r2, #76]
08024bb4  23f48033  bic	r3, r3, #65536
08024bb8  0b43      orrs	r3, r1
08024bba  d364      str	r3, [r2, #76]
08024bbc  2368      ldr	r3, [r4]
08024bbe  4be6      b	#-874 ; -> 0x08024858 ; branch_target=0x08024858
08024bc0  1e4a      ldr	r2, [pc, #120] ; [0x08024c3c] = 0x58024400
08024bc2  926a      ldr	r2, [r2, #40]
08024bc4  02f00302  and	r2, r2, #3
08024bc8  032a      cmp	r2, #3
08024bca  3ff49bac  beq.w	#-1738 ; -> 0x08024504 ; branch_target=0x08024504
08024bce  0121      movs	r1, #1
08024bd0  201d      adds	r0, r4, #4
08024bd2  fff765fa  bl	#-2870 ; -> 0x080240a0 ; branch_target=0x080240a0
08024bd6  0546      mov	r5, r0
08024bd8  002d      cmp	r5, #0
08024bda  3ff4ebae  beq.w	#-554 ; -> 0x080249b4 ; branch_target=0x080249b4
08024bde  11e0      b	#34 ; -> 0x08024c04 ; branch_target=0x08024c04
08024be0  164a      ldr	r2, [pc, #88] ; [0x08024c3c] = 0x58024400
08024be2  926a      ldr	r2, [r2, #40]
08024be4  02f00302  and	r2, r2, #3
08024be8  032a      cmp	r2, #3
08024bea  3ff406ae  beq.w	#-1012 ; -> 0x080247fa ; branch_target=0x080247fa
08024bee  201d      adds	r0, r4, #4
08024bf0  fff756fa  bl	#-2900 ; -> 0x080240a0 ; branch_target=0x080240a0
08024bf4  0546      mov	r5, r0
08024bf6  002d      cmp	r5, #0
08024bf8  3ff407af  beq.w	#-498 ; -> 0x08024a0a ; branch_target=0x08024a0a
08024bfc  45e0      b	#138 ; -> 0x08024c8a ; branch_target=0x08024c8a
08024bfe  2368      ldr	r3, [r4]
08024c00  2e46      mov	r6, r5
08024c02  0ee6      b	#-996 ; -> 0x08024822 ; branch_target=0x08024822
08024c04  2368      ldr	r3, [r4]
08024c06  2e46      mov	r6, r5
08024c08  7ee4      b	#-1796 ; -> 0x08024508 ; branch_target=0x08024508
08024c0a  2368      ldr	r3, [r4]
08024c0c  2e46      mov	r6, r5
08024c0e  fff7fbbb  b.w	#-2058 ; -> 0x08024408 ; branch_target=0x08024408
08024c12  0a4a      ldr	r2, [pc, #40] ; [0x08024c3c] = 0x58024400
08024c14  616c      ldr	r1, [r4, #68]
08024c16  d36c      ldr	r3, [r2, #76]
08024c18  23f00303  bic	r3, r3, #3
08024c1c  0b43      orrs	r3, r1
08024c1e  d364      str	r3, [r2, #76]
08024c20  2368      ldr	r3, [r4]
08024c22  d2e4      b	#-1628 ; -> 0x080245ca ; branch_target=0x080245ca
08024c24  2368      ldr	r3, [r4]
08024c26  2e46      mov	r6, r5
08024c28  fff7d2bb  b.w	#-2140 ; -> 0x080243d0 ; branch_target=0x080243d0
08024c2c  2368      ldr	r3, [r4]
08024c2e  2e46      mov	r6, r5
08024c30  fff77cbb  b.w	#-2312 ; -> 0x0802432c ; branch_target=0x0802432c
08024c34  2368      ldr	r3, [r4]
08024c36  2e46      mov	r6, r5
08024c38  0de4      b	#-2022 ; -> 0x08024456 ; branch_target=0x08024456
08024c44  2368      ldr	r3, [r4]
08024c46  2e46      mov	r6, r5
08024c48  fff799bb  b.w	#-2254 ; -> 0x0802437e ; branch_target=0x0802437e
08024c4c  2368      ldr	r3, [r4]
08024c4e  2e46      mov	r6, r5
08024c50  58e5      b	#-1360 ; -> 0x08024704 ; branch_target=0x08024704
08024c52  2368      ldr	r3, [r4]
08024c54  2e46      mov	r6, r5
08024c56  a3e5      b	#-1210 ; -> 0x080247a0 ; branch_target=0x080247a0
08024c58  2368      ldr	r3, [r4]
08024c5a  2e46      mov	r6, r5
08024c5c  79e5      b	#-1294 ; -> 0x08024752 ; branch_target=0x08024752
08024c5e  2368      ldr	r3, [r4]
08024c60  2e46      mov	r6, r5
08024c62  1de4      b	#-1990 ; -> 0x080244a0 ; branch_target=0x080244a0
08024c64  bd4a      ldr	r2, [pc, #756] ; [0x08024f5c] = 0x58024400
08024c66  616f      ldr	r1, [r4, #116]
08024c68  536d      ldr	r3, [r2, #84]
08024c6a  23f00703  bic	r3, r3, #7
08024c6e  0b43      orrs	r3, r1
08024c70  5365      str	r3, [r2, #84]
08024c72  2368      ldr	r3, [r4]
08024c74  fde4      b	#-1542 ; -> 0x08024672 ; branch_target=0x08024672
08024c76  b94a      ldr	r2, [pc, #740] ; [0x08024f5c] = 0x58024400
08024c78  d4f89010  ldr.w	r1, [r4, #144]
08024c7c  936d      ldr	r3, [r2, #88]
08024c7e  23f00703  bic	r3, r3, #7
08024c82  0b43      orrs	r3, r1
08024c84  9365      str	r3, [r2, #88]
08024c86  2368      ldr	r3, [r4]
08024c88  15e5      b	#-1494 ; -> 0x080246b6 ; branch_target=0x080246b6
08024c8a  2368      ldr	r3, [r4]
08024c8c  2e46      mov	r6, r5
08024c8e  b6e5      b	#-1172 ; -> 0x080247fe ; branch_target=0x080247fe
08024c90  2368      ldr	r3, [r4]
08024c92  2e46      mov	r6, r5
08024c94  28e4      b	#-1968 ; -> 0x080244e8 ; branch_target=0x080244e8
08024c96  2368      ldr	r3, [r4]
08024c98  2e46      mov	r6, r5
08024c9a  cbe4      b	#-1642 ; -> 0x08024634 ; branch_target=0x08024634
08024c9c  af4a      ldr	r2, [pc, #700] ; [0x08024f5c] = 0x58024400
08024c9e  926a      ldr	r2, [r2, #40]
08024ca0  02f00302  and	r2, r2, #3
08024ca4  032a      cmp	r2, #3
08024ca6  3ff4baad  beq.w	#-1164 ; -> 0x0802481e ; branch_target=0x0802481e
08024caa  0121      movs	r1, #1
08024cac  04f12400  add.w	r0, r4, #36
08024cb0  fff768fa  bl	#-2864 ; -> 0x08024184 ; branch_target=0x08024184
08024cb4  0546      mov	r5, r0
08024cb6  002d      cmp	r5, #0
08024cb8  3ff48dae  beq.w	#-742 ; -> 0x080249d6 ; branch_target=0x080249d6
08024cbc  9fe7      b	#-194 ; -> 0x08024bfe ; branch_target=0x08024bfe
08024cbe  a74a      ldr	r2, [pc, #668] ; [0x08024f5c] = 0x58024400
08024cc0  926a      ldr	r2, [r2, #40]
08024cc2  02f00302  and	r2, r2, #3
08024cc6  032a      cmp	r2, #3
08024cc8  3ff467ac  beq.w	#-1842 ; -> 0x0802459a ; branch_target=0x0802459a
08024ccc  0121      movs	r1, #1
08024cce  04f12400  add.w	r0, r4, #36
08024cd2  fff757fa  bl	#-2898 ; -> 0x08024184 ; branch_target=0x08024184
08024cd6  0546      mov	r5, r0
08024cd8  002d      cmp	r5, #0
08024cda  3ff4d9ab  beq.w	#-2126 ; -> 0x08024490 ; branch_target=0x08024490
08024cde  bee7      b	#-132 ; -> 0x08024c5e ; branch_target=0x08024c5e
08024ce0  9e4a      ldr	r2, [pc, #632] ; [0x08024f5c] = 0x58024400
08024ce2  926a      ldr	r2, [r2, #40]
08024ce4  02f00302  and	r2, r2, #3
08024ce8  032a      cmp	r2, #3
08024cea  3ff43eac  beq.w	#-1924 ; -> 0x0802456a ; branch_target=0x0802456a
08024cee  0221      movs	r1, #2
08024cf0  201d      adds	r0, r4, #4
08024cf2  fff7d5f9  bl	#-3158 ; -> 0x080240a0 ; branch_target=0x080240a0
08024cf6  0546      mov	r5, r0
08024cf8  002d      cmp	r5, #0
08024cfa  3ff47dab  beq.w	#-2310 ; -> 0x080243f8 ; branch_target=0x080243f8
08024cfe  84e7      b	#-248 ; -> 0x08024c0a ; branch_target=0x08024c0a
08024d00  964a      ldr	r2, [pc, #600] ; [0x08024f5c] = 0x58024400
08024d02  926a      ldr	r2, [r2, #40]
08024d04  02f00302  and	r2, r2, #3
08024d08  032a      cmp	r2, #3
08024d0a  3ff408ae  beq.w	#-1008 ; -> 0x0802491e ; branch_target=0x0802491e
08024d0e  0221      movs	r1, #2
08024d10  04f12400  add.w	r0, r4, #36
08024d14  fff736fa  bl	#-2964 ; -> 0x08024184 ; branch_target=0x08024184
08024d18  0546      mov	r5, r0
08024d1a  002d      cmp	r5, #0
08024d1c  3ff4e9ac  beq.w	#-1582 ; -> 0x080246f2 ; branch_target=0x080246f2
08024d20  94e7      b	#-216 ; -> 0x08024c4c ; branch_target=0x08024c4c
08024d22  8e4a      ldr	r2, [pc, #568] ; [0x08024f5c] = 0x58024400
08024d24  926a      ldr	r2, [r2, #40]
08024d26  02f00302  and	r2, r2, #3
08024d2a  032a      cmp	r2, #3
08024d2c  3ff404ae  beq.w	#-1016 ; -> 0x08024938 ; branch_target=0x08024938
08024d30  0221      movs	r1, #2
08024d32  04f12400  add.w	r0, r4, #36
08024d36  fff725fa  bl	#-2998 ; -> 0x08024184 ; branch_target=0x08024184
08024d3a  0546      mov	r5, r0
08024d3c  002d      cmp	r5, #0
08024d3e  3ff4ffac  beq.w	#-1538 ; -> 0x08024740 ; branch_target=0x08024740
08024d42  89e7      b	#-238 ; -> 0x08024c58 ; branch_target=0x08024c58
08024d44  854a      ldr	r2, [pc, #532] ; [0x08024f5c] = 0x58024400
08024d46  926a      ldr	r2, [r2, #40]
08024d48  02f00302  and	r2, r2, #3
08024d4c  032a      cmp	r2, #3
08024d4e  3ff403ae  beq.w	#-1018 ; -> 0x08024958 ; branch_target=0x08024958
08024d52  0221      movs	r1, #2
08024d54  04f12400  add.w	r0, r4, #36
08024d58  fff714fa  bl	#-3032 ; -> 0x08024184 ; branch_target=0x08024184
08024d5c  0546      mov	r5, r0
08024d5e  002d      cmp	r5, #0
08024d60  3ff415ad  beq.w	#-1494 ; -> 0x0802478e ; branch_target=0x0802478e
08024d64  75e7      b	#-278 ; -> 0x08024c52 ; branch_target=0x08024c52
08024d66  7d4a      ldr	r2, [pc, #500] ; [0x08024f5c] = 0x58024400
08024d68  926a      ldr	r2, [r2, #40]
08024d6a  02f00302  and	r2, r2, #3
08024d6e  032a      cmp	r2, #3
08024d70  00f0db80  beq.w	#438 ; -> 0x08024f2a ; branch_target=0x08024f2a
08024d74  0221      movs	r1, #2
08024d76  201d      adds	r0, r4, #4
08024d78  fff792f9  bl	#-3292 ; -> 0x080240a0 ; branch_target=0x080240a0
08024d7c  0546      mov	r5, r0
08024d7e  002d      cmp	r5, #0
08024d80  3ff447af  beq.w	#-370 ; -> 0x08024c12 ; branch_target=0x08024c12
08024d84  1fe4      b	#-1986 ; -> 0x080245c6 ; branch_target=0x080245c6
08024d86  754a      ldr	r2, [pc, #468] ; [0x08024f5c] = 0x58024400
08024d88  926a      ldr	r2, [r2, #40]
08024d8a  02f00302  and	r2, r2, #3
08024d8e  032a      cmp	r2, #3
08024d90  3ff40eac  beq.w	#-2020 ; -> 0x080245b0 ; branch_target=0x080245b0
08024d94  0121      movs	r1, #1
08024d96  04f12400  add.w	r0, r4, #36
08024d9a  fff7f3f9  bl	#-3098 ; -> 0x08024184 ; branch_target=0x08024184
08024d9e  0546      mov	r5, r0
08024da0  002d      cmp	r5, #0
08024da2  3ff498ab  beq.w	#-2256 ; -> 0x080244d6 ; branch_target=0x080244d6
08024da6  73e7      b	#-282 ; -> 0x08024c90 ; branch_target=0x08024c90
08024da8  6c4a      ldr	r2, [pc, #432] ; [0x08024f5c] = 0x58024400
08024daa  926a      ldr	r2, [r2, #40]
08024dac  02f00302  and	r2, r2, #3
08024db0  032a      cmp	r2, #3
08024db2  3ff494aa  beq.w	#-2776 ; -> 0x080242de ; branch_target=0x080242de
08024db6  0021      movs	r1, #0
08024db8  04f12400  add.w	r0, r4, #36
08024dbc  fff7e2f9  bl	#-3132 ; -> 0x08024184 ; branch_target=0x08024184
08024dc0  0546      mov	r5, r0
08024dc2  002d      cmp	r5, #0
08024dc4  7ff493ae  bne.w	#-730 ; -> 0x08024aee ; branch_target=0x08024aee
08024dc8  644a      ldr	r2, [pc, #400] ; [0x08024f5c] = 0x58024400
08024dca  616d      ldr	r1, [r4, #84]
08024dcc  136d      ldr	r3, [r2, #80]
08024dce  23f00703  bic	r3, r3, #7
08024dd2  0b43      orrs	r3, r1
08024dd4  1365      str	r3, [r2, #80]
08024dd6  2368      ldr	r3, [r4]
08024dd8  fff783ba  b.w	#-2810 ; -> 0x080242e2 ; branch_target=0x080242e2
08024ddc  5f4a      ldr	r2, [pc, #380] ; [0x08024f5c] = 0x58024400
08024dde  926a      ldr	r2, [r2, #40]
08024de0  02f00302  and	r2, r2, #3
08024de4  032a      cmp	r2, #3
08024de6  3ff4cbab  beq.w	#-2154 ; -> 0x08024580 ; branch_target=0x08024580
08024dea  0021      movs	r1, #0
08024dec  04f12400  add.w	r0, r4, #36
08024df0  fff7c8f9  bl	#-3184 ; -> 0x08024184 ; branch_target=0x08024184
08024df4  0546      mov	r5, r0
08024df6  002d      cmp	r5, #0
08024df8  3ff425ab  beq.w	#-2486 ; -> 0x08024446 ; branch_target=0x08024446
08024dfc  1ae7      b	#-460 ; -> 0x08024c34 ; branch_target=0x08024c34
08024dfe  574a      ldr	r2, [pc, #348] ; [0x08024f5c] = 0x58024400
08024e00  926a      ldr	r2, [r2, #40]
08024e02  02f00302  and	r2, r2, #3
08024e06  032a      cmp	r2, #3
08024e08  3ff421ae  beq.w	#-958 ; -> 0x08024a4e ; branch_target=0x08024a4e
08024e0c  0221      movs	r1, #2
08024e0e  2430      adds	r0, #36
08024e10  fff7b8f9  bl	#-3216 ; -> 0x08024184 ; branch_target=0x08024184
08024e14  0646      mov	r6, r0
08024e16  fff747ba  b.w	#-2930 ; -> 0x080242a8 ; branch_target=0x080242a8
08024e1a  504a      ldr	r2, [pc, #320] ; [0x08024f5c] = 0x58024400
08024e1c  926a      ldr	r2, [r2, #40]
08024e1e  02f00302  and	r2, r2, #3
08024e22  032a      cmp	r2, #3
08024e24  3ff490ab  beq.w	#-2272 ; -> 0x08024548 ; branch_target=0x08024548
08024e28  0021      movs	r1, #0
08024e2a  04f12400  add.w	r0, r4, #36
08024e2e  fff7a9f9  bl	#-3246 ; -> 0x08024184 ; branch_target=0x08024184
08024e32  0546      mov	r5, r0
08024e34  002d      cmp	r5, #0
08024e36  3ff499aa  beq.w	#-2766 ; -> 0x0802436c ; branch_target=0x0802436c
08024e3a  03e7      b	#-506 ; -> 0x08024c44 ; branch_target=0x08024c44
08024e3c  474a      ldr	r2, [pc, #284] ; [0x08024f5c] = 0x58024400
08024e3e  926a      ldr	r2, [r2, #40]
08024e40  02f00302  and	r2, r2, #3
08024e44  032a      cmp	r2, #3
08024e46  3ff44aaa  beq.w	#-2924 ; -> 0x080242de ; branch_target=0x080242de
08024e4a  0021      movs	r1, #0
08024e4c  201d      adds	r0, r4, #4
08024e4e  fff727f9  bl	#-3506 ; -> 0x080240a0 ; branch_target=0x080240a0
08024e52  0546      mov	r5, r0
08024e54  48e6      b	#-880 ; -> 0x08024ae8 ; branch_target=0x08024ae8
08024e56  414a      ldr	r2, [pc, #260] ; [0x08024f5c] = 0x58024400
08024e58  926a      ldr	r2, [r2, #40]
08024e5a  02f00302  and	r2, r2, #3
08024e5e  032a      cmp	r2, #3
08024e60  3ff47dab  beq.w	#-2310 ; -> 0x0802455e ; branch_target=0x0802455e
08024e64  0021      movs	r1, #0
08024e66  04f12400  add.w	r0, r4, #36
08024e6a  fff78bf9  bl	#-3306 ; -> 0x08024184 ; branch_target=0x08024184
08024e6e  0546      mov	r5, r0
08024e70  002d      cmp	r5, #0
08024e72  3ff4a4aa  beq.w	#-2744 ; -> 0x080243be ; branch_target=0x080243be
08024e76  d5e6      b	#-598 ; -> 0x08024c24 ; branch_target=0x08024c24
08024e78  384a      ldr	r2, [pc, #224] ; [0x08024f5c] = 0x58024400
08024e7a  926a      ldr	r2, [r2, #40]
08024e7c  02f00302  and	r2, r2, #3
08024e80  032a      cmp	r2, #3
08024e82  3ff456ab  beq.w	#-2388 ; -> 0x08024532 ; branch_target=0x08024532
08024e86  0021      movs	r1, #0
08024e88  04f12400  add.w	r0, r4, #36
08024e8c  fff77af9  bl	#-3340 ; -> 0x08024184 ; branch_target=0x08024184
08024e90  0546      mov	r5, r0
08024e92  002d      cmp	r5, #0
08024e94  3ff442aa  beq.w	#-2940 ; -> 0x0802431c ; branch_target=0x0802431c
08024e98  c8e6      b	#-624 ; -> 0x08024c2c ; branch_target=0x08024c2c
08024e9a  304a      ldr	r2, [pc, #192] ; [0x08024f5c] = 0x58024400
08024e9c  926a      ldr	r2, [r2, #40]
08024e9e  02f00302  and	r2, r2, #3
08024ea2  032a      cmp	r2, #3
08024ea4  39d0      beq	#114 ; -> 0x08024f1a ; branch_target=0x08024f1a
08024ea6  0121      movs	r1, #1
08024ea8  04f12400  add.w	r0, r4, #36
08024eac  fff76af9  bl	#-3372 ; -> 0x08024184 ; branch_target=0x08024184
08024eb0  0546      mov	r5, r0
08024eb2  002d      cmp	r5, #0
08024eb4  3ff4dfae  beq.w	#-578 ; -> 0x08024c76 ; branch_target=0x08024c76
08024eb8  fff7fbbb  b.w	#-2058 ; -> 0x080246b2 ; branch_target=0x080246b2
08024ebc  274a      ldr	r2, [pc, #156] ; [0x08024f5c] = 0x58024400
08024ebe  926a      ldr	r2, [r2, #40]
08024ec0  02f00302  and	r2, r2, #3
08024ec4  032a      cmp	r2, #3
08024ec6  2cd0      beq	#88 ; -> 0x08024f22 ; branch_target=0x08024f22
08024ec8  0121      movs	r1, #1
08024eca  04f12400  add.w	r0, r4, #36
08024ece  fff759f9  bl	#-3406 ; -> 0x08024184 ; branch_target=0x08024184
08024ed2  0546      mov	r5, r0
08024ed4  002d      cmp	r5, #0
08024ed6  3ff4c5ae  beq.w	#-630 ; -> 0x08024c64 ; branch_target=0x08024c64
08024eda  fff7c8bb  b.w	#-2160 ; -> 0x0802466e ; branch_target=0x0802466e
08024ede  1f4a      ldr	r2, [pc, #124] ; [0x08024f5c] = 0x58024400
08024ee0  926a      ldr	r2, [r2, #40]
08024ee2  02f00302  and	r2, r2, #3
08024ee6  032a      cmp	r2, #3
08024ee8  3ff4a2ab  beq.w	#-2236 ; -> 0x08024630 ; branch_target=0x08024630
08024eec  0121      movs	r1, #1
08024eee  04f12400  add.w	r0, r4, #36
08024ef2  fff747f9  bl	#-3442 ; -> 0x08024184 ; branch_target=0x08024184
08024ef6  0546      mov	r5, r0
08024ef8  002d      cmp	r5, #0
08024efa  3ff49fad  beq.w	#-1218 ; -> 0x08024a3c ; branch_target=0x08024a3c
08024efe  cae6      b	#-620 ; -> 0x08024c96 ; branch_target=0x08024c96
08024f00  0325      movs	r5, #3
08024f02  2368      ldr	r3, [r4]
08024f04  2e46      mov	r6, r5
08024f06  fff763bb  b.w	#-2362 ; -> 0x080245d0 ; branch_target=0x080245d0
08024f0a  144a      ldr	r2, [pc, #80] ; [0x08024f5c] = 0x58024400
08024f0c  d36a      ldr	r3, [r2, #44]
08024f0e  43f40033  orr	r3, r3, #131072
08024f12  d362      str	r3, [r2, #44]
08024f14  426e      ldr	r2, [r0, #100]
08024f16  fff7ceb9  b.w	#-3172 ; -> 0x080242b6 ; branch_target=0x080242b6
08024f1a  0126      movs	r6, #1
08024f1c  3546      mov	r5, r6
08024f1e  fff7cabb  b.w	#-2156 ; -> 0x080246b6 ; branch_target=0x080246b6
08024f22  0126      movs	r6, #1
08024f24  3546      mov	r5, r6
08024f26  fff7a4bb  b.w	#-2232 ; -> 0x08024672 ; branch_target=0x08024672
08024f2a  0126      movs	r6, #1
08024f2c  3546      mov	r5, r6
08024f2e  fff74cbb  b.w	#-2408 ; -> 0x080245ca ; branch_target=0x080245ca
08024f32  fbf733fa  bl	#-19354 ; -> 0x0802039c ; branch_target=0x0802039c
08024f36  dff82480  ldr.w	r8, [pc, #36] ; [0x08024f5c] = 0x58024400
08024f3a  0746      mov	r7, r0
08024f3c  41f28839  movw	r9, #5000
08024f40  04e0      b	#8 ; -> 0x08024f4c ; branch_target=0x08024f4c
08024f42  fbf72bfa  bl	#-19370 ; -> 0x0802039c ; branch_target=0x0802039c
08024f46  c01b      subs	r0, r0, r7
08024f48  4845      cmp	r0, r9
08024f4a  13d8      bhi	#38 ; -> 0x08024f74 ; branch_target=0x08024f74
08024f4c  d8f87030  ldr.w	r3, [r8, #112]
08024f50  9b07      lsls	r3, r3, #30
08024f52  f6d5      bpl	#-20 ; -> 0x08024f42 ; branch_target=0x08024f42
08024f54  d4f8b020  ldr.w	r2, [r4, #176]
08024f58  abe5      b	#-1194 ; -> 0x08024ab2 ; branch_target=0x08024ab2
08024f60  0749      ldr	r1, [pc, #28] ; [0x08024f80] = 0x58024400
08024f62  084b      ldr	r3, [pc, #32] ; [0x08024f84] = 0x00ffffcf
08024f64  03ea1213  and.w	r3, r3, r2, lsr #4
08024f68  0a69      ldr	r2, [r1, #16]
08024f6a  22f47c52  bic	r2, r2, #16128
08024f6e  1343      orrs	r3, r2
08024f70  0b61      str	r3, [r1, #16]
08024f72  a9e5      b	#-1198 ; -> 0x08024ac8 ; branch_target=0x08024ac8
08024f74  0326      movs	r6, #3
08024f76  2368      ldr	r3, [r4]
08024f78  3546      mov	r5, r6
08024f7a  fff729bb  b.w	#-2478 ; -> 0x080245d0 ; branch_target=0x080245d0
