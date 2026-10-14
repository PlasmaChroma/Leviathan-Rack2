; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
0802d900  2de9f04f  push.w	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
0802d904  4021      movs	r1, #64
0802d906  85b0      sub	sp, #20
0802d908  b648      ldr	r0, [pc, #728] ; [0x0802dbe4] = 0x58021800
0802d90a  f5f779fd  bl	#-42254 ; -> 0x08023400 ; branch_target=0x08023400
0802d90e  b64c      ldr	r4, [pc, #728] ; [0x0802dbe8] = 0x20002f60
0802d910  0346      mov	r3, r0
0802d912  8021      movs	r1, #128
0802d914  b348      ldr	r0, [pc, #716] ; [0x0802dbe4] = 0x58021800
0802d916  2370      strb	r3, [r4]
0802d918  f5f772fd  bl	#-42268 ; -> 0x08023400 ; branch_target=0x08023400
0802d91c  0346      mov	r3, r0
0802d91e  8021      movs	r1, #128
0802d920  b248      ldr	r0, [pc, #712] ; [0x0802dbec] = 0x58020c00
0802d922  6370      strb	r3, [r4, #1]
0802d924  f5f76cfd  bl	#-42280 ; -> 0x08023400 ; branch_target=0x08023400
0802d928  0346      mov	r3, r0
0802d92a  4021      movs	r1, #64
0802d92c  b048      ldr	r0, [pc, #704] ; [0x0802dbf0] = 0x58020400
0802d92e  b14e      ldr	r6, [pc, #708] ; [0x0802dbf4] = 0x20002f0c
0802d930  a370      strb	r3, [r4, #2]
0802d932  f5f765fd  bl	#-42294 ; -> 0x08023400 ; branch_target=0x08023400
0802d936  0346      mov	r3, r0
0802d938  0821      movs	r1, #8
0802d93a  ac48      ldr	r0, [pc, #688] ; [0x0802dbec] = 0x58020c00
0802d93c  e370      strb	r3, [r4, #3]
0802d93e  f5f75ffd  bl	#-42306 ; -> 0x08023400 ; branch_target=0x08023400
0802d942  0146      mov	r1, r0
0802d944  2071      strb	r0, [r4, #4]
0802d946  41f27070  movw	r0, #6000
0802d94a  d6e90023  ldrd	r2, r3, [r6]
0802d94e  0132      adds	r2, #1
0802d950  0133      adds	r3, #1
0802d952  8242      cmp	r2, r0
0802d954  c6e90023  strd	r2, r3, [r6]
0802d958  04d9      bls	#8 ; -> 0x0802d964 ; branch_target=0x0802d964
0802d95a  a74a      ldr	r2, [pc, #668] ; [0x0802dbf8] = 0x20002e84
0802d95c  1268      ldr	r2, [r2]
0802d95e  002a      cmp	r2, #0
0802d960  00f03c81  beq.w	#632 ; -> 0x0802dbdc ; branch_target=0x0802dbdc
0802d964  41f27072  movw	r2, #6000
0802d968  9342      cmp	r3, r2
0802d96a  04d9      bls	#8 ; -> 0x0802d976 ; branch_target=0x0802d976
0802d96c  a34b      ldr	r3, [pc, #652] ; [0x0802dbfc] = 0x20002e80
0802d96e  1b68      ldr	r3, [r3]
0802d970  002b      cmp	r3, #0
0802d972  00f02381  beq.w	#582 ; -> 0x0802dbbc ; branch_target=0x0802dbbc
0802d976  a24f      ldr	r7, [pc, #648] ; [0x0802dc00] = 0x20002ed0
0802d978  dff8bcb2  ldr.w	r11, [pc, #700] ; [0x0802dc38] = 0x20002ecc
0802d97c  3b68      ldr	r3, [r7]
0802d97e  dbf80020  ldr.w	r2, [r11]
0802d982  013b      subs	r3, #1
0802d984  013a      subs	r2, #1
0802d986  002b      cmp	r3, #0
0802d988  3b60      str	r3, [r7]
0802d98a  cbf80020  str.w	r2, [r11]
0802d98e  01da      bge	#2 ; -> 0x0802d994 ; branch_target=0x0802d994
0802d990  0023      movs	r3, #0
0802d992  3b60      str	r3, [r7]
0802d994  002a      cmp	r2, #0
0802d996  02da      bge	#4 ; -> 0x0802d99e ; branch_target=0x0802d99e
0802d998  0022      movs	r2, #0
0802d99a  cbf80020  str.w	r2, [r11]
0802d99e  994d      ldr	r5, [pc, #612] ; [0x0802dc04] = 0x20002f48
0802d9a0  dff898c2  ldr.w	r12, [pc, #664] ; [0x0802dc3c] = 0x20002ee4
0802d9a4  94f800e0  ldrb.w	lr, [r4]
0802d9a8  2a78      ldrb	r2, [r5]
0802d9aa  dcf80080  ldr.w	r8, [r12]
0802d9ae  bef1000f  cmp.w	lr, #0
0802d9b2  50d0      beq	#160 ; -> 0x0802da56 ; branch_target=0x0802da56
0802d9b4  042a      cmp	r2, #4
0802d9b6  944a      ldr	r2, [pc, #592] ; [0x0802dc08] = 0x20002f58
0802d9b8  00f04a84  beq.w	#2196 ; -> 0x0802e250 ; branch_target=0x0802e250
0802d9bc  1378      ldrb	r3, [r2]
0802d9be  7345      cmp	r3, lr
0802d9c0  00f08083  beq.w	#1792 ; -> 0x0802e0c4 ; branch_target=0x0802e0c4
0802d9c4  0323      movs	r3, #3
0802d9c6  b8f1010f  cmp.w	r8, #1
0802d9ca  82f800e0  strb.w	lr, [r2]
0802d9ce  2b70      strb	r3, [r5]
0802d9d0  00f06683  beq.w	#1740 ; -> 0x0802e0a0 ; branch_target=0x0802e0a0
0802d9d4  8d4b      ldr	r3, [pc, #564] ; [0x0802dc0c] = 0x20002ef8
0802d9d6  1b68      ldr	r3, [r3]
0802d9d8  2bb1      cbz	r3, #10 ; -> 0x0802d9e6 ; branch_target=0x0802d9e6
0802d9da  052b      cmp	r3, #5
0802d9dc  03d0      beq	#6 ; -> 0x0802d9e6 ; branch_target=0x0802d9e6
0802d9de  4ff07e51  mov.w	r1, #1065353216
0802d9e2  8b4b      ldr	r3, [pc, #556] ; [0x0802dc10] = 0x20002418
0802d9e4  1960      str	r1, [r3]
0802d9e6  8b49      ldr	r1, [pc, #556] ; [0x0802dc14] = 0x20002f34
0802d9e8  0023      movs	r3, #0
0802d9ea  6078      ldrb	r0, [r4, #1]
0802d9ec  0b60      str	r3, [r1]
0802d9ee  6b78      ldrb	r3, [r5, #1]
0802d9f0  042b      cmp	r3, #4
0802d9f2  00f05381  beq.w	#678 ; -> 0x0802dc9c ; branch_target=0x0802dc9c
0802d9f6  5778      ldrb	r7, [r2, #1]
0802d9f8  874b      ldr	r3, [pc, #540] ; [0x0802dc18] = 0x20002edc
0802d9fa  8742      cmp	r7, r0
0802d9fc  d3f800c0  ldr.w	r12, [r3]
0802da00  00f0d280  beq.w	#420 ; -> 0x0802dba8 ; branch_target=0x0802dba8
0802da04  5070      strb	r0, [r2, #1]
0802da06  0028      cmp	r0, #0
0802da08  40f09981  bne.w	#818 ; -> 0x0802dd3e ; branch_target=0x0802dd3e
0802da0c  0227      movs	r7, #2
0802da0e  bcf1010f  cmp.w	r12, #1
0802da12  6f70      strb	r7, [r5, #1]
0802da14  00d1      bne	#0 ; -> 0x0802da18 ; branch_target=0x0802da18
0802da16  1860      str	r0, [r3]
0802da18  4b68      ldr	r3, [r1, #4]
0802da1a  dff82492  ldr.w	r9, [pc, #548] ; [0x0802dc40] = 0x200144d4
0802da1e  092b      cmp	r3, #9
0802da20  d9f80000  ldr.w	r0, [r9]
0802da24  40f2e382  bls.w	#1478 ; -> 0x0802dfee ; branch_target=0x0802dfee
0802da28  d9f80030  ldr.w	r3, [r9]
0802da2c  002b      cmp	r3, #0
0802da2e  40f39d84  ble.w	#2362 ; -> 0x0802e36c ; branch_target=0x0802e36c
0802da32  0133      adds	r3, #1
0802da34  032b      cmp	r3, #3
0802da36  c9f80030  str.w	r3, [r9]
0802da3a  00f00385  beq.w	#2566 ; -> 0x0802e444 ; branch_target=0x0802e444
0802da3e  0c2b      cmp	r3, #12
0802da40  00f0b285  beq.w	#2916 ; -> 0x0802e5a8 ; branch_target=0x0802e5a8
0802da44  2b78      ldrb	r3, [r5]
0802da46  042b      cmp	r3, #4
0802da48  02d0      beq	#4 ; -> 0x0802da50 ; branch_target=0x0802da50
0802da4a  744b      ldr	r3, [pc, #464] ; [0x0802dc1c] = 0x20002f08
0802da4c  0020      movs	r0, #0
0802da4e  1860      str	r0, [r3]
0802da50  0023      movs	r3, #0
0802da52  7360      str	r3, [r6, #4]
0802da54  46e1      b	#652 ; -> 0x0802dce4 ; branch_target=0x0802dce4
0802da56  6078      ldrb	r0, [r4, #1]
0802da58  0028      cmp	r0, #0
0802da5a  40f0b280  bne.w	#356 ; -> 0x0802dbc2 ; branch_target=0x0802dbc2
0802da5e  dff8cce1  ldr.w	lr, [pc, #460] ; [0x0802dc2c] = 0x20002f04
0802da62  042a      cmp	r2, #4
0802da64  cef80000  str.w	r0, [lr]
0802da68  dff8b0e1  ldr.w	lr, [pc, #432] ; [0x0802dc1c] = 0x20002f08
0802da6c  cef80000  str.w	r0, [lr]
0802da70  00f0f782  beq.w	#1518 ; -> 0x0802e062 ; branch_target=0x0802e062
0802da74  644a      ldr	r2, [pc, #400] ; [0x0802dc08] = 0x20002f58
0802da76  1178      ldrb	r1, [r2]
0802da78  0029      cmp	r1, #0
0802da7a  00f01883  beq.w	#1584 ; -> 0x0802e0ae ; branch_target=0x0802e0ae
0802da7e  0023      movs	r3, #0
0802da80  0221      movs	r1, #2
0802da82  b8f1010f  cmp.w	r8, #1
0802da86  1370      strb	r3, [r2]
0802da88  2970      strb	r1, [r5]
0802da8a  01d1      bne	#2 ; -> 0x0802da90 ; branch_target=0x0802da90
0802da8c  ccf80030  str.w	r3, [r12]
0802da90  6049      ldr	r1, [pc, #384] ; [0x0802dc14] = 0x20002f34
0802da92  dff8ac91  ldr.w	r9, [pc, #428] ; [0x0802dc40] = 0x200144d4
0802da96  0b68      ldr	r3, [r1]
0802da98  d9f80000  ldr.w	r0, [r9]
0802da9c  092b      cmp	r3, #9
0802da9e  38d8      bhi	#112 ; -> 0x0802db12 ; branch_target=0x0802db12
0802daa0  0028      cmp	r0, #0
0802daa2  40f30c84  ble.w	#2072 ; -> 0x0802e2be ; branch_target=0x0802e2be
0802daa6  0023      movs	r3, #0
0802daa8  cdf80490  str.w	r9, [sp, #4]
0802daac  dff878a1  ldr.w	r10, [pc, #376] ; [0x0802dc28] = 0x20002f50
0802dab0  c9f80030  str.w	r3, [r9]
0802dab4  4c4b      ldr	r3, [pc, #304] ; [0x0802dbe8] = 0x20002f60
0802dab6  dff85081  ldr.w	r8, [pc, #336] ; [0x0802dc08] = 0x20002f58
0802daba  9946      mov	r9, r3
0802dabc  cde90212  strd	r1, r2, [sp, #8]
0802dac0  0022      movs	r2, #0
0802dac2  4021      movs	r1, #64
0802dac4  5648      ldr	r0, [pc, #344] ; [0x0802dc20] = 0x58020800
0802dac6  0af8012b  strb	r2, [r10], #1
0802daca  09f8012b  strb	r2, [r9], #1
0802dace  08f8012b  strb	r2, [r8], #1
0802dad2  0122      movs	r2, #1
0802dad4  f5f79afc  bl	#-42700 ; -> 0x0802340c ; branch_target=0x0802340c
0802dad8  0122      movs	r2, #1
0802dada  8021      movs	r1, #128
0802dadc  5048      ldr	r0, [pc, #320] ; [0x0802dc20] = 0x58020800
0802dade  f5f795fc  bl	#-42710 ; -> 0x0802340c ; branch_target=0x0802340c
0802dae2  0122      movs	r2, #1
0802dae4  4ff48051  mov.w	r1, #4096
0802dae8  4048      ldr	r0, [pc, #256] ; [0x0802dbec] = 0x58020c00
0802daea  f5f78ffc  bl	#-42722 ; -> 0x0802340c ; branch_target=0x0802340c
0802daee  0122      movs	r2, #1
0802daf0  4ff40051  mov.w	r1, #8192
0802daf4  3d48      ldr	r0, [pc, #244] ; [0x0802dbec] = 0x58020c00
0802daf6  f5f789fc  bl	#-42734 ; -> 0x0802340c ; branch_target=0x0802340c
0802dafa  0122      movs	r2, #1
0802dafc  4ff40041  mov.w	r1, #32768
0802db00  3b48      ldr	r0, [pc, #236] ; [0x0802dbf0] = 0x58020400
0802db02  f5f783fc  bl	#-42746 ; -> 0x0802340c ; branch_target=0x0802340c
0802db06  474b      ldr	r3, [pc, #284] ; [0x0802dc24] = 0x20002f55
0802db08  5345      cmp	r3, r10
0802db0a  d9d1      bne	#-78 ; -> 0x0802dac0 ; branch_target=0x0802dac0
0802db0c  039a      ldr	r2, [sp, #12]
0802db0e  dde90191  ldrd	r9, r1, [sp, #4]
0802db12  d9f80030  ldr.w	r3, [r9]
0802db16  002b      cmp	r3, #0
0802db18  40f38384  ble.w	#2310 ; -> 0x0802e422 ; branch_target=0x0802e422
0802db1c  0023      movs	r3, #0
0802db1e  424f      ldr	r7, [pc, #264] ; [0x0802dc28] = 0x20002f50
0802db20  dff8c480  ldr.w	r8, [pc, #196] ; [0x0802dbe8] = 0x20002f60
0802db24  c9f80030  str.w	r3, [r9]
0802db28  dff8dca0  ldr.w	r10, [pc, #220] ; [0x0802dc08] = 0x20002f58
0802db2c  dff8f090  ldr.w	r9, [pc, #240] ; [0x0802dc20] = 0x58020800
0802db30  cde90112  strd	r1, r2, [sp, #4]
0802db34  0022      movs	r2, #0
0802db36  4021      movs	r1, #64
0802db38  4846      mov	r0, r9
0802db3a  07f8012b  strb	r2, [r7], #1
0802db3e  08f8012b  strb	r2, [r8], #1
0802db42  0af8012b  strb	r2, [r10], #1
0802db46  0122      movs	r2, #1
0802db48  f5f760fc  bl	#-42816 ; -> 0x0802340c ; branch_target=0x0802340c
0802db4c  0122      movs	r2, #1
0802db4e  8021      movs	r1, #128
0802db50  4846      mov	r0, r9
0802db52  f5f75bfc  bl	#-42826 ; -> 0x0802340c ; branch_target=0x0802340c
0802db56  0122      movs	r2, #1
0802db58  4ff48051  mov.w	r1, #4096
0802db5c  2348      ldr	r0, [pc, #140] ; [0x0802dbec] = 0x58020c00
0802db5e  f5f755fc  bl	#-42838 ; -> 0x0802340c ; branch_target=0x0802340c
0802db62  0122      movs	r2, #1
0802db64  4ff40051  mov.w	r1, #8192
0802db68  2048      ldr	r0, [pc, #128] ; [0x0802dbec] = 0x58020c00
0802db6a  f5f74ffc  bl	#-42850 ; -> 0x0802340c ; branch_target=0x0802340c
0802db6e  0122      movs	r2, #1
0802db70  4ff40041  mov.w	r1, #32768
0802db74  1e48      ldr	r0, [pc, #120] ; [0x0802dbf0] = 0x58020400
0802db76  f5f749fc  bl	#-42862 ; -> 0x0802340c ; branch_target=0x0802340c
0802db7a  2a4b      ldr	r3, [pc, #168] ; [0x0802dc24] = 0x20002f55
0802db7c  bb42      cmp	r3, r7
0802db7e  d9d1      bne	#-78 ; -> 0x0802db34 ; branch_target=0x0802db34
0802db80  6b78      ldrb	r3, [r5, #1]
0802db82  dde90112  ldrd	r1, r2, [sp, #4]
0802db86  0193      str	r3, [sp, #4]
0802db88  0198      ldr	r0, [sp, #4]
0802db8a  0023      movs	r3, #0
0802db8c  0428      cmp	r0, #4
0802db8e  3360      str	r3, [r6]
0802db90  00f08380  beq.w	#262 ; -> 0x0802dc9a ; branch_target=0x0802dc9a
0802db94  2548      ldr	r0, [pc, #148] ; [0x0802dc2c] = 0x20002f04
0802db96  5778      ldrb	r7, [r2, #1]
0802db98  0360      str	r3, [r0]
0802db9a  6078      ldrb	r0, [r4, #1]
0802db9c  1e4b      ldr	r3, [pc, #120] ; [0x0802dc18] = 0x20002edc
0802db9e  8742      cmp	r7, r0
0802dba0  d3f800c0  ldr.w	r12, [r3]
0802dba4  7ff42eaf  bne.w	#-420 ; -> 0x0802da04 ; branch_target=0x0802da04
0802dba8  0028      cmp	r0, #0
0802dbaa  40f01082  bne.w	#1056 ; -> 0x0802dfce ; branch_target=0x0802dfce
0802dbae  bcf1010f  cmp.w	r12, #1
0802dbb2  6870      strb	r0, [r5, #1]
0802dbb4  40f08380  bne.w	#262 ; -> 0x0802dcbe ; branch_target=0x0802dcbe
0802dbb8  1860      str	r0, [r3]
0802dbba  80e0      b	#256 ; -> 0x0802dcbe ; branch_target=0x0802dcbe
0802dbbc  1c4a      ldr	r2, [pc, #112] ; [0x0802dc30] = 0x20002ed4
0802dbbe  1360      str	r3, [r2]
0802dbc0  d9e6      b	#-590 ; -> 0x0802d976 ; branch_target=0x0802d976
0802dbc2  042a      cmp	r2, #4
0802dbc4  7ff456af  bne.w	#-340 ; -> 0x0802da74 ; branch_target=0x0802da74
0802dbc8  0f4a      ldr	r2, [pc, #60] ; [0x0802dc08] = 0x20002f58
0802dbca  b8f1010f  cmp.w	r8, #1
0802dbce  82f800e0  strb.w	lr, [r2]
0802dbd2  40f07b82  bne.w	#1270 ; -> 0x0802e0cc ; branch_target=0x0802e0cc
0802dbd6  ccf800e0  str.w	lr, [r12]
0802dbda  77e2      b	#1262 ; -> 0x0802e0cc ; branch_target=0x0802e0cc
0802dbdc  1548      ldr	r0, [pc, #84] ; [0x0802dc34] = 0x20002ed8
0802dbde  0260      str	r2, [r0]
0802dbe0  c0e6      b	#-640 ; -> 0x0802d964 ; branch_target=0x0802d964
0802dc44  a3f1330c  sub.w	r12, r3, #51
0802dc48  bcf5b56f  cmp.w	r12, #1448
0802dc4c  00f24584  bhi.w	#2186 ; -> 0x0802e4da ; branch_target=0x0802e4da
0802dc50  dff8b4c2  ldr.w	r12, [pc, #692] ; [0x0802df08] = 0x20000b20
0802dc54  0423      movs	r3, #4
0802dc56  4ff0000e  mov.w	lr, #0
0802dc5a  994f      ldr	r7, [pc, #612] ; [0x0802dec0] = 0x2001348c
0802dc5c  6b70      strb	r3, [r5, #1]
0802dc5e  dcf80030  ldr.w	r3, [r12]
0802dc62  0133      adds	r3, #1
0802dc64  03f00f00  and	r0, r3, #15
0802dc68  40f24163  movw	r3, #1601
0802dc6c  4b60      str	r3, [r1, #4]
0802dc6e  954b      ldr	r3, [pc, #596] ; [0x0802dec4] = 0x20002f08
0802dc70  ccf80000  str.w	r0, [r12]
0802dc74  4ff0010c  mov.w	r12, #1
0802dc78  c3f800e0  str.w	lr, [r3]
0802dc7c  924b      ldr	r3, [pc, #584] ; [0x0802dec8] = 0x20002f04
0802dc7e  c3f800c0  str.w	r12, [r3]
0802dc82  fb6a      ldr	r3, [r7, #44]
0802dc84  9842      cmp	r0, r3
0802dc86  06d0      beq	#12 ; -> 0x0802dc96 ; branch_target=0x0802dc96
0802dc88  904b      ldr	r3, [pc, #576] ; [0x0802decc] = 0x20002434
0802dc8a  f862      str	r0, [r7, #44]
0802dc8c  c3f800e0  str.w	lr, [r3]
0802dc90  8f4b      ldr	r3, [pc, #572] ; [0x0802ded0] = 0x20002eb8
0802dc92  c3f800c0  str.w	r12, [r3]
0802dc96  0023      movs	r3, #0
0802dc98  3360      str	r3, [r6]
0802dc9a  6078      ldrb	r0, [r4, #1]
0802dc9c  2378      ldrb	r3, [r4]
0802dc9e  a678      ldrb	r6, [r4, #2]
0802dca0  0344      add	r3, r0
0802dca2  3344      add	r3, r6
0802dca4  e678      ldrb	r6, [r4, #3]
0802dca6  3344      add	r3, r6
0802dca8  2679      ldrb	r6, [r4, #4]
0802dcaa  9b19      adds	r3, r3, r6
0802dcac  40f07e81  bne.w	#764 ; -> 0x0802dfac ; branch_target=0x0802dfac
0802dcb0  8848      ldr	r0, [pc, #544] ; [0x0802ded4] = 0x20002edc
0802dcb2  6b70      strb	r3, [r5, #1]
0802dcb4  0668      ldr	r6, [r0]
0802dcb6  5370      strb	r3, [r2, #1]
0802dcb8  012e      cmp	r6, #1
0802dcba  00f0fe82  beq.w	#1532 ; -> 0x0802e2ba ; branch_target=0x0802e2ba
0802dcbe  dbf80030  ldr.w	r3, [r11]
0802dcc2  002b      cmp	r3, #0
0802dcc4  07dd      ble	#14 ; -> 0x0802dcd6 ; branch_target=0x0802dcd6
0802dcc6  844b      ldr	r3, [pc, #528] ; [0x0802ded8] = 0x20002ef4
0802dcc8  1b68      ldr	r3, [r3]
0802dcca  002b      cmp	r3, #0
0802dccc  00f06882  beq.w	#1232 ; -> 0x0802e1a0 ; branch_target=0x0802e1a0
0802dcd0  052b      cmp	r3, #5
0802dcd2  00f06582  beq.w	#1226 ; -> 0x0802e1a0 ; branch_target=0x0802e1a0
0802dcd6  7a4f      ldr	r7, [pc, #488] ; [0x0802dec0] = 0x2001348c
0802dcd8  804b      ldr	r3, [pc, #512] ; [0x0802dedc] = 0x20002ec0
0802dcda  b7f90600  ldrsh.w	r0, [r7, #6]
0802dcde  1860      str	r0, [r3]
0802dce0  0023      movs	r3, #0
0802dce2  4b60      str	r3, [r1, #4]
0802dce4  9378      ldrb	r3, [r2, #2]
0802dce6  a078      ldrb	r0, [r4, #2]
0802dce8  8342      cmp	r3, r0
0802dcea  7d4b      ldr	r3, [pc, #500] ; [0x0802dee0] = 0x20002f20
0802dcec  05d0      beq	#10 ; -> 0x0802dcfa ; branch_target=0x0802dcfa
0802dcee  0128      cmp	r0, #1
0802dcf0  00f05282  beq.w	#1188 ; -> 0x0802e198 ; branch_target=0x0802e198
0802dcf4  9070      strb	r0, [r2, #2]
0802dcf6  0020      movs	r0, #0
0802dcf8  8860      str	r0, [r1, #8]
0802dcfa  e078      ldrb	r0, [r4, #3]
0802dcfc  d678      ldrb	r6, [r2, #3]
0802dcfe  8642      cmp	r6, r0
0802dd00  05d0      beq	#10 ; -> 0x0802dd0e ; branch_target=0x0802dd0e
0802dd02  0128      cmp	r0, #1
0802dd04  00f04a82  beq.w	#1172 ; -> 0x0802e19c ; branch_target=0x0802e19c
0802dd08  d070      strb	r0, [r2, #3]
0802dd0a  0020      movs	r0, #0
0802dd0c  c860      str	r0, [r1, #12]
0802dd0e  2079      ldrb	r0, [r4, #4]
0802dd10  1479      ldrb	r4, [r2, #4]
0802dd12  8442      cmp	r4, r0
0802dd14  06d0      beq	#12 ; -> 0x0802dd24 ; branch_target=0x0802dd24
0802dd16  0128      cmp	r0, #1
0802dd18  1071      strb	r0, [r2, #4]
0802dd1a  4ff00002  mov.w	r2, #0
0802dd1e  08bf      it	eq
0802dd20  1861      streq	r0, [r3, #16]
0802dd22  0a61      str	r2, [r1, #16]
0802dd24  9a68      ldr	r2, [r3, #8]
0802dd26  012a      cmp	r2, #1
0802dd28  1ad0      beq	#52 ; -> 0x0802dd60 ; branch_target=0x0802dd60
0802dd2a  da68      ldr	r2, [r3, #12]
0802dd2c  012a      cmp	r2, #1
0802dd2e  4ed0      beq	#156 ; -> 0x0802ddce ; branch_target=0x0802ddce
0802dd30  1a69      ldr	r2, [r3, #16]
0802dd32  012a      cmp	r2, #1
0802dd34  00f08180  beq.w	#258 ; -> 0x0802de3a ; branch_target=0x0802de3a
0802dd38  05b0      add	sp, #20
0802dd3a  bde8f08f  pop.w	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
0802dd3e  0320      movs	r0, #3
0802dd40  bcf1010f  cmp.w	r12, #1
0802dd44  6870      strb	r0, [r5, #1]
0802dd46  00f0af81  beq.w	#862 ; -> 0x0802e0a8 ; branch_target=0x0802e0a8
0802dd4a  634b      ldr	r3, [pc, #396] ; [0x0802ded8] = 0x20002ef4
0802dd4c  1b68      ldr	r3, [r3]
0802dd4e  002b      cmp	r3, #0
0802dd50  c6d0      beq	#-116 ; -> 0x0802dce0 ; branch_target=0x0802dce0
0802dd52  052b      cmp	r3, #5
0802dd54  c4d0      beq	#-120 ; -> 0x0802dce0 ; branch_target=0x0802dce0
0802dd56  634b      ldr	r3, [pc, #396] ; [0x0802dee4] = 0x20002414
0802dd58  4ff07e50  mov.w	r0, #1065353216
0802dd5c  1860      str	r0, [r3]
0802dd5e  bfe7      b	#-130 ; -> 0x0802dce0 ; branch_target=0x0802dce0
0802dd60  6148      ldr	r0, [pc, #388] ; [0x0802dee8] = 0x20002f50
0802dd62  8478      ldrb	r4, [r0, #2]
0802dd64  002c      cmp	r4, #0
0802dd66  40f0fd80  bne.w	#506 ; -> 0x0802df64 ; branch_target=0x0802df64
0802dd6a  0c68      ldr	r4, [r1]
0802dd6c  322c      cmp	r4, #50
0802dd6e  40f20082  bls.w	#1024 ; -> 0x0802e172 ; branch_target=0x0802e172
0802dd72  0420      movs	r0, #4
0802dd74  2870      strb	r0, [r5]
0802dd76  40f24160  movw	r0, #1601
0802dd7a  0860      str	r0, [r1]
0802dd7c  5b48      ldr	r0, [pc, #364] ; [0x0802deec] = 0x20002e94
0802dd7e  0068      ldr	r0, [r0]
0802dd80  00bb      cbnz	r0, #64 ; -> 0x0802ddc4 ; branch_target=0x0802ddc4
0802dd82  5b4c      ldr	r4, [pc, #364] ; [0x0802def0] = 0x20002430
0802dd84  5b4e      ldr	r6, [pc, #364] ; [0x0802def4] = 0x20002e84
0802dd86  2768      ldr	r7, [r4]
0802dd88  d6f800e0  ldr.w	lr, [r6]
0802dd8c  5a4c      ldr	r4, [pc, #360] ; [0x0802def8] = 0x20000a20
0802dd8e  4fea071c  lsl.w	r12, r7, #4
0802dd92  04eb0717  add.w	r7, r4, r7, lsl #4
0802dd96  54f80c80  ldr.w	r8, [r4, r12]
0802dd9a  bef1000f  cmp.w	lr, #0
0802dd9e  40f09283  bne.w	#1828 ; -> 0x0802e4c6 ; branch_target=0x0802e4c6
0802dda2  0434      adds	r4, #4
0802dda4  5548      ldr	r0, [pc, #340] ; [0x0802defc] = 0x20002e7c
0802dda6  3260      str	r2, [r6]
0802dda8  4ff48066  mov.w	r6, #1024
0802ddac  c0f80080  str.w	r8, [r0]
0802ddb0  0ceb0400  add.w	r0, r12, r4
0802ddb4  44f80c60  str.w	r6, [r4, r12]
0802ddb8  4260      str	r2, [r0, #4]
0802ddba  0020      movs	r0, #0
0802ddbc  504a      ldr	r2, [pc, #320] ; [0x0802df00] = 0x20002e9c
0802ddbe  c7f80ce0  str.w	lr, [r7, #12]
0802ddc2  1060      str	r0, [r2]
0802ddc4  0022      movs	r2, #0
0802ddc6  9a60      str	r2, [r3, #8]
0802ddc8  da68      ldr	r2, [r3, #12]
0802ddca  012a      cmp	r2, #1
0802ddcc  b0d1      bne	#-160 ; -> 0x0802dd30 ; branch_target=0x0802dd30
0802ddce  464c      ldr	r4, [pc, #280] ; [0x0802dee8] = 0x20002f50
0802ddd0  4c4e      ldr	r6, [pc, #304] ; [0x0802df04] = 0x20002e80
0802ddd2  94f803c0  ldrb.w	r12, [r4, #3]
0802ddd6  3068      ldr	r0, [r6]
0802ddd8  bcf1000f  cmp.w	r12, #0
0802dddc  50d1      bne	#160 ; -> 0x0802de80 ; branch_target=0x0802de80
0802ddde  4f68      ldr	r7, [r1, #4]
0802dde0  322f      cmp	r7, #50
0802dde2  40f2ae81  bls.w	#860 ; -> 0x0802e142 ; branch_target=0x0802e142
0802dde6  484c      ldr	r4, [pc, #288] ; [0x0802df08] = 0x20000b20
0802dde8  40f2416e  movw	lr, #1601
0802ddec  2768      ldr	r7, [r4]
0802ddee  c1f804e0  str.w	lr, [r1, #4]
0802ddf2  4ff0040e  mov.w	lr, #4
0802ddf6  454c      ldr	r4, [pc, #276] ; [0x0802df0c] = 0x20000920
0802ddf8  85f801e0  strb.w	lr, [r5, #1]
0802ddfc  07fa0efe  lsl.w	lr, r7, lr
0802de00  04eb0717  add.w	r7, r4, r7, lsl #4
0802de04  54f80e80  ldr.w	r8, [r4, lr]
0802de08  0028      cmp	r0, #0
0802de0a  40f02b82  bne.w	#1110 ; -> 0x0802e264 ; branch_target=0x0802e264
0802de0e  3260      str	r2, [r6]
0802de10  0434      adds	r4, #4
0802de12  3f4e      ldr	r6, [pc, #252] ; [0x0802df10] = 0x200011a0
0802de14  4ff4806c  mov.w	r12, #1024
0802de18  c6f80080  str.w	r8, [r6]
0802de1c  0eeb0406  add.w	r6, lr, r4
0802de20  44f80ec0  str.w	r12, [r4, lr]
0802de24  7260      str	r2, [r6, #4]
0802de26  3b4a      ldr	r2, [pc, #236] ; [0x0802df14] = 0x200011e0
0802de28  f860      str	r0, [r7, #12]
0802de2a  0020      movs	r0, #0
0802de2c  1060      str	r0, [r2]
0802de2e  0022      movs	r2, #0
0802de30  da60      str	r2, [r3, #12]
0802de32  1a69      ldr	r2, [r3, #16]
0802de34  012a      cmp	r2, #1
0802de36  7ff47faf  bne.w	#-258 ; -> 0x0802dd38 ; branch_target=0x0802dd38
0802de3a  0868      ldr	r0, [r1]
0802de3c  3228      cmp	r0, #50
0802de3e  71d9      bls	#226 ; -> 0x0802df24 ; branch_target=0x0802df24
0802de40  3548      ldr	r0, [pc, #212] ; [0x0802df18] = 0x20002ef8
0802de42  40f24162  movw	r2, #1601
0802de46  0a60      str	r2, [r1]
0802de48  0421      movs	r1, #4
0802de4a  0268      ldr	r2, [r0]
0802de4c  2970      strb	r1, [r5]
0802de4e  0132      adds	r2, #1
0802de50  2649      ldr	r1, [pc, #152] ; [0x0802deec] = 0x20002e94
0802de52  052a      cmp	r2, #5
0802de54  0968      ldr	r1, [r1]
0802de56  0260      str	r2, [r0]
0802de58  40f39481  ble.w	#808 ; -> 0x0802e184 ; branch_target=0x0802e184
0802de5c  0022      movs	r2, #0
0802de5e  0129      cmp	r1, #1
0802de60  0260      str	r2, [r0]
0802de62  00f09581  beq.w	#810 ; -> 0x0802e190 ; branch_target=0x0802e190
0802de66  1649      ldr	r1, [pc, #88] ; [0x0802dec0] = 0x2001348c
0802de68  8868      ldr	r0, [r1, #8]
0802de6a  9042      cmp	r0, r2
0802de6c  03d0      beq	#6 ; -> 0x0802de76 ; branch_target=0x0802de76
0802de6e  1848      ldr	r0, [pc, #96] ; [0x0802ded0] = 0x20002eb8
0802de70  0124      movs	r4, #1
0802de72  8a60      str	r2, [r1, #8]
0802de74  0460      str	r4, [r0]
0802de76  0022      movs	r2, #0
0802de78  1a61      str	r2, [r3, #16]
0802de7a  05b0      add	sp, #20
0802de7c  bde8f08f  pop.w	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
0802de80  0028      cmp	r0, #0
0802de82  d4d1      bne	#-88 ; -> 0x0802de2e ; branch_target=0x0802de2e
0802de84  4f68      ldr	r7, [r1, #4]
0802de86  dff898c0  ldr.w	r12, [pc, #152] ; [0x0802df20] = 0x20002e88
0802de8a  322f      cmp	r7, #50
0802de8c  dcf80060  ldr.w	r6, [r12]
0802de90  40f2f181  bls.w	#994 ; -> 0x0802e276 ; branch_target=0x0802e276
0802de94  40f24164  movw	r4, #1601
0802de98  3046      mov	r0, r6
0802de9a  204e      ldr	r6, [pc, #128] ; [0x0802df1c] = 0x20000820
0802de9c  4c60      str	r4, [r1, #4]
0802de9e  3468      ldr	r4, [r6]
0802dea0  074f      ldr	r7, [pc, #28] ; [0x0802dec0] = 0x2001348c
0802dea2  b4fa84f4  clz	r4, r4
0802dea6  6409      lsrs	r4, r4, #5
0802dea8  3460      str	r4, [r6]
0802deaa  7c62      str	r4, [r7, #36]
0802deac  084c      ldr	r4, [pc, #32] ; [0x0802ded0] = 0x20002eb8
0802deae  2260      str	r2, [r4]
0802deb0  7a69      ldr	r2, [r7, #20]
0802deb2  8242      cmp	r2, r0
0802deb4  bbd0      beq	#-138 ; -> 0x0802de2e ; branch_target=0x0802de2e
0802deb6  7861      str	r0, [r7, #20]
0802deb8  0120      movs	r0, #1
0802deba  054a      ldr	r2, [pc, #20] ; [0x0802ded0] = 0x20002eb8
0802debc  1060      str	r0, [r2]
0802debe  b6e7      b	#-148 ; -> 0x0802de2e ; branch_target=0x0802de2e
0802df24  4868      ldr	r0, [r1, #4]
0802df26  3228      cmp	r0, #50
0802df28  40f21281  bls.w	#548 ; -> 0x0802e150 ; branch_target=0x0802e150
0802df2c  a648      ldr	r0, [pc, #664] ; [0x0802e1c8] = 0x20002ef4
0802df2e  40f24162  movw	r2, #1601
0802df32  4a60      str	r2, [r1, #4]
0802df34  0421      movs	r1, #4
0802df36  0268      ldr	r2, [r0]
0802df38  6970      strb	r1, [r5, #1]
0802df3a  0132      adds	r2, #1
0802df3c  a349      ldr	r1, [pc, #652] ; [0x0802e1cc] = 0x20002e90
0802df3e  052a      cmp	r2, #5
0802df40  0968      ldr	r1, [r1]
0802df42  0260      str	r2, [r0]
0802df44  40f3a981  ble.w	#850 ; -> 0x0802e29a ; branch_target=0x0802e29a
0802df48  0022      movs	r2, #0
0802df4a  0129      cmp	r1, #1
0802df4c  0260      str	r2, [r0]
0802df4e  00f0aa81  beq.w	#852 ; -> 0x0802e2a6 ; branch_target=0x0802e2a6
0802df52  9f49      ldr	r1, [pc, #636] ; [0x0802e1d0] = 0x2001348c
0802df54  c868      ldr	r0, [r1, #12]
0802df56  9042      cmp	r0, r2
0802df58  8dd0      beq	#-230 ; -> 0x0802de76 ; branch_target=0x0802de76
0802df5a  9e48      ldr	r0, [pc, #632] ; [0x0802e1d4] = 0x20002eb8
0802df5c  0124      movs	r4, #1
0802df5e  ca60      str	r2, [r1, #12]
0802df60  0460      str	r4, [r0]
0802df62  88e7      b	#-240 ; -> 0x0802de76 ; branch_target=0x0802de76
0802df64  9c4c      ldr	r4, [pc, #624] ; [0x0802e1d8] = 0x20002e84
0802df66  2668      ldr	r6, [r4]
0802df68  002e      cmp	r6, #0
0802df6a  7ff42baf  bne.w	#-426 ; -> 0x0802ddc4 ; branch_target=0x0802ddc4
0802df6e  0f68      ldr	r7, [r1]
0802df70  dff898c2  ldr.w	r12, [pc, #664] ; [0x0802e20c] = 0x20002e8c
0802df74  322f      cmp	r7, #50
0802df76  dcf80040  ldr.w	r4, [r12]
0802df7a  40f28581  bls.w	#778 ; -> 0x0802e288 ; branch_target=0x0802e288
0802df7e  974e      ldr	r6, [pc, #604] ; [0x0802e1dc] = 0x200023d0
0802df80  2046      mov	r0, r4
0802df82  40f24164  movw	r4, #1601
0802df86  924f      ldr	r7, [pc, #584] ; [0x0802e1d0] = 0x2001348c
0802df88  0c60      str	r4, [r1]
0802df8a  3468      ldr	r4, [r6]
0802df8c  b4fa84f4  clz	r4, r4
0802df90  6409      lsrs	r4, r4, #5
0802df92  3460      str	r4, [r6]
0802df94  3c62      str	r4, [r7, #32]
0802df96  8f4c      ldr	r4, [pc, #572] ; [0x0802e1d4] = 0x20002eb8
0802df98  2260      str	r2, [r4]
0802df9a  3a69      ldr	r2, [r7, #16]
0802df9c  8242      cmp	r2, r0
0802df9e  3ff411af  beq.w	#-478 ; -> 0x0802ddc4 ; branch_target=0x0802ddc4
0802dfa2  3861      str	r0, [r7, #16]
0802dfa4  0120      movs	r0, #1
0802dfa6  8b4a      ldr	r2, [pc, #556] ; [0x0802e1d4] = 0x20002eb8
0802dfa8  1060      str	r0, [r2]
0802dfaa  0be7      b	#-490 ; -> 0x0802ddc4 ; branch_target=0x0802ddc4
0802dfac  8c4b      ldr	r3, [pc, #560] ; [0x0802e1e0] = 0x20002edc
0802dfae  5070      strb	r0, [r2, #1]
0802dfb0  1868      ldr	r0, [r3]
0802dfb2  0128      cmp	r0, #1
0802dfb4  00f04981  beq.w	#658 ; -> 0x0802e24a ; branch_target=0x0802e24a
0802dfb8  4b68      ldr	r3, [r1, #4]
0802dfba  0133      adds	r3, #1
0802dfbc  b3f5c86f  cmp.w	r3, #1600
0802dfc0  4b60      str	r3, [r1, #4]
0802dfc2  40f28f80  bls.w	#286 ; -> 0x0802e0e4 ; branch_target=0x0802e0e4
0802dfc6  4ff4c863  mov.w	r3, #1600
0802dfca  4b60      str	r3, [r1, #4]
0802dfcc  8ae6      b	#-748 ; -> 0x0802dce4 ; branch_target=0x0802dce4
0802dfce  0120      movs	r0, #1
0802dfd0  8445      cmp	r12, r0
0802dfd2  6870      strb	r0, [r5, #1]
0802dfd4  f0d1      bne	#-32 ; -> 0x0802dfb8 ; branch_target=0x0802dfb8
0802dfd6  0020      movs	r0, #0
0802dfd8  dff84092  ldr.w	r9, [pc, #576] ; [0x0802e21c] = 0x200144d4
0802dfdc  1860      str	r0, [r3]
0802dfde  0223      movs	r3, #2
0802dfe0  d9f80000  ldr.w	r0, [r9]
0802dfe4  6b70      strb	r3, [r5, #1]
0802dfe6  4b68      ldr	r3, [r1, #4]
0802dfe8  092b      cmp	r3, #9
0802dfea  3ff61dad  bhi.w	#-1478 ; -> 0x0802da28 ; branch_target=0x0802da28
0802dfee  0028      cmp	r0, #0
0802dff0  40f32382  ble.w	#1094 ; -> 0x0802e43a ; branch_target=0x0802e43a
0802dff4  dff828a2  ldr.w	r10, [pc, #552] ; [0x0802e220] = 0x20002f60
0802dff8  0023      movs	r3, #0
0802dffa  cdf80490  str.w	r9, [sp, #4]
0802dffe  c9f80030  str.w	r3, [r9]
0802e002  d146      mov	r9, r10
0802e004  dff80082  ldr.w	r8, [pc, #512] ; [0x0802e208] = 0x20002f50
0802e008  9246      mov	r10, r2
0802e00a  764f      ldr	r7, [pc, #472] ; [0x0802e1e4] = 0x20002f58
0802e00c  0291      str	r1, [sp, #8]
0802e00e  0022      movs	r2, #0
0802e010  4021      movs	r1, #64
0802e012  7548      ldr	r0, [pc, #468] ; [0x0802e1e8] = 0x58020800
0802e014  08f8012b  strb	r2, [r8], #1
0802e018  09f8012b  strb	r2, [r9], #1
0802e01c  07f8012b  strb	r2, [r7], #1
0802e020  0122      movs	r2, #1
0802e022  f5f7f3f9  bl	#-44058 ; -> 0x0802340c ; branch_target=0x0802340c
0802e026  0122      movs	r2, #1
0802e028  8021      movs	r1, #128
0802e02a  6f48      ldr	r0, [pc, #444] ; [0x0802e1e8] = 0x58020800
0802e02c  f5f7eef9  bl	#-44068 ; -> 0x0802340c ; branch_target=0x0802340c
0802e030  0122      movs	r2, #1
0802e032  4ff48051  mov.w	r1, #4096
0802e036  6d48      ldr	r0, [pc, #436] ; [0x0802e1ec] = 0x58020c00
0802e038  f5f7e8f9  bl	#-44080 ; -> 0x0802340c ; branch_target=0x0802340c
0802e03c  0122      movs	r2, #1
0802e03e  4ff40051  mov.w	r1, #8192
0802e042  6a48      ldr	r0, [pc, #424] ; [0x0802e1ec] = 0x58020c00
0802e044  f5f7e2f9  bl	#-44092 ; -> 0x0802340c ; branch_target=0x0802340c
0802e048  0122      movs	r2, #1
0802e04a  4ff40041  mov.w	r1, #32768
0802e04e  6848      ldr	r0, [pc, #416] ; [0x0802e1f0] = 0x58020400
0802e050  f5f7dcf9  bl	#-44104 ; -> 0x0802340c ; branch_target=0x0802340c
0802e054  674b      ldr	r3, [pc, #412] ; [0x0802e1f4] = 0x20002f55
0802e056  4345      cmp	r3, r8
0802e058  d9d1      bne	#-78 ; -> 0x0802e00e ; branch_target=0x0802e00e
0802e05a  5246      mov	r2, r10
0802e05c  dde90191  ldrd	r9, r1, [sp, #4]
0802e060  e2e4      b	#-1596 ; -> 0x0802da28 ; branch_target=0x0802da28
0802e062  a278      ldrb	r2, [r4, #2]
0802e064  94f803e0  ldrb.w	lr, [r4, #3]
0802e068  7244      add	r2, lr
0802e06a  ca42      cmn	r2, r1
0802e06c  40f04283  bne.w	#1668 ; -> 0x0802e6f4 ; branch_target=0x0802e6f4
0802e070  5c4a      ldr	r2, [pc, #368] ; [0x0802e1e4] = 0x20002f58
0802e072  b8f1010f  cmp.w	r8, #1
0802e076  2870      strb	r0, [r5]
0802e078  1070      strb	r0, [r2]
0802e07a  1dd0      beq	#58 ; -> 0x0802e0b8 ; branch_target=0x0802e0b8
0802e07c  3bb1      cbz	r3, #14 ; -> 0x0802e08e ; branch_target=0x0802e08e
0802e07e  5e4b      ldr	r3, [pc, #376] ; [0x0802e1f8] = 0x20002ef8
0802e080  1b68      ldr	r3, [r3]
0802e082  002b      cmp	r3, #0
0802e084  00f0ce80  beq.w	#412 ; -> 0x0802e224 ; branch_target=0x0802e224
0802e088  052b      cmp	r3, #5
0802e08a  00f0cb80  beq.w	#406 ; -> 0x0802e224 ; branch_target=0x0802e224
0802e08e  504f      ldr	r7, [pc, #320] ; [0x0802e1d0] = 0x2001348c
0802e090  5a4b      ldr	r3, [pc, #360] ; [0x0802e1fc] = 0x20002ec4
0802e092  b7f90410  ldrsh.w	r1, [r7, #4]
0802e096  1960      str	r1, [r3]
0802e098  0023      movs	r3, #0
0802e09a  5949      ldr	r1, [pc, #356] ; [0x0802e200] = 0x20002f34
0802e09c  0b60      str	r3, [r1]
0802e09e  a6e4      b	#-1716 ; -> 0x0802d9ee ; branch_target=0x0802d9ee
0802e0a0  0023      movs	r3, #0
0802e0a2  ccf80030  str.w	r3, [r12]
0802e0a6  95e4      b	#-1750 ; -> 0x0802d9d4 ; branch_target=0x0802d9d4
0802e0a8  0020      movs	r0, #0
0802e0aa  1860      str	r0, [r3]
0802e0ac  4de6      b	#-870 ; -> 0x0802dd4a ; branch_target=0x0802dd4a
0802e0ae  0021      movs	r1, #0
0802e0b0  b8f1010f  cmp.w	r8, #1
0802e0b4  2970      strb	r1, [r5]
0802e0b6  e1d1      bne	#-62 ; -> 0x0802e07c ; branch_target=0x0802e07c
0802e0b8  0023      movs	r3, #0
0802e0ba  ccf80030  str.w	r3, [r12]
0802e0be  0223      movs	r3, #2
0802e0c0  2b70      strb	r3, [r5]
0802e0c2  e5e4      b	#-1590 ; -> 0x0802da90 ; branch_target=0x0802da90
0802e0c4  0123      movs	r3, #1
0802e0c6  9845      cmp	r8, r3
0802e0c8  2b70      strb	r3, [r5]
0802e0ca  f5d0      beq	#-22 ; -> 0x0802e0b8 ; branch_target=0x0802e0b8
0802e0cc  4c49      ldr	r1, [pc, #304] ; [0x0802e200] = 0x20002f34
0802e0ce  0b68      ldr	r3, [r1]
0802e0d0  0133      adds	r3, #1
0802e0d2  b3f5c86f  cmp.w	r3, #1600
0802e0d6  0b60      str	r3, [r1]
0802e0d8  1cd9      bls	#56 ; -> 0x0802e114 ; branch_target=0x0802e114
0802e0da  4ff4c863  mov.w	r3, #1600
0802e0de  6078      ldrb	r0, [r4, #1]
0802e0e0  0b60      str	r3, [r1]
0802e0e2  84e4      b	#-1784 ; -> 0x0802d9ee ; branch_target=0x0802d9ee
0802e0e4  40f2dc50  movw	r0, #1500
0802e0e8  8342      cmp	r3, r0
0802e0ea  7ff6fbad  bls.w	#-1034 ; -> 0x0802dce4 ; branch_target=0x0802dce4
0802e0ee  0868      ldr	r0, [r1]
0802e0f0  8342      cmp	r3, r0
0802e0f2  7ff6f7ad  bls.w	#-1042 ; -> 0x0802dce4 ; branch_target=0x0802dce4
0802e0f6  b0f5967f  cmp.w	r0, #300
0802e0fa  00f2bd82  bhi.w	#1402 ; -> 0x0802e678 ; branch_target=0x0802e678
0802e0fe  344f      ldr	r7, [pc, #208] ; [0x0802e1d0] = 0x2001348c
0802e100  0120      movs	r0, #1
0802e102  344b      ldr	r3, [pc, #208] ; [0x0802e1d4] = 0x20002eb8
0802e104  1860      str	r0, [r3]
0802e106  fb88      ldrh	r3, [r7, #6]
0802e108  c31a      subs	r3, r0, r3
0802e10a  fb80      strh	r3, [r7, #6]
0802e10c  40f24163  movw	r3, #1601
0802e110  4b60      str	r3, [r1, #4]
0802e112  e7e5      b	#-1074 ; -> 0x0802dce4 ; branch_target=0x0802dce4
0802e114  40f2dc50  movw	r0, #1500
0802e118  8342      cmp	r3, r0
0802e11a  10d9      bls	#32 ; -> 0x0802e13e ; branch_target=0x0802e13e
0802e11c  4868      ldr	r0, [r1, #4]
0802e11e  8342      cmp	r3, r0
0802e120  0dd9      bls	#26 ; -> 0x0802e13e ; branch_target=0x0802e13e
0802e122  b0f5967f  cmp.w	r0, #300
0802e126  00f2b782  bhi.w	#1390 ; -> 0x0802e698 ; branch_target=0x0802e698
0802e12a  294f      ldr	r7, [pc, #164] ; [0x0802e1d0] = 0x2001348c
0802e12c  0120      movs	r0, #1
0802e12e  294b      ldr	r3, [pc, #164] ; [0x0802e1d4] = 0x20002eb8
0802e130  1860      str	r0, [r3]
0802e132  bb88      ldrh	r3, [r7, #4]
0802e134  c31a      subs	r3, r0, r3
0802e136  bb80      strh	r3, [r7, #4]
0802e138  40f24163  movw	r3, #1601
0802e13c  0b60      str	r3, [r1]
0802e13e  6078      ldrb	r0, [r4, #1]
0802e140  55e4      b	#-1878 ; -> 0x0802d9ee ; branch_target=0x0802d9ee
0802e142  0028      cmp	r0, #0
0802e144  7ff473ae  bne.w	#-794 ; -> 0x0802de2e ; branch_target=0x0802de2e
0802e148  e270      strb	r2, [r4, #3]
0802e14a  2e4a      ldr	r2, [pc, #184] ; [0x0802e204] = 0x20002e88
0802e14c  1060      str	r0, [r2]
0802e14e  6ee6      b	#-804 ; -> 0x0802de2e ; branch_target=0x0802de2e
0802e150  2d49      ldr	r1, [pc, #180] ; [0x0802e208] = 0x20002f50
0802e152  0879      ldrb	r0, [r1, #4]
0802e154  0028      cmp	r0, #0
0802e156  40f0aa80  bne.w	#340 ; -> 0x0802e2ae ; branch_target=0x0802e2ae
0802e15a  1446      mov	r4, r2
0802e15c  1c48      ldr	r0, [pc, #112] ; [0x0802e1d0] = 0x2001348c
0802e15e  0a71      strb	r2, [r1, #4]
0802e160  026b      ldr	r2, [r0, #48]
0802e162  a242      cmp	r2, r4
0802e164  3ff487ae  beq.w	#-754 ; -> 0x0802de76 ; branch_target=0x0802de76
0802e168  1a4a      ldr	r2, [pc, #104] ; [0x0802e1d4] = 0x20002eb8
0802e16a  0121      movs	r1, #1
0802e16c  0463      str	r4, [r0, #48]
0802e16e  1160      str	r1, [r2]
0802e170  81e6      b	#-766 ; -> 0x0802de76 ; branch_target=0x0802de76
0802e172  194c      ldr	r4, [pc, #100] ; [0x0802e1d8] = 0x20002e84
0802e174  2468      ldr	r4, [r4]
0802e176  002c      cmp	r4, #0
0802e178  7ff424ae  bne.w	#-952 ; -> 0x0802ddc4 ; branch_target=0x0802ddc4
0802e17c  8270      strb	r2, [r0, #2]
0802e17e  234a      ldr	r2, [pc, #140] ; [0x0802e20c] = 0x20002e8c
0802e180  1460      str	r4, [r2]
0802e182  1fe6      b	#-962 ; -> 0x0802ddc4 ; branch_target=0x0802ddc4
0802e184  0129      cmp	r1, #1
0802e186  7ff46eae  bne.w	#-804 ; -> 0x0802de66 ; branch_target=0x0802de66
0802e18a  002a      cmp	r2, #0
0802e18c  7ff46bae  bne.w	#-810 ; -> 0x0802de66 ; branch_target=0x0802de66
0802e190  0121      movs	r1, #1
0802e192  0a46      mov	r2, r1
0802e194  0160      str	r1, [r0]
0802e196  66e6      b	#-820 ; -> 0x0802de66 ; branch_target=0x0802de66
0802e198  9860      str	r0, [r3, #8]
0802e19a  abe5      b	#-1194 ; -> 0x0802dcf4 ; branch_target=0x0802dcf4
0802e19c  d860      str	r0, [r3, #12]
0802e19e  b3e5      b	#-1178 ; -> 0x0802dd08 ; branch_target=0x0802dd08
0802e1a0  1b4b      ldr	r3, [pc, #108] ; [0x0802e210] = 0x20000b20
0802e1a2  1c48      ldr	r0, [pc, #112] ; [0x0802e214] = 0x20000920
0802e1a4  1b68      ldr	r3, [r3]
0802e1a6  1c4f      ldr	r7, [pc, #112] ; [0x0802e218] = 0x20002434
0802e1a8  00eb0310  add.w	r0, r0, r3, lsl #4
0802e1ac  3b68      ldr	r3, [r7]
0802e1ae  d0e90160  ldrd	r6, r0, [r0, #4]
0802e1b2  0133      adds	r3, #1
0802e1b4  06fb00f0  mul	r0, r6, r0
0802e1b8  3b60      str	r3, [r7]
0802e1ba  8342      cmp	r3, r0
0802e1bc  fff68bad  blt.w	#-1258 ; -> 0x0802dcd6 ; branch_target=0x0802dcd6
0802e1c0  0023      movs	r3, #0
0802e1c2  3b60      str	r3, [r7]
0802e1c4  87e5      b	#-1266 ; -> 0x0802dcd6 ; branch_target=0x0802dcd6
0802e224  c34b      ldr	r3, [pc, #780] ; [0x0802e534] = 0x20002430
0802e226  c44f      ldr	r7, [pc, #784] ; [0x0802e538] = 0x20002438
0802e228  1968      ldr	r1, [r3]
0802e22a  c44b      ldr	r3, [pc, #784] ; [0x0802e53c] = 0x20000a20
0802e22c  03eb0113  add.w	r3, r3, r1, lsl #4
0802e230  d3e90113  ldrd	r1, r3, [r3, #4]
0802e234  03fb01f1  mul	r1, r3, r1
0802e238  3b68      ldr	r3, [r7]
0802e23a  0133      adds	r3, #1
0802e23c  8b42      cmp	r3, r1
0802e23e  3b60      str	r3, [r7]
0802e240  fff625af  blt.w	#-438 ; -> 0x0802e08e ; branch_target=0x0802e08e
0802e244  0023      movs	r3, #0
0802e246  3b60      str	r3, [r7]
0802e248  21e7      b	#-446 ; -> 0x0802e08e ; branch_target=0x0802e08e
0802e24a  0020      movs	r0, #0
0802e24c  1860      str	r0, [r3]
0802e24e  b3e6      b	#-666 ; -> 0x0802dfb8 ; branch_target=0x0802dfb8
0802e250  b8f1010f  cmp.w	r8, #1
0802e254  82f800e0  strb.w	lr, [r2]
0802e258  7ff438af  bne.w	#-400 ; -> 0x0802e0cc ; branch_target=0x0802e0cc
0802e25c  0023      movs	r3, #0
0802e25e  ccf80030  str.w	r3, [r12]
0802e262  33e7      b	#-410 ; -> 0x0802e0cc ; branch_target=0x0802e0cc
0802e264  b64a      ldr	r2, [pc, #728] ; [0x0802e540] = 0x200011a0
0802e266  c6f800c0  str.w	r12, [r6]
0802e26a  1268      ldr	r2, [r2]
0802e26c  a2eb0802  sub.w	r2, r2, r8
0802e270  9209      lsrs	r2, r2, #6
0802e272  7a60      str	r2, [r7, #4]
0802e274  dbe5      b	#-1098 ; -> 0x0802de2e ; branch_target=0x0802de2e
0802e276  022e      cmp	r6, #2
0802e278  00f0d781  beq.w	#942 ; -> 0x0802e62a ; branch_target=0x0802e62a
0802e27c  701c      adds	r0, r6, #1
0802e27e  b14f      ldr	r7, [pc, #708] ; [0x0802e544] = 0x2001348c
0802e280  e270      strb	r2, [r4, #3]
0802e282  ccf80000  str.w	r0, [r12]
0802e286  13e6      b	#-986 ; -> 0x0802deb0 ; branch_target=0x0802deb0
0802e288  022c      cmp	r4, #2
0802e28a  00f0c681  beq.w	#908 ; -> 0x0802e61a ; branch_target=0x0802e61a
0802e28e  8270      strb	r2, [r0, #2]
0802e290  601c      adds	r0, r4, #1
0802e292  ac4f      ldr	r7, [pc, #688] ; [0x0802e544] = 0x2001348c
0802e294  ccf80000  str.w	r0, [r12]
0802e298  7fe6      b	#-770 ; -> 0x0802df9a ; branch_target=0x0802df9a
0802e29a  0129      cmp	r1, #1
0802e29c  7ff459ae  bne.w	#-846 ; -> 0x0802df52 ; branch_target=0x0802df52
0802e2a0  002a      cmp	r2, #0
0802e2a2  7ff456ae  bne.w	#-852 ; -> 0x0802df52 ; branch_target=0x0802df52
0802e2a6  0121      movs	r1, #1
0802e2a8  0a46      mov	r2, r1
0802e2aa  0160      str	r1, [r0]
0802e2ac  51e6      b	#-862 ; -> 0x0802df52 ; branch_target=0x0802df52
0802e2ae  0128      cmp	r0, #1
0802e2b0  00f0b181  beq.w	#866 ; -> 0x0802e616 ; branch_target=0x0802e616
0802e2b4  0022      movs	r2, #0
0802e2b6  1446      mov	r4, r2
0802e2b8  50e7      b	#-352 ; -> 0x0802e15c ; branch_target=0x0802e15c
0802e2ba  0360      str	r3, [r0]
0802e2bc  ffe4      b	#-1538 ; -> 0x0802dcbe ; branch_target=0x0802dcbe
0802e2be  4b68      ldr	r3, [r1, #4]
0802e2c0  312b      cmp	r3, #49
0802e2c2  00f20a81  bhi.w	#532 ; -> 0x0802e4da ; branch_target=0x0802e4da
0802e2c6  6b78      ldrb	r3, [r5, #1]
0802e2c8  0193      str	r3, [sp, #4]
0802e2ca  9a4b      ldr	r3, [pc, #616] ; [0x0802e534] = 0x20002430
0802e2cc  dff86c82  ldr.w	r8, [pc, #620] ; [0x0802e53c] = 0x20000a20
0802e2d0  1b68      ldr	r3, [r3]
0802e2d2  dff8bcc2  ldr.w	r12, [pc, #700] ; [0x0802e590] = 0x20002e84
0802e2d6  08eb031e  add.w	lr, r8, r3, lsl #4
0802e2da  dcf800a0  ldr.w	r10, [r12]
0802e2de  4fea031c  lsl.w	r12, r3, #4
0802e2e2  def804e0  ldr.w	lr, [lr, #4]
0802e2e6  984b      ldr	r3, [pc, #608] ; [0x0802e548] = 0x20002ef8
0802e2e8  baf1010f  cmp.w	r10, #1
0802e2ec  d3f80090  ldr.w	r9, [r3]
0802e2f0  0ef1ff33  add.w	r3, lr, #4294967295
0802e2f4  4feaa313  asr.w	r3, r3, #6
0802e2f8  03f10103  add.w	r3, r3, #1
0802e2fc  08bf      it	eq
0802e2fe  5346      moveq	r3, r10
0802e300  3b60      str	r3, [r7]
0802e302  0127      movs	r7, #1
0802e304  914b      ldr	r3, [pc, #580] ; [0x0802e54c] = 0x20002ed8
0802e306  1f60      str	r7, [r3]
0802e308  b9f1000f  cmp.w	r9, #0
0802e30c  00f0a481  beq.w	#840 ; -> 0x0802e658 ; branch_target=0x0802e658
0802e310  b9f1050f  cmp.w	r9, #5
0802e314  00f0a081  beq.w	#832 ; -> 0x0802e658 ; branch_target=0x0802e658
0802e318  3368      ldr	r3, [r6]
0802e31a  41f26f37  movw	r7, #4975
0802e31e  a3f5806c  sub.w	r12, r3, #1024
0802e322  bc45      cmp	r12, r7
0802e324  00f2d681  bhi.w	#940 ; -> 0x0802e6d4 ; branch_target=0x0802e6d4
0802e328  07ee903a  vmov	s15, r3
0802e32c  dfed886a  vldr	s13, [pc, #544] ; [0x0802e550] = 0x3c800000 / f32_bits_interpretation=0.015625
0802e330  884f      ldr	r7, [pc, #544] ; [0x0802e554] = 0x200023f8
0802e332  b8eee77a  vcvt.f32.s32	s14, s15
0802e336  c6ee877a  vdiv.f32	s15, s13, s14
0802e33a  c7ed007a  vstr	s15, [r7]
0802e33e  07ee903a  vmov	s15, r3
0802e342  9fed857a  vldr	s14, [pc, #532] ; [0x0802e558] = 0x3faaaaaa / f32_bits_interpretation=1.333333254
0802e346  7f4f      ldr	r7, [pc, #508] ; [0x0802e544] = 0x2001348c
0802e348  f8ee677a  vcvt.f32.u32	s15, s15
0802e34c  b869      ldr	r0, [r7, #24]
0802e34e  67ee877a  vmul.f32	s15, s15, s14
0802e352  fdeee77a  vcvt.s32.f32	s15, s15
0802e356  17ee903a  vmov	r3, s15
0802e35a  9842      cmp	r0, r3
0802e35c  3ff414ac  beq.w	#-2008 ; -> 0x0802db88 ; branch_target=0x0802db88
0802e360  7e4b      ldr	r3, [pc, #504] ; [0x0802e55c] = 0x20002eb8
0802e362  c7ed067a  vstr	s15, [r7, #24]
0802e366  0127      movs	r7, #1
0802e368  1f60      str	r7, [r3]
0802e36a  0de4      b	#-2022 ; -> 0x0802db88 ; branch_target=0x0802db88
0802e36c  d1e90003  ldrd	r0, r3, [r1]
0802e370  3128      cmp	r0, #49
0802e372  00f2b680  bhi.w	#364 ; -> 0x0802e4e2 ; branch_target=0x0802e4e2
0802e376  b3f5fa7f  cmp.w	r3, #500
0802e37a  bff463ab  bhs.w	#-2362 ; -> 0x0802da44 ; branch_target=0x0802da44
0802e37e  784b      ldr	r3, [pc, #480] ; [0x0802e560] = 0x20000b20
0802e380  dff810c2  ldr.w	r12, [pc, #528] ; [0x0802e594] = 0x20000920
0802e384  1868      ldr	r0, [r3]
0802e386  774b      ldr	r3, [pc, #476] ; [0x0802e564] = 0x20002e80
0802e388  d3f80080  ldr.w	r8, [r3]
0802e38c  0ceb0013  add.w	r3, r12, r0, lsl #4
0802e390  0001      lsls	r0, r0, #4
0802e392  5f68      ldr	r7, [r3, #4]
0802e394  b8f1010f  cmp.w	r8, #1
0802e398  734b      ldr	r3, [pc, #460] ; [0x0802e568] = 0x20002ef4
0802e39a  d3f800e0  ldr.w	lr, [r3]
0802e39e  07f1ff33  add.w	r3, r7, #4294967295
0802e3a2  4feaa313  asr.w	r3, r3, #6
0802e3a6  03f10103  add.w	r3, r3, #1
0802e3aa  08bf      it	eq
0802e3ac  4346      moveq	r3, r8
0802e3ae  4ff00108  mov.w	r8, #1
0802e3b2  cbf80030  str.w	r3, [r11]
0802e3b6  6d4b      ldr	r3, [pc, #436] ; [0x0802e56c] = 0x20002ed4
0802e3b8  c3f80080  str.w	r8, [r3]
0802e3bc  bef1000f  cmp.w	lr, #0
0802e3c0  00f03a81  beq.w	#628 ; -> 0x0802e638 ; branch_target=0x0802e638
0802e3c4  bef1050f  cmp.w	lr, #5
0802e3c8  00f03681  beq.w	#620 ; -> 0x0802e638 ; branch_target=0x0802e638
0802e3cc  7368      ldr	r3, [r6, #4]
0802e3ce  41f26f30  movw	r0, #4975
0802e3d2  a3f58067  sub.w	r7, r3, #1024
0802e3d6  8742      cmp	r7, r0
0802e3d8  00f26f81  bhi.w	#734 ; -> 0x0802e6ba ; branch_target=0x0802e6ba
0802e3dc  07ee903a  vmov	s15, r3
0802e3e0  dfed5b6a  vldr	s13, [pc, #364] ; [0x0802e550] = 0x3c800000 / f32_bits_interpretation=0.015625
0802e3e4  6248      ldr	r0, [pc, #392] ; [0x0802e570] = 0x200023f4
0802e3e6  b8eee77a  vcvt.f32.s32	s14, s15
0802e3ea  c6ee877a  vdiv.f32	s15, s13, s14
0802e3ee  c0ed007a  vstr	s15, [r0]
0802e3f2  07ee903a  vmov	s15, r3
0802e3f6  9fed587a  vldr	s14, [pc, #352] ; [0x0802e558] = 0x3faaaaaa / f32_bits_interpretation=1.333333254
0802e3fa  524f      ldr	r7, [pc, #328] ; [0x0802e544] = 0x2001348c
0802e3fc  f8ee677a  vcvt.f32.u32	s15, s15
0802e400  f869      ldr	r0, [r7, #28]
0802e402  67ee877a  vmul.f32	s15, s15, s14
0802e406  fdeee77a  vcvt.s32.f32	s15, s15
0802e40a  17ee903a  vmov	r3, s15
0802e40e  9842      cmp	r0, r3
0802e410  3ff418ab  beq.w	#-2512 ; -> 0x0802da44 ; branch_target=0x0802da44
0802e414  514b      ldr	r3, [pc, #324] ; [0x0802e55c] = 0x20002eb8
0802e416  0120      movs	r0, #1
0802e418  c7ed077a  vstr	s15, [r7, #28]
0802e41c  1860      str	r0, [r3]
0802e41e  fff711bb  b.w	#-2526 ; -> 0x0802da44 ; branch_target=0x0802da44
0802e422  d1e90030  ldrd	r3, r0, [r1]
0802e426  3128      cmp	r0, #49
0802e428  3ff60cac  bhi.w	#-2024 ; -> 0x0802dc44 ; branch_target=0x0802dc44
0802e42c  6878      ldrb	r0, [r5, #1]
0802e42e  b3f5fa7f  cmp.w	r3, #500
0802e432  0190      str	r0, [sp, #4]
0802e434  bff4a8ab  bhs.w	#-2224 ; -> 0x0802db88 ; branch_target=0x0802db88
0802e438  47e7      b	#-370 ; -> 0x0802e2ca ; branch_target=0x0802e2ca
0802e43a  0b68      ldr	r3, [r1]
0802e43c  312b      cmp	r3, #49
0802e43e  9ed9      bls	#-196 ; -> 0x0802e37e ; branch_target=0x0802e37e
0802e440  fff700bb  b.w	#-2560 ; -> 0x0802da44 ; branch_target=0x0802da44
0802e444  cde90112  strd	r1, r2, [sp, #4]
0802e448  fef772ff  bl	#-4380 ; -> 0x0802d330 ; branch_target=0x0802d330
0802e44c  494b      ldr	r3, [pc, #292] ; [0x0802e574] = 0x200144d0
0802e44e  1b68      ldr	r3, [r3]
0802e450  012b      cmp	r3, #1
0802e452  dde90112  ldrd	r1, r2, [sp, #4]
0802e456  7ff4f5aa  bne.w	#-2582 ; -> 0x0802da44 ; branch_target=0x0802da44
0802e45a  0023      movs	r3, #0
0802e45c  464f      ldr	r7, [pc, #280] ; [0x0802e578] = 0x20002f50
0802e45e  dff838a1  ldr.w	r10, [pc, #312] ; [0x0802e598] = 0x20002f60
0802e462  c9f80030  str.w	r3, [r9]
0802e466  dff83481  ldr.w	r8, [pc, #308] ; [0x0802e59c] = 0x20002f58
0802e46a  dff834b1  ldr.w	r11, [pc, #308] ; [0x0802e5a0] = 0x58020800
0802e46e  dff83491  ldr.w	r9, [pc, #308] ; [0x0802e5a4] = 0x58020c00
0802e472  0022      movs	r2, #0
0802e474  4021      movs	r1, #64
0802e476  5846      mov	r0, r11
0802e478  07f8012b  strb	r2, [r7], #1
0802e47c  0af8012b  strb	r2, [r10], #1
0802e480  08f8012b  strb	r2, [r8], #1
0802e484  0122      movs	r2, #1
0802e486  f4f7c1ff  bl	#-45182 ; -> 0x0802340c ; branch_target=0x0802340c
0802e48a  0122      movs	r2, #1
0802e48c  8021      movs	r1, #128
0802e48e  5846      mov	r0, r11
0802e490  f4f7bcff  bl	#-45192 ; -> 0x0802340c ; branch_target=0x0802340c
0802e494  0122      movs	r2, #1
0802e496  4ff48051  mov.w	r1, #4096
0802e49a  4846      mov	r0, r9
0802e49c  f4f7b6ff  bl	#-45204 ; -> 0x0802340c ; branch_target=0x0802340c
0802e4a0  0122      movs	r2, #1
0802e4a2  4ff40051  mov.w	r1, #8192
0802e4a6  4846      mov	r0, r9
0802e4a8  f4f7b0ff  bl	#-45216 ; -> 0x0802340c ; branch_target=0x0802340c
0802e4ac  0122      movs	r2, #1
0802e4ae  4ff40041  mov.w	r1, #32768
0802e4b2  3248      ldr	r0, [pc, #200] ; [0x0802e57c] = 0x58020400
0802e4b4  f4f7aaff  bl	#-45228 ; -> 0x0802340c ; branch_target=0x0802340c
0802e4b8  314b      ldr	r3, [pc, #196] ; [0x0802e580] = 0x20002f55
0802e4ba  bb42      cmp	r3, r7
0802e4bc  d9d1      bne	#-78 ; -> 0x0802e472 ; branch_target=0x0802e472
0802e4be  dde90112  ldrd	r1, r2, [sp, #4]
0802e4c2  fff7bfba  b.w	#-2690 ; -> 0x0802da44 ; branch_target=0x0802da44
0802e4c6  2f4a      ldr	r2, [pc, #188] ; [0x0802e584] = 0x20002e7c
0802e4c8  3060      str	r0, [r6]
0802e4ca  1268      ldr	r2, [r2]
0802e4cc  a2eb0802  sub.w	r2, r2, r8
0802e4d0  9209      lsrs	r2, r2, #6
0802e4d2  7a60      str	r2, [r7, #4]
0802e4d4  1d4a      ldr	r2, [pc, #116] ; [0x0802e54c] = 0x20002ed8
0802e4d6  1060      str	r0, [r2]
0802e4d8  74e4      b	#-1816 ; -> 0x0802ddc4 ; branch_target=0x0802ddc4
0802e4da  6b78      ldrb	r3, [r5, #1]
0802e4dc  0193      str	r3, [sp, #4]
0802e4de  fff753bb  b.w	#-2394 ; -> 0x0802db88 ; branch_target=0x0802db88
0802e4e2  333b      subs	r3, #51
0802e4e4  b3f5b56f  cmp.w	r3, #1448
0802e4e8  3ff6acaa  bhi.w	#-2728 ; -> 0x0802da44 ; branch_target=0x0802da44
0802e4ec  1148      ldr	r0, [pc, #68] ; [0x0802e534] = 0x20002430
0802e4ee  0423      movs	r3, #4
0802e4f0  4ff0010c  mov.w	r12, #1
0802e4f4  134f      ldr	r7, [pc, #76] ; [0x0802e544] = 0x2001348c
0802e4f6  2b70      strb	r3, [r5]
0802e4f8  4ff0000e  mov.w	lr, #0
0802e4fc  0368      ldr	r3, [r0]
0802e4fe  0133      adds	r3, #1
0802e500  03f00f03  and	r3, r3, #15
0802e504  0360      str	r3, [r0]
0802e506  40f24160  movw	r0, #1601
0802e50a  0860      str	r0, [r1]
0802e50c  1e48      ldr	r0, [pc, #120] ; [0x0802e588] = 0x20002f08
0802e50e  c0f800c0  str.w	r12, [r0]
0802e512  1e48      ldr	r0, [pc, #120] ; [0x0802e58c] = 0x20002f04
0802e514  c0f800e0  str.w	lr, [r0]
0802e518  b86a      ldr	r0, [r7, #40]
0802e51a  8342      cmp	r3, r0
0802e51c  3ff498aa  beq.w	#-2768 ; -> 0x0802da50 ; branch_target=0x0802da50
0802e520  bb62      str	r3, [r7, #40]
0802e522  054b      ldr	r3, [pc, #20] ; [0x0802e538] = 0x20002438
0802e524  c3f800e0  str.w	lr, [r3]
0802e528  0c4b      ldr	r3, [pc, #48] ; [0x0802e55c] = 0x20002eb8
0802e52a  c3f800c0  str.w	r12, [r3]
0802e52e  fff78fba  b.w	#-2786 ; -> 0x0802da50 ; branch_target=0x0802da50
0802e5a8  cde90112  strd	r1, r2, [sp, #4]
0802e5ac  fef7c0fe  bl	#-4736 ; -> 0x0802d330 ; branch_target=0x0802d330
0802e5b0  0023      movs	r3, #0
0802e5b2  554f      ldr	r7, [pc, #340] ; [0x0802e708] = 0x20002f50
0802e5b4  c9f80030  str.w	r3, [r9]
0802e5b8  dff884b1  ldr.w	r11, [pc, #388] ; [0x0802e740] = 0x20002f60
0802e5bc  dff87ca1  ldr.w	r10, [pc, #380] ; [0x0802e73c] = 0x20002f58
0802e5c0  dff88091  ldr.w	r9, [pc, #384] ; [0x0802e744] = 0x58020800
0802e5c4  dff88081  ldr.w	r8, [pc, #384] ; [0x0802e748] = 0x58020c00
0802e5c8  0022      movs	r2, #0
0802e5ca  4021      movs	r1, #64
0802e5cc  4846      mov	r0, r9
0802e5ce  07f8012b  strb	r2, [r7], #1
0802e5d2  0bf8012b  strb	r2, [r11], #1
0802e5d6  0af8012b  strb	r2, [r10], #1
0802e5da  0122      movs	r2, #1
0802e5dc  f4f716ff  bl	#-45524 ; -> 0x0802340c ; branch_target=0x0802340c
0802e5e0  0122      movs	r2, #1
0802e5e2  8021      movs	r1, #128
0802e5e4  4846      mov	r0, r9
0802e5e6  f4f711ff  bl	#-45534 ; -> 0x0802340c ; branch_target=0x0802340c
0802e5ea  0122      movs	r2, #1
0802e5ec  4ff48051  mov.w	r1, #4096
0802e5f0  4046      mov	r0, r8
0802e5f2  f4f70bff  bl	#-45546 ; -> 0x0802340c ; branch_target=0x0802340c
0802e5f6  0122      movs	r2, #1
0802e5f8  4ff40051  mov.w	r1, #8192
0802e5fc  4046      mov	r0, r8
0802e5fe  f4f705ff  bl	#-45558 ; -> 0x0802340c ; branch_target=0x0802340c
0802e602  0122      movs	r2, #1
0802e604  4ff40041  mov.w	r1, #32768
0802e608  4048      ldr	r0, [pc, #256] ; [0x0802e70c] = 0x58020400
0802e60a  f4f7fffe  bl	#-45570 ; -> 0x0802340c ; branch_target=0x0802340c
0802e60e  404b      ldr	r3, [pc, #256] ; [0x0802e710] = 0x20002f55
0802e610  bb42      cmp	r3, r7
0802e612  d9d1      bne	#-78 ; -> 0x0802e5c8 ; branch_target=0x0802e5c8
0802e614  53e7      b	#-346 ; -> 0x0802e4be ; branch_target=0x0802e4be
0802e616  0222      movs	r2, #2
0802e618  9fe5      b	#-1218 ; -> 0x0802e15a ; branch_target=0x0802e15a
0802e61a  3e4a      ldr	r2, [pc, #248] ; [0x0802e714] = 0x20002ed8
0802e61c  8670      strb	r6, [r0, #2]
0802e61e  3046      mov	r0, r6
0802e620  3d4f      ldr	r7, [pc, #244] ; [0x0802e718] = 0x2001348c
0802e622  ccf80060  str.w	r6, [r12]
0802e626  1660      str	r6, [r2]
0802e628  b7e4      b	#-1682 ; -> 0x0802df9a ; branch_target=0x0802df9a
0802e62a  3c4a      ldr	r2, [pc, #240] ; [0x0802e71c] = 0x20002ed4
0802e62c  3a4f      ldr	r7, [pc, #232] ; [0x0802e718] = 0x2001348c
0802e62e  e070      strb	r0, [r4, #3]
0802e630  ccf80000  str.w	r0, [r12]
0802e634  1060      str	r0, [r2]
0802e636  3be4      b	#-1930 ; -> 0x0802deb0 ; branch_target=0x0802deb0
0802e638  8444      add	r12, r0
0802e63a  3948      ldr	r0, [pc, #228] ; [0x0802e720] = 0x20002434
0802e63c  dcf80830  ldr.w	r3, [r12, #8]
0802e640  03fb07f7  mul	r7, r3, r7
0802e644  0368      ldr	r3, [r0]
0802e646  0133      adds	r3, #1
0802e648  bb42      cmp	r3, r7
0802e64a  0360      str	r3, [r0]
0802e64c  fff6faa9  blt.w	#-3084 ; -> 0x0802da44 ; branch_target=0x0802da44
0802e650  0023      movs	r3, #0
0802e652  0360      str	r3, [r0]
0802e654  fff7f6b9  b.w	#-3092 ; -> 0x0802da44 ; branch_target=0x0802da44
0802e658  e044      add	r8, r12
0802e65a  324f      ldr	r7, [pc, #200] ; [0x0802e724] = 0x20002438
0802e65c  d8f80830  ldr.w	r3, [r8, #8]
0802e660  03fb0efe  mul	lr, r3, lr
0802e664  3b68      ldr	r3, [r7]
0802e666  0133      adds	r3, #1
0802e668  7345      cmp	r3, lr
0802e66a  3b60      str	r3, [r7]
0802e66c  fff68caa  blt.w	#-2792 ; -> 0x0802db88 ; branch_target=0x0802db88
0802e670  0023      movs	r3, #0
0802e672  3b60      str	r3, [r7]
0802e674  fff788ba  b.w	#-2800 ; -> 0x0802db88 ; branch_target=0x0802db88
0802e678  2b4b      ldr	r3, [pc, #172] ; [0x0802e728] = 0x20000b20
0802e67a  1868      ldr	r0, [r3]
0802e67c  cde90112  strd	r1, r2, [sp, #4]
0802e680  04f0bcfe  bl	#19832 ; -> 0x080333fc ; branch_target=0x080333fc
0802e684  0120      movs	r0, #1
0802e686  05f037fa  bl	#21614 ; -> 0x08033af8 ; branch_target=0x08033af8
0802e68a  40f24163  movw	r3, #1601
0802e68e  0199      ldr	r1, [sp, #4]
0802e690  029a      ldr	r2, [sp, #8]
0802e692  4b60      str	r3, [r1, #4]
0802e694  fff726bb  b.w	#-2484 ; -> 0x0802dce4 ; branch_target=0x0802dce4
0802e698  244b      ldr	r3, [pc, #144] ; [0x0802e72c] = 0x20002430
0802e69a  1868      ldr	r0, [r3]
0802e69c  cde90112  strd	r1, r2, [sp, #4]
0802e6a0  04f090fe  bl	#19744 ; -> 0x080333c4 ; branch_target=0x080333c4
0802e6a4  0020      movs	r0, #0
0802e6a6  05f027fa  bl	#21582 ; -> 0x08033af8 ; branch_target=0x08033af8
0802e6aa  40f24163  movw	r3, #1601
0802e6ae  0199      ldr	r1, [sp, #4]
0802e6b0  6078      ldrb	r0, [r4, #1]
0802e6b2  029a      ldr	r2, [sp, #8]
0802e6b4  0b60      str	r3, [r1]
0802e6b6  fff79ab9  b.w	#-3276 ; -> 0x0802d9ee ; branch_target=0x0802d9ee
0802e6ba  5f1e      subs	r7, r3, #1
0802e6bc  40f2fe30  movw	r0, #1022
0802e6c0  8742      cmp	r7, r0
0802e6c2  3ff696ae  bhi.w	#-724 ; -> 0x0802e3f2 ; branch_target=0x0802e3f2
0802e6c6  1a48      ldr	r0, [pc, #104] ; [0x0802e730] = 0x60000000
0802e6c8  00eb8300  add.w	r0, r0, r3, lsl #2
0802e6cc  0768      ldr	r7, [r0]
0802e6ce  1948      ldr	r0, [pc, #100] ; [0x0802e734] = 0x200023f4
0802e6d0  0760      str	r7, [r0]
0802e6d2  8ee6      b	#-740 ; -> 0x0802e3f2 ; branch_target=0x0802e3f2
0802e6d4  03f1ff3c  add.w	r12, r3, #4294967295
0802e6d8  40f2fe37  movw	r7, #1022
0802e6dc  bc45      cmp	r12, r7
0802e6de  3ff62eae  bhi.w	#-932 ; -> 0x0802e33e ; branch_target=0x0802e33e
0802e6e2  134f      ldr	r7, [pc, #76] ; [0x0802e730] = 0x60000000
0802e6e4  07eb8307  add.w	r7, r7, r3, lsl #2
0802e6e8  d7f800c0  ldr.w	r12, [r7]
0802e6ec  124f      ldr	r7, [pc, #72] ; [0x0802e738] = 0x200023f8
0802e6ee  c7f800c0  str.w	r12, [r7]
0802e6f2  24e6      b	#-952 ; -> 0x0802e33e ; branch_target=0x0802e33e
0802e6f4  114a      ldr	r2, [pc, #68] ; [0x0802e73c] = 0x20002f58
0802e6f6  b8f1010f  cmp.w	r8, #1
0802e6fa  1070      strb	r0, [r2]
0802e6fc  7ff4e6ac  bne.w	#-1588 ; -> 0x0802e0cc ; branch_target=0x0802e0cc
0802e700  ccf80000  str.w	r0, [r12]
0802e704  e2e4      b	#-1596 ; -> 0x0802e0cc ; branch_target=0x0802e0cc
