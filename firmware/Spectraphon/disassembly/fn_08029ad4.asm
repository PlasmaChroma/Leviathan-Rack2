; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08029ad4  036a      ldr	r3, [r0, #32]
08029ad6  10b5      push	{r4, lr}
08029ad8  1a78      ldrb	r2, [r3]
08029ada  202a      cmp	r2, #32
08029adc  00f0a480  beq.w	#328 ; -> 0x08029c28 ; branch_target=0x08029c28
08029ae0  052a      cmp	r2, #5
08029ae2  4ff0010e  mov.w	lr, #1
08029ae6  08bf      it	eq
08029ae8  e522      moveq	r2, #229
08029aea  4a72      strb	r2, [r1, #9]
08029aec  036a      ldr	r3, [r0, #32]
08029aee  5a78      ldrb	r2, [r3, #1]
08029af0  202a      cmp	r2, #32
08029af2  00f09f80  beq.w	#318 ; -> 0x08029c34 ; branch_target=0x08029c34
08029af6  052a      cmp	r2, #5
08029af8  0ef1010c  add.w	r12, lr, #1
08029afc  8e44      add	lr, r1
08029afe  08bf      it	eq
08029b00  e522      moveq	r2, #229
08029b02  8ef80920  strb.w	r2, [lr, #9]
08029b06  036a      ldr	r3, [r0, #32]
08029b08  9a78      ldrb	r2, [r3, #2]
08029b0a  202a      cmp	r2, #32
08029b0c  00f09780  beq.w	#302 ; -> 0x08029c3e ; branch_target=0x08029c3e
08029b10  052a      cmp	r2, #5
08029b12  0cf1010e  add.w	lr, r12, #1
08029b16  8c44      add	r12, r1
08029b18  08bf      it	eq
08029b1a  e522      moveq	r2, #229
08029b1c  8cf80920  strb.w	r2, [r12, #9]
08029b20  036a      ldr	r3, [r0, #32]
08029b22  da78      ldrb	r2, [r3, #3]
08029b24  202a      cmp	r2, #32
08029b26  00f08f80  beq.w	#286 ; -> 0x08029c48 ; branch_target=0x08029c48
08029b2a  052a      cmp	r2, #5
08029b2c  0ef1010c  add.w	r12, lr, #1
08029b30  8e44      add	lr, r1
08029b32  08bf      it	eq
08029b34  e522      moveq	r2, #229
08029b36  8ef80920  strb.w	r2, [lr, #9]
08029b3a  036a      ldr	r3, [r0, #32]
08029b3c  1a79      ldrb	r2, [r3, #4]
08029b3e  202a      cmp	r2, #32
08029b40  00f08780  beq.w	#270 ; -> 0x08029c52 ; branch_target=0x08029c52
08029b44  052a      cmp	r2, #5
08029b46  0cf1010e  add.w	lr, r12, #1
08029b4a  8c44      add	r12, r1
08029b4c  08bf      it	eq
08029b4e  e522      moveq	r2, #229
08029b50  8cf80920  strb.w	r2, [r12, #9]
08029b54  036a      ldr	r3, [r0, #32]
08029b56  5a79      ldrb	r2, [r3, #5]
08029b58  202a      cmp	r2, #32
08029b5a  7ed0      beq	#252 ; -> 0x08029c5a ; branch_target=0x08029c5a
08029b5c  052a      cmp	r2, #5
08029b5e  0ef1010c  add.w	r12, lr, #1
08029b62  8e44      add	lr, r1
08029b64  08bf      it	eq
08029b66  e522      moveq	r2, #229
08029b68  8ef80920  strb.w	r2, [lr, #9]
08029b6c  036a      ldr	r3, [r0, #32]
08029b6e  9a79      ldrb	r2, [r3, #6]
08029b70  202a      cmp	r2, #32
08029b72  76d0      beq	#236 ; -> 0x08029c62 ; branch_target=0x08029c62
08029b74  052a      cmp	r2, #5
08029b76  0cf1010e  add.w	lr, r12, #1
08029b7a  8c44      add	r12, r1
08029b7c  08bf      it	eq
08029b7e  e522      moveq	r2, #229
08029b80  8cf80920  strb.w	r2, [r12, #9]
08029b84  036a      ldr	r3, [r0, #32]
08029b86  93f807c0  ldrb.w	r12, [r3, #7]
08029b8a  bcf1200f  cmp.w	r12, #32
08029b8e  6ed0      beq	#220 ; -> 0x08029c6e ; branch_target=0x08029c6e
08029b90  bcf1050f  cmp.w	r12, #5
08029b94  0ef10102  add.w	r2, lr, #1
08029b98  8e44      add	lr, r1
08029b9a  08bf      it	eq
08029b9c  4ff0e50c  moveq.w	r12, #229
08029ba0  8ef809c0  strb.w	r12, [lr, #9]
08029ba4  036a      ldr	r3, [r0, #32]
08029ba6  93f808c0  ldrb.w	r12, [r3, #8]
08029baa  bcf1200f  cmp.w	r12, #32
08029bae  10d0      beq	#32 ; -> 0x08029bd2 ; branch_target=0x08029bd2
08029bb0  531c      adds	r3, r2, #1
08029bb2  bcf1050f  cmp.w	r12, #5
08029bb6  01eb020e  add.w	lr, r1, r2
08029bba  4ff02e04  mov.w	r4, #46
08029bbe  0b44      add	r3, r1
08029bc0  08bf      it	eq
08029bc2  4ff0e50c  moveq.w	r12, #229
08029bc6  8ef80940  strb.w	r4, [lr, #9]
08029bca  0232      adds	r2, #2
08029bcc  83f809c0  strb.w	r12, [r3, #9]
08029bd0  036a      ldr	r3, [r0, #32]
08029bd2  93f809c0  ldrb.w	r12, [r3, #9]
08029bd6  bcf1200f  cmp.w	r12, #32
08029bda  4cd0      beq	#152 ; -> 0x08029c76 ; branch_target=0x08029c76
08029bdc  bcf1050f  cmp.w	r12, #5
08029be0  02f1010e  add.w	lr, r2, #1
08029be4  0a44      add	r2, r1
08029be6  08bf      it	eq
08029be8  4ff0e50c  moveq.w	r12, #229
08029bec  82f809c0  strb.w	r12, [r2, #9]
08029bf0  036a      ldr	r3, [r0, #32]
08029bf2  9b7a      ldrb	r3, [r3, #10]
08029bf4  202b      cmp	r3, #32
08029bf6  3cd0      beq	#120 ; -> 0x08029c72 ; branch_target=0x08029c72
08029bf8  052b      cmp	r3, #5
08029bfa  0ef10102  add.w	r2, lr, #1
08029bfe  8e44      add	lr, r1
08029c00  08bf      it	eq
08029c02  e523      moveq	r3, #229
08029c04  8ef80930  strb.w	r3, [lr, #9]
08029c08  8b18      adds	r3, r1, r2
08029c0a  0022      movs	r2, #0
08029c0c  5a72      strb	r2, [r3, #9]
08029c0e  036a      ldr	r3, [r0, #32]
08029c10  db7a      ldrb	r3, [r3, #11]
08029c12  0b72      strb	r3, [r1, #8]
08029c14  036a      ldr	r3, [r0, #32]
08029c16  db69      ldr	r3, [r3, #28]
08029c18  0b60      str	r3, [r1]
08029c1a  036a      ldr	r3, [r0, #32]
08029c1c  d3f81630  ldr.w	r3, [r3, #22]
08029c20  1a0c      lsrs	r2, r3, #16
08029c22  cb80      strh	r3, [r1, #6]
08029c24  8a80      strh	r2, [r1, #4]
08029c26  10bd      pop	{r4, pc}
08029c28  5a78      ldrb	r2, [r3, #1]
08029c2a  4ff0000e  mov.w	lr, #0
08029c2e  202a      cmp	r2, #32
08029c30  7ff461af  bne.w	#-318 ; -> 0x08029af6 ; branch_target=0x08029af6
08029c34  9a78      ldrb	r2, [r3, #2]
08029c36  f446      mov	r12, lr
08029c38  202a      cmp	r2, #32
08029c3a  7ff469af  bne.w	#-302 ; -> 0x08029b10 ; branch_target=0x08029b10
08029c3e  da78      ldrb	r2, [r3, #3]
08029c40  e646      mov	lr, r12
08029c42  202a      cmp	r2, #32
08029c44  7ff471af  bne.w	#-286 ; -> 0x08029b2a ; branch_target=0x08029b2a
08029c48  1a79      ldrb	r2, [r3, #4]
08029c4a  f446      mov	r12, lr
08029c4c  202a      cmp	r2, #32
08029c4e  7ff479af  bne.w	#-270 ; -> 0x08029b44 ; branch_target=0x08029b44
08029c52  5a79      ldrb	r2, [r3, #5]
08029c54  e646      mov	lr, r12
08029c56  202a      cmp	r2, #32
08029c58  80d1      bne	#-256 ; -> 0x08029b5c ; branch_target=0x08029b5c
08029c5a  9a79      ldrb	r2, [r3, #6]
08029c5c  f446      mov	r12, lr
08029c5e  202a      cmp	r2, #32
08029c60  88d1      bne	#-240 ; -> 0x08029b74 ; branch_target=0x08029b74
08029c62  e646      mov	lr, r12
08029c64  93f807c0  ldrb.w	r12, [r3, #7]
08029c68  bcf1200f  cmp.w	r12, #32
08029c6c  90d1      bne	#-224 ; -> 0x08029b90 ; branch_target=0x08029b90
08029c6e  7246      mov	r2, lr
08029c70  99e7      b	#-206 ; -> 0x08029ba6 ; branch_target=0x08029ba6
08029c72  7246      mov	r2, lr
08029c74  c8e7      b	#-112 ; -> 0x08029c08 ; branch_target=0x08029c08
08029c76  9646      mov	lr, r2
08029c78  bbe7      b	#-138 ; -> 0x08029bf2 ; branch_target=0x08029bf2
