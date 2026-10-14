; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: prologue_heuristic
08025328  00b5      push	{lr}
0802532a  85b0      sub	sp, #20
0802532c  74d0      beq	#232 ; -> 0x08025418 ; branch_target=0x08025418
0802532e  b0f5007f  cmp.w	r0, #512
08025332  2cd0      beq	#88 ; -> 0x0802538e ; branch_target=0x0802538e
08025334  b0f5806f  cmp.w	r0, #1024
08025338  00f0cd80  beq.w	#410 ; -> 0x080254d6 ; branch_target=0x080254d6
0802533c  b0f5006f  cmp.w	r0, #2048
08025340  7ed0      beq	#252 ; -> 0x08025440 ; branch_target=0x08025440
08025342  b0f5805f  cmp.w	r0, #4096
08025346  58d0      beq	#176 ; -> 0x080253fa ; branch_target=0x080253fa
08025348  b0f5002f  cmp.w	r0, #524288
0802534c  00f00281  beq.w	#516 ; -> 0x08025554 ; branch_target=0x08025554
08025350  b0f5803f  cmp.w	r0, #65536
08025354  00f00d81  beq.w	#538 ; -> 0x08025572 ; branch_target=0x08025572
08025358  b0f5804f  cmp.w	r0, #16384
0802535c  00f0e180  beq.w	#450 ; -> 0x08025522 ; branch_target=0x08025522
08025360  b0f5004f  cmp.w	r0, #32768
08025364  38d1      bne	#112 ; -> 0x080253d8 ; branch_target=0x080253d8
08025366  994b      ldr	r3, [pc, #612] ; [0x080255cc] = 0x58024400
08025368  1b6d      ldr	r3, [r3, #80]
0802536a  03f04053  and	r3, r3, #805306368
0802536e  b3f1805f  cmp.w	r3, #268435456
08025372  00f08d80  beq.w	#282 ; -> 0x08025490 ; branch_target=0x08025490
08025376  b3f1005f  cmp.w	r3, #536870912
0802537a  00f03d81  beq.w	#634 ; -> 0x080255f8 ; branch_target=0x080255f8
0802537e  5bbb      cbnz	r3, #86 ; -> 0x080253d8 ; branch_target=0x080253d8
08025380  924b      ldr	r3, [pc, #584] ; [0x080255cc] = 0x58024400
08025382  1868      ldr	r0, [r3]
08025384  10f40030  ands	r0, r0, #131072
08025388  43d0      beq	#134 ; -> 0x08025412 ; branch_target=0x08025412
0802538a  9148      ldr	r0, [pc, #580] ; [0x080255d0] = 0x017d7840
0802538c  41e0      b	#130 ; -> 0x08025412 ; branch_target=0x08025412
0802538e  8f4a      ldr	r2, [pc, #572] ; [0x080255cc] = 0x58024400
08025390  136d      ldr	r3, [r2, #80]
08025392  03f4e073  and	r3, r3, #448
08025396  802b      cmp	r3, #128
08025398  00f09880  beq.w	#304 ; -> 0x080254cc ; branch_target=0x080254cc
0802539c  20d9      bls	#64 ; -> 0x080253e0 ; branch_target=0x080253e0
0802539e  c02b      cmp	r3, #192
080253a0  36d0      beq	#108 ; -> 0x08025410 ; branch_target=0x08025410
080253a2  b3f5807f  cmp.w	r3, #256
080253a6  17d1      bne	#46 ; -> 0x080253d8 ; branch_target=0x080253d8
080253a8  d36c      ldr	r3, [r2, #76]
080253aa  1168      ldr	r1, [r2]
080253ac  03f04053  and	r3, r3, #805306368
080253b0  4907      lsls	r1, r1, #29
080253b2  02d5      bpl	#4 ; -> 0x080253ba ; branch_target=0x080253ba
080253b4  002b      cmp	r3, #0
080253b6  00f0ae80  beq.w	#348 ; -> 0x08025516 ; branch_target=0x08025516
080253ba  844a      ldr	r2, [pc, #528] ; [0x080255cc] = 0x58024400
080253bc  1268      ldr	r2, [r2]
080253be  d005      lsls	r0, r2, #23
080253c0  03d5      bpl	#6 ; -> 0x080253ca ; branch_target=0x080253ca
080253c2  b3f1805f  cmp.w	r3, #268435456
080253c6  00f0ff80  beq.w	#510 ; -> 0x080255c8 ; branch_target=0x080255c8
080253ca  804a      ldr	r2, [pc, #512] ; [0x080255cc] = 0x58024400
080253cc  1268      ldr	r2, [r2]
080253ce  9103      lsls	r1, r2, #14
080253d0  02d5      bpl	#4 ; -> 0x080253d8 ; branch_target=0x080253d8
080253d2  b3f1005f  cmp.w	r3, #536870912
080253d6  d8d0      beq	#-80 ; -> 0x0802538a ; branch_target=0x0802538a
080253d8  0020      movs	r0, #0
080253da  05b0      add	sp, #20
080253dc  5df804fb  ldr	pc, [sp], #4
080253e0  002b      cmp	r3, #0
080253e2  4cd0      beq	#152 ; -> 0x0802547e ; branch_target=0x0802547e
080253e4  402b      cmp	r3, #64
080253e6  f7d1      bne	#-18 ; -> 0x080253d8 ; branch_target=0x080253d8
080253e8  1068      ldr	r0, [r2]
080253ea  10f00060  ands	r0, r0, #134217728
080253ee  10d0      beq	#32 ; -> 0x08025412 ; branch_target=0x08025412
080253f0  01a8      add	r0, sp, #4
080253f2  fff7c9fd  bl	#-1134 ; -> 0x08024f88 ; branch_target=0x08024f88
080253f6  0198      ldr	r0, [sp, #4]
080253f8  0be0      b	#22 ; -> 0x08025412 ; branch_target=0x08025412
080253fa  744a      ldr	r2, [pc, #464] ; [0x080255cc] = 0x58024400
080253fc  136d      ldr	r3, [r2, #80]
080253fe  03f4e043  and	r3, r3, #28672
08025402  b3f5005f  cmp.w	r3, #8192
08025406  61d0      beq	#194 ; -> 0x080254cc ; branch_target=0x080254cc
08025408  37d9      bls	#110 ; -> 0x0802547a ; branch_target=0x0802547a
0802540a  b3f5405f  cmp.w	r3, #12288
0802540e  79d1      bne	#242 ; -> 0x08025504 ; branch_target=0x08025504
08025410  7048      ldr	r0, [pc, #448] ; [0x080255d4] = 0x00bb8000
08025412  05b0      add	sp, #20
08025414  5df804fb  ldr	pc, [sp], #4
08025418  6c4b      ldr	r3, [pc, #432] ; [0x080255cc] = 0x58024400
0802541a  1b6d      ldr	r3, [r3, #80]
0802541c  03f00703  and	r3, r3, #7
08025420  042b      cmp	r3, #4
08025422  d9d8      bhi	#-78 ; -> 0x080253d8 ; branch_target=0x080253d8
08025424  01a2      adr	r2, #4
08025426  52f823f0  ldr.w	pc, [r2, r3, lsl #2]
0802542a  00bf      nop
0802542c  9154      strb	r1, [r2, r2]
0802542e  0208      lsrs	r2, r0, #32
08025430  b554      strb	r5, [r6, r2]
08025432  0208      lsrs	r2, r0, #32
08025434  a154      strb	r1, [r4, r2]
08025436  0208      lsrs	r2, r0, #32
08025438  1154      strb	r1, [r2, r0]
0802543a  0208      lsrs	r2, r0, #32
0802543c  9d54      strb	r5, [r3, r2]
0802543e  0208      lsrs	r2, r0, #32
08025440  624a      ldr	r2, [pc, #392] ; [0x080255cc] = 0x58024400
08025442  936d      ldr	r3, [r2, #88]
08025444  03f0e063  and	r3, r3, #117440512
08025448  b3f1007f  cmp.w	r3, #33554432
0802544c  3ed0      beq	#124 ; -> 0x080254cc ; branch_target=0x080254cc
0802544e  37d9      bls	#110 ; -> 0x080254c0 ; branch_target=0x080254c0
08025450  b3f1407f  cmp.w	r3, #50331648
08025454  dcd0      beq	#-72 ; -> 0x08025410 ; branch_target=0x08025410
08025456  b3f1806f  cmp.w	r3, #67108864
0802545a  bdd1      bne	#-134 ; -> 0x080253d8 ; branch_target=0x080253d8
0802545c  d36c      ldr	r3, [r2, #76]
0802545e  1268      ldr	r2, [r2]
08025460  03f04053  and	r3, r3, #805306368
08025464  5207      lsls	r2, r2, #29
08025466  a8d5      bpl	#-176 ; -> 0x080253ba ; branch_target=0x080253ba
08025468  002b      cmp	r3, #0
0802546a  a6d1      bne	#-180 ; -> 0x080253ba ; branch_target=0x080253ba
0802546c  574b      ldr	r3, [pc, #348] ; [0x080255cc] = 0x58024400
0802546e  5a48      ldr	r0, [pc, #360] ; [0x080255d8] = 0x03d09000
08025470  1b68      ldr	r3, [r3]
08025472  c3f3c103  ubfx	r3, r3, #3, #2
08025476  d840      lsrs	r0, r3
08025478  cbe7      b	#-106 ; -> 0x08025412 ; branch_target=0x08025412
0802547a  002b      cmp	r3, #0
0802547c  46d1      bne	#140 ; -> 0x0802550c ; branch_target=0x0802550c
0802547e  1068      ldr	r0, [r2]
08025480  10f00070  ands	r0, r0, #33554432
08025484  c5d0      beq	#-118 ; -> 0x08025412 ; branch_target=0x08025412
08025486  01a8      add	r0, sp, #4
08025488  fff7b2fe  bl	#-668 ; -> 0x080251f0 ; branch_target=0x080251f0
0802548c  0298      ldr	r0, [sp, #8]
0802548e  c0e7      b	#-128 ; -> 0x08025412 ; branch_target=0x08025412
08025490  4e4b      ldr	r3, [pc, #312] ; [0x080255cc] = 0x58024400
08025492  1868      ldr	r0, [r3]
08025494  10f00070  ands	r0, r0, #33554432
08025498  bbd0      beq	#-138 ; -> 0x08025412 ; branch_target=0x08025412
0802549a  f4e7      b	#-24 ; -> 0x08025486 ; branch_target=0x08025486
080254aa  01a8      add	r0, sp, #4
080254ac  fff706fe  bl	#-1012 ; -> 0x080250bc ; branch_target=0x080250bc
080254b0  0198      ldr	r0, [sp, #4]
080254b2  aee7      b	#-164 ; -> 0x08025412 ; branch_target=0x08025412
080254c0  002b      cmp	r3, #0
080254c2  dcd0      beq	#-72 ; -> 0x0802547e ; branch_target=0x0802547e
080254c4  b3f1807f  cmp.w	r3, #16777216
080254c8  8ed0      beq	#-228 ; -> 0x080253e8 ; branch_target=0x080253e8
080254ca  85e7      b	#-246 ; -> 0x080253d8 ; branch_target=0x080253d8
080254cc  1068      ldr	r0, [r2]
080254ce  10f00050  ands	r0, r0, #536870912
080254d2  9ed0      beq	#-196 ; -> 0x08025412 ; branch_target=0x08025412
080254d4  e9e7      b	#-46 ; -> 0x080254aa ; branch_target=0x080254aa
080254d6  3d4a      ldr	r2, [pc, #244] ; [0x080255cc] = 0x58024400
080254d8  936d      ldr	r3, [r2, #88]
080254da  03f46003  and	r3, r3, #14680064
080254de  b3f5800f  cmp.w	r3, #4194304
080254e2  f3d0      beq	#-26 ; -> 0x080254cc ; branch_target=0x080254cc
080254e4  06d8      bhi	#12 ; -> 0x080254f4 ; branch_target=0x080254f4
080254e6  002b      cmp	r3, #0
080254e8  c9d0      beq	#-110 ; -> 0x0802547e ; branch_target=0x0802547e
080254ea  b3f5001f  cmp.w	r3, #2097152
080254ee  3ff47baf  beq.w	#-266 ; -> 0x080253e8 ; branch_target=0x080253e8
080254f2  71e7      b	#-286 ; -> 0x080253d8 ; branch_target=0x080253d8
080254f4  b3f5c00f  cmp.w	r3, #6291456
080254f8  8ad0      beq	#-236 ; -> 0x08025410 ; branch_target=0x08025410
080254fa  b3f5000f  cmp.w	r3, #8388608
080254fe  3ff453af  beq.w	#-346 ; -> 0x080253a8 ; branch_target=0x080253a8
08025502  69e7      b	#-302 ; -> 0x080253d8 ; branch_target=0x080253d8
08025504  b3f5804f  cmp.w	r3, #16384
08025508  a8d0      beq	#-176 ; -> 0x0802545c ; branch_target=0x0802545c
0802550a  65e7      b	#-310 ; -> 0x080253d8 ; branch_target=0x080253d8
0802550c  b3f5805f  cmp.w	r3, #4096
08025510  3ff46aaf  beq.w	#-300 ; -> 0x080253e8 ; branch_target=0x080253e8
08025514  60e7      b	#-320 ; -> 0x080253d8 ; branch_target=0x080253d8
08025516  1368      ldr	r3, [r2]
08025518  2f48      ldr	r0, [pc, #188] ; [0x080255d8] = 0x03d09000
0802551a  c3f3c103  ubfx	r3, r3, #3, #2
0802551e  d840      lsrs	r0, r3
08025520  77e7      b	#-274 ; -> 0x08025412 ; branch_target=0x08025412
08025522  2a4a      ldr	r2, [pc, #168] ; [0x080255cc] = 0x58024400
08025524  936d      ldr	r3, [r2, #88]
08025526  03f0e043  and	r3, r3, #1879048192
0802552a  b3f1405f  cmp.w	r3, #805306368
0802552e  6ed0      beq	#220 ; -> 0x0802560e ; branch_target=0x0802560e
08025530  3cd8      bhi	#120 ; -> 0x080255ac ; branch_target=0x080255ac
08025532  b3f1805f  cmp.w	r3, #268435456
08025536  5fd0      beq	#190 ; -> 0x080255f8 ; branch_target=0x080255f8
08025538  b3f1005f  cmp.w	r3, #536870912
0802553c  27d1      bne	#78 ; -> 0x0802558e ; branch_target=0x0802558e
0802553e  234b      ldr	r3, [pc, #140] ; [0x080255cc] = 0x58024400
08025540  1868      ldr	r0, [r3]
08025542  10f00050  ands	r0, r0, #536870912
08025546  3ff464af  beq.w	#-312 ; -> 0x08025412 ; branch_target=0x08025412
0802554a  01a8      add	r0, sp, #4
0802554c  fff7b6fd  bl	#-1172 ; -> 0x080250bc ; branch_target=0x080250bc
08025550  0298      ldr	r0, [sp, #8]
08025552  5ee7      b	#-324 ; -> 0x08025412 ; branch_target=0x08025412
08025554  1d4a      ldr	r2, [pc, #116] ; [0x080255cc] = 0x58024400
08025556  936d      ldr	r3, [r2, #88]
08025558  03f44033  and	r3, r3, #196608
0802555c  b3f5803f  cmp.w	r3, #65536
08025560  40d0      beq	#128 ; -> 0x080255e4 ; branch_target=0x080255e4
08025562  b3f5003f  cmp.w	r3, #131072
08025566  3ff479af  beq.w	#-270 ; -> 0x0802545c ; branch_target=0x0802545c
0802556a  002b      cmp	r3, #0
0802556c  7ff434af  bne.w	#-408 ; -> 0x080253d8 ; branch_target=0x080253d8
08025570  3ae7      b	#-396 ; -> 0x080253e8 ; branch_target=0x080253e8
08025572  164b      ldr	r3, [pc, #88] ; [0x080255cc] = 0x58024400
08025574  da6c      ldr	r2, [r3, #76]
08025576  d203      lsls	r2, r2, #15
08025578  8bd5      bpl	#-234 ; -> 0x08025492 ; branch_target=0x08025492
0802557a  1868      ldr	r0, [r3]
0802557c  10f00060  ands	r0, r0, #134217728
08025580  3ff447af  beq.w	#-370 ; -> 0x08025412 ; branch_target=0x08025412
08025584  01a8      add	r0, sp, #4
08025586  fff7fffc  bl	#-1538 ; -> 0x08024f88 ; branch_target=0x08024f88
0802558a  0398      ldr	r0, [sp, #12]
0802558c  41e7      b	#-382 ; -> 0x08025412 ; branch_target=0x08025412
0802558e  002b      cmp	r3, #0
08025590  7ff422af  bne.w	#-444 ; -> 0x080253d8 ; branch_target=0x080253d8
08025594  fef744fd  bl	#-5496 ; -> 0x08024020 ; branch_target=0x08024020
08025598  0c4b      ldr	r3, [pc, #48] ; [0x080255cc] = 0x58024400
0802559a  104a      ldr	r2, [pc, #64] ; [0x080255dc] = 0x08049aac
0802559c  1b6a      ldr	r3, [r3, #32]
0802559e  c3f30213  ubfx	r3, r3, #4, #3
080255a2  d35c      ldrb	r3, [r2, r3]
080255a4  03f01f03  and	r3, r3, #31
080255a8  d840      lsrs	r0, r3
080255aa  32e7      b	#-412 ; -> 0x08025412 ; branch_target=0x08025412
080255ac  b3f1804f  cmp.w	r3, #1073741824
080255b0  04d0      beq	#8 ; -> 0x080255bc ; branch_target=0x080255bc
080255b2  b3f1a04f  cmp.w	r3, #1342177280
080255b6  3ff4e3ae  beq.w	#-570 ; -> 0x08025380 ; branch_target=0x08025380
080255ba  0de7      b	#-486 ; -> 0x080253d8 ; branch_target=0x080253d8
080255bc  034b      ldr	r3, [pc, #12] ; [0x080255cc] = 0x58024400
080255be  1868      ldr	r0, [r3]
080255c0  10f48070  ands	r0, r0, #256
080255c4  3ff425af  beq.w	#-438 ; -> 0x08025412 ; branch_target=0x08025412
080255c8  0548      ldr	r0, [pc, #20] ; [0x080255e0] = 0x003d0900
080255ca  22e7      b	#-444 ; -> 0x08025412 ; branch_target=0x08025412
080255e4  1068      ldr	r0, [r2]
080255e6  10f00050  ands	r0, r0, #536870912
080255ea  3ff412af  beq.w	#-476 ; -> 0x08025412 ; branch_target=0x08025412
080255ee  01a8      add	r0, sp, #4
080255f0  fff764fd  bl	#-1336 ; -> 0x080250bc ; branch_target=0x080250bc
080255f4  0398      ldr	r0, [sp, #12]
080255f6  0ce7      b	#-488 ; -> 0x08025412 ; branch_target=0x08025412
080255f8  084b      ldr	r3, [pc, #32] ; [0x0802561c] = 0x58024400
080255fa  1868      ldr	r0, [r3]
080255fc  10f00060  ands	r0, r0, #134217728
08025600  3ff407af  beq.w	#-498 ; -> 0x08025412 ; branch_target=0x08025412
08025604  01a8      add	r0, sp, #4
08025606  fff7bffc  bl	#-1666 ; -> 0x08024f88 ; branch_target=0x08024f88
0802560a  0298      ldr	r0, [sp, #8]
0802560c  01e7      b	#-510 ; -> 0x08025412 ; branch_target=0x08025412
0802560e  1068      ldr	r0, [r2]
08025610  10f00400  ands	r0, r0, #4
08025614  3ff4fdae  beq.w	#-518 ; -> 0x08025412 ; branch_target=0x08025412
08025618  28e7      b	#-432 ; -> 0x0802546c ; branch_target=0x0802546c
