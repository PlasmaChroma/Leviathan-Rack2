; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08025f04  2de9f047  push.w	{r4, r5, r6, r7, r8, r9, r10, lr}
08025f08  0546      mov	r5, r0
08025f0a  86b0      sub	sp, #24
08025f0c  0f46      mov	r7, r1
08025f0e  faf745fa  bl	#-23414 ; -> 0x0802039c ; branch_target=0x0802039c
08025f12  0821      movs	r1, #8
08025f14  0646      mov	r6, r0
08025f16  2868      ldr	r0, [r5]
08025f18  02f0f0fa  bl	#9696 ; -> 0x080284fc ; branch_target=0x080284fc
08025f1c  0446      mov	r4, r0
08025f1e  18b1      cbz	r0, #6 ; -> 0x08025f28 ; branch_target=0x08025f28
08025f20  2046      mov	r0, r4
08025f22  06b0      add	sp, #24
08025f24  bde8f087  pop.w	{r4, r5, r6, r7, r8, r9, r10, pc}
08025f28  696c      ldr	r1, [r5, #68]
08025f2a  2868      ldr	r0, [r5]
08025f2c  0904      lsls	r1, r1, #16
08025f2e  03f05df8  bl	#12474 ; -> 0x08028fec ; branch_target=0x08028fec
08025f32  0446      mov	r4, r0
08025f34  0028      cmp	r0, #0
08025f36  f3d1      bne	#-26 ; -> 0x08025f20 ; branch_target=0x08025f20
08025f38  4ff0ff32  mov.w	r2, #4294967295
08025f3c  0823      movs	r3, #8
08025f3e  6946      mov	r1, sp
08025f40  2868      ldr	r0, [r5]
08025f42  cde90023  strd	r2, r3, [sp]
08025f46  3022      movs	r2, #48
08025f48  0223      movs	r3, #2
08025f4a  cde90223  strd	r2, r3, [sp, #8]
08025f4e  0022      movs	r2, #0
08025f50  0123      movs	r3, #1
08025f52  cde90423  strd	r2, r3, [sp, #16]
08025f56  02f0bbfa  bl	#9590 ; -> 0x080284d0 ; branch_target=0x080284d0
08025f5a  2868      ldr	r0, [r5]
08025f5c  03f0daf9  bl	#13236 ; -> 0x08029314 ; branch_target=0x08029314
08025f60  0446      mov	r4, r0
08025f62  0028      cmp	r0, #0
08025f64  dcd1      bne	#-72 ; -> 0x08025f20 ; branch_target=0x08025f20
08025f66  8246      mov	r10, r0
08025f68  8146      mov	r9, r0
08025f6a  40f22a58  movw	r8, #1322
08025f6e  04e0      b	#8 ; -> 0x08025f7a ; branch_target=0x08025f7a
08025f70  faf714fa  bl	#-23512 ; -> 0x0802039c ; branch_target=0x0802039c
08025f74  831b      subs	r3, r0, r6
08025f76  0133      adds	r3, #1
08025f78  24d0      beq	#72 ; -> 0x08025fc4 ; branch_target=0x08025fc4
08025f7a  2868      ldr	r0, [r5]
08025f7c  436b      ldr	r3, [r0, #52]
08025f7e  13ea080f  tst.w	r3, r8
08025f82  436b      ldr	r3, [r0, #52]
08025f84  0cd1      bne	#24 ; -> 0x08025fa0 ; branch_target=0x08025fa0
08025f86  1903      lsls	r1, r3, #12
08025f88  f2d4      bmi	#-28 ; -> 0x08025f70 ; branch_target=0x08025f70
08025f8a  002c      cmp	r4, #0
08025f8c  f0d1      bne	#-32 ; -> 0x08025f70 ; branch_target=0x08025f70
08025f8e  02f085fa  bl	#9482 ; -> 0x0802849c ; branch_target=0x0802849c
08025f92  8146      mov	r9, r0
08025f94  2868      ldr	r0, [r5]
08025f96  0124      movs	r4, #1
08025f98  02f080fa  bl	#9472 ; -> 0x0802849c ; branch_target=0x0802849c
08025f9c  8246      mov	r10, r0
08025f9e  e7e7      b	#-50 ; -> 0x08025f70 ; branch_target=0x08025f70
08025fa0  1a07      lsls	r2, r3, #28
08025fa2  16d4      bmi	#44 ; -> 0x08025fd2 ; branch_target=0x08025fd2
08025fa4  436b      ldr	r3, [r0, #52]
08025fa6  9b07      lsls	r3, r3, #30
08025fa8  0fd4      bmi	#30 ; -> 0x08025fca ; branch_target=0x08025fca
08025faa  446b      ldr	r4, [r0, #52]
08025fac  14f02004  ands	r4, r4, #32
08025fb0  13d1      bne	#38 ; -> 0x08025fda ; branch_target=0x08025fda
08025fb2  9afa8af2  rev.w	r2, r10
08025fb6  99fa89f3  rev.w	r3, r9
08025fba  0a49      ldr	r1, [pc, #40] ; [0x08025fe4] = 0x18000f3a
08025fbc  8163      str	r1, [r0, #56]
08025fbe  c7e90023  strd	r2, r3, [r7]
08025fc2  ade7      b	#-166 ; -> 0x08025f20 ; branch_target=0x08025f20
08025fc4  4ff00044  mov.w	r4, #2147483648
08025fc8  aae7      b	#-172 ; -> 0x08025f20 ; branch_target=0x08025f20
08025fca  0223      movs	r3, #2
08025fcc  1c46      mov	r4, r3
08025fce  8363      str	r3, [r0, #56]
08025fd0  a6e7      b	#-180 ; -> 0x08025f20 ; branch_target=0x08025f20
08025fd2  0823      movs	r3, #8
08025fd4  1c46      mov	r4, r3
08025fd6  8363      str	r3, [r0, #56]
08025fd8  a2e7      b	#-188 ; -> 0x08025f20 ; branch_target=0x08025f20
08025fda  2023      movs	r3, #32
08025fdc  1c46      mov	r4, r3
08025fde  8363      str	r3, [r0, #56]
08025fe0  9ee7      b	#-196 ; -> 0x08025f20 ; branch_target=0x08025f20
