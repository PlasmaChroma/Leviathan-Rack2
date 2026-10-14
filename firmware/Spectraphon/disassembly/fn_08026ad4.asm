; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08026ad4  f0b5      push	{r4, r5, r6, r7, lr}
08026ad6  87b0      sub	sp, #28
08026ad8  68b1      cbz	r0, #26 ; -> 0x08026af6 ; branch_target=0x08026af6
08026ada  90f83030  ldrb.w	r3, [r0, #48]
08026ade  0446      mov	r4, r0
08026ae0  03f0ff02  and	r2, r3, #255
08026ae4  002b      cmp	r3, #0
08026ae6  44d0      beq	#136 ; -> 0x08026b72 ; branch_target=0x08026b72
08026ae8  0323      movs	r3, #3
08026aea  2046      mov	r0, r4
08026aec  84f83030  strb.w	r3, [r4, #48]
08026af0  fff72efd  bl	#-1444 ; -> 0x08026550 ; branch_target=0x08026550
08026af4  18b1      cbz	r0, #6 ; -> 0x08026afe ; branch_target=0x08026afe
08026af6  0126      movs	r6, #1
08026af8  3046      mov	r0, r6
08026afa  07b0      add	sp, #28
08026afc  f0bd      pop	{r4, r5, r6, r7, pc}
08026afe  01a9      add	r1, sp, #4
08026b00  2046      mov	r0, r4
08026b02  fff745fe  bl	#-886 ; -> 0x08026790 ; branch_target=0x08026790
08026b06  0028      cmp	r0, #0
08026b08  f5d1      bne	#-22 ; -> 0x08026af6 ; branch_target=0x08026af6
08026b0a  a16b      ldr	r1, [r4, #56]
08026b0c  9df81430  ldrb.w	r3, [sp, #20]
08026b10  0129      cmp	r1, #1
08026b12  9df81520  ldrb.w	r2, [sp, #21]
08026b16  dbb2      uxtb	r3, r3
08026b18  d2b2      uxtb	r2, r2
08026b1a  2ed0      beq	#92 ; -> 0x08026b7a ; branch_target=0x08026b7a
08026b1c  0346      mov	r3, r0
08026b1e  e168      ldr	r1, [r4, #12]
08026b20  2046      mov	r0, r4
08026b22  a365      str	r3, [r4, #88]
08026b24  fff710ff  bl	#-480 ; -> 0x08026948 ; branch_target=0x08026948
08026b28  0646      mov	r6, r0
08026b2a  0028      cmp	r0, #0
08026b2c  e3d1      bne	#-58 ; -> 0x08026af6 ; branch_target=0x08026af6
08026b2e  f9f735fc  bl	#-26518 ; -> 0x0802039c ; branch_target=0x0802039c
08026b32  0746      mov	r7, r0
08026b34  07e0      b	#14 ; -> 0x08026b46 ; branch_target=0x08026b46
08026b36  636b      ldr	r3, [r4, #52]
08026b38  0343      orrs	r3, r0
08026b3a  6363      str	r3, [r4, #52]
08026b3c  f9f72efc  bl	#-26532 ; -> 0x0802039c ; branch_target=0x0802039c
08026b40  c31b      subs	r3, r0, r7
08026b42  0133      adds	r3, #1
08026b44  20d0      beq	#64 ; -> 0x08026b88 ; branch_target=0x08026b88
08026b46  616c      ldr	r1, [r4, #68]
08026b48  2068      ldr	r0, [r4]
08026b4a  0904      lsls	r1, r1, #16
08026b4c  02f066fd  bl	#10956 ; -> 0x0802961c ; branch_target=0x0802961c
08026b50  0546      mov	r5, r0
08026b52  0028      cmp	r0, #0
08026b54  efd1      bne	#-34 ; -> 0x08026b36 ; branch_target=0x08026b36
08026b56  2068      ldr	r0, [r4]
08026b58  2946      mov	r1, r5
08026b5a  01f0b5fc  bl	#6506 ; -> 0x080284c8 ; branch_target=0x080284c8
08026b5e  c0f34320  ubfx	r0, r0, #9, #4
08026b62  0428      cmp	r0, #4
08026b64  ead1      bne	#-44 ; -> 0x08026b3c ; branch_target=0x08026b3c
08026b66  0123      movs	r3, #1
08026b68  6563      str	r5, [r4, #52]
08026b6a  e562      str	r5, [r4, #44]
08026b6c  84f83030  strb.w	r3, [r4, #48]
08026b70  c2e7      b	#-124 ; -> 0x08026af8 ; branch_target=0x08026af8
08026b72  0276      strb	r2, [r0, #24]
08026b74  0ef02afe  bl	#60500 ; -> 0x080357cc ; branch_target=0x080357cc
08026b78  b6e7      b	#-148 ; -> 0x08026ae8 ; branch_target=0x08026ae8
08026b7a  1343      orrs	r3, r2
08026b7c  14bf      ite	ne
08026b7e  4ff40073  movne.w	r3, #512
08026b82  4ff48073  moveq.w	r3, #256
08026b86  cae7      b	#-108 ; -> 0x08026b1e ; branch_target=0x08026b1e
08026b88  4ff00042  mov.w	r2, #2147483648
08026b8c  0123      movs	r3, #1
08026b8e  0326      movs	r6, #3
08026b90  6263      str	r2, [r4, #52]
08026b92  84f83030  strb.w	r3, [r4, #48]
08026b96  afe7      b	#-162 ; -> 0x08026af8 ; branch_target=0x08026af8
