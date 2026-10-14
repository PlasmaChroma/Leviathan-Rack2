; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
0802c4e0  f8b5      push	{r3, r4, r5, r6, r7, lr}
0802c4e2  9f4c      ldr	r4, [pc, #636] ; [0x0802c760] = 0xe000ed00
0802c4e4  0023      movs	r3, #0
0802c4e6  9f4a      ldr	r2, [pc, #636] ; [0x0802c764] = 0xe0001000
0802c4e8  d4f8fc10  ldr.w	r1, [r4, #252]
0802c4ec  9e4d      ldr	r5, [pc, #632] ; [0x0802c768] = 0xc5acce55 / f32_bits_interpretation=-5529.791504
0802c4ee  41f08071  orr	r1, r1, #16777216
0802c4f2  dfed9e6a  vldr	s13, [pc, #632] ; [0x0802c76c] = 0x3c800000 / f32_bits_interpretation=0.015625
0802c4f6  c4f8fc10  str.w	r1, [r4, #252]
0802c4fa  0124      movs	r4, #1
0802c4fc  c2f8b05f  str.w	r5, [r2, #4016]
0802c500  0546      mov	r5, r0
0802c502  5360      str	r3, [r2, #4]
0802c504  1168      ldr	r1, [r2]
0802c506  9a48      ldr	r0, [pc, #616] ; [0x0802c770] = 0x20002eb4
0802c508  2143      orrs	r1, r4
0802c50a  1160      str	r1, [r2]
0802c50c  4ff07e51  mov.w	r1, #1065353216
0802c510  984a      ldr	r2, [pc, #608] ; [0x0802c774] = 0x2000239c
0802c512  0360      str	r3, [r0]
0802c514  1162      str	r1, [r2, #32]
0802c516  9160      str	r1, [r2, #8]
0802c518  0022      movs	r2, #0
0802c51a  9749      ldr	r1, [pc, #604] ; [0x0802c778] = 0x20001160
0802c51c  0a60      str	r2, [r1]
0802c51e  9749      ldr	r1, [pc, #604] ; [0x0802c77c] = 0x20002e6c
0802c520  0a60      str	r2, [r1]
0802c522  9749      ldr	r1, [pc, #604] ; [0x0802c780] = 0x20002e70
0802c524  0a60      str	r2, [r1]
0802c526  9749      ldr	r1, [pc, #604] ; [0x0802c784] = 0x20002e74
0802c528  0a60      str	r2, [r1]
0802c52a  9749      ldr	r1, [pc, #604] ; [0x0802c788] = 0x20002f08
0802c52c  974a      ldr	r2, [pc, #604] ; [0x0802c78c] = 0x20002428
0802c52e  0b60      str	r3, [r1]
0802c530  9749      ldr	r1, [pc, #604] ; [0x0802c790] = 0x2000242c
0802c532  1360      str	r3, [r2]
0802c534  0b60      str	r3, [r1]
0802c536  2346      mov	r3, r4
0802c538  9649      ldr	r1, [pc, #600] ; [0x0802c794] = 0x20002ec8
0802c53a  974a      ldr	r2, [pc, #604] ; [0x0802c798] = 0x60000004
0802c53c  0c60      str	r4, [r1]
0802c53e  07ee903a  vmov	s15, r3
0802c542  0133      adds	r3, #1
0802c544  b8eee77a  vcvt.f32.s32	s14, s15
0802c548  b3f5806f  cmp.w	r3, #1024
0802c54c  c6ee877a  vdiv.f32	s15, s13, s14
0802c550  e2ec017a  vstmia	r2!, {s15}
0802c554  f3d1      bne	#-26 ; -> 0x0802c53e ; branch_target=0x0802c53e
0802c556  914a      ldr	r2, [pc, #580] ; [0x0802c79c] = 0x2001348c
0802c558  9369      ldr	r3, [r2, #24]
0802c55a  03eb4303  add.w	r3, r3, r3, lsl #1
0802c55e  43f39903  sbfx	r3, r3, #2, #26
0802c562  b3f5806f  cmp.w	r3, #1024
0802c566  c0f25d82  blt.w	#1210 ; -> 0x0802ca24 ; branch_target=0x0802ca24
0802c56a  07ee903a  vmov	s15, r3
0802c56e  f8eee77a  vcvt.f32.s32	s15, s15
0802c572  86eea77a  vdiv.f32	s14, s13, s15
0802c576  d369      ldr	r3, [r2, #28]
0802c578  8949      ldr	r1, [pc, #548] ; [0x0802c7a0] = 0x200023f8
0802c57a  03eb4303  add.w	r3, r3, r3, lsl #1
0802c57e  81ed007a  vstr	s14, [r1]
0802c582  43f39903  sbfx	r3, r3, #2, #26
0802c586  b3f5806f  cmp.w	r3, #1024
0802c58a  c0f24282  blt.w	#1156 ; -> 0x0802ca12 ; branch_target=0x0802ca12
0802c58e  07ee903a  vmov	s15, r3
0802c592  dfed766a  vldr	s13, [pc, #472] ; [0x0802c76c] = 0x3c800000 / f32_bits_interpretation=0.015625
0802c596  f8eee77a  vcvt.f32.s32	s15, s15
0802c59a  86eea77a  vdiv.f32	s14, s13, s15
0802c59e  8148      ldr	r0, [pc, #516] ; [0x0802c7a4] = 0x200023f4
0802c5a0  0023      movs	r3, #0
0802c5a2  966a      ldr	r6, [r2, #40]
0802c5a4  80ed007a  vstr	s14, [r0]
0802c5a8  7f48      ldr	r0, [pc, #508] ; [0x0802c7a8] = 0x200023fc
0802c5aa  d46a      ldr	r4, [r2, #44]
0802c5ac  0360      str	r3, [r0]
0802c5ae  7f48      ldr	r0, [pc, #508] ; [0x0802c7ac] = 0x20002e54
0802c5b0  7f49      ldr	r1, [pc, #508] ; [0x0802c7b0] = 0x200022bc
0802c5b2  0360      str	r3, [r0]
0802c5b4  7f48      ldr	r0, [pc, #508] ; [0x0802c7b4] = 0x20002e58
0802c5b6  0b60      str	r3, [r1]
0802c5b8  0360      str	r3, [r0]
0802c5ba  7f48      ldr	r0, [pc, #508] ; [0x0802c7b8] = 0x20002430
0802c5bc  0b67      str	r3, [r1, #112]
0802c5be  0660      str	r6, [r0]
0802c5c0  7e48      ldr	r0, [pc, #504] ; [0x0802c7bc] = 0x20000b20
0802c5c2  0b66      str	r3, [r1, #96]
0802c5c4  0460      str	r4, [r0]
0802c5c6  0124      movs	r4, #1
0802c5c8  7d48      ldr	r0, [pc, #500] ; [0x0802c7c0] = 0x200011e0
0802c5ca  0b65      str	r3, [r1, #80]
0802c5cc  0360      str	r3, [r0]
0802c5ce  7d48      ldr	r0, [pc, #500] ; [0x0802c7c4] = 0x20002e9c
0802c5d0  0b64      str	r3, [r1, #64]
0802c5d2  0360      str	r3, [r0]
0802c5d4  7c48      ldr	r0, [pc, #496] ; [0x0802c7c8] = 0x20002ea0
0802c5d6  0b63      str	r3, [r1, #48]
0802c5d8  0360      str	r3, [r0]
0802c5da  7c48      ldr	r0, [pc, #496] ; [0x0802c7cc] = 0x20002ea4
0802c5dc  0b62      str	r3, [r1, #32]
0802c5de  0360      str	r3, [r0]
0802c5e0  0b61      str	r3, [r1, #16]
0802c5e2  7b48      ldr	r0, [pc, #492] ; [0x0802c7d0] = 0x20002ea8
0802c5e4  7b49      ldr	r1, [pc, #492] ; [0x0802c7d4] = 0x20000880
0802c5e6  0360      str	r3, [r0]
0802c5e8  0b60      str	r3, [r1]
0802c5ea  0021      movs	r1, #0
0802c5ec  7a48      ldr	r0, [pc, #488] ; [0x0802c7d8] = 0x20002e84
0802c5ee  7b4e      ldr	r6, [pc, #492] ; [0x0802c7dc] = 0x20002ef8
0802c5f0  0160      str	r1, [r0]
0802c5f2  7b48      ldr	r0, [pc, #492] ; [0x0802c7e0] = 0x20001140
0802c5f4  0360      str	r3, [r0]
0802c5f6  7b48      ldr	r0, [pc, #492] ; [0x0802c7e4] = 0x20002e50
0802c5f8  0360      str	r3, [r0]
0802c5fa  7b48      ldr	r0, [pc, #492] ; [0x0802c7e8] = 0x20002e60
0802c5fc  0360      str	r3, [r0]
0802c5fe  7b48      ldr	r0, [pc, #492] ; [0x0802c7ec] = 0x20002e5c
0802c600  0360      str	r3, [r0]
0802c602  7b48      ldr	r0, [pc, #492] ; [0x0802c7f0] = 0x20002e68
0802c604  0360      str	r3, [r0]
0802c606  7b48      ldr	r0, [pc, #492] ; [0x0802c7f4] = 0x20002e64
0802c608  0360      str	r3, [r0]
0802c60a  7b48      ldr	r0, [pc, #492] ; [0x0802c7f8] = 0x20001180
0802c60c  0360      str	r3, [r0]
0802c60e  7b48      ldr	r0, [pc, #492] ; [0x0802c7fc] = 0x20002e78
0802c610  0360      str	r3, [r0]
0802c612  7b48      ldr	r0, [pc, #492] ; [0x0802c800] = 0x20000840
0802c614  0360      str	r3, [r0]
0802c616  7b48      ldr	r0, [pc, #492] ; [0x0802c804] = 0x200023d4
0802c618  0360      str	r3, [r0]
0802c61a  9768      ldr	r7, [r2, #8]
0802c61c  7a48      ldr	r0, [pc, #488] ; [0x0802c808] = 0x200023d8
0802c61e  3760      str	r7, [r6]
0802c620  d768      ldr	r7, [r2, #12]
0802c622  7a4e      ldr	r6, [pc, #488] ; [0x0802c80c] = 0x20002ef4
0802c624  0360      str	r3, [r0]
0802c626  3760      str	r7, [r6]
0802c628  1769      ldr	r7, [r2, #16]
0802c62a  794e      ldr	r6, [pc, #484] ; [0x0802c810] = 0x20002e8c
0802c62c  7948      ldr	r0, [pc, #484] ; [0x0802c814] = 0x200023dc
0802c62e  3760      str	r7, [r6]
0802c630  5769      ldr	r7, [r2, #20]
0802c632  794e      ldr	r6, [pc, #484] ; [0x0802c818] = 0x20002e88
0802c634  0360      str	r3, [r0]
0802c636  3760      str	r7, [r6]
0802c638  176a      ldr	r7, [r2, #32]
0802c63a  784e      ldr	r6, [pc, #480] ; [0x0802c81c] = 0x200023d0
0802c63c  7848      ldr	r0, [pc, #480] ; [0x0802c820] = 0x20002f50
0802c63e  3760      str	r7, [r6]
0802c640  784e      ldr	r6, [pc, #480] ; [0x0802c824] = 0x20000820
0802c642  576a      ldr	r7, [r2, #36]
0802c644  3760      str	r7, [r6]
0802c646  784e      ldr	r6, [pc, #480] ; [0x0802c828] = 0x20002e80
0802c648  3160      str	r1, [r6]
0802c64a  784e      ldr	r6, [pc, #480] ; [0x0802c82c] = 0x200011c0
0802c64c  3360      str	r3, [r6]
0802c64e  784e      ldr	r6, [pc, #480] ; [0x0802c830] = 0x20002e98
0802c650  3360      str	r3, [r6]
0802c652  784b      ldr	r3, [pc, #480] ; [0x0802c834] = 0x20002e90
0802c654  1c60      str	r4, [r3]
0802c656  784b      ldr	r3, [pc, #480] ; [0x0802c838] = 0x20002e94
0802c658  1c60      str	r4, [r3]
0802c65a  1388      ldrh	r3, [r2]
0802c65c  5488      ldrh	r4, [r2, #2]
0802c65e  8370      strb	r3, [r0, #2]
0802c660  136b      ldr	r3, [r2, #48]
0802c662  c470      strb	r4, [r0, #3]
0802c664  0371      strb	r3, [r0, #4]
0802c666  754b      ldr	r3, [pc, #468] ; [0x0802c83c] = 0x20002434
0802c668  754a      ldr	r2, [pc, #468] ; [0x0802c840] = 0x20002438
0802c66a  1960      str	r1, [r3]
0802c66c  754c      ldr	r4, [pc, #468] ; [0x0802c844] = 0x2000243c
0802c66e  764b      ldr	r3, [pc, #472] ; [0x0802c848] = 0x080e0000
0802c670  2160      str	r1, [r4]
0802c672  1160      str	r1, [r2]
0802c674  1a68      ldr	r2, [r3]
0802c676  40f2d543  movw	r3, #1237
0802c67a  7449      ldr	r1, [pc, #464] ; [0x0802c84c] = 0x200144d4
0802c67c  9a42      cmp	r2, r3
0802c67e  0e68      ldr	r6, [r1]
0802c680  00f02482  beq.w	#1096 ; -> 0x0802cacc ; branch_target=0x0802cacc
0802c684  724a      ldr	r2, [pc, #456] ; [0x0802c850] = 0x3e808312 / f32_bits_interpretation=0.2509999871
0802c686  032e      cmp	r6, #3
0802c688  7249      ldr	r1, [pc, #456] ; [0x0802c854] = 0x20001280
0802c68a  734c      ldr	r4, [pc, #460] ; [0x0802c858] = 0x200012c0
0802c68c  ca61      str	r2, [r1, #28]
0802c68e  8a61      str	r2, [r1, #24]
0802c690  4a61      str	r2, [r1, #20]
0802c692  0a61      str	r2, [r1, #16]
0802c694  ca60      str	r2, [r1, #12]
0802c696  8a60      str	r2, [r1, #8]
0802c698  4a60      str	r2, [r1, #4]
0802c69a  0a60      str	r2, [r1]
0802c69c  6f49      ldr	r1, [pc, #444] ; [0x0802c85c] = 0x20001240
0802c69e  704b      ldr	r3, [pc, #448] ; [0x0802c860] = 0x20001200
0802c6a0  ca61      str	r2, [r1, #28]
0802c6a2  8a61      str	r2, [r1, #24]
0802c6a4  4a61      str	r2, [r1, #20]
0802c6a6  0a61      str	r2, [r1, #16]
0802c6a8  ca60      str	r2, [r1, #12]
0802c6aa  8a60      str	r2, [r1, #8]
0802c6ac  4a60      str	r2, [r1, #4]
0802c6ae  0a60      str	r2, [r1]
0802c6b0  6c4a      ldr	r2, [pc, #432] ; [0x0802c864] = 0x20001300
0802c6b2  6d48      ldr	r0, [pc, #436] ; [0x0802c868] = 0x37840803 / f32_bits_interpretation=1.573935697e-05
0802c6b4  6d49      ldr	r1, [pc, #436] ; [0x0802c86c] = 0x20001380
0802c6b6  1860      str	r0, [r3]
0802c6b8  5860      str	r0, [r3, #4]
0802c6ba  9860      str	r0, [r3, #8]
0802c6bc  d860      str	r0, [r3, #12]
0802c6be  1861      str	r0, [r3, #16]
0802c6c0  5861      str	r0, [r3, #20]
0802c6c2  9861      str	r0, [r3, #24]
0802c6c4  d861      str	r0, [r3, #28]
0802c6c6  9fed1c6b  vldr	d6, [pc, #112] ; [0x0802c738] = 0x00003f48 / f64_bits_interpretation=5.1522057812640675e-310
0802c6ca  9fed1d7b  vldr	d7, [pc, #116] ; [0x0802c740] = 0x00000028 / f64_bits_interpretation=1.7230605822657774e-310
0802c6ce  84ed026b  vstr	d6, [r4, #8]
0802c6d2  82ed026b  vstr	d6, [r2, #8]
0802c6d6  9fed1c6b  vldr	d6, [pc, #112] ; [0x0802c748] = 0x00007e68 / f64_bits_interpretation=8.5813509802623577e-310
0802c6da  84ed007b  vstr	d7, [r4]
0802c6de  82ed007b  vstr	d7, [r2]
0802c6e2  84ed046b  vstr	d6, [r4, #16]
0802c6e6  9fed1a7b  vldr	d7, [pc, #104] ; [0x0802c750] = 0x000007d0 / f64_bits_interpretation=4.2439915829186759e-311
0802c6ea  82ed046b  vstr	d6, [r2, #16]
0802c6ee  9fed1a6b  vldr	d6, [pc, #104] ; [0x0802c758] = 0x0000bd88 / f64_bits_interpretation=1.2010496179260648e-309
0802c6f2  84ed066b  vstr	d6, [r4, #24]
0802c6f6  82ed066b  vstr	d6, [r2, #24]
0802c6fa  1862      str	r0, [r3, #32]
0802c6fc  5862      str	r0, [r3, #36]
0802c6fe  9862      str	r0, [r3, #40]
0802c700  d862      str	r0, [r3, #44]
0802c702  81ed007b  vstr	d7, [r1]
0802c706  81ed027b  vstr	d7, [r1, #8]
0802c70a  81ed047b  vstr	d7, [r1, #16]
0802c70e  81ed067b  vstr	d7, [r1, #24]
0802c712  81ed087b  vstr	d7, [r1, #32]
0802c716  81ed0a7b  vstr	d7, [r1, #40]
0802c71a  40f0d280  bne.w	#420 ; -> 0x0802c8c2 ; branch_target=0x0802c8c2
0802c71e  4a4a      ldr	r2, [pc, #296] ; [0x0802c848] = 0x080e0000
0802c720  106a      ldr	r0, [r2, #32]
0802c722  0860      str	r0, [r1]
0802c724  506a      ldr	r0, [r2, #36]
0802c726  4860      str	r0, [r1, #4]
0802c728  906a      ldr	r0, [r2, #40]
0802c72a  8860      str	r0, [r1, #8]
0802c72c  d06a      ldr	r0, [r2, #44]
0802c72e  c860      str	r0, [r1, #12]
0802c730  106b      ldr	r0, [r2, #48]
0802c732  0861      str	r0, [r1, #16]
0802c734  506b      ldr	r0, [r2, #52]
0802c736  9be0      b	#310 ; -> 0x0802c870 ; branch_target=0x0802c870
0802c870  4861      str	r0, [r1, #20]
0802c872  906b      ldr	r0, [r2, #56]
0802c874  8861      str	r0, [r1, #24]
0802c876  d06b      ldr	r0, [r2, #60]
0802c878  c861      str	r0, [r1, #28]
0802c87a  106c      ldr	r0, [r2, #64]
0802c87c  0862      str	r0, [r1, #32]
0802c87e  506c      ldr	r0, [r2, #68]
0802c880  4862      str	r0, [r1, #36]
0802c882  906c      ldr	r0, [r2, #72]
0802c884  8862      str	r0, [r1, #40]
0802c886  d06c      ldr	r0, [r2, #76]
0802c888  c862      str	r0, [r1, #44]
0802c88a  116e      ldr	r1, [r2, #96]
0802c88c  1960      str	r1, [r3]
0802c88e  916e      ldr	r1, [r2, #104]
0802c890  506e      ldr	r0, [r2, #100]
0802c892  9960      str	r1, [r3, #8]
0802c894  5860      str	r0, [r3, #4]
0802c896  d16e      ldr	r1, [r2, #108]
0802c898  d960      str	r1, [r3, #12]
0802c89a  106f      ldr	r0, [r2, #112]
0802c89c  516f      ldr	r1, [r2, #116]
0802c89e  1861      str	r0, [r3, #16]
0802c8a0  5961      str	r1, [r3, #20]
0802c8a2  906f      ldr	r0, [r2, #120]
0802c8a4  d16f      ldr	r1, [r2, #124]
0802c8a6  9861      str	r0, [r3, #24]
0802c8a8  d961      str	r1, [r3, #28]
0802c8aa  d2f88410  ldr.w	r1, [r2, #132]
0802c8ae  d2f88000  ldr.w	r0, [r2, #128]
0802c8b2  5962      str	r1, [r3, #36]
0802c8b4  1862      str	r0, [r3, #32]
0802c8b6  d2f88810  ldr.w	r1, [r2, #136]
0802c8ba  d2f88c20  ldr.w	r2, [r2, #140]
0802c8be  9962      str	r1, [r3, #40]
0802c8c0  da62      str	r2, [r3, #44]
0802c8c2  0024      movs	r4, #0
0802c8c4  5c4b      ldr	r3, [pc, #368] ; [0x0802ca38] = 0x2000241c
0802c8c6  5d4a      ldr	r2, [pc, #372] ; [0x0802ca3c] = 0x20000900
0802c8c8  0026      movs	r6, #0
0802c8ca  1c60      str	r4, [r3]
0802c8cc  5c4b      ldr	r3, [pc, #368] ; [0x0802ca40] = 0x20002424
0802c8ce  3146      mov	r1, r6
0802c8d0  1460      str	r4, [r2]
0802c8d2  5c4a      ldr	r2, [pc, #368] ; [0x0802ca44] = 0x20002420
0802c8d4  1c60      str	r4, [r3]
0802c8d6  5c4b      ldr	r3, [pc, #368] ; [0x0802ca48] = 0x200008c0
0802c8d8  1460      str	r4, [r2]
0802c8da  5c4a      ldr	r2, [pc, #368] ; [0x0802ca4c] = 0x20002414
0802c8dc  1c60      str	r4, [r3]
0802c8de  5c4b      ldr	r3, [pc, #368] ; [0x0802ca50] = 0x200008e0
0802c8e0  1460      str	r4, [r2]
0802c8e2  5c4a      ldr	r2, [pc, #368] ; [0x0802ca54] = 0x2000240c
0802c8e4  1c60      str	r4, [r3]
0802c8e6  5c4b      ldr	r3, [pc, #368] ; [0x0802ca58] = 0x20002efc
0802c8e8  1460      str	r4, [r2]
0802c8ea  5c4a      ldr	r2, [pc, #368] ; [0x0802ca5c] = 0x20002410
0802c8ec  1e60      str	r6, [r3]
0802c8ee  5c4b      ldr	r3, [pc, #368] ; [0x0802ca60] = 0x20002f00
0802c8f0  1460      str	r4, [r2]
0802c8f2  3022      movs	r2, #48
0802c8f4  5b48      ldr	r0, [pc, #364] ; [0x0802ca64] = 0x200013c0
0802c8f6  1e60      str	r6, [r3]
0802c8f8  09f0c3fd  bl	#39814 ; -> 0x08036482 ; branch_target=0x08036482
0802c8fc  4ff44062  mov.w	r2, #3072
0802c900  3146      mov	r1, r6
0802c902  5948      ldr	r0, [pc, #356] ; [0x0802ca68] = 0x20001400
0802c904  09f0bdfd  bl	#39802 ; -> 0x08036482 ; branch_target=0x08036482
0802c908  584b      ldr	r3, [pc, #352] ; [0x0802ca6c] = 0x20000620
0802c90a  5948      ldr	r0, [pc, #356] ; [0x0802ca70] = 0x20000420
0802c90c  5949      ldr	r1, [pc, #356] ; [0x0802ca74] = 0x20000220
0802c90e  03f5007c  add.w	r12, r3, #512
0802c912  594a      ldr	r2, [pc, #356] ; [0x0802ca78] = 0x20000020
0802c914  1c60      str	r4, [r3]
0802c916  0833      adds	r3, #8
0802c918  43f8044c  str	r4, [r3, #-4]
0802c91c  0830      adds	r0, #8
0802c91e  6345      cmp	r3, r12
0802c920  0c60      str	r4, [r1]
0802c922  4c60      str	r4, [r1, #4]
0802c924  02f10802  add.w	r2, r2, #8
0802c928  40f8084c  str	r4, [r0, #-8]
0802c92c  01f10801  add.w	r1, r1, #8
0802c930  40f8044c  str	r4, [r0, #-4]
0802c934  42f8084c  str	r4, [r2, #-8]
0802c938  42f8044c  str	r4, [r2, #-4]
0802c93c  ead1      bne	#-44 ; -> 0x0802c914 ; branch_target=0x0802c914
0802c93e  4ff40072  mov.w	r2, #512
0802c942  0021      movs	r1, #0
0802c944  4d48      ldr	r0, [pc, #308] ; [0x0802ca7c] = 0x20000f40
0802c946  09f09cfd  bl	#39736 ; -> 0x08036482 ; branch_target=0x08036482
0802c94a  4ff40072  mov.w	r2, #512
0802c94e  0021      movs	r1, #0
0802c950  4b48      ldr	r0, [pc, #300] ; [0x0802ca80] = 0x20002c40
0802c952  09f096fd  bl	#39724 ; -> 0x08036482 ; branch_target=0x08036482
0802c956  4ff40072  mov.w	r2, #512
0802c95a  0021      movs	r1, #0
0802c95c  4948      ldr	r0, [pc, #292] ; [0x0802ca84] = 0x20000d40
0802c95e  09f090fd  bl	#39712 ; -> 0x08036482 ; branch_target=0x08036482
0802c962  4ff40072  mov.w	r2, #512
0802c966  0021      movs	r1, #0
0802c968  4748      ldr	r0, [pc, #284] ; [0x0802ca88] = 0x20002a40
0802c96a  09f08afd  bl	#39700 ; -> 0x08036482 ; branch_target=0x08036482
0802c96e  474b      ldr	r3, [pc, #284] ; [0x0802ca8c] = 0x20002408
0802c970  4ff48062  mov.w	r2, #1024
0802c974  0021      movs	r1, #0
0802c976  4648      ldr	r0, [pc, #280] ; [0x0802ca90] = 0x30000440 / f32_bits_interpretation=4.657216834e-10
0802c978  1c60      str	r4, [r3]
0802c97a  09f082fd  bl	#39684 ; -> 0x08036482 ; branch_target=0x08036482
0802c97e  4ff48062  mov.w	r2, #1024
0802c982  0021      movs	r1, #0
0802c984  4348      ldr	r0, [pc, #268] ; [0x0802ca94] = 0x30001040 / f32_bits_interpretation=4.658922137e-10
0802c986  09f07cfd  bl	#39672 ; -> 0x08036482 ; branch_target=0x08036482
0802c98a  4ff48062  mov.w	r2, #1024
0802c98e  0021      movs	r1, #0
0802c990  4148      ldr	r0, [pc, #260] ; [0x0802ca98] = 0x30000c40 / f32_bits_interpretation=4.658353703e-10
0802c992  09f076fd  bl	#39660 ; -> 0x08036482 ; branch_target=0x08036482
0802c996  4ff48062  mov.w	r2, #1024
0802c99a  0021      movs	r1, #0
0802c99c  3f48      ldr	r0, [pc, #252] ; [0x0802ca9c] = 0x30000840 / f32_bits_interpretation=4.657785269e-10
0802c99e  09f070fd  bl	#39648 ; -> 0x08036482 ; branch_target=0x08036482
0802c9a2  4ff48062  mov.w	r2, #1024
0802c9a6  0021      movs	r1, #0
0802c9a8  3d48      ldr	r0, [pc, #244] ; [0x0802caa0] = 0x38000000 / f32_bits_interpretation=3.051757812e-05
0802c9aa  09f06afd  bl	#39636 ; -> 0x08036482 ; branch_target=0x08036482
0802c9ae  0023      movs	r3, #0
0802c9b0  3c49      ldr	r1, [pc, #240] ; [0x0802caa4] = 0x20002eec
0802c9b2  2846      mov	r0, r5
0802c9b4  3c4a      ldr	r2, [pc, #240] ; [0x0802caa8] = 0x20002f66
0802c9b6  0b60      str	r3, [r1]
0802c9b8  1370      strb	r3, [r2]
0802c9ba  05f0f5fe  bl	#24042 ; -> 0x080327a8 ; branch_target=0x080327a8
0802c9be  4ff48072  mov.w	r2, #256
0802c9c2  3449      ldr	r1, [pc, #208] ; [0x0802ca94] = 0x30001040 / f32_bits_interpretation=4.658922137e-10
0802c9c4  3948      ldr	r0, [pc, #228] ; [0x0802caac] = 0x200149b0
0802c9c6  f9f7f9f8  bl	#-28174 ; -> 0x08025bbc ; branch_target=0x08025bbc
0802c9ca  394c      ldr	r4, [pc, #228] ; [0x0802cab0] = 0x20002f68
0802c9cc  0346      mov	r3, r0
0802c9ce  4ff48072  mov.w	r2, #256
0802c9d2  3149      ldr	r1, [pc, #196] ; [0x0802ca98] = 0x30000c40 / f32_bits_interpretation=4.658353703e-10
0802c9d4  3748      ldr	r0, [pc, #220] ; [0x0802cab4] = 0x20014880
0802c9d6  2370      strb	r3, [r4]
0802c9d8  f9f7f0f8  bl	#-28192 ; -> 0x08025bbc ; branch_target=0x08025bbc
0802c9dc  0346      mov	r3, r0
0802c9de  4ff48072  mov.w	r2, #256
0802c9e2  2e49      ldr	r1, [pc, #184] ; [0x0802ca9c] = 0x30000840 / f32_bits_interpretation=4.657785269e-10
0802c9e4  3448      ldr	r0, [pc, #208] ; [0x0802cab8] = 0x200147e8
0802c9e6  2370      strb	r3, [r4]
0802c9e8  f9f7e8f8  bl	#-28208 ; -> 0x08025bbc ; branch_target=0x08025bbc
0802c9ec  0346      mov	r3, r0
0802c9ee  4ff48072  mov.w	r2, #256
0802c9f2  2b49      ldr	r1, [pc, #172] ; [0x0802caa0] = 0x38000000 / f32_bits_interpretation=3.051757812e-05
0802c9f4  3148      ldr	r0, [pc, #196] ; [0x0802cabc] = 0x20014750
0802c9f6  2370      strb	r3, [r4]
0802c9f8  f9f7e0f8  bl	#-28224 ; -> 0x08025bbc ; branch_target=0x08025bbc
0802c9fc  0346      mov	r3, r0
0802c9fe  4ff48072  mov.w	r2, #256
0802ca02  2349      ldr	r1, [pc, #140] ; [0x0802ca90] = 0x30000440 / f32_bits_interpretation=4.657216834e-10
0802ca04  2e48      ldr	r0, [pc, #184] ; [0x0802cac0] = 0x20014918
0802ca06  2370      strb	r3, [r4]
0802ca08  f9f76cf9  bl	#-27944 ; -> 0x08025ce4 ; branch_target=0x08025ce4
0802ca0c  2d4b      ldr	r3, [pc, #180] ; [0x0802cac4] = 0x20002f67
0802ca0e  1870      strb	r0, [r3]
0802ca10  f8bd      pop	{r3, r4, r5, r6, r7, pc}
0802ca12  002b      cmp	r3, #0
0802ca14  40f35e81  ble.w	#700 ; -> 0x0802ccd4 ; branch_target=0x0802ccd4
0802ca18  2b49      ldr	r1, [pc, #172] ; [0x0802cac8] = 0x60000000
0802ca1a  01eb8303  add.w	r3, r1, r3, lsl #2
0802ca1e  93ed007a  vldr	s14, [r3]
0802ca22  bce5      b	#-1160 ; -> 0x0802c59e ; branch_target=0x0802c59e
0802ca24  002b      cmp	r3, #0
0802ca26  40f35981  ble.w	#690 ; -> 0x0802ccdc ; branch_target=0x0802ccdc
0802ca2a  2749      ldr	r1, [pc, #156] ; [0x0802cac8] = 0x60000000
0802ca2c  01eb8303  add.w	r3, r1, r3, lsl #2
0802ca30  93ed007a  vldr	s14, [r3]
0802ca34  9fe5      b	#-1218 ; -> 0x0802c576 ; branch_target=0x0802c576
0802cacc  002e      cmp	r6, #0
0802cace  04dd      ble	#8 ; -> 0x0802cada ; branch_target=0x0802cada
0802cad0  844b      ldr	r3, [pc, #528] ; [0x0802cce4] = 0x200144d0
0802cad2  1b68      ldr	r3, [r3]
0802cad4  002b      cmp	r3, #0
0802cad6  3ff4d5ad  beq.w	#-1110 ; -> 0x0802c684 ; branch_target=0x0802c684
0802cada  834b      ldr	r3, [pc, #524] ; [0x0802cce8] = 0x080e0000
0802cadc  8349      ldr	r1, [pc, #524] ; [0x0802ccec] = 0x20001380
0802cade  186a      ldr	r0, [r3, #32]
0802cae0  834a      ldr	r2, [pc, #524] ; [0x0802ccf0] = 0x20001200
0802cae2  0860      str	r0, [r1]
0802cae4  586a      ldr	r0, [r3, #36]
0802cae6  834e      ldr	r6, [pc, #524] ; [0x0802ccf4] = 0x20001300
0802cae8  4860      str	r0, [r1, #4]
0802caea  986a      ldr	r0, [r3, #40]
0802caec  824c      ldr	r4, [pc, #520] ; [0x0802ccf8] = 0x200012c0
0802caee  8860      str	r0, [r1, #8]
0802caf0  d86a      ldr	r0, [r3, #44]
0802caf2  c860      str	r0, [r1, #12]
0802caf4  186b      ldr	r0, [r3, #48]
0802caf6  0861      str	r0, [r1, #16]
0802caf8  586b      ldr	r0, [r3, #52]
0802cafa  4861      str	r0, [r1, #20]
0802cafc  986b      ldr	r0, [r3, #56]
0802cafe  8861      str	r0, [r1, #24]
0802cb00  d86b      ldr	r0, [r3, #60]
0802cb02  c861      str	r0, [r1, #28]
0802cb04  186c      ldr	r0, [r3, #64]
0802cb06  0862      str	r0, [r1, #32]
0802cb08  586c      ldr	r0, [r3, #68]
0802cb0a  4862      str	r0, [r1, #36]
0802cb0c  986c      ldr	r0, [r3, #72]
0802cb0e  8862      str	r0, [r1, #40]
0802cb10  d86c      ldr	r0, [r3, #76]
0802cb12  c862      str	r0, [r1, #44]
0802cb14  196e      ldr	r1, [r3, #96]
0802cb16  7948      ldr	r0, [pc, #484] ; [0x0802ccfc] = 0x20001280
0802cb18  1160      str	r1, [r2]
0802cb1a  596e      ldr	r1, [r3, #100]
0802cb1c  5160      str	r1, [r2, #4]
0802cb1e  996e      ldr	r1, [r3, #104]
0802cb20  9160      str	r1, [r2, #8]
0802cb22  d96e      ldr	r1, [r3, #108]
0802cb24  d160      str	r1, [r2, #12]
0802cb26  196f      ldr	r1, [r3, #112]
0802cb28  1161      str	r1, [r2, #16]
0802cb2a  596f      ldr	r1, [r3, #116]
0802cb2c  5161      str	r1, [r2, #20]
0802cb2e  996f      ldr	r1, [r3, #120]
0802cb30  9161      str	r1, [r2, #24]
0802cb32  d96f      ldr	r1, [r3, #124]
0802cb34  d161      str	r1, [r2, #28]
0802cb36  d3f88010  ldr.w	r1, [r3, #128]
0802cb3a  1162      str	r1, [r2, #32]
0802cb3c  d3f88410  ldr.w	r1, [r3, #132]
0802cb40  5162      str	r1, [r2, #36]
0802cb42  d3f88810  ldr.w	r1, [r3, #136]
0802cb46  9162      str	r1, [r2, #40]
0802cb48  d3f88c10  ldr.w	r1, [r3, #140]
0802cb4c  d162      str	r1, [r2, #44]
0802cb4e  d3f8a020  ldr.w	r2, [r3, #160]
0802cb52  3260      str	r2, [r6]
0802cb54  d3f8a420  ldr.w	r2, [r3, #164]
0802cb58  7260      str	r2, [r6, #4]
0802cb5a  d3f8a820  ldr.w	r2, [r3, #168]
0802cb5e  b260      str	r2, [r6, #8]
0802cb60  d3f8ac20  ldr.w	r2, [r3, #172]
0802cb64  f260      str	r2, [r6, #12]
0802cb66  d3f8b020  ldr.w	r2, [r3, #176]
0802cb6a  3261      str	r2, [r6, #16]
0802cb6c  d3f8b420  ldr.w	r2, [r3, #180]
0802cb70  7261      str	r2, [r6, #20]
0802cb72  d3f8b820  ldr.w	r2, [r3, #184]
0802cb76  b261      str	r2, [r6, #24]
0802cb78  d3f8bc20  ldr.w	r2, [r3, #188]
0802cb7c  f261      str	r2, [r6, #28]
0802cb7e  d3f8c020  ldr.w	r2, [r3, #192]
0802cb82  2260      str	r2, [r4]
0802cb84  d3f8c420  ldr.w	r2, [r3, #196]
0802cb88  6260      str	r2, [r4, #4]
0802cb8a  d3f8c820  ldr.w	r2, [r3, #200]
0802cb8e  a260      str	r2, [r4, #8]
0802cb90  d3f8cc20  ldr.w	r2, [r3, #204]
0802cb94  e260      str	r2, [r4, #12]
0802cb96  d3f8d020  ldr.w	r2, [r3, #208]
0802cb9a  2261      str	r2, [r4, #16]
0802cb9c  d3f8d420  ldr.w	r2, [r3, #212]
0802cba0  6261      str	r2, [r4, #20]
0802cba2  d3f8d820  ldr.w	r2, [r3, #216]
0802cba6  a261      str	r2, [r4, #24]
0802cba8  d3f8dc20  ldr.w	r2, [r3, #220]
0802cbac  e261      str	r2, [r4, #28]
0802cbae  d3f8e020  ldr.w	r2, [r3, #224]
0802cbb2  0260      str	r2, [r0]
0802cbb4  d3f8e410  ldr.w	r1, [r3, #228]
0802cbb8  514a      ldr	r2, [pc, #324] ; [0x0802cd00] = 0x080e0100
0802cbba  4160      str	r1, [r0, #4]
0802cbbc  d3f8e810  ldr.w	r1, [r3, #232]
0802cbc0  8160      str	r1, [r0, #8]
0802cbc2  d3f8ec10  ldr.w	r1, [r3, #236]
0802cbc6  c160      str	r1, [r0, #12]
0802cbc8  d3f8f010  ldr.w	r1, [r3, #240]
0802cbcc  0161      str	r1, [r0, #16]
0802cbce  d3f8f410  ldr.w	r1, [r3, #244]
0802cbd2  4161      str	r1, [r0, #20]
0802cbd4  d3f8f810  ldr.w	r1, [r3, #248]
0802cbd8  8161      str	r1, [r0, #24]
0802cbda  d3f8fc10  ldr.w	r1, [r3, #252]
0802cbde  c161      str	r1, [r0, #28]
0802cbe0  4849      ldr	r1, [pc, #288] ; [0x0802cd04] = 0x20001240
0802cbe2  d2f800e0  ldr.w	lr, [r2]
0802cbe6  c1f800e0  str.w	lr, [r1]
0802cbea  d2f804e0  ldr.w	lr, [r2, #4]
0802cbee  c1f804e0  str.w	lr, [r1, #4]
0802cbf2  d2f808e0  ldr.w	lr, [r2, #8]
0802cbf6  c1f808e0  str.w	lr, [r1, #8]
0802cbfa  d2f80ce0  ldr.w	lr, [r2, #12]
0802cbfe  c1f80ce0  str.w	lr, [r1, #12]
0802cc02  d2f810e0  ldr.w	lr, [r2, #16]
0802cc06  c1f810e0  str.w	lr, [r1, #16]
0802cc0a  d2f814e0  ldr.w	lr, [r2, #20]
0802cc0e  c1f814e0  str.w	lr, [r1, #20]
0802cc12  d2f818e0  ldr.w	lr, [r2, #24]
0802cc16  c1f818e0  str.w	lr, [r1, #24]
0802cc1a  d2f81ce0  ldr.w	lr, [r2, #28]
0802cc1e  c1f81ce0  str.w	lr, [r1, #28]
0802cc22  d3f82071  ldr.w	r7, [r3, #288]
0802cc26  3762      str	r7, [r6, #32]
0802cc28  d3f82471  ldr.w	r7, [r3, #292]
0802cc2c  7762      str	r7, [r6, #36]
0802cc2e  d3f82871  ldr.w	r7, [r3, #296]
0802cc32  b762      str	r7, [r6, #40]
0802cc34  d3f82c71  ldr.w	r7, [r3, #300]
0802cc38  f762      str	r7, [r6, #44]
0802cc3a  d3f83071  ldr.w	r7, [r3, #304]
0802cc3e  3763      str	r7, [r6, #48]
0802cc40  d3f83471  ldr.w	r7, [r3, #308]
0802cc44  7763      str	r7, [r6, #52]
0802cc46  d3f83871  ldr.w	r7, [r3, #312]
0802cc4a  b763      str	r7, [r6, #56]
0802cc4c  d3f83c71  ldr.w	r7, [r3, #316]
0802cc50  f763      str	r7, [r6, #60]
0802cc52  d3f84061  ldr.w	r6, [r3, #320]
0802cc56  2662      str	r6, [r4, #32]
0802cc58  d3f84461  ldr.w	r6, [r3, #324]
0802cc5c  6662      str	r6, [r4, #36]
0802cc5e  d3f84861  ldr.w	r6, [r3, #328]
0802cc62  a662      str	r6, [r4, #40]
0802cc64  d3f84c61  ldr.w	r6, [r3, #332]
0802cc68  e662      str	r6, [r4, #44]
0802cc6a  d3f85061  ldr.w	r6, [r3, #336]
0802cc6e  2663      str	r6, [r4, #48]
0802cc70  d3f85461  ldr.w	r6, [r3, #340]
0802cc74  6663      str	r6, [r4, #52]
0802cc76  d3f85861  ldr.w	r6, [r3, #344]
0802cc7a  a663      str	r6, [r4, #56]
0802cc7c  d3f85c31  ldr.w	r3, [r3, #348]
0802cc80  e363      str	r3, [r4, #60]
0802cc82  136e      ldr	r3, [r2, #96]
0802cc84  0362      str	r3, [r0, #32]
0802cc86  536e      ldr	r3, [r2, #100]
0802cc88  4362      str	r3, [r0, #36]
0802cc8a  936e      ldr	r3, [r2, #104]
0802cc8c  8362      str	r3, [r0, #40]
0802cc8e  d36e      ldr	r3, [r2, #108]
0802cc90  c362      str	r3, [r0, #44]
0802cc92  136f      ldr	r3, [r2, #112]
0802cc94  0363      str	r3, [r0, #48]
0802cc96  536f      ldr	r3, [r2, #116]
0802cc98  4363      str	r3, [r0, #52]
0802cc9a  936f      ldr	r3, [r2, #120]
0802cc9c  8363      str	r3, [r0, #56]
0802cc9e  d36f      ldr	r3, [r2, #124]
0802cca0  c363      str	r3, [r0, #60]
0802cca2  d2f88000  ldr.w	r0, [r2, #128]
0802cca6  d2f88430  ldr.w	r3, [r2, #132]
0802ccaa  0862      str	r0, [r1, #32]
0802ccac  4b62      str	r3, [r1, #36]
0802ccae  d2f88800  ldr.w	r0, [r2, #136]
0802ccb2  d2f88c30  ldr.w	r3, [r2, #140]
0802ccb6  8862      str	r0, [r1, #40]
0802ccb8  cb62      str	r3, [r1, #44]
0802ccba  d2f89000  ldr.w	r0, [r2, #144]
0802ccbe  d2f89430  ldr.w	r3, [r2, #148]
0802ccc2  0863      str	r0, [r1, #48]
0802ccc4  4b63      str	r3, [r1, #52]
0802ccc6  d2f89800  ldr.w	r0, [r2, #152]
0802ccca  d2f89c30  ldr.w	r3, [r2, #156]
0802ccce  8863      str	r0, [r1, #56]
0802ccd0  cb63      str	r3, [r1, #60]
0802ccd2  f6e5      b	#-1044 ; -> 0x0802c8c2 ; branch_target=0x0802c8c2
0802ccd4  0c4b      ldr	r3, [pc, #48] ; [0x0802cd08] = 0x60000000
0802ccd6  93ed007a  vldr	s14, [r3]
0802ccda  60e4      b	#-1856 ; -> 0x0802c59e ; branch_target=0x0802c59e
0802ccdc  0a4b      ldr	r3, [pc, #40] ; [0x0802cd08] = 0x60000000
0802ccde  93ed007a  vldr	s14, [r3]
0802cce2  48e4      b	#-1904 ; -> 0x0802c576 ; branch_target=0x0802c576
