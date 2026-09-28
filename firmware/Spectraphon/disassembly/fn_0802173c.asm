; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
0802173c  0268      ldr	r2, [r0]
0802173e  274b      ldr	r3, [pc, #156] ; [0x080217dc] = 0x58025408
08021740  2749      ldr	r1, [pc, #156] ; [0x080217e0] = 0x58025430
08021742  30b4      push	{r4, r5}
08021744  274d      ldr	r5, [pc, #156] ; [0x080217e4] = 0x5802541c
08021746  284c      ldr	r4, [pc, #160] ; [0x080217e8] = 0x58025444
08021748  aa42      cmp	r2, r5
0802174a  18bf      it	ne
0802174c  9a42      cmpne	r2, r3
0802174e  0cbf      ite	eq
08021750  0123      moveq	r3, #1
08021752  0023      movne	r3, #0
08021754  8a42      cmp	r2, r1
08021756  08bf      it	eq
08021758  43f00103  orreq	r3, r3, #1
0802175c  2831      adds	r1, #40
0802175e  a242      cmp	r2, r4
08021760  08bf      it	eq
08021762  43f00103  orreq	r3, r3, #1
08021766  2834      adds	r4, #40
08021768  8a42      cmp	r2, r1
0802176a  08bf      it	eq
0802176c  43f00103  orreq	r3, r3, #1
08021770  2831      adds	r1, #40
08021772  a242      cmp	r2, r4
08021774  08bf      it	eq
08021776  43f00103  orreq	r3, r3, #1
0802177a  8a42      cmp	r2, r1
0802177c  08bf      it	eq
0802177e  43f00103  orreq	r3, r3, #1
08021782  d1b2      uxtb	r1, r2
08021784  13b9      cbnz	r3, #4 ; -> 0x0802178c ; branch_target=0x0802178c
08021786  194b      ldr	r3, [pc, #100] ; [0x080217ec] = 0x58025494
08021788  9a42      cmp	r2, r3
0802178a  12d1      bne	#36 ; -> 0x080217b2 ; branch_target=0x080217b2
0802178c  a1f10803  sub.w	r3, r1, #8
08021790  1749      ldr	r1, [pc, #92] ; [0x080217f0] = 0xcccccccd / f32_bits_interpretation=-107374184
08021792  184a      ldr	r2, [pc, #96] ; [0x080217f4] = 0x16009600
08021794  a1fb0313  umull	r1, r3, r1, r3
08021798  0121      movs	r1, #1
0802179a  174c      ldr	r4, [pc, #92] ; [0x080217f8] = 0x58025880
0802179c  02eb1312  add.w	r2, r2, r3, lsr #4
080217a0  c3f30413  ubfx	r3, r3, #4, #5
080217a4  9200      lsls	r2, r2, #2
080217a6  9940      lsls	r1, r3
080217a8  8166      str	r1, [r0, #104]
080217aa  c0e91824  strd	r2, r4, [r0, #96]
080217ae  30bc      pop	{r4, r5}
080217b0  7047      bx	lr
080217b2  a1f11003  sub.w	r3, r1, #16
080217b6  1149      ldr	r1, [pc, #68] ; [0x080217fc] = 0xbffdfbf0 / f32_bits_interpretation=-1.984251022
080217b8  114c      ldr	r4, [pc, #68] ; [0x08021800] = 0xaaaaaaab
080217ba  1144      add	r1, r2
080217bc  a4fb0343  umull	r4, r3, r4, r3
080217c0  a829      cmp	r1, #168
080217c2  4fea1313  lsr.w	r3, r3, #4
080217c6  00d8      bhi	#0 ; -> 0x080217ca ; branch_target=0x080217ca
080217c8  0833      adds	r3, #8
080217ca  0e4a      ldr	r2, [pc, #56] ; [0x08021804] = 0x10008200
080217cc  03f01f04  and	r4, r3, #31
080217d0  0121      movs	r1, #1
080217d2  1a44      add	r2, r3
080217d4  a140      lsls	r1, r4
080217d6  0c4c      ldr	r4, [pc, #48] ; [0x08021808] = 0x40020880 / f32_bits_interpretation=2.031768799
080217d8  9200      lsls	r2, r2, #2
080217da  e5e7      b	#-54 ; -> 0x080217a8 ; branch_target=0x080217a8
