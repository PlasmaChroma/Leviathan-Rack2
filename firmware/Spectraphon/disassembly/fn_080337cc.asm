; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
080337cc  2de9f04f  push.w	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
080337d0  0446      mov	r4, r0
080337d2  91b0      sub	sp, #68
080337d4  0d46      mov	r5, r1
080337d6  0021      movs	r1, #0
080337d8  1646      mov	r6, r2
080337da  f8f7b3f8  bl	#-32410 ; -> 0x0802b944 ; branch_target=0x0802b944
080337de  0c22      movs	r2, #12
080337e0  07a9      add	r1, sp, #28
080337e2  2046      mov	r0, r4
080337e4  04ab      add	r3, sp, #16
080337e6  f7f73dfd  bl	#-34182 ; -> 0x0802b264 ; branch_target=0x0802b264
080337ea  a94a      ldr	r2, [pc, #676] ; [0x08033a90] = 0x46464952 / f32_bits_interpretation=12690.33008
080337ec  0799      ldr	r1, [sp, #28]
080337ee  8346      mov	r11, r0
080337f0  9142      cmp	r1, r2
080337f2  06d1      bne	#12 ; -> 0x08033802 ; branch_target=0x08033802
080337f4  a2f57002  sub.w	r2, r2, #15728640
080337f8  0999      ldr	r1, [sp, #36]
080337fa  a2f2fb72  subw	r2, r2, #2043
080337fe  9142      cmp	r1, r2
08033800  03d0      beq	#6 ; -> 0x0803380a ; branch_target=0x0803380a
08033802  5846      mov	r0, r11
08033804  11b0      add	sp, #68
08033806  bde8f08f  pop.w	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
0803380a  e768      ldr	r7, [r4, #12]
0803380c  2046      mov	r0, r4
0803380e  3946      mov	r1, r7
08033810  f8f798f8  bl	#-32464 ; -> 0x0802b944 ; branch_target=0x0802b944
08033814  8346      mov	r11, r0
08033816  0028      cmp	r0, #0
08033818  f3d1      bne	#-26 ; -> 0x08033802 ; branch_target=0x08033802
0803381a  083f      subs	r7, #8
0803381c  0146      mov	r1, r0
0803381e  2046      mov	r0, r4
08033820  0897      str	r7, [sp, #32]
08033822  f8f78ff8  bl	#-32482 ; -> 0x0802b944 ; branch_target=0x0802b944
08033826  8346      mov	r11, r0
08033828  0028      cmp	r0, #0
0803382a  ead1      bne	#-44 ; -> 0x08033802 ; branch_target=0x08033802
0803382c  0c22      movs	r2, #12
0803382e  07a9      add	r1, sp, #28
08033830  2046      mov	r0, r4
08033832  0deb0203  add.w	r3, sp, r2
08033836  f7f749fe  bl	#-33646 ; -> 0x0802b4cc ; branch_target=0x0802b4cc
0803383a  8346      mov	r11, r0
0803383c  0028      cmp	r0, #0
0803383e  e0d1      bne	#-64 ; -> 0x08033802 ; branch_target=0x08033802
08033840  06f00103  and	r3, r6, #1
08033844  0096      str	r6, [sp]
08033846  0c27      movs	r7, #12
08033848  dff86082  ldr.w	r8, [pc, #608] ; [0x08033aac] = 0x20746d66
0803384c  8246      mov	r10, r0
0803384e  0646      mov	r6, r0
08033850  0193      str	r3, [sp, #4]
08033852  3946      mov	r1, r7
08033854  2046      mov	r0, r4
08033856  f8f775f8  bl	#-32534 ; -> 0x0802b944 ; branch_target=0x0802b944
0803385a  8346      mov	r11, r0
0803385c  0028      cmp	r0, #0
0803385e  d0d1      bne	#-96 ; -> 0x08033802 ; branch_target=0x08033802
08033860  04ab      add	r3, sp, #16
08033862  0822      movs	r2, #8
08033864  2046      mov	r0, r4
08033866  05a9      add	r1, sp, #20
08033868  f7f7fcfc  bl	#-34312 ; -> 0x0802b264 ; branch_target=0x0802b264
0803386c  8346      mov	r11, r0
0803386e  0028      cmp	r0, #0
08033870  c7d1      bne	#-114 ; -> 0x08033802 ; branch_target=0x08033802
08033872  059b      ldr	r3, [sp, #20]
08033874  4345      cmp	r3, r8
08033876  1dd0      beq	#58 ; -> 0x080338b4 ; branch_target=0x080338b4
08033878  864a      ldr	r2, [pc, #536] ; [0x08033a94] = 0x61746164
0803387a  9342      cmp	r3, r2
0803387c  00f08f80  beq.w	#286 ; -> 0x0803399e ; branch_target=0x0803399e
08033880  069b      ldr	r3, [sp, #24]
08033882  86f00102  eor	r2, r6, #1
08033886  0833      adds	r3, #8
08033888  1f44      add	r7, r3
0803388a  8af00103  eor	r3, r10, #1
0803388e  1343      orrs	r3, r2
08033890  fa07      lsls	r2, r7, #31
08033892  48bf      it	mi
08033894  0137      addmi	r7, #1
08033896  b946      mov	r9, r7
08033898  002b      cmp	r3, #0
0803389a  dad1      bne	#-76 ; -> 0x08033852 ; branch_target=0x08033852
0803389c  4946      mov	r1, r9
0803389e  2046      mov	r0, r4
080338a0  f8f750f8  bl	#-32608 ; -> 0x0802b944 ; branch_target=0x0802b944
080338a4  8346      mov	r11, r0
080338a6  2046      mov	r0, r4
080338a8  f8f76efb  bl	#-31012 ; -> 0x0802bf88 ; branch_target=0x0802bf88
080338ac  2046      mov	r0, r4
080338ae  f7f771ff  bl	#-33054 ; -> 0x0802b794 ; branch_target=0x0802b794
080338b2  a6e7      b	#-180 ; -> 0x08033802 ; branch_target=0x08033802
080338b4  069b      ldr	r3, [sp, #24]
080338b6  cdf82880  str.w	r8, [sp, #40]
080338ba  0b93      str	r3, [sp, #44]
080338bc  764b      ldr	r3, [pc, #472] ; [0x08033a98] = 0x6d753038
080338be  9d42      cmp	r5, r3
080338c0  00f0df80  beq.w	#446 ; -> 0x08033a82 ; branch_target=0x08033a82
080338c4  54d8      bhi	#168 ; -> 0x08033970 ; branch_target=0x08033970
080338c6  a3f18263  sub.w	r3, r3, #68157440
080338ca  a3f53333  sub.w	r3, r3, #183296
080338ce  a3f53573  sub.w	r3, r3, #724
080338d2  9d42      cmp	r5, r3
080338d4  00f0cb80  beq.w	#406 ; -> 0x08033a6e ; branch_target=0x08033a6e
080338d8  32d9      bls	#100 ; -> 0x08033940 ; branch_target=0x08033940
080338da  704b      ldr	r3, [pc, #448] ; [0x08033a9c] = 0x696d6164
080338dc  9d42      cmp	r5, r3
080338de  00f0c280  beq.w	#388 ; -> 0x08033a66 ; branch_target=0x08033a66
080338e2  03f18063  add.w	r3, r3, #67108864
080338e6  03f5c023  add.w	r3, r3, #393216
080338ea  9d42      cmp	r5, r3
080338ec  02d1      bne	#4 ; -> 0x080338f4 ; branch_target=0x080338f4
080338ee  0223      movs	r3, #2
080338f0  adf83030  strh.w	r3, [sp, #48]
080338f4  0123      movs	r3, #1
080338f6  4bf68031  movw	r1, #48000
080338fa  2046      mov	r0, r4
080338fc  adf83230  strh.w	r3, [sp, #50]
08033900  674b      ldr	r3, [pc, #412] ; [0x08033aa0] = 0x0002ee00
08033902  cde90d13  strd	r1, r3, [sp, #52]
08033906  0423      movs	r3, #4
08033908  3946      mov	r1, r7
0803390a  adf83c30  strh.w	r3, [sp, #60]
0803390e  f8f719f8  bl	#-32718 ; -> 0x0802b944 ; branch_target=0x0802b944
08033912  0028      cmp	r0, #0
08033914  40f0ba80  bne.w	#372 ; -> 0x08033a8c ; branch_target=0x08033a8c
08033918  03ab      add	r3, sp, #12
0803391a  1622      movs	r2, #22
0803391c  0aa9      add	r1, sp, #40
0803391e  2046      mov	r0, r4
08033920  f7f7d4fd  bl	#-33880 ; -> 0x0802b4cc ; branch_target=0x0802b4cc
08033924  0028      cmp	r0, #0
08033926  40f0b180  bne.w	#354 ; -> 0x08033a8c ; branch_target=0x08033a8c
0803392a  bbf1010f  cmp.w	r11, #1
0803392e  5ad0      beq	#180 ; -> 0x080339e6 ; branch_target=0x080339e6
08033930  069b      ldr	r3, [sp, #24]
08033932  0126      movs	r6, #1
08033934  0833      adds	r3, #8
08033936  1f44      add	r7, r3
08033938  8af00103  eor	r3, r10, #1
0803393c  b946      mov	r9, r7
0803393e  abe7      b	#-170 ; -> 0x08033898 ; branch_target=0x08033898
08033940  03f17843  add.w	r3, r3, #4160749568
08033944  03f51c23  add.w	r3, r3, #638976
08033948  03f6d463  addw	r3, r3, #3796
0803394c  9d42      cmp	r5, r3
0803394e  00f09380  beq.w	#294 ; -> 0x08033a78 ; branch_target=0x08033a78
08033952  03f1a063  add.w	r3, r3, #83886080
08033956  03f2fa23  addw	r3, r3, #762
0803395a  9d42      cmp	r5, r3
0803395c  cad1      bne	#-108 ; -> 0x080338f4 ; branch_target=0x080338f4
0803395e  2022      movs	r2, #32
08033960  0323      movs	r3, #3
08033962  4ff0010b  mov.w	r11, #1
08033966  adf80a20  strh.w	r2, [sp, #10]
0803396a  adf83030  strh.w	r3, [sp, #48]
0803396e  c1e7      b	#-126 ; -> 0x080338f4 ; branch_target=0x080338f4
08033970  4c4b      ldr	r3, [pc, #304] ; [0x08033aa4] = 0x73693332
08033972  9d42      cmp	r5, r3
08033974  34d0      beq	#104 ; -> 0x080339e0 ; branch_target=0x080339e0
08033976  05d9      bls	#10 ; -> 0x08033984 ; branch_target=0x08033984
08033978  4b4b      ldr	r3, [pc, #300] ; [0x08033aa8] = 0x75733038
0803397a  9d42      cmp	r5, r3
0803397c  bad1      bne	#-140 ; -> 0x080338f4 ; branch_target=0x080338f4
0803397e  0822      movs	r2, #8
08033980  0123      movs	r3, #1
08033982  eee7      b	#-36 ; -> 0x08033962 ; branch_target=0x08033962
08033984  a3f5fe73  sub.w	r3, r3, #508
08033988  9d42      cmp	r5, r3
0803398a  05d0      beq	#10 ; -> 0x08033998 ; branch_target=0x08033998
0803398c  fe33      adds	r3, #254
0803398e  9d42      cmp	r5, r3
08033990  b0d1      bne	#-160 ; -> 0x080338f4 ; branch_target=0x080338f4
08033992  1822      movs	r2, #24
08033994  0123      movs	r3, #1
08033996  e4e7      b	#-56 ; -> 0x08033962 ; branch_target=0x08033962
08033998  1022      movs	r2, #16
0803399a  0123      movs	r3, #1
0803399c  e1e7      b	#-62 ; -> 0x08033962 ; branch_target=0x08033962
0803399e  009b      ldr	r3, [sp]
080339a0  3946      mov	r1, r7
080339a2  2046      mov	r0, r4
080339a4  0693      str	r3, [sp, #24]
080339a6  f7f7cdff  bl	#-32870 ; -> 0x0802b944 ; branch_target=0x0802b944
080339aa  8346      mov	r11, r0
080339ac  0028      cmp	r0, #0
080339ae  7ff428af  bne.w	#-432 ; -> 0x08033802 ; branch_target=0x08033802
080339b2  03aa      add	r2, sp, #12
080339b4  05a9      add	r1, sp, #20
080339b6  2046      mov	r0, r4
080339b8  1346      mov	r3, r2
080339ba  0822      movs	r2, #8
080339bc  f7f786fd  bl	#-34036 ; -> 0x0802b4cc ; branch_target=0x0802b4cc
080339c0  8346      mov	r11, r0
080339c2  0028      cmp	r0, #0
080339c4  7ff41daf  bne.w	#-454 ; -> 0x08033802 ; branch_target=0x08033802
080339c8  009b      ldr	r3, [sp]
080339ca  1f44      add	r7, r3
080339cc  019b      ldr	r3, [sp, #4]
080339ce  07f10809  add.w	r9, r7, #8
080339d2  abb9      cbnz	r3, #42 ; -> 0x08033a00 ; branch_target=0x08033a00
080339d4  4f46      mov	r7, r9
080339d6  86f00103  eor	r3, r6, #1
080339da  4ff0010a  mov.w	r10, #1
080339de  5be7      b	#-330 ; -> 0x08033898 ; branch_target=0x08033898
080339e0  2022      movs	r2, #32
080339e2  0123      movs	r3, #1
080339e4  bde7      b	#-134 ; -> 0x08033962 ; branch_target=0x08033962
080339e6  07f11601  add.w	r1, r7, #22
080339ea  2046      mov	r0, r4
080339ec  f7f7aaff  bl	#-32940 ; -> 0x0802b944 ; branch_target=0x0802b944
080339f0  03ab      add	r3, sp, #12
080339f2  0222      movs	r2, #2
080339f4  0df10a01  add.w	r1, sp, #10
080339f8  2046      mov	r0, r4
080339fa  f7f767fd  bl	#-34098 ; -> 0x0802b4cc ; branch_target=0x0802b4cc
080339fe  97e7      b	#-210 ; -> 0x08033930 ; branch_target=0x08033930
08033a00  4946      mov	r1, r9
08033a02  8df80900  strb.w	r0, [sp, #9]
08033a06  2046      mov	r0, r4
08033a08  0937      adds	r7, #9
08033a0a  f7f79bff  bl	#-32970 ; -> 0x0802b944 ; branch_target=0x0802b944
08033a0e  03ab      add	r3, sp, #12
08033a10  0122      movs	r2, #1
08033a12  0df10901  add.w	r1, sp, #9
08033a16  2046      mov	r0, r4
08033a18  b946      mov	r9, r7
08033a1a  f7f757fd  bl	#-34130 ; -> 0x0802b4cc ; branch_target=0x0802b4cc
08033a1e  d4f80ca0  ldr.w	r10, [r4, #12]
08033a22  2046      mov	r0, r4
08033a24  5146      mov	r1, r10
08033a26  f7f78dff  bl	#-32998 ; -> 0x0802b944 ; branch_target=0x0802b944
08033a2a  8346      mov	r11, r0
08033a2c  0028      cmp	r0, #0
08033a2e  7ff4e8ae  bne.w	#-560 ; -> 0x08033802 ; branch_target=0x08033802
08033a32  aaf10803  sub.w	r3, r10, #8
08033a36  0146      mov	r1, r0
08033a38  2046      mov	r0, r4
08033a3a  0893      str	r3, [sp, #32]
08033a3c  f7f782ff  bl	#-33020 ; -> 0x0802b944 ; branch_target=0x0802b944
08033a40  8346      mov	r11, r0
08033a42  0028      cmp	r0, #0
08033a44  7ff4ddae  bne.w	#-582 ; -> 0x08033802 ; branch_target=0x08033802
08033a48  03ab      add	r3, sp, #12
08033a4a  0c22      movs	r2, #12
08033a4c  07a9      add	r1, sp, #28
08033a4e  2046      mov	r0, r4
08033a50  f7f73cfd  bl	#-34184 ; -> 0x0802b4cc ; branch_target=0x0802b4cc
08033a54  8346      mov	r11, r0
08033a56  0028      cmp	r0, #0
08033a58  7ff4d3ae  bne.w	#-602 ; -> 0x08033802 ; branch_target=0x08033802
08033a5c  86f00103  eor	r3, r6, #1
08033a60  4ff0010a  mov.w	r10, #1
08033a64  18e7      b	#-464 ; -> 0x08033898 ; branch_target=0x08033898
08033a66  1123      movs	r3, #17
08033a68  adf83030  strh.w	r3, [sp, #48]
08033a6c  42e7      b	#-380 ; -> 0x080338f4 ; branch_target=0x080338f4
08033a6e  40f20313  movw	r3, #259
08033a72  adf83030  strh.w	r3, [sp, #48]
08033a76  3de7      b	#-390 ; -> 0x080338f4 ; branch_target=0x080338f4
08033a78  4ff48173  mov.w	r3, #258
08033a7c  adf83030  strh.w	r3, [sp, #48]
08033a80  38e7      b	#-400 ; -> 0x080338f4 ; branch_target=0x080338f4
08033a82  40f20113  movw	r3, #257
08033a86  adf83030  strh.w	r3, [sp, #48]
08033a8a  33e7      b	#-410 ; -> 0x080338f4 ; branch_target=0x080338f4
08033a8c  8346      mov	r11, r0
08033a8e  b8e6      b	#-656 ; -> 0x08033802 ; branch_target=0x08033802
